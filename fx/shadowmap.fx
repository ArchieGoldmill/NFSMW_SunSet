struct PS_INPUT_ShadowMap
{
	float4 position : POSITION;
	float2 tex : TEXCOORD;
};


void VS_ShadowMap(VS_INPUT IN, out PS_INPUT_ShadowMap OUT)
{
	OUT.position = mul(IN.position, WorldViewProj);
	OUT.tex.xy = IN.tex.xy;
}

float4 PS_ShadowMap(PS_INPUT_ShadowMap IN) : COLOR
{
	float4 v_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.tex);
	return v_tex;
}

technique ShadowMap
{
	pass p0
	{
		CullMode = <Cull_Mode>;
		AlphaTestEnable = (BaseBlendState[0]);
		AlphaRef = (BaseBlendState[1]);
		AlphaBlendEnable = (BaseBlendState[2]);
		SrcBlend = (BaseBlendState[3]);
		DestBlend = (BaseBlendState[4]);

		VertexShader = compile vs_3_0 VS_ShadowMap();
		PixelShader = compile ps_3_0 PS_ShadowMap();
	}
}