#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <wrl/client.h>

#include <wchar.h>
#include <fstream>

struct MatrixesBuffer {
    DirectX::XMMATRIX modelMatrix;
    DirectX::XMMATRIX viewMatrix;
    DirectX::XMMATRIX projectionMatrix;
};

class Shader {
    bool initialized;
    bool released;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> matrixesBuffer;

public:
    Shader();
    ~Shader();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

    bool setShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, DirectX::XMMATRIX modelMatrix, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix);

public:
    bool initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, HWND windowHandle);
    bool render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, DirectX::XMMATRIX modelMatrix, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix);
    void release();

private:
    bool initializeShader(WCHAR* filename, Microsoft::WRL::ComPtr<ID3D11Device> device, HWND windowHandle);
    void renderShader(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
    void releaseShader();

    void outputShaderErrorMessage(Microsoft::WRL::ComPtr<ID3D10Blob> errorMessage, HWND windowHandle, WCHAR* shaderFilename);
};
