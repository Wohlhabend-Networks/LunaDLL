#include "../../LuaProxy.h"
#include "../../../GlobalFuncs.h"
#include "../../../Globals.h"
#include "../../../Rendering/Rendering.h"
#include "../../../Misc/RuntimeHook.h"
#include "../../../Rendering/RenderOps/RenderStringOp.h"

void LuaProxy::Text::windowDebug(const char* debugText, lua_State* L)
{
    int resultCode = MessageBoxA(0, debugText, "Debug", MB_OKCANCEL);
    if (resultCode == IDCANCEL)
        luaL_error(L, "Pressed cancel on windowDebug!");
}

void LuaProxy::Text::windowDebugSimple(const char* debugText)
{
    MessageBoxA(0, debugText, "Debug", MB_OK);
}


void LuaProxy::Text::print(const luabind::object& toPrint, int x, int y)
{
    LuaProxy::Text::print(toPrint, 3, x, y);
}


void LuaProxy::Text::print(const luabind::object& toPrint, int type, int x, int y)
{
    lua_State* L = toPrint.interpreter();
    // FIXME: Use better method than calling the tostring function directly! (defensive programming?)
    std::wstring txt = utf8_decode(luabind::call_function<std::string>(L, "tostring", toPrint));
    
    if (type == 3)
        for (wstring::iterator it = txt.begin(); it != txt.end(); ++it)
            *it = towupper(*it);

    gLunaRender.AddOp(new RenderStringOp(txt, type, (float)x, (float)y));
}


void LuaProxy::Text::showMessageBox(const std::string &text)
{
    showSMBXMessageBox(text);
}