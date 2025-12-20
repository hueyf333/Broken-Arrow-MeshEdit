// File: MeshEditor/data/shaders/Grid.hlsl
cbuffer SceneConstants : register(b0)
{
    float4x4 view;
    float4x4 projection;
    float4x4 model;
    float4 color;
};

struct VSInput
{
    float3 position : POSITION;
    float3 color : COLOR0;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 color : COLOR0;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    float4 worldPos = mul(float4(input.position, 1.0f), model);
    output.position = mul(worldPos, view);
    output.position = mul(output.position, projection);
    output.color = input.color;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return float4(input.color, 1.0f);
}
