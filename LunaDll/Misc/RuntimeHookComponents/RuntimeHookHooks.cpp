#include "../RuntimeHook.h"
#include "../../LuaMain/LunaLuaMain.h"
#include "../../LuaMain/LuaEvents.h"
#include <comutil.h>
#include "../../Input/Input.h"
#include "../../GlobalFuncs.h"
#include "../../Misc/MiscFuncs.h"
#include "../../SdlMusic/MusicManager.h"
#include "../../HardcodedGraphics/HardcodedGraphicsManager.h"
#include "../ErrorReporter.h"

#include "../SHMemServer.h"

extern void InitHook()
{
    if (gStartupSettings.newLauncher){
        typedef bool(*RunProc)(void);
        typedef void(*GetPromptResultProc)(void*);
        typedef void(*FreeVarsProc)(void);
        newLauncherLib = LoadLibraryA("LunadllNewLauncher.dll");
        if (!newLauncherLib){
            std::string errMsg = "Failed to load the new Launcher D:!\nLunadllNewLauncher.dll is missing?\nError Code: ";
            errMsg += std::to_string((long long)GetLastError());
            MessageBoxA(NULL, errMsg.c_str(), "Error", 0);
            return;
        }
        RunProc hRunProc = (RunProc)GetProcAddress(newLauncherLib, "run");
        GL_PROC_CHECK(hRunProc, run, newLauncherLib, LunadllNewLauncher.dll)
            GetPromptResultProc hPrompt = (GetPromptResultProc)GetProcAddress(newLauncherLib, "GetPromptResult");
        GL_PROC_CHECK(hPrompt, GetPromptResult, newLauncherLib, LunadllNewLauncher.dll)
            FreeVarsProc hFreeVarsProc = (FreeVarsProc)GetProcAddress(newLauncherLib, "FreeVars");
        GL_PROC_CHECK(hFreeVarsProc, FreeVars, newLauncherLib, LunadllNewLauncher.dll)
            hRunProc();
        resultStruct settings;
        hPrompt((void*)&settings);

        if (!settings.result){
            GM_ISLEVELEDITORMODE = 0; //set run to false
            _exit(0);
        }
        GM_ISGAME = -1;
        if (settings.result == 2){
            GM_ISLEVELEDITORMODE = -1;
        }
        GM_NOSOUND = COMBOOL(settings.NoSound);
        GM_FRAMESKIP = COMBOOL(settings.disableFrameskip);
    }
    else{
        GM_ISLEVELEDITORMODE = COMBOOL(gStartupSettings.lvlEditor);
        GM_ISGAME = COMBOOL(gStartupSettings.game);
        GM_FRAMESKIP = COMBOOL(gStartupSettings.frameskip);
        GM_NOSOUND = COMBOOL(gStartupSettings.noSound);
    }

    if (gStartupSettings.debugger){
        newDebugger = LoadLibraryA("LunadllNewLauncher.dll");
        if (!newDebugger){
            std::string errMsg = "Failed to load the new Launcher D:!\nLunadllNewLauncher.dll is missing?\nError Code: ";
            errMsg += std::to_string((long long)GetLastError());
            MessageBoxA(NULL, errMsg.c_str(), "Error", 0);
            newDebugger = NULL;
            return;
        }
        runAsyncDebuggerProc = (void(*)(void))GetProcAddress(newDebugger, "runAsyncDebugger");
        asyncBitBltProc = (int(*)(HDC, int, int, int, int, HDC, int, int, unsigned int))GetProcAddress(newDebugger, "asyncBitBlt@36");
        GL_PROC_CHECK(runAsyncDebuggerProc, runAsyncDebugger, newDebugger, LunadllNewLauncher.dll)
            GL_PROC_CHECK(asyncBitBltProc, asyncBitBlt, newDebugger, LunadllNewLauncher.dll)
            //PATCH_JMP(0x4242D0, &bitBltHook);

            *(void**)0xB2F1D8 = (void*)asyncBitBltProc;
        runAsyncDebuggerProc();
    }
    if (gStartupSettings.logger){
        if (!newDebugger)
            newDebugger = LoadLibraryA("LunadllNewLauncher.dll");

        runAsyncLoggerProc = (void(*)(void))GetProcAddress(newDebugger, "runAsyncLogger");
        asyncLogProc = (void(*)(const char*))GetProcAddress(newDebugger, "asyncLog");
        GL_PROC_CHECK(runAsyncLoggerProc, runAsyncLogger, newDebugger, LunadllNewLauncher.dll)
            GL_PROC_CHECK(asyncLogProc, asyncLog, newDebugger, LunadllNewLauncher.dll)
            runAsyncLoggerProc();
    }



    /*void (*exitCall)(void);
    exitCall = (void(*)(void))0x8D6BB0;
    exitCall();*/
}

