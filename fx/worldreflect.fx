#include "global.fx"
#include "spotlights.fx"

struct VS_INPUT
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 tex : TEXCOORD;
	float4 color : COLOR;
};

struct PS_INPUT
{
	float4 position : POSITION;
	float3 normal : NORMAL0;
	float3 tangent : TEXCOORD5;
	float2 uv : TEXCOORD0;
	float4 world_pos : TEXCOORD1;
	float3 world_nomral : TEXCOORD2;
	float4 color : COLOR0;
	float4 reflection : TEXCOORD4;
};

texture ReflectedTex : REFLECTEDTEX;
sampler MISCMAP1_SAMPLER = sampler_state
{
	Texture = <ReflectedTex>;
	AddressU = CLAMP;
	AddressV = CLAMP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture MISCMAP5_TEXTURE : FILTERTEXTURE0;
sampler2D MISCMAP5_SAMPLER = sampler_state
{
	texture = MISCMAP5_TEXTURE;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture MISCMAP6_TEXTURE : FILTERTEXTURE1;
sampler2D MISCMAP6_SAMPLER = sampler_state
{
	texture = MISCMAP6_TEXTURE;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

#include "normalmap.fx"

PS_INPUT VS_Base(VS_INPUT IN)
{
	PS_INPUT OUT;

	OUT.position = world_position(IN.position);
	OUT.uv = IN.tex.xy;
	OUT.tangent = normalize(IN.tangent);
	OUT.normal = normalize(IN.normal);
	OUT.world_pos = mul(IN.position, cmWorldMat);
	OUT.world_nomral = normalize(mul(OUT.normal, (float3x3) cmWorldMat));
	
	float4 p = OUT.position;
	p.y = -p.y;
	p.xy += p.w;
	p.xy *= 0.5;
	OUT.reflection = p;
	
	return OUT;
}

void VS_Main(VS_INPUT IN, out PS_INPUT OUT)
{
	OUT = VS_Base(IN);
	OUT.color = float4(0.4, 0.4, 0.4, 1);
}

void VS_LitVertex(VS_INPUT IN, out PS_INPUT OUT)
{
	OUT = VS_Base(IN);
	OUT.color = float4(ApplySpotLights(OUT.world_nomral, OUT.world_pos.xyz, 12), 1);
}

float4 PS_Base(PS_INPUT IN, float3 light)
{
	float3 normal = ApplyNormalMap(IN.normal, IN.tangent, IN.uv);
	
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	float3 albedo = diffuse_tex.rgb;
	
	float puddle_mask = tex2D(MISCMAP5_SAMPLER, IN.world_pos.xy / 20).r;
	albedo = lerp(albedo, albedo / 15, puddle_mask);
	
	float4 reflection_uv = IN.reflection;
	
	float2 rainDrops = tex2D(MISCMAP6_SAMPLER, IN.world_pos.xy).rg * 2 - 1;
	reflection_uv.xy += rainDrops;
	
	reflection_uv.xy += normal.xy * 0.1;
	
	float3 reflection_sample = tex2Dproj(MISCMAP1_SAMPLER, reflection_uv).rgb;
	reflection_sample *= max(puddle_mask, 0.2);
	
	float3 final = albedo;
	final *= light;
	final += reflection_sample;
	
	return float4(final, 1);
}

float4 PS_LitPixel(PS_INPUT IN, int lightCount) : COLOR
{
	float3 light = ApplySpotLights(IN.world_nomral, IN.world_pos.xyz, lightCount);
	light = max(light, float3(0.4, 0.4, 0.4));
	return PS_Base(IN, light);
}

float4 PS_Unlit(PS_INPUT IN) : COLOR
{
	return PS_Base(IN, IN.color.rgb);
}

#include "techniques.fx"
#include "shadowmap.fx"