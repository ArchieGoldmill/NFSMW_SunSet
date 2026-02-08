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

texture FilterTexture1 : FILTERTEXTURE1;
sampler3D VOLUMEMAP_SAMPLER = sampler_state
{
	texture = <FilterTexture1>;
	AddressU = CLAMP;
	AddressV = CLAMP;
	AddressW = CLAMP;
	MIPFILTER = NONE;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

bool BloomThreshold(float3 hdrColor)
{
	return any(hdrColor > cvBloomParams.y);
}

float3 ACESFilm(float3 x)
{
	float tA = 2.51;
	float tB = 0.03;
	float tC = 2.43;
	float tD = 0.59;
	float tE = 0.14;
	return clamp((x * (tA * x + tB)) / (x * (tC * x + tD) + tE), 0.0, 1.0);
}

float4 PS_Bloom(PS_INPUT IN) : COLOR
{
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	
	float3 hdrColor = DeCompressColourSpace(diffuse_tex.rgb);
	hdrColor *= cfFilter.w;
	
	diffuse_tex.rgb = BloomThreshold(hdrColor) ? hdrColor : float3(0.0, 0.0, 0.0);

	return diffuse_tex;
}

float4 PS_ApplyBloom(PS_INPUT IN) : COLOR
{
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	
	float3 hdrColor = DeCompressColourSpace(diffuse_tex.rgb);
	
	hdrColor *= cfFilter.w;
	
	if (cvBloomParams.x > 0)
	{
		float3 bloom_tex = tex2D(FILTERTEXTURE0_SAMPLER, IN.uv).rgb;
		hdrColor += bloom_tex * cvBloomParams.x;
	}
	
	if (cfFilter.z > 0)
	{
		hdrColor = ACESFilm(hdrColor);
	}
	
	hdrColor = saturate(hdrColor);
	
	if (cfFilter.y > 0)
	{
		float3 filtered = tex3D(VOLUMEMAP_SAMPLER, hdrColor).rgb;
		hdrColor = lerp(hdrColor, filtered, cfFilter.y);
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