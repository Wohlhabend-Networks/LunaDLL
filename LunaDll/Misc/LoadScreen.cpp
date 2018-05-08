#include <thread>
#include <atomic>
#include <string>
#include "../Globals.h"
#include "../Defines.h"
#include "../GlobalFuncs.h"
#include "AsmPatch.h"
#include "../Rendering/GL/GLEngine.h"
#include "../Rendering/Rendering.h"
#include <lua.hpp>

static std::thread* loadThread = nullptr;
static std::atomic<bool> killThreadFlag = false;

static void luasetconst(lua_State* L, const char* name, lua_Number number)
{
	lua_pushnumber(L, number);
	lua_setglobal(L, name);
}

#define DEF_CONST(L, C) luasetconst(L, #C, C)

static bool checkElapsedTime(lua_State* L, DWORD loadScreenStartTick)
{
	DWORD msElapsed = GetTickCount() - loadScreenStartTick;

	lua_getglobal(L, "_loadScreenTimeout");
	if (lua_isnumber(L, -1))
	{
		lua_Number timeout = 1000.0 * lua_tonumber(L, -1);
		lua_pop(L, 1);
		return msElapsed > timeout;
	}
	lua_pop(L, 1);

	return true;
}

static void LoadThread(void)
{
	DWORD loadScreenStartTick = GetTickCount();

	static std::string mainCode;
	if (mainCode.length() == 0)
	{
		std::wstring lapi = gAppPathWCHAR;
		lapi = lapi.append(L"\\scripts\\base\\engine\\loadscreen.lua");

		FILE* theFile = _wfopen(lapi.c_str(), L"rb");
		if (!theFile)
		{
			return;
		}
		fseek(theFile, 0, SEEK_END);
		size_t len = ftell(theFile);
		rewind(theFile);
		if (len > 0)
		{
			mainCode.resize(len);
			fread(&mainCode[0], 1, len, theFile);
		}
		fclose(theFile);
	}

	static lua_State* L = nullptr;
	if (L == nullptr)
	{
		L = luaL_newstate();
		lua_pushcfunction(L, luaopen_base);
		lua_call(L, 0, 0);
		lua_pushcfunction(L, luaopen_math);
		lua_call(L, 0, 0);
		lua_pushcfunction(L, luaopen_string);
		lua_call(L, 0, 0);
		lua_pushcfunction(L, luaopen_table);
		lua_call(L, 0, 0);
		lua_pushcfunction(L, luaopen_bit);
		lua_call(L, 0, 0);
		lua_pushcfunction(L, luaopen_package);
		lua_call(L, 0, 0);
		lua_pushcfunction(L, luaopen_io);
		lua_call(L, 0, 0);
		lua_pushcfunction(L, luaopen_ffi);
		lua_call(L, 0, 0);
		lua_pushcfunction(L, luaopen_jit);
		lua_call(L, 0, 0);

		lua_pushstring(L, WStr2Str(gAppPathWCHAR).c_str());
		lua_setglobal(L, "_smbxPath");

		lua_pushnumber(L, 0.0);
		lua_setglobal(L, "_loadScreenTimeout");

		luasetconst(L, "FIELD_BYTE", 1);
		luasetconst(L, "FIELD_WORD", 2);
		luasetconst(L, "FIELD_DWORD", 3);
		luasetconst(L, "FIELD_FLOAT", 4);
		luasetconst(L, "FIELD_DFLOAT", 5);
		luasetconst(L, "FIELD_STRING", 6);
		luasetconst(L, "FIELD_BOOL", 7);

		DEF_CONST(L, GL_FLOAT);
		DEF_CONST(L, GL_FLOAT_VEC2);
		DEF_CONST(L, GL_FLOAT_VEC3);
		DEF_CONST(L, GL_FLOAT_VEC4);
		DEF_CONST(L, GL_FLOAT_MAT2);
		DEF_CONST(L, GL_FLOAT_MAT3);
		DEF_CONST(L, GL_FLOAT_MAT4);
		DEF_CONST(L, GL_FLOAT_MAT2x3);
		DEF_CONST(L, GL_FLOAT_MAT2x4);
		DEF_CONST(L, GL_FLOAT_MAT3x2);
		DEF_CONST(L, GL_FLOAT_MAT3x4);
		DEF_CONST(L, GL_FLOAT_MAT4x2);
		DEF_CONST(L, GL_FLOAT_MAT4x3);
		DEF_CONST(L, GL_INT);
		DEF_CONST(L, GL_INT_VEC2);
		DEF_CONST(L, GL_INT_VEC3);
		DEF_CONST(L, GL_INT_VEC4);
		DEF_CONST(L, GL_UNSIGNED_INT);
		DEF_CONST(L, GL_UNSIGNED_INT_VEC2);
		DEF_CONST(L, GL_UNSIGNED_INT_VEC3);
		DEF_CONST(L, GL_UNSIGNED_INT_VEC4);
		DEF_CONST(L, GL_DOUBLE);
		DEF_CONST(L, GL_DOUBLE_VEC2);
		DEF_CONST(L, GL_DOUBLE_VEC3);
		DEF_CONST(L, GL_DOUBLE_VEC4);
		DEF_CONST(L, GL_DOUBLE_MAT2);
		DEF_CONST(L, GL_DOUBLE_MAT3);
		DEF_CONST(L, GL_DOUBLE_MAT4);
		DEF_CONST(L, GL_DOUBLE_MAT2x3);
		DEF_CONST(L, GL_DOUBLE_MAT2x4);
		DEF_CONST(L, GL_DOUBLE_MAT3x2);
		DEF_CONST(L, GL_DOUBLE_MAT3x4);
		DEF_CONST(L, GL_DOUBLE_MAT4x2);
		DEF_CONST(L, GL_DOUBLE_MAT4x3);
		DEF_CONST(L, GL_SAMPLER_2D);

		if (luaL_loadbuffer(L, mainCode.c_str(), mainCode.length(), "=loadscreen.lua"))
		{
			MessageBoxA(NULL, lua_tostring(L, -1), "LunaLua LoadScreen Syntax Error", MB_OK | MB_ICONWARNING);
			return;
		}
		if (lua_pcall(L, 0, 0, 0))
		{
			MessageBoxA(NULL, lua_tostring(L, -1), "LunaLua LoadScreen Critical Error", MB_OK | MB_ICONWARNING);
			return;
		}
	}
	if (L == nullptr) return;

	lua_getglobal(L, "init");
	if (lua_pcall(L, 0, 0, 0))
	{
		MessageBoxA(NULL, lua_tostring(L, -1), "LunaLua LoadScreen Error", MB_OK | MB_ICONWARNING);
	}

	
	do {
		lua_getglobal(L, "onDraw");
		if (lua_pcall(L, 0, 0, 0))
		{
			MessageBoxA(NULL, lua_tostring(L, -1), "LunaLua LoadScreen Error", MB_OK | MB_ICONWARNING);
		}

		gLunaRender.StartFrameRender();

		gLunaRender.RenderBelowPriority(DBL_MAX);

		g_GLEngine.RenderCameraToScreen(NULL, 0, 0, 800, 600, NULL, 0, 0, 800, 600, 0);

		g_GLEngine.EndFrame(NULL);

		gLunaRender.EndFrameRender();

		if (!killThreadFlag || !checkElapsedTime(L, loadScreenStartTick))
		{
			Sleep(15);
		}
	} while (!killThreadFlag || !checkElapsedTime(L, loadScreenStartTick));
	killThreadFlag = false;
}

