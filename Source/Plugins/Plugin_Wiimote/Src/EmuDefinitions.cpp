// Copyright (C) 2003-2008 Dolphin Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official SVN repository and contact information can be found at
// http://code.google.com/p/dolphin-emu/

#ifndef _EMU_DECLARATIONS_
#define _EMU_DECLARATIONS_

#include "pluginspecs_wiimote.h"

#include <vector>
#include <string>
#include "Common.h"
#include "wiimote_hid.h"
#include "EmuDefinitions.h"
#include "Encryption.h"
#include "Console.h" // for startConsoleWin, wprintf, GetConsoleHwnd

extern SWiimoteInitialize g_WiimoteInitialize;

namespace WiiMoteEmu
{
	
//******************************************************************************
// Definitions and variable declarations
//******************************************************************************

u8 g_Leds = 0x1;
u8 g_IR = 0x1; // 1 = on

u8 g_Eeprom[WIIMOTE_EEPROM_SIZE];

u8 g_RegSpeaker[WIIMOTE_REG_SPEAKER_SIZE];
u8 g_RegExt[WIIMOTE_REG_EXT_SIZE];
u8 g_RegExtTmp[WIIMOTE_REG_EXT_SIZE];
u8 g_RegIr[WIIMOTE_REG_IR_SIZE];

u8 g_ReportingMode; // the reporting mode and channel id
u16 g_ReportingChannel;

wiimote_key g_ExtKey; // the extension encryption key

} // namespace

#endif	//_EMU_DECLARATIONS_