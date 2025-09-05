#include "global.fx"
#include "shadow.fx"
#include "lighting.fx"
#include "spotlights.fx"
#include "fog.fx"
#include "emissive.fx"

float4 cvWindowColor;

texture WindowReflection : WINDOWREFLECTION;
sampler reflected_sampler = sampler_state
{
	Texture = <WindowReflection>;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

struct VS_INPUT
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 tex : TEXCOORD;
	float4 color : COLOR;
};

struct PS_INPUT
{
	float4 position : POSITION;
	float3 normal : NORMAL0;
	float3 tangent : TEXCOORD5;
	float2 uv : TEXCOORD0;
	float3 view : TEXCOORD1;
	float4 color : COLOR0;
	float4 spotlight : COLOR1;
	float4 shadow_tex : TEXCOORD3;
	float4 local_pos : TEXCOORD4;
};

PS_INPUT VS_Base(VS_INPUT IN)
{
	PS_INPUT OUT;
	
	OUT.position = clip_pos(IN.position);
	OUT.shadow_tex = vertex_shadow_tex(IN.position);
	OUT.uv = uv_offset(IN.tex);
	OUT.tangent = IN.tangent;
	OUT.normal = IN.normal;
	OUT.local_pos = float4(IN.position.xyz, OUT.position.z);
	OUT.color = vertex_color(IN.color);
	OUT.view = vertex_view(IN.position.xyz);
	
	return OUT;
}

float3 GetWindowReflection(const float3 nview, const float3 normal, const float2 uv, const float alpha)
{
	float3 vR = reflect(nview, normal);
	
	float2 vCylinderMap;
	vCylinderMap.x = atan2(vR.y, vR.x);
	vCylinderMap.y = uv.y;
	
	return tex2D(reflected_sampler, vCylinderMap).rgb * alpha;
}

float4 PS_LitPixel(PS_INPUT IN, uniform int lightCount) : COLOR
{
	float3 normal = normalize(IN.normal);
	float3 nview = normalize(IN.view);
	
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	
	SpotLightResult light = ApplySpotLights(normal, IN.local_pos.xyz, lightCount, -1, IN.spotlight.rgb);
	
	float3 albedo = diffuse_tex.rgb;
	float reflect_scale = smoothstep(0, 0.2, diffuse_tex.a);
	
	float3 lightDir = normalize(LocalLightVec);
	float ndotl = dot(normal, lightDir);
	float shadow = DoShadow(IN.shadow_tex, ndotl);
	
	float3 diffuse = GetDiffuse(ndotl);
	float3 specular = GetSpecular(normal, lightDir, nview, 10);
	
	float3 windowGlowColor;
	float windowGlowMask;
	if (cvEmissive.a > 0)
	{
		windowGlowMask = tex2D(EMISSIVE_SAMPLER, IN.uv).r;
		windowGlowColor = cvEmissive.rgb;
	}
	else
	{
		windowGlowMask = reflect_scale;
		windowGlowColor = cvWindowColor.rgb;
	}
	
	windowGlowMask *= cvAmbientColor.w;
	
	float3 finalLight = lerp(IN.color.rgb + diffuse * shadow, windowGlowColor, windowGlowMask) + light.Diffuse;
	
	albedo = lerp(albedo, albedo.rrr, windowGlowMask);
	
	float3 reflection = GetWindowReflection(nview, normal, IN.uv, diffuse_tex.a);
	
	float3 final = albedo;
	final.rgb += reflection * (1 - cvAmbientColor.w);
	final.rgb *= finalLight;
	final.rgb += specular * shadow * reflect_scale;
	final.rgb += light.Specular * reflect_scale;
	
	APPLY_FOG
	
	return float4(final, 1);
}

#include "prelit.fx"
#include "prepass.fx"
#include "techniques.fx"
#include "shadowmap.fx"