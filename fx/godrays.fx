float4 cvSunDir : LIGHTDIR;
float4 cvGodRaysColor;

float3 GetGodRaySample(float2 uv)
{
	float depth = tex2D(HEIGHTMAP_SAMPLER, uv).r;
	float3 samp = float3(1, 1, 1) * step(4000, depth);
	
	float dist = distance(cvSunDir.xy, uv);
	dist = smoothstep(0.5, 0.0, dist);
	
	return samp * dist;
}

float3 GetGodRays(float2 uv)
{
	const int NUM_SAMPLES = 100;
	
	float2 texCoord = uv;
	float Density = 1.0;
	float Decay = 0.97;
	float Weight = 0.02;
	
	float2 deltaTexCoord = (texCoord - cvSunDir.xy);
	deltaTexCoord *= 1.0f / NUM_SAMPLES * Density;
	float3 color = float3(0, 0, 0);
	float illuminationDecay = 1.0f;
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		texCoord -= deltaTexCoord;
		
		float3 sample = GetGodRaySample(texCoord);
		sample *= illuminationDecay * Weight;
		color += sample;
		illuminationDecay *= Decay;
	}
	
	return color * cvGodRaysColor.rgb;
}

float4 PS_GodRays(PS_INPUT IN) : COLOR
{
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	
	diffuse_tex.rgb += GetGodRays(IN.uv) * 0.3 * cvSunDir.z;

	return diffuse_tex;
}

technique GodRays
{
	pass p0
	{
		VertexShader = compile vs_3_0 VS_ScreenFilter();
		PixelShader = compile ps_3_0 PS_GodRays();
	}
}