static void __stdcall CustomLoadScreenHook(void)
{
	native_cleanupLevel();

	if (loadThread != nullptr) return;

	GLEngineProxy::CheckRendererInit();

	killThreadFlag = false;
	loadThread = new std::thread(LoadThread);
}

void LunaLoadScreenKill()
{
	if (loadThread == nullptr) return;
	killThreadFlag = true;
	while (killThreadFlag)
	{
		native_rtcDoEvents();
		Sleep(15);
	}
	loadThread->join();
	delete loadThread;
	loadThread = nullptr;
}

void LunaLoadScreenSetEnable(bool skip)
{
	static auto nosound1a = PATCH(0x8C13E5).JMP(0x8C13E5 + 7).NOP_PAD_TO_SIZE<7>();
	static auto nosound1b = PATCH(0x8C13FF).JMP(0x8C13FF + 15).NOP_PAD_TO_SIZE<15>();
	static auto skipRenderAndWait1 = PATCH(0x8C1421).CALL(CustomLoadScreenHook).JMP(0x8C1435).NOP_PAD_TO_SIZE<10>();

	static auto nosound2 = PATCH(0x8E1A06).JMP(0x8E1A06 + 16).NOP_PAD_TO_SIZE<16>();
	static auto skipRenderAndWait2 = PATCH(0x8E1A36).CALL(CustomLoadScreenHook).JMP(0x8E1A4A).NOP_PAD_TO_SIZE<10>();

	static auto nosound3 = PATCH(0x8CE2BF).JMP(0x8CE2BF + 21).NOP_PAD_TO_SIZE<21>();
	static auto skipRenderAndWait3 = PATCH(0x8CE333).CALL(CustomLoadScreenHook).JMP(0x8CE355).NOP_PAD_TO_SIZE<24>();

	static Patchable* const patches[] = {
		&nosound1a,
		&nosound1b,
		&skipRenderAndWait1,
		&nosound2,
		&skipRenderAndWait2,
		&nosound3,
		&skipRenderAndWait3,
		nullptr
	};

	if (skip)
	{
		for (unsigned int idx = 0; patches[idx] != nullptr; idx++)
		{
			patches[idx]->Apply();
		}
	}
	else
	{
		for (unsigned int idx = 0; patches[idx] != nullptr; idx++)
		{
			patches[idx]->Unapply();
		}
	}
}