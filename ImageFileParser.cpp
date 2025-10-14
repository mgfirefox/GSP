#include "ImageFileParser.h"

bool ImageFileParser::parseFile(std::string filename, ImageData& imageData)
{
    std::string format = getFileFormat(filename);
    if (format == "dds")
    {
        return parseDdsFile(filename, imageData);
    }

    return false;
}

bool ImageFileParser::parseDdsFile(std::string filename, ImageData& imageData)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }

    DdsMagicNumber magicNumber = {};
    file.read(reinterpret_cast<char*>(&magicNumber), DdsMagicNumberSize);
    if (!file || file.gcount() != DdsMagicNumberSize)
    {
        return false;
    }
    if (magicNumber != DdsMagicNumberDds)
    {
        return false;
    }

    DdsHeader header = {};
    file.read(reinterpret_cast<char*>(&header), sizeof(DdsHeader));
    if (!file || file.gcount() != sizeof(DdsHeader))
    {
        return false;
    }
    if (header.size != sizeof(DdsHeader))
    {
        return false;
    }
    if (!(header.flags & (static_cast<uint32>(Ddsd::Width) | static_cast<uint32>(Ddsd::Height) |
        static_cast<uint32>(Ddsd::PixelFormat))))
    {
        return false;
    }

    if (header.width == 0 || header.height == 0)
    {
        return false;
    }
    imageData.width = header.width;
    imageData.height = header.height;

    if (header.flags & static_cast<uint32>(Ddsd::MipmapCount))
    {
        imageData.mipmapLevels = header.mipMapCount;
    }
    else
    {
        imageData.mipmapLevels = 1;
    }

    imageData.mipmapDataItems = std::vector<MipmapData>(imageData.mipmapLevels);

    if (header.flags & static_cast<uint32>(Ddsd::Depth))
    {
        if (header.width == 0)
        {
            return false;
        }
        imageData.depth = header.depth;
    }
    else
    {
        imageData.depth = 1;
    }

    DdsPixelFormat& pixelFormat = header.pixelFormat;
    if (pixelFormat.size != sizeof(DdsPixelFormat))
    {
        return false;
    }
    if (pixelFormat.flags & (static_cast<uint32>(Ddpf::Rgb) | static_cast<uint32>(Ddpf::Alpha)))
    {
        if (pixelFormat.rgbBitCount == 32)
        {
            if (pixelFormat.rBitMask & Dds32BitMaskFirst8Bit && pixelFormat.gBitMask
                & Dds32BitMaskSecond8Bit
                && pixelFormat.bBitMask & Dds32BitMaskThird8Bit && pixelFormat.
                aBitMask & Dds32BitMaskFourth8Bit)
            {
                imageData.format = DXGI_FORMAT_R8G8B8A8_UNORM;

                for (uint32 i = 0, width = imageData.width, height = imageData.height; i < imageData
                     .mipmapLevels; i++, width /= 2, height /= 2)
                {
                    MipmapData& mipmapData = imageData.mipmapDataItems[i];
                    mipmapData.rowPitch = (width * pixelFormat.rgbBitCount + 7) / 8;
                    mipmapData.depthPitch = mipmapData.rowPitch * height;

                    mipmapData.data = std::vector<unsigned char>(mipmapData.depthPitch);
                    for (uint32 j = 0; j < height; j++)
                    {
                        file.read(
                            reinterpret_cast<char*>(mipmapData.data.data() + j * mipmapData.
                                rowPitch),
                            mipmapData.rowPitch);
                        if (!file || file.gcount() != mipmapData.rowPitch)
                        {
                            return false;
                        }
                    }

                    imageData.mipmapDataItems.push_back(mipmapData);

                    if (width == 1 || height == 1)
                    {
                        break;
                    }
                }
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else if (pixelFormat.flags & static_cast<uint32>(Ddpf::FourCc))
    {
        if (header.pitchOrLinearSize == 0)
        {
            return false;
        }

        DdsMagicNumber magicNumber1 = {pixelFormat.fourCc};
        if (magicNumber1 == DdsMagicNumberDxt5)
        {
            imageData.format = DXGI_FORMAT_BC3_UNORM;

            bool isBlockHeightInitialized = false;
            for (uint32 i = 0, width = imageData.width, blockHeight = 0; i < imageData.mipmapLevels;
                 i++, width /= 2, blockHeight /= 2)
            {
                MipmapData& mipmapData = imageData.mipmapDataItems[i];
                mipmapData.rowPitch = (width + 3) / 4 * DdsBc2BlockSize;

                if (isBlockHeightInitialized)
                {
                    if (blockHeight == 0)
                    {
                        blockHeight = 1;
                    }

                    mipmapData.depthPitch = mipmapData.rowPitch * blockHeight;
                }
                else
                {
                    mipmapData.depthPitch = header.pitchOrLinearSize;

                    blockHeight = mipmapData.depthPitch / mipmapData.rowPitch;
                    isBlockHeightInitialized = true;
                }

                mipmapData.data = std::vector<unsigned char>(mipmapData.depthPitch);
                for (uint32 j = 0; j < blockHeight; j++)
                {
                    file.read(
                        reinterpret_cast<char*>(mipmapData.data.data() + j * mipmapData.rowPitch),
                        mipmapData.rowPitch);
                    if (!file || file.gcount() != mipmapData.rowPitch)
                    {
                        return false;
                    }
                }

                imageData.mipmapDataItems.push_back(mipmapData);

                if (width == 1)
                {
                    break;
                }
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    file.close();

    return true;
}
