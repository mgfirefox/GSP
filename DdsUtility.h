#pragma once
#include "IntUtility.h"

constexpr int32 DdsBc1BlockSize = 8; // BC1, BC4
constexpr int32 DdsBc2BlockSize = 16; // all BCn except BC1, BC4

constexpr int32 DdsMagicNumberSize = 4;

union DdsMagicNumber
{
    uint32 number;
    unsigned char chars[DdsMagicNumberSize];
};

constexpr DdsMagicNumber DdsMagicNumberDds = {0x20534444}; // 'DDS '
constexpr DdsMagicNumber DdsMagicNumberDxt5 = {0x35545844}; // 'DTX5'

constexpr uint32 Dds32BitMaskFirst8Bit = {0x000000ff};
constexpr uint32 Dds32BitMaskSecond8Bit = {0x0000ff00};
constexpr uint32 Dds32BitMaskThird8Bit = {0x00ff0000};
constexpr uint32 Dds32BitMaskFourth8Bit = {0xff000000};

struct DdsPixelFormat
{
    uint32 size;
    uint32 flags;
    uint32 fourCc;
    uint32 rgbBitCount;
    uint32 rBitMask;
    uint32 gBitMask;
    uint32 bBitMask;
    uint32 aBitMask;
};

enum class DdsCaps : uint32
{
    Complex = 0x8,
    Mipmap = 0x400000,
    Texture = 0x1000,
};

enum class DdsCaps2 : uint32
{
    Cubemap = 0x200,
    CubemapPositiveX = 0x400,
    CubemapNegativeX = 0x800,
    CubemapPositiveY = 0x1000,
    CubemapNegativeY = 0x2000,
    CubemapPositiveZ = 0x4000,
    CubemapNegativeZ = 0x8000,
    Volume = 0x200000,
};

enum class Ddsd : uint32
{
    Caps = 0x1,
    Height = 0x2,
    Width = 0x4,
    Pitch = 0x8,
    PixelFormat = 0x1000,
    MipmapCount = 0x20000,
    LinearSize = 0x80000,
    Depth = 0x800000,
};

enum class Ddpf : uint32
{
    Alphapixels = 0x1,
    Alpha = 0x2,
    FourCc = 0x4,
    Rgb = 0x40,
    Yuv = 0x200,
    Luminance = 0x20000,
};

struct DdsHeader
{
    uint32 size;
    uint32 flags;
    uint32 height;
    uint32 width;
    uint32 pitchOrLinearSize;
    uint32 depth;
    uint32 mipMapCount;
    uint32 reserved1[11];
    DdsPixelFormat pixelFormat;
    uint32 caps;
    uint32 caps2;
    uint32 caps3;
    uint32 caps4;
    uint32 reserved2;
};

inline bool operator==(const DdsMagicNumber& lhs, const DdsMagicNumber& rhs)
{
    return lhs.number == rhs.number;
}

inline bool operator!=(const DdsMagicNumber& lhs, const DdsMagicNumber& rhs)
{
    return !(lhs == rhs);
}
