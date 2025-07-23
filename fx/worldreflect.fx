#include "global.fx"
#include "spotlights.fx"
#include "shadow.fx"
#include "normalmap.fx"

float4 LocalLightVec : LOCALLIGHTDIRVEC;
float4 LocalEyePos : LOCALEYEPOS;
float4 DiffuseColour : DIFFUSECOLOUR;
float4 AmbientColour : AMBIENTCOLOUR;
float4 SpecularColour : SPECULARCOLOUR;
float SpecularPower : SPECULARPOWER;

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
};

texture ReflectedTex : REFLECTEDTEX;
sampler MISCMAP1_SAMPLER = sampler_state
{
	Texture = <ReflectedTex>;
	AddressU = CLAMP;
	AddressV = CLAMP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture FilterTexture0 : FILTERTEXTURE0;
sampler2D FILTERTEXTURE0_SAMPLER = sampler_state
{
	texture = FilterTexture0;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture FilterTexture1 : FILTERTEXTURE1;
sampler2D FILTERTEXTURE1_SAMPLER = sampler_state
{
	texture = FilterTexture1;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture FilterTexture2 : FILTERTEXTURE2;
sampler2D FILTERTEXTURE2_SAMPLER = sampler_state
{
	texture = FilterTexture2;
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
	
	float3 roadDetail = tex2D(FILTERTEXTURE2_SAMPLER, IN.world_pos.xy * 0.3).rgb * 2 - 1;
	float3 bitangent = cross(normal, float3(1, 0, 0));
	float3 tangent = normalize(cross(bitangent, normal));
	float3x3 tbn = float3x3(tangent, cross(normal, tangent), normal);
	normal = mul(normalize(roadDetail), tbn);
	
	float3 light = lightCount > 0 ? ApplySpotLights(normal, IN.world_pos.xyz, lightCount) : IN.color.rgb;
	
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	float3 albedo = diffuse_tex.rgb;
	
	float3 lightDir = normalize(LocalLightVec);
	float ndotl = dot(normal, lightDir);
	float shadow = DoShadow(IN.shadow_tex, ndotl);
	
	float3 diffuse = ndotl * cvDiffuseColor.rgb;
	
	float puddle_mask = tex2D(FILTERTEXTURE0_SAMPLER, IN.world_pos.xy / 20).r * cvRainParams.y;
	albedo = lerp(albedo, albedo / 15, puddle_mask);
	
	float4 reflection_uv = IN.reflection;
	
	float2 rainDrops = tex2D(FILTERTEXTURE1_SAMPLER, IN.world_pos.xy).rg * 2 - 1;
	reflection_uv.xy += rainDrops * cvRainParams.x;
	
	reflection_uv.xy += normal.xy * 0.1;
	
	float3 reflection_sample = tex2Dproj(MISCMAP1_SAMPLER, reflection_uv).rgb;
	reflection_sample *= puddle_mask;
	
	float3 finalLight = cvAmbientColor + diffuse * shadow + light;
	
	float reflectance = dot(original_normal, float3(0, 0, 1));
	
	float3 final = albedo;
	final *= finalLight;
	final += reflection_sample * reflectance;
	
	return float4(final, 1);
}

#include "techniques.fx"
#include "shadowmap.fx"