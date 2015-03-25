#include "../LuaProxy.h"
#include "../LuaHelper.h"
#include "../../SMBXInternal/NPCs.h"
#include "../../Misc/MiscFuncs.h"
#include "../../GlobalFuncs.h"
#include "../../Misc/VB6StrPtr.h"
#include <vector>
#include "../../Misc/RuntimeHook.h"

int LuaProxy::NPC::count()
{
    return (int)GM_NPCS_COUNT;
}

luabind::object LuaProxy::NPC::get(lua_State* L)
{
    return get(luabind::object(L, -1), luabind::object(L, -1), L);
}

luabind::object LuaProxy::NPC::get(luabind::object idFilter, luabind::object sectionFilter, lua_State* L)
{
    std::unique_ptr<bool> lookupTableNpcID;
    std::unique_ptr<bool> lookupTableSectionID;

    try
    {
        lookupTableNpcID = std::unique_ptr<bool>(LuaHelper::generateFilterTable(L, idFilter, 292));
    }
    catch (LuaHelper::invalidIDException* e)
    {
        luaL_error(L, "Invalid NPC-ID!\nNeed NPC-ID between 1-292\nGot NPC-ID: %d", e->usedID());
        return luabind::object();
    }
    catch (LuaHelper::invalidTypeException* /*e*/)
    {
        luaL_error(L, "Invalid args for npcID (arg #1, expected table or number, got %s)", lua_typename(L, luabind::type(idFilter)));
        return luabind::object();
    }

    try
    {
        lookupTableSectionID = std::unique_ptr<bool>(LuaHelper::generateFilterTable(L, sectionFilter, 20, 0));
    }
    catch (LuaHelper::invalidIDException* e)
    {
        luaL_error(L, "Invalid Section!\nNeed Section-Index between 0-20\nGot Section-Index: %d", e->usedID());
        return luabind::object();
    }
    catch (LuaHelper::invalidTypeException* /*e*/)
    {
        luaL_error(L, "Invalid args for section number (arg #2, expected table or number, got %s)", lua_typename(L, luabind::type(sectionFilter)));
        return luabind::object();
    }

    luabind::object retNPCs = luabind::newtable(L);
    int npcIndex = 1;
    
    for (int i = 0; i < GM_NPCS_COUNT; i++) {
        NPCMOB* thisnpc = ::NPC::Get(i);
        if (lookupTableNpcID.get()[thisnpc->id]) {
            if (lookupTableSectionID.get()[::NPC::GetSection(thisnpc)]) {
                retNPCs[npcIndex++] = LuaProxy::NPC(i);
            }
        }
    }
    
    return retNPCs;
}

LuaProxy::NPC LuaProxy::NPC::spawn(short npcid, double x, double y, short section, lua_State* L)
{
    return LuaProxy::NPC::spawn(npcid, x, y, section, false, false, L);
}

LuaProxy::NPC LuaProxy::NPC::spawn(short npcid, double x, double y, short section, bool respawn, lua_State* L)
{
    return LuaProxy::NPC::spawn(npcid, x, y, section, respawn, false, L);
}

