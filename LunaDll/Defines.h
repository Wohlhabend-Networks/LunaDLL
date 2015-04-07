//*** Defines.h - It has defines in it, also basically the known RAM map. ***
#ifndef Defines_hhhh
#define Defines_hhhh

#include <windows.h>
#include "Misc/VB6StrPtr.h"

#define LUNA_VERSION        8
#define LUALUA_VERSION      "LUNALUA V0.7 BETA"
#define COMPILE_PLAYGROUND  0 //See Misc/Playground.cpp
#define COMBOOL(b) (b ? -1 : 0)

enum FIELDTYPE {
    FT_INVALID = 0,
    FT_BYTE = 1,
    FT_WORD = 2,
    FT_DWORD = 3,
    FT_FLOAT = 4,
    FT_DFLOAT = 5
};

enum OPTYPE {
    OP_Assign = 0,
    OP_Add = 1,
    OP_Sub = 2,
    OP_Mult = 3,
    OP_Div = 4,
    OP_XOR = 5,
    OP_ABS = 6
};

enum COMPARETYPE {
    CMPT_EQUALS = 0,
    CMPT_GREATER = 1,
    CMPT_LESS = 2,
    CMPT_NOTEQ =3
};

enum DIRECTION {
    DIR_UP = 1,
    DIR_RIGHT = 2,
    DIR_DOWN = 3,
    DIR_LEFT = 4
};

enum PRIORITY {
    PRI_LOW = 0,
    PRI_MID,
    PRI_HIGH
};

#define GM_BASE             0x00B25000
#define GM_END              0x00B2E000

#define GM_FILE_START       0x00400000
#define FN_OFFSET2ADDR(offs) (void*)(GM_FILE_START + (DWORD)offs)

#define GM_PLAYER_KEY_UP    0
#define GM_PLAYER_KEY_DOWN  1
#define GM_PLAYER_KEY_LEFT  2
#define GM_PLAYER_KEY_RIGHT 3
#define GM_PLAYER_KEY_JUMP  4
#define GM_PLAYER_KEY_SJUMP 5
#define GM_PLAYER_KEY_X     6
#define GM_PLAYER_KEY_RUN   7
#define GM_PLAYER_KEY_SEL   8
#define GM_PLAYER_KEY_STR   9


// General
#define GM_SCRN_HDC         *(DWORD*)0x00B25028
#define GM_MODE_INTRO       *(WORD*)0x00B2C89C

// Pre-Defined Strings      
#define GM_STR_NULL         *(DWORD*)0x00423D00
#define GM_STR_MSGBOX       *(DWORD*)0x00B250E4
#define GM_STR_CHECKPOINT   *(DWORD*)0x00B250B0     //This contains the levelname (GM_FULLPATH) of the hitted checkpoint

// NPC Settings
#define GM_CONF_WIDTH       *(DWORD*)0x00B25BA8
#define GM_CONF_HEIGHT      *(DWORD*)0x00B25BC4
#define GM_CONF_GFXWIDTH    *(DWORD*)0x00B25BE0
#define GM_CONF_GFXHEIGHT   *(DWORD*)0x00B25BFC

// Frame counters
#define GM_TRANS_FRAMECT    *(DWORD*)0x00B2C670
//#define GM_ACTIVE_FRAMECT *(DWORD*)0x00B2C67C  Float?

// MOB Related memory
#define GM_NPCS_PTR         *(DWORD*)0x00B259E8     // +0xAD58 + 0x20  to NPCs
#define GM_NPCS_COUNT       *(WORD*)0x00B2595A 
#define GM_PLAYERS_PTR      *(DWORD*)0x00B25A20
#define GM_PLAYERS_COUNT    *(WORD*)0x00B2595E
#define GM_EDIT_PLAYERS_PTR *(DWORD*)0x00CF74D8     // Editor Template player

// HUD stuff
#define GM_STAR_COUNT       *(DWORD*)0x00B251E0
#define GM_COINS            *(WORD*)0x00B2C5A8
#define GM_PLAYER_LIVES     *(FLOAT*)0x00B2C5AC

// States
#define GM_FREEZWITCH_ACTIV *(WORD*)0x00B2C8B4
#define GM_PAUSE_OPEN       *(WORD*)0x00B250E2
#define GM_CUR_MENUCHOICE   *(WORD*)0x00B2C880      // Current menu choice

// Camera
#define GM_CAMERA_X         *(DWORD*)0x00B2B984
#define GM_CAMERA_Y         *(DWORD*)0x00B2B9A0

// Overworld base struct
#define GM_OVERWORLD_PTR    *(DWORD*)0x00B2C5C8

// Overworld Level Array
#define GM_LEVEL_COUNT      *(WORD*)0x00B25960
#define GM_LEVEL_BASE       *(DWORD*)0x00B25994


// Level related memory
#define GM_LVL_BOUNDARIES   *(DWORD*)0x00B257D4     // 6 doubles each section, L/U/R/D/?/?
#define GM_LVLFILENAME_PTR  *(DWORD*)0x00B2C5A4     // Lvl filename
#define GM_LVLNAME_PTR      *(DWORD*)0x00B2D764
#define GM_FULLPATH         *(DWORD*)0x00B2C618     // Full path to current .lvl file
#define GM_FULLDIR          *(DWORD*)0x00B2C61C     // Full path to current world dir
#define GM_CUR_LVL          *(WORD*)0x00B2C6D8

// Background objects
#define GM_BGO_COUNT        *(WORD*)0x00B25958
#define GM_BGOS_PTR         *(DWORD*)0x00B259B0

// Warps
#define GM_WARP_COUNT       *(WORD*)0x00B258E2
#define GM_WARPS_PTR        *(DWORD*)0x00B258F4

// Water/Quicksand areas
#define GM_WATER_AREA_COUNT *(WORD*)0x00B25700
#define GM_WATER_AREAS_PTR  *(DWORD*)0x00B256F4

// Layers
#define GM_LAYER_UNK_PTR    *(DWORD*)0x00B259E8
#define GM_LAYER_ARRAY_PTR  *(DWORD*)0x00B2C6B0

// Events
#define GM_EVENTS_PTR       *(DWORD*)0x00B2C6CC
#define GM_EVENT_TIMES_PTR  *(DWORD*)0x00B2D104     // array of 100 shorts
#define GM_EVENT_COUNT      *(WORD*)0x00B2D710

