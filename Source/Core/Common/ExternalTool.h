// Copyright 2025 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Common/DynamicLibrary.h"

namespace Common
{

struct ExternalTool
{
  ExternalTool(const std::string& filename);
  ~ExternalTool();
  bool IsOpen() const;
 
  struct Callbacks
  {
    void* (*GetMRAM)(void);
    void* (*GetCPUState)(void);
  };
  
  enum class MessageType : uint64_t
  {
    Initialize = 1,
    Shutdown,
    Reload,

    // Events
    OnLoad,
    OnUnload,
    OnFrame,
    OnUpdate,
    OnPaused,
    OnResumed,
    OnBreakpoint,
    OnStepComplete,

    FrameTexture,
    
    // Debug commands
    Debug_Break,
    Debug_Continue,
    Debug_Step,
    Debug_StepOver,
    Debug_StepOut,
    Debug_AddBreakpoint,
    Debug_RemoveBreakpoint,
    Debug_ClearBreakpoints,
    
    Callbacks,
  };
  
  struct Message
  {
    MessageType type;
    void* data;
  };
  static_assert(sizeof(Message) == 16);
  
  void SendMessage(MessageType, void *data = nullptr);
  void Reload();
  
private:
  void RegisterCallbacks();
  void (*m_onMessage)(Message);
  DynamicLibrary *m_library;
  const std::string m_filename;
};

extern std::vector<std::unique_ptr<ExternalTool>> ExternalTools;

}
