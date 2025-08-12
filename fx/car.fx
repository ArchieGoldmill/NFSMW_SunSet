#include "global.fx"
#include "shadow.fx"
#include "normalmap.fx"
#include "lighting.fx"
#include "spotlights.fx"
#include "car_rain.fx"
#include "fog.fx"

float4x4 WorldView : WORLDVIEW;

float4 DiffuseMin : DIFFUSEMIN;
float4 DiffuseRange : DIFFUSERANGE;
float4 EnvmapRange : ENVMAPANGE;
float EnvmapPower : ENVMAPPOWER;
float cfMetallicScale;
float SpecularPower : SPECULARPOWER;
float4 SpecularRange : SPECULARRANGE;
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

float4 PS_LitPixel(PS_INPUT IN, uniform int lightCount) : COLOR
{
	float3 normal = normalize(IN.normal);
	
	if (cbUseNormalMap)
	{
		float3 tangent = GetTangent(normal, IN.uv, IN.local_pos.xyz);
		normal = ApplyNormalMap(normal, tangent, IN.uv);
	}
	
	float3 envmapMin, specularMin;
	normal = ApplyRainDrops(IN.local_pos.xyz, normal, envmapMin, specularMin);
	
	float3 view = IN.view;
	float viewLen = length(view);
	float3 nview = normalize(view);
	
	float3 noise_sample = tex3Dbias(MISCMAP1_SAMPLER, float4(IN.local_pos.xyz * 50, -3)).xyz;
	float3 flakeNoise = noise_sample * 2 - 1;
	
	float flake = saturate(viewLen * -0.25 + 1.2);
	flake *= 0.04 * cfMetallicScale;
	
	float3 flake_normal = normalize(normal + flakeNoise * flake);
	
	float vdotn = dot(nview, normal);
	
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	
	float4 diffuse_scale = DiffuseMin + vdotn * DiffuseRange;
	
	SpotLightResult light = ApplySpotLights(flake_normal, IN.local_pos.xyz, lightCount, SpecularPower * 5, IN.spotlight.rgb);
	
	float3 lightDir = normalize(LocalLightVec);
	float ndotl = dot(IN.normal.xyz, lightDir);
	float shadow = DoShadow(IN.shadow_tex, ndotl);
	
	float3 diffuse = ndotl * cvDiffuseColor.rgb;
	float3 specular = GetSpecular(flake_normal, lightDir, nview, SpecularPower);
	
	float3 envmap_sample = texCUBE(ENVIROMAP_SAMPLER, mul(float4(reflect(-nview, normal), 0), WorldView).xyz).rgb;
	float env_vdotn = pow(vdotn, EnvmapPower);
	float3 envmap_scale = envmapMin.rgb + env_vdotn * EnvmapRange.rgb;
	envmap_sample *= envmap_scale * 0.5;
	
	float spec_vdotn = pow(vdotn, SpecularPower);
	float3 spec_scale = specularMin + spec_vdotn * SpecularRange.rgb;
	
	float3 finalLight = IN.color.rgb * (cvAmbientColor.rgb + diffuse * shadow) + light.Diffuse;
	
	float4 final = diffuse_tex;
	final *= diffuse_scale;
	final.rgb *= finalLight;
	final.rgb += lerp(0.0, 0.05, flakeNoise.r * cfMetallicScale) * saturate(1.2 - viewLen * 0.15);
	final.rgb += envmap_sample * diffuse_scale.a;
	final.rgb += specular * shadow * spec_scale;
	final.rgb += light.Specular * spec_scale;
	
	APPLY_ALPHA_EMISSIVE
	APPLY_FOG
	
	return final;
}

#include "techniques.fx"
#include "shadowmap.fx"