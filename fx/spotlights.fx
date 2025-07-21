struct SpotLight
{
	float3 Position;
	float Range;
	float3 Direction;
	float OuterCos;
	float3 Color;
	float InnerCos;
};

SpotLight caSpotLights[24];
 
float3 GetSpotlight(SpotLight light, float3 normal, float3 worldPos)
{
	float3 lightVec = light.Position - worldPos;
	float distance = length(lightVec);
	float3 L = lightVec / distance;

	float spotCos = dot(-L, light.Direction);
	float spotAtten = saturate((spotCos - light.OuterCos) / (light.InnerCos - light.OuterCos));

	float atten = saturate(1.0 - distance / light.Range);
	atten *= atten;

	float NdotL = saturate(dot(normal, L));

	return light.Color * NdotL * atten * spotAtten;
}

float3 ApplySpotLights(float3 normal, float3 worldPos, int count)
{
	float3 color = float3(0, 0, 0);
	for (int i = 0; i < count; ++i)
	{
		SpotLight spotlight = caSpotLights[i];
		color += GetSpotlight(spotlight, normal, worldPos);
	}
	
	float len = length(color);
	len = min(len, 4.0);
	color = normalize(color) * len;
	
	return color;
}