#pragma once
#include <d3d11.h>

#include <wrl/client.h>
#include <memory>

#include <vector>

#include <string>

#include "Direct3d.h"

#include "ImageFileParser.h"

#include "ImageFileParserUtility.h"

class Texture
{
    bool initialized;
    bool released;

    std::shared_ptr<Direct3d> direct3d;

    ImageFileParser fileParser;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> buffer;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;

public:
    Texture(std::shared_ptr<Direct3d> direct3d);
    ~Texture();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> getBuffer();
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> getShaderResourceView();

    bool initialize(std::string filename);
    bool initialize(ImageData imageData);
    void release();

private:
    bool readImageData(std::string filename, ImageData& imageData);
    bool initializeBuffer(ImageData imageData, bool& shouldGenerateMipmaps);
    bool initializeShaderResourceView(DXGI_FORMAT format, bool shouldGenerateMipmaps);
};
