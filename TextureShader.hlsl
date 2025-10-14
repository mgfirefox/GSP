struct VertexInput
{
    float4 position : POSITION;
    float2 textureCoordinates : TEXCOORD0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 textureCoordinates : TEXCOORD0;
};

cbuffer MvpBuffer : register(b0)
{
    matrix mvpMatrix;
};

Texture2D diffuseColorTexture : register(t0);
SamplerState samplerState : register(s0);

PixelInput VertexMain(VertexInput input)
{
    input.position.w = 1.0f;

    PixelInput output;
    output.position = mul(input.position, mvpMatrix);
    output.textureCoordinates = input.textureCoordinates;

    return output;
}

float4 PixelMain(PixelInput input) : SV_TARGET
{
    return diffuseColorTexture.Sample(samplerState, input.textureCoordinates);
}