// Blocks
#define GM_BLOCK_COUNT      *(WORD*)0x00B25956
#define GM_BLOCKS_PTR       *(DWORD*)0x00B25A04

// Backgrounds
#define GM_SEC_BG_ARRAY     *(DOUBLE*)0x00B258B8
#define GM_BG_XPOS_PTR      *(DOUBLE*)0x00B2B984

// Animations
#define GM_ANIM_COUNT       *(WORD*)0x00B2595C
#define GM_ANIM_PTR         *(DWORD*)0x00B259CC

// Sound
#define GM_MUSIC_PATHS_PTR  *(DWORD*)0x00B257B8 
#define GM_SEC_MUSIC_TBL_P  *(DWORD*)0x00B25828     // 21 shorts containing music # for each section

// Input
#define GM_VKEY_TABLE_PTR   *(DWORD*)0x00B25068 
#define GM_INPUTSTR_BUF_PTR *(DWORD*)0x00B2C898

// Saves
#define GM_CUR_SAVE_SLOT    *(WORD*)0x00B2C62A      // 1 2 or 3

// Cheats
#define GM_PLAYER_INVULN    *(WORD*)0x00B2C8C0      // 0xFFFF = invuln
#define GM_PLAYER_INFJUMP   *(WORD*)0x00B2C8AC      // 0xFFFF = infinite jumps
#define GM_CHEATED          *(WORD*)0x00B2C8C4      // 0xFFFF = cheated

// Miscs
#define GM_GRAVITY          *(WORD*)0x00B2C6F4      // 12 = default, 0 = non-falling
#define GM_JUMPHIGHT        *(WORD*)0x00B2C6DC      // 20 = default, higher number = higher jumps
#define GM_JUMPHIGHT_BOUNCE *(WORD*)0x00B2C6E2      // Same as normal jumphight, but from NPC
#define GM_EARTHQUAKE       *(WORD*)0x00B250AC      // 0 = default, higher number = bigger shaking, slowly resets to 0

#define GM_MARIO_VS_LUIGI_T *(WORD*)0x00B2D760      // 0 = default, if higher than 0 then display text "Mario VS Luigi"
#define GM_WINS_T           *(WORD*)0x00B2D762      // 0 = default, if higher than 0 then display text "WINS!"

#define GM_WINNING          *(WORD*)0x00B2C59E      // 0 = not winning, if higher than 0 then winning by this win-type
#define GM_WORLD_MODE       *(WORD*)0x00B2C5B4      // 0xFFFF = leave current level
#define GM_INTRO_MODE       *(WORD*)0x00B2C620

#define GM_UNK_OV_DATABLOCK *(DWORD*)0x00B25164     // Pointer to some kind of overworld data block involving locked character selection (not 100% sure)

//Hitbox
#define GM_HITBOX_H_PTR     *(WORD*)0x00B2C6FC      // player hitbox height for each character/power-up state (starts with small mario through small link, then cycles same way through each power up)
#define GM_HITBOX_H_D_PTR   *(WORD*)0x00B2C742      // hitbox heights while ducking
#define GM_HITBOX_W_PTR     *(WORD*)0x00B2C788      // hitbox widths

//Startup Config:
#define GM_ISLEVELEDITORMODE    *(WORD*)0x00B25134
#define GM_ISGAME               *(WORD*)0x00B25046
#define GM_NOSOUND              *(WORD*)0x00B2D734
#define GM_FRAMESKIP            *(WORD*)0x00B2C684

//Graphics Memory
#define GM_GFX_NPC_PTR      *(DWORD*)0xB2CA98   // Array of NPC graphics HDCs, len 300, indexed by (npc.id - 1)
#define GM_GFX_NPC_MASK_PTR *(DWORD*)0xB2CAB4   // Array of NPC mask graphics HDCs, len 300, indexed by (npc.id - 1)

static  short** npc_gfxoffsetx = (short**)0x00B25B70;
static  short** npc_gfxoffsety = (short**)0x00B25B8C;
static  short** npc_width = (short**)0x00B25BA8;
static  short** npc_height = (short**)0x00B25BC4;
static  short** npc_gfxwidth = (short**)0x00B25BE0;
static  short** npc_gfxheight = (short**)0x00B25BFC;
static  short** npc_speed = (short**)0x00B25C18;
static  short** npc_isShell = (short**)0x00B25C34;
static  short** npc_npcblock = (short**)0x00B25C50;
static  short** npc_npcblocktop = (short**)0x00B25C6C;
static  short** isInteractableNPC_ptr = (short**)0x00B25C88;
static  short** isCoin_ptr = (short**)0x00B25CA4;
static  short** isVineNPC_ptr = (short**)0x00B25CC0;
static  short** isCollectableGoalNPC_ptr = (short**)0x00B25CDC;
static  short** npc_isflying = (short**)0x00B25CF8;
static  short** npc_isWaterNPC = (short**)0x00B25D14;
static  short** npc_jumphurt = (short**)0x00B25D30;
static  short** npc_noblockcollision = (short**)0x00B25D4C;
static  short** npc_score = (short**)0x00B25D68;
static  short** npc_playerblocktop = (short**)0x00B25D84;
static  short** npc_grabtop = (short**)0x00B25DA0;
static  short** npc_cliffturn = (short**)0x00B25DBC;
static  short** npc_nohurt = (short**)0x00B25DD8;
static  short** npc_playerblock = (short**)0x00B25DF4;
static  short** npc_grabside = (short**)0x00B25E2C;
static  short** isShoeNPC_ptr = (short**)0x00B25E48;
static  short** isYoshiNPC_ptr = (short**)0x00B25E64;
static  short** npc_noyoshi = (short**)0x00B25E9C;
static  short** npc_foreground = (short**)0x00B25EB8;
static  short** npc_isBot = (short**)0x00B25ED4;
static  short** isVegetableNPC_ptr = (short**)0x00B25F0C;
static  short** npc_nofireball = (short**)0x00B25F28;
static  short** npc_noiceball = (short**)0x00B25F44;
static  short** npc_nogravity = (short**)0x00B25F60;
static  short** npc_frames = (short**)0x00B25F7C;
static  short** npc_framespeed = (short**)0x00B25F98;
static  short** npc_framestyle = (short**)0x00B25FB4;

