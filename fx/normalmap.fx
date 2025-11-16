texture NormalMapTexture : NormalMapTexture;
sampler NORMALMAP_SAMPLER = sampler_state
{
	Texture = <NormalMapTexture>;
	AddressU = <BaseAddressU>;
	AddressV = <BaseAddressV>;
	MIPFILTER = <BaseTextureFilterParam>;
	MINFILTER = <BaseMinTextureFilter>;
	MAGFILTER = <BaseMagTextureFilter>;
};

float3 ApplyNormalMap(float3 normal, float3 tangent, float2 uv, bool flip = false)
{
	float3x3 tbn = { tangent, cross(normal, tangent), normal };
	float3 normalSample = tex2D(NORMALMAP_SAMPLER, uv).rgb;
	
	if (flip)
	{
		normalSample.g = 1 - normalSample.g;
	}
	
	normalSample = normalSample * 2 - 1;
	normal = mul(normalize(normalSample), tbn);
	
	return normal;
}