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

#include "Globals.h"
#include "Common.h"
#include "IniFile.h"
#include "Config.h"

Config g_Config;

Config::Config()
{
    memset(this, 0, sizeof(Config));
}

void Config::Load()
{
    std::string temp;
    IniFile iniFile;
    iniFile.Load(FULL_CONFIG_DIR "gfx_opengl.ini");

	// get resolution
    iniFile.Get("Hardware", "WindowedRes", &temp, 0);
	if(temp.empty())
		temp = "640x480";
    strcpy(iWindowedRes, temp.c_str());
    iniFile.Get("Hardware", "FullscreenRes", &temp, 0);
	if(temp.empty())
		temp = "640x480";
    strcpy(iFSResolution, temp.c_str());

    iniFile.Get("Hardware", "Fullscreen", &bFullscreen, 0); // Hardware
	iniFile.Get("Hardware", "RenderToMainframe", &renderToMainframe, 0);
	iniFile.Get("Settings", "StretchToFit", &bStretchToFit, false);
	iniFile.Get("Settings", "KeepAR", &bKeepAR, false);
	iniFile.Get("Settings", "HideCursor", &bHideCursor, false);

    iniFile.Get("Settings", "SafeTextureCache", &bSafeTextureCache, false); // Settings
    iniFile.Get("Settings", "ShowFPS", &bShowFPS, false); // Settings
	iniFile.Get("Settings", "OverlayStats", &bOverlayStats, false);
    iniFile.Get("Settings", "DLOptimize", &iCompileDLsLevel, 0);
    iniFile.Get("Settings", "DumpTextures", &bDumpTextures, 0);
    iniFile.Get("Settings", "ShowShaderErrors", &bShowShaderErrors, 0);
    iniFile.Get("Settings", "Multisample", &iMultisampleMode, 0);
    if (iMultisampleMode == 0)
        iMultisampleMode = 1;
	std::string s;
    iniFile.Get("Settings", "TexDumpPath", &s, 0);
    if (s.size() < sizeof(texDumpPath) )
        strcpy(texDumpPath, s.c_str());
    else {
        strncpy(texDumpPath, s.c_str(), sizeof(texDumpPath)-1);
        texDumpPath[sizeof(texDumpPath)-1] = 0;
    }

	iniFile.Get("Settings", "TexFmtOverlayEnable", &bTexFmtOverlayEnable, 0);
	iniFile.Get("Settings", "TexFmtOverlayCenter", &bTexFmtOverlayCenter, 0);
	iniFile.Get("Settings", "UseXFB", &bUseXFB, 0);
	iniFile.Get("Settings", "WireFrame", &bWireFrame, 0);
	iniFile.Get("Settings", "DisableLighting", &bDisableLighting, 0);
	iniFile.Get("Settings", "DisableTexturing", &bDisableTexturing, 0);

    iniFile.Get("Enhancements", "ForceFiltering", &bForceFiltering, 0);
    iniFile.Get("Enhancements", "MaxAnisotropy", &iMaxAnisotropy, 3);  // NOTE - this is x in (1 << x)

	iniFile.Get("Hacks", "EFBToTextureDisable", &bEFBToTextureDisable, 0);
	iniFile.Get("Hacks", "EFBToTextureDisableHotKey", &bEFBToTextureDisableHotKey, 0);	
	iniFile.Get("Hacks", "ProjectionHax1", &bProjectionHax1, 0);
	iniFile.Get("Hacks", "ProjectionHax2", &bProjectionHax2, 0);
}

void Config::Save()
{
    IniFile iniFile;
    iniFile.Load(FULL_CONFIG_DIR "gfx_opengl.ini");
    iniFile.Set("Hardware", "WindowedRes", iWindowedRes);
    iniFile.Set("Hardware", "FullscreenRes", iFSResolution);
    iniFile.Set("Hardware", "Fullscreen", bFullscreen);
	iniFile.Set("Hardware", "RenderToMainframe", renderToMainframe);
	iniFile.Set("Settings", "StretchToFit", bStretchToFit);
	iniFile.Set("Settings", "KeepAR", bKeepAR);
	iniFile.Set("Settings", "HideCursor", bHideCursor);

    iniFile.Set("Settings", "SafeTextureCache", bSafeTextureCache);
    iniFile.Set("Settings", "ShowFPS", bShowFPS);
	iniFile.Set("Settings", "OverlayStats", bOverlayStats);
    iniFile.Set("Settings", "DLOptimize", iCompileDLsLevel);
    iniFile.Set("Settings", "DumpTextures", bDumpTextures);
    iniFile.Set("Settings", "ShowShaderErrors", bShowShaderErrors);
    iniFile.Set("Settings", "Multisample", iMultisampleMode);
    iniFile.Set("Settings", "TexDumpPath", texDumpPath);
	iniFile.Set("Settings", "TexFmtOverlayEnable", bTexFmtOverlayEnable);
	iniFile.Set("Settings", "TexFmtOverlayCenter", bTexFmtOverlayCenter);
	iniFile.Set("Settings", "UseXFB", bUseXFB);
	iniFile.Set("Settings", "Wireframe", bWireFrame);
	iniFile.Set("Settings", "DisableLighting", bDisableLighting);
	iniFile.Set("Settings", "DisableTexturing", bDisableTexturing);

    iniFile.Set("Enhancements", "ForceFiltering", bForceFiltering);
    iniFile.Set("Enhancements", "MaxAnisotropy", iMaxAnisotropy);

	iniFile.Set("Hacks", "EFBToTextureDisable", bEFBToTextureDisable);
	iniFile.Set("Hacks", "EFBToTextureDisableHotKey", bEFBToTextureDisableHotKey);
	iniFile.Set("Hacks", "ProjectionHax1", bProjectionHax1);
	iniFile.Set("Hacks", "ProjectionHax2", bProjectionHax2);

    iniFile.Save(FULL_CONFIG_DIR "gfx_opengl.ini");
}
