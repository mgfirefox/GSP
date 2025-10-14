#include "Renderer.h"

const std::wstring Renderer::materialShaderFilename = L"MaterialShader.hlsl";
const std::wstring Renderer::textureShaderFilename = L"TextureShader.hlsl";

const std::string Renderer::sceneFilename = "Scene001.scene";

const std::string Renderer::spriteTextureFilename = "AdImage.dds";

const float Renderer::perspectiveNear = 0.1f;
const float Renderer::perspectiveFar = 1000.0f;

const float Renderer::orthographicNear = 0.1f;
const float Renderer::orthographicFar = 100.0f;

const float Renderer::fieldOfView = DirectX::XMConvertToRadians(45.0f);

const std::string Renderer::resetCameraActionName = "resetCamera";
const std::string Renderer::cameraRotationActionName = "cameraRotation";

const float Renderer::cameraMovementSpeed = 50.0f;
const float Renderer::keyboardCameraRotationSpeed = 50.0f;
const float Renderer::mouseCameraRotationSpeed = 10.0f;

const std::string Renderer::sound3dFilename = "TestMono.wav";

Renderer::Renderer(std::shared_ptr<Window> window, std::shared_ptr<Input> input,
                   std::shared_ptr<Timer> timer) : direct3d(), materialShader(), scene(), camera(),
                                                   directSound()
{
    initialized = false;
    released = false;

    vsyncEnabled = false;

    this->window = window;

    this->input = input;
    this->timer = timer;
}

Renderer::~Renderer()
{
    release();

    window.reset();

    timer.reset();
    input.reset();
}

bool Renderer::isInitialized()
{
    return initialized;
}

void Renderer::setInitialized()
{
    initialized = true;
    released = false;
}

bool Renderer::isReleased()
{
    return released;
}

void Renderer::setReleased()
{
    initialized = false;
    released = true;
}

bool Renderer::initialize(bool isVsyncEnabled)
{
    if (isInitialized())
    {
        release();
    }

    vsyncEnabled = isVsyncEnabled;

    bool result = initializeDirect3d();
    if (!result)
    {
        return false;
    }

    result = initializeShaders();
    if (!result)
    {
        return false;
    }

    result = initializeScene();
    if (!result)
    {
        return false;
    }

    result = initializeCamera();
    if (!result)
    {
        return false;
    }

    result = initializeSprite();
    if (!result)
    {
        return false;
    }

    result = initializeDirectSound();
    if (!result)
    {
        return false;
    }

    result = initializeSounds();
    if (!result)
    {
        return false;
    }

    /*result = initializeXaudio2();
    if (!result)
    {
        return false;
    }*/

    setInitialized();
    return true;
}

