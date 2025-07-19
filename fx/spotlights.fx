struct SpotLight
{
	float3 Position;
	float Range;
	float3 Direction;
	float Power;
	float3 Color;
	float Intensity;
};

SpotLight SpotLights[24] : SPLINE;
 
float3 GetSpotlight(SpotLight light, float3 normal, float3 worldPos)
{
	float3 lightVec = light.Position - worldPos;
	float distance = length(lightVec);
	float3 lightDir = normalize(lightVec);

	float3 spotDir = normalize(-light.Direction);

	float NdotL = dot(normal, lightDir);
	NdotL = max(NdotL, -0.5 * NdotL);

	float spotCos = saturate(dot(lightDir, spotDir));
	float spotFactor = pow(spotCos, light.Power);

	float attenuation = saturate(1.0 - distance / light.Range);

	return light.Color * light.Intensity * NdotL * spotFactor * attenuation;
}

float3 ApplySpotLights(float3 normal, float3 worldPos, int count)
{
	float3 color = float3(0, 0, 0);
	for (int i = 0; i < count; ++i)
	{
		SpotLight spotlight = SpotLights[i];
		color += GetSpotlight(spotlight, normal, worldPos);
	}

	return color;
}