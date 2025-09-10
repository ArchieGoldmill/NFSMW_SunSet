float4x4 matShadowMapWVP : SHADOWTRANSFORM;
float ShadowMapScale : SHADOWMAPSCALE;

texture ShadowMapTex : SHADOWMAP;
sampler SHADOWMAP_SAMPLER = sampler_state
{
	Texture = <ShadowMapTex>;
	BorderColor = 0xFFFFFFFF;
	AddressU = BORDER;
	AddressV = BORDER;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

float4 vertex_shadow_tex(const float4 vPosition)
{
	return mul(vPosition, matShadowMapWVP);
}

float GetShadow(float2 uv, float biased)
{
	float depth = tex2Dproj(SHADOWMAP_SAMPLER, float4(uv, biased, 1.0)).r;
	return depth;
}

const float2 poissonDisk[10] =
{
	float2(0, 0),
	float2(-0.75, -0.61),
	float2(0.35, -0.88),
	float2(-0.92, 0.28),
	float2(0.94, 0.34),
	float2(-0.28, 0.91),
	float2(0.25, 0.43),
	float2(-0.49, -0.55),
	float2(0.51, -0.15),
	float2(0.02, 0.15)
};

float DoShadow10(float2 uv, float biased)
{
	float2 texelSize = ShadowMapScale * 2;
			
	float sum = 0;
	sum += GetShadow(uv + poissonDisk[0] * texelSize, biased);
	sum += GetShadow(uv + poissonDisk[1] * texelSize, biased);
	sum += GetShadow(uv + poissonDisk[2] * texelSize, biased);
	sum += GetShadow(uv + poissonDisk[3] * texelSize, biased);
	sum += GetShadow(uv + poissonDisk[4] * texelSize, biased);
	sum += GetShadow(uv + poissonDisk[5] * texelSize, biased);
	sum += GetShadow(uv + poissonDisk[6] * texelSize, biased);
	sum += GetShadow(uv + poissonDisk[7] * texelSize, biased);
	sum += GetShadow(uv + poissonDisk[8] * texelSize, biased);
	sum += GetShadow(uv + poissonDisk[9] * texelSize, biased);
	
	return sum / 10.0;
}

float DoShadow(float4 clipPos, float ndotl)
{
	float shadow = 1;
	
	if (ndotl <= 0)
	{
		shadow = 0.0;
	}
	else
	{
		float2 uv = clipPos.xy / clipPos.w;
		float biased = clipPos.z / clipPos.w - 0.0001 * (1 - ndotl);
		shadow = DoShadow10(uv, biased);
		
		float fY = clipPos.y / clipPos.w;
		float fade = saturate((fY + 0.3) * 2);
		shadow = saturate(shadow + (1 - fade));
	}
	
	return shadow;
}