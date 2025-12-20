// File: MeshEditor/data/shaders/Grid.hlsl
cbuffer GridCB : register(b0)
{
    float4x4 ViewProj;
    float4 GridColor;
};

struct VSInput
{
    float3 Position : POSITION;
};

struct PSInput
{
    float4 Position : SV_POSITION;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.Position = mul(float4(input.Position, 1.0f), ViewProj);
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return GridColor;
}
