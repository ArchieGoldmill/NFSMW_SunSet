struct SpotLight
{
	float3 Position;
	float Range;
	float3 Direction;
	float OuterCos;
	float3 Color;
	float InnerCos;
};

struct SpotLightResult
{
	float3 Diffuse;
	float3 Specular;
};

SpotLight caSpotLights[24];

SpotLightResult GetSpotlight(SpotLight light, float3 localNormal, float3 localPos, float3 view, float shine)
{
	float3 L = light.Position - localPos;
	float distance = length(L);
	L /= distance;

	float3 H = normalize(L + view);

	float spotCos = dot(-L, light.Direction);
	float diffuseAtten = smoothstep(light.OuterCos, light.InnerCos, spotCos);
	float specAtten = spotCos;
	
	float distAtten = saturate(1.0 - distance / light.Range);
	distAtten *= distAtten;
	
	float NdotL = saturate(dot(localNormal, L));
	float3 diffuse = NdotL * diffuseAtten;

	float spec;
	float NdotH = saturate(dot(localNormal, H));
	
	if (shine < 0 || specAtten < 0)
	{
		spec = float3(0, 0, 0);
	}
	else
	{
		spec = pow(NdotH, shine) * NdotL * specAtten * distAtten;
	}
	
	SpotLightResult result;
	
	result.Diffuse = light.Color * diffuse * distAtten;
	result.Specular = spec;

	return result;
}

SpotLightResult ApplySpotLights(float3 normal, float3 localPos, int count, float shine)
{
	SpotLightResult result;
	result.Diffuse = float3(1, 1, 1) * 0.000001;
	result.Specular = result.Diffuse;
	
	float3 view = normalize(LocalEyePos - localPos);
	for (int i = 0; i < count; ++i)
	{
		SpotLight spotlight = caSpotLights[i];
		if (spotlight.Range > 0)
		{
			SpotLightResult current = GetSpotlight(spotlight, normal, localPos, view, shine);
			result.Diffuse += current.Diffuse;
			result.Specular += current.Specular;
		}
	}
	
	float len = length(result.Diffuse);
	len = min(len, 4.0);
	result.Diffuse = normalize(result.Diffuse) * len;
	
	return result;
}

SpotLightResult ApplySpotLights(float3 normal, float3 localPos, int count, float shine, float3 defaultResult)
{
	if (count > 0)
	{
		return ApplySpotLights(normal, localPos, count, shine);
	}
	
	SpotLightResult result;
	result.Diffuse = defaultResult;
	result.Specular = float3(0, 0, 0);
	return result;
}