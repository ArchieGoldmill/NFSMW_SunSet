void VS_Main(VS_INPUT IN, out PS_INPUT OUT)
{
	OUT = VS_Base(IN);
	OUT.spotlight = float4(0.0, 0.0, 0.0, 1.0);
}

void VS_LitVertex(VS_INPUT IN, out PS_INPUT OUT)
{
	OUT = VS_Base(IN);
	SpotLightResult result = ApplySpotLights(OUT.normal, OUT.local_pos.xyz, 12, -1);
	OUT.spotlight = float4(result.Diffuse, 1);
}

void VS_Stub(VS_INPUT IN, out PS_INPUT OUT)
{
	OUT.position = mul(IN.position, WorldViewProj);
}

float4 PS_Stub(PS_INPUT IN) : COLOR
{
	return float4(0, 1, 0, 1);
}

technique world <int shader = 1; >
{
	pass p0
	{
		COMMON_PASS_BODY

		VertexShader = compile vs_3_0 VS_Stub();
		PixelShader = compile ps_3_0 PS_Stub();
	}
}

technique LitPixel_4
{
	pass p0
	{
		COMMON_PASS_BODY

		VertexShader = compile vs_3_0 VS_Main();
		PixelShader = compile ps_3_0 PS_LitPixel(4);
	}
}

technique LitPixel_8
{
	pass p0
	{
		COMMON_PASS_BODY

		VertexShader = compile vs_3_0 VS_Main();
		PixelShader = compile ps_3_0 PS_LitPixel(8);
	}
}

technique LitPixel_16
{
	pass p0
	{
		COMMON_PASS_BODY

		VertexShader = compile vs_3_0 VS_Main();
		PixelShader = compile ps_3_0 PS_LitPixel(16);
	}
}

technique LitPixel_24
{
	pass p0
	{
		COMMON_PASS_BODY

		VertexShader = compile vs_3_0 VS_Main();
		PixelShader = compile ps_3_0 PS_LitPixel(24);
	}
}

technique Unlit
{
	pass p0
	{
		COMMON_PASS_BODY

		VertexShader = compile vs_3_0 VS_Main();
		PixelShader = compile ps_3_0 PS_LitPixel(0);
	}
}

technique LitVertex
{
	pass p0
	{
		COMMON_PASS_BODY

		VertexShader = compile vs_3_0 VS_LitVertex();
		PixelShader = compile ps_3_0 PS_LitPixel(0);
	}
}

technique Invisible
{
	pass p0
	{
		VertexShader = null;
		PixelShader = null;
	}
}