#pragma once
const int DDS_BC1_BLOCK_SIZE = 8; // BC1, BC4
const int DDS_BC2_BLOCK_SIZE = 16; // all BCn except BC1, BC4

const int DDS_MAGIC_NUMBER_SIZE = 4;

union DDS_MAGIC_NUMBER {
    unsigned int number;
    unsigned char chars[DDS_MAGIC_NUMBER_SIZE];
};

const DDS_MAGIC_NUMBER DDS_MAGIC_NUMBER_DDS = { 0x20534444 }; // 'DDS '
const DDS_MAGIC_NUMBER DDS_MAGIC_NUMBER_DXT5 = { 0x35545844 }; // 'DTX5'

const DDS_MAGIC_NUMBER DDS_32BIT_MASK_FIRST_8BIT = { 0x000000ff };
const DDS_MAGIC_NUMBER DDS_32BIT_MASK_SECOND_8BIT = { 0x0000ff00 };
const DDS_MAGIC_NUMBER DDS_32BIT_MASK_THIRD_8BIT = { 0x00ff0000 };
const DDS_MAGIC_NUMBER DDS_32BIT_MASK_FOURTH_8BIT = { 0xff000000 };

enum DDSCAPS2_FLAG {
    DDSCAPS2_CUBEMAP = 0x200,
    DDSCAPS2_CUBEMAP_POSITIVEX = 0x400,
    DDSCAPS2_CUBEMAP_NEGATIVEX = 0x800,
    DDSCAPS2_CUBEMAP_POSITIVEY = 0x1000,
    DDSCAPS2_CUBEMAP_NEGATIVEY = 0x2000,
    DDSCAPS2_CUBEMAP_POSITIVEZ = 0x4000,
    DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x8000,
    DDSCAPS2_VOLUME = 0x200000,
};

enum DDSCAPS_FLAG {
    DDSCAPS_COMPLEX = 0x8,
    DDSCAPS_MIPMAP = 0x400000,
    DDSCAPS_TEXTURE = 0x1000,
};

enum DDSD_FLAG {
    DDSD_CAPS = 0x1,
    DDSD_HEIGHT = 0x2,
    DDSD_WIDTH = 0x4,
    DDSD_PITCH = 0x8,
    DDSD_PIXELFORMAT = 0x1000,
    DDSD_MIPMAPCOUNT = 0x20000,
    DDSD_LINEARSIZE = 0x80000,
    DDSD_DEPTH = 0x800000,
};

struct DDS_PIXELFORMAT {
    unsigned long dwSize;
    unsigned long dwFlags;
    unsigned long dwFourCC;
    unsigned long dwRGBBitCount;
    unsigned long dwRBitMask;
    unsigned long dwGBitMask;
    unsigned long dwBBitMask;
    unsigned long dwABitMask;
};

enum DDPF_FLAG {
    DDPF_ALPHAPIXELS = 0x1,
    DDPF_ALPHA = 0x2,
    DDPF_FOURCC = 0x4,
    DDPF_RGB = 0x40,
    DDPF_YUV = 0x200,
    DDPF_LUMINANCE = 0x20000,
};

struct DDS_HEADER {
    unsigned long dwSize;
    unsigned long dwFlags;
    unsigned long dwHeight;
    unsigned long dwWidth;
    unsigned long dwPitchOrLinearSize;
    unsigned long dwDepth;
    unsigned long dwMipMapCount;
    unsigned long dwReserved1[11];
    DDS_PIXELFORMAT ddspf;
    unsigned long dwCaps;
    unsigned long dwCaps2;
    unsigned long dwCaps3;
    unsigned long dwCaps4;
    unsigned long dwReserved2;
};
