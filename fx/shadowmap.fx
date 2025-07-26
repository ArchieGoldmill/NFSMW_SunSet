
void VS_ShadowMap(VS_INPUT IN, out PS_INPUT OUT)
{
	OUT.position = mul(IN.position, WorldViewProj);
	OUT.uv.xy = IN.tex.xy;
}

float4 PS_ShadowMap(PS_INPUT IN) : COLOR
{
	float4 v_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv.xy);
	return v_tex;
}

technique ShadowMap
{
	pass p0
	{
		COMMON_PASS_BODY

		VertexShader = compile vs_3_0 VS_ShadowMap();
		PixelShader = compile ps_3_0 PS_ShadowMap();
	}
}