#include "global.fx"
#include "shadow.fx"
#include "normalmap.fx"
#include "lighting.fx"
#include "spotlights.fx"
#include "fog.fx"

float4 LocalLightVec : LOCALLIGHTDIRVEC;

float3 cvAmbientColor;
float3 cvDiffuseColor;
float2 cvRainParams;

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
	float4 color : COLOR0;
	float4 reflection : TEXCOORD4;
	float4 shadow_tex : TEXCOORD3;
	float4 local_pos : TEXCOORD2;
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

texture MISCMAP1_TEXTURE;
sampler2D MISCMAP1_SAMPLER = sampler_state
{
	texture = MISCMAP1_TEXTURE;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture MISCMAP2_TEXTURE;
sampler2D MISCMAP2_SAMPLER = sampler_state
{
	texture = MISCMAP2_TEXTURE;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture MISCMAP3_TEXTURE;
sampler2D MISCMAP3_SAMPLER = sampler_state
{
	texture = MISCMAP3_TEXTURE;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture SpecularMap : SPECULARMAPTEXTURE;
sampler SPECULARMAP_SAMPLER = sampler_state
{
	Texture = <SpecularMap>;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
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
	OUT.local_pos = IN.position;
	OUT.local_pos.w = OUT.position.z;

	float4 p = OUT.position;
	p.y = -p.y;
	p.xy += p.w;
	p.xy *= 0.5;
	OUT.reflection = p;
	
	return OUT;
}

float4 PS_LitPixel(PS_INPUT IN, int lightCount) : COLOR
{
	float3 original_normal = IN.normal;
	
	float3 normal = ApplyNormalMap(IN.normal, IN.tangent, IN.uv);

	// Apply road detail normal map
	float3 roadDetail = tex2D(MISCMAP3_SAMPLER, IN.world_pos.xy * 0.7).rgb * 2 - 1;
	float3 bitangent = cross(normal, float3(1, 0, 0));
	float3 tangent = normalize(cross(bitangent, normal));
	float3x3 tbn = float3x3(tangent, cross(normal, tangent), normal);
	normal = normalize(mul(normalize(roadDetail), tbn));
	
	float specMap = tex2D(SPECULARMAP_SAMPLER, IN.uv).r * 2;

	SpotLightResult light = ApplySpotLights(normal, IN.local_pos.xyz, lightCount, lerp(10, 60, specMap), IN.color.rgb);

	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	float3 albedo = diffuse_tex.rgb;
	
	float3 lightDir = normalize(LocalLightVec);
	float ndotl = dot(normal, lightDir);
	float shadow = DoShadow(IN.shadow_tex, ndotl);
	
	float3 diffuse = ndotl * cvDiffuseColor.rgb;
	float3 specular = GetSpecular(normal, lightDir, IN.local_pos.xyz);
	specular *= specMap;
	
	float puddle_mask = tex2D(MISCMAP1_SAMPLER, IN.world_pos.xy / 20).r * cvRainParams.y;
	float reflMin = 1 - smoothstep(0, 0.2, length(cvDiffuseColor.rgb));
	puddle_mask = max(puddle_mask, 0.05 * reflMin);
	
	// Make puddles darker so reflection is more visible
	albedo = lerp(albedo, albedo / 15, puddle_mask);
	
	float4 reflection_uv = IN.reflection;
	
	// Distort reflection by rain drops
	float2 rainDrops = tex2D(MISCMAP2_SAMPLER, IN.world_pos.xy).rg * 2 - 1;
	reflection_uv.xy += rainDrops * cvRainParams.x;
	
	// Distor reflection by normal map
	reflection_uv.xy += normal.xy * 0.1;
	
	float3 reflection_sample = tex2Dproj(REFLECTEDTEX_SAMPLER, reflection_uv).rgb;
	reflection_sample *= puddle_mask;
	
	// Vertical surfaces should not reflect
	float reflectance = dot(original_normal, float3(0, 0, 1));
	
	// Apply road detail in shadow
	float shadowDetail = saturate(dot(normal, original_normal));
	albedo *= lerp(shadowDetail, 1, shadow);
	
	float3 final = albedo;
	final *= cvAmbientColor + diffuse * shadow + light.Diffuse;
	final += specular * shadow;
	final += light.Specular;
	final += reflection_sample * reflectance;
	
	APPLY_FOG
	
	return float4(final, 1);
}

#include "techniques.fx"
#include "shadowmap.fx"