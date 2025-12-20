// File: MeshEditor/data/shaders/BasicMesh.hlsl
cbuffer CameraCB : register(b0)
{
    float4x4 ViewProj;
    float4x4 World;
    float4 BaseColor;
    uint UseTexture;
    float3 Padding;
};

Texture2D BaseTexture : register(t0);
SamplerState LinearSampler : register(s0);

struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
};

struct PSInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    float4 worldPos = mul(float4(input.Position, 1.0f), World);
    output.Position = mul(worldPos, ViewProj);
    output.Normal = input.Normal;
    output.TexCoord = input.TexCoord;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 color = BaseColor;
    if (UseTexture != 0)
    {
        color *= BaseTexture.Sample(LinearSampler, input.TexCoord);
    }
    return color;
}
