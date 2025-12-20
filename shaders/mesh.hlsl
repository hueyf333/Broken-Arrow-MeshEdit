cbuffer ObjectConstants : register(b0) {
    float4x4 model;
    float4x4 view;
    float4x4 proj;
    float4 color;
    uint useTexture;
    float3 pad;
};

Texture2D diffuseTexture : register(t0);
SamplerState samplerLinear : register(s0);

struct VSInput {
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

VSOutput VSMain(VSInput input) {
    VSOutput output;
    float4 worldPos = mul(float4(input.position, 1.0f), model);
    output.position = mul(worldPos, view);
    output.position = mul(output.position, proj);
    output.normal = input.normal;
    output.uv = input.uv;
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET {
    float3 lightDir = normalize(float3(0.4f, 0.7f, -0.2f));
    float lighting = saturate(dot(input.normal, lightDir)) * 0.6f + 0.4f;
    float4 base = color;
    if (useTexture == 1) {
        base *= diffuseTexture.Sample(samplerLinear, input.uv);
    }
    return float4(base.rgb * lighting, base.a);
}
