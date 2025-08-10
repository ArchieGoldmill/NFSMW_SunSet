#include "global.fx"
#include "lighting.fx"
#include "fog.fx"

float cfTimeTicker;
float3 cvWaterColor;

texture MISCMAP4_TEXTURE;
sampler2D MISCMAP4_SAMPLER = sampler_state
{
	texture = MISCMAP4_TEXTURE;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture ReflectedTex : REFLECTEDTEX;
sampler REFLECTEDTEX_SAMPLER = sampler_state
{
	Texture = <ReflectedTex>;
	AddressU = CLAMP;
	AddressV = CLAMP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

struct VS_WATER
{
	float4 position : POSITION;
};

struct PS_WATER
{
	float4 position : POSITION;
	float4 uv : TEXCOORD0;
	float4 screen : TEXCOORD1;
	float4 local_pos : TEXCOORD2;
	float3 view : TEXCOORD3;
};

void VS_Water(VS_WATER IN, out PS_WATER OUT)
{
	OUT.position = clip_pos(IN.position);
	OUT.view = vertex_view(IN.position.xyz);
	OUT.local_pos = float4(IN.position.xyz, OUT.position.z);
	
	float3 world_pos = mul(IN.position, cmWorldMat).xyz;
	world_pos.y *= 0.7;
	OUT.uv.xy = float2(world_pos.xy / 100 + float2(1.7, 0.0) * cfTimeTicker / 100);
	
	OUT.uv.zw = float3(world_pos.xy / 150 + float2(-1.7, 2.0) * cfTimeTicker / 150, 1);
	
	float4 p = OUT.position;
	p.y = -p.y;
	OUT.screen = p;
}

float4 PS_Water(PS_WATER IN) : COLOR
{
	float3 component1 = tex2Dbias(MISCMAP4_SAMPLER, float4(IN.uv.xy, 0, -20)).rgb * 2 - 1;
	float3 component2 = tex2Dbias(MISCMAP4_SAMPLER, float4(IN.uv.zw, 0, -20)).rgb * 2 - 1;

	float3 normal = normalize(component1 + component2);
	
	float2 screenUV = IN.screen.xy / IN.screen.w * 0.5 + 0.5;
	float3 reflectionSampleMain = tex2D(REFLECTEDTEX_SAMPLER, saturate(screenUV + normal.rg * 0.003)).rgb;
	float3 reflectionSample = tex2D(REFLECTEDTEX_SAMPLER, saturate(screenUV + normal.rg * 0.1)).rgb;
	
	float3 lightDir = normalize(LocalLightVec);
	float3 specular = GetSpecular(normal, lightDir, normalize(IN.view));
	
	float4 final = float4(reflectionSample * 0.5 + reflectionSampleMain * 0.6, 1);
	final.rgb *= cvWaterColor.rgb;
	final.rgb += specular;
	
	APPLY_FOG
	
	return final;
}

technique Water
{
	pass p0
	{
		COMMON_PASS_BODY

		VertexShader = compile vs_3_0 VS_Water();
		PixelShader = compile ps_3_0 PS_Water();
	}
}