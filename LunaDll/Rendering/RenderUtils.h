#ifndef RenderUtils_hhhh
#define RenderUtils_hhhh

#include <windows.h>
#include <string>
#include <vector>
#include <tuple>

HBITMAP CreateEmptyBitmap(int width, int height, int bpp, void** data);
HBITMAP LoadGfxAsBitmap(const std::wstring& filename);
std::tuple<std::vector<HBITMAP>, int> LoadAnimatedGfx(const std::wstring& filename);

#endif