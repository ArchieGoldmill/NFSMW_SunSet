float4 LocalEyePos : LOCALEYEPOS;
float4x4 WorldView : WORLDVIEW;

#include "global.fx"
#include "shadow.fx"
#include "fog.fx"

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
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
	float4 color : COLOR;
};

struct PS_INPUT
{
	float4 position : POSITION;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
	float4 local_pos : TEXCOORD1;
	float4 depth : TEXCOORD2;
};

void VS_Main(VS_INPUT IN, out PS_INPUT OUT)
{
	OUT.position = clip_pos(IN.position);
	OUT.uv = uv_offset(IN.tex);
	OUT.color = IN.color;
	OUT.local_pos = float4(IN.position.xyz, OUT.position.z);
	
	OUT.depth.xy = OUT.position.xy / OUT.position.w;
	OUT.depth.xy = OUT.depth.xy * 0.5 + 0.5;
	OUT.depth.y = 1.0 - OUT.depth.y;
	
	OUT.depth.z = mul(IN.position, WorldView).z;
}

float4 PS_Main(PS_INPUT IN) : COLOR
{
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	float depth = tex2D(HEIGHTMAP_SAMPLER, IN.depth.xy).r;
	
	float4 final = diffuse_tex * IN.color;
	final.rgb *= 2;
	final.a *= 3;
	
	final.a *= smoothstep(0, 0.3, depth - IN.depth.z);
	
	APPLY_FOG
	
	return final;
}

technique particles<int shader = 1; int shadowLevel = 2;>
{
	pass p0
	{
		COMMON_PASS_BODY

		VertexShader = compile vs_3_0 VS_Main();
		PixelShader = compile ps_3_0 PS_Main();
	}
}