float4 cvEmissive;

texture EMISSIVE_TEXTURE;
sampler2D EMISSIVE_SAMPLER = sampler_state
{
	texture = EMISSIVE_TEXTURE;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = <BaseTextureFilterParam>;
	MINFILTER = <BaseMinTextureFilter>;
	MAGFILTER = <BaseMagTextureFilter>;
};

float3 GetEmissive(float2 uv)
{
	if (cvEmissive.a > 0)
	{
		float3 mask = tex2D(EMISSIVE_SAMPLER, uv).rgb;
		return cvEmissive.rgb * mask;
	}
	
	return float3(0, 0, 0);
}