static  short** npcdef_gfxoffsetx = (short**)0x00B25FC0;
static  short** npcdef_gfxoffsety = (short**)0x00B2621A;
static  short** npcdef_width = (short**)0x00B26474;
static  short** npcdef_height = (short**)0x00B266CE;
static  short** npcdef_gfxwidth = (short**)0x00B26928;
static  short** npcdef_gfxheight = (short**)0x00B26B82;
static  short** npcdef_isShell = (short**)0x00B26DDC;
static  short** npcdef_npcblock = (short**)0x00B27036;
static  short** npcdef_npcblocktop = (short**)0x00B27290;
static  short** npcdef_isInteractableNPC = (short**)0x00B274EA;
static  short** npcdef_isCoin = (short**)0x00B27744;
static  short** npcdef_isVine = (short**)0x00B2799E;
static  short** npcdef_isCollectableGoal = (short**)0x00B27BF8;
static  short** npcdef_isFlyingNPC = (short**)0x00B27E52;
static  short** npcdef_isWaterNPC = (short**)0x00B280AC;
static  short** npcdef_jumphurt = (short**)0x00B28306;
static  short** npcdef_noblockcollision = (short**)0x00B28560;
static  short** npcdef_score = (short**)0x00B287BA;
static  short** npcdef_playerblocktop = (short**)0x00B28A14;
static  short** npcdef_grabtop = (short**)0x00B28C6E;
static  short** npcdef_cliffturn = (short**)0x00B28EC8;
static  short** npcdef_nohurt = (short**)0x00B29122;
static  short** npcdef_playerblock = (short**)0x00B2937C;
static  short** npcdef_grabside = (short**)0x00B29830;
static  short** npcdef_isShoeNPC = (short**)0x00B29A8A;
static  short** npcdef_isYoshiNPC = (short**)0x00B29CE4;
static  short** npcdef_noYoshi = (short**)0x00B2A198;
static  short** npcdef_foreground = (short**)0x00B2A3F2;
static  short** npcdef_isBot = (short**)0x00B2A64C;
static  short** npcdef_isVegetableNPC = (short**)0x00B2AB00;
static  short** npcdef_speed = (short**)0x00B2AD5C;
static  short** npcdef_nofireball = (short**)0x00B2B210;
static  short** npcdef_noiceball = (short**)0x00B2B46A;
static  short** npcdef_nogravity = (short**)0x00B2B6C4;

/////////////////////
///  -Assembly-   ///
/////////////////////

//VASM = value is assembly code
//npcToCoins ending Animation Settings
#define VASM_END_ANIM       *(BYTE*)0x00A3C86E      // = 11
#define VASM_END_COINSOUND  *(BYTE*)0x00A3C87F      // = 14
#define VASM_END_COINVAL    *(BYTE*)0x00A3C891      // = 1


////////////////////////
/// -Shared Strcuts- ///
////////////////////////

struct Momentum
{
    double x;
    double y;
    double height;
    double width;
    double speedX;
    double speedY;
};

struct Bounds
{
    double left;
    double top;
    double bottom;
    double right;
    double unk1;
    double unk2;
};

struct KeyMap{
    short    UKeyState;
    short    DKeyState;
    short    LKeyState;
    short    RKeyState;
    short    JKeyState;
    short    SJKeyState;
    short    XKeyState;
    short    RNKeyState;
    short    SELKeyState;
    short    STRKeyState;
};

////////////////////////
///    -Imports-     ///
////////////////////////

#define IMP_vbaStrCmp       *(DWORD*)0x004010F8 // Ptr to __stdcall
#define IMP_vbaStrCopy      *(DWORD*)0x004011b0 // Ptr to __fastcall
#define IMP_vbaFreeStr      *(DWORD*)0x00401248 // Ptr to __fastcall
#define IMP_rtcRandomize    *(DWORD*)0x00401090 // Ptr to __stdcall

static const auto native_rtcRandomize = (void(__stdcall *)(void *))IMP_rtcRandomize;

////////////////////////
///    -Functions-   ///
////////////////////////

// Print using game's text function

//      Arg1 = WCSTR* string
//      Arg2 = Int* fonttype
//      Arg3 = float* x
//      Arg4 = float* y
#define GF_PRINT            0x00951F50

// Start kill event for a player
//      Arg1 = int* to Index of player
#define GF_KILL_PLAYER      0x009B66D0

//      Arg1 = int* Index of player
#define GF_HARM_PLAYER      0x009B51E0

//      Arg1 = int* Sprite ID
//      Arg2 = int* Unk flags
//      Arg3 = int* Unk
#define GF_INIT_NPC         0x00A03630

//      Arg1 = int* Index of NPC in NPC list
#define GF_UPDATE_NPC       0x00A3B680

//      Arg1 = int* Index of section containing music settings to play now
#define GF_PLAY_MUSIC       0x00A61B40

//      Arg1 = int* SoundIndex
#define GF_PLAY_SFX         0x00A73FD0

//      Arg1 = POS* structure of player, POS* structure of block
//      1=Collision from top, 2=From right, 3=From bottom, 4=From left, 5=?
#define GF_MOB_BLOCK_COL    0x00994250

//      Arg1 = int* (1)
//      Arg2 = Momentum* structure
#define GF_IS_ON_CAMERA     0x00993DE0

//      Arg1 = wchar_t** name of event to start
//      Arg2 = short* [VB6 Bool] Force no smoke (ignore Event setting)
#define GF_TRIGGER_EVENT    0x00AA42D0

//      Arg1
#define GF_BITBLT_CALL      0x004242D0

//      Arg1 = int* Killed NPC index
//      Arg2 = NPCMOB* Killer NPC
#define GF_NPC_CLEANUP      0x00A315A0

//      No Args, Does convert every NPC to coins (Like player would win)
#define GF_NPC_TO_COINS     0x00A3C580

//      Arg1 = short* Animation ID
//      Arg2 = Momentum* (for x and y coor)
//      Arg3 = float* Animation frame (NOTE: Not the direct frame of the gif-file)
//      Arg4 = short* npcID (for yoshi egg)
//      Arg5 = short* [VB 6 Bool] True, if only drawing the mask => Shadow effect
#define GF_RUN_ANIM         0x009E7380

