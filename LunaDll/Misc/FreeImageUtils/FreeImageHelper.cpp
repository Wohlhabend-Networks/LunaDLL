#include "FreeImageHelper.h"


HBITMAP FreeImageHelper::CreateEmptyBitmap(int width, int height, int bpp, void** data)
{
    BITMAPINFO convBMI;
    memset(&convBMI, 0, sizeof(BITMAPINFO));
    convBMI.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    convBMI.bmiHeader.biWidth = width;
    convBMI.bmiHeader.biHeight = -(int)height;
    convBMI.bmiHeader.biPlanes = 1;
    convBMI.bmiHeader.biBitCount = bpp;
    convBMI.bmiHeader.biCompression = BI_RGB;
    HDC screenHDC = GetDC(NULL);
    if (screenHDC == NULL)
    {
        return NULL;
    }
    *data = NULL;
    HBITMAP convHBMP = CreateDIBSection(screenHDC, &convBMI, DIB_RGB_COLORS, data, NULL, 0);

    ReleaseDC(NULL, screenHDC);
    screenHDC = NULL;

    return convHBMP;
}

HBITMAP FreeImageHelper::FromFreeImage(FIBITMAP* bitmap)
{
    int width = FreeImage_GetWidth(bitmap);
    int height = FreeImage_GetHeight(bitmap);
    
    // Convert to 32bit so we can be sure that it is compatible.
    FIBITMAP* frame32bit = FreeImage_ConvertTo32Bits(bitmap);
    // Make it premultiplied so we can use it for opengl
    FreeImage_PreMultiplyWithAlpha(frame32bit);

    // Make HBITMAP handle
    BYTE* out; //BGRA
    HBITMAP outBitmap = FreeImageHelper::CreateEmptyBitmap(width, height, 32, (void**)&out);

    // Copy the bits from out FI bitmap to HBITMAP
    FreeImage_ConvertToRawBits(out, frame32bit, FreeImage_GetPitch(frame32bit), 32, 0x0000FF, 0x00FF00, 0xFF0000, true);

    // Free and return
    FreeImage_Unload(frame32bit);
    return outBitmap;
}
