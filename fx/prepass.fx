float4x4 WorldView : WORLDVIEW;
float cfAlphaClip;

void VS_ZPrePass(VS_INPUT IN, out PS_INPUT OUT)
{
	OUT.position = mul(IN.position, WorldViewProj);
	OUT.uv.xy = IN.tex.xy + TextureOffset.xy;
	OUT.local_pos = mul(IN.position, WorldView);
}

float4 PS_ZPrePass(PS_INPUT IN) : COLOR
{
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	
	float linearDepth = IN.local_pos.z;

	return float4(linearDepth, linearDepth, linearDepth, diffuse_tex.a);
}

technique ZPrePass
{
	pass p0
	{
		COMMON_PASS_BODY
		VertexShader = compile vs_3_0 VS_ZPrePass();
		PixelShader = compile ps_3_0 PS_ZPrePass();
	}
}