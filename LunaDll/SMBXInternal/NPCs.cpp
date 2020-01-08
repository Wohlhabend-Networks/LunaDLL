#include "NPCs.h"
#include "../Misc/MiscFuncs.h"
#include <list>
#include <unordered_map>

// GET
NPCMOB* NPC::Get(int index) {
    if(index < 0 || index > GM_NPCS_COUNT)
        return nullptr;
        
    return &((NPCMOB*)GM_NPCS_PTR)[index + 129]; // +129 makes an offset of 0xAD58
}

// GET where 0 is the Dummy NPC
NPCMOB * NPC::GetRaw(int index)
{
    if (index < 0 || index > GM_NPCS_COUNT)
        return nullptr;

    return &((NPCMOB*)GM_NPCS_PTR)[index + 128];
}

NPCMOB * NPC::GetDummyNPC()
{
    return &((NPCMOB*)GM_NPCS_PTR)[128];
}

// GET FIRST MATCH
NPCMOB* NPC::GetFirstMatch(int ID, int section) {
    bool anyID = (ID == -1 ? true : false);
    bool anySec = (section == -1 ? true : false);
    NPCMOB* thisnpc = NULL;

    for(int i = 0; i < GM_NPCS_COUNT; i++) {
        thisnpc = Get(i);
        if(thisnpc->id == ID || anyID) {
            if(GetSection(thisnpc) == section || anySec) {
                return thisnpc; //matched
            }
        }
    }
    return NULL; //not matched
}

// FIND ALL
void NPC::FindAll(int ID, int section, std::list<NPCMOB*>* return_list) {
    bool anyID = (ID == -1 ? true : false);
    bool anySec = (section == -1 ? true : false);
    NPCMOB* thisnpc = NULL;	

    for(int i = 0; i < GM_NPCS_COUNT; i++) {
        thisnpc = Get(i);
        if(thisnpc->id == ID || anyID) {
            if(GetSection(thisnpc) == section || anySec) {
                return_list->push_back(thisnpc);
            }
        }
    }
}

// SET HITS
void NPC::SetHits(NPCMOB* npc, float hits) {
    npc->hitCount = hits;
}

// GET SECTION
short NPC::GetSection(NPCMOB* npc) {
    return npc->currentSection;
}

// FACE DIRECTION
void NPC::FaceDirection(NPCMOB* npc, float direction) {
    npc->directionFaced2 = direction; // The version at 0x118
}

// MEM SET
void NPC::MemSet(int ID, int offset, double value, OPTYPE operation, FIELDTYPE ftype) {
    char* dbg =  "MemSetDbg";
    if(ftype == FT_INVALID || offset > 0x15C)
        return;
    bool anyID = (ID == -1 ? true : false);
    NPCMOB* thisnpc;

    for(int i = 0; i < GM_NPCS_COUNT; i++) {
        thisnpc = Get(i);
        if(anyID || thisnpc->id == ID) {
            void* ptr = ((&(*(byte*)thisnpc)) + offset);
            MemAssign((int)ptr, value, operation, ftype);
        }
    }//for
}

// ALL SET HITS
void NPC::AllSetHits(int identity, int section, float hits) {
    bool anyID = (identity == -1 ? true : false);
    bool anySec = (section == -1 ? true : false);
    NPCMOB* thisnpc;

    for(int i = 0; i < GM_NPCS_COUNT; i++) {
        thisnpc = Get(i);
        if (anyID || thisnpc->id == identity) {
            if(anySec || GetSection(thisnpc) == section) {
                SetHits(thisnpc, hits);
            }
        }
    }	
}