LuaProxy::NPC LuaProxy::NPC::spawn(short npcid, double x, double y, short section, bool respawn, bool centered, lua_State* L)
{

    if (npcid < 1 || npcid > 292){
        luaL_error(L, "Invalid NPC-ID!\nNeed NPC-ID between 1-292\nGot NPC-ID: %d", npcid);
        return LuaProxy::NPC(-1);
    }

    if (section < 0 || section > 20){
        luaL_error(L, "Invalid Section!\nNeed Section-Index between 0-20\nGot Section-Index: %d", section);
        return LuaProxy::NPC(-1);
    }

    if (GM_NPCS_COUNT >= 5000){
        luaL_error(L, "Over 5000 NPCs, cannot spawn more!");
        return LuaProxy::NPC(-1);
    }

    LuaProxy::NPC theNewNPC(GM_NPCS_COUNT);
    void* nativeAddr = theNewNPC.getNativeAddr();


    memset(nativeAddr, 0, 0x158);
    WORD* widthArray = (WORD*)GM_CONF_WIDTH;
    WORD* heightArray = (WORD*)GM_CONF_HEIGHT;
    WORD* gfxWidthArray = (WORD*)GM_CONF_GFXWIDTH;
    WORD* gfxHeightArray = (WORD*)GM_CONF_GFXHEIGHT;

    short width = widthArray[npcid];
    short height = heightArray[npcid];
    short gfxWidth = gfxWidthArray[npcid];
    short gfxHeight = gfxHeightArray[npcid];

    gfxWidth = (gfxWidth ? gfxWidth : width);
    gfxHeight = (gfxHeight ? gfxHeight : height);

    if (centered) {
        x -= 0.5 * (double)width;
        y -= 0.5 * (double)height;
    }

    PATCH_OFFSET(nativeAddr, 0x78, double, x);
    PATCH_OFFSET(nativeAddr, 0x80, double, y);
    PATCH_OFFSET(nativeAddr, 0x88, double, height);
    PATCH_OFFSET(nativeAddr, 0x90, double, width);
    PATCH_OFFSET(nativeAddr, 0x98, double, 0.0);
    PATCH_OFFSET(nativeAddr, 0xA0, double, 0.0);

    PATCH_OFFSET(nativeAddr, 0xA8, double, x);
    PATCH_OFFSET(nativeAddr, 0xB0, double, y);
    PATCH_OFFSET(nativeAddr, 0xB8, double, gfxHeight);
    PATCH_OFFSET(nativeAddr, 0xC0, double, gfxWidth);

    if (respawn) {
        PATCH_OFFSET(nativeAddr, 0xDC, WORD, npcid);
    }
    PATCH_OFFSET(nativeAddr, 0xE2, WORD, npcid);

    PATCH_OFFSET(nativeAddr, 0x12A, WORD, 180);
    PATCH_OFFSET(nativeAddr, 0x124, WORD, -1);
    PATCH_OFFSET(nativeAddr, 0x146, WORD, section);

    ++(GM_NPCS_COUNT);

    return theNewNPC;
}


LuaProxy::NPC::NPC(int index)
{
	m_index = index;
}

int LuaProxy::NPC::id(lua_State* L) const
{
	if(!isValid_throw(L))
		return 0;
	return (int)::NPC::Get(m_index)->id;
}

float LuaProxy::NPC::direction(lua_State* L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::NPC::Get(m_index)->directionFaced;
}

void LuaProxy::NPC::setDirection(float direction, lua_State* L)
{
	if(!isValid_throw(L))
		return;

	NPCMOB* npc =  ::NPC::Get(m_index);
	setSpeedX(0.0, L);
    npc->directionFaced = direction;
}

double LuaProxy::NPC::x(lua_State* L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::NPC::Get(m_index)->momentum.x;
}

void LuaProxy::NPC::setX(double x, lua_State* L)
{
	if(!isValid_throw(L))
		return;
	::NPC::Get(m_index)->momentum.x = x;
}

double LuaProxy::NPC::y(lua_State* L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::NPC::Get(m_index)->momentum.y;
}

void LuaProxy::NPC::setY(double y, lua_State* L)
{
	if(!isValid_throw(L))
		return;
	::NPC::Get(m_index)->momentum.y = y;
}

double LuaProxy::NPC::speedX(lua_State* L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::NPC::Get(m_index)->momentum.speedX;
}

void LuaProxy::NPC::setSpeedX(double speedX, lua_State* L)
{
	if(!isValid_throw(L))
		return;
	::NPC::Get(m_index)->momentum.speedX = speedX;
}

double LuaProxy::NPC::speedY(lua_State* L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::NPC::Get(m_index)->momentum.speedY;
}

void LuaProxy::NPC::setSpeedY(double speedY, lua_State* L)
{
	if(!isValid_throw(L))
		return;
	::NPC::Get(m_index)->momentum.speedY = speedY;
}

void LuaProxy::NPC::mem(int offset, LuaProxy::L_FIELDTYPE ftype, const luabind::object &value, lua_State* L)
{
	if(!isValid_throw(L))
		return;
	
	NPCMOB* mnpc = ::NPC::Get(m_index);
	void* ptr = ((&(*(byte*)mnpc)) + offset);
	LuaProxy::mem((int)ptr, ftype, value, L);
}

void LuaProxy::NPC::kill(lua_State* L)
{
	if(!isValid_throw(L))
		return;
	::NPC::Get(m_index)->killFlag = 1;
}

void LuaProxy::NPC::kill(int killEffectID, lua_State* L)
{
	if(!isValid_throw(L))
		return;
	::NPC::Get(m_index)->killFlag = killEffectID;
}

luabind::object LuaProxy::NPC::mem(int offset, LuaProxy::L_FIELDTYPE ftype, lua_State* L) const
{
	if(!isValid_throw(L))
		return luabind::object();

	NPCMOB* mnpc = ::NPC::Get(m_index);
	void* ptr = ((&(*(byte*)mnpc)) + offset);
	return LuaProxy::mem((int)ptr, ftype, L);
}

