#include "LunaImage.h"
#include <Windows.h>
#include <string>
#include <cstdlib>
#include <atomic>
#include "../Misc/FreeImageUtils/FreeImageHelper.h"
#include "../Misc/FreeImageUtils/FreeImageData.h"
#include "../Misc/FreeImageUtils/FreeImageGifData.h"
#include "GL/GLEngineProxy.h"
#include "../Misc/ResourceFileMapper.h"

static std::unordered_map<std::wstring, std::weak_ptr<LunaImage>> g_lunaImageCache;
static std::vector<std::shared_ptr<LunaImage>> g_lunaImageHolder;

std::atomic<uint32_t> LunaImage::totalRawMem = 0;
std::atomic<uint32_t> LunaImage::totalCompMem = 0;

void LunaImage::holdCachedImages()
{
    g_lunaImageHolder.clear();
    for (auto cacheEntry = g_lunaImageCache.begin(); cacheEntry != g_lunaImageCache.end();)
    {
        std::shared_ptr<LunaImage> cachePtr = cacheEntry->second.lock();
        if (cachePtr)
        {
            g_lunaImageHolder.push_back(std::move(cachePtr));
            cacheEntry++;
        }
        else
        {
            g_lunaImageCache.erase(cacheEntry++);
        }
    }
}

void LunaImage::releaseCachedImages()
{
    g_lunaImageHolder.clear();
    for (auto cacheEntry = g_lunaImageCache.begin(); cacheEntry != g_lunaImageCache.end();)
    {
        std::shared_ptr<LunaImage> cachePtr = cacheEntry->second.lock();
        if (cachePtr)
        {
            cacheEntry++;
        }
        else
        {
            g_lunaImageCache.erase(cacheEntry++);
        }
    }
}

uint64_t LunaImage::getNewUID()
{
    static std::atomic<uint64_t> uidCounter = 1;
    return uidCounter.fetch_add(1, std::memory_order_relaxed);
}

std::shared_ptr<LunaImage> LunaImage::fromData(int width, int height, const uint8_t* data)
{
    // Allocate and copy into LunaImage
    std::shared_ptr<LunaImage> img = std::make_shared<LunaImage>();
    img->w = width;
    img->h = height;
    img->data = std::malloc(4 * width * height);
    if (img->data != nullptr)
    {
        memcpy(img->data, data, 4 * width * height);
		totalRawMem += 4 * width * height;
    }
    else
    {
        img = nullptr;
    }

    return img;
}

std::shared_ptr<LunaImage> LunaImage::fromHDC(HDC hdc)
{
    if (hdc == nullptr) return nullptr;

    HBITMAP convHBMP = CopyBitmapFromHdc(hdc);
    if (convHBMP == nullptr) return nullptr;

    BITMAP bmp;
    GetObject(convHBMP, sizeof(BITMAP), &bmp);

    // Allocate and copy into LunaImage
    std::shared_ptr<LunaImage> img = std::make_shared<LunaImage>();
    img->w = bmp.bmWidth;
    img->h = bmp.bmHeight;
    img->data = std::malloc(4 * img->w*img->h);
    if (img->data != nullptr)
    {
        memcpy(img->data, bmp.bmBits, 4 * img->w*img->h);
		totalRawMem += 4 * img->w*img->h;
    }
    else
    {
        img = nullptr;
    }

    // Deallocate temporary conversion memory
    DeleteObject(convHBMP);
    convHBMP = NULL;

    return img;
}

std::shared_ptr<LunaImage> LunaImage::fromFile(const wchar_t* filename, const ResourceFileInfo* metadata)
{
    if ((filename == nullptr) || (filename[0] == L'\0')) return nullptr;

    // If we have no file metadata, try to get some
    ResourceFileInfo localMetadata;
    if ((metadata != nullptr) && (!metadata->done))
    {
        metadata = nullptr;
    }
    if (metadata == nullptr) {
        localMetadata = GetResourceFileInfo(filename);
        if (localMetadata.done)
        {
            metadata = &localMetadata;
        }
    }
    if (metadata == nullptr)
    {
        return nullptr;
    }

    auto cacheSearchResult = g_lunaImageCache.find(filename);
    if (cacheSearchResult != g_lunaImageCache.end())
    {
        std::shared_ptr<LunaImage> cachePtr = cacheSearchResult->second.lock();
        if (cachePtr && (cachePtr->fileMetadata == *metadata))
        {
            return std::move(cachePtr);
        }
        else
        {
            g_lunaImageCache.erase(cacheSearchResult);
        }
    }

    std::shared_ptr<LunaImage> img = std::make_shared<LunaImage>();
    img->load(filename);
    if ((img->getW() == 0) && (img->getH() == 0))
    {
        return nullptr;
    }
    img->fileMetadata = *metadata;

    g_lunaImageCache[filename] = img;

    return std::move(img);
}