// ALL FACE
void NPC::AllFace(int identity, int section, double x) {
    bool anyID = (identity == -1 ? true : false);
    bool anySec = (section == -1 ? true : false);
    NPCMOB* thisnpc;
    for(int i = 0; i < GM_NPCS_COUNT; i++) {
        thisnpc = Get(i);
        if (anyID || thisnpc->id == identity) {
            if(anySec || GetSection(thisnpc) == section) {
                if(x < thisnpc->momentum.x) {
                    FaceDirection(thisnpc, -1);
                } else {
                    FaceDirection(thisnpc, 1);
                }
            }
        }
    }	
}

// Declerations of inbuilt NPC property arrays
static uint32_t npcprop_vulnerableharmtypes[NPC::MAX_ID + 1] = { 0 };
static int16_t npcprop_spinjumpsafe[NPC::MAX_ID+1] = { 0 };
static int16_t npcprop_nowaterphysics[NPC::MAX_ID + 1] = { 0 };
static int16_t npcprop_harmlessgrab[NPC::MAX_ID + 1] = { 0 };
static int16_t npcprop_harmlessthrown[NPC::MAX_ID + 1] = { 0 };
static int16_t npcprop_ignorethrownnpcs[NPC::MAX_ID + 1] = { 0 };
static int16_t npcprop_linkshieldable[NPC::MAX_ID + 1] = { 0 };
static int16_t npcprop_noshieldfireeffect[NPC::MAX_ID + 1] = { 0 };
static int16_t npcprop_notcointransformable[NPC::MAX_ID + 1] = { 0 };

// Other NPC-related config data, not by ID
static std::unordered_map<unsigned int, bool> npc_semisolidCollidingFlyTypeMap = { { 1, true } };

