// File: MeshEditor/data/shaders/BasicMesh.hlsl
cbuffer CameraCB : register(b0)
{
    float4x4 uViewProj;
};

cbuffer ObjectCB : register(b1)
{
    float4x4 uWorld;
    float4 uBaseColor;
    uint uHasTexture;
    float3 uPad;
};

Texture2D uTexture : register(t0);
SamplerState uSampler : register(s0);

struct VSInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    float4 worldPos = mul(float4(input.pos, 1.0f), uWorld);
    output.pos = mul(worldPos, uViewProj);
    output.uv = input.uv;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    if (uHasTexture != 0)
    {
        return uTexture.Sample(uSampler, input.uv) * uBaseColor;
    }
    return uBaseColor;
}
