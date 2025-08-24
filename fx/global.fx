#ifndef GLOBAL_HEADER
#define GLOBAL_HEADER

float4x4 WorldViewProj : WORLDVIEWPROJECTION;
float4x4 cmWorldMat : LOCALWORLD;
float4 TextureOffset : TEXTUREOFFSET;
float4 LocalLightVec : LOCALLIGHTDIRVEC;

int Cull_Mode : CULL_MODE;
int BaseBlendState[5] : BLENDSTATE;
int BaseTextureFilterParam : BASETEXTUREFILTERPARAM;
int BaseMinTextureFilter : BASEMINTEXTUREFILTER;
int BaseMagTextureFilter : BASEMAGTEXTUREFILTER;
int BaseAddressU;
int BaseAddressV;

float4 clip_pos(float4 screen_pos)
{
	return mul(screen_pos, WorldViewProj);
}

float2 uv_offset(float2 tex)
{
	return tex + TextureOffset.xy;
}

texture diffusemap : DiffuseMap;
sampler DIFFUSEMAP_SAMPLER = sampler_state
{
	texture = <diffusemap>;
	AddressU = <BaseAddressU>;
	AddressV = <BaseAddressV>;
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

#endif // GLOBAL_HEADER