#include "Renderer.h"
#include "Application.h"

Renderer::Renderer() : direct3D(), camera(), materialShader(), scene() {
    initialized = false;
    released = false;
}

Renderer::~Renderer() {
    release();
}

bool Renderer::isInitialized() {
    return initialized;
}

void Renderer::setInitialized() {
    initialized = true;
    released = false;
}

bool Renderer::isReleased() {
    return released;
}

void Renderer::setReleased() {
    initialized = false;
    released = true;
}

bool Renderer::initialize(int screenWidth, int screenHeight, HWND windowHandle) {
    if (isInitialized()) {
        release();
    }

    direct3D = std::make_unique<Direct3D>();
    bool result = direct3D->initialize(screenWidth, screenHeight, windowHandle, VSYNC_ENABLED, FULLSCREEN_ENABLED);
    if (!result) {
        MessageBox(windowHandle, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

    /*const UINT inputElementsQuantity = 1;
    D3D11_INPUT_ELEMENT_DESC inputElementsDescs[inputElementsQuantity]{};*/

    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementsDescs;

    D3D11_INPUT_ELEMENT_DESC positionDesc = {};
    positionDesc.SemanticName = "POSITION";
    positionDesc.SemanticIndex = 0;
    positionDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
    positionDesc.InputSlot = 0;
    positionDesc.AlignedByteOffset = 0;
    positionDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    positionDesc.InstanceDataStepRate = 0;
    inputElementsDescs.push_back(positionDesc);

    D3D11_INPUT_ELEMENT_DESC texureCoordinatesDesc = {};
    texureCoordinatesDesc.SemanticName = "TEXCOORD";
    texureCoordinatesDesc.SemanticIndex = 0;
    texureCoordinatesDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
    texureCoordinatesDesc.InputSlot = 0;
    texureCoordinatesDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    texureCoordinatesDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    texureCoordinatesDesc.InstanceDataStepRate = 0;
    inputElementsDescs.push_back(texureCoordinatesDesc);

    materialShader = std::make_unique<Shader>();
    result = materialShader->initialize(direct3D->getDevice(), windowHandle, L"material_shader.hlsl", inputElementsDescs.data(), (UINT)inputElementsDescs.size());
    if (!result) {
        MessageBox(windowHandle, L"Could not initialize the Material Shader", L"Error", MB_OK);
        return false;
    }

    textureShader = std::make_unique<Shader>();
    result = textureShader->initialize(direct3D->getDevice(), windowHandle, L"texture_shader.hlsl", inputElementsDescs.data(), (UINT)inputElementsDescs.size());
    if (!result) {
        MessageBox(windowHandle, L"Could not initialize the Texture Shader", L"Error", MB_OK);
        return false;
    }

    D3D11_BUFFER_DESC mvpConstantBufferDesc = {};
    mvpConstantBufferDesc.ByteWidth = sizeof(MvpBuffer);
    mvpConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    mvpConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    mvpConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    mvpConstantBufferDesc.MiscFlags = 0;
    mvpConstantBufferDesc.StructureByteStride = 0;

    std::shared_ptr<ConstantBuffer<MvpBuffer>> mvpConstantBuffer(new ConstantBuffer<MvpBuffer>(), BaseConstantBuffer::deleter);
    result = mvpConstantBuffer->initialize(direct3D->getDevice(), mvpConstantBufferDesc);
    if (!result) {
        return false;
    }

    materialShader->setVsConstantBuffer(VS_MVP_BUFFER_SLOT, mvpConstantBuffer);
    textureShader->setVsConstantBuffer(VS_MVP_BUFFER_SLOT, mvpConstantBuffer);

    D3D11_BUFFER_DESC materialConstantBufferDesc = {};
    materialConstantBufferDesc.ByteWidth = sizeof(MaterialBuffer);
    materialConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    materialConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    materialConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    materialConstantBufferDesc.MiscFlags = 0;
    materialConstantBufferDesc.StructureByteStride = 0;

    std::shared_ptr<ConstantBuffer<MaterialBuffer>> materialConstantBuffer(new ConstantBuffer<MaterialBuffer>(), BaseConstantBuffer::deleter);
    result = materialConstantBuffer->initialize(direct3D->getDevice(), materialConstantBufferDesc);
    if (!result) {
        return false;
    }

    materialShader->setPsConstantBuffer(PS_MATERIAL_BUFFER_SLOT, materialConstantBuffer);

    D3D11_SAMPLER_DESC samplerStateDesc = {};
    samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerStateDesc.MipLODBias = 0.0f;
    samplerStateDesc.MaxAnisotropy = 1;
    samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

    float* borderColor = samplerStateDesc.BorderColor;
    borderColor[0] = 0.0f;
    borderColor[1] = 0.0f;
    borderColor[2] = 0.0f;
    borderColor[3] = 0.0f;
    borderColor = NULL;
    
    samplerStateDesc.MinLOD = 0;
    samplerStateDesc.MaxLOD = D3D11_FLOAT32_MAX;

    std::shared_ptr<Sampler> sampler(new Sampler, Sampler::deleter);
    sampler->initialize(direct3D->getDevice(), samplerStateDesc);

    materialShader->setPsSampler(PS_SAMPLER_SLOT, sampler);
    textureShader->setPsSampler(PS_SAMPLER_SLOT, sampler);

    scene = std::make_unique<Scene>();
    result = scene->initialize(direct3D->getDevice(), direct3D->getDeviceContext(), "Scene001.scene");
    if (!result) {
        MessageBox(windowHandle, L"Could not initialize the Scene", L"Error", MB_OK);
        return false;
    }

    Transformation cameraTransformation;
    cameraTransformation.position = DirectX::XMFLOAT3(0.0f, 0.0f, -10.0f);
    cameraTransformation.rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

    camera = std::make_unique<Camera>();
    camera->initialize(FIELD_OF_VIEW, (float)screenWidth, (float)screenHeight, PERSPECTIVE_NEAR, PERSPECTIVE_FAR);

    std::unique_ptr<Input>& input = Input::getInput();
    ActionFunction resetCameraRotationFunction = [=](std::string actionName, InputObject inputObject) {
        if (inputObject.inputState == InputState::END) {
            std::unique_ptr<Transformation>& cameraTransformation = camera->getTransformation();
            cameraTransformation->position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
            cameraTransformation->rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

            camera->update();
        }
        };
    int keyCodes[] = { 'Z' };
    input->bindAction(RESET_CAMERA_ACTION_NAME, resetCameraRotationFunction, keyCodes, 1, NULL, 0);

    /*input->test = [=](float x, float y) {
        float cameraPitchRotation = y * cameraRotationSpeed;
        float cameraYawRotation = x * cameraRotationSpeed;

        Transformation cameraTransformation = camera->getTransformation();
        cameraTransformation.rotate(DirectX::XMFLOAT3(cameraPitchRotation, cameraYawRotation, 0.0f));

        DirectX::XMFLOAT3 rotation = cameraTransformation.rotation;
        if (rotation.x > 90.0f) {
            rotation.x = 90.0f;
        }
        else if (rotation.x < -90.0f) {
            rotation.x = -90.0f;
        }
        cameraTransformation.rotation = rotation;

        camera->setTransformation(cameraTransformation);

        camera->update();
        };*/

    ActionFunction cameraRotationFunction = [=](std::string actionName, InputObject inputObject) {
        std::unique_ptr<Timer>& timer = Timer::getTimer();
        float deltaTime = timer->getDeltaTime();

        DirectX::XMINT3 delta = inputObject.delta;

        /*wchar_t c[32];
        swprintf_s(c, 32, L"%d %d", delta.x, delta.y);
        c[31] = '\0';
        OutputDebugString(L"\n\nMouse Moved: ");
        OutputDebugString(c);*/

        float cameraPitchRotation = delta.y * cameraRotationSpeed * deltaTime;
        float cameraYawRotation = delta.x * cameraRotationSpeed * deltaTime;

        std::unique_ptr<Transformation>& cameraTransformation = camera->getTransformation();
        cameraTransformation->rotate(DirectX::XMFLOAT3(cameraPitchRotation, cameraYawRotation, 0.0f));

        DirectX::XMFLOAT3& rotation = cameraTransformation->rotation;
        if (rotation.x > 90.0f) {
            rotation.x = 90.0f;
        }
        else if (rotation.x < -90.0f) {
            rotation.x = -90.0f;
        }

        camera->update();
        };
    InputType inputTypes[] = { InputType::MOUSE_MOVE };
    input->bindAction(CAMERA_ROTATION_ACTION_NAME, cameraRotationFunction, inputTypes, 1, NULL, 0);

    sprite = std::shared_ptr<Sprite>(new Sprite, Sprite::deleter);
    result = sprite->initialize(direct3D->getDevice(), direct3D->getDeviceContext(), "C:/Users/ma2ks/Desktop/ad_image_.dds", DirectX::XMFLOAT2(0.075f, 0.1f));
    if (!result) {
        return false;
    }

    setInitialized();
    return true;
}

void Renderer::updateCamera() {
    std::unique_ptr<Timer>& timer = Timer::getTimer();
    float deltaTime = timer->getDeltaTime();

    std::unique_ptr<Input>& input = Input::getInput();

    float cameraForwardDirection = (int)(input->isKeyDown('W') - input->isKeyDown('S')) * cameraMovementSpeed * deltaTime;
    float cameraRightDirection = (int)(input->isKeyDown('D') - input->isKeyDown('A')) * cameraMovementSpeed * deltaTime;
    float cameraUpDirection = (int)(input->isKeyDown('Q') - input->isKeyDown('E')) * cameraMovementSpeed * deltaTime;

    /*wchar_t c[32];
    swprintf_s(c, 32, L"%f %f %f", cameraForwardDirection, cameraRightDirection, cameraUpDirection);
    c[31] = '\0';
    OutputDebugString(L"\n\nDirections: ");
    OutputDebugString(c);*/

    float cameraPitchKeyboardRotation = (float)(input->isKeyDown(VK_UP) - input->isKeyDown(VK_DOWN));
    float cameraYawKeyboardRotation = (float)(input->isKeyDown(VK_RIGHT) - input->isKeyDown(VK_LEFT));

    float cameraPitchRotation = cameraPitchKeyboardRotation * cameraRotationSpeed * deltaTime;
    float cameraYawRotation = cameraYawKeyboardRotation * cameraRotationSpeed * deltaTime;

    std::unique_ptr<Transformation>& cameraTransformation = camera->getTransformation();
    cameraTransformation->rotate(DirectX::XMFLOAT3(cameraPitchRotation, cameraYawRotation, 0.0f));

    DirectX::XMFLOAT3& cameraRotation = cameraTransformation->rotation;
    if (cameraRotation.x > 90.0f) {
        cameraRotation.x = 90.0f;
    }
    else if (cameraRotation.x < -90.0f) {
        cameraRotation.x = -90.0f;
    }

    cameraTransformation->translate(DirectX::XMFLOAT3(cameraRightDirection, 0.0f, cameraForwardDirection), true);
    cameraTransformation->translate(DirectX::XMFLOAT3(0.0f, cameraUpDirection, 0.0f));

    camera->update();
}

bool Renderer::renderFrame() {
    direct3D->beginScene(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

    DirectX::XMMATRIX viewMatrix = camera->getViewMatrix();
    DirectX::XMMATRIX perspectiveProjectionMatrix = camera->getPerspectiveProjectionMatrix();

    DirectX::XMMATRIX vpMatrix = DirectX::XMMatrixMultiply(viewMatrix, perspectiveProjectionMatrix);

    std::vector<std::shared_ptr<Model>> models = scene->getModels();
    for (const auto& model : models) {
        DirectX::XMMATRIX modelMatrix = model->getTransformation()->getTransformationMatrix();

        MvpBuffer mvpBuffer = {};
        mvpBuffer.mvpMatrix = DirectX::XMMatrixMultiply(modelMatrix, vpMatrix);
        mvpBuffer.mvpMatrix = DirectX::XMMatrixTranspose(mvpBuffer.mvpMatrix);

        bool result = materialShader->setVsConstantBufferData(direct3D->getDeviceContext(), VS_MVP_BUFFER_SLOT, mvpBuffer);
        if (!result) {
            return false;
        }

        result = model->render(direct3D->getDeviceContext(), materialShader);
        if (!result) {
            return false;
        }
    }

    DirectX::XMMATRIX orthographicProjectionMatrix = camera->getOrthographicProjectionMatrix();

    DirectX::XMMATRIX modelMatrix = sprite->getTransformation()->getTransformationMatrix();

    MvpBuffer mvpBuffer = {};
    mvpBuffer.mvpMatrix = DirectX::XMMatrixMultiply(modelMatrix, orthographicProjectionMatrix);
    mvpBuffer.mvpMatrix = DirectX::XMMatrixTranspose(mvpBuffer.mvpMatrix);

    bool result = textureShader->setVsConstantBufferData(direct3D->getDeviceContext(), VS_MVP_BUFFER_SLOT, mvpBuffer);
    if (!result) {
        return false;
    }

    sprite->render(direct3D->getDeviceContext(), textureShader);

    direct3D->endScene();

    return true;
}

void Renderer::release() {
    if (isReleased()) {
        return;
    }

    sprite.reset();

    std::unique_ptr<Input>& input = Input::getInput();
    input->unbindAction(CAMERA_ROTATION_ACTION_NAME);
    input->unbindAction(RESET_CAMERA_ACTION_NAME);
    
    camera->release();
    camera.reset();

    scene->release();
    scene.reset();

    materialShader->release();
    materialShader.reset();
    
    direct3D->release();
    direct3D.reset();

    setReleased();
}

