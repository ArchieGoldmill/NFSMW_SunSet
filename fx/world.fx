#include "global.fx"
#include "shadow.fx"
#include "normalmap.fx"
#include "lighting.fx"
#include "spotlights.fx"
#include "fog.fx"
#include "emissive.fx"
#include "hdr.fx"

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
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
	float4 shadow_tex : TEXCOORD3;
	float4 local_pos : TEXCOORD4;
	float3 world_pos : TEXCOORD5;
	float3 world_normal : TEXCOORD6;
};

PS_INPUT VS_Base(VS_INPUT IN)
{
	PS_INPUT OUT;

	OUT.position = clip_pos(IN.position);
	OUT.shadow_tex = vertex_shadow_tex(IN.position);
	OUT.uv = uv_offset(IN.tex);
	OUT.normal = IN.normal;
	OUT.local_pos = float4(IN.position.xyz, OUT.position.z);
	OUT.color = vertex_color(IN.color);
	OUT.world_pos = ToWorldPos(IN.position);
	OUT.world_normal = ToWorldNormal(normalize(IN.normal));
	
	return OUT;
}

float4 PS_LitPixel(PS_INPUT IN, uniform int lightCount) : COLOR
{
	float3 normal = normalize(IN.normal);
	
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	
	SpotLightResult spotlight = ApplySpotLights(normalize(IN.world_normal), IN.world_pos.xyz, lightCount, -1);
	
	float3 lightDir = normalize(LocalLightVec);
	float ndotl = dot(normal, lightDir);
	float shadow = DoShadow(IN.shadow_tex, ndotl);
	
	float3 diffuse = GetDiffuse(ndotl);
	
	float3 finalLight = IN.color.rgb + diffuse * shadow + spotlight.Diffuse;
	
	float4 final = diffuse_tex;
	final.a *= IN.color.a;
	final.rgb *= finalLight;
	final.rgb += GetEmissive(IN.uv);
	
	if (BaseBlendState[4] != 2)
	{
		APPLY_FOG
	}
	
	final.rgb = CompressColourSpace(final.rgb);
	
	return final;
}

#include "techniques.fx"
#include "water.fx"
#include "shadowmap.fx"
#include "prelit.fx"
#include "prepass.fx"