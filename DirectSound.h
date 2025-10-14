#pragma once
#include <dsound.h>
#include <DirectXMath.h>

#include <wrl/client.h>
#include <memory>

#include "Window.h"

#include "IntUtility.h"

class DirectSound
{
    bool initialized;
    bool released;

    std::shared_ptr<Window> window;

    Microsoft::WRL::ComPtr<IDirectSound8> directSound;
    Microsoft::WRL::ComPtr<IDirectSoundBuffer> primaryBuffer;
    Microsoft::WRL::ComPtr<IDirectSound3DListener8> listener3d;

    WAVEFORMATEX waveFormat;

    DirectX::XMFLOAT3 listener3dPosition;

public:
    DirectSound(std::shared_ptr<Window> window);
    ~DirectSound();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    Microsoft::WRL::ComPtr<IDirectSound8> getDirectSound();
    Microsoft::WRL::ComPtr<IDirectSoundBuffer> getPrimaryBuffer();
    Microsoft::WRL::ComPtr<IDirectSound3DListener8> getListener3d();

    DirectX::XMFLOAT3 getListener3dPosition();
    bool setListener3dPosition(DirectX::XMFLOAT3 position);

    bool setListener3dOrientation(DirectX::XMFLOAT3 forward, DirectX::XMFLOAT3 up);

    bool initialize(WAVEFORMATEX waveFormat);
    void release();

    bool createSecondaryBuffer(Microsoft::WRL::ComPtr<IDirectSoundBuffer>& secondaryBuffer,
                               DSBUFFERDESC secondaryBufferDesc, bool is3d);
    bool createSecondaryBuffer8(Microsoft::WRL::ComPtr<IDirectSoundBuffer8>& secondaryBuffer,
                                DSBUFFERDESC secondaryBufferDesc, bool is3d);

private:
    bool initializeDirectSound8();
    bool initializePrimaryBuffer();
    bool initializeListener3d8();
};