static bool pathIsPNG(const wchar_t* file)
{
	// Null check
	if (file == nullptr) return false;

	// Get string length
	uint32_t len = 0;
	for (len = 0; file[len] != L'\0'; len++);

	// Length check
	if (len < 4) return false;

	uint32_t pos = len - 4;
	if (file[pos] != L'.') return false;
	if ((file[pos + 1] != L'p') && (file[pos + 1] != L'P')) return false;
	if ((file[pos + 2] != L'n') && (file[pos + 2] != L'N')) return false;
	if ((file[pos + 3] != L'g') && (file[pos + 3] != L'G')) return false;
	
	return true;
}

static uint32_t readPngU32(const unsigned char* data)
{
	return (
		(((uint32_t)data[0]) << 24) |
		(((uint32_t)data[1]) << 16) |
		(((uint32_t)data[2]) << 8) |
		(((uint32_t)data[3]) << 0)
		);
}

static bool ReadPNGImageSize(const unsigned char* data, uint32_t len, uint32_t sizeOut[2])
{
	static const unsigned char PNG_HEADER[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
	static const unsigned char IHDR[4] = { 'I', 'H', 'D', 'R' };

	// Check header
	if ((len < 8) || (memcmp(data, PNG_HEADER, 8) != 0)) return false;
	data = &data[8];
	len -= 8;

	// Read idat chunk length
	if (len < 4) return false;
	uint32_t ihdrLen = readPngU32(data);
	data = &data[4];
	len -= 4;

	// Read idat chunk header
	if ((len < (4 + ihdrLen)) || (memcmp(data, IHDR, 4) != 0)) return false;
	data = &data[4];
	len -= 4;

	// Make sure idata chunk length is at least the standard size
	if (ihdrLen < 13) return false;

	// We won't check the CRC... if we got this far okay we'll trust it for these purposes
	if (sizeOut)
	{
		sizeOut[0] = readPngU32(&data[0]);
		sizeOut[1] = readPngU32(&data[4]);
	}

	return true;
}

void* CopyPNGAndGetMetadata(const wchar_t* filename, uint32_t sizeOut[2], uint32_t* fileSizeOut)
{
	HANDLE hFile = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return nullptr;

	DWORD fileSize = GetFileSize(hFile, NULL);
	if (fileSize == INVALID_FILE_SIZE)
	{
		CloseHandle(hFile);
		return nullptr;
	}

	HANDLE hMap = CreateFileMappingW(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hMap == NULL)
	{
		CloseHandle(hFile);
		return nullptr;
	}

	void* dataPtr = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, fileSize);
	if (dataPtr == NULL)
	{
		CloseHandle(hMap);
		CloseHandle(hFile);
		return nullptr;
	}

	void* dataCopy = std::malloc(fileSize);
	memcpy(dataCopy, dataPtr, fileSize);

	UnmapViewOfFile(dataPtr);
	dataPtr = nullptr;
	CloseHandle(hMap);
	CloseHandle(hFile);

	if (fileSizeOut != nullptr)
	{
		*fileSizeOut = fileSize;
	}
	bool ret = ReadPNGImageSize((unsigned char*)dataCopy, fileSize, sizeOut);
	if (!ret)
	{
		std::free(dataCopy);
		dataCopy = nullptr;
	}

	return dataCopy;
}

void LunaImage::load(const wchar_t* file)
{
    std::lock_guard<std::mutex> lock(mut);

    clearInternal();

	if ((file == nullptr) || (file[0] == '\0'))
	{
		return;
	}

	bool isPNG = pathIsPNG(file);
	if (isPNG)
	{
		uint32_t imgSize[2];
		uint32_t dataSize;
		void* dataPtr = CopyPNGAndGetMetadata(file, imgSize, &dataSize);
		if (dataPtr != nullptr)
		{
			w = imgSize[0];
			h = imgSize[1];
			if ((w == 0) || (h == 0))
			{
				// Invalid size
				clearInternal();
				return;
			}

			compressedDataPtr = dataPtr;
			compressedDataSize = dataSize;
			totalCompMem += dataSize;
			isPngImage = isPNG;
		}
	}

	if (compressedDataPtr == nullptr)
	{
		FreeImageData bitmapData;
		if (bitmapData.loadFile(file))
		{
			w = bitmapData.getWidth();
			h = bitmapData.getHeight();
			if ((w == 0) || (h == 0))
			{
				// Invalid size
				clearInternal();
				return;
			}
			data = std::malloc(4 * w*h);
			if (data == nullptr)
			{
				// Unable to allocate?
				clearInternal();
				return;
			}
			totalRawMem += 4 * w*h;

			// Convert/Copy image data
			if (!bitmapData.toRawBGRA(data))
			{
				// Unable to convert?
				clearInternal();
				return;
			}
			isPngImage = isPNG;
		}
	}
}

