#include "global.fx"
#include "shadow.fx"
#include "normalmap.fx"
#include "lighting.fx"
#include "spotlights.fx"

float4 LocalLightVec : LOCALLIGHTDIRVEC;

float3 cvAmbientColor;
float3 cvDiffuseColor;

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
	float4 shadow_tex : TEXCOORD3;
	float4 local_pos : TEXCOORD4;
};

PS_INPUT VS_Base(VS_INPUT IN)
{
	PS_INPUT OUT;

	OUT.position = world_position(IN.position);
	OUT.shadow_tex = vertex_shadow_tex(IN.position);
	OUT.uv = IN.tex.xy;
	OUT.tangent = normalize(IN.tangent);
	OUT.normal = normalize(IN.normal);
	OUT.world_pos = mul(IN.position, cmWorldMat);
	OUT.world_nomral = normalize(mul(OUT.normal, (float3x3) cmWorldMat));
	OUT.local_pos = IN.position;
	
	return OUT;
}

float4 PS_LitPixel(PS_INPUT IN, int lightCount) : COLOR
{
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	
	float3 light = lightCount > 0 ? ApplySpotLights(IN.normal, IN.local_pos.xyz, lightCount, -1) : IN.color;
	
	float3 lightDir = normalize(LocalLightVec);
	float ndotl = dot(IN.normal, lightDir);
	float shadow = DoShadow(IN.shadow_tex, ndotl);
	
	float3 diffuse = ndotl * cvDiffuseColor.rgb;
	float3 specular = GetSpecular(IN.normal, lightDir, IN.local_pos.xyz);
	
	float3 finalLight = cvAmbientColor + (diffuse + specular) * shadow + light;
	
	float4 final = diffuse_tex;
	final.rgb *= finalLight;
	
	return final;
}

#include "techniques.fx"
#include "shadowmap.fx"