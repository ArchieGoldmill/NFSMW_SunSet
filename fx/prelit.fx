#include "fog.fx"

void VS_Prelit(VS_INPUT IN, out PS_INPUT OUT)
{
	OUT.position = mul(IN.position, WorldViewProj);
	OUT.uv.xy = IN.tex.xy + TextureOffset.xy;
	
	OUT.local_pos = IN.position;
	OUT.local_pos.w = OUT.position.z;
	
	OUT.color.rgb = lerp(float3(1, 1, 1), IN.color.rgb, cvEmissive.a);
	OUT.color.a = IN.color.a;
}

float4 PS_Prelit(PS_INPUT IN) : COLOR
{
	float4 diffuse_tex = tex2D(DIFFUSEMAP_SAMPLER, IN.uv);
	
	float4 final = diffuse_tex;
	final *= IN.color;
	final.rgb *= cvEmissive.rgb;
	
	if (BaseBlendState[4] != 2)
	{
		APPLY_FOG
	}
	
	return final;
}

technique Prelit
{
	pass p0
	{
		COMMON_PASS_BODY

		VertexShader = compile vs_3_0 VS_Prelit();
		PixelShader = compile ps_3_0 PS_Prelit();
	}
}