//      Arg1 = short* Base score
//      Arg2 = Momentum* (for x and y coor)
//      Arg3 = short* Score factor
//      Actually Arg1 and Arg3 will be multiplied
#define GF_SCORE_RELEATED   0x008F80C0

//      This function displays the native SMBX messagebox AND menu.
//      To display you text you need to set GM_STR_MSGBOX before the call and reset it after the call.
//      Arg1 = short* Some sort of type (mostly the value 1)
#define GF_MSGBOX           0x008E54C0

//      Arg1 = VB6StrPtr* layerName
//      Arg2 = short* [VB 6 Bool] NoSmoke: False, to display the smoke effect
#define GF_SHOW_LAYER       0x00AA2760

//      Arg1 = VB6StrPtr* layerName
//      Arg2 = short* [VB 6 Bool] NoSmoke: False, to display the smoke effect
#define GF_HIDE_LAYER       0x00AA3730

#define GF_POW              0x009E4600

#define GF_LOAD_LOCAL_GFX   0x00ACD220

static const auto native_print          = (void(__stdcall *)(VB6StrPtr* /*Text*/, short* /*fonttype*/, float* /*x*/, float* /*y*/))GF_PRINT;

static const auto native_killPlayer     = (void(__stdcall *)(short* /*playerIndex*/))GF_KILL_PLAYER;
static const auto native_harmPlayer     = (void(__stdcall *)(short* /*playerIndex*/))GF_HARM_PLAYER;

static const auto native_playMusic      = (void(__stdcall *)(short* /*section*/))GF_PLAY_MUSIC;
static const auto native_playSFX        = (void(__stdcall *)(short* /*soundIndex*/))GF_PLAY_SFX;

static const auto native_npcToCoins     = (void(__stdcall *)())GF_NPC_TO_COINS;
static const auto native_doPow          = (void(__stdcall *)())GF_POW;

static const auto native_runEffect      = (void(__stdcall *)(short* /*EffectID*/, Momentum* /*coor*/, float* /*EffectFrame*/, short* /*npcID*/, short* /*showOnlyMask*/))GF_RUN_ANIM;
static const auto native_addScoreEffect = (void(__stdcall *)(short* /*baseValue*/, Momentum* /*coor*/, short* /*factor*/))GF_SCORE_RELEATED;

static const auto native_msgbox         = (void(__stdcall *)(short* /*unkVal*/))GF_MSGBOX;

static const auto native_triggerEvent   = (void(__stdcall *)(VB6StrPtr* /*eventName*/, short* /*forceNoSmoke*/))GF_TRIGGER_EVENT;

static const auto native_showLayer      = (void(__stdcall *)(VB6StrPtr* /*layerName*/, short* /*noSmoke*/))GF_SHOW_LAYER;
static const auto native_hideLayer      = (void(__stdcall *)(VB6StrPtr* /*layerName*/, short* /*noSmoke*/))GF_HIDE_LAYER;

static const auto native_loadLocalGfx   = (void(__stdcall *)())GF_LOAD_LOCAL_GFX;

/*
_O_Pub_Obj_Inf1_Event0x3                008BD770 P
_O_Pub_Obj_Inf1_Event0x4                008BD900 P
_O_Pub_Obj_Inf1_Event0x5                008BD9E0 P
_O_Pub_Obj_Inf1_Event0x6                008BDAB0 P
_O_Pub_Obj_Inf1_Event0x7                008BDB40 P
_O_Pub_Obj_Inf1_Event0x8                008BDE80 P
_O_Pub_Obj_Inf1_Event0x9                008BDF10 P
_O_Pub_Obj_Inf1_Event0xA                008BE080 P
_O_Pub_Obj_Inf1_Event0xB                008BE100 P
_O_Pub_Obj_Inf1_Event0xC                008BE1A0 P
_O_Pub_Obj_Inf1_Event0xD                008BE360 P
unkInitVals                             008BE410
Sub_Main                                008BE9C0 P
loadNPCDefaults                         008C2720
levelLoop                               008CA210
introLoop                               008CA650
exitMainGame                            008D6BB0
saveLevel                               008D6CF0
loadLevel                               008D8F40
goToWorldmap                            008DDE30
doWinAnimations                         008DE080
saveWorld                               008DEB60
loadWorld                               008DF5B0
worldLoop                               008E06B0
loadWorlds                              008E35E0
enumAllBattleLevels                     008E3D10
unkCountPercentageOfGameSave            008E41D0
saveGame                                008E47D0
loadGame                                008E4E00
unkMessageBox_8E54C0                    008E54C0
loadDefaultControls                     008E6700
loadGameConfiguration                   008E6910
unkMusicManagement_8E6F70               008E6F70
updateScanCheat                         008E7490
constructCreditsText                    008F7200
unkDoorsCount                           008F7D70
addToCredits                            008F8000
createMomentumStruct                    008F8050
funcScoreRelated                        008F80C0
unkBlockBoolInit                        008FE2A0
unkRenderWorldHUD_8FEB10                008FEB10
unkRender_909290                        00909290
initGameHDC                             0094F680
setLevelWindowDefaults                  0094F8D0
PrintText                               00951F50
unkBackgroundDraw                       00954F50
initGameWindow                          0096AD80
unkRenderHud_96BF20                     0096BF20
unkPaintNPCOnScreen                     009920D0
unkCollisionCheck                       00992F50
IsOnCamera                              00993DE0
MobBlockColl                            00994250
runGameLoop                             00995A20
HarmPlayer                              009B51E0
KillPlayer                              009B66D0
unkDuckingUpdate                        009B7BF0
unkPickupAnimationRelated               009BB2B0
unkPlayerUpdate9C0C50                   009C0C50
unkPlayerCollisionCheck                 009CAEC0
togglePSwitch                           009E33B0
doPOW                                   009E4600
updateEffect                            009E4900
runEffect                               009E7380
cleanupEffect                           009EC150
runEditorLevel                          00A02220
InitNPC                                 00A03630
unkOnInteractA24CD0                     00A24CD0
NpcKill                                 00A315A0
UpdateNPC                               00A3B680
doBombExplosion                         00A3BA90
npcs_to_coins                           00A3C580
triggerMovementOfSkullRaft              00A446A0
updateNPC                               00A448D0
updateNPCRelated                        00A52BB0
unkOnUpdateMovementNPC                  00A60AF0
PlayMusic                               00A61B40
initAssignSoundFiles                    00A624D0
playSound                               00A73FD0
unkCheckMusicState                      00A74460
updateInput                             00A74910
unkJoystick                             00A75680
_O_Pub_Obj_InfD_Event0x1                00A75970 P
_O_Pub_Obj_InfD_Event0x2                00A75A40 P
_O_Pub_Obj_InfD_Event0x3                00A75AB0 P
_O_Pub_Obj_InfD_Event0x4                00A75BE0 P
_O_Pub_Obj_InfD_Event0x5                00A75D10 P
_O_Pub_Obj_InfD_Event0x6                00A75D80 P
_O_Pub_Obj_InfD_Event0x7                00A75E00 P
_O_Pub_Obj_InfD_Event0x8                00A76100 P
_O_Pub_Obj_InfD_Event0x9                00A761B0 P
_O_Pub_Obj_InfE_Event0x1                00A76A80 P
_O_Pub_Obj_InfE_Event0x2                00A76B50 P
_O_Pub_Obj_InfE_Event0x3                00A76BC0 P
_O_Pub_Obj_InfE_Event0x4                00A76CF0 P
_O_Pub_Obj_InfE_Event0x5                00A76E90 P
_O_Pub_Obj_InfE_Event0x6                00A77060 P
_O_Pub_Obj_InfE_Event0x7                00A773D0 P
_O_Pub_Obj_InfE_Event0x8                00A77480 P
_O_Pub_Obj_InfE_Event0x9                00A77500 P
_O_Pub_Obj_InfF_Event0x1                00A77F30 P
_O_Pub_Obj_InfF_Event0x2                00A78050 P
_O_Pub_Obj_InfF_Event0x3                00A78170 P
_O_Pub_Obj_InfF_Event0x4                00A7C7B0 P
_O_Pub_Obj_InfF_Event0x5                00A7C820 P
_O_Pub_Obj_InfF_Event0x6                00A7C890 P
_O_Pub_Obj_InfF_Event0x7                00A7CB40 P
_O_Pub_Obj_InfF_Event0x8                00A7CDF0 P
_O_Pub_Obj_InfF_Event0x9                00A7CEE0 P
_O_Pub_Obj_InfF_Event0xA                00A7D0D0 P
_O_Pub_Obj_InfF_Event0xB                00A7D140 P
_O_Pub_Obj_InfF_Event0xC                00A7D570 P
_O_Pub_Obj_InfF_Event0xD                00A7D680 P
_O_Pub_Obj_InfF_Event0xE                00A7D790 P
_O_Pub_Obj_InfF_Event0xF                00A7D980 P
_O_Pub_Obj_InfF_Event0x10               00A7DBE0 P
_O_Pub_Obj_InfF_Event0x11               00A7DC70 P
_O_Pub_Obj_InfF_Event0x12               00A7DCE0 P
_O_Pub_Obj_InfF_Event0x13               00A7DE00 P
_O_Pub_Obj_InfF_Event0x14               00A7DE80 P
_O_Pub_Obj_InfF_Event0x15               00A7DF00 P
_O_Pub_Obj_InfF_Event0x16               00A7E020 P
_O_Pub_Obj_InfF_Event0x17               00A7E0A0 P
_O_Pub_Obj_InfF_Event0x18               00A7E120 P
_O_Pub_Obj_InfF_Event0x19               00A7E1A0 P
_O_Pub_Obj_InfF_Event0x1A               00A7E220 P
_O_Pub_Obj_InfF_Event0x1B               00A7E2B0 P
_O_Pub_Obj_InfF_Event0x1C               00A7E330 P
_O_Pub_Obj_InfF_Event0x1D               00A7E420 P
_O_Pub_Obj_InfF_Event0x1E               00A7E8C0 P
_O_Pub_Obj_InfF_Event0x1F               00A7E9B0 P
_O_Pub_Obj_InfF_Event0x20               00A7EF10 P
_O_Pub_Obj_Inf10_Method0x1              00A814C0 P
_O_Pub_Obj_Inf10_Event0x2               00A81530 P
_O_Pub_Obj_Inf10_Event0x3               00A81E20 P
_O_Pub_Obj_Inf10_Event0x4               00A81E90 P
_O_Pub_Obj_Inf10_Event0x5               00A82050 P
_O_Pub_Obj_Inf10_Event0x6               00A820C0 P
_O_Pub_Obj_Inf10_Event0x7               00A82140 P
_O_Pub_Obj_Inf10_Event0x8               00A82210 P
_O_Pub_Obj_Inf10_Event0x9               00A82280 P
_O_Pub_Obj_Inf10_Event0xA               00A824E0 P
_O_Pub_Obj_Inf10_Event0xB               00A829E0 P
_O_Pub_Obj_Inf11_Event0x1               00A82A60 P
_O_Pub_Obj_Inf11_Event0x2               00A82EC0 P
_O_Pub_Obj_Inf11_Event0x3               00A83300 P
_O_Pub_Obj_Inf11_Event0x4               00A83740 P
_O_Pub_Obj_Inf11_Event0x5               00A83B80 P
_O_Pub_Obj_Inf11_Event0x6               00A83FC0 P
_O_Pub_Obj_Inf11_Event0x7               00A84280 P
_O_Pub_Obj_Inf11_Event0x8               00A843A0 P
_O_Pub_Obj_Inf11_Event0x9               00A84840 P
_O_Pub_Obj_Inf11_Event0xA               00A84A00 P
_O_Pub_Obj_Inf11_Event0xB               00A84DC0 P
_O_Pub_Obj_Inf11_Event0xC               00A850C0 P
_O_Pub_Obj_Inf11_Event0xD               00A85590 P
_O_Pub_Obj_Inf11_Event0xE               00A86D30 P
_O_Pub_Obj_Inf12_Event0x2               00A86F50 P
_O_Pub_Obj_Inf12_Event0x3               00A87790 P
_O_Pub_Obj_Inf12_Event0x4               00A87900 P
_O_Pub_Obj_Inf12_Event0x5               00A87B40 P
_O_Pub_Obj_Inf12_Event0x6               00A87CB0 P
_O_Pub_Obj_Inf12_Event0x7               00A87EA0 P
_O_Pub_Obj_Inf12_Event0x8               00A87FF0 P
_O_Pub_Obj_Inf12_Event0x9               00A88140 P
_O_Pub_Obj_Inf12_Event0xA               00A89250 P
_O_Pub_Obj_Inf12_Event0xB               00A89370 P
_O_Pub_Obj_Inf12_Event0xC               00A89520 P
_O_Pub_Obj_Inf12_Event0xD               00A896D0 P
_O_Pub_Obj_Inf12_Event0xE               00A89840 P
_O_Pub_Obj_Inf12_Event0xF               00A89C00 P
_O_Pub_Obj_Inf12_Event0x10              00A89FC0 P
_O_Pub_Obj_Inf12_Event0x11              00A8A380 P
_O_Pub_Obj_Inf12_Event0x12              00A8A740 P
_O_Pub_Obj_Inf12_Event0x13              00A8AB00 P
_O_Pub_Obj_Inf12_Event0x14              00A8AEC0 P
_O_Pub_Obj_Inf12_Event0x15              00A8B0A0 P
_O_Pub_Obj_Inf12_Event0x1               00A8B150 P
_O_Pub_Obj_Inf13_Event0x2               00A8D850 P
_O_Pub_Obj_Inf13_Event0x3               00A8DD40 P
_O_Pub_Obj_Inf13_Event0x4               00A8E300 P
_O_Pub_Obj_Inf13_Event0x5               00A8E5B0 P
_O_Pub_Obj_Inf13_Event0x6               00A8E7A0 P
_O_Pub_Obj_Inf13_Event0x7               00A8E990 P
_O_Pub_Obj_Inf13_Event0x8               00A8EB80 P
_O_Pub_Obj_Inf13_Event0x9               00A8EE60 P
_O_Pub_Obj_Inf13_Event0xA               00A8F420 P
_O_Pub_Obj_Inf13_Event0xB               00A8F950 P
_O_Pub_Obj_Inf13_Event0xC               00A8FA70 P
_O_Pub_Obj_Inf13_Event0xD               00A8FD50 P
_O_Pub_Obj_Inf13_Event0xE               00A90310 P
_O_Pub_Obj_Inf13_Event0xF               00A93770 P
_O_Pub_Obj_Inf13_Event0x10              00A93830 P
_O_Pub_Obj_Inf13_Event0x11              00A93970 P
_O_Pub_Obj_Inf13_Event0x12              00A94050 P
_O_Pub_Obj_Inf13_Event0x1               00A941D0 P
_O_Pub_Obj_Inf14_Event0x1               00A943F0 P
_O_Pub_Obj_Inf14_Event0x2               00A946F0 P
_O_Pub_Obj_Inf14_Event0x3               00A94AF0 P
_O_Pub_Obj_Inf14_Event0x4               00A94C10 P
_O_Pub_Obj_Inf15_Event0x1               00A94FD0 P
_O_Pub_Obj_Inf15_Event0x2               00A95140 P
_O_Pub_Obj_Inf15_Event0x3               00A952B0 P
_O_Pub_Obj_Inf15_Event0x4               00A95590 P
_O_Pub_Obj_Inf15_Event0x5               00A95870 P
_O_Pub_Obj_Inf15_Event0x6               00A95B50 P
_O_Pub_Obj_Inf15_Event0x7               00A95C70 P
_O_Pub_Obj_Inf15_Event0x8               00A95DE0 P
_O_Pub_Obj_Inf15_Event0x9               00A95F50 P
_O_Pub_Obj_Inf15_Event0xA               00A960C0 P
_O_Pub_Obj_Inf15_Event0xB               00A966B0 P
_O_Pub_Obj_Inf16_Event0x1               00A96A40 P
_O_Pub_Obj_Inf16_Event0x2               00A96D60 P
_O_Pub_Obj_Inf16_Event0x3               00A97140 P
_O_Pub_Obj_Inf17_Event0x1               00A97440 P
_O_Pub_Obj_Inf18_Method0x1              00A975B0 P
_O_Pub_Obj_Inf18_Event0x2               00A97720 P
_O_Pub_Obj_Inf18_Event0x3               00A97890 P
_O_Pub_Obj_Inf18_Event0x4               00A97C20 P
_O_Pub_Obj_Inf19_Event0x1               00A97D90 P
_O_Pub_Obj_Inf1A_Event0x1               00A97F00 P
saveOldDisplayOptions                   00A98070
unkRestoreResolution                    00A98150
applyFullscreenResolution               00A98190
saveDisplayOptions                      00A984A0
_O_Pub_Obj_Inf1D_Event0x1               00A9A1E0 P
_O_Pub_Obj_Inf1D_Event0x2               00A9A430 P
_O_Pub_Obj_Inf1D_Event0x3               00A9A4C0 P
_O_Pub_Obj_Inf1E_Event0x1               00A9A680 P
_O_Pub_Obj_Inf1E_Event0x2               00A9A760 P
_O_Pub_Obj_Inf1E_Event0x3               00A9A840 P
_O_Pub_Obj_Inf1E_Event0x4               00A9A920 P
_O_Pub_Obj_Inf1E_Event0x5               00A9AA00 P
_O_Pub_Obj_Inf1E_Event0x6               00A9AAE0 P
_O_Pub_Obj_Inf1E_Event0x7               00A9AB70 P
_O_Pub_Obj_Inf1E_Event0x8               00A9AC00 P
_O_Pub_Obj_Inf1E_Event0x9               00A9ACC0 P
_O_Pub_Obj_Inf1E_Event0xA               00A9AD80 P
_O_Pub_Obj_Inf1E_Event0xB               00A9AE40 P
_O_Pub_Obj_Inf1E_Event0xC               00A9AF00 P
_O_Pub_Obj_Inf1E_Event0xD               00A9B850 P
_O_Pub_Obj_Inf1E_Event0xE               00A9BCF0 P
_O_Pub_Obj_Inf1E_Event0xF               00A9C190 P
_O_Pub_Obj_Inf1E_Event0x10              00A9C980 P
_O_Pub_Obj_Inf1E_Event0x11              00A9D170 P
_O_Pub_Obj_Inf1E_Event0x12              00A9D200 P
_O_Pub_Obj_Inf1E_Event0x13              00A9D2C0 P
_O_Pub_Obj_Inf1E_Event0x14              00A9D300 P
_O_Pub_Obj_Inf1E_Event0x15              00A9D390 P
_O_Pub_Obj_Inf1E_Event0x16              00A9D420 P
_O_Pub_Obj_Inf1E_Event0x17              00A9D4E0 P
_O_Pub_Obj_Inf1E_Event0x18              00A9D570 P
_O_Pub_Obj_Inf1E_Event0x19              00A9D590 P
_O_Pub_Obj_Inf1E_Event0x1A              00A9D620 P
_O_Pub_Obj_Inf1E_Event0x1B              00A9D6E0 P
_O_Pub_Obj_Inf1E_Event0x1C              00A9D7A0 P
_O_Pub_Obj_Inf1E_Event0x1D              00A9D830 P
_O_Pub_Obj_Inf1E_Event0x1E              00A9D8F0 P
_O_Pub_Obj_Inf1E_Event0x1F              00A9D9B0 P
_O_Pub_Obj_Inf1E_Event0x20              00A9DA40 P
_O_Pub_Obj_Inf1E_Event0x21              00A9DAD0 P
_O_Pub_Obj_Inf1E_Event0x22              00A9DB90 P
_O_Pub_Obj_Inf1F_Method0x1              00A9E440 P
_O_Pub_Obj_Inf1F_Event0x2               00A9E7D0 P
_O_Pub_Obj_Inf1F_Event0x3               00A9E910 P
_O_Pub_Obj_Inf20_Event0x2               00A9E9A0 P
_O_Pub_Obj_Inf20_Event0x3               00A9F260 P
_O_Pub_Obj_Inf20_Event0x4               00A9FE00 P
_O_Pub_Obj_Inf20_Event0x5               00A9FF10 P
_O_Pub_Obj_Inf20_Event0x6               00A9FFA0 P
_O_Pub_Obj_Inf20_Event0x7               00AA0F20 P
_O_Pub_Obj_Inf20_Event0x8               00AA1250 P
_O_Pub_Obj_Inf20_Event0x1               00AA2160 P
TriggerEvent                            00AA42D0
_O_Pub_Obj_Inf22_Event0x2               00AA7F30 P
_O_Pub_Obj_Inf22_Event0x3               00AA8410 P
_O_Pub_Obj_Inf22_Event0x4               00AA87A0 P
_O_Pub_Obj_Inf22_Event0x5               00AA8B30 P
_O_Pub_Obj_Inf22_Event0x6               00AA8EC0 P
_O_Pub_Obj_Inf22_Event0x7               00AA9250 P
_O_Pub_Obj_Inf22_Event0x8               00AA96A0 P
_O_Pub_Obj_Inf22_Event0x9               00AA9AF0 P
_O_Pub_Obj_Inf22_Event0xA               00AA9E80 P
_O_Pub_Obj_Inf22_Event0xB               00AAA210 P
_O_Pub_Obj_Inf22_Event0xC               00AAA560 P
_O_Pub_Obj_Inf22_Event0xD               00AAA970 P
_O_Pub_Obj_Inf22_Event0xE               00AAADE0 P
_O_Pub_Obj_Inf22_Event0xF               00AAB770 P
_O_Pub_Obj_Inf22_Event0x10              00AABDD0 P
_O_Pub_Obj_Inf22_Event0x11              00AAD210 P
_O_Pub_Obj_Inf22_Event0x12              00AADD30 P
_O_Pub_Obj_Inf22_Event0x13              00AAE1F0 P
_O_Pub_Obj_Inf22_Event0x14              00AAE660 P
_O_Pub_Obj_Inf22_Event0x15              00AAECC0 P
_O_Pub_Obj_Inf22_Event0x16              00AAF5C0 P
_O_Pub_Obj_Inf22_Event0x17              00AAFA80 P
_O_Pub_Obj_Inf22_Event0x18              00AAFEF0 P
_O_Pub_Obj_Inf22_Event0x19              00AB03C0 P
_O_Pub_Obj_Inf22_Event0x1A              00AB0840 P
_O_Pub_Obj_Inf22_Event0x1B              00AB0BD0 P
_O_Pub_Obj_Inf22_Event0x1C              00AB0F60 P
_O_Pub_Obj_Inf22_Event0x1D              00AB12F0 P
_O_Pub_Obj_Inf22_Event0x1E              00AB1680 P
_O_Pub_Obj_Inf22_Event0x1F              00AB1D30 P
_O_Pub_Obj_Inf22_Event0x20              00AB1DC0 P
_O_Pub_Obj_Inf22_Event0x21              00AB2110 P
_O_Pub_Obj_Inf22_Event0x22              00AB2250 P
_O_Pub_Obj_Inf22_Event0x23              00AB28E0 P
_O_Pub_Obj_Inf22_Event0x24              00AB2D00 P
_O_Pub_Obj_Inf22_Event0x25              00AB3120 P
_O_Pub_Obj_Inf22_Event0x26              00AB3490 P
_O_Pub_Obj_Inf22_Event0x1               00AB4A70 P
_O_Pub_Obj_Inf22_Event0x27              00AB92B0 P
_O_Pub_Obj_Inf22_Event0x28              00AB96D0 P
_O_Pub_Obj_Inf23_Method0x1              00AB9AF0 P
_O_Pub_Obj_Inf24_Event0x1               00AB9B80 P
loadDefaultGraphics                     00ABA390
clearGraphics                           00ACB440
loadLocalGraphics                       00ACD220
unkLoadGraphics                         00AD3420
unkLoadLocalGraphics                    00AD9DC0
loadCustomWorldGraphics                 00ADFF90
loadDefaultWorldGraphics                00AE2630
_O_Pub_Obj_Inf26_Event0x2               00AE8B30 P
_O_Pub_Obj_Inf26_Event0x3               00AE8C70 P
_O_Pub_Obj_Inf26_Event0x4               00AE90D0 P
_O_Pub_Obj_Inf26_Event0x5               00AE91D0 P
_O_Pub_Obj_Inf26_Event0x1               00AE9240 P
_O_Pub_Obj_Inf26_Event0x6               00AEA740 P
_O_Pub_Obj_Inf27_Event0x2               00AEAB70 P
_O_Pub_Obj_Inf27_Event0x3               00AEAC00 P
_O_Pub_Obj_Inf27_Event0x4               00AEACF0 P
_O_Pub_Obj_Inf27_Event0x5               00AEAE60 P
_O_Pub_Obj_Inf27_Method0x1              00AEB180 P
_O_Pub_Obj_Inf27_Event0x6               00AEB9E0 P
_O_Pub_Obj_Inf27_Event0x7               00AEBB50 P
_O_Pub_Obj_Inf28_Event0x1               00AEC010 P
_O_Pub_Obj_Inf28_Event0x2               00AEC1C0 P
_O_Pub_Obj_Inf28_Event0x3               00AEC250 P
_O_Pub_Obj_Inf28_Event0x4               00AEC530 P
_O_Pub_Obj_Inf29_Method0x1              00AEC690 P
_O_Pub_Obj_Inf29_Event0x2               00AEC970 P
_O_Pub_Obj_Inf29_Event0x3               00AECAE0 P
_O_Pub_Obj_Inf29_Event0x4               00AECD20 P
_O_Pub_Obj_Inf2A_Method0x1              00AECE90 P
_O_Pub_Obj_Inf2B_Event0x19              00AED060 P
_O_Pub_Obj_Inf2B_Event0x1A              00AED320 P
_O_Pub_Obj_Inf2B_Event0x1B              00AEDAC0 P
_O_Pub_Obj_Inf2B_Event0x1C              00AEE490 P
_O_Pub_Obj_Inf2B_Event0x1D              00AEE8B0 P
_O_Pub_Obj_Inf2B_Event0x1E              00AEEFE0 P
_O_Pub_Obj_Inf2B_Event0x1               00AEFA60 P
_O_Pub_Obj_Inf2B_Event0x2               00B0C0B0 P
_O_Pub_Obj_Inf2B_Event0x3               00B0C4B0 P
_O_Pub_Obj_Inf2B_Event0x4               00B0EB70 P
_O_Pub_Obj_Inf2B_Event0x5               00B0F2A0 P
_O_Pub_Obj_Inf2B_Event0x6               00B0F4B0 P
_O_Pub_Obj_Inf2B_Event0x7               00B0FB00 P
_O_Pub_Obj_Inf2B_Event0x8               00B0FEF0 P
_O_Pub_Obj_Inf2B_Event0x9               00B10250 P
_O_Pub_Obj_Inf2B_Event0xA               00B10580 P
_O_Pub_Obj_Inf2B_Event0xB               00B10890 P
_O_Pub_Obj_Inf2B_Event0xC               00B113E0 P
_O_Pub_Obj_Inf2B_Event0xD               00B11740 P
_O_Pub_Obj_Inf2B_Event0xE               00B121B0 P
_O_Pub_Obj_Inf2B_Event0xF               00B12420 P
_O_Pub_Obj_Inf2B_Event0x10              00B12690 P
_O_Pub_Obj_Inf2B_Event0x11              00B137F0 P
_O_Pub_Obj_Inf2B_Event0x12              00B13980 P
_O_Pub_Obj_Inf2B_Event0x13              00B145C0 P
_O_Pub_Obj_Inf2B_Event0x14              00B14980 P
_O_Pub_Obj_Inf2B_Event0x15              00B15680 P
_O_Pub_Obj_Inf2B_Event0x16              00B15B20 P
_O_Pub_Obj_Inf2B_Event0x1F              00B17480 P
_O_Pub_Obj_Inf2B_Event0x20              00B17890 P
_O_Pub_Obj_Inf2B_Event0x21              00B17970 P
_O_Pub_Obj_Inf2B_Event0x22              00B17AA0 P
_O_Pub_Obj_Inf2B_Event0x23              00B18270 P
_O_Pub_Obj_Inf2B_Event0x24              00B18500 P
_O_Pub_Obj_Inf2B_Event0x25              00B18980 P
_O_Pub_Obj_Inf2B_Event0x17              00B1A060 P
_O_Pub_Obj_Inf2B_Event0x18              00B1AD90 P
_O_Pub_Obj_Inf2B_Event0x26              00B1B430 P
_O_Pub_Obj_Inf2C_Event0x1               00B1B6F0 P
_O_Pub_Obj_Inf2C_Event0x2               00B1B960 P
_O_Pub_Obj_Inf2C_Event0x3               00B1D740 P
_O_Pub_Obj_Inf2C_Event0x4               00B1D820 P
_O_Pub_Obj_Inf2C_Event0x5               00B1D9A0 P
_O_Pub_Obj_Inf2C_Event0x6               00B1DA30 P
_O_Pub_Obj_Inf2C_Event0x7               00B1DC20 P
_O_Pub_Obj_Inf2C_Event0x8               00B1DD90 P
_O_Pub_Obj_Inf2C_Event0x9               00B1DE70 P
_O_Pub_Obj_Inf2D_Method0x1              00B1E510 P
_O_Pub_Obj_Inf2D_Event0x2               00B1E5A0 P
_O_Pub_Obj_Inf2E_Event0x1               00B1F790 P
_O_Pub_Obj_Inf2E_Event0x2               00B1FAE0 P
_O_Pub_Obj_Inf2E_Event0x3_tmrLoad_Timer 00B1FDF0 P
_O_Pub_Obj_Inf2F_Event0x1               00B20000 P
_O_Pub_Obj_Inf2F_Event0x2               00B20080 P
_O_Pub_Obj_Inf2F_Event0x3               00B200F0 P
_O_Pub_Obj_Inf2F_Event0x4               00B20160 P
_O_Pub_Obj_Inf2F_Event0x5               00B20230 P
saveNPCDefaults                         00B202B0
loadDefaultNPCConfiguration             00B20870
loadNPCConfigurations                   00B20E50
loadNPCConfiguration                    00B21200
_O_Pub_Obj_Inf31_Event0x1               00B22900 P
_O_Pub_Obj_Inf31_Event0x3               00B229C0 P
_O_Pub_Obj_Inf31_Event0x4               00B22B20 P
_O_Pub_Obj_Inf31_Event0x5               00B22DB0 P
_O_Pub_Obj_Inf31_Event0x2               00B22E90 P
_O_Pub_Obj_Inf31_Event0x6               00B23F40 P

*/

//DEBUG:
#define dbgbox(msg) MessageBoxW(NULL, msg, L"Dbg", NULL);
#define dbgboxA(msg) MessageBoxA(NULL, msg, "Dbg", NULL);

#endif


