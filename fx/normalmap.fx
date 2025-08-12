texture NormalMapTexture : NormalMapTexture;
sampler NORMALMAP_SAMPLER = sampler_state
{
	Texture = <NormalMapTexture>;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = <BaseTextureFilterParam>;
	MINFILTER = <BaseMinTextureFilter>;
	MAGFILTER = <BaseMagTextureFilter>;
};

float3 ApplyNormalMap(float3 normal, float3 tangent, float2 uv)
{
	float3x3 tbn = { tangent, cross(normal, tangent), normal };
	float3 normalSample = tex2D(NORMALMAP_SAMPLER, uv).rgb * 2 - 1;
	normal = mul(normalize(normalSample), tbn);
	
	return normal;
}