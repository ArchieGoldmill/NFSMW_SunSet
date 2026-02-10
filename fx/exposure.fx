#include "hdr.fx"

float4 cvTexelSize;

float4 PS_Luminance(PS_INPUT IN) : COLOR
{
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	
	float3 color = DeCompressColourSpace(diffuse_tex.rgb);
	
	float lum = dot(color, LuminanceVector);
	lum = max(lum, 0.0001);
	
	return float4(log(lum), 0, 0, 0);
}

float4 PS_Exposure(PS_INPUT IN) : COLOR
{
	float sum = 0.0;
	float2 texelSize = cvTexelSize.xy;

	sum += tex2D(DIFFUSEMAP_SAMPLER, IN.uv + texelSize * float2(-0.5, -0.5)).r;
	sum += tex2D(DIFFUSEMAP_SAMPLER, IN.uv + texelSize * float2(0.5, -0.5)).r;
	sum += tex2D(DIFFUSEMAP_SAMPLER, IN.uv + texelSize * float2(-0.5, 0.5)).r;
	sum += tex2D(DIFFUSEMAP_SAMPLER, IN.uv + texelSize * float2(0.5, 0.5)).r;

	return float4(sum * 0.25, 0, 0, 0);
}

technique Luminance
{
	pass p0
	{
		VertexShader = compile vs_3_0 VS_ScreenFilter();
		PixelShader = compile ps_3_0 PS_Luminance();
	}
}

technique Exposure
{
	pass p0
	{
		VertexShader = compile vs_3_0 VS_ScreenFilter();
		PixelShader = compile ps_3_0 PS_Exposure();
	}
}