// Initialization of inbuilt NPC property arrays
void NPC::InitProperties() {
    for (int i = 1; i <= NPC::MAX_ID; i++)
    {
        npcprop_vulnerableharmtypes[i] = 0;
        npcprop_spinjumpsafe[i] = 0;
        npcprop_nowaterphysics[i] = 0;
        npcprop_harmlessgrab[i] = 0;
        npcprop_harmlessthrown[i] = 0;
        npcprop_ignorethrownnpcs[i] = 0;
        npcprop_linkshieldable[i] = 0;
        npcprop_noshieldfireeffect[i] = 0;
        npcprop_notcointransformable[i] = 0;
    }

    // Set built-in spinjump safe IDs
    npcprop_spinjumpsafe[0xB3]  = -1;
    npcprop_spinjumpsafe[0x25]  = -1;
    npcprop_spinjumpsafe[0xB4]  = -1;
    npcprop_spinjumpsafe[0x26]  = -1;
    npcprop_spinjumpsafe[0x2A]  = -1;
    npcprop_spinjumpsafe[0x2B]  = -1;
    npcprop_spinjumpsafe[0x2C]  = -1;
    npcprop_spinjumpsafe[0x8]   = -1;
    npcprop_spinjumpsafe[0xC]   = -1;
    npcprop_spinjumpsafe[0x24]  = -1;
    npcprop_spinjumpsafe[0x33]  = -1;
    npcprop_spinjumpsafe[0x34]  = -1;
    npcprop_spinjumpsafe[0x35]  = -1;
    npcprop_spinjumpsafe[0x36]  = -1;
    npcprop_spinjumpsafe[0x4A]  = -1;
    npcprop_spinjumpsafe[0x5D]  = -1;
    npcprop_spinjumpsafe[0xC8]  = -1;
    npcprop_spinjumpsafe[0xCD]  = -1;
    npcprop_spinjumpsafe[0xCF]  = -1;
    npcprop_spinjumpsafe[0xC9]  = -1;
    npcprop_spinjumpsafe[0xC7]  = -1;
    npcprop_spinjumpsafe[0xF5]  = -1;
    npcprop_spinjumpsafe[0x100] = -1;
    npcprop_spinjumpsafe[0x105] = -1;
    npcprop_spinjumpsafe[0x113] = -1;
    npcprop_spinjumpsafe[0x11D] = -1;
    npcprop_spinjumpsafe[0x11E] = -1;
    npcprop_spinjumpsafe[0x10E] = -1;

    // Set built-in harmless grab IDs
    npcprop_harmlessgrab[0xC3] = -1;
    npcprop_harmlessgrab[0x16] = -1;
    npcprop_harmlessgrab[0x1A] = -1;
    npcprop_harmlessgrab[0x20] = -1;
    npcprop_harmlessgrab[0xEE] = -1;
    npcprop_harmlessgrab[0xEF] = -1;
    npcprop_harmlessgrab[0xC1] = -1;
    npcprop_harmlessgrab[0x23] = -1;
    npcprop_harmlessgrab[0xBF] = -1;
    npcprop_harmlessgrab[0x31] = -1;
    npcprop_harmlessgrab[0x86] = -1;
    npcprop_harmlessgrab[0x9A] = -1;
    npcprop_harmlessgrab[0x9B] = -1;
    npcprop_harmlessgrab[0x9C] = -1;
    npcprop_harmlessgrab[0x9D] = -1;
    npcprop_harmlessgrab[0x1F] = -1;
    npcprop_harmlessgrab[0xF0] = -1;
    npcprop_harmlessgrab[0x116] = -1;
    npcprop_harmlessgrab[0x117] = -1;
    npcprop_harmlessgrab[0x124] = -1;

    // Set built-in link shieldable NPCs
    // Player projectiles aren't here since they're only shieldable in battle mode
    npcprop_linkshieldable[0x57] = -1;
    npcprop_linkshieldable[0x114] = -1;
    npcprop_linkshieldable[0x55] = -1;
    npcprop_linkshieldable[0x85] = -1;
    npcprop_linkshieldable[0xF6] = -1;
    npcprop_linkshieldable[0x1E] = -1;
    npcprop_linkshieldable[0xCA] = -1;
    npcprop_linkshieldable[0xD2] = -1;
    // And which shouldn't create fire effects
    npcprop_noshieldfireeffect[0xCA] = -1;
    npcprop_noshieldfireeffect[0xD2] = -1;
    npcprop_noshieldfireeffect[0xAB] = -1;
    npcprop_noshieldfireeffect[0xD] = -1;
    npcprop_noshieldfireeffect[0x109] = -1;
    npcprop_noshieldfireeffect[0x1E] = -1;

    // Directly default non-transformable
    npcprop_notcointransformable[13] = -1;
    npcprop_notcointransformable[108] = -1;
    npcprop_notcointransformable[26] = -1;
    npcprop_notcointransformable[91] = -1;
    npcprop_notcointransformable[171] = -1;
    npcprop_notcointransformable[56] = -1;
    npcprop_notcointransformable[60] = -1;
    npcprop_notcointransformable[62] = -1;
    npcprop_notcointransformable[64] = -1;
    npcprop_notcointransformable[66] = -1;
    npcprop_notcointransformable[104] = -1;
    npcprop_notcointransformable[30] = -1;
    npcprop_notcointransformable[17] = -1;
    npcprop_notcointransformable[291] = -1;
    npcprop_notcointransformable[292] = -1;
    npcprop_notcointransformable[266] = -1;
    npcprop_notcointransformable[57] = -1;
    npcprop_notcointransformable[58] = -1;
    npcprop_notcointransformable[78] = -1;
    npcprop_notcointransformable[79] = -1;
    npcprop_notcointransformable[80] = -1;
    npcprop_notcointransformable[81] = -1;
    npcprop_notcointransformable[82] = -1;
    npcprop_notcointransformable[83] = -1;
    npcprop_notcointransformable[91] = -1;
    npcprop_notcointransformable[260] = -1;
    npcprop_notcointransformable[259] = -1;
    // implied by default isInteractableNPC_ptr
    npcprop_notcointransformable[9] = -1;
    npcprop_notcointransformable[10] = -1;
    npcprop_notcointransformable[11] = -1;
    npcprop_notcointransformable[14] = -1;
    npcprop_notcointransformable[16] = -1;
    npcprop_notcointransformable[33] = -1;
    npcprop_notcointransformable[34] = -1;
    npcprop_notcointransformable[41] = -1;
    npcprop_notcointransformable[75] = -1;
    npcprop_notcointransformable[88] = -1;
    npcprop_notcointransformable[90] = -1;
    npcprop_notcointransformable[94] = -1;
    npcprop_notcointransformable[97] = -1;
    npcprop_notcointransformable[101] = -1;
    npcprop_notcointransformable[102] = -1;
    npcprop_notcointransformable[103] = -1;
    npcprop_notcointransformable[107] = -1;
    npcprop_notcointransformable[138] = -1;
    npcprop_notcointransformable[152] = -1;
    npcprop_notcointransformable[153] = -1;
    npcprop_notcointransformable[169] = -1;
    npcprop_notcointransformable[170] = -1;
    npcprop_notcointransformable[178] = -1;
    npcprop_notcointransformable[182] = -1;
    npcprop_notcointransformable[183] = -1;
    npcprop_notcointransformable[184] = -1;
    npcprop_notcointransformable[185] = -1;
    npcprop_notcointransformable[186] = -1;
    npcprop_notcointransformable[187] = -1;
    npcprop_notcointransformable[188] = -1;
    npcprop_notcointransformable[192] = -1;
    npcprop_notcointransformable[196] = -1;
    npcprop_notcointransformable[197] = -1;
    npcprop_notcointransformable[198] = -1;
    npcprop_notcointransformable[240] = -1;
    npcprop_notcointransformable[248] = -1;
    npcprop_notcointransformable[249] = -1;
    npcprop_notcointransformable[250] = -1;
    npcprop_notcointransformable[251] = -1;
    npcprop_notcointransformable[252] = -1;
    npcprop_notcointransformable[253] = -1;
    npcprop_notcointransformable[254] = -1;
    npcprop_notcointransformable[258] = -1;
    npcprop_notcointransformable[264] = -1;
    npcprop_notcointransformable[273] = -1;
    npcprop_notcointransformable[274] = -1;
    npcprop_notcointransformable[277] = -1;
    // implied by default isYoshi
    npcprop_notcointransformable[95] = -1;
    npcprop_notcointransformable[98] = -1;
    npcprop_notcointransformable[99] = -1;
    npcprop_notcointransformable[100] = -1;
    npcprop_notcointransformable[148] = -1;
    npcprop_notcointransformable[149] = -1;
    npcprop_notcointransformable[150] = -1;
    npcprop_notcointransformable[228] = -1;
    // implied by default isShoe
    npcprop_notcointransformable[35] = -1;
    npcprop_notcointransformable[191] = -1;
    npcprop_notcointransformable[193] = -1;
    // implied by default isVegetable
    npcprop_notcointransformable[92] = -1;
    npcprop_notcointransformable[139] = -1;
    npcprop_notcointransformable[140] = -1;
    npcprop_notcointransformable[141] = -1;
    npcprop_notcointransformable[142] = -1;
    npcprop_notcointransformable[143] = -1;
    npcprop_notcointransformable[144] = -1;
    npcprop_notcointransformable[145] = -1;
    npcprop_notcointransformable[146] = -1;
    npcprop_notcointransformable[147] = -1;
    // implied by default IsVine
    npcprop_notcointransformable[213] = -1;
    npcprop_notcointransformable[214] = -1;
    npcprop_notcointransformable[215] = -1;
    npcprop_notcointransformable[216] = -1;
    npcprop_notcointransformable[217] = -1;
    npcprop_notcointransformable[218] = -1;
    npcprop_notcointransformable[219] = -1;
    npcprop_notcointransformable[220] = -1;
    npcprop_notcointransformable[221] = -1;
    npcprop_notcointransformable[222] = -1;
    npcprop_notcointransformable[223] = -1;
    npcprop_notcointransformable[224] = -1;

    npc_semisolidCollidingFlyTypeMap.clear();
    npc_semisolidCollidingFlyTypeMap[1] = true;
}

