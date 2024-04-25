// Copyright 2023 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <vector>
#include <type_traits>
#include <map>

#include "Common/DynamicLibrary.h"

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

constexpr std::size_t constexpr_strlen(std::string_view s) { return s.size(); }
/* http://lolengine.net/blog/2011/12/20/cpp-constant-string-hash */
#define H1(s,i,x)   (x*65599u+(uint8_t)s[(i)<constexpr_strlen(s)?constexpr_strlen(s)-1-(i):constexpr_strlen(s)])
#define H4(s,i,x)   H1(s,i,H1(s,i+1,H1(s,i+2,H1(s,i+3,x))))
#define H16(s,i,x)  H4(s,i,H4(s,i+4,H4(s,i+8,H4(s,i+12,x))))
#define H64(s,i,x)  H16(s,i,H16(s,i+16,H16(s,i+32,H16(s,i+48,x))))
#define H256(s,i,x) H64(s,i,H64(s,i+64,H64(s,i+128,H64(s,i+192,x))))
#define mhash(s) ((uint32_t)(H256(s,0,0)^(H256(s,0,0)>>16)))

#define EXTERN_MESSAGE_INITIALIZE         0
#define EXTERN_MESSAGE_ON_UPDATE          1
#define EXTERN_MESSAGE_ON_VIDEO           2
#define EXTERN_MESSAGE_ON_SAVESTATE       3
#define EXTERN_MESSAGE_ON_LOADSTATE       4
#define EXTERN_MESSAGE_SAVESTATE_POINTER  5
#define EXTERN_MESSAGE_LOADSTATE_POINTER  6
#define EXTERN_MESSAGE_SAVESTATE_FUNCTION 7
#define EXTERN_MESSAGE_MRAM_POINTER       8
#define EXTERN_MESSAGE_CONFIG_POINTER     9

namespace Common
{

static inline uint8_t* GetRAM()
{
  auto& system = Core::System::GetInstance();
  auto& memory = system.GetMemory();
  return memory.GetRAM();
}

static std::map<std::string, std::unique_ptr<AbstractTexture>, std::less<>> texture_map;

static inline auto CreateTexture(std::string name, u32 width, u32 height) -> bool {
  TextureConfig config;
  config.width = width;
  config.height = height;
  
  //std::unique_ptr<AbstractTexture> tex = g_gfx->CreateTexture(config);
  texture_map[name] = g_gfx->CreateTexture(config);
  return true;
}

static inline auto DestroyTexture(std::string name) -> bool {
  if (texture_map.find(name) != texture_map.end()) {
    texture_map[name].release();
    return true;
  }
  return false;
}

static inline const char* GetExternalToolsConfigurationPath()
{
    std::string libraries_dir = Config::Get(Config::MAIN_EXTERNAL_TOOLS_PATH);
    return strdup(libraries_dir.c_str());
}

struct ExternalTool
{
  struct Message
  {
    int type;
    void* data;
  };
  
  void RegisterPointers()
  {
    struct Message msg;
      
    msg.type = mhash("mram-function");
    msg.data = reinterpret_cast<void*>(&GetRAM);
    on_message(msg);
      
      auto& system = Core::System::GetInstance();
      auto& ppc = system.GetPPCState();
      
      struct
      {
          uint32_t *pc;
          uint32_t *npc;
          uint32_t *gpr;
          uint32_t *lr;
      } processorState;
      
      processorState.pc = &ppc.pc;
      processorState.npc = &ppc.npc;
      processorState.gpr = ppc.gpr;
      processorState.lr = &ppc.spr[SPR_LR];
      
      msg.type = mhash("ppcstate");
      msg.data = reinterpret_cast<void*>(&processorState);
      on_message(msg);
      
    msg.type = EXTERN_MESSAGE_CONFIG_POINTER;
    msg.data = reinterpret_cast<void*>(&GetExternalToolsConfigurationPath);
    on_message(msg);
  }
    
  void Launch()
  {
    if (library.IsOpen())
    {
      //on_load = reinterpret_cast<int (*)()>(reinterpret_cast<uint64_t>(library.GetSymbolAddress("on_load")));
      on_message = reinterpret_cast<void (*)(struct Message)>(reinterpret_cast<uint64_t>(library.GetSymbolAddress("on_message")));
      //on_unload = reinterpret_cast<void (*)()>(reinterpret_cast<uint64_t>(library.GetSymbolAddress("on_unload")));
      
      if (!on_message)
      {
        fprintf(stderr, "Cannot launch external tool: necessary callbacks not found or not implemented\n");
        return;
      }
        
        int ret = 0;
        struct Message msg;
          msg.type = mhash("load");
          msg.data = &ret;
          this->Message(msg);
          if (ret < 0) {
              library.Close();
          }
      
//      if (on_load() < 0)
//        library.Close();
        
      RegisterPointers();
    }
  }
  
  void Close()
  {
    if (library.IsOpen())
    {
      if (on_unload)
        on_unload();
      
      library.Close();
    }
  }
  
  void Message(struct Message msg)
  {
    if (library.IsOpen() && on_message)
      on_message(msg);
  }
  
  DynamicLibrary library;
  
private:
  int (*on_load)();
  void (*on_message)(struct Message msg);
  void (*on_unload)(void);
};

extern std::vector<ExternalTool*> external_tools;

static inline ExternalTool* LoadExternalTool(const char* filename)
{
  ExternalTool* tool = new ExternalTool;
  tool->library.Open(filename);
  
  external_tools.push_back(tool);
  return tool;
}

}  // namespace Common
