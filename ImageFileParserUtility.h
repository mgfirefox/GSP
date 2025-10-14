#pragma once
#include <d3d11.h>

struct MipmapData
{
    uint32 rowPitch;
    uint32 depthPitch;

    std::vector<unsigned char> data;
};

struct ImageData
{
    uint32 width;
    uint32 height;
    uint32 depth;

    DXGI_FORMAT format;

    uint32 mipmapLevels;
    std::vector<MipmapData> mipmapDataItems;
};
