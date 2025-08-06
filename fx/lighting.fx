float4 LocalEyePos : LOCALEYEPOS;

float4 cvSpecularColor;
float4 cvBrightness;
float4 cvAmbientColor;
float4 cvDiffuseColor;

float3 GetSpecular(float3 normal, float3 lightDir, float3 local_pos, float power)
{
	float3 view = normalize(LocalEyePos.xyz - local_pos);
	float3 reflectDir = normalize(reflect(-lightDir, normal));
	float specularFactor = pow(saturate(dot(reflectDir, view)), power);
	return specularFactor * cvSpecularColor.rgb;
}

float3 GetSpecular(float3 normal, float3 lightDir, float3 local_pos)
{
	return GetSpecular(normal, lightDir, local_pos, cvSpecularColor.w);
}

float4 GetVertexColor(float4 color)
{
	color.rgb *= cvAmbientColor.rgb * 2;
	return color;
}