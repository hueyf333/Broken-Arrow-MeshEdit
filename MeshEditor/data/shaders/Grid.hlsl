// File: MeshEditor/data/shaders/Grid.hlsl
cbuffer CameraCB : register(b0)
{
    float4x4 uViewProj;
};

struct VSInput
{
    float3 pos : POSITION;
    float4 color : COLOR0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.pos = mul(float4(input.pos, 1.0f), uViewProj);
    output.color = input.color;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}