extern void forceTermination()
{
    _exit(0);
}

extern int LoadWorld()
{
#ifndef NO_SDL
    if (!episodeStarted)
    {
        std::string wldPath = wstr2str(std::wstring((wchar_t*)GM_FULLDIR));
        MusicManager::loadCustomSounds(wldPath + "\\");
        episodeStarted = true;
    }
#endif

    gSkipSMBXHUD = false;
    gIsOverworld = true;
    gLunaRender.ClearAll();
    gSpriteMan.ResetSpriteManager();
    gCellMan.Reset();
    gSavedVarBank.ClearBank();
    Input::ResetAll();

    gLunaRender.ReloadScreenHDC();

    // Init var bank
    gSavedVarBank.TryLoadWorldVars();
    gSavedVarBank.CheckSaveDeletion();
    gSavedVarBank.CopyBank(&gAutoMan.m_UserVars);

    gLunaLua = CLunaLua();
    gLunaLua.init(CLunaLua::LUNALUA_WORLD, std::wstring((wchar_t*)GM_FULLDIR));

    // Recount deaths
    gDeathCounter.Recount();

    short plValue = GM_PLAYERS_COUNT;
#ifndef __MINGW32__
    __asm {
        MOV EAX, 1
            MOV CX, plValue
    }
#else
    asm(".intel_syntax noprefix\n"
        "mov eax, 1\n"
        "mov cx, %0\n"
        ".att_syntax\n": "=r" (plValue));
    //".intel_syntax prefix" :: [plValue] "g" (plValue) : "edx");

#endif
}

extern DWORD WorldLoop()
{
    gSavedVarBank.CheckSaveDeletion();

    // Update inputs
    Input::CheckSpecialCheats();
    Input::UpdateInputTasks();

    gLunaLua.doEvents();

    gSavedVarBank.SaveIfNeeded();

    return GetTickCount();
}



