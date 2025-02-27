#include "Shader.h"

Shader::Shader() : vertexShader(), pixelShader(), inputLayout(), matrixesBuffer() {
    initialized = false;
    released = false;
}

Shader::~Shader() {
    release();
}

bool Shader::isInitialized() {
    return initialized;
}

void Shader::setInitialized() {
    initialized = true;
    released = false;
}

bool Shader::isReleased() {
    return released;
}

void Shader::setReleased() {
    initialized = false;
    released = true;
}

bool Shader::setShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, DirectX::XMMATRIX modelMatrix, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix) {
    modelMatrix = DirectX::XMMatrixTranspose(modelMatrix);
    viewMatrix = DirectX::XMMatrixTranspose(viewMatrix);
    projectionMatrix = DirectX::XMMatrixTranspose(projectionMatrix);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = deviceContext->Map(matrixesBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) {
        return false;
    }

    MatrixesBuffer* data = (MatrixesBuffer*)mappedResource.pData;
    data->modelMatrix = modelMatrix;
    data->viewMatrix = viewMatrix;
    data->projectionMatrix = projectionMatrix;

    deviceContext->Unmap(matrixesBuffer.Get(), 0);
    deviceContext->VSSetConstantBuffers(0, 1, matrixesBuffer.GetAddressOf());

    return true;
}

bool Shader::initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, HWND windowHandle) {
    if (isInitialized()) {
        release();
    }

    wchar_t filename[] = L"shader.hlsl";
    bool result = initializeShader(filename, device, windowHandle);
    if (!result) {
        return false;
    }

    setInitialized();
    return true;
}

bool Shader::render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, DirectX::XMMATRIX modelMatrix, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix) {
    bool result = setShaderParameters(deviceContext, modelMatrix, viewMatrix, projectionMatrix);
    if (!result) {
        return false;
    }

    renderShader(deviceContext);

    return true;
}

void Shader::release() {
    if (isReleased()) {
        return;
    }

    releaseShader();

    setReleased();
}

bool Shader::initializeShader(WCHAR* filename, Microsoft::WRL::ComPtr<ID3D11Device> device, HWND windowHandle) {
    Microsoft::WRL::ComPtr<ID3D10Blob> errorMessage;
    Microsoft::WRL::ComPtr<ID3D10Blob> vertexShaderBuffer;
    HRESULT result = D3DCompileFromFile(filename, NULL, NULL, "VertexMain", "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, vertexShaderBuffer.GetAddressOf(), errorMessage.GetAddressOf());
    if (FAILED(result)) {
        if (errorMessage) {
            outputShaderErrorMessage(errorMessage, windowHandle, filename);
        }
        else {
            MessageBox(windowHandle, filename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    Microsoft::WRL::ComPtr<ID3D10Blob> pixelShaderBuffer;
    result = D3DCompileFromFile(filename, NULL, NULL, "PixelMain", "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, pixelShaderBuffer.GetAddressOf(), errorMessage.GetAddressOf());
    if (FAILED(result)) {
        if (errorMessage) {
            outputShaderErrorMessage(errorMessage, windowHandle, filename);
        }
        else {
            MessageBox(windowHandle, filename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, vertexShader.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }

    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, pixelShader.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }

    const unsigned int elementsQuantity = 1;
    D3D11_INPUT_ELEMENT_DESC elements[elementsQuantity]{};

    D3D11_INPUT_ELEMENT_DESC& position = elements[0];
    position.SemanticName = "POSITION";
    position.SemanticIndex = 0;
    position.Format = DXGI_FORMAT_R32G32B32_FLOAT;
    position.InputSlot = 0;
    position.AlignedByteOffset = 0;
    position.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    position.InstanceDataStepRate = 0;

    /*D3D11_INPUT_ELEMENT_DESC& color = elementsLayout[1];
    color.SemanticName = "COLOR";
    color.SemanticIndex = 0;
    color.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    color.InputSlot = 0;
    color.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    color.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    color.InstanceDataStepRate = 0;*/

    result = device->CreateInputLayout(elements, elementsQuantity, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), inputLayout.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }

    vertexShaderBuffer.Reset();
    pixelShaderBuffer.Reset();

    D3D11_BUFFER_DESC matrixesBufferDesc = {};
    matrixesBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixesBufferDesc.ByteWidth = sizeof(MatrixesBuffer);
    matrixesBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixesBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixesBufferDesc.MiscFlags = 0;
    matrixesBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&matrixesBufferDesc, NULL, matrixesBuffer.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }

    return true;
}

void Shader::renderShader(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext) {
    deviceContext->IASetInputLayout(inputLayout.Get());

    deviceContext->VSSetShader(vertexShader.Get(), NULL, 0);
    deviceContext->PSSetShader(pixelShader.Get(), NULL, 0);
}

void Shader::releaseShader() {
    matrixesBuffer.Reset();
    inputLayout.Reset();

    pixelShader.Reset();
    vertexShader.Reset();
}

void Shader::outputShaderErrorMessage(Microsoft::WRL::ComPtr<ID3D10Blob> errorMessage, HWND windowHandle, WCHAR* shaderFilename) {
    char* compileErrors = (char*)(errorMessage->GetBufferPointer());
    unsigned long long bufferSize = errorMessage->GetBufferSize();

    std::ofstream file;
    file.open("shader-error.txt");
    if (file.is_open()) {
        for (unsigned long long i = 0; i < bufferSize; i++) {
            file << compileErrors[i];
        }
    }
    file.close();

    errorMessage.Reset();

    MessageBox(windowHandle, L"Compiling Shader Error. Check shader-error.txt for more information", shaderFilename, MB_OK);
}
