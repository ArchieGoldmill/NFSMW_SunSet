float4x4 cmWorldIT;
float4 cvWorldEyePos;

struct SpotLightResult
{
	float3 Diffuse;
	float3 Specular;
};

float4 cvaSpPositionRange[32];
float4 cvaSpDirectionOuterCos[32];
float4 cvaSpColorInnerCos[32];
float cfaSpSpecular[32];

SpotLightResult GetSpotlight(const int i, const float3 normal, const float3 pos, const float3 view, const float shine)
{
	float3 lightPos = cvaSpPositionRange[i].xyz;
	float lightRange = cvaSpPositionRange[i].w;
	float3 lightDir = cvaSpDirectionOuterCos[i].xyz;
	float outerCos = cvaSpDirectionOuterCos[i].w;
	float3 lightColor = cvaSpColorInnerCos[i].xyz;
	float innerCos = cvaSpColorInnerCos[i].w;
	
	float3 L = lightPos - pos;
	float distance = length(L);
	L /= distance;

	float spotCos = dot(-L, lightDir);
	float diffuseAtten = smoothstep(outerCos, innerCos, spotCos);

	float distAtten = saturate(1.0 - distance / lightRange);
	distAtten *= distAtten;

	float dotL = dot(normal, L);
	dotL = smoothstep(-0.5, 1.0, dotL);
	
	float3 lightScale = lightColor * dotL * distAtten;
	
	float3 spec = float3(0, 0, 0);
	
#ifdef SPOT_SPECULAR
	float3 H = normalize(L + view);
	float NdotH = saturate(dot(normal, H));
	spec = pow(NdotH, shine);
	spec *= lightScale * saturate(spotCos);
#endif

	SpotLightResult result;
	
	result.Diffuse = lightScale * diffuseAtten;
	result.Specular = spec * cfaSpSpecular[i];
	
	return result;
}

SpotLightResult ApplySpotLights1(const float3 normal, const float3 pos, const int count, const float shine, const float maxPower)
{
	SpotLightResult result;
	result.Diffuse = 0;
	result.Specular = 0;
	
	float3 view = normalize(cvWorldEyePos.xyz - pos);
	for (int i = 0; i < count; ++i)
	{
		if (cvaSpPositionRange[i].w > 0)
		{
			SpotLightResult current = GetSpotlight(i, normal, pos, view, shine);
			result.Diffuse += current.Diffuse;
			result.Specular += current.Specular;
		}
	}
	
	float len = length(result.Diffuse);
	len = min(len, maxPower);
	result.Diffuse = normalize(result.Diffuse) * len;

	return result;
}

SpotLightResult ApplySpotLights(const float3 normal, const float3 pos, const int count, const float shine, const float maxPower = 4.5)
{
	if (count > 0)
	{
		return ApplySpotLights1(normal, pos, count, shine, maxPower);
	}
	
	SpotLightResult result;
	result.Diffuse = float3(0, 0, 0);
	result.Specular = float3(0, 0, 0);
	
	return result;
}

float3 ToWorldNormal(float3 normal)
{
	float3 world_normal = mul(normal, (float3x3) cmWorldIT);
	return normalize(world_normal);
}

float3 ToWorldPos(float4 position)
{
	return mul(position, cmWorldMat).xyz;
}