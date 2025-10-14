#pragma once
#include <DirectXMath.h>

#include "Sound.h"

#include "IntUtility.h"

class Sound3d : public Sound
{
    bool initialized;
    bool released;

    Microsoft::WRL::ComPtr<IDirectSound3DBuffer8> secondaryBuffer3d;

    float maxDistance;

    DirectX::XMFLOAT3 position;

public:
    Sound3d(std::shared_ptr<DirectSound> directSound);
    ~Sound3d() override;

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    Microsoft::WRL::ComPtr<IDirectSound3DBuffer8> getSecondaryBuffer3d();

    bool setMinDistance(float minDistance);

    float getMaxDistance();
    bool setMaxDistance(float maxDistance);

    DirectX::XMFLOAT3 getPosition();
    bool setPosition(DirectX::XMFLOAT3 position);

    bool initialize(std::string filename, bool isLooping = false,
                    float minDistance = DS3D_DEFAULTMINDISTANCE,
                    float maxDistance = DS3D_DEFAULTMAXDISTANCE, int32 volume = DSBVOLUME_MAX,
                    DirectX::XMFLOAT3 position = {});
    bool initialize(SoundData soundData, bool isLooping = false,
                    float minDistance = DS3D_DEFAULTMINDISTANCE,
                    float maxDistance = DS3D_DEFAULTMAXDISTANCE, int32 volume = DSBVOLUME_MAX,
                    DirectX::XMFLOAT3 position = {});
    void release() override;

private:
    bool initializeSecondaryBuffer8(SoundData soundData, bool is3d) override;
    bool initializeSecondaryBuffer3d8();
};