// Internal C++ getters for inbuilt NPC property arrays
uint32_t NPC::GetVulnerableHarmTypes(int id) {
    if ((id < 1) || (id > NPC::MAX_ID)) return 0;
    return npcprop_vulnerableharmtypes[id];
}

bool NPC::GetSpinjumpSafe(int id) {
    if ((id < 1) || (id > NPC::MAX_ID)) return false;
    return (npcprop_spinjumpsafe[id] != 0);
}

bool NPC::GetNoWaterPhysics(int id) {
    if ((id < 1) || (id > NPC::MAX_ID)) return false;
    return (npcprop_nowaterphysics[id] != 0);
}

bool NPC::GetHarmlessGrab(int id) {
    if ((id < 1) || (id > NPC::MAX_ID)) return false;
    return (npcprop_harmlessgrab[id] != 0);
}

bool NPC::GetHarmlessThrown(int id) {
    if ((id < 1) || (id > NPC::MAX_ID)) return false;
    return (npcprop_harmlessthrown[id] != 0);
}

bool NPC::GetIgnoreThrownNPCs(int id) {
    if ((id < 1) || (id > NPC::MAX_ID)) return false;
    return (npcprop_ignorethrownnpcs[id] != 0);
}

bool NPC::GetLinkShieldable(int id) {
    if ((id < 1) || (id > NPC::MAX_ID)) return false;
    return (npcprop_linkshieldable[id] != 0);
}

