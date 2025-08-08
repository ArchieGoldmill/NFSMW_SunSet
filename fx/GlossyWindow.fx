#include "global.fx"
#include "shadow.fx"
#include "lighting.fx"
#include "spotlights.fx"
#include "fog.fx"

texture WindowReflection : WINDOWREFLECTION;
sampler reflected_sampler = sampler_state
{
	Texture = <WindowReflection>;
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
	float4 world_pos : TEXCOORD1;
	float3 world_nomral : TEXCOORD2;
	float4 color : COLOR0;
	float4 spotlight : COLOR1;
	float4 shadow_tex : TEXCOORD3;
	float4 local_pos : TEXCOORD4;
};

PS_INPUT VS_Base(VS_INPUT IN)
{
	PS_INPUT OUT;

	OUT.position = world_position(IN.position);
	OUT.shadow_tex = vertex_shadow_tex(IN.position);
	OUT.uv = IN.tex + TextureOffset.xy;
	OUT.tangent = normalize(IN.tangent);
	OUT.normal = normalize(IN.normal);
	OUT.world_pos = mul(IN.position, cmWorldMat);
	OUT.world_nomral = normalize(mul(OUT.normal, (float3x3) cmWorldMat));
	OUT.local_pos = IN.position;
	OUT.local_pos.w = OUT.position.z;
	OUT.color = GetVertexColor(IN.color);
	
	return OUT;
}

float4 PS_LitPixel(PS_INPUT IN, uniform int lightCount) : COLOR
{
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	
	SpotLightResult light = ApplySpotLights(IN.normal, IN.local_pos.xyz, lightCount, 30, IN.spotlight.rgb);
	
	float3 albedo = diffuse_tex.rgb;
	float reflect_scale = smoothstep(0, 0.2, diffuse_tex.a);
	
	float3 lightDir = normalize(LocalLightVec);
	float ndotl = dot(IN.normal, lightDir);
	float shadow = DoShadow(IN.shadow_tex, ndotl);
	
	float3 diffuse = ndotl * cvDiffuseColor.rgb;
	float3 specular = GetSpecular(IN.normal, lightDir, IN.local_pos.xyz, lerp(cvSpecularColor.w, 1, reflect_scale));
	
	float3 finalLight = IN.color.rgb + diffuse * shadow + light.Diffuse;
	finalLight = lerp(finalLight, float3(1, 0.8, 0.6) * 5, reflect_scale * cvAmbientColor.w);
	albedo = lerp(albedo, albedo.rrr, reflect_scale * cvAmbientColor.w);
	
	float3 viewDir = normalize(LocalEyePos.xyz - IN.local_pos.xyz);
	float3 vR = reflect(viewDir, IN.normal);
	
	float2 vCylinderMap;
	vCylinderMap.x = atan2(vR.y, vR.x);
	vCylinderMap.y = IN.uv.y;
	float4 reflection = tex2D(reflected_sampler, vCylinderMap);
	
	float3 final = albedo;
	final.rgb *= finalLight;
	final.rgb += specular * shadow * reflect_scale;
	final.rgb += light.Specular * reflect_scale;
	final.rgb += reflection.rgb * diffuse_tex.a * (1 - cvAmbientColor.w) * shadow;
	
	APPLY_FOG
	
	return float4(final, 1);
}

#include "techniques.fx"
#include "shadowmap.fx"