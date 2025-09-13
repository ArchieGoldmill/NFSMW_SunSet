#include "hdr.fx"

float4 cvBloomParams;

texture FilterTexture0 : FILTERTEXTURE0;
sampler FILTERTEXTURE0_SAMPLER = sampler_state
{
	texture = <FilterTexture0>;
	AddressU = CLAMP;
	AddressV = CLAMP;
	MIPFILTER = NONE;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

bool BloomThreshold(float3 hdrColor)
{
	return any(hdrColor > cvBloomParams.y);
}

float4 PS_Bloom(PS_INPUT IN) : COLOR
{
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	
	float3 hdrColor = DeCompressColourSpace(diffuse_tex.rgb);
	
	diffuse_tex.rgb = BloomThreshold(hdrColor) ? hdrColor : float3(0.0, 0.0, 0.0);

	return diffuse_tex;
}

float4 PS_ApplyBloom(PS_INPUT IN) : COLOR
{
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	
	float3 hdrColor = DeCompressColourSpace(diffuse_tex.rgb);
	
	if (cvBloomParams.x > 0)
	{
		float3 bloom_tex = tex2D(FILTERTEXTURE0_SAMPLER, IN.uv).rgb;
		hdrColor += bloom_tex * cvBloomParams.x;
	}

	return float4(hdrColor, 1);
}

technique ExtractBloom
{
	pass p0
	{
		VertexShader = compile vs_3_0 VS_ScreenFilter();
		PixelShader = compile ps_3_0 PS_Bloom();
	}
}

technique ApplyBloom
{
	pass p0
	{
		VertexShader = compile vs_3_0 VS_ScreenFilter();
		PixelShader = compile ps_3_0 PS_ApplyBloom();
	}
}