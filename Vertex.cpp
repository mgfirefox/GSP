#include "Vertex.h"

bool Vertex::operator==(const Vertex& vertex) const
{
    return DirectX::XMVector3Equal(DirectX::XMLoadFloat3(&position),
                                   DirectX::XMLoadFloat3(&vertex.position))
        && DirectX::XMVector3Equal(DirectX::XMLoadFloat3(&textureCoordinates),
                                   DirectX::XMLoadFloat3(&vertex.textureCoordinates))
        && DirectX::XMVector3Equal(DirectX::XMLoadFloat3(&normal),
                                   DirectX::XMLoadFloat3(&vertex.normal));
}
