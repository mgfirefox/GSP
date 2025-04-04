struct VertexInput
{
    float4 position : POSITION;
};

struct PixelInput
{
    float4 position : SV_POSITION;
};

cbuffer MvpBuffer : register(b0)
{
    matrix mvpMatrix;
};

PixelInput VertexMain(VertexInput input)
{
    input.position.w = 1.0f;
    
    PixelInput output;
    output.position = mul(input.position, mvpMatrix);
    
    return output;
}

float4 PixelMain(PixelInput input) : SV_TARGET
{
    return float4(0.82f, 0.82f, 0.82f, 1.0f);
}
