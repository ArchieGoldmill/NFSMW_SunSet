
void VS_LightQuery(VS_INPUT IN, out PS_INPUT OUT)
{
	OUT.position = mul(IN.position, WorldViewProj);
}

float4 PS_LightQuery(PS_INPUT IN) : COLOR
{
	return float4(1, 0, 0, 0.2);
}

technique LightQuery
{
	pass p0
	{
		ColorWriteEnable = 0;

		ZEnable = TRUE;
		ZWriteEnable = FALSE;
		ZFunc = LessEqual;

		CullMode = NONE;

		AlphaTestEnable = FALSE;
		AlphaBlendEnable = FALSE;

		SrcBlend = One;
		DestBlend = Zero;

		VertexShader = compile vs_3_0 VS_LightQuery();
		PixelShader = compile ps_3_0 PS_LightQuery();
	}
}