#pragma once
#include <d3dx9.h>
#include <map>
#include "TextureInfo.h"

enum TechniqueType
{
	Technique_Unlit,
	Technique_LitPixel_8,
	Technique_LitPixel_16,
	Technique_LitPixel_24,
	Technique_LitVertex,
	Technique_ShadowMap,
	Technique_Count,
	Technique_Invalid = -1
};

enum class ShaderParam
{
	caSpotLights,
	count
};

struct ShaderParams
{
	D3DXHANDLE Techniques[Technique_Count];
	D3DXHANDLE Params[(int)ShaderParam::count];
};

enum class shader_type : unsigned int
{
	WorldShader,
	WorldReflectShader,
	WorldBoneShader,
	WorldNormalMap,
	CarShader,
	GlossyWindow,
	billboardshader,
	WorldMinShader,
	WorldNoFogShader,
	FEShader,
	FEMaskShader,
	FilterShader,
	OverbrightShader,
	ScreenFilterShader,
	RainDropShader,
	RunwayLightShader,
	VisualTreatmentShader,
	WorldPrelitShader,
	ParticlesShader,
	skyshader,
	shadow_map_mesh,
	SkyboxCurrentGen,
	ShadowPolyCurrentGen,
	CarShadowMapShader,
	WorldDepthShader,
	WorldNormalMapDepth,
	CarShaderDepth,
	GlossyWindowDepth,
	TreeDepthShader,
	shadow_map_mesh_depth,
	NormalMapNoFog
};

enum class shader_param : unsigned int
{
	AMBIENT,
	AMBIENTCOEFF,
	AMBIENTCOLOUR,
	BLENDSTATE,
	BASEMAGTEXTUREFILTER,
	BASEMINTEXTUREFILTER,
	BASEMIPTEXTUREFILTER,
	BASETEXTUREFILTERPARAM,
	BLENDMATRICES,
	COLORWRITEMODE,
	CLOUDSCROLL,
	CULL_MODE,
	DIFFUSECOLOUR,
	DiffuseMap,
	DIFFUSEMIN,
	DIFFUSERANGE,
	DIFFUSETEX,
	DISPLACEMAP,
	DOFFALLOFF,
	DOFOFFSET1,
	DOFOFFSET2,
	ENVMAPANGE,
	ENVMAPMIN,
	ENVMAPPOWER,
	EnvMapTexture,
	FFAMBIENTCOLOR,
	FFSKYBLENDFACTOR,
	FFSKYCOLOR,
	FECOLORWRITEFLAG,
	FEMaskMap,
	FILTERBLEND,
	FILTERTEXTURE0,
	FILTERTEXTURE1,
	FILTERTEXTURE2,
	FILTERTEXTURE3,
	FILTERWEIGHTS,
	FOCALRANGE,
	FOGCOLOR,
	FOGON,
	FOG_BR_PLUS_BM,
	FOG_ONE_OVER_BRBM,
	FOG_CONST_1,
	FOG_CONST_2,
	FOG_CONST_3,
	FOGVALUE,
	GAINMAP,
	HARMONIC,
	HEADLIGHTCLIPMAT,
	HEADLIGHTCLIPTEX,
	HEADLIGHTCOLOR,
	HEADLIGHTTEX,
	HEADLIGHTWVP,
	HeightMapTexture,
	INVSHADOWSTRENGTH,
	LIGHTCOLOR,
	LIGHTDIR,
	LOCALEYEPOS,
	LOCALLIGHTDIRVEC,
	LOCALCOLOURMATRIX,
	LOCALDIRECTIONMATRIX,
	MOTIONALPHA,
	NormalMapTexture,
	OPACITYMAPTEXTURE,
	OVERBRIGHTGREYSCALE,
	OVERBRIGHTOFFSET,
	RAMPMAP,
	RAINDROPOFFSET,
	RAINDROPALPHA,
	REFLECTEDPROJ,
	REFLECTEDTEX,
	REFLECTEDTEXMIN,
	SATURATIONSCALE,
	SCREENOFFSET,
	SHADOWCOLORMAP,
	SHADOWCURRENTGEN_COLOR,
	SHADOWLEVEL,
	SHADOWMAP,
	SHADOWMAPSCALE,
	SHADOWTRANSFORM,
	SPECULARCOLOUR,
	SPECULARMAPTEXTURE,
	SPECULARMIN,
	SPECULARPOWER,
	SPECULARRANGE,
	SPLINE,
	SKYMORPHALPHA,
	SKYMORPHALPHA2,
	SKYMORPHTEXTURE,
	SKYRGBA,
	SKY_TEXTURE_MISC_1,
	SKY_TEXTURE_MISC_2,
	SKY_TEXTURE_MISC_3,
	SKY_DIFFUSESCALE,
	SUNCOLOR,
	TEXTUREOFFSET,
	TEXTUREOFFSETMATRIX,
	TINTCOLOR,
	LOCALWORLD,
	WINDOWREFLECTION,
	WORLDLIGHTDIRVEC,
	WORLDVIEW,
	WORLDVIEWPROJECTION
};

struct eEffectParam
{
	char name[0x20];
	unsigned int key;
	D3DXHANDLE handle;
};

std::map<shader_type, ShaderParams> ShaderParamsMap;

struct eEffect
{
	virtual void Dtor() = 0;
	virtual void Start() = 0;
	virtual void End() = 0;

	inline static eEffect** List = (eEffect**)0x0093DE78;
	inline static eEffect*& Current = *(eEffect**)(0x00982C80);

	shader_type id;
	std::uint32_t stride;
	std::int32_t main_technique_number;
	D3DXHANDLE main_technique_handle;
	UINT PassCount;
	int field_18;
	int field_1C;
	int field_20;
	int field_24;
	int field_28;
	int field_2C;
	int field_30;
	int field_34;
	int field_38;
	int field_3C;
	bool Active;
	eEffectParam* Params;
	ID3DXEffect* D3DEffect;
	IDirect3DVertexDeclaration9* VertexDecl;

	void SetParams()
	{
		FUNC(0x006C5500, void, __thiscall, _SetParams, eEffect*);
		_SetParams(this);
	}

	void SetTexture(shader_param param, TextureInfo* textureInfo)
	{
		this->D3DEffect->SetTexture(this->Params[(int)param].handle, textureInfo->PlatInfo->D3DTexture);
	}

	void SetTexture(shader_param param, LPDIRECT3DVOLUMETEXTURE9 texture)
	{
		this->D3DEffect->SetTexture(this->Params[(int)param].handle, texture);
	}

	void SetVector(shader_param param, D3DXVECTOR4* vector)
	{
		this->D3DEffect->SetVector(this->Params[(int)param].handle, vector);
	}

	void SetFloat(shader_param param, float f)
	{
		this->D3DEffect->SetFloat(this->Params[(int)param].handle, f);
	}

	void SetValue(ShaderParam p, void* val, int size)
	{
		auto handle = ShaderParamsMap[this->id].Params[(int)p];
		if (handle)
		{
			this->D3DEffect->SetValue(handle, val, size);
		}
	}
};