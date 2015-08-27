#include "../LuaProxy.h"
#include "../../SMBXInternal/CustomGraphics.h"

LuaProxy::PlayerSettings LuaProxy::PlayerSettings::get(Characters character, PowerupID powerupID, lua_State* L)
{
    PlayerSettings plSettings = PlayerSettings(character, powerupID);
    plSettings.isValid_throw(L);
    return plSettings;
}

int LuaProxy::PlayerSettings::getHitboxWidth(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    return SMBX_CustomGraphics::getPlayerHitboxWidth(m_powerupID, m_character);
}

void LuaProxy::PlayerSettings::setHitboxWidth(int width, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBX_CustomGraphics::setPlayerHitboxWidth(m_powerupID, m_character, width);
}

int LuaProxy::PlayerSettings::getHitboxHeight(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    return SMBX_CustomGraphics::getPlayerHitboxHeight(m_powerupID, m_character);
}

void LuaProxy::PlayerSettings::setHitboxHeight(int height, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBX_CustomGraphics::setPlayerHitboxHeight(m_powerupID, m_character, height);
}

int LuaProxy::PlayerSettings::getHitboxDuckHeight(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    return SMBX_CustomGraphics::getPlayerHitboxDuckHeight(m_powerupID, m_character);
}

void LuaProxy::PlayerSettings::setHitboxDuckHeight(int duckHeight, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBX_CustomGraphics::setPlayerHitboxDuckHeight(m_powerupID, m_character, duckHeight);
}

int LuaProxy::PlayerSettings::getGrabOffsetX(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    return SMBX_CustomGraphics::getPlayerGrabOffsetX(m_powerupID, m_character);
}

void LuaProxy::PlayerSettings::setGrabOffsetX(int grabOffsetX, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBX_CustomGraphics::setPlayerGrabOffsetX(m_powerupID, m_character, grabOffsetX);
}

int LuaProxy::PlayerSettings::getGrabOffsetY(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    return SMBX_CustomGraphics::getPlayerGrabOffsetY(m_powerupID, m_character);
}

void LuaProxy::PlayerSettings::setGrabOffsetY(int grabOffsetY, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBX_CustomGraphics::setPlayerGrabOffsetY(m_powerupID, m_character, grabOffsetY);
}

int LuaProxy::PlayerSettings::getSpriteOffset(int indexX, int indexY, lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    return 0;
}

void LuaProxy::PlayerSettings::setSpriteOffset(int indexX, int indexY, int value, lua_State* L)
{
    if (!isValid_throw(L))
        return;
}

PowerupID LuaProxy::PlayerSettings::getPowerupID(lua_State* L) const
{
    if (!isValid_throw(L))
        return PLAYER_SMALL;
    return m_powerupID;
}

void LuaProxy::PlayerSettings::setPowerupID(PowerupID val, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    m_powerupID = val;
    isValid_throw(L);
}

Characters LuaProxy::PlayerSettings::getCharacter(lua_State* L) const
{
    if (!isValid_throw(L))
        return CHARACTER_MARIO;
    return m_character;
}

void LuaProxy::PlayerSettings::setCharacter(Characters val, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    m_character = val;
    isValid_throw(L);
}


bool LuaProxy::PlayerSettings::isValid_throw(lua_State* L) const
{
    if ((int)m_character < CHARACTER_MARIO || (int)m_character > CHARACTER_LINK){
        luaL_error(L, "Invalid character id %d!", m_character);
        return false;
    }
    if ((int)m_powerupID < PLAYER_SMALL || (int)m_powerupID > PLAYER_ICE){
        luaL_error(L, "Invalid powerup id %d!", m_powerupID);
        return false;
    }
    return true;
}
