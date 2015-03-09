#include "LunaLuaMain.h"
#include "../GlobalFuncs.h"
#include "../Level/Level.h"
#include "../Misc/MiscFuncs.h"
#include "../MOBs/PlayerMOB.h"
#include "../MOBs/NPCs.h"

#include "LuaHelper.h"
#include "LuaProxy.h"
#include "LuaEvents.h"
#include <string>

const std::wstring CLunaLua::LuaLibsPath = L"\\LuaScriptsLib\\mainV2.lua";
using namespace luabind;


std::wstring CLunaLua::getLuaLibsPath()
{
	wstring lapi = getModulePath();
	lapi = lapi.append(L"\\LuaScriptsLib\\mainV2.lua");
	return lapi;
}


CLunaLua::CLunaLua() :
		m_type(CLunaLua::LUNALUA_LEVEL),
		m_luaEventTableName(""),
		L(0)
{}

CLunaLua::~CLunaLua()
{
	//Just to be safe
	shutdown();
}

bool CLunaLua::shutdown()
{
	//Shutdown the lua module if possible
	if(!isValid())
		return false;
		
	lua_close(L);
	L = NULL;
	return true;
}

void CLunaLua::init(LuaLunaType type, std::wstring codePath, std::wstring levelPath /*= std::wstring()*/)
{

	//Just to be safe
	shutdown();
	//Open up a new Lua State
	L = luaL_newstate();
	//Open all luabind functions
	luabind::open(L);
	//Set the new type
	m_type = type;

	//Open up "safe" standard lua libraries
	lua_pushcfunction(L, luaopen_base);
	lua_call(L,0,0);
	lua_pushcfunction(L, luaopen_math);
	lua_call(L,0,0);
	lua_pushcfunction(L, luaopen_string);
	lua_call(L,0,0);
	lua_pushcfunction(L, luaopen_table);
	lua_call(L,0,0);
	lua_pushcfunction(L, luaopen_debug);
	lua_call(L,0,0);
	lua_pushcfunction(L, luaopen_os);
	lua_call(L,0,0);
	lua_pushcfunction(L, luaopen_package);
	lua_call(L,0,0);

	//Remove unsafe apis
	{
		object _G = globals(L);
		object osTable = _G["os"];
		osTable["execute"] = object();
		osTable["exit"] = object();
		osTable["getenv"] = object();
		osTable["remove"] = object();
		osTable["rename"] = object();
		osTable["setlocal"] = object();
		osTable["tmpname"] = object();
	}

	//Read the API-File
	std::wstring wLuaCode;
	if(!readFile(wLuaCode, getLuaLibsPath(), L"Since v0.3 the LuaScriptsLib-Folder with\nall its content is required.\nBe sure you installed everything correctly!")){
		shutdown();
	}
	//Convert to ASCII, as lua doesn't support unicode
	std::string LuaCode = utf8_encode(wLuaCode);

	//Bind all functions, propeties ect...
	bindAll();

	//Setup default contants
	setupDefaults();

	//Reset event data
	LuaEvents::resetToDefaults();

	//Load the Lua API
	bool errLapi = false;
	int lapierrcode = luaL_loadbuffer(L, LuaCode.c_str(), LuaCode.length(), "mainV2.lua")  || lua_pcall(L, 0, LUA_MULTRET, 0);
	if(!(lapierrcode == 0)){
		object error_msg(from_stack(L, -1));
		LuaProxy::windowDebug(object_cast<const char*>(error_msg));
		errLapi = true;
	}

	//Shutdown if an error happend.
	if(errLapi){
		shutdown();
		return;
	}

	//Call the lua api init funtion.
	{
		object _G = globals(L);
		const char* initFunc = object_cast<const char*>(_G["__lapiInit"]);
		m_luaEventTableName = std::string(object_cast<const char*>(_G["__lapiEventMgr"]));


		try
		{
			if(LuaHelper::is_function(L, initFunc)){
				luabind::call_function<void>(L, initFunc, utf8_encode(codePath), utf8_encode(levelPath));
			}
		}
		catch (error& /*e*/)
		{
			errLapi = true;
		}
	}

	//Shutdown if an error happend.
	if(errLapi){
		shutdown();
		return;
	}
}

