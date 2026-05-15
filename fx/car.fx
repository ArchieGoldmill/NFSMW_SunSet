#define SPOT_SPECULAR

#include "global.fx"
#include "shadow.fx"
#include "normalmap.fx"
#include "lighting.fx"
#include "spotlights.fx"
#include "car_rain.fx"
#include "fog.fx"
#include "hdr.fx"
#include "emissive.fx"

float4 DiffuseMin : DIFFUSEMIN;
float4 DiffuseRange : DIFFUSERANGE;
float4 EnvmapRange : ENVMAPANGE;
float EnvmapPower : ENVMAPPOWER;
float SpecularPower : SPECULARPOWER;
float4 SpecularRange : SPECULARRANGE;
float4 cvCarEmissive;
float cfMetallicScale;
float cfSpecularHotSpot;
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
	float4 shadow_tex : TEXCOORD3;
	float4 local_pos : TEXCOORD4;
	float3 view : TEXCOORD2;
	float3 world_pos : TEXCOORD5;
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
	OUT.color = lerp(float4(1, 1, 1, 1), saturate(IN.color), cvDiffuseColor.w);
	OUT.view = vertex_view(IN.position.xyz);
	OUT.world_pos = ToWorldPos(IN.position);
	
	return OUT;
}

float3 GetTangent(float3 fpNormal, float2 texcoord, float3 local_pos)
{
	float3 dp1 = ddx(local_pos);
	float3 dp2 = ddy(local_pos);
	
	float2 duv1 = max(abs(ddx(texcoord)), 0.001f);
	float2 duv2 = max(abs(ddy(texcoord)), 0.001f);
	
	float3 dp2perp = cross(dp2, fpNormal);
	float3 dp1perp = cross(fpNormal, dp1);
	
	float3 T = dp2perp * duv1.x + dp1perp * duv2.x;
	float3 B = dp2perp * duv1.y + dp1perp * duv2.y;
	
	return normalize(T * rsqrt(max(dot(T, T), dot(B, B))));
}

float GetFlakeScale(float viewLen)
{
	float flake = saturate(viewLen * -0.25 + 1.2);
	flake *= 0.1 * cfMetallicScale;
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
	
	float3 mapNormal = normal;
	
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
	
	SpotLightResult light = ApplySpotLights(ToWorldNormal(normal), IN.world_pos.xyz, lightCount, SpecularPower * 500, 2);
	
	float3 lightDir = normalize(LocalLightVec);
	float ndotl = saturate(dot(mapNormal, lightDir));
	float shadow = DoShadow(IN.shadow_tex, ndotl);
	
	float3 diffuse = ndotl * cvDiffuseColor.rgb;
	float3 specular = GetSpecular(flake_normal, lightDir, nview, SpecularPower);
	
	float3 hotSpot = GetSpecular(normal, lightDir, nview, 200) * cfSpecularHotSpot * shadow;
	float hotSpotIntensity = dot(hotSpot, 0.333);
	
	float3 envmap_sample = texCUBE(ENVIROMAP_SAMPLER, mul(float4(reflect(-nview, normal), 0), WorldView).xyz).rgb * 1.3;
	envmap_sample = DeCompressColourSpace(envmap_sample);
	
	float3 F0 = float3(0.04, 0.04, 0.04);
	float3 fresnel = F0 + (1.0 - F0) * pow(1.0 - vdotn, 5.0);
	envmap_sample *= clamp(fresnel, 0.3, 1.0);
	
	float env_vdotn = pow(vdotn, EnvmapPower);
	float3 envmapMin = lerp(EnvmapMin.rgb, float3(1.5, 1.5, 1.5), rainPower);
	float3 envmap_scale = envmapMin + env_vdotn * EnvmapRange.rgb;
	envmap_sample *= envmap_scale * 0.5;
	
	float spec_vdotn = pow(vdotn, SpecularPower);
	float3 specularMin = lerp(SpecularMin.xyz, float3(0.0, 0.0, 0.0), rainPower);
	float3 spec_scale = specularMin + spec_vdotn * SpecularRange.rgb;
	spec_scale = lerp(spec_scale, vinyl_scale.rgb, vinyl);
	
	float3 finalLight = IN.color.rgb * (cvAmbientColor.rgb + diffuse * shadow) + light.Diffuse;
	
	float fake_ao = lerp(0.8, 1.1, pow(vdotn, 2));
	
	float4 final = diffuse_tex;
	final *= diffuse_scale;
	final.rgb *= fake_ao;
	final.rgb *= finalLight;
	final.rgb += envmap_sample * diffuse_scale.a;
	final.rgb += specular * spec_scale;
	final.rgb += hotSpot;
	final.rgb += light.Specular * cfSpecularHotSpot;
	final.rgb += GetEmissive(IN.uv, diffuse_tex.rgb);
	final.rgb = lerp(final.rgb, cvCarEmissive.rgb * diffuse_tex.rgb, cvCarEmissive.w);
	final.a += hotSpotIntensity;
	
	APPLY_FOG
	
	final.rgb = CompressColourSpace(final.rgb);
	
	return final;
}

#include "techniques.fx"
#include "shadowmap.fx"
#include "prelit.fx"