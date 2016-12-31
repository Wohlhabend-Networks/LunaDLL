--[[

'##:::::::'##::::'##:'##::: ##::::'###::::'##:::::::'##::::'##::::'###::::
 ##::::::: ##:::: ##: ###:: ##:::'## ##::: ##::::::: ##:::: ##:::'## ##:::
 ##::::::: ##:::: ##: ####: ##::'##:. ##:: ##::::::: ##:::: ##::'##:. ##::
 ##::::::: ##:::: ##: ## ## ##:'##:::. ##: ##::::::: ##:::: ##:'##:::. ##:
 ##::::::: ##:::: ##: ##. ####: #########: ##::::::: ##:::: ##: #########:
 ##::::::: ##:::: ##: ##:. ###: ##.... ##: ##::::::: ##:::: ##: ##.... ##:
 ########:. #######:: ##::. ##: ##:::: ##: ########:. #######:: ##:::: ##:
........:::.......:::..::::..::..:::::..::........:::.......:::..:::::..::
    '########:'##::: ##::'######:::'####:'##::: ##:'########::::
     ##.....:: ###:: ##:'##... ##::. ##:: ###:: ##: ##.....:::::
     ##::::::: ####: ##: ##:::..:::: ##:: ####: ##: ##::::::::::
     ######::: ## ## ##: ##::'####:: ##:: ## ## ##: ######::::::
     ##...:::: ##. ####: ##::: ##::: ##:: ##. ####: ##...:::::::
     ##::::::: ##:. ###: ##::: ##::: ##:: ##:. ###: ##::::::::::
     ########: ##::. ##:. ######:::'####: ##::. ##: ########::::
    ........::..::::..:::......::::....::..::::..::........:::::

This is currently a second version of main.lua

It is a attempt to merge lunaworld.lua and lunadll.lua in one state.

]]

--LunaLua Version

__LUNALUA = "0.7"
__isLuaError = false

__episodePath = ""
__customFolderPath = ""

-- Modified Native functions:
os.exit = function() error("Shutdown") end