//Setup default constants
void CLunaLua::setupDefaults()
{
	object _G = globals(L);
	_G["PLAYER_SMALL"] = 1;
	_G["PLAYER_BIG"] = 2;
	_G["PLAYER_FIREFLOWER"] = 3;
	_G["PLAYER_LEAF"] = 4;
	_G["PLAYER_TANOOKIE"] = 5;
	_G["PLAYER_HAMMER"] = 6;
	_G["PLAYER_ICE"] = 7;

	_G["FIND_ANY"] = -1;

	_G["DIR_RIGHT"] = 1;
	_G["DIR_RANDOM"] = 0;
	_G["DIR_LEFT"] = -1;

	_G["FIELD_BYTE"] = 1;
	_G["FIELD_WORD"] = 2;
	_G["FIELD_DWORD"] = 3;
	_G["FIELD_FLOAT"] = 4;
	_G["FIELD_DFLOAT"] = 5;
	_G["FIELD_STRING"] = 6;

	_G["KEY_UP"] = GM_PLAYER_KEY_UP;
	_G["KEY_DOWN"] = GM_PLAYER_KEY_DOWN;
	_G["KEY_LEFT"] = GM_PLAYER_KEY_LEFT;
	_G["KEY_RIGHT"] = GM_PLAYER_KEY_RIGHT;
	_G["KEY_JUMP"] = GM_PLAYER_KEY_JUMP;
	_G["KEY_SPINJUMP"] = GM_PLAYER_KEY_SJUMP;
	_G["KEY_X"] = GM_PLAYER_KEY_X;
	_G["KEY_RUN"] = GM_PLAYER_KEY_RUN;
	_G["KEY_SEL"] = GM_PLAYER_KEY_SEL;
	_G["KEY_STR"] = GM_PLAYER_KEY_STR;

	_G["ODIR_UP"] = 1;
	_G["ODIR_LEFT"] = 2;
	_G["ODIR_DOWN"] = 3;
	_G["ODIR_RIGHT"] = 4;
	if(m_type == LUNALUA_WORLD){
		_G["isOverworld"] = true;
		_G["world"] = LuaProxy::World();
	}
	if(m_type == LUNALUA_LEVEL){
		_G["player"] = LuaProxy::Player();
		LuaProxy::Player pl(2);
		if(pl.isValid())
			_G["player2"] = pl;
	}
}

