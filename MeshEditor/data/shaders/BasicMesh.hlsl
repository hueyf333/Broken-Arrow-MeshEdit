// File: MeshEditor/data/shaders/BasicMesh.hlsl
cbuffer SceneConstants : register(b0)
{
    float4x4 view;
    float4x4 projection;
    float4x4 model;
    float4 color;
    float4 selectionColor;
    int useTexture;
    float3 padding;
};

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D baseColorTex : register(t0);
SamplerState samp : register(s0);

PSInput VSMain(VSInput input)
{
    PSInput output;
    float4 worldPos = mul(float4(input.position, 1.0f), model);
    output.position = mul(worldPos, view);
    output.position = mul(output.position, projection);
    output.uv = input.uv;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    if (useTexture == 1)
    {
        return baseColorTex.Sample(samp, input.uv);
    }
    return color;
}