-- FFI-based APIs (remove direct acccess to FFI though)
local function initFFIBasedAPIs()
    local ffi = require("ffi")
    
    -- Add high resolution clock under the Misc namespace
    ffi.cdef[[
        typedef long            BOOL;
        BOOL QueryPerformanceFrequency(int64_t *lpFrequency);
        BOOL QueryPerformanceCounter(int64_t *lpPerformanceCount);
    ]]
    local kernel32 = ffi.load("kernel32.dll")
    local function GetPerformanceFrequency()
        local anum = ffi.new("int64_t[1]");
        if kernel32.QueryPerformanceFrequency(anum) == 0 then
            return nil
        end
        return tonumber(anum[0])
    end
    local function GetPerformanceCounter()
        local anum = ffi.new("int64_t[1]")
        if kernel32.QueryPerformanceCounter(anum) == 0 then
            return nil
        end
        return tonumber(anum[0])
    end
    local performanceCounterFreq = GetPerformanceFrequency()
    Misc.clock = function()
        return GetPerformanceCounter() / performanceCounterFreq
    end
    
    -- Add GL engine FFI APIs
    ffi.cdef[[
        void* LunaLuaAlloc(size_t size);
        void LunaLuaGlDrawTriangles(const float* vert, const float* tex, unsigned int count);
        uint32_t* LunaLuaGetImageResourceBits(uint32_t bmpBoxIntPtr);
    ]]
    local LunaDLL = ffi.load("LunaDll.dll")
    
    local function safeMallocArray(typeName, count)
        local byteSize = ffi.sizeof(typeName)
        if(byteSize == nil)then
            error("Invalid type for allocating native array!", 2)
        end
        local ptrData = LunaDLL.LunaLuaAlloc(count * byteSize)
        local castedPtrData = ffi.cast(typeName .. "*", ptrData)
        return castedPtrData
    end
    
    Graphics.glNewCoordArray = function(size)
        return ffi.new("float[?]", size)
    end
    Graphics.glDrawTriangles = function(arg1, arg2, arg3)
        local arrLen = 2*arg3
        local arg1_raw = safeMallocArray("float", arrLen)
        local arg2_raw = safeMallocArray("float", arrLen)
        for i = 0,arrLen-1 do
            arg1_raw[i] = arg1[i] or 0
        end
        for i = 0,arrLen-1 do
            arg2_raw[i] = arg2[i] or 0
        end
        LunaDLL.LunaLuaGlDrawTriangles(arg1_raw, arg2_raw, arg3)
    end
    Graphics.getBits32 = function(resImg)
        -- type check
        if(type(resImg) ~= "userdata")then
            error("Wrong type for getBits argument #1 (expected LuaResourceImage, got " .. type(resImg) .. ")", 2)
        end
        if(resImg.__type == "LuaResourceImage")then
            error("Wrong type for getBits arguemnt #1 (expected LuaResourceImage, got " .. tostring(resImg.__type) .. ")", 2)
        end
        local nativeBitArray = LunaDLL.LunaLuaGetImageResourceBits(resImg.__BMPBoxPtr)
        
        -- get bits
        local bitMT = setmetatable({
        -- Normal Fields
            __data = nativeBitArray,
            __size = resImg.width * resImg.height - 1,
            __resImgRef = resImg -- Hold a strong reference, to prevent deallocation to the data
        }, {
        -- Metamethods
            __index = function(tbl, key)
                if(0 > key or tbl.__size < key)then error("Bit-Array out of bounds. (Valid index: 0-" .. tbl.__size .. ", got " .. key .. ")", 2) end
                return tbl.__data[key]
            end,
            __newindex = function(tbl, key, value)
                if(0 > key or tbl.__size < key)then error("Bit-Array out of bounds. (Valid index: 0-" .. tbl.__size .. ", got " .. key .. ")", 2) end
                tbl.__data[key] = value
            end
        })
        
        return bitMT
    end
    
    local function convertGlArray(arr, arr_len, glArrayType)
        if (arr == nil) then return 0 end
        local arr_offset = 0
        if (arr[0] == nil) then arr_offset = 1 end
        local mallocType = glArrayType or "float"
        local arr_raw = safeMallocArray(mallocType, arr_len)
        for i = 0,arr_len-1 do
            arr_raw[i] = arr[i+arr_offset] or 0
        end
        return tonumber(ffi.cast("unsigned int", arr_raw))
    end
    
    local function getGlElementCount(arr, divisor)
        local len_offset = 0
        if (arr[0] ~= nil) then len_offset = 1 end
        return math.floor((#arr + len_offset) / divisor)
    end
    
    
    Graphics.glTypeTable = {
        [GL_FLOAT]              = {glType = "number",   rawType = "float"},
        [GL_FLOAT_VEC2]         = {glType = "table",    rawType = "float",          glTableSize = 2},   -- 1x2
        [GL_FLOAT_VEC3]         = {glType = "table",    rawType = "float",          glTableSize = 3},   -- 1x3
        [GL_FLOAT_VEC4]         = {glType = "table",    rawType = "float",          glTableSize = 4},   -- 1x4
        [GL_FLOAT_MAT2]         = {glType = "table",    rawType = "float",          glTableSize = 4},   -- 2x2
        [GL_FLOAT_MAT3]         = {glType = "table",    rawType = "float",          glTableSize = 9},   -- 3x3
        [GL_FLOAT_MAT4]         = {glType = "table",    rawType = "float",          glTableSize = 16},  -- 4x4
        [GL_FLOAT_MAT2x3]       = {glType = "table",    rawType = "float",          glTableSize = 6},   -- 2x3
        [GL_FLOAT_MAT2x4]       = {glType = "table",    rawType = "float",          glTableSize = 8},   -- 2x4
        [GL_FLOAT_MAT3x2]       = {glType = "table",    rawType = "float",          glTableSize = 6},   -- 3x2
        [GL_FLOAT_MAT3x4]       = {glType = "table",    rawType = "float",          glTableSize = 12},  -- 3x4
        [GL_FLOAT_MAT4x2]       = {glType = "table",    rawType = "float",          glTableSize = 8},   -- 4x2
        [GL_FLOAT_MAT4x3]       = {glType = "table",    rawType = "float",          glTableSize = 12},  -- 4x3
        [GL_INT]                = {glType = "number",   rawType = "int"},  
        [GL_INT_VEC2]           = {glType = "table",    rawType = "int",            glTableSize = 2},   -- 1x2
        [GL_INT_VEC3]           = {glType = "table",    rawType = "int",            glTableSize = 3},   -- 1x3
        [GL_INT_VEC4]           = {glType = "table",    rawType = "int",            glTableSize = 4},   -- 1x4
        [GL_UNSIGNED_INT]       = {glType = "number",   rawType = "unsigned int"},  
        [GL_UNSIGNED_INT_VEC2]  = {glType = "table",    rawType = "unsigned int",   glTableSize = 2},   -- 1x2
        [GL_UNSIGNED_INT_VEC3]  = {glType = "table",    rawType = "unsigned int",   glTableSize = 3},   -- 1x3
        [GL_UNSIGNED_INT_VEC4]  = {glType = "table",    rawType = "unsigned int",   glTableSize = 4},   -- 1x4
        [GL_DOUBLE]             = {glType = "number",   rawType = "double"},
        [GL_DOUBLE_VEC2]        = {glType = "table",    rawType = "double",         glTableSize = 2},   -- 1x2
        [GL_DOUBLE_VEC3]        = {glType = "table",    rawType = "double",         glTableSize = 3},   -- 1x3
        [GL_DOUBLE_VEC4]        = {glType = "table",    rawType = "double",         glTableSize = 4},   -- 1x4
        [GL_DOUBLE_MAT2]        = {glType = "table",    rawType = "double",         glTableSize = 4},   -- 2x2
        [GL_DOUBLE_MAT3]        = {glType = "table",    rawType = "double",         glTableSize = 9},   -- 3x3
        [GL_DOUBLE_MAT4]        = {glType = "table",    rawType = "double",         glTableSize = 16},  -- 4x4
        [GL_DOUBLE_MAT2x3]      = {glType = "table",    rawType = "double",         glTableSize = 6},   -- 2x3
        [GL_DOUBLE_MAT2x4]      = {glType = "table",    rawType = "double",         glTableSize = 8},   -- 2x4
        [GL_DOUBLE_MAT3x2]      = {glType = "table",    rawType = "double",         glTableSize = 6},   -- 3x2
        [GL_DOUBLE_MAT3x4]      = {glType = "table",    rawType = "double",         glTableSize = 12},  -- 3x4
        [GL_DOUBLE_MAT4x2]      = {glType = "table",    rawType = "double",         glTableSize = 8},   -- 4x2
        [GL_DOUBLE_MAT4x3]      = {glType = "table",    rawType = "double",         glTableSize = 12}   -- 4x3
    }
    
    local function validateAndConvertVariableTable(variableArgs, variableInfoTable, variableTypeName)
        local formattedReturn = {}
        for varName, varValue in pairs(variableArgs) do
            local varInfo = variableInfoTable[varName]
            if(varInfo == nil)then
                error("Invalid " .. variableTypeName .. " " .. varName .. " (does not exists)", 3)
            end
            -- TODO: Support multi-dimensional arrays
            if(varInfo.arrayDepth > 1)then
                error("Multi-dimensional arrays are not supported yet!", 3)
            end
            
            local variableType = varInfo.type
            local glTypeOfVariable = Graphics.glTypeTable[variableType]
            local glTypeOfVariableInArg = type(varValue)
            
            local flatternedResult = nil -- This array will be the input array
            if(glTypeOfVariable.glType == "number")then -- myUniformVar = 1.0
                if(glTypeOfVariableInArg == "number")then
                    flatternedResult = {varValue}
                elseif(glTypeOfVariableInArg == "table")then -- myUniformVar = {1.0, 1.0, 1.0}
                    flatternedResult = varValue
                else
                    error("Invalid type for " .. variableTypeName .. " " .. varName .. " (expected number or table got " .. type(varValue) .. ")", 3)
                end
            elseif(glTypeOfVariable.glType == "table")then
                if(glTypeOfVariableInArg ~= "table")then
                    error("Invalid type for " .. variableTypeName .. " " .. varName .. " (expected table got " .. type(varValue) .. ")", 3)
                end
                local firstElem = varValue[1]
                local typeOfFirstElem = type(firstElem)
                if(typeOfFirstElem == "number")then -- myUniformVar = {1.0, 1.0, 1.0, 1.0}
                    flatternedResult = varValue
                elseif(typeOfFirstElem == "table")then -- myUniformVar = {{1.0, 1.0}, {1.0, 1.0}}
                    flatternedResult = {}
                    for i = 1, #varValue do
                        local nextElem = varValue[i]
                        for j = 1, #nextElem do
                            flatternedResult[#flatternedResult + 1] = nextElem[j]
                        end
                    end
                else
                    error("Invalid type for " .. variableTypeName .. " " .. varName .. " (expected number or table got " .. type(varValue) .. ")", 3)
                end
            end

            local sizeOfType = glTypeOfVariable.glTableSize or 1
            local totalNumberOfExpectedElements = varInfo.arrayCount * sizeOfType
            local actualNumberOfExpectedElements = #flatternedResult
            if(totalNumberOfExpectedElements ~= actualNumberOfExpectedElements)then
                error("Invalid number of elements for " .. variableTypeName .. " " .. varName .. " (expected " .. totalNumberOfExpectedElements .. " [" .. varInfo.arrayCount .. "x" .. sizeOfType .. "], got " .. actualNumberOfExpectedElements .. ")", 3)
            end
            
            formattedReturn[varInfo.id] = {glType = varInfo.type, data = convertGlArray(flatternedResult, totalNumberOfExpectedElements, glTypeOfVariable.rawType), count = varInfo.arrayCount}
        end
        return formattedReturn
    end
    
    
    Graphics.GL_POINTS         = 0
    Graphics.GL_LINES          = 1
    Graphics.GL_LINE_LOOP      = 2
    Graphics.GL_LINE_STRIP     = 3
    Graphics.GL_TRIANGLES      = 4
    Graphics.GL_TRIANGLE_STRIP = 5
    Graphics.GL_TRIANGLE_FAN   = 6
    Graphics.glDraw = function(args)
        local priority = args['priority'] or 1.0
        local texture = args['texture']
        local color = args['color'] or {1.0, 1.0, 1.0, 1.0}
        if (type(color) == "number") then
            error("Numeric colors support not yet existing")
        elseif (#color == 3) then
            color = {color[1], color[2], color[3], 1.0}
        end
        local vertCoords = args['vertexCoords']
        local texCoords = args['textureCoords']
        local vertColor = args['vertexColors']
        local arr_len = nil
        if (vertCoords == nil) then
            error("vertexCoords is required")
        end
        local arr_len = getGlElementCount(vertCoords, 2)
        if (texCoords ~= nil) then
            if (arr_len ~= getGlElementCount(texCoords, 2)) then
                error("Incorrect textureCoords len")
            end
        end
        if (vertColor ~= nil) then
            if (arr_len ~= getGlElementCount(vertColor, 4)) then
                error("Incorrect vertexColors len")
            end
        end
        vertCoords = convertGlArray(vertCoords, arr_len*2)
        texCoords = convertGlArray(texCoords, arr_len*2)
        vertColor = convertGlArray(vertColor, arr_len*4)
    
        local shader = args['shader'];
        if(shader)then
            if(not shader.isCompiled)then
                error("Shader is not compiled!", 2)
            end
        end
        
        -- Validate uniforms
        -- TODO: Also accept uniforms/attributes with named argument, i.e. {x = 5, y = 5}
        local uniformArgs = args['uniforms'] -- should be converted to [offsetID] = {type = [glEnumId], data = glArrayPtr}
        local uniformArgsConverted = nil
        if(uniformArgs ~= nil and shader ~= nil)then
            uniformArgsConverted = validateAndConvertVariableTable(uniformArgs, shader:getUniformInfo(), "uniform")
        end
        
        -- Validate attributes
        local attributeArgs = args['attributes'] -- should be converted to [offsetID] = {type = [glEnumId], data = glArrayPtr}
        local attributeArgsConverted = nil
        if(attributeArgs ~= nil and shader ~= nil)then
            attributeArgsConverted = validateAndConvertVariableTable(attributeArgs, shader:getAttributeInfo(), "attribute")
        end
    
        Graphics.__glInternalDraw{
            priority = priority, primitive = args['primitive'], sceneCoords = args['sceneCoords'], texture = texture,
            r = color[1], g = color[2], b = color[3], a = color[4],
            rawVer = vertCoords, rawTex = texCoords, rawCol = vertColor, rawCnt = arr_len,
            shader = shader, uniforms = uniformArgsConverted, attributes = attributeArgsConverted
        }
    end

    -- This function creates the "virtual" attributes for the sprite table.
    local function makeSpriteTable(spriteTypeKey, spriteIdx)
        local spriteMT = {
            __index = function(tbl, key)
                if (spriteTypeKey == "hardcoded") then
                    if (key == "img") then
                        return Graphics.__getHardcodedSpriteOverride("hardcoded-"..spriteIdx)
                    end
                else
                    if (key == "img") then
                        return Graphics.__getSpriteOverride(spriteTypeKey, spriteIdx)
                    end
                end
                error("Graphics.sprites." .. tostring(spriteTypeKey) .. "[" .. tostring(spriteIdx) .. "]." .. tostring(key) .. " does not exist")
            end,
            __newindex = function(tbl,key,val)
                if (spriteTypeKey == "hardcoded") then
                    if (key == "img") then
                        return Graphics.__setHardcodedSpriteOverride("hardcoded-"..spriteIdx, val)
                    end
                else
                    if (key == "img") then
                        Graphics.__setSpriteOverride(spriteTypeKey, spriteIdx, val)
                        return
                    end
                end
                error("Graphics.sprites." .. tostring(spriteTypeKey) .. "[" .. tostring(spriteIdx) .. "]." .. tostring(key) .. " does not exist")
            end
        }
        return setmetatable({}, spriteMT);
    end
    
    -- This function will create the Graphics.sprite.**** table, where **** is spriteTypeKey
    -- i.e Graphics.sprite.block
    local function makeSpriteTypeTable(spriteTypeKey)
        local spriteTypeMT = {
            __index = function(tbl, spriteIdx)
                return makeSpriteTable(spriteTypeKey, spriteIdx)
            end,
            __newindex = function(tbl,key,val)
                error("Cannot write to Graphics.sprites." .. spriteTypeKey .. " table")
            end
        }
        return setmetatable({}, spriteTypeMT)
    end
    
    -- To improve performance, we can cache those type tables
    local spriteTypeTableCache = {}
    local spritesMetatable = {
    __index = function(tbl,spriteTypeKey)
        if(not spriteTypeTableCache[spriteTypeKey])then
            spriteTypeTableCache[spriteTypeKey] = makeSpriteTypeTable(spriteTypeKey)
        end
        return spriteTypeTableCache[spriteTypeKey]
    end,
    __newindex = function(tbl,key,val)
        error("Cannot write to Graphics.sprites table")
    end
    }
    Graphics.sprites = {}
    setmetatable(Graphics.sprites, spritesMetatable);
    
    
    -- This function creates the "virtual" attributes for the sound table.
    local function makeSoundTable(soundIdx)
        if (type(soundIdx) ~= "number") or (math.floor(soundIdx) ~= soundIdx) or (soundIdx < 1) or (soundIdx > 91) then
            error("Audio.sounds" .. "[" .. tostring(soundIdx) .. "] does not exist")
        end
        local soundAlias = "sound" .. tostring(soundIdx)
        local spriteMT = {
            __index = function(tbl, key)
                if (key == "sfx") then
                    return Audio.__getChunkForAlias(soundAlias)
                end
                if (key == "muted") then
                    return Audio.__getMuteForAlias(soundAlias, val)
                end
                error("Audio.sounds" .. "[" .. tostring(soundIdx) .. "]." .. tostring(key) .. " does not exist")
            end,
            __newindex = function(tbl,key,val)
                if (key == "sfx") then
                    Audio.__setOverrideForAlias(soundAlias, val)
                    return
                end
                if (key == "muted") then
                    Audio.__setMuteForAlias(soundAlias, val)
                    return
                end
                error("Audio.sounds" .. "[" .. tostring(soundIdx) .. "]." .. tostring(key) .. " does not exist")
            end
        }
        return setmetatable({}, spriteMT);
    end
    
    local soundsMetatable = {
        __index = function(tbl,soundIdx)
            return makeSoundTable(soundIdx)
        end,
        __newindex = function(tbl,key,val)
            error("Cannot write to Audio.sounds table")
        end
    }
    Audio.sounds = {}
    setmetatable(Audio.sounds, soundsMetatable);
    local setmusvol = Audio.MusicVolume;
    local musvol = 80;
    Audio.MusicVolume(musvol);
    Audio.MusicVolume = function(volume)
        if(volume == nil) then return musvol;
        else
            musvol = volume;
            setmusvol(volume);
        end
    end
    
    -- Helper function for loadImage
    Graphics.loadImageResolved = function(file)
        local path = Misc.resolveGraphicsFile(file)
        if (path == nil) then
            error("Cannot find image: " .. file)
        end
        return Graphics.loadImage(path)
    end
    
    if (not isOverworld) then
        local bumpableMetatable = {
            __index = function(tbl, id)
                return Block._getBumpable(id)
            end,
            __newindex = function(tbl, id, val)
                Block._setBumpable(id, val)
            end
        }
        Block.bumpable = setmetatable({}, bumpableMetatable);
        
        local harmTypeMap = {
            [HARM_TYPE_JUMP]=true,
            [HARM_TYPE_FROMBELOW]=true,
            [HARM_TYPE_NPC]=true,
            [HARM_TYPE_PROJECTILE_USED]=true,
            [HARM_TYPE_LAVA]=true,
            [HARM_TYPE_HELD]=true,
            [HARM_TYPE_TAIL]=true,
            [HARM_TYPE_SPINJUMP]=true,
            [HARM_TYPE_OFFSCREEN]=true,
            [HARM_TYPE_SWORD]=true
        }
        local vulnerableHarmTypesMetatable = {
            __index = function(tbl, id)
                local mskVal = NPC._getVulnerableHarmTypes(id)
                local tblVal = {}
                for v,_ in pairs(harmTypeMap) do
                    if (bit.band(mskVal, bit.lshift(1, v)) ~= 0) then
                        table.insert(tblVal, v)
                    end
                end
                return tblVal
            end,
            __newindex = function(tbl, id, tblVal)
                local mskVal = 0
                for _,v in ipairs(tblVal) do
                    if harmTypeMap[v] then
                        mskVal = bit.bor(mskVal, bit.lshift(1, v))
                    end
                end
                NPC._setVulnerableHarmTypes(id, mskVal)
            end
        }
        NPC.vulnerableHarmTypes = setmetatable({}, vulnerableHarmTypesMetatable);
        
        local spinjumpSafeMetatable = {
            __index = function(tbl, id)
                return NPC._getSpinjumpSafe(id)
            end,
            __newindex = function(tbl, id, val)
                NPC._setSpinjumpSafe(id, val)
            end
        }
        NPC.spinjumpSafe = setmetatable({}, spinjumpSafeMetatable);
    end
    
    -- Limit access to FFI
    package.preload['ffi'] = nil
    package.loaded['ffi'] = nil
end

local function initJSON()
    package.path = package.path .. ";./LuaScriptsLib/ext/?.lua"
    _G["json"] = require("lunajson")
end

local nativeIO = io.open
local nativeIsSamePath = Misc.isSamePath
local function initSafeIO()
    io.open = function(filename, mode)
        local badFiles = {
            "./config/luna.ini",
            "./config/game.ini",
            "./config/autostart.ini",
            "./luna.ini",
            "./game.ini",
            "./autostart.ini",
            "./LuaScriptsLib/mainV2.lua"
        }
        local hasSame = false
        for _, nextPath in pairs(badFiles) do
            if(nativeIsSamePath(nextPath, filename))then
                hasSame = true
            end
        end
        if hasSame then
            error("You cannot access this path: " .. filename);
        end
        return nativeIO(filename, mode)
    end
end

initSafeIO()
initJSON()
initFFIBasedAPIs()

-- We want the JIT running, so it's initially preloaded, but disable access to it
package.preload['jit'] = nil
package.loaded['jit'] = nil

-- ERR HANDLING v2.0, Let's get some more good ol' data
function __xpcall (f, ...)
  return xpcall(f,
    function (msg)
      -- build the error message
      return "==> "..msg..'\n'.."============="..'\n'..debug.traceback()
    end, ...)
end
-- ERR HANDLING END

-- ====================================================================
-- =================== NEW REWORKED MAIN FUNTION ======================
-- ====================================================================

--=====================================================================
--[[ Utils ]]--
function string:split(sep)
    local sep, fields = sep or ":", {}
    local pattern = string.format("([^%s]+)", sep)
    self:gsub(pattern, function(c) fields[#fields+1] = c end)
    return fields
end

function __xpcallCheck(returnData)
    if(not returnData[1])then
        Text.windowDebugSimple(returnData[2])
        __isLuaError = true
        return false
    end
    return true
end



local function findLast(haystack, needle)
    local i=haystack:match(".*"..needle.."()")
    if i==nil then return -1 else return i-1 end
end

local function filenameOfPath(path)
    local lastIndexOfSlash = findLast(path, "/")
    local lastIndexOfBackslash = findLast(path, "\\")
    if(lastIndexOfSlash == -1 and lastIndexOfBackslash == -1)then return path end
    
    local lastIndex = lastIndexOfSlash
    if(lastIndex < lastIndexOfBackslash)then
        lastIndex = lastIndexOfBackslash
    end
    return path:sub(lastIndex + 1)
end

local function isAbsolutePath(possiblePath)
    return possiblePath:find("%a:[/\\]") == 1 -- Either returns the first character with the matching search pattern or nil
end


-- Version check
function compareLunaVersion(...)
    local versionNum = {...}
    -- We want to check the version number depending on the depth.
    -- i.e. You use compareVersion(0, 7, 2) then the first 3 version number
    local versionNumCount = #versionNum
    local internalVersionNumCount = #__LUNA_VERSION_TABLE
    if(versionNumCount > internalVersionNumCount)then
        versionNumCount = internalVersionNumCount
    end
    
    for i = 1, versionNumCount do
        if(versionNum[i] > __LUNA_VERSION_TABLE[i])then
            return 1
        end
        if(versionNum[i] < __LUNA_VERSION_TABLE[i])then
            return -1
        end
    end
    return 0
end

--=====================================================================
--[[ API Functions ]]--
local APIHelper = {}

local function loadAPIByPath(path)
    local endings = {
        ".lua",
        ".dll"
    }
    for _,ending in pairs(endings) do
        func, err = loadfile(path..ending)
        if(func)then
            local loadedAPI = func()
            if(type(loadedAPI) ~= "table")then
                error("API \""..apiPath.."\" did not return the api-table (got "..type(loadedAPI)..")", 2)
            end
            return loadedAPI
        else
            if(not err:find("such file"))then
                error(err, 2)
            end
        end
    end
end

function APIHelper.doAPI(apiTableHolder, apiPath)
    local apiName = filenameOfPath(apiPath)
    if(apiTableHolder[apiName])then
        return apiTableHolder[apiName], false
    end
    
    local loadedAPI = nil
    if(isAbsolutePath(apiPath))then
        loadedAPI = loadAPIByPath(apiPath)
    else
        local searchInPath = {
            __episodePath,
            __customFolderPath,
            getSMBXPath().."\\LuaScriptsLib\\"
        }
        local func, err
        for _,apiSearchPath in pairs(searchInPath) do
            loadedAPI = loadAPIByPath(apiSearchPath..apiPath)
            if(loadedAPI) then break end
        end
    end
    
    if(not loadedAPI) then error("No API found \""..apiPath.."\"",2) end
   
    apiTableHolder[apiName] = loadedAPI
    if(type(loadedAPI["onInitAPI"])=="function")then
        loadedAPI.onInitAPI()
    end
    return loadedAPI, true
end

local function isAPILoadedByAPITable(apiTable, api)
    if(type(api)=="table")then
        for _,v in pairs(apiTable) do
            if(v == api)then
                return true
            end
        end
    end
    if(type(api)=="string")then
        if(apiTable[api])then
            return true
        end
    end
    return false
end

-- API Namespace implementation
local _loadingAsShared = false
local function implementAPINamespace(loadedAPIsTable)
    return {
        load = (function(api, isShared)
            local ret
            local oldLoadingAsShared = _loadingAsShared
            if (isShared == nil) or (isShared) then
                _loadingAsShared = true
                ret = APIHelper.doAPI(UserCodeManager.sharedAPIs, api)
            elseif (loadedAPIsTable ~= nil) then
                _loadingAsShared = false
                ret = APIHelper.doAPI(loadedAPIsTable, api)
            else
                error("Cannot load APIs as non-shared outside usercode")
            end
            _loadingAsShared = oldLoadingAsShared
            return ret
        end),
        isLoaded = (function(api)
            if(isAPILoadedByAPITable(UserCodeManager.sharedAPIs, api)) then
                return true
            end
            if (loadedAPIsTable ~= nil) then
                return isAPILoadedByAPITable(loadedAPIsTable, api)
            end
            return false
        end),
        addHandler = (function(apiTable, event, eventHandler, beforeMainCall)
            EventManager.addAPIListener(apiTable, event, eventHandler, beforeMainCall)
        end),
        remHandler = (function(apiTable, event, eventHandler)
            return EventManager.removeAPIListener(apiTable, event, eventHandler)
        end),
        isLoadingShared = (function()
            return _loadingAsShared
        end)
    }
end
API = implementAPINamespace(nil)

-- Deprecated API APIs
function isAPILoaded(api)
    if(isAPILoadedByAPITable(UserCodeManager.sharedAPIs, api)) then return true end
    for _, nextCodeFile in pairs(UserCodeManager.codefiles) do
        if(isAPILoadedByAPITable(nextCodeFile.loadedAPIs, api))then
            return true
        end
    end
    return false
end
function loadSharedAPI(api)
    return API.load(api)
end
function registerEvent(apiTable, event, eventHandler, beforeMainCall)
    EventManager.addAPIListener(apiTable, event, eventHandler, beforeMainCall)
end
function unregisterEvent(apiTable, event, eventHandler)
    return EventManager.removeAPIListener(apiTable, event, eventHandler)
end
function clearEvents(apiTable)
    EventManager.clearAPIListeners(apiTable)
end

function registerCustomEvent(obj, eventName)
    local queue = {};
    local mt = getmetatable(obj);
    if(mt == nil) then
        mt = {__index = function(tbl,key) return rawget(tbl,key) end, __newindex = function(tbl,key,val) rawset(tbl,key,val) end}
    end
    local index_f = mt.__index;
    local newindex_f = mt.__newindex;
    
    mt.__index = function(tbl, key)
        if(key == eventName) then
            return function(...)
                for _,v in ipairs(queue) do
                    v(...);
                end
            end
        else
            return index_f(tbl, key);
        end
    end
    
    mt.__newindex = function (tbl,key,val)
        if(key == eventName) then
            table.insert(queue, val);
        else
            newindex_f(tbl,key,val);
        end
    end
    
    setmetatable(obj,mt);
end




--=====================================================================
--[[ Main User Code Manager ]]--
UserCodeManager = {}
UserCodeManager.codefiles = {}
UserCodeManager.sharedAPIs = {}

-- Codefile manager START
function UserCodeManager.addCodeFile(codeFileName, loadedCodeFile, apiTable)
    local newCodeFileEntry = {
        name = codeFileName,
        loadedAPIs = apiTable,
        instance = loadedCodeFile
    }
    table.insert(UserCodeManager.codefiles, newCodeFileEntry)
end

function UserCodeManager.getCodeFile(codeFileName)
    for _, v in pairs(UserCodeManager.codefiles) do
        if(v.name == codeFileName)then
            return v
        end
    end
    return nil
end

-- Codefile manager END

function UserCodeManager.loadCodeFile(codeFileName, codeFilePath)
    -- 1. Setup the usercode instance
    local usercodeInstance = {}
    local loadedAPIsTable = {}
    
    local thisAPINamesace = implementAPINamespace(loadedAPIsTable)
    
    -- 2. Setup environment
    local usercodeEnvironment = {
        -- 2.1 Add loadAPI function
        loadAPI = (function(api)
            return thisAPINamesace.load(api, false)
        end),
        
        -- API Namespace implementation
        API = thisAPINamesace
    }
    
    -- 2.2 Add custom environment (FIXME: Add proxy environment later!)
    local eventEnvironment = {}
    
    -- 3. Add access to global environment
    setmetatable( usercodeEnvironment, { __index = _G } )
    
    -- 4. Load the code file and add environment
    local codeFile, err = loadfile(codeFilePath)
    if codeFile then
        -- 4.1 Set environment to the usercode
        setfenv( codeFile, usercodeEnvironment )
        -- 4.2 Execute file for initial run.
        codeFile()
    else
        -- 4.3 If file not found then ignore error, otherwise throw error!
        if(not err:find("such file"))then
            Text.windowDebugSimple("Error: "..err)
        end
        return false
    end
    
    -- 5. Directly add "global" fields to the table.
    for k,v in pairs( usercodeEnvironment ) do
        usercodeInstance[k] =  v
    end
    
    -- 6. Notify usercode file that loading has finished via "onLoad".
    if(type(usercodeInstance["onLoad"]) == "function")then
        usercodeInstance.onLoad()
    end

    -- 7. Now add the code file to the usercode table
    UserCodeManager.addCodeFile(codeFileName, usercodeEnvironment, loadedAPIsTable)
    
    -- 8. Subscript to all events
    EventManager.addUserListener(usercodeEnvironment)
    
    return true
end









--=====================================================================
--[[ Main Event Manager ]]--
EventManager = {}
EventManager.userListeners = {}
EventManager.apiListeners = {}
EventManager.queuedEvents = {}

-- ====================== Event Management ==============================
function EventManager.callApiListeners(name, isBefore, ...)
    for _, nextAPIToHandle in pairs(EventManager.apiListeners) do
        if(nextAPIToHandle.callBefore == isBefore)then
            if(nextAPIToHandle.eventName == name)then
                local hostObject = nextAPIToHandle.api
                if hostObject[nextAPIToHandle.eventHandlerName] then
                    hostObject[nextAPIToHandle.eventHandlerName](...)
                end
            end
        end
    end
end
    
function EventManager.callEvent(name, ...)
    local mainName, childName = unpack(name:split("."))
    if(mainName == nil or childName == nil)then
        mainName, childName = unpack(name:split(":"))
    end
    
    -- Call API listeners before usercodes.
    EventManager.callApiListeners(name, true, ...)
    
    -- Call usercode files
    for _, nextUserListener in pairs(EventManager.userListeners)do
        local hostObject = nextUserListener
        if(childName)then
            hostObject = nextUserListener[mainName]
            mainName = childName
        end
        if(hostObject[mainName])then
            hostObject[mainName](...)
        end
    end
    
    -- Call API Listeners after usercodes.
    EventManager.callApiListeners(name, false, ...)
end

function EventManager.queueEvent(name, ...)
    local newQueueEntry =
    {
        eventName = name,
        parameters = {...}
    }
    table.insert(EventManager.queuedEvents, newQueueEntry)
end
function EventManager.manageEventObj(eventObj, ...)
    local directEventName = eventObj.directEventName
    if(directEventName == "")then
        directEventName = eventObj.eventName .. "Direct"
    end
    EventManager.callEvent(directEventName, eventObj, ...)
    if(eventObj.loopable)then
        EventManager.queueEvent(eventObj.eventName, ...)
    end
end

-- ================== Event Distribution ===========================
-- This will add a new listener object.
-- table listenerObject (A code file)
function EventManager.addUserListener(listenerObject)
    table.insert(EventManager.userListeners, listenerObject)
end

function EventManager.addAPIListener(thisTable, event, eventHandler, beforeMainCall)
    if(type(thisTable) == "string")then
        error("\nOutdated version of API is trying to use registerEvent with string\nPlease contact the api developer to fix this issue!",2)
    end
    eventHandler = eventHandler or event --FIXME: Handle ==> NPC:onKill
    if (beforeMainCall == nil) then
        beforeMainCall = true
    end
    local newApiHandler =
    {
        api = thisTable,
        eventName = event,
        eventHandlerName = eventHandler,
        callBefore = beforeMainCall
    }
    
    table.insert(EventManager.apiListeners, newApiHandler)
end

function EventManager.clearAPIListeners(apiTable)
    for i = #EventManager.apiListeners, 1, -1 do
        local apiObj = EventManager.apiListeners[i]
        if(apiObj.api == apiTable) then
            table.remove(EventManager.apiListeners, i)
        end
    end
end

-- FIXME: Check also if "beforeMainCall"
function EventManager.removeAPIListener(apiTable, event, eventHandler)
    for i = 1, #EventManager.apiListeners do
        local apiObj = EventManager.apiListeners[i]
        if(apiObj.api == apiTable and
            apiObj.eventName == event and
            apiObj.eventHandlerName == eventHandler)then
            table.remove(EventManager.apiListeners, i)
            return true
        end
    end
    return false
end

function EventManager.doQueue()
    while(#EventManager.queuedEvents > 0)do
        local nextQueuedEvent = table.remove(EventManager.queuedEvents)
        EventManager.callEvent(nextQueuedEvent.eventName, unpack(nextQueuedEvent.parameters))
    end
    
    if(not isOverworld)then
        __ClassicEvents.doEvents()
    end
end









-- ===== FUNCTION USED BY LUNALUA ===== --
-- usage for luabind, always do with event-object

--[[
    The new core uses three functions:
        * __callEvent(...)
            - This function is called when LunaLua grabs a new event. This event is then futher processed and queued if possible.
              The first argument is always an event object by LunaLua core.
              The function which process it is called 'EventManager.manageEventObj'.
        * __doEventQueue()
            - This function is called when LunaLua should process the queued events.
              The function which process it is called 'EventManager.doQueue'.
        * __onInit(episodePath, lvlName)
            - This function is doing the initializing.
 
]]
function __callEvent(...)
    local pcallReturns = {__xpcall(EventManager.manageEventObj, ...)}
    __xpcallCheck(pcallReturns)
end

function __doEventQueue()
    local pcallReturns = {__xpcall(EventManager.doQueue)}
    __xpcallCheck(pcallReturns)
end

--Preloading function
--This code segment won't post any errors!
function __onInit(episodePath, lvlName)
    local pcallReturns = {__xpcall(function()
        --SEGMENT TO ADD PRELOADED APIS START
        Defines = APIHelper.doAPI(_G, "core\\defines")
        APIHelper.doAPI(_G, "uservar")
        DBG = APIHelper.doAPI(_G, "core\\dbg")
        __ClassicEvents = APIHelper.doAPI(_G, "core\\classicevents")
        Profiler = APIHelper.doAPI(_G, "core\\profiler")
        LunaTime = APIHelper.doAPI(_G, "core\\lunatime")
        Warn = APIHelper.doAPI(_G, "core\\warn")
        --SEGMENT TO ADD PRELOADED APIS END
        
        __episodePath = episodePath
        __customFolderPath = episodePath..string.sub(lvlName, 0, -5).."\\"
        local noFileLoaded = true
        if(UserCodeManager.loadCodeFile("lunabase", getSMBXPath().."\\LuaScriptsLib\\basegame\\lunabase.lua")) then noFileLoaded = false end
        if(not isOverworld)then
            -- Modern
            if(UserCodeManager.loadCodeFile("lunaglobal", episodePath .. "luna.lua")) then noFileLoaded = false end
            if(UserCodeManager.loadCodeFile("lunalocal", __customFolderPath.."luna.lua")) then noFileLoaded = false end
            -- Deprecated
            if(UserCodeManager.loadCodeFile("lunadll", __customFolderPath.."lunadll.lua")) then noFileLoaded = false end
            if(UserCodeManager.loadCodeFile("lunaworld", episodePath .. "lunaworld.lua")) then noFileLoaded = false end
        else
            -- Modern
            if(UserCodeManager.loadCodeFile("mapglobal", episodePath .. "map.lua")) then noFileLoaded = false end
            -- Deprecated
            if(UserCodeManager.loadCodeFile("lunaoverworld", episodePath .. "lunaoverworld.lua")) then noFileLoaded = false end
        end
        
        if(noFileLoaded)then
            __isLuaError = true
            return
        end
    end)}
    __xpcallCheck(pcallReturns)
end

