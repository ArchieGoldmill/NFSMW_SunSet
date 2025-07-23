float4 cvSpecularColor;
float4 LocalEyePos : LOCALEYEPOS;

float3 GetSpecular(float3 normal, float3 lightDir, float3 local_pos)
{
	float3 view = normalize(LocalEyePos.xyz - local_pos);
	float3 reflectDir = normalize(reflect(-lightDir, normal));
	float specularFactor = pow(saturate(dot(reflectDir, view)), cvSpecularColor.w);
	return specularFactor * cvSpecularColor.rgb;
}