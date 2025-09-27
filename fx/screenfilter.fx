#include "hdr.fx"

float cfDepth : FILTERBLEND;

texture diffusemap : DiffuseMap;
sampler DIFFUSEMAP_SAMPLER = sampler_state
{
	texture = <diffusemap>;
	AddressU = CLAMP;
	AddressV = CLAMP;
	MIPFILTER = NONE;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture HEIGHTMAP_TEXTURE : HeightMapTexture;
sampler HEIGHTMAP_SAMPLER = sampler_state
{
	Texture = <HEIGHTMAP_TEXTURE>;
	AddressU = CLAMP;
	AddressV = CLAMP;
	MIPFILTER = NONE;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

struct VS_INPUT
{
	float4 position : POSITION;
	float2 tex : TEXCOORD;
};

struct PS_INPUT
{
	float4 position : POSITION;
	float2 uv : TEXCOORD0;
};

void VS_ScreenFilter(VS_INPUT IN, out PS_INPUT OUT)
{
	OUT.position = IN.position;
	OUT.uv = IN.tex;
}

float4 PS_Depth(PS_INPUT IN) : COLOR
{
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	
	float depth = tex2D(HEIGHTMAP_SAMPLER, IN.uv).r;
	diffuse_tex.a = smoothstep(cfDepth, 0, depth);
	
	return diffuse_tex;
}

technique Depth <int shader = 1; >
{
	pass p0
	{
		VertexShader = compile vs_3_0 VS_ScreenFilter();
		PixelShader = compile ps_3_0 PS_Depth();
	}
}

#include "godrays.fx"
#include "bloom.fx"
#include "blur.fx"