extern int __stdcall printLunaLuaVersion(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, unsigned int dwRop)
{
#ifndef NO_SDL
    if (episodeStarted)
    {   //Reset sounds to default when main menu is loaded
        MusicManager::resetSoundsToDefault();
        episodeStarted = false;
    }
#endif
    Render::Print(utf8_decode(LUALUA_VERSION), 3, 5, 5);
    if (newDebugger)
    {
        if (asyncBitBltProc){
            return asyncBitBltProc(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
        }
    }
    return BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
}

extern void* WorldRender()
{
    if (gShowDemoCounter)
        gDeathCounter.Draw();

    gSpriteMan.RunSprites();
    gLunaRender.RenderAll();

    return (void*)0xB25010;
}

extern MCIERROR __stdcall mciSendStringHookA(__in LPCSTR lpstrCommand, __out_ecount_opt(uReturnLength) LPSTR lpstrReturnString, __in UINT uReturnLength, __in_opt HWND hwndCallback)
{
    bool doLogInput = true;
    bool doLogOutput = true;

    if (lpstrCommand == 0){
        doLogInput = false;
    }
    else{
        if (lpstrCommand[0] == 0){
            doLogInput = false;
        }
    }

    std::string inputStr = "";
    std::string outputStr = "";

    if (doLogInput){
        inputStr += "Input: ";
        inputStr += lpstrCommand;

        if (newDebugger){
            if (asyncLogProc){
                asyncLogProc(inputStr.c_str());
            }
        }
    }

    //Swap to restore old code or to use emulator
    //MCIERROR ret = mciSendStringA(lpstrCommand, lpstrReturnString, uReturnLength, hwndCallback);
    MCIERROR ret = gMciEmulator.mciEmulate(lpstrCommand, lpstrReturnString, uReturnLength, hwndCallback);

    if (lpstrReturnString == 0){
        doLogOutput = false;
    }
    else{
        if (lpstrReturnString[0] == 0){
            doLogOutput = false;
        }
    }
    if (doLogOutput){
        outputStr += "Output: ";
        outputStr += lpstrReturnString;

        if (newDebugger){
            if (asyncLogProc){
                asyncLogProc(outputStr.c_str());
            }
        }
    }
    return ret;
}

extern float __stdcall vbaR4VarHook(VARIANTARG* variant)
{
    if (asyncLogProc)
    {
        stringstream q;
        q << variant->vt << " ";
        if (variant->vt == VT_R8)
        {
            q << "src:" << variant->dblVal << " dst:" << static_cast<float>(variant->dblVal);
        }
        string rr("vbaR4VarHook type:" + q.str() + ";");
        asyncLogProc(rr.c_str());
    }

    switch (variant->vt)
    {
    case VT_BSTR:
    {
        wchar_t* str = variant->bstrVal;
        return (float)wcstod(str, NULL);
    }
        break;
    case (VT_BSTR | VT_BYREF) :
    {
        if (variant->pbstrVal == NULL) return 0.0;
        wchar_t* str = *(variant->pbstrVal);
        return (float)wcstod(str, NULL);
    }
                              break;
    case VT_I2:
    {
        short str = variant->iVal;
        return (float)str;
    }
        break;
    case (VT_I2 | VT_BYREF) :
    {
        if (variant->piVal == NULL) return 0.0;
        return (float)((int)(*(variant->piVal)));
    }
                            break;
    case VT_I4:
    {
        long str = variant->lVal;
        return (float)str;
    }
        break;
    case (VT_I4 | VT_BYREF) :
    {
        if (variant->plVal == NULL) return 0.0;
        return (float)(*variant->plVal);
    }
                            break;
    case VT_BOOL:
    {
        bool str = variant->boolVal;
        return (float)(int)str;
    }
        break;
    case (VT_BOOL | VT_BYREF) :
    {
        if (variant->pboolVal == NULL) return 0.0;
        bool str = *(variant->pboolVal);
        return (float)(int)str;
    }
                              break;
    case VT_R4:
    {
        return variant->fltVal;
    }
        break;
    case (VT_R4 | VT_BYREF) :
    {
        if (variant->pfltVal == NULL) return 0.0;
        return (*variant->pfltVal);
    }
                            break;
    case VT_R8:
    {
        return static_cast<float>(variant->dblVal);
    }
        break;
    case (VT_R8 | VT_BYREF) :
    {
        if (variant->pdblVal == NULL) return 0.0;
        return (float)(*variant->pdblVal);
    }
                            break;
    case VT_CY:
    {
        CY x = variant->cyVal;
        float y = (float)x.Hi;
        float z = (float)x.Lo;
        while (z>1.0)
        {
            z /= 10;
        }
        y += z;
        return y;
    }
        break;
    case (VT_CY | VT_BYREF) :
    {
        if (variant->pcyVal == 0) return 0.0;
        CY x = *(variant->pcyVal);
        float y = (float)x.Hi;
        float z = (float)x.Lo;
        while (z>1.0)
        {
            z /= 10;
        }
        y += z;
        return y;
    }
                            break;
    case  VT_UI1:
    {
        return (float)((int)((char)variant->bVal));
    }
        break;
    case (VT_UI1 | VT_BYREF) :
    {
        return (float)((int)((char)(*(variant->pbVal))));
    }
    default:
        break;
    }
    return 0.0;//__vbaR4Var(variant);
}

extern int __stdcall rtcMsgBoxHook(VARIANTARG* msgText, DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4)
{
    std::wstring msg((wchar_t*)msgText->bstrVal);
    if (gHook_SkipTestMsgBox){
        if (msg == std::wstring((wchar_t*)0x42BE28))
            return 7;
    }
    gHook_SkipTestMsgBox = false;
    return rtcMsgBox(msgText, arg1, arg2, arg3, arg4);
}



extern void __stdcall doEventsLevelEditorHook()
{
    /*void* lvlEditForm = *(void**)(0x00B2D7E8);
    if(lvlEditForm){
    char* lvlEditFormVtbl = *(char**)lvlEditForm;
    ((HRESULT (__stdcall*)(void *, void *))*((char**)lvlEditFormVtbl + 58))(lvlEditForm, )

    }*/

    HMODULE vmVB6Lib = GetModuleHandleA("msvbvm60.dll");
    GetProcAddress(vmVB6Lib, "rtcDoEvents")();
}


extern int __stdcall __vbaStrCmp_TriggerSMBXEventHook(BSTR nullStr, BSTR eventName)
{

    int(__stdcall *origCmp)(BSTR, BSTR) = (int(__stdcall *)(BSTR, BSTR))IMP_vbaStrCmp;
    Event TriggerEventData("onEvent", true);
    gLunaLua.callEvent(&TriggerEventData, utf8_encode(eventName));
    if (TriggerEventData.native_cancelled())
        return 0;
    return origCmp(nullStr, eventName);

}

extern void checkLevelShutdown()
{
    if (GM_WORLD_MODE || GM_INTRO_MODE){
        if (gLunaLua.isValid()){
            Event shutdownEvent("onExitLevel", false);
            shutdownEvent.setDirectEventName("onExitLevel");
            shutdownEvent.setLoopable(false);
            gLunaLua.callEvent(&shutdownEvent);
            gLunaLua.shutdown();

            //Clean & stop all user started sounds and musics
            PGE_MusPlayer::MUS_stopMusic();
            PGE_Sounds::clearSoundBuffer();
        }
    }

    __asm{
        CMP WORD PTR DS : [0x00B2C5B4], 0
    }
}


extern void handleError(int errCode)
{

    //HERE NEED ESI CMP CODE (ORIGINAL CODE)
}