void CLunaLua::bindAll()
{
	//Bind stuff for world and level
	module(L)
		[
			def("getSMBXPath", &LuaProxy::getSMBXPath),
			def("windowDebug", &LuaProxy::windowDebug),
			def("printText", (void(*)(const char*, int, int)) &LuaProxy::print),
			def("printText", (void(*)(const char*, int, int, int)) &LuaProxy::print),
			def("loadImage", &LuaProxy::loadImage),
			def("placeSprite", (void(*)(int, int, int, int, const char*, int))&LuaProxy::placeSprite),
			def("placeSprite", (void(*)(int, int, int, int, const char*))&LuaProxy::placeSprite),
			def("placeSprite", (void(*)(int, int, int, int))&LuaProxy::placeSprite),
			def("mem", (void(*)(int, LuaProxy::L_FIELDTYPE, luabind::object)) &LuaProxy::mem),
			def("mem", (luabind::object(*)(int, LuaProxy::L_FIELDTYPE, lua_State*)) &LuaProxy::mem),

			def("playSFX", (void(*)(int))&LuaProxy::playSFX),
			def("playSFX", (void(*)(const char*))&LuaProxy::playSFX),
			def("playSFXSDL", (void(*)(const char*))&LuaProxy::playSFXSDL),
			def("clearSFXBuffer", (void(*)())&LuaProxy::clearSFXBuffer),
			def("MusicOpen", (void(*)(const char*))&LuaProxy::MusicOpen),
			def("MusicPlay", (void(*)())&LuaProxy::MusicPlay),
			def("MusicPlayFadeIn", (void(*)(int))&LuaProxy::MusicPlay),
			def("MusicStop", (void(*)())&LuaProxy::MusicStop),
			def("MusicStopFadeOut", (void(*)(int))&LuaProxy::MusicStopFadeOut),
			def("MusicVolume", (void(*)(int))&LuaProxy::MusicVolume),

			class_<RECT>("RECT")
			.def_readwrite("left", &RECT::left)
			.def_readwrite("top", &RECT::top)
			.def_readwrite("right", &RECT::right)
			.def_readwrite("bottom", &RECT::bottom),

			class_<LuaProxy::RECTd>("RECTd")
			.def_readwrite("left", &LuaProxy::RECTd::left)
			.def_readwrite("top", &LuaProxy::RECTd::top)
			.def_readwrite("right", &LuaProxy::RECTd::right)
			.def_readwrite("bottom", &LuaProxy::RECTd::bottom),

			class_<Event>("Event")
			.property("eventName", &Event::eventName)
			.property("cancellable", &Event::isCancellable)
			.property("cancelled", &Event::cancelled, &Event::setCancelled)
            .property("loopable", &Event::getLoopable, &Event::setLoopable)
            .property("directEventName", &Event::getDirectEventName, &Event::setDirectEventName),

			class_<LuaProxy::Data>("Data")
				.enum_("DataTypes")
				[
					value("DATA_LEVEL", LuaProxy::Data::DATA_LEVEL),
					value("DATA_WORLD", LuaProxy::Data::DATA_WORLD),
					value("DATA_GLOBAL", LuaProxy::Data::DATA_GLOBAL)
				]
			.def(constructor<LuaProxy::Data::DataType>())
			.def(constructor<LuaProxy::Data::DataType, std::string>())
			.def(constructor<LuaProxy::Data::DataType, bool>())
			.def(constructor<LuaProxy::Data::DataType, std::string, bool>())
			.def("set", &LuaProxy::Data::set)
			.def("get", static_cast<std::string(LuaProxy::Data::*)(std::string)>(&LuaProxy::Data::get))
			.def("get", static_cast<luabind::object(LuaProxy::Data::*)(lua_State*)>(&LuaProxy::Data::get))
			.def("save", static_cast<void(LuaProxy::Data::*)()>(&LuaProxy::Data::save))
			.def("save", static_cast<void(LuaProxy::Data::*)(std::string)>(&LuaProxy::Data::save))
			.property("dataType", &LuaProxy::Data::dataType, &LuaProxy::Data::setDataType)
			.property("sectionName", &LuaProxy::Data::sectionName, &LuaProxy::Data::setSectionName)
			.property("useSaveSlot", &LuaProxy::Data::useSaveSlot, &LuaProxy::Data::setUseSaveSlot),

			

			def("newRECT", &LuaProxy::newRECT),
			def("newRECTd", &LuaProxy::newRECTd),

			namespace_("UserData")[
				def("setValue", &LuaProxy::SaveBankProxy::setValue),
					def("getValue", &LuaProxy::SaveBankProxy::getValue),
					def("isValueSet", &LuaProxy::SaveBankProxy::isValueSet),
					def("values", &LuaProxy::SaveBankProxy::values),
					def("save", &LuaProxy::SaveBankProxy::save)
			],

			class_<LuaProxy::VBStr>("VBStr")
			.def(constructor<long>())
			.property("str", &LuaProxy::VBStr::str, &LuaProxy::VBStr::setStr)
			.property("length", &LuaProxy::VBStr::length, &LuaProxy::VBStr::setLength)
			.def("clear", &LuaProxy::VBStr::clear)
			.def(tostring(self))
		];
	if(m_type == LUNALUA_WORLD){
		module(L)
			[
				def("levels", &LuaProxy::levels),
				def("findlevels", &LuaProxy::findlevels),
				def("findlevel", &LuaProxy::findlevel),

				class_<LuaProxy::World>("World")
				.property("playerX", &LuaProxy::World::playerX, &LuaProxy::World::setPlayerX)
				.property("playerY", &LuaProxy::World::playerY, &LuaProxy::World::setPlayerY)
				.property("playerWalkingDirection", &LuaProxy::World::currentWalkingDirection, &LuaProxy::World::setCurrentWalkingDirection)
				.property("playerWalkingTimer", &LuaProxy::World::currentWalkingTimer, &LuaProxy::World::setCurrentWalkingTimer)
				.property("playerWalkingFrame", &LuaProxy::World::currentWalkingFrame, &LuaProxy::World::setCurrentWalkingFrame)	
				.property("playerWalkingFrameTimer", &LuaProxy::World::currentWalkingFrameTimer, &LuaProxy::World::setCurrentWalkingFrameTimer)
				.property("playerIsCurrentWalking", &LuaProxy::World::playerIsCurrentWalking)
				.property("levelTitle", &LuaProxy::World::levelTitle)
				.property("playerCurrentDirection", &LuaProxy::World::getCurrentDirection)
				.def("mem", static_cast<void (LuaProxy::World::*)(int, LuaProxy::L_FIELDTYPE, luabind::object)>(&LuaProxy::World::mem))
				.def("mem", static_cast<luabind::object (LuaProxy::World::*)(int, LuaProxy::L_FIELDTYPE, lua_State*)>(&LuaProxy::World::mem)),

				class_<LuaProxy::LevelObject>("Level")
				.property("x", &LuaProxy::LevelObject::x, &LuaProxy::LevelObject::setX)
				.property("y", &LuaProxy::LevelObject::y, &LuaProxy::LevelObject::setY)
				.property("levelTitle", &LuaProxy::LevelObject::levelTitle)
				.def("mem", static_cast<void (LuaProxy::LevelObject::*)(int, LuaProxy::L_FIELDTYPE, luabind::object)>(&LuaProxy::LevelObject::mem))
				.def("mem", static_cast<luabind::object (LuaProxy::LevelObject::*)(int, LuaProxy::L_FIELDTYPE, lua_State*)>(&LuaProxy::LevelObject::mem))

			];
	}

	if(m_type == LUNALUA_LEVEL){
		module(L)
			[
				def("totalNPC", &LuaProxy::totalNPCs),
				def("npcs", &LuaProxy::npcs),
				def("findnpcs", &LuaProxy::findNPCs),
				def("triggerEvent", &LuaProxy::triggerEvent),
				def("playMusic", &LuaProxy::playMusic),
				def("gravity", (unsigned short(*)())&LuaProxy::gravity),
				def("gravity", (void(*)(unsigned short))&LuaProxy::gravity),
				def("earthquake", (unsigned short(*)())&LuaProxy::earthquake),
				def("earthquake", (void(*)(unsigned short))&LuaProxy::earthquake),
				def("jumpheight", (unsigned short(*)())&LuaProxy::jumpheight),
				def("jumpheight", (void(*)(unsigned short))&LuaProxy::jumpheight),
				def("jumpheightBounce", (unsigned short(*)())&LuaProxy::jumpheightBounce),
				def("jumpheightBounce", (void(*)(unsigned short))&LuaProxy::jumpheightBounce),
				def("runAnimation", (void(*)(int, double, double, double, double, double, double,int))&LuaProxy::runAnimation),
				def("runAnimation", (void(*)(int, double, double, double, double, int))&LuaProxy::runAnimation),
				def("runAnimation", (void(*)(int, double, double, int))&LuaProxy::runAnimation),
				def("npcToCoins", &LuaProxy::npcToCoins),
				def("blocks", &LuaProxy::blocks),
				def("findblocks", &LuaProxy::findblocks),
				def("findlayer", &LuaProxy::findlayer),
				def("exitLevel", &LuaProxy::exitLevel),
				def("winState", (unsigned short(*)())&LuaProxy::winState),
				def("winState", (void(*)(unsigned short))&LuaProxy::winState),
				def("animations", &LuaProxy::animations),
				def("getInput", &LuaProxy::getInput),
				def("hud", &LuaProxy::hud),
				def("getLevelFilename", &LuaProxy::getLevelFilename),
				def("getLevelName", &LuaProxy::getLevelName),
				def("spawnNPC", &LuaProxy::spawnNPC),		

				class_<LuaProxy::Animation>("Animation")
				.def(constructor<int>())
				.def("mem", static_cast<void (LuaProxy::Animation::*)(int, LuaProxy::L_FIELDTYPE, luabind::object)>(&LuaProxy::Animation::mem))
				.def("mem", static_cast<luabind::object (LuaProxy::Animation::*)(int, LuaProxy::L_FIELDTYPE, lua_State*)>(&LuaProxy::Animation::mem))
				.property("id", &LuaProxy::Animation::id, &LuaProxy::Animation::setId)
				.property("x", &LuaProxy::Animation::x, &LuaProxy::Animation::setX)
				.property("y", &LuaProxy::Animation::y, &LuaProxy::Animation::setY)
				.property("speedX", &LuaProxy::Animation::speedX, &LuaProxy::Animation::setSpeedX)
				.property("speedY", &LuaProxy::Animation::speedY, &LuaProxy::Animation::setSpeedY)
				.property("width", &LuaProxy::Animation::width, &LuaProxy::Animation::setWidth)
				.property("height", &LuaProxy::Animation::height, &LuaProxy::Animation::setHeight)
				.property("timer", &LuaProxy::Animation::timer, &LuaProxy::Animation::setTimer),

				class_<LuaProxy::Layer>("Layer")
				.def(constructor<int>())
				.def("stop", &LuaProxy::Layer::stop)
				.property("speedX", &LuaProxy::Layer::speedX, &LuaProxy::Layer::setSpeedX)
				.property("speedY", &LuaProxy::Layer::speedY, &LuaProxy::Layer::setSpeedY)
				.property("layerName", &LuaProxy::Layer::layerName),

				class_<LuaProxy::Section>("Section")
				.def(constructor<int>())
				.property("boundary", &LuaProxy::Section::boundary, &LuaProxy::Section::setBoundary),

				class_<LuaProxy::NPC>("NPC")
				.def(constructor<int>())
				.def("mem", static_cast<void (LuaProxy::NPC::*)(int, LuaProxy::L_FIELDTYPE, luabind::object, lua_State*)>(&LuaProxy::NPC::mem))
				.def("mem", static_cast<luabind::object (LuaProxy::NPC::*)(int, LuaProxy::L_FIELDTYPE, lua_State*)>(&LuaProxy::NPC::mem))
				.def("kill", static_cast<void (LuaProxy::NPC::*)(lua_State*)>(&LuaProxy::NPC::kill))
				.def("kill", static_cast<void (LuaProxy::NPC::*)(int, lua_State*)>(&LuaProxy::NPC::kill))
				.property("id", &LuaProxy::NPC::id)
				.property("direction", &LuaProxy::NPC::direction, &LuaProxy::NPC::setDirection)
				.property("x", &LuaProxy::NPC::x, &LuaProxy::NPC::setX)
				.property("y", &LuaProxy::NPC::y, &LuaProxy::NPC::setY)
				.property("speedX", &LuaProxy::NPC::speedX, &LuaProxy::NPC::setSpeedX)
				.property("speedY", &LuaProxy::NPC::speedY, &LuaProxy::NPC::setSpeedY)
				.property("attachedLayerName", &LuaProxy::NPC::attachedLayerName)
				.property("activateEventName", &LuaProxy::NPC::activateEventName)
				.property("deathEventName", &LuaProxy::NPC::deathEventName)
				.property("noMoreObjInLayer", &LuaProxy::NPC::noMoreObjInLayer)
				.property("talkEventName", &LuaProxy::NPC::talkEventName)
				.property("msg", &LuaProxy::NPC::msg)
				.property("layerName", &LuaProxy::NPC::layerName)
				.property("attachedLayerObj", &LuaProxy::NPC::attachedLayerObj)
				.property("layerObj", &LuaProxy::NPC::layerObj)
				.property("isValid", &LuaProxy::NPC::isValid),

				class_<LuaProxy::Player>("Player")
				.def(constructor<>())
				.def(constructor<int>())
				.def("mem", static_cast<void (LuaProxy::Player::*)(int, LuaProxy::L_FIELDTYPE, luabind::object, lua_State*)>(&LuaProxy::Player::mem))
				.def("mem", static_cast<luabind::object (LuaProxy::Player::*)(int, LuaProxy::L_FIELDTYPE, lua_State*)>(&LuaProxy::Player::mem))
				.def("kill", &LuaProxy::Player::kill)
				.def("harm", &LuaProxy::Player::harm)
				.property("screen", &LuaProxy::Player::screen)
				.property("section", &LuaProxy::Player::section)
				.property("sectionObj", &LuaProxy::Player::sectionObj)
				.property("x", &LuaProxy::Player::x, &LuaProxy::Player::setX)
				.property("y", &LuaProxy::Player::y, &LuaProxy::Player::setY)
				.property("speedX", &LuaProxy::Player::speedX, &LuaProxy::Player::setSpeedX)
				.property("speedY", &LuaProxy::Player::speedY, &LuaProxy::Player::setSpeedY)
				.property("powerup", &LuaProxy::Player::powerup, &LuaProxy::Player::setPowerup)
				.property("reservePowerup", &LuaProxy::Player::reservePowerup, &LuaProxy::Player::setReservePowerup)
				.property("holdingNPC", &LuaProxy::Player::holdingNPC)
				.property("isValid", &LuaProxy::Player::isValid)
				/*Generated by code*/
				.property("ToadDoubleJReady", &LuaProxy::Player::toadDoubleJReady, &LuaProxy::Player::setToadDoubleJReady)
				.property("SparklingEffect", &LuaProxy::Player::sparklingEffect, &LuaProxy::Player::setSparklingEffect)
				.property("UnknownCTRLLock1", &LuaProxy::Player::unknownCTRLLock1, &LuaProxy::Player::setUnknownCTRLLock1)
				.property("UnknownCTRLLock2", &LuaProxy::Player::unknownCTRLLock2, &LuaProxy::Player::setUnknownCTRLLock2)
				.property("QuicksandEffectTimer", &LuaProxy::Player::quicksandEffectTimer, &LuaProxy::Player::setQuicksandEffectTimer)
				.property("OnSlipperyGround", &LuaProxy::Player::onSlipperyGround, &LuaProxy::Player::setOnSlipperyGround)
				.property("IsAFairy", &LuaProxy::Player::isAFairy, &LuaProxy::Player::setIsAFairy)
				.property("FairyAlreadyInvoked", &LuaProxy::Player::fairyAlreadyInvoked, &LuaProxy::Player::setFairyAlreadyInvoked)
				.property("FairyFramesLeft", &LuaProxy::Player::fairyFramesLeft, &LuaProxy::Player::setFairyFramesLeft)
				.property("SheathHasKey", &LuaProxy::Player::sheathHasKey, &LuaProxy::Player::setSheathHasKey)
				.property("SheathAttackCooldown", &LuaProxy::Player::sheathAttackCooldown, &LuaProxy::Player::setSheathAttackCooldown)
				.property("Hearts", &LuaProxy::Player::hearts, &LuaProxy::Player::setHearts)
				.property("PeachHoverAvailable", &LuaProxy::Player::peachHoverAvailable, &LuaProxy::Player::setPeachHoverAvailable)
				.property("PressingHoverButton", &LuaProxy::Player::pressingHoverButton, &LuaProxy::Player::setPressingHoverButton)
				.property("PeachHoverTimer", &LuaProxy::Player::peachHoverTimer, &LuaProxy::Player::setPeachHoverTimer)
				.property("Unused1", &LuaProxy::Player::unused1, &LuaProxy::Player::setUnused1)
				.property("PeachHoverTrembleSpeed", &LuaProxy::Player::peachHoverTrembleSpeed, &LuaProxy::Player::setPeachHoverTrembleSpeed)
				.property("PeachHoverTrembleDir", &LuaProxy::Player::peachHoverTrembleDir, &LuaProxy::Player::setPeachHoverTrembleDir)
				.property("ItemPullupTimer", &LuaProxy::Player::itemPullupTimer, &LuaProxy::Player::setItemPullupTimer)
				.property("ItemPullupMomentumSave", &LuaProxy::Player::itemPullupMomentumSave, &LuaProxy::Player::setItemPullupMomentumSave)
				.property("Unused2", &LuaProxy::Player::unused2, &LuaProxy::Player::setUnused2)
				.property("UnkClimbing1", &LuaProxy::Player::unkClimbing1, &LuaProxy::Player::setUnkClimbing1)
				.property("UnkClimbing2", &LuaProxy::Player::unkClimbing2, &LuaProxy::Player::setUnkClimbing2)
				.property("UnkClimbing3", &LuaProxy::Player::unkClimbing3, &LuaProxy::Player::setUnkClimbing3)
				.property("WaterState", &LuaProxy::Player::waterState, &LuaProxy::Player::setWaterState)
				.property("IsInWater", &LuaProxy::Player::isInWater, &LuaProxy::Player::setIsInWater)
				.property("WaterStrokeTimer", &LuaProxy::Player::waterStrokeTimer, &LuaProxy::Player::setWaterStrokeTimer)
				.property("UnknownHoverTimer", &LuaProxy::Player::unknownHoverTimer, &LuaProxy::Player::setUnknownHoverTimer)
				.property("SlidingState", &LuaProxy::Player::slidingState, &LuaProxy::Player::setSlidingState)
				.property("SlidingGroundPuffs", &LuaProxy::Player::slidingGroundPuffs, &LuaProxy::Player::setSlidingGroundPuffs)
				.property("ClimbingState", &LuaProxy::Player::climbingState, &LuaProxy::Player::setClimbingState)
				.property("UnknownTimer", &LuaProxy::Player::unknownTimer, &LuaProxy::Player::setUnknownTimer)
				.property("UnknownFlag", &LuaProxy::Player::unknownFlag, &LuaProxy::Player::setUnknownFlag)
				.property("UnknownPowerupState", &LuaProxy::Player::unknownPowerupState, &LuaProxy::Player::setUnknownPowerupState)
				.property("SlopeRelated", &LuaProxy::Player::slopeRelated, &LuaProxy::Player::setSlopeRelated)
				.property("TanookiStatueActive", &LuaProxy::Player::tanookiStatueActive, &LuaProxy::Player::setTanookiStatueActive)
				.property("TanookiMorphCooldown", &LuaProxy::Player::tanookiMorphCooldown, &LuaProxy::Player::setTanookiMorphCooldown)
				.property("TanookiActiveFrameCount", &LuaProxy::Player::tanookiActiveFrameCount, &LuaProxy::Player::setTanookiActiveFrameCount)
				.property("IsSpinjumping", &LuaProxy::Player::isSpinjumping, &LuaProxy::Player::setIsSpinjumping)
				.property("SpinjumpStateCounter", &LuaProxy::Player::spinjumpStateCounter, &LuaProxy::Player::setSpinjumpStateCounter)
				.property("SpinjumpLandDirection", &LuaProxy::Player::spinjumpLandDirection, &LuaProxy::Player::setSpinjumpLandDirection)
				.property("CurrentKillCombo", &LuaProxy::Player::currentKillCombo, &LuaProxy::Player::setCurrentKillCombo)
				.property("GroundSlidingPuffsState", &LuaProxy::Player::groundSlidingPuffsState, &LuaProxy::Player::setGroundSlidingPuffsState)
				.property("WarpNearby", &LuaProxy::Player::warpNearby, &LuaProxy::Player::setWarpNearby)
				.property("Unknown5C", &LuaProxy::Player::unknown5C, &LuaProxy::Player::setUnknown5C)
				.property("Unknown5E", &LuaProxy::Player::unknown5E, &LuaProxy::Player::setUnknown5E)
				.property("HasJumped", &LuaProxy::Player::hasJumped, &LuaProxy::Player::setHasJumped)
				.property("CurXPos", &LuaProxy::Player::curXPos, &LuaProxy::Player::setCurXPos)
				.property("CurYPos", &LuaProxy::Player::curYPos, &LuaProxy::Player::setCurYPos)
				.property("Height", &LuaProxy::Player::height, &LuaProxy::Player::setHeight)
				.property("Width", &LuaProxy::Player::width, &LuaProxy::Player::setWidth)
				.property("CurXSpeed", &LuaProxy::Player::curXSpeed, &LuaProxy::Player::setCurXSpeed)
				.property("CurYSpeed", &LuaProxy::Player::curYSpeed, &LuaProxy::Player::setCurYSpeed)
				.property("Identity", &LuaProxy::Player::identity, &LuaProxy::Player::setIdentity)
				.property("UKeyState", &LuaProxy::Player::uKeyState, &LuaProxy::Player::setUKeyState)
				.property("DKeyState", &LuaProxy::Player::dKeyState, &LuaProxy::Player::setDKeyState)
				.property("LKeyState", &LuaProxy::Player::lKeyState, &LuaProxy::Player::setLKeyState)
				.property("RKeyState", &LuaProxy::Player::rKeyState, &LuaProxy::Player::setRKeyState)
				.property("JKeyState", &LuaProxy::Player::jKeyState, &LuaProxy::Player::setJKeyState)
				.property("SJKeyState", &LuaProxy::Player::sJKeyState, &LuaProxy::Player::setSJKeyState)
				.property("XKeyState", &LuaProxy::Player::xKeyState, &LuaProxy::Player::setXKeyState)
				.property("RNKeyState", &LuaProxy::Player::rNKeyState, &LuaProxy::Player::setRNKeyState)
				.property("SELKeyState", &LuaProxy::Player::sELKeyState, &LuaProxy::Player::setSELKeyState)
				.property("STRKeyState", &LuaProxy::Player::sTRKeyState, &LuaProxy::Player::setSTRKeyState)
				.property("FacingDirection", &LuaProxy::Player::facingDirection, &LuaProxy::Player::setFacingDirection)
				.property("MountType", &LuaProxy::Player::mountType, &LuaProxy::Player::setMountType)
				.property("MountColor", &LuaProxy::Player::mountColor, &LuaProxy::Player::setMountColor)
				.property("MountState", &LuaProxy::Player::mountState, &LuaProxy::Player::setMountState)
				.property("MountHeightOffset", &LuaProxy::Player::mountHeightOffset, &LuaProxy::Player::setMountHeightOffset)
				.property("MountGfxIndex", &LuaProxy::Player::mountGfxIndex, &LuaProxy::Player::setMountGfxIndex)
				.property("CurrentPowerup", &LuaProxy::Player::currentPowerup, &LuaProxy::Player::setCurrentPowerup)
				.property("CurrentPlayerSprite", &LuaProxy::Player::currentPlayerSprite, &LuaProxy::Player::setCurrentPlayerSprite)
				.property("Unused116", &LuaProxy::Player::unused116, &LuaProxy::Player::setUnused116)
				.property("GfxMirrorX", &LuaProxy::Player::gfxMirrorX, &LuaProxy::Player::setGfxMirrorX)
				.property("UpwardJumpingForce", &LuaProxy::Player::upwardJumpingForce, &LuaProxy::Player::setUpwardJumpingForce)
				.property("JumpButtonHeld", &LuaProxy::Player::jumpButtonHeld, &LuaProxy::Player::setJumpButtonHeld)
				.property("SpinjumpButtonHeld", &LuaProxy::Player::spinjumpButtonHeld, &LuaProxy::Player::setSpinjumpButtonHeld)
				.property("ForcedAnimationState", &LuaProxy::Player::forcedAnimationState, &LuaProxy::Player::setForcedAnimationState)
				.property("Unknown124", &LuaProxy::Player::unknown124, &LuaProxy::Player::setUnknown124)
				.property("Unknown128", &LuaProxy::Player::unknown128, &LuaProxy::Player::setUnknown128)
				.property("DownButtonMirror", &LuaProxy::Player::downButtonMirror, &LuaProxy::Player::setDownButtonMirror)
				.property("InDuckingPosition", &LuaProxy::Player::inDuckingPosition, &LuaProxy::Player::setInDuckingPosition)
				.property("SelectButtonMirror", &LuaProxy::Player::selectButtonMirror, &LuaProxy::Player::setSelectButtonMirror)
				.property("Unknown132", &LuaProxy::Player::unknown132, &LuaProxy::Player::setUnknown132)
				.property("DownButtonTapped", &LuaProxy::Player::downButtonTapped, &LuaProxy::Player::setDownButtonTapped)
				.property("Unknown136", &LuaProxy::Player::unknown136, &LuaProxy::Player::setUnknown136)
				.property("XMomentumPush", &LuaProxy::Player::xMomentumPush, &LuaProxy::Player::setXMomentumPush)
				.property("DeathState", &LuaProxy::Player::deathState, &LuaProxy::Player::setDeathState)
				.property("DeathTimer", &LuaProxy::Player::deathTimer, &LuaProxy::Player::setDeathTimer)
				.property("BlinkTimer", &LuaProxy::Player::blinkTimer, &LuaProxy::Player::setBlinkTimer)
				.property("BlinkState", &LuaProxy::Player::blinkState, &LuaProxy::Player::setBlinkState)
				.property("Unknown144", &LuaProxy::Player::unknown144, &LuaProxy::Player::setUnknown144)
				.property("LayerStateStanding", &LuaProxy::Player::layerStateStanding, &LuaProxy::Player::setLayerStateStanding)
				.property("LayerStateLeftContact", &LuaProxy::Player::layerStateLeftContact, &LuaProxy::Player::setLayerStateLeftContact)
				.property("LayerStateTopContact", &LuaProxy::Player::layerStateTopContact, &LuaProxy::Player::setLayerStateTopContact)
				.property("LayerStateRightContact", &LuaProxy::Player::layerStateRightContact, &LuaProxy::Player::setLayerStateRightContact)
				.property("PushedByMovingLayer", &LuaProxy::Player::pushedByMovingLayer, &LuaProxy::Player::setPushedByMovingLayer)
				.property("Unused150", &LuaProxy::Player::unused150, &LuaProxy::Player::setUnused150)
				.property("Unused152", &LuaProxy::Player::unused152, &LuaProxy::Player::setUnused152)
				.property("HeldNPCIndex", &LuaProxy::Player::heldNPCIndex, &LuaProxy::Player::setHeldNPCIndex)
				.property("Unknown156", &LuaProxy::Player::unknown156, &LuaProxy::Player::setUnknown156)
				.property("PowerupBoxContents", &LuaProxy::Player::powerupBoxContents, &LuaProxy::Player::setPowerupBoxContents)
				.property("CurrentSection", &LuaProxy::Player::currentSection, &LuaProxy::Player::setCurrentSection)
				.property("WarpTimer", &LuaProxy::Player::warpTimer, &LuaProxy::Player::setWarpTimer)
				.property("Unknown15E", &LuaProxy::Player::unknown15E, &LuaProxy::Player::setUnknown15E)
				.property("ProjectileTimer1", &LuaProxy::Player::projectileTimer1, &LuaProxy::Player::setProjectileTimer1)
				.property("ProjectileTimer2", &LuaProxy::Player::projectileTimer2, &LuaProxy::Player::setProjectileTimer2)
				.property("TailswipeTimer", &LuaProxy::Player::tailswipeTimer, &LuaProxy::Player::setTailswipeTimer)
				.property("Unknown166", &LuaProxy::Player::unknown166, &LuaProxy::Player::setUnknown166)
				.property("TakeoffSpeed", &LuaProxy::Player::takeoffSpeed, &LuaProxy::Player::setTakeoffSpeed)
				.property("CanFly", &LuaProxy::Player::canFly, &LuaProxy::Player::setCanFly)
				.property("IsFlying", &LuaProxy::Player::isFlying, &LuaProxy::Player::setIsFlying)
				.property("FlightTimeRemaining", &LuaProxy::Player::flightTimeRemaining, &LuaProxy::Player::setFlightTimeRemaining)
				.property("HoldingFlightRunButton", &LuaProxy::Player::holdingFlightRunButton, &LuaProxy::Player::setHoldingFlightRunButton)
				.property("HoldingFlightButton", &LuaProxy::Player::holdingFlightButton, &LuaProxy::Player::setHoldingFlightButton)
				.property("NPCBeingStoodOnIndex", &LuaProxy::Player::nPCBeingStoodOnIndex, &LuaProxy::Player::setNPCBeingStoodOnIndex)
				.property("Unknown178", &LuaProxy::Player::unknown178, &LuaProxy::Player::setUnknown178)
				.property("Unknown17A", &LuaProxy::Player::unknown17A, &LuaProxy::Player::setUnknown17A)
				.property("Unused17C", &LuaProxy::Player::unused17C, &LuaProxy::Player::setUnused17C)
				.property("Unused17E", &LuaProxy::Player::unused17E, &LuaProxy::Player::setUnused17E)
				.property("Unused180", &LuaProxy::Player::unused180, &LuaProxy::Player::setUnused180)
				.property("Unused182", &LuaProxy::Player::unused182, &LuaProxy::Player::setUnused182)
				.property("Unused184", &LuaProxy::Player::unused184, &LuaProxy::Player::setUnused184),

				class_<LuaProxy::Block>("Block")
				.def(constructor<int>())
				.def("mem", static_cast<void (LuaProxy::Block::*)(int, LuaProxy::L_FIELDTYPE, luabind::object)>(&LuaProxy::Block::mem))
				.def("mem", static_cast<luabind::object (LuaProxy::Block::*)(int, LuaProxy::L_FIELDTYPE, lua_State*)>(&LuaProxy::Block::mem))
				.def("collidesWith", &LuaProxy::Block::collidesWith)
				.property("x", &LuaProxy::Block::x, &LuaProxy::Block::setX)
				.property("y", &LuaProxy::Block::y, &LuaProxy::Block::setY)
				.property("speedX", &LuaProxy::Block::speedX, &LuaProxy::Block::setSpeedX)
				.property("speedY", &LuaProxy::Block::speedY, &LuaProxy::Block::setSpeedY)
				.property("id", &LuaProxy::Block::id, &LuaProxy::Block::setId)
				.property("invisible", &LuaProxy::Block::invisible, &LuaProxy::Block::setInvisible)
				.property("slippery", &LuaProxy::Block::slippery, &LuaProxy::Block::setSlippery)
				.property("layerName", &LuaProxy::Block::layerName)
				.property("layerObj", &LuaProxy::Block::layerObj)
			];
	}
}

void CLunaLua::doEvents()
{
	//If the lua module is not valid anyway, then just return
	if(!isValid())
		return;

	
	//If player is not valid then shutdown the lua module
	if(m_type == LUNALUA_LEVEL){
		if(!Player::Get(1)){
			shutdown();
			return;
		}
	}

	bool err = false;
	try
	{
		luabind::object evTable = LuaHelper::getEventCallbase(L, m_luaEventTableName);
		luabind::object cl = evTable["onLoop"];
		luabind::call_function<void>(cl);
		if(m_type == LUNALUA_LEVEL)
			LuaEvents::proccesEvents(L, m_luaEventTableName);
	}
	catch (luabind::error& /*e*/)
	{
		err = true;
	}
	if(err)
		shutdown();

	if(m_type == LUNALUA_LEVEL)
		LuaEvents::finishEventHandling();
}