LuaProxy::VBStr LuaProxy::NPC::attachedLayerName(lua_State* L) const
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	return VBStr(thisnpc->attachedLayerName.ptr);
}

void LuaProxy::NPC::setAttachedLayerName(const luabind::object &value, lua_State* L)
{
	if (!isValid_throw(L)) return;

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	LuaHelper::assignVB6StrPtr(&thisnpc->attachedLayerName, value, L);
}

luabind::object LuaProxy::NPC::attachedLayerObj(lua_State *L) const
{
	if(!isValid_throw(L))
		return luabind::object();

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	return findlayer(((std::string)thisnpc->attachedLayerName).c_str(), L);
}

void LuaProxy::NPC::setAttachedLayerObj(const LuaProxy::Layer &value, lua_State *L)
{
	if (!isValid_throw(L)) return;

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	thisnpc->attachedLayerName = ::Layer::Get(value.layerIndex())->ptLayerName;
}

LuaProxy::VBStr LuaProxy::NPC::activateEventName(lua_State* L) const
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	return VBStr(thisnpc->activateEventLayerName.ptr);
}

void LuaProxy::NPC::setActivateEventName(const luabind::object &value, lua_State* L)
{
	if (!isValid_throw(L)) return;

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	LuaHelper::assignVB6StrPtr(&thisnpc->activateEventLayerName, value, L);
}

LuaProxy::VBStr LuaProxy::NPC::deathEventName(lua_State* L) const
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	return VBStr(thisnpc->deathEventName.ptr);
}

void LuaProxy::NPC::setDeathEventName(const luabind::object &value, lua_State* L)
{
	if (!isValid_throw(L)) return;

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	LuaHelper::assignVB6StrPtr(&thisnpc->deathEventName, value, L);
}

LuaProxy::VBStr LuaProxy::NPC::talkEventName(lua_State* L) const
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	return VBStr(thisnpc->talkEventName.ptr);
}

void LuaProxy::NPC::setTalkEventName(const luabind::object &value, lua_State* L)
{
	if (!isValid_throw(L)) return;

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	LuaHelper::assignVB6StrPtr(&thisnpc->talkEventName, value, L);
}

LuaProxy::VBStr LuaProxy::NPC::noMoreObjInLayer(lua_State* L) const
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	return VBStr(thisnpc->noMoreObjInLayerEventName.ptr);
}

void LuaProxy::NPC::setNoMoreObjInLayer(const luabind::object &value, lua_State* L)
{
	if (!isValid_throw(L)) return;

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	LuaHelper::assignVB6StrPtr(&thisnpc->noMoreObjInLayerEventName, value, L);
}

LuaProxy::VBStr LuaProxy::NPC::msg(lua_State* L) const
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	return VBStr(thisnpc->talkMsg.ptr);
}

void LuaProxy::NPC::setMsg(const luabind::object &value, lua_State* L)
{
	if (!isValid_throw(L)) return;

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	LuaHelper::assignVB6StrPtr(&thisnpc->talkMsg, value, L);
}

LuaProxy::VBStr LuaProxy::NPC::layerName(lua_State* L) const
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	return VBStr(thisnpc->layerName.ptr);
}

void LuaProxy::NPC::setLayerName(const luabind::object &value, lua_State* L)
{
	if (!isValid_throw(L)) return;

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	LuaHelper::assignVB6StrPtr(&thisnpc->layerName, value, L);
}

luabind::object LuaProxy::NPC::layerObj(lua_State *L) const
{
	if(!isValid_throw(L))
		return luabind::object();

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	return findlayer(((std::string)thisnpc->layerName).c_str(), L);
}

void LuaProxy::NPC::setLayerObj(const LuaProxy::Layer &value, lua_State *L)
{
	if (!isValid_throw(L)) return;

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	thisnpc->layerName = ::Layer::Get(value.layerIndex())->ptLayerName;
}

bool LuaProxy::NPC::isValid() const
{
	return !(m_index < 0 || m_index > GM_NPCS_COUNT-1);
}


bool LuaProxy::NPC::isValid_throw(lua_State *L) const
{
	if(!isValid()){
		luaL_error(L, "Invalid NPC-Pointer");
		return false;
	}
	return true;
}

void* LuaProxy::NPC::getNativeAddr() const
{
	return (void*)::NPC::Get(m_index);
}
