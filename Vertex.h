#pragma once
#include <DirectXMath.h>

#include <functional>

struct Vertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 textureCoordinates;
    DirectX::XMFLOAT3 normal;

    bool operator==(const Vertex& vertex) const;
};

namespace std
{
    template <>
    struct hash<DirectX::XMFLOAT3>
    {
        size_t operator()(const DirectX::XMFLOAT3& float3) const;
    };

    template <>
    struct hash<Vertex>
    {
        size_t operator()(const Vertex& vertex) const;
    };

    inline size_t hash<DirectX::XMFLOAT3>::operator()(const DirectX::XMFLOAT3& float3) const
    {
        return hash<float>()(float3.x) ^ hash<float>()(float3.y) ^ hash<float>()(float3.z);
    }

    inline size_t hash<Vertex>::operator()(const Vertex& vertex) const
    {
        return hash<DirectX::XMFLOAT3>()(vertex.position)
            ^ hash<DirectX::XMFLOAT3>()(vertex.textureCoordinates)
            ^ hash<DirectX::XMFLOAT3>()(vertex.normal);
    }
}
