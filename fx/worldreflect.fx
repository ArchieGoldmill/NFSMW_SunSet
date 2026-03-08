#define SPOT_SPECULAR

#include "global.fx"
#include "shadow.fx"
#include "normalmap.fx"
#include "lighting.fx"
#include "spotlights.fx"
#include "fog.fx"
#include "hdr.fx"

float4 cvRainParams;

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
	float3 world_pos : TEXCOORD1;
	float4 color : COLOR0;
	float4 reflection : TEXCOORD4;
	float4 shadow_tex : TEXCOORD3;
	float4 local_pos : TEXCOORD2;
	float3 view : TEXCOORD6;
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
	MIPFILTER = <BaseTextureFilterParam>;
	MINFILTER = <BaseMinTextureFilter>;
	MAGFILTER = <BaseMagTextureFilter>;
};

texture MISCMAP2_TEXTURE;
sampler2D MISCMAP2_SAMPLER = sampler_state
{
	texture = MISCMAP2_TEXTURE;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = <BaseTextureFilterParam>;
	MINFILTER = <BaseMinTextureFilter>;
	MAGFILTER = <BaseMagTextureFilter>;
};

texture MISCMAP3_TEXTURE;
sampler2D MISCMAP3_SAMPLER = sampler_state
{
	texture = MISCMAP3_TEXTURE;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = <BaseTextureFilterParam>;
	MINFILTER = <BaseMinTextureFilter>;
	MAGFILTER = <BaseMagTextureFilter>;
};

PS_INPUT VS_Base(VS_INPUT IN)
{
	PS_INPUT OUT;

	OUT.position = clip_pos(IN.position);
	OUT.shadow_tex = vertex_shadow_tex(IN.position);
	OUT.uv = IN.tex.xy;
	OUT.tangent = IN.tangent;
	OUT.normal = IN.normal;
	OUT.world_pos = ToWorldPos(IN.position);
	OUT.local_pos = float4(IN.position.xyz, OUT.position.z);
	OUT.color = vertex_color(IN.color);
	OUT.view = vertex_view(IN.position.xyz);
	
	float4 p = OUT.position;
	p.y = -p.y;
	p.xy += p.w;
	p.xy *= 0.5;
	OUT.reflection = p;
	
	return OUT;
}

float4 PS_LitPixel(PS_INPUT IN, uniform int lightCount) : COLOR
{
	float3 og_normal = normalize(IN.normal);
	float3 normal = ApplyNormalMap(og_normal, normalize(IN.tangent), IN.uv);
	float3 nview = normalize(IN.view);

	// Apply road detail normal map
	float3 roadDetail = tex2Dbias(MISCMAP3_SAMPLER, float4(IN.world_pos.xy * 0.6, 0, 0)).rgb * 2 - 1;
	float3 flatNormal = float3(0.0, 0.0, 1.0);
	roadDetail = lerp(flatNormal, roadDetail, IN.normal.z);
	float3 bitangent = cross(normal, float3(1, 0, 0));
	float3 tangent = normalize(cross(bitangent, normal));
	float3x3 tbn = float3x3(tangent, cross(normal, tangent), normal);
	normal = normalize(mul(normalize(roadDetail), tbn));
	
	float specMap = GetSpecularMap(IN.uv);

	SpotLightResult light = ApplySpotLights(ToWorldNormal(normal), IN.world_pos.xyz, lightCount, lerp(50, 90, specMap));

	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	float3 albedo = diffuse_tex.rgb + roadDetail.r * 0.2;
	
	float3 lightDir = normalize(LocalLightVec);
	float ndotl = saturate(dot(normal, lightDir));
	float shadow = DoShadow(IN.shadow_tex, saturate(dot(og_normal, lightDir)));
	
	float3 diffuse = GetDiffuse(ndotl);
	float3 specular = GetSpecular(normal, lightDir, nview);
	specular *= saturate(specMap + 0.25);
	
	float puddle_mask = tex2D(MISCMAP1_SAMPLER, IN.world_pos.xy / 20).r;
	puddle_mask = lerp(1, puddle_mask, cvRainParams.w);
	puddle_mask *= cvRainParams.y;
	float reflMin = 1 - smoothstep(0, 0.2, length(cvDiffuseColor.rgb));
	puddle_mask = max(puddle_mask, 0.05 * reflMin);
	
	albedo *= 1 - puddle_mask;
	
	float4 reflection_uv = IN.reflection;
	
	// Distort reflection by rain drops
	float2 rainDrops = tex2D(MISCMAP2_SAMPLER, IN.world_pos.xy).rg - 4.0 / 255.0;
	rainDrops = rainDrops * 2 - 1;
	reflection_uv.xy -= rainDrops * cvRainParams.x;
	
	// Distor reflection by normal map
	reflection_uv.xy += normal.xy * 0.1;
	
	float3 reflection_sample = tex2Dproj(REFLECTEDTEX_SAMPLER, reflection_uv).rgb;
	reflection_sample *= puddle_mask;
	
	// Vertical surfaces should not reflect
	float reflectance = dot(og_normal, float3(0, 0, 1));
	
	// Apply road detail in shadow
	float shadowDetail = saturate(dot(normal, og_normal));
	albedo *= lerp(shadowDetail, 1, shadow);
	
	float3 final = albedo;
	final *= IN.color.rgb + diffuse * shadow + light.Diffuse;
	final += specular * shadow;
	final += light.Specular;
	final += reflection_sample * reflectance;
	
	APPLY_FOG

	final.rgb = CompressColourSpace(final.rgb);
	
	return float4(final, 1);
}

#include "techniques.fx"
#include "shadowmap.fx"
#include "prepass.fx"