void LunaImage::clearInternal()
{
    if (mask)
    {
        mask = nullptr;
    }
    
	if (hbmp != nullptr)
    {
        // If we hae a hbmp, then it's where data is allocated so we only need
        // to deallocate the hbmp
        ::DeleteObject(hbmp);
        hbmp = nullptr;
        data = nullptr;
    }
    else if (data != nullptr)
    {
        // otherwise if we have data, directly deallocate that
		totalRawMem -= 4 * w*h;
        std::free(data);
        data = nullptr;
    }

	if (compressedDataPtr != nullptr)
	{
		totalCompMem -= compressedDataSize;
		std::free(compressedDataPtr);
		compressedDataPtr = nullptr;
	}
	compressedDataSize = 0;
	mustKeepData = false;
	isPngImage = false;

    w = 0;
    h = 0;
}

HBITMAP LunaImage::asHBITMAP()
{
    std::lock_guard<std::mutex> lock(mut);

    if (hbmp != nullptr) return hbmp;
    if (data == nullptr) return nullptr;

    // Convert this to a HBITMAP
    void* newData = nullptr;
    hbmp = FreeImageHelper::CreateEmptyBitmap(w, h, 32, (void**)&newData);

    // Copy data and replace with the HBITMAP's pointer
    memcpy(newData, data, 4 * w*h);
    std::free(data);
    data = newData;

    return hbmp;
}

bool LunaImage::tryMaskToRGBA()
{
	std::lock_guard<std::mutex> lock(mut);

	auto sdata = (uint8_t*)getDataPtr();
	if (sdata == nullptr) return false;
	if (mask == nullptr) return false;

	std::lock_guard<std::mutex> mlock(mask->mut);
	if ((h != mask->h) || (w != mask->w)) return false;
	auto mdata = (const uint8_t*)mask->getDataPtr();
	if (mdata == nullptr) return false;

    uint32_t byteCount = 4*w*h;
	if (!mask->isPngImage)
	{
		// Handle regular GIF masks

		for (uint32_t idx = 0; idx < byteCount; idx += 4)
		{
			uint32_t mainPix = (
				((uint32_t)(sdata[idx + 0]) << 16) |
				((uint32_t)(sdata[idx + 1]) << 8) |
				((uint32_t)(sdata[idx + 2]) << 0)
				);
			uint32_t maskPix = (
				((uint32_t)(mdata[idx + 0]) << 16) |
				((uint32_t)(mdata[idx + 1]) << 8) |
				((uint32_t)(mdata[idx + 2]) << 0)
				);

			// Transparent
			if ((mainPix == 0x000000) && (maskPix == 0xFFFFFF)) continue;

			// Dark bits in the image, that the mask doesn't mask, are bad
			if (((mainPix ^ 0xFFFFFF) & maskPix) != 0)
			{
				mask->notifyTextureified();
				return false;
			}
		}

		// Set up alpha channel correctly
		for (uint32_t idx = 0; idx < byteCount; idx += 4)
		{
			uint32_t mainPix = (
				((uint32_t)(sdata[idx + 0]) << 16) |
				((uint32_t)(sdata[idx + 1]) << 8) |
				((uint32_t)(sdata[idx + 2]) << 0)
				);
			uint32_t maskPix = (
				((uint32_t)(mdata[idx + 0]) << 16) |
				((uint32_t)(mdata[idx + 1]) << 8) |
				((uint32_t)(mdata[idx + 2]) << 0)
				);

			// Transparent
			if ((mainPix == 0x000000) && (maskPix == 0xFFFFFF))
			{
				sdata[idx + 0] = 0x00;
				sdata[idx + 1] = 0x00;
				sdata[idx + 2] = 0x00;
				sdata[idx + 3] = 0x00;
			}
			else
			{
				sdata[idx + 3] = 0xFF;
			}
		}
	}
	else
	{
		// Handle when the thing to use for a mask is a PNG's alpha channel
		for (uint32_t idx = 0; idx < byteCount; idx += 4)
		{
			if (mdata[idx + 3] == 0x00)
			{
				sdata[idx + 0] = 0x00;
				sdata[idx + 1] = 0x00;
				sdata[idx + 2] = 0x00;
				sdata[idx + 3] = 0x00;
			}
			else
			{
				sdata[idx + 3] = 0xFF;
			}
		}
	}

	// If this somehow had compressed source data, toss it
	if (compressedDataPtr != nullptr)
	{
		totalCompMem -= compressedDataSize;
		std::free(compressedDataPtr);
		compressedDataPtr = nullptr;
	}
	compressedDataSize = 0;

    // Toss out the mask
	mask->notifyTextureified();
    mask = nullptr;

    return true;
}

