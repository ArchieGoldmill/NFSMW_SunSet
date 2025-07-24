

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

float3 GetSpotlight(SpotLight light, float3 localNormal, float3 localPos, float3 view, float shine)
{
	float3 L = light.Position - localPos;
	float distance = length(L);
	L /= distance;

	float3 H = normalize(L + view);

	float spotCos = dot(-L, light.Direction);
	float diffuseAtten = smoothstep(light.OuterCos, light.InnerCos, spotCos);
	float specAtten = spotCos * 5;
	
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
		spec = pow(NdotH, shine) * NdotL * specAtten;
	}

	return light.Color * (diffuse + spec) * distAtten;
}

float3 ApplySpotLights(float3 normal, float3 localPos, int count, float shine)
{
	float3 color = float3(0, 0, 0);
	
	float3 view = normalize(LocalEyePos - localPos);
	for (int i = 0; i < count; ++i)
	{
		SpotLight spotlight = caSpotLights[i];
		color += GetSpotlight(spotlight, normal, localPos, view, shine);
	}
	
	float len = length(color);
	len = min(len, 4.0);
	color = normalize(color) * len;
	
	return color;
}