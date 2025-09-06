struct SpotLightResult
{
	float3 Diffuse;
	float3 Specular;
};

float4 cvaSpPositionRange[24];
float4 cvaSpDirectionOuterCos[24];
float4 cvaSpColorInnerCos[24];

SpotLightResult GetSpotlight(const int i, const float3 localNormal, const float3 localPos, const float3 view, const float shine)
{
	float3 L = cvaSpPositionRange[i].xyz - localPos;
	float distance = length(L);
	L /= distance;

	float spotCos = dot(-L, cvaSpDirectionOuterCos[i].xyz);
	float diffuseAtten = smoothstep(cvaSpDirectionOuterCos[i].w, cvaSpColorInnerCos[i].w, spotCos);

	float distAtten = saturate(1.0 - distance / cvaSpPositionRange[i].w);
	distAtten *= distAtten;

	float dotL = dot(localNormal, L);
	dotL = smoothstep(-0.5, 1.0, dotL);
	
	float3 spec = float3(0, 0, 0);
	
	float3 lightScale = cvaSpColorInnerCos[i].xyz * dotL * distAtten;
	
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

SpotLightResult ApplySpotLights1(const float3 normal, const float3 localPos, const int count, const float shine, float maxPower = 5.0)
{
	SpotLightResult result;
	result.Diffuse = float3(1, 1, 1) * 0.000001;
	result.Specular = result.Diffuse;
	
	float3 view = normalize(LocalEyePos - localPos);
	for (int i = 0; i < count; ++i)
	{
		if (cvaSpPositionRange[i].w > 0)
		{
			SpotLightResult current = GetSpotlight(i, normal, localPos, view, shine);
			result.Diffuse += current.Diffuse;
			result.Specular += current.Specular;
		}
	}
	
	float len = length(result.Diffuse);
	len = min(len, maxPower);
	result.Diffuse = normalize(result.Diffuse) * len;
	
	return result;
}

SpotLightResult ApplySpotLights(const float3 normal, const float3 localPos, const int count, const float shine, const float3 defaultResult, float maxPower = 10.0)
{
	if (count > 0)
	{
		return ApplySpotLights1(normal, localPos, count, shine, maxPower);
	}
	
	SpotLightResult result;
	result.Diffuse = defaultResult;
	result.Specular = float3(0, 0, 0);
	return result;
}