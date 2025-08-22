float4 cvRainParams;
float4 EnvmapMin : ENVMAPMIN;
float4 SpecularMin : SPECULARMIN;

texture MISCMAP2_TEXTURE;
sampler2D MISCMAP2_SAMPLER = sampler_state
{
	texture = <MISCMAP2_TEXTURE>;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture MISCMAP3_TEXTURE;
sampler2D MISCMAP3_SAMPLER = sampler_state
{
	texture = <MISCMAP3_TEXTURE>;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

float3 ApplyRainSlide(float3 position, float3 normal)
{
	float2 offset = float2(0, 0);
	float time = cvRainParams.z * 0.2;
	float power = 0;

	float powerXY = abs(dot(normal.xy, float2(0, 1)));
	float powerZY = abs(dot(normal.zy, float2(0, 1)));
	float powerYX = abs(dot(normal.yx, float2(0, 1)));

	float horiz_power = 0;
	float2 rain_uv;
	if (powerXY > powerYX)
	{
		horiz_power = powerXY;
		rain_uv = position.xz;
	}
	else
	{
		horiz_power = powerYX;
		rain_uv = position.yz;
	}

	offset = float2(0, time);
	power = horiz_power * powerZY * cvRainParams.x * 0.15;

	float3 rain_slide_sample = tex2D(MISCMAP3_SAMPLER, rain_uv * 2 + offset).xyz;
	
	float3 bitangent = cross(normal, float3(0, 0, 1)); // up vector
	float3 tangent = normalize(cross(bitangent, normal));
	float3x3 tbn = { tangent, cross(normal, tangent), normal };

	rain_slide_sample = rain_slide_sample * 2 - 1;
	rain_slide_sample.z = sqrt(1 - dot(rain_slide_sample.xy, rain_slide_sample.xy));
	rain_slide_sample = mul(normalize(rain_slide_sample), tbn);

	return lerp(normal, rain_slide_sample, power);
}

float3 ApplyRainDrops(float3 position, float3 normal, out float rainPower)
{
	if (cvRainParams.y > 0.0)
	{
		normal = ApplyRainSlide(position, normal);
		float3 bitangent = cross(normal, float3(1, 0, 0)); // forward vector
		float3 tangent = normalize(cross(bitangent, normal));
		
		float3x3 tbn = { tangent, cross(normal, tangent), normal };

		float4 rain_drop_sample = tex2D(MISCMAP2_SAMPLER, position.xy * 2);

		float3 rain_drop = rain_drop_sample.xyz;
		rainPower = saturate(rain_drop_sample.w);
		float rainMap = rain_drop_sample.z;
		rainMap = frac(rainMap - cvRainParams.z * (rainMap + 1));

		float dot_final = pow(abs(dot(normal, float3(0, 0, 1))), 2);
		rainPower *= dot_final * rainMap * cvRainParams.y;

		rain_drop = rain_drop * 2 - 1;
		rain_drop.z = sqrt(1 - dot(rain_drop.xy, rain_drop.xy));
		rain_drop = mul(normalize(rain_drop), tbn);
		normal = lerp(normal, rain_drop, rainPower);
	}
	else
	{
		rainPower = 0;
	}
	
	return normal;
}