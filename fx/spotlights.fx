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

	float spotCos = dot(-L, light.Direction);
	float diffuseAtten = smoothstep(light.OuterCos, light.InnerCos, spotCos);

	float distAtten = saturate(1.0 - distance / light.Range);
	distAtten *= distAtten;

	float dotL = dot(localNormal, L);
	dotL = smoothstep(-0.5, 1.0, dotL);
	
	float3 spec = float3(0, 0, 0);
	
	float3 lightScale = light.Color * dotL * distAtten;
	
#ifdef SPOT_SPECULAR
	float3 H = normalize(L + view);
	float NdotH = saturate(dot(localNormal, H));
	spec = pow(NdotH, shine);
	spec *= lightScale * saturate(spotCos);
#endif

	SpotLightResult result;
	
	result.Diffuse = lightScale * diffuseAtten;
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
	len = min(len, 2.0);
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