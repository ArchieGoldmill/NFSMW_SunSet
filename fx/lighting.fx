#ifndef LIGHTING_HEADER
#define LIGHTING_HEADER

float4 LocalEyePos : LOCALEYEPOS;

float4 cvSpecularColor;
float4 cvAmbientColor;
float4 cvDiffuseColor;

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

float GetSpecularMap(float2 uv)
{
	return tex2D(SPECULARMAP_SAMPLER, uv).r;
}

float3 GetSpecular(float3 normal, float3 lightDir, float3 view, float power)
{
	float3 reflectDir = normalize(reflect(-lightDir, normal));
	float specularFactor = saturate(dot(reflectDir, view));
	specularFactor = pow(specularFactor, power);
	return specularFactor * cvSpecularColor;
}

float3 GetSpecular(float3 normal, float3 lightDir, float3 view)
{
	return GetSpecular(normal, lightDir, view, cvSpecularColor.w);
}

float3 GetDiffuse(float ndotl)
{
	float halfLambert = saturate(ndotl);
	return halfLambert * cvDiffuseColor.rgb;
}

float4 vertex_color(float4 color)
{
	color.rgb = lerp(0.5, color.rgb * 2, cvDiffuseColor.w) * cvAmbientColor.rgb;
	return color;
}

float3 vertex_view(float3 local_pos)
{
	return LocalEyePos.xyz - local_pos;
}

#endif // LIGHTING_HEADER