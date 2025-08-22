#include "global.fx"

float4 LocalEyePos : LOCALEYEPOS;

#include "fog.fx"

struct VS_INPUT
{
	float4 position : POSITION;
	float2 tex : TEXCOORD;
	float4 color : COLOR;
};

struct PS_INPUT
{
	float4 position : POSITION;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
	float4 local_pos : TEXCOORD4;
};

void VS_Prelit(VS_INPUT IN, out PS_INPUT OUT)
{
	OUT.position = mul(IN.position, WorldViewProj);
	OUT.color = IN.color;
	OUT.uv.xy = IN.tex.xy + TextureOffset.xy;
	OUT.local_pos = IN.position;
	OUT.local_pos.w = OUT.position.z;
}

float4 PS_Prelit(PS_INPUT IN) : COLOR
{
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	
	float4 final = diffuse_tex;
	final *= IN.color * 2;
	
	APPLY_FOG
	
	return final;
}

technique worldprelit <int shader = 1; >
{
	pass p0
	{
		COMMON_PASS_BODY

		VertexShader = compile vs_3_0 VS_Prelit();
		PixelShader = compile ps_3_0 PS_Prelit();
	}
}