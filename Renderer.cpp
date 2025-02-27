#include "Renderer.h"
#include "Application.h"

Renderer::Renderer() : direct3D(), camera(), shader(), scene(), lastMousePosition{} {
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

    Transformation cameraTransformation;
    cameraTransformation.position = DirectX::XMFLOAT3(0.0f, 0.0f, -10.0f);
    cameraTransformation.rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

    camera = std::make_unique<Camera>();
    camera->initialize(FIELD_OF_VIEW, (float)screenWidth, (float)screenHeight, SCREEN_NEAR, SCREEN_FAR, cameraTransformation);

    std::unique_ptr<Input>& input = Input::getInput();
    ActionFunction resetCameraRotationFunction = [=](std::string actionName, InputObject inputObject) {
        if (inputObject.inputState == InputState::END) {
            Transformation cameraTransformation = camera->getTransformation();
            cameraTransformation.rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
            camera->setTransformation(cameraTransformation);

            camera->update();
        }
        };
    int keyCodes[] = { 'Z' };
    input->bindAction(RESET_CAMERA_ROTATION_ACTION_NAME, resetCameraRotationFunction, keyCodes, 1);

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
        DirectX::XMINT3 delta = inputObject.delta;

        wchar_t c[32];
        swprintf_s(c, 32, L"%d %d", delta.x, delta.y);
        c[31] = '\0';
        OutputDebugString(L"\n\nMouse Delta: ");
        OutputDebugString(c);

        float cameraPitchRotation = delta.y * cameraRotationSpeed;
        float cameraYawRotation = delta.x * cameraRotationSpeed;

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
        };
    InputType inputTypes[] = { InputType::MOUSE_MOVE };
    input->bindAction(CAMERA_ROTATION_ACTION_NAME, cameraRotationFunction, inputTypes, 1);

    shader = std::make_unique<Shader>();
    result = shader->initialize(direct3D->getDevice(), windowHandle);
    if (!result) {
        MessageBox(windowHandle, L"Could not initialize the Shader", L"Error", MB_OK);
        return false;
    }

    scene = std::make_unique<Scene>();
    result = scene->initialize(direct3D->getDevice(), "Scene001.scene");
    if (!result) {
        MessageBox(windowHandle, L"Could not initialize the Scene", L"Error", MB_OK);
        return false;
    }

    setInitialized();
    return true;
}

bool Renderer::renderFrame() {
    direct3D->beginScene(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

    std::unique_ptr<Input>& input = Input::getInput();
    float cameraForwardDirection = (int)(input->isKeyDown('W') - input->isKeyDown('S')) * cameraMovementSpeed;
    float cameraRightDirection = (int)(input->isKeyDown('D') - input->isKeyDown('A')) * cameraMovementSpeed;
    float cameraUpDirection = (int)(input->isKeyDown('Q') - input->isKeyDown('E')) * cameraMovementSpeed;

    float cameraPitchKeyboardRotation = (float)(input->isKeyDown(VK_UP) - input->isKeyDown(VK_DOWN));
    float cameraYawKeyboardRotation = (float)(input->isKeyDown(VK_RIGHT) - input->isKeyDown(VK_LEFT));

    /*DirectX::XMINT3 mousePosition = input->getMousePosition();
    DirectX::XMINT3 cameraMouseRotation = DirectX::XMINT3(mousePosition.x - lastMousePosition.x, mousePosition.y - lastMousePosition.y, 0);
    lastMousePosition = mousePosition;*/

    /*float cameraPitchRotation = (cameraMouseRotation.y + cameraPitchKeyboardRotation) * cameraRotationSpeed;
    float cameraYawRotation = (cameraMouseRotation.x + cameraYawKeyboardRotation) * cameraRotationSpeed;*/

    /*wchar_t c[32];
    swprintf_s(c, 32, L"%f %f", cameraPitchRotation, cameraYawRotation);
    c[31] = '\0';
    OutputDebugString(L"\n\nCamera Rotation: ");
    OutputDebugString(c);*/

    float cameraPitchRotation = cameraPitchKeyboardRotation * cameraRotationSpeed;
    float cameraYawRotation = cameraYawKeyboardRotation * cameraRotationSpeed;

    /*wchar_t c[16];
    _itow_s(cameraMouseRotation.x, c, 16, 16);
    c[15] = '\0';
    OutputDebugString(L"\nMouse Delta ");
    OutputDebugString(c);
    OutputDebugString(L"\n");*/

    Transformation cameraTransformation = camera->getTransformation();
    if (cameraPitchRotation != 0.0f || cameraYawRotation != 0.0f) {
        cameraTransformation.rotate(DirectX::XMFLOAT3(cameraPitchRotation, cameraYawRotation, 0.0f));

        DirectX::XMFLOAT3 cameraRotation = cameraTransformation.rotation;
        if (cameraRotation.x > 90.0f) {
            cameraRotation.x = 90.0f;
        }
        else if (cameraRotation.x < -90.0f) {
            cameraRotation.x = -90.0f;
        }
        cameraTransformation.rotation = cameraRotation;
    }
    if (cameraForwardDirection != 0.0f || cameraRightDirection != 0.0f) {
        cameraTransformation.translate(DirectX::XMFLOAT3(cameraRightDirection, 0.0f, cameraForwardDirection), true);
    }
    if (cameraUpDirection != 0.0f) {
        cameraTransformation.translate(DirectX::XMFLOAT3(0.0f, cameraUpDirection, 0.0f));
    }

    /*wchar_t c[32];
    swprintf_s(c, 32, L"%f %f", cameraRotation.x, cameraRotation.y);
    c[31] = '\0';

    OutputDebugString(L"\n\nMouse Rotation: ");
    OutputDebugString(c);*/

    camera->setTransformation(cameraTransformation);

    camera->update();

    DirectX::XMMATRIX viewMatrix = camera->getViewMatrix();
    DirectX::XMMATRIX projectionMatrix = camera->getPerspectiveProjectionMatrix();

    std::vector<std::shared_ptr<Model>> models = scene->getModels();
    for (const auto& model : models) {
        DirectX::XMMATRIX modelMatrix = model->getTransformation().getTransformationMatrix();

        bool result = shader->render(direct3D->getDeviceContext(), modelMatrix, viewMatrix, projectionMatrix);
        if (!result) {
            return false;
        }
        model->render(direct3D->getDeviceContext());
    }

    direct3D->endScene();

    return true;
}

void Renderer::release() {
    if (isReleased()) {
        return;
    }

    scene->release();
    scene.reset();

    shader->release();
    shader.reset();

    camera->release();
    camera.reset();

    direct3D->release();
    direct3D.reset();

    setReleased();
}
