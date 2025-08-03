float cfTimeTicker;
float3 cvWaterColor;

texture MISCMAP4_TEXTURE;
sampler2D MISCMAP4_SAMPLER = sampler_state
{
	texture = MISCMAP4_TEXTURE;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture ReflectedTex : REFLECTEDTEX;
sampler REFLECTEDTEX_SAMPLER = sampler_state
{
	Texture = <ReflectedTex>;
	AddressU = CLAMP;
	AddressV = CLAMP;
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
};


void VS_Water(VS_INPUT IN, out PS_INPUT OUT)
{
	OUT.position = world_position(IN.position);
	OUT.tangent = LocalEyePos.xyz - IN.position.xyz;
	
	float3 world_pos = mul(IN.position, cmWorldMat).xyz;
	world_pos.y *= 0.7;
	OUT.uv = float2(world_pos.xy / 100 + float2(1.7, 0.0) * cfTimeTicker / 100);
	
	OUT.normal = float3(world_pos.xy / 150 + float2(-1.7, 2.0) * cfTimeTicker / 150, 1);
	
	float4 p = OUT.position;
	p.y = -p.y;
	OUT.shadow_tex = p;
	
	OUT.local_pos = IN.position;
	OUT.local_pos.w = OUT.position.z;
}

float4 PS_Water(PS_INPUT IN) : COLOR
{
	float3 component1 = tex2Dbias(MISCMAP4_SAMPLER, float4(IN.uv, 0, -20)).rgb * 2 - 1;
	float3 component2 = tex2Dbias(MISCMAP4_SAMPLER, float4(IN.normal.xy, 0, -20)).rgb * 2 - 1;

	float3 normal = normalize(component1 + component2);
	
	float2 screenUV = IN.shadow_tex.xy / IN.shadow_tex.w * 0.5 + 0.5;
	float4 reflectionSampleMain = tex2D(REFLECTEDTEX_SAMPLER, saturate(screenUV + normal.rg * 0.003));
	float4 reflectionSample = tex2D(REFLECTEDTEX_SAMPLER, saturate(screenUV + normal.rg * 0.1));
	
	float4 final = reflectionSample * 0.5 + reflectionSampleMain * 0.6;
	final.rgb *= cvWaterColor.rgb;
	final.a = 1;
	
	float3 view = normalize(IN.tangent);
	float3 lightDir = normalize(LocalLightVec.xyz);
	float3 reflectDir = normalize(reflect(-lightDir, float3(0, 0, 1)));
	float3 reflectDir1 = normalize(reflect(-lightDir, normal));
	float specularFactor = pow(saturate(dot(reflectDir, view)), 10.0);
	float specularFactor1 = pow(saturate(dot(reflectDir1, view)), 10.0);
	float3 specular = (specularFactor + specularFactor1) / 2 * max(float3(0.2, 0.2, 0.2), cvSpecularColor.rgb);
	
	final.rgb += specular;
	
	APPLY_FOG
	
	return final;
}

technique Water
{
	pass p0
	{
		COMMON_PASS_BODY

		VertexShader = compile vs_3_0 VS_Water();
		PixelShader = compile ps_3_0 PS_Water();
	}
}