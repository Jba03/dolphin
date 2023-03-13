// Copyright 2021 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <vector>
#include <type_traits>

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
      
    msg.type = EXTERN_MESSAGE_MRAM_POINTER;
    msg.data = reinterpret_cast<void*>(&GetRAM);
    on_message(msg);
      
    msg.type = EXTERN_MESSAGE_CONFIG_POINTER;
    msg.data = reinterpret_cast<void*>(&GetExternalToolsConfigurationPath);
    on_message(msg);
  }
    
  void Launch()
  {
    if (library.IsOpen())
    {
      on_load = reinterpret_cast<int (*)()>(reinterpret_cast<uint64_t>(library.GetSymbolAddress("on_load")));
      on_message = reinterpret_cast<void (*)(struct Message)>(reinterpret_cast<uint64_t>(library.GetSymbolAddress("on_message")));
      on_unload = reinterpret_cast<void (*)()>(reinterpret_cast<uint64_t>(library.GetSymbolAddress("on_unload")));
      
      if (!on_load || !on_message)
      {
        fprintf(stderr, "Cannot launch external tool: necessary callbacks not found or not implemented\n");
        return;
      }
      
      if (on_load() < 0)
        library.Close();
        
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
