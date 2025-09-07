float4x4 WorldViewProj : WORLDVIEWPROJECTION;
float4 cvSunDir : LIGHTDIR;

void VS_GodRays(VS_INPUT IN, out PS_INPUT OUT)
{
	OUT.position = IN.position;
	OUT.uv = IN.tex;
}

float3 GodRays(float2 sun, float2 uv)
{
	float density = 1.0;
	float weight = 0.002;
	float decay = 1.0;
	float exposure = 1.0;
	const int numSamples = 100;
	
	float3 fragColor = float3(0.0, 0.0, 0.0);

	float2 deltaTextCoord = float2(uv - sun.xy);

	float2 textCoord = uv.xy;
	deltaTextCoord *= (1.0 / float(numSamples)) * density;
	float illuminationDecay = 1.0;

	for (int i = 0; i < numSamples; i++)
	{
		textCoord -= deltaTextCoord;
		
		float depth = tex2D(HEIGHTMAP_SAMPLER, textCoord).r;
		float3 samp = float3(1, 1, 1) * step(1000, depth);
		
		samp *= illuminationDecay * weight;
		fragColor += samp;
		illuminationDecay *= decay;
	}

	fragColor *= exposure;

	return fragColor;
}

float4 PS_GodRays(PS_INPUT IN) : COLOR
{
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	
	diffuse_tex.rgb += GodRays(cvSunDir.xy, IN.uv) * cvSunDir.z;

	return diffuse_tex;
}