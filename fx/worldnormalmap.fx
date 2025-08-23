#include "global.fx"
#include "shadow.fx"
#include "normalmap.fx"
#include "lighting.fx"
#include "spotlights.fx"
#include "fog.fx"
#include "emissive.fx"

struct VS_INPUT
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
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
	float4 local_pos : TEXCOORD2;
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

float4 PS_LitPixel(PS_INPUT IN, uniform int lightCount) : COLOR
{
	float3 normal = ApplyNormalMap(normalize(IN.normal), normalize(IN.tangent), IN.uv);
	
	float3 lightDir = normalize(LocalLightVec);
	float ndotl = saturate(dot(normal, lightDir));
	float shadow = DoShadow(IN.shadow_tex, ndotl);
	
	float3 diffuse = GetDiffuse(ndotl);
	float3 specular = GetSpecular(normal, lightDir, normalize(IN.view));
	SpotLightResult spotlight = ApplySpotLights(normal, IN.local_pos.xyz, lightCount, -1, IN.spotlight.rgb);
	
	float3 finalLight = IN.color.rgb + diffuse * shadow + spotlight.Diffuse;
	
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	
	float specMap = GetSpecularMap(IN.uv);
	
	float4 final = diffuse_tex;
	final.a *= IN.color.a;
	final.rgb *= finalLight;
	final.rgb += specular * shadow * specMap;
	final.rgb += spotlight.Specular * specMap;
	final.rgb += GetEmissive(IN.uv);
	
	APPLY_FOG
	
	return final;
}

#include "techniques.fx"
#include "shadowmap.fx"
#include "prelit.fx"
#include "prepass.fx"