bool NPC::GetNoShieldFireEffect(int id) {
    if ((id < 1) || (id > NPC::MAX_ID)) return false;
    return (npcprop_noshieldfireeffect[id] != 0);
}

bool NPC::GetNotCoinTransformable(int id) {
    if ((id < 1) || (id > NPC::MAX_ID)) return false;
    return (npcprop_notcointransformable[id] != 0);
}

// Getter for address of NPC property arrays
uintptr_t NPC::GetPropertyTableAddress(const std::string& s)
{
    if (s == "vulnerableharmtypes")
    {
        return reinterpret_cast<uintptr_t>(npcprop_vulnerableharmtypes);
    }
    else if (s == "spinjumpsafe")
    {
        return reinterpret_cast<uintptr_t>(npcprop_spinjumpsafe);
    }
    else if (s == "nowaterphysics")
    {
        return reinterpret_cast<uintptr_t>(npcprop_nowaterphysics);
    }
    else if (s == "harmlessgrab")
    {
        return reinterpret_cast<uintptr_t>(npcprop_harmlessgrab);
    }
    else if (s == "harmlessthrown")
    {
        return reinterpret_cast<uintptr_t>(npcprop_harmlessthrown);
    }
    else if (s == "ignorethrownnpcs")
    {
        return reinterpret_cast<uintptr_t>(npcprop_ignorethrownnpcs);
    }
    else if (s == "linkshieldable")
    {
        return reinterpret_cast<uintptr_t>(npcprop_linkshieldable);
    }
    else if (s == "noshieldfireeffect")
    {
        return reinterpret_cast<uintptr_t>(npcprop_noshieldfireeffect);
    }
    else if (s == "notcointransformable")
    {
        return reinterpret_cast<uintptr_t>(npcprop_notcointransformable);
    }
    else
    {
        return 0;
    }
}

bool NPC::CheckSemisolidCollidingFlyType(unsigned int flyType)
{
    auto it = npc_semisolidCollidingFlyTypeMap.find(flyType);
    if (it != npc_semisolidCollidingFlyTypeMap.end())
    {
        return it->second;
    }
    return false;
}

void NPC::SetSemisolidCollidingFlyType(unsigned int flyType, bool shouldCollide)
{
    npc_semisolidCollidingFlyTypeMap[flyType] = shouldCollide;
}
