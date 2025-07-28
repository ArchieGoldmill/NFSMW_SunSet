float4x4 WorldViewProj : WORLDVIEWPROJECTION;
float4x4 cmWorldMat : LOCALWORLD;
float4 ScreenOffset : SCREENOFFSET;
int Cull_Mode : CULL_MODE;
int BaseBlendState[5] : BLENDSTATE;
int BaseTextureFilterParam : BASETEXTUREFILTERPARAM;
int BaseMinTextureFilter : BASEMINTEXTUREFILTER;
int BaseMagTextureFilter : BASEMAGTEXTUREFILTER;
float4 TextureOffset : TEXTUREOFFSET;
float4 cvBrightness;

float4 world_position(float4 screen_pos)
{
	float4 p = mul(screen_pos, WorldViewProj);
	p.xy += ScreenOffset.xy * p.w;
	return p;
}

float4 screen_position(float4 screen_pos)
{
	screen_pos.xy += ScreenOffset.xy;
	return screen_pos;
}

texture diffusemap : DiffuseMap;
sampler DIFFUSEMAP_SAMPLER = sampler_state
{
	texture = <diffusemap>;
	AddressU = WRAP;
	AddressV = WRAP;
	MIPFILTER = <BaseTextureFilterParam>;
	MINFILTER = <BaseMinTextureFilter>;
	MAGFILTER = <BaseMagTextureFilter>;
};

#define COMMON_PASS_BODY \
	CullMode = <Cull_Mode>; \
	AlphaTestEnable = (BaseBlendState[0]); \
	AlphaRef = (BaseBlendState[1]); \
	AlphaBlendEnable = (BaseBlendState[2]); \
	SrcBlend = (BaseBlendState[3]); \
	DestBlend = (BaseBlendState[4]);

#define APPLY_ALPHA_EMISSIVE final.rgb = lerp(final.rgb, final.rgb * cvBrightness.rgb * cvBrightness.a, saturate(cvBrightness.a * diffuse_tex.a));