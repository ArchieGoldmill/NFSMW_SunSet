#include "global.fx"
#include "shadow.fx"
#include "normalmap.fx"
#include "lighting.fx"
#include "spotlights.fx"
#include "car_rain.fx"

float4 LocalLightVec : LOCALLIGHTDIRVEC;
float4x4 WorldView : WORLDVIEW;

float4 DiffuseMin : DIFFUSEMIN;
float4 DiffuseRange : DIFFUSERANGE;
float4 EnvmapRange : ENVMAPANGE;
float EnvmapPower : ENVMAPPOWER;
float cfMetallicScale;

float3 cvAmbientColor;
float3 cvDiffuseColor;

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
	float3 envmapMin, specularMin;
	float3 normal = ApplyRainDrops(IN.local_pos.xyz, normalize(IN.normal), envmapMin, specularMin);
	
	float3 view = LocalEyePos.xyz - IN.local_pos.xyz;
	float3 nview = normalize(view);
	
	float3 noise_sample = tex3Dbias(MISCMAP1_SAMPLER, float4(IN.local_pos.xyz * 50, -3)).xyz;
	float3 flakeNoise = noise_sample * 2 - 1;
	float flake = 1 - smoothstep(0.0, 5.0, length(view));
	flake *= cfMetallicScale * 0.1;
	float3 flake_normal = normalize(normal + flakeNoise * flake);
	
	float vdotn = dot(nview, normal);
	
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	
	float4 diffuse_scale = DiffuseMin + vdotn * DiffuseRange;
	
	SpotLightResult light = ApplySpotLights(flake_normal, IN.local_pos.xyz, lightCount, 150, IN.color.rgb);
	
	float3 lightDir = normalize(LocalLightVec);
	float ndotl = dot(IN.normal, lightDir);
	float shadow = DoShadow(IN.shadow_tex, ndotl);
	
	float3 diffuse = ndotl * cvDiffuseColor.rgb;
	float3 specular = GetSpecular(flake_normal, lightDir, IN.local_pos.xyz);
	
	float3 envmap_sample = texCUBE(ENVIROMAP_SAMPLER, mul(float4(reflect(-nview, normal), 0), WorldView).xyz).rgb;
	float env_vdotn = pow(vdotn, EnvmapPower);
	float3 envmap_scale = envmapMin.rgb + env_vdotn * EnvmapRange.rgb;
	envmap_sample *= envmap_scale * 0.5;
	
	float3 finalLight = cvAmbientColor + diffuse * shadow + light.Diffuse;
	
	float4 final = diffuse_tex;
	final *= diffuse_scale;
	final.rgb *= finalLight;
	final.rgb *= max(0.8, noise_sample.r);
	final.rgb += envmap_sample * diffuse_scale.a;
	final.rgb += specular * shadow;
	final.rgb += light.Specular;
	
	return final;
}

#include "techniques.fx"
#include "shadowmap.fx"