bool Renderer::renderFrame()
{
    direct3d->onFrameStarted(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

    bool result = updateCamera();
    if (!result)
    {
        return false;
    }

    result = updateScene();
    if (!result)
    {
        return false;
    }

    result = updateSprite();
    if (!result)
    {
        return false;
    }

    result = updateListener3d();
    if (!result)
    {
        return false;
    }

    result = updateSounds();
    if (!result)
    {
        return false;
    }

    direct3d->onFrameFinished();

    return true;
}

void Renderer::release()
{
    if (isReleased())
    {
        return;
    }

    xaudio2.reset();

    sound3d.reset();

    directSound.reset();

    sprite.reset();

    camera.reset();

    scene.reset();

    textureShader.reset();
    materialShader.reset();

    direct3d.reset();

    input->unbindAction(cameraRotationActionName);
    input->unbindAction(resetCameraActionName);

    vsyncEnabled = false;

    setReleased();
}

bool Renderer::initializeDirect3d()
{
    direct3d = createSharedPointer<Direct3d>(window);
    bool result = direct3d->initialize(vsyncEnabled);
    if (!result)
    {
        MessageBox(window->getHandle(), L"Could not initialize Direct3D", L"Error", MB_OK);

        return false;
    }

    return true;
}

bool Renderer::initializeShaders()
{
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs(2);

    D3D11_INPUT_ELEMENT_DESC& positionDesc = inputElementDescs[0];
    positionDesc.SemanticName = "POSITION";
    positionDesc.SemanticIndex = 0;
    positionDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
    positionDesc.InputSlot = 0;
    positionDesc.AlignedByteOffset = 0;
    positionDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    positionDesc.InstanceDataStepRate = 0;

    D3D11_INPUT_ELEMENT_DESC& textureCoordinateDesc = inputElementDescs[1];
    textureCoordinateDesc.SemanticName = "TEXCOORD";
    textureCoordinateDesc.SemanticIndex = 0;
    textureCoordinateDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
    textureCoordinateDesc.InputSlot = 0;
    textureCoordinateDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    textureCoordinateDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    textureCoordinateDesc.InstanceDataStepRate = 0;

    materialShader = createSharedPointer<Shader>(direct3d);
    bool result = materialShader->initialize(materialShaderFilename, inputElementDescs.data(),
                                             inputElementDescs.size());
    if (!result)
    {
        MessageBox(window->getHandle(), L"Could not initialize MaterialShader", L"Error", MB_OK);

        return false;
    }

    textureShader = createSharedPointer<Shader>(direct3d);
    result = textureShader->initialize(textureShaderFilename,
                                       inputElementDescs.data(),
                                       inputElementDescs.size());
    if (!result)
    {
        MessageBox(window->getHandle(), L"Could not initialize TextureShader", L"Error", MB_OK);

        return false;
    }

    D3D11_BUFFER_DESC mvpConstantBufferDesc = {};
    mvpConstantBufferDesc.ByteWidth = sizeof(MvpBuffer);
    mvpConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    mvpConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    mvpConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    mvpConstantBufferDesc.MiscFlags = 0;
    mvpConstantBufferDesc.StructureByteStride = 0;

    std::shared_ptr<ConstantBuffer<MvpBuffer>> mvpConstantBuffer = createSharedPointer<
        ConstantBuffer<MvpBuffer>>(direct3d);
    result = mvpConstantBuffer->initialize(mvpConstantBufferDesc);
    if (!result)
    {
        return false;
    }

    result = materialShader->setVertexShaderConstantBuffer(mvpConstantBuffer, VsMvpBufferSlotIndex);
    if (!result)
    {
        return false;
    }

    result = textureShader->setVertexShaderConstantBuffer(mvpConstantBuffer, VsMvpBufferSlotIndex);
    if (!result)
    {
        return false;
    }

    D3D11_BUFFER_DESC materialConstantBufferDesc = {};
    materialConstantBufferDesc.ByteWidth = sizeof(MaterialBuffer);
    materialConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    materialConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    materialConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    materialConstantBufferDesc.MiscFlags = 0;
    materialConstantBufferDesc.StructureByteStride = 0;

    std::shared_ptr<ConstantBuffer<MaterialBuffer>> materialConstantBuffer = createSharedPointer<
        ConstantBuffer<MaterialBuffer>>(direct3d);
    result = materialConstantBuffer->initialize(materialConstantBufferDesc);
    if (!result)
    {
        return false;
    }

    result = materialShader->setPixelShaderConstantBuffer(materialConstantBuffer,
                                                          PsMaterialBufferSlotIndex);
    if (!result)
    {
        return false;
    }

    std::shared_ptr<Sampler> sampler = createSharedPointer<Sampler>(direct3d);
    result = sampler->initialize();
    if (!result)
    {
        return false;
    }

    result = materialShader->setPixelShaderSampler(sampler, PsSamplerSlotIndex);
    if (!result)
    {
        return false;
    }

    result = textureShader->setPixelShaderSampler(sampler, PsSamplerSlotIndex);
    if (!result)
    {
        return false;
    }

    return true;
}

bool Renderer::initializeScene()
{
    scene = createSharedPointer<Scene>(materialShader, direct3d);
    bool result = scene->initialize(sceneFilename);
    if (!result)
    {
        MessageBox(window->getHandle(), L"Could not initialize Scene", L"Error", MB_OK);

        return false;
    }

    return true;
}

bool Renderer::initializeCamera()
{
    camera = createSharedPointer<Camera>();
    bool result = camera->initialize(fieldOfView, static_cast<float>(window->getWidth()),
                                     static_cast<float>(window->getHeight()), perspectiveNear,
                                     perspectiveFar);
    if (!result)
    {
        return false;
    }

    ActionFunction resetCameraRotationFunction = [=
        ](std::string actionName, InputObject inputObject)
    {
        if (inputObject.inputState == InputState::End)
        {
            Transformation cameraTransformation = {};
            cameraTransformation.position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
            cameraTransformation.orientation = DirectX::XMFLOAT3(90.0f, 0.0f, 0.0f);

            camera->setTransformation(cameraTransformation);
        }
    };
    int32 keyCodes[] = {'Z'};

    input->bindAction(resetCameraActionName, resetCameraRotationFunction, keyCodes, 1);

    return true;
}

bool Renderer::initializeSprite()
{
    sprite = createSharedPointer<Sprite>(textureShader, window, direct3d);
    bool result = sprite->initialize(spriteTextureFilename, DirectX::XMFLOAT2(0.075f, 0.1f));
    if (!result)
    {
        MessageBox(window->getHandle(), L"Could not initialize Sprite", L"Error", MB_OK);

        return false;
    }

    return true;
}

bool Renderer::initializeDirectSound()
{
    WAVEFORMATEX waveFormat = {};
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nChannels = 1;
    waveFormat.nSamplesPerSec = 44100;
    waveFormat.wBitsPerSample = 16;
    waveFormat.cbSize = 0;
    
    directSound = createSharedPointer<DirectSound>(window);
    bool result = directSound->initialize(waveFormat);
    if (!result)
    {
        MessageBox(window->getHandle(), L"Could not initialize DirectSound", L"Error", MB_OK);

        return false;
    }

    return true;
}

bool Renderer::initializeSounds()
{
    sound3d = createSharedPointer<Sound3d>(directSound);
    bool result = sound3d->initialize(sound3dFilename, DSBVOLUME_MAX, 10.0f, 100.0f);
    if (!result)
    {
        MessageBox(window->getHandle(), L"Could not initialize 3D Sound", L"Error", MB_OK);
    }

    return true;
}

bool Renderer::initializeXaudio2()
{
    WAVEFORMATEX waveFormat = {};
    waveFormat.nChannels = 2;
    waveFormat.nSamplesPerSec = 44100;
    waveFormat.cbSize = 0;
    
    xaudio2 = createSharedPointer<Xaudio2>();
    bool result = xaudio2->initialize(waveFormat);
    if (!result)
    {
        MessageBox(window->getHandle(), L"Could not initialize XAudio2", L"Error", MB_OK);

        return false;
    }

    return true;
}

bool Renderer::updateCamera()
{
    float deltaTime = timer->getDeltaTime();

    float keyboardCameraPitch = (input->isKeyDown(VK_UP) - input->
        isKeyDown(VK_DOWN)) * keyboardCameraRotationSpeed * deltaTime;
    float keyboardCameraYaw = (input->isKeyDown(VK_RIGHT) - input->
        isKeyDown(VK_LEFT)) * keyboardCameraRotationSpeed * deltaTime;

    DirectX::XMINT2 mouseMovementDelta = input->getMouseDelta().getXy();

    float mouseCameraPitch = mouseMovementDelta.y * mouseCameraRotationSpeed * deltaTime;
    float mouseCameraYaw = mouseMovementDelta.x * mouseCameraRotationSpeed * deltaTime;

    float cameraPitch = keyboardCameraPitch + mouseCameraPitch;
    float cameraYaw = keyboardCameraYaw + mouseCameraYaw;

    Transformation cameraTransformation = camera->getTransformation();
    cameraTransformation.rotate(DirectX::XMFLOAT3(cameraPitch, cameraYaw, 0.0f));

    DirectX::XMFLOAT3& cameraOrientation = cameraTransformation.orientation;
    if (cameraOrientation.x > 90.0f)
    {
        cameraOrientation.x = 90.0f;
    }
    else if (cameraOrientation.x < -90.0f)
    {
        cameraOrientation.x = -90.0f;
    }

    float cameraForwardDirection = (input->isKeyDown('W') - input->isKeyDown('S')) *
        cameraMovementSpeed * deltaTime;
    float cameraRightDirection = (input->isKeyDown('D') - input->isKeyDown('A')) *
        cameraMovementSpeed * deltaTime;
    float cameraUpDirection = (input->isKeyDown('Q') - input->isKeyDown('E')) * cameraMovementSpeed
        * deltaTime;

    cameraTransformation.translate(
        DirectX::XMFLOAT3(cameraRightDirection, 0.0f, cameraForwardDirection), true);
    cameraTransformation.translate(DirectX::XMFLOAT3(0.0f, cameraUpDirection, 0.0f));

    camera->setTransformation(cameraTransformation);

    return true;
}

bool Renderer::updateScene()
{
    DirectX::XMMATRIX viewMatrix = camera->getViewMatrix();
    DirectX::XMMATRIX perspectiveProjectionMatrix = camera->getPerspectiveProjectionMatrix();

    DirectX::XMMATRIX viewProjectionMatrix = DirectX::XMMatrixMultiply(
        viewMatrix, perspectiveProjectionMatrix);

    bool result = scene->render(viewProjectionMatrix);
    if (!result)
    {
        return false;
    }

    return true;
}

bool Renderer::updateSprite()
{
    DirectX::XMMATRIX orthographicProjectionMatrix = camera->getOrthographicProjectionMatrix();

    bool result = sprite->render(orthographicProjectionMatrix);
    if (!result)
    {
        return false;
    }

    return true;
}

bool Renderer::updateListener3d()
{
    Transformation cameraTransformation = camera->getTransformation();

    bool result = directSound->setListener3dPosition(cameraTransformation.position);
    if (!result)
    {
        return false;
    }

    DirectX::XMFLOAT3 forward = {};
    DirectX::XMStoreFloat3(&forward, cameraTransformation.getForward());
    DirectX::XMFLOAT3 up = {};
    DirectX::XMStoreFloat3(&up, cameraTransformation.getUp());

    result = directSound->setListener3dOrientation(forward, up);
    if (!result)
    {
        return false;
    }

    return true;
}

bool Renderer::updateSounds()
{
    DirectX::XMFLOAT3 listener3dPosition = directSound->getListener3dPosition();
    DirectX::XMVECTOR listener3dPositionVector = DirectX::XMLoadFloat3(&listener3dPosition);

    DirectX::XMFLOAT3 sound3dPosition = sound3d->getPosition();
    DirectX::XMVECTOR sound3dPositionVector = DirectX::XMLoadFloat3(&sound3dPosition);

    DirectX::XMVECTOR substraction = DirectX::XMVectorSubtract(
        listener3dPositionVector, sound3dPositionVector);

    DirectX::XMVECTOR sqrDistanceVector = DirectX::XMVector3LengthSq(substraction);
    float sqrDistance = DirectX::XMVectorGetX(sqrDistanceVector);

    float sound3dMaxDistance = sound3d->getMaxDistance();

    /*wchar_t c[32];
    swprintf_s(c, 32, L"%f %f", sqrDistance, sound3dMaxDistance * sound3dMaxDistance);
    c[31] = '\0';
    OutputDebugString(L"\nSqrDistance: ");
    OutputDebugString(c);*/

    if (!sound3d->isPlaying())
    {
        bool result = sound3d->play();
        if (!result)
        {
            return false;
        }
    }

    if (sqrDistance > sound3dMaxDistance * sound3dMaxDistance)
    {
        if (!sound3d->isMuted())
        {
            bool result = sound3d->mute();
            if (!result)
            {
                return false;
            }
        }
    }
    else
    {
        if (sound3d->isMuted())
        {
            bool result = sound3d->unmute();
            if (!result)
            {
                return false;
            }
        }
    }

    return true;
}
