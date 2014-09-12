#include "Globals.h"
#include "LevelCodes.h"
#include "DeathCounter.h"
#include <windows.h>
#include "Shlwapi.h"
#include <time.h>
#include "Logging.h"
#include "LuaMain/LunaLuaMain.h"

// Global settings
bool gLunaEnabled;
bool gShowDemoCounter;
bool gSkipSMBXHUD;			// Whether or not the SMBX HUD will be drawn

// Global variables
int	gFrames;

int* gpFreeGlob;

int gLastDownPress;
int gDownTapped;
int gLastUpPress;
int gUpTapped;
int gLastLeftPress;
int gLeftTapped;
int gLastRightPress;
int gRightTapped;

int gLastJumpPress;
int gJumpTapped;
int gLastRunPress;
int gRunTapped;

int	gCurrentMainPlayer;

short gNumpad4;
short gNumpad8;
short gNumpad2;
short gNumpad6;

LvlEnum gLevelEnum;

// Global objs
Renderer gLunaRender;

DeathCounter gDeathCounter;

AutocodeManager gAutoMan;

Logger gLogger;

CSpriteManager gSpriteMan;

CellManager gCellMan;

/// INIT GLOBALS
void InitGlobals() {
	char* dbg = "GLOBAL INIT DBG";
	gLunaEnabled = true;
	gShowDemoCounter = false;
	gSkipSMBXHUD = false;
	gLogger.m_Enabled = false;
	gCellMan.Reset();

	gFrames = 0;

	gLastDownPress = 0;
	gDownTapped = 0;
	gLastUpPress = 0;
	gUpTapped = 0;
	gLastLeftPress = 0;
	gLeftTapped = 0;
	gLastRightPress = 0;
	gRightTapped = 0;

	gLastJumpPress = 0;
	gJumpTapped = 0;
	gLastRunPress = 0;
	gRunTapped = 0;

	gCurrentMainPlayer = 1;

	gNumpad4 = 0;
	gNumpad8 = 0;
	gNumpad2 = 0;
	gNumpad6 = 0;

	gLevelEnum = Invalid;

	srand((int)time(NULL));

	// Allocate 40k bytes of free mem
	gpFreeGlob = NULL;
	gpFreeGlob = new int[10000];
	ResetFreeGlob();

	// Try to activate death counter, and disable it if there's some problem
	gDeathCounter.TryLoadStats();
	if(gDeathCounter.mStatFileOK == false)
		gDeathCounter.mEnabled = false;
	
	/// Init autocode manager	
	HMODULE hModule = GetModuleHandleW(NULL);
	WCHAR path[MAX_PATH];
	int count = GetModuleFileNameW(hModule, path, MAX_PATH);
	//PathRemoveFileSpec(path);	
	for(int i = count; i > 3; i--) {
		if(path[i] == L'\\') {
			path[i] = 0;
			break;
		}
	}
	gAutoMan.Clear(true);
	gAutoMan.ReadGlobals(std::wstring(path));
	gAutoMan.m_GlobalEnabled = true;
}

/// RESET FREE GLOB
void ResetFreeGlob() {
	if(gpFreeGlob)
		ZeroMemory(gpFreeGlob, 10000 * sizeof(int));
}

/// CLEAN UP
void CleanUp() {
    LunaLua::TryClose();
	if(gpFreeGlob)
		delete[] gpFreeGlob;
}
