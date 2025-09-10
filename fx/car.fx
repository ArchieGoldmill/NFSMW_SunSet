#define SPOT_SPECULAR

#include "global.fx"
#include "shadow.fx"
#include "normalmap.fx"
#include "lighting.fx"
#include "spotlights.fx"
#include "car_rain.fx"
#include "fog.fx"
#include "hdr.fx"

float4x4 WorldView : WORLDVIEW;

float4 DiffuseMin : DIFFUSEMIN;
float4 DiffuseRange : DIFFUSERANGE;
float4 EnvmapRange : ENVMAPANGE;
float EnvmapPower : ENVMAPPOWER;
float SpecularPower : SPECULARPOWER;
float4 SpecularRange : SPECULARRANGE;
float4 cvCarEmissive;
float cfMetallicScale;
float cfVinylScale;
bool cbUseNormalMap;

texture EnvMapTex : EnvMapTexture;
samplerCUBE ENVIROMAP_SAMPLER = sampler_state
{
	texture = <EnvMapTex>;
	AddressU = MIRROR;
	AddressV = MIRROR;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture MISCMAP1_TEXTURE;
sampler3D MISCMAP1_SAMPLER = sampler_state
{
	texture = <MISCMAP1_TEXTURE>;
	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;
	MIPFILTER = POINT;
	MINFILTER = POINT;
	MAGFILTER = POINT;
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
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
	float4 spotlight : COLOR1;
	float4 shadow_tex : TEXCOORD3;
	float4 local_pos : TEXCOORD4;
	float3 view : TEXCOORD2;
};

PS_INPUT VS_Base(VS_INPUT IN)
{
	PS_INPUT OUT;

	OUT.position = clip_pos(IN.position);
	OUT.shadow_tex = vertex_shadow_tex(IN.position);
	OUT.uv = IN.tex.xy;
	OUT.normal.xyz = normalize(IN.normal);
	OUT.local_pos = IN.position;
	OUT.local_pos.w = OUT.position.z;
	OUT.color = saturate(IN.color);
	OUT.view = vertex_view(IN.position.xyz);
	
	return OUT;
}

float3 GetTangent(float3 normal, float2 uv, float3 local_pos)
{
	float3 dpdx = ddx(local_pos);
	float3 dpdy = ddy(local_pos);
	float2 duvdx = ddx(uv);
	float2 duvdy = ddy(uv);

	float r = 1.0 / (duvdx.x * duvdy.y - duvdx.y * duvdy.x);
	float3 tangent = normalize((dpdx * duvdy.y - dpdy * duvdx.y) * r);

	return normalize(tangent - normal * dot(normal, tangent));
}

float GetFlakeScale(float viewLen)
{
	float flake = saturate(viewLen * -0.25 + 1.2);
	flake *= 0.04 * cfMetallicScale;
	return flake;
}

float4 PS_LitPixel(PS_INPUT IN, uniform int lightCount) : COLOR
{
	float3 normal = normalize(IN.normal);
	
	if (cbUseNormalMap)
	{
		float3 tangent = GetTangent(normal, IN.uv, IN.local_pos.xyz);
		normal = ApplyNormalMap(normal, tangent, IN.uv);
	}
	
	float rainPower;
	normal = ApplyRainDrops(IN.local_pos.xyz, normal, rainPower);
	
	float viewLen = length(IN.view);
	float3 nview = normalize(IN.view);
	
	float3 noise_sample = tex3Dbias(MISCMAP1_SAMPLER, float4(IN.local_pos.xyz * 50, -3)).xyz;
	float3 flakeNoise = noise_sample * 2 - 1;
	
	float flake = GetFlakeScale(viewLen);
	
	float3 flake_normal = normalize(normal + flakeNoise * flake);
	
	float vdotn = saturate(dot(nview, normal));
	
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	
	float vinyl = cfVinylScale * diffuse_tex.a;
	flake_normal = lerp(flake_normal, normal, vinyl);
	
	float4 diffuse_scale = DiffuseMin + vdotn * DiffuseRange;
	float4 vinyl_scale = float4(0.5, 0.5, 0.5, 0.5) + vdotn * float4(0.5, 0.5, 0.5, 0.5);
	diffuse_scale = lerp(diffuse_scale, vinyl_scale, vinyl);
	
	SpotLightResult light = ApplySpotLights(flake_normal, IN.local_pos.xyz, lightCount, SpecularPower * 100, IN.spotlight.rgb, 2.0);
	
	float3 lightDir = normalize(LocalLightVec);
	float ndotl = dot(IN.normal.xyz, lightDir);
	float shadow = DoShadow(IN.shadow_tex, ndotl);
	
	float3 diffuse = ndotl * cvDiffuseColor.rgb;
	float3 specular = GetSpecular(flake_normal, lightDir, nview, SpecularPower);
	
	float3 envmap_sample = texCUBE(ENVIROMAP_SAMPLER, mul(float4(reflect(-nview, normal), 0), WorldView).xyz).rgb;
	envmap_sample = DeCompressColourSpace(envmap_sample);
	float env_vdotn = pow(vdotn, EnvmapPower);
	float3 envmapMin = lerp(EnvmapMin.xyz, float3(1.5, 1.5, 1.5), rainPower);
	float3 envmap_scale = envmapMin + env_vdotn * EnvmapRange.rgb;
	envmap_sample *= envmap_scale * 0.5;
	
	float spec_vdotn = pow(vdotn, SpecularPower);
	float3 specularMin = lerp(SpecularMin.xyz, float3(0.0, 0.0, 0.0), rainPower);
	float3 spec_scale = specularMin + spec_vdotn * SpecularRange.rgb;
	spec_scale = lerp(spec_scale, vinyl_scale.rgb, vinyl);
	
	float3 finalLight = IN.color.rgb * (cvAmbientColor.rgb + diffuse * shadow) + light.Diffuse;
	
	float metallic = 0.05 * flakeNoise.r * cfMetallicScale * saturate(1.2 - viewLen * 0.15) * (1 - vinyl);
	
	float4 final = diffuse_tex;
	final *= diffuse_scale;
	final.rgb *= finalLight;
	final.rgb += metallic;
	final.rgb += envmap_sample * diffuse_scale.a;
	final.rgb += specular * shadow * spec_scale;
	final.rgb += light.Specular * spec_scale;
	final.rgb = lerp(final.rgb, cvCarEmissive.rgb * diffuse_tex.rgb, cvCarEmissive.w);
	
	APPLY_FOG
	
	final.rgb = CompressColourSpace(final.rgb);
	
	return final;
}

#include "techniques.fx"
#include "shadowmap.fx"