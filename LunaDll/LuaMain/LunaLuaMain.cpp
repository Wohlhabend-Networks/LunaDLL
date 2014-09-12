#include "LunaLuaMain.h"
#include "../Level.h"
#include "../MiscFuncs.h"
#include "../PlayerMOB.h"
#include "LuaHelper.h"
#include <string>


std::string utf8_encode(const std::wstring &wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo( size_needed, 0 );
	WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

void dbg(std::wstring txt){
    MessageBoxW(0, txt.c_str(), L"Debug", 0);
}

void windowDebug(const char* debugText){
    MessageBoxA(0, debugText, "Debug", 0);
}

void windowError(const char* errorText){
    MessageBoxA(0, errorText, "Error", 0);
}

void LunaLua::init(wstring main_path)
{
    TryClose();

    mainState = luaL_newstate();
	luabind::open(mainState);
    lua_pushcfunction(mainState, luaopen_base);
    lua_call(mainState,0,0);
    lua_pushcfunction(mainState, luaopen_math);
    lua_call(mainState,0,0);
    lua_pushcfunction(mainState, luaopen_string);
    lua_call(mainState,0,0);
    lua_pushcfunction(mainState, luaopen_table);
    lua_call(mainState,0,0);
    lua_pushcfunction(mainState, luaopen_debug);
    lua_call(mainState,0,0);


	wstring full_path = main_path.append(Level::GetName());	
	full_path = removeExtension(full_path);
	full_path = full_path.append(L"\\lunadll.lua");

	wifstream code_file(full_path, ios::binary|ios::in);
    if(!code_file.is_open()){
        code_file.close();
        return;
    }
    using namespace luabind;

	std::wstring wluacode((std::istreambuf_iterator<wchar_t>(code_file)), std::istreambuf_iterator<wchar_t>());
    code_file.close();
    std::string luacode = utf8_encode(wluacode);
    int errcode = luaL_loadbuffer(mainState, luacode.c_str(), luacode.length(), "lunadll.lua")  || lua_pcall(mainState, 0, LUA_MULTRET, 0);
    
	module(mainState)
	[
        def("windowDebug", &windowDebug)
	];

    if(!(errcode == 0)){
        object error_msg(from_stack(mainState, -1));
        windowDebug(object_cast<const char*>(error_msg));
        lua_pop(GetLua(), 1);
        TryClose();
        return;
    }
	try
	{
        if(LuaHelper::is_function(mainState, "onLoad")){
            luabind::call_function<void>(mainState, "onLoad");
        }
	}
	catch (error& e)
	{
        object error_msg(from_stack(mainState, -1));
        windowDebug(object_cast<const char*>(error_msg));
        lua_pop(GetLua(), 1);
        TryClose();
	}
}



void LunaLua::TryClose()
{
    if(mainState)
        lua_close(mainState);
}


void LunaLua::Do()
{
    PlayerMOB* demo = Player::Get(1);
    if(demo == 0)
        return;

    try
    {
        if(LuaHelper::is_function(mainState, "onLoop")){
            luabind::call_function<void>(mainState, "onLoop");
        }

        std::string curSecLoop = "onLoopSection";
        curSecLoop = curSecLoop.append(std::to_string((long long)demo->CurrentSection));
        if(LuaHelper::is_function(mainState, curSecLoop.c_str())){
            luabind::call_function<void>(mainState, curSecLoop.c_str());
        }
    }
    catch (luabind::error& e)
	{
        luabind::object error_msg(luabind::from_stack(e.state(), -1));
        windowDebug(luabind::object_cast<const char*>(error_msg));
    }
}
