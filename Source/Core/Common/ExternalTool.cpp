// Copyright 2025 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <cstddef>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_set>

#include <vector>
#include <type_traits>
#include <map>

#include "Common/ExternalTool.h"

#include "Core/State.h"

#include "Common/CommonPaths.h"
#include "Common/FileUtil.h"
#include "Common/IOFile.h"
#include "Common/Logging/Log.h"
#include "Common/NandPaths.h"
#include "Common/SettingsHandler.h"
#include "Common/Timer.h"
#include "Common/Version.h"
#include "Core/Config/MainSettings.h"
#include "Core/Config/UISettings.h"
#include "Core/Core.h"
#include "Core/HW/Memmap.h"
#include "Core/Host.h"
#include "Core/System.h"
#include "Core/HLE/HLE.h"
#include "Core/PowerPC/PowerPC.h"

#include "VideoCommon/AbstractGfx.h"
#include "VideoCommon/AbstractPipeline.h"
#include "VideoCommon/AbstractShader.h"
#include "VideoCommon/FramebufferShaderGen.h"
#include "VideoCommon/NetPlayChatUI.h"
#include "VideoCommon/NetPlayGolfUI.h"
#include "VideoCommon/OnScreenDisplay.h"
#include "VideoCommon/PerformanceMetrics.h"
#include "VideoCommon/Present.h"
#include "VideoCommon/Statistics.h"
#include "VideoCommon/VertexManagerBase.h"
#include "VideoCommon/VideoConfig.h"

static void* GetMRAM()
{
  auto& system = Core::System::GetInstance();
  auto& memory = system.GetMemory();
  return memory.GetRAM();
}

static void* GetPPCState()
{
  auto& system = Core::System::GetInstance();
  auto& ppcstate = system.GetPPCState();
  return reinterpret_cast<void*>(&ppcstate);
}

static bool DoSaveState(int slot)
{
  auto& system = Core::System::GetInstance();
  State::Save(system, slot);
}

static bool DoLoadState(int slot)
{
  auto& system = Core::System::GetInstance();
  State::Load(system, slot);
}

static bool DoSaveStateToBuffer(Common::UniqueBuffer<u8>& buffer)
{
  auto& system = Core::System::GetInstance();
//  State::Save(system, buffer);
}

static bool DoLoadStateFromBuffer(Common::UniqueBuffer<u8>& buffer)
{
  auto& system = Core::System::GetInstance();
//  State::LoadFromBuffer(system, buffer);
}

static bool Reload(void)
{
  auto& tools = Common::ExternalTools;
  for (auto& tool : tools)
  {
    tool->Reload();
  }
}


namespace Common
{

std::vector<std::unique_ptr<ExternalTool>> ExternalTools;

static inline const char* GetExternalToolsConfigurationPath()
{
  const std::string& libraries_dir = Config::Get(Config::MAIN_EXTERNAL_TOOLS_PATH);
  return libraries_dir.c_str();
}

ExternalTool::ExternalTool(const std::string& filename) : m_filename(filename)
{
  m_library = new DynamicLibrary();
  m_library->Open(filename.c_str());
  if (m_library->IsOpen())
  {
    m_onMessage = reinterpret_cast<void (*)(Message)>(reinterpret_cast<uint64_t>(m_library->GetSymbolAddress("OnMessage")));
    if (!m_onMessage)
    {
      fprintf(stderr, "Cannot launch external tool: necessary callbacks not found or not implemented\n");
      return;
    }
    
    SendMessage(MessageType::Initialize);
    RegisterCallbacks();
  }
}

void ExternalTool::RegisterCallbacks()
{
  Callbacks callbacks {};
  callbacks.GetMRAM = GetMRAM;
  
  SendMessage(MessageType::Callbacks, &callbacks);
}

void ExternalTool::SendMessage(MessageType type, void *data)
{
  Message msg{};
  msg.type = type;
  msg.data = data;
  
  if (m_library->IsOpen() && m_onMessage)
    m_onMessage(msg);
}

bool ExternalTool::IsOpen() const
{
  return m_library->IsOpen();
}

void ExternalTool::Reload()
{
  SendMessage(MessageType::OnUnload);
  if (m_library->IsOpen())
  {
    m_library->Close();
  }
  
  m_library->Open(m_filename.c_str());
}

ExternalTool::~ExternalTool()
{
  if (m_library->IsOpen())
  {
    SendMessage(MessageType::OnUnload);
    m_library->Close();
  }
  
  delete m_library;
}

}  // namespace Common
