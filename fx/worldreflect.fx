#include "global.fx"
#include "spotlights.fx"

struct VS_INPUT
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float4 tex : TEXCOORD;
	float4 color : COLOR;
};

struct PS_INPUT
{
	float4 position : POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
	float4 world_pos : TEXCOORD1;
	float3 world_nomral : TEXCOORD2;
	float4 color : COLOR0;
};

void VS_Main(VS_INPUT IN, out PS_INPUT OUT)
{
	OUT.position = world_position(IN.position);
	OUT.normal = normalize(IN.normal);
	OUT.uv = IN.tex.xy;
	OUT.world_pos = mul(IN.position, cmWorldMat);
	OUT.world_nomral = normalize(mul(OUT.normal, (float3x3) cmWorldMat));
	OUT.color = float4(0, 0, 0, 0);
}

void VS_LitVertex(VS_INPUT IN, out PS_INPUT OUT)
{
	OUT.position = world_position(IN.position);
	OUT.normal = normalize(IN.normal);
	OUT.uv = IN.tex.xy;
	OUT.world_pos = mul(IN.position, cmWorldMat);
	OUT.world_nomral = normalize(mul(OUT.normal, (float3x3) cmWorldMat));
	OUT.color = float4(ApplySpotLights(OUT.world_nomral, OUT.world_pos.xyz, 12), 1);
}

float4 PS_LitPixel(PS_INPUT IN, int lightCount) : COLOR
{
	float4 tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	float3 light = ApplySpotLights(IN.world_nomral, IN.world_pos.xyz, lightCount);
	light = max(light, float3(0.4, 0.4, 0.4));
	tex.rgb *= light;
	
	return tex;
}

float4 PS_Unlit(PS_INPUT IN) : COLOR
{
	float4 tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	float3 light = max(IN.color.rgb, float3(0.4, 0.4, 0.4));
	tex.rgb *= light;
	
	return tex;
}

#include "techniques.fx"
#include "shadowmap.fx"