void LunaImage::drawMasked(int dx, int dy, int w, int h, int sx, int sy, bool drawMask, bool drawMain)
{
    if (g_GLEngine.IsEnabled())
    {
        if (drawMask) {
            auto obj = std::make_shared<GLEngineCmd_DrawSprite>();
            obj->mXDest = dx;
            obj->mYDest = dy;
            obj->mWidthDest = w;
            obj->mHeightDest = h;
            obj->mXSrc = sx;
            obj->mYSrc = sy;
            obj->mWidthSrc = w;
            obj->mHeightSrc = h;

            obj->mImg = mask;
            obj->mOpacity = 1.0f;
            obj->mMode = GLDraw::RENDER_MODE_MULTIPLY; //  GLDraw::RENDER_MODE_AND
            g_GLEngine.QueueCmd(obj);
        }

        if (drawMain)
        {
            auto obj = std::make_shared<GLEngineCmd_DrawSprite>();
            obj->mXDest = dx;
            obj->mYDest = dy;
            obj->mWidthDest = w;
            obj->mHeightDest = h;
            obj->mXSrc = sx;
            obj->mYSrc = sy;
            obj->mWidthSrc = w;
            obj->mHeightSrc = h;

            obj->mImg = shared_from_this();
            obj->mOpacity = 1.0f;
            obj->mMode = GLDraw::RENDER_MODE_MAX; //  GLDraw::RENDER_MODE_OR
            g_GLEngine.QueueCmd(obj);
        }
    }
    else
    {
        // LUNAIMAGE_TODO: Support GDI renderer
    }
}

void LunaImage::drawRGBA(int dx, int dy, int w, int h, int sx, int sy, bool maskOnly, float opacity)
{
    if (g_GLEngine.IsEnabled())
    {
        // LUNAIMAGE_TODO: Implement silhouette drawing of RGBA images for the case of mask-only drawing

        auto obj = std::make_shared<GLEngineCmd_DrawSprite>();
        obj->mXDest = dx;
        obj->mYDest = dy;
        obj->mWidthDest = w;
        obj->mHeightDest = h;
        obj->mXSrc = sx;
        obj->mYSrc = sy;
        obj->mWidthSrc = w;
        obj->mHeightSrc = h;

        obj->mImg = shared_from_this();
        obj->mOpacity = opacity;
        obj->mMode = GLDraw::RENDER_MODE_ALPHA;
        g_GLEngine.QueueCmd(obj);
    }
    else
    {
        // LUNAIMAGE_TODO: Support GDI renderer
    }
}

void LunaImage::draw(int dx, int dy, int w, int h, int sx, int sy, bool drawMask, bool drawMain, float opacity)
{
    if (opacity <= 0.0f) return;
    if ((!drawMask) && (!drawMain)) return;

    if (mask)
    {
        // LUNAIMAGE_TODO: If opacity is not 1.0, do a forced conversion to RGBA
        drawMasked(dx, dy, w, h, sx, sy, drawMask, drawMain);
    }
    else
    {
        drawRGBA(dx, dy, w, h, sx, sy, !drawMain, opacity);
    }
}

LunaImage::~LunaImage()
{
    if (g_GLEngine.IsEnabled())
    {
        g_GLEngine.NotifyTextureDeletion(uid);
    }
    clearInternal();
}

void* LunaImage::getDataPtr() {
	if ((data == nullptr) && (compressedDataPtr != nullptr))
	{
		FreeImageData bitmapData;
		if (bitmapData.loadMem((unsigned char*)compressedDataPtr, compressedDataSize, "c.png"))
		{
			if ((w != bitmapData.getWidth()) || (h != bitmapData.getHeight()))
			{
				// Unexpected size
				clearInternal();
				return nullptr;
			}

			data = std::malloc(4 * w*h);
			if (data == nullptr)
			{
				// Unable to allocate?
				clearInternal();
				return nullptr;
			}
			totalRawMem += 4 * w*h;

			// Convert/Copy image data
			if (!bitmapData.toRawBGRA(data))
			{
				// Unable to convert?
				clearInternal();
				return nullptr;
			}
		}
	}
	return data;
}

uint32_t LunaImage::getDataPtrAsInt()
{
	std::lock_guard<std::mutex> lock(mut);
	mustKeepData = true;
    return (uint32_t)getDataPtr();
}

void LunaImage::notifyTextureified()
{
	if ((compressedDataPtr != nullptr) && (!mustKeepData))
	{
		totalRawMem -= 4 * w*h;
		std::free(data);
		data = nullptr;
	}
}

