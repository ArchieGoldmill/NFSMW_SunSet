#ifndef FOG_HEADER
#define FOG_HEADER

float4 cvFogColor;
float4 cvFogSunColor;
float4 cvFogValue;

float CalculateFog(float distance)
{
	float fog_end = cvFogValue.x;
	float fog_recip_end_minus_start = cvFogValue.y;
	float dist_fog_power = cvFogValue.z;
	float fog_exponent = cvFogValue.w;

	float fog_intensity_dist = saturate((fog_end - distance) * fog_recip_end_minus_start);
	fog_intensity_dist = pow((1.0 - fog_intensity_dist), fog_exponent) * dist_fog_power;

	float result = fog_intensity_dist;

	result = 1.0 - result;
	
	return result;
}

float3 ApplyFogColor(float3 color, float factor, float3 local_pos)
{
	float3 lightDir = normalize(LocalLightVec);
	
	float3 view = normalize(local_pos - LocalEyePos.xyz);
	
	float toSun = saturate(dot(view, lightDir));
	toSun = smoothstep(0.6, 1.0, toSun);
	
	float3 fogColor = lerp(cvFogColor.rgb, cvFogSunColor.rgb, toSun);
	
	return lerp(fogColor, color.rgb, factor);
}

#define APPLY_FOG\
	float fog = CalculateFog(IN.local_pos.w);\
	final.rgb = ApplyFogColor(final.rgb, fog, IN.local_pos.xyz);

#endif // FOG_HEADER