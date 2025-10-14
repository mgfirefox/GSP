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

cbuffer MaterialBuffer : register(b0)
{
    float3 diffuseColor;
    float opacity;

    bool hasDiffuseColorTexture;
};

SamplerState samplerState : register(s0);
Texture2D diffuseColorTexture : register(t0);

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
    float4 color = 1.0f;

    if (hasDiffuseColorTexture)
    {
        color *= diffuseColorTexture.Sample(samplerState, input.textureCoordinates);
    }
    color *= (diffuseColor, opacity);

    return color;
}
