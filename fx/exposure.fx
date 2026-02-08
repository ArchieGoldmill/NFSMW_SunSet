#include "hdr.fx"

float4 cvTexelSize;

float4 PS_Luminance(PS_INPUT IN) : COLOR
{
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	float3 color = diffuse_tex.rgb;

	float lum = dot(color, LuminanceVector);

	return float4(lum, 0, 0, 0);
}

float4 PS_Exposure(PS_INPUT IN) : COLOR
{
	float sum = 0.0;
	float2 texelSize = cvTexelSize.xy;

	sum += tex2D(DIFFUSEMAP_SAMPLER, IN.uv + texelSize * float2(-0.5, -0.5)).r;
	sum += tex2D(DIFFUSEMAP_SAMPLER, IN.uv + texelSize * float2(0.5, -0.5)).r;
	sum += tex2D(DIFFUSEMAP_SAMPLER, IN.uv + texelSize * float2(-0.5, 0.5)).r;
	sum += tex2D(DIFFUSEMAP_SAMPLER, IN.uv + texelSize * float2(0.5, 0.5)).r;

	float avg = sum * 0.25;

	return float4(avg, 0, 0, 1);
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