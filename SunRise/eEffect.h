#pragma once
#include <d3dx9.h>
#include <map>
#include "TextureInfo.h"
#include "Game.h"

enum TechniqueType
{
	Technique_Unlit,
	Technique_Prelit,
	Technique_LitPixel_4,
	Technique_LitPixel_8,
	Technique_LitPixel_16,
	Technique_LitPixel_24,
	Technique_LitVertex,
	Technique_ShadowMap,
	Technique_Invisible,
	Technique_Water,
	Technique_ZPrePass,

	Technique_Count,
	Technique_Invalid = -1
};

const char* ShaderParamNames[] =
{
	"cvDiffuseColor",
	"cvAmbientColor",
	"cvSpecularColor",
	"cvSunDirection",
	"cvSkyBeta",
	"cvSkyParams",
	"cvRainParams",
	"cfMetallicScale",
	"cfSpecularHotSpot",
	"cvFogValue",
	"cvFogColor",
	"cvFogSunColor",
	"cvCloudColor",
	"cvEmissive",
	"cvCarEmissive",
	"cvWaterColor",
	"cfTimeTicker",
	"cbUseNormalMap",
	"cfVinylScale",
	"cvWindowColor",
	"cvaSpPositionRange",
	"cvaSpDirectionOuterCos",
	"cvaSpColorInnerCos",
	"cfaSpSpecular",
	"cvTexelSize",
	"cvBloomParams",
	"cvGodRaysColor",
	"cfShadowsEnabled",
	"cvLightning",

	"MISCMAP1_TEXTURE",
	"MISCMAP2_TEXTURE",
	"MISCMAP3_TEXTURE",
	"MISCMAP4_TEXTURE",
	"EMISSIVE_TEXTURE",
	"SkyNoiseTexture",

	"BaseAddressU",
	"BaseAddressV",
};

enum class ShaderParam
{
	cvDiffuseColor,
	cvAmbientColor,
	cvSpecularColor,
	cvSunDirection,
	cvSkyBeta,
	cvSkyParams,
	cvRainParams,
	cfMetallicScale,
	cfSpecularHotSpot,
	cvFogValue,
	cvFogColor,
	cvFogSunColor,
	cvCloudColor,
	cvEmissive,
	cvCarEmissive,
	cvWaterColor,
	cfTimeTicker,
	cbUseNormalMap,
	cfVinylScale,
	cvWindowColor,
	cvaSpPositionRange,
	cvaSpDirectionOuterCos,
	cvaSpColorInnerCos,
	cfaSpSpecular,
	cvTexelSize,
	cvBloomParams,
	cvGodRaysColor,
	cfShadowsEnabled,
	cvLightning,

	MISCMAP1_TEXTURE,
	MISCMAP2_TEXTURE,
	MISCMAP3_TEXTURE,
	MISCMAP4_TEXTURE,
	EMISSIVE_TEXTURE,
	SkyNoiseTexture,

	BaseAddressU,
	BaseAddressV,

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
	NormalMapNoFog,
	_count
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

ShaderParams ShaderParamsMap[(int)shader_type::_count];

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

	static eEffect* Get(shader_type type)
	{
		return eEffect::List[(int)type];
	}

	void SetParams()
	{
		FUNC(0x006C5500, void, __thiscall, _SetParams, eEffect*);
		_SetParams(this);
	}

	void SetTexture(ShaderParam p, TextureInfo* textureInfo)
	{
		if (textureInfo)
		{
			auto handle = ShaderParamsMap[(int)this->id].Params[(int)p];
			if (handle)
			{
				this->D3DEffect->SetTexture(handle, textureInfo->PlatInfo->D3DTexture);
			}
		}
	}

	void SetTexture(ShaderParam p, LPDIRECT3DVOLUMETEXTURE9 texture)
	{
		auto handle = ShaderParamsMap[(int)this->id].Params[(int)p];
		if (handle)
		{
			this->D3DEffect->SetTexture(handle, texture);
		}
	}

	void SetTexture(shader_param p, IDirect3DTexture9* texture)
	{
		auto handle = this->Params[(int)p].handle;
		if (handle)
		{
			this->D3DEffect->SetTexture(handle, texture);
		}
	}

	void SetTexture(ShaderParam p, IDirect3DCubeTexture9* texture)
	{
		auto handle = ShaderParamsMap[(int)this->id].Params[(int)p];
		if (handle)
		{
			this->D3DEffect->SetTexture(handle, texture);
		}
	}

	void SetVector(ShaderParam p, D3DXVECTOR4* v)
	{
		auto handle = ShaderParamsMap[(int)this->id].Params[(int)p];
		if (handle)
		{
			this->D3DEffect->SetVector(handle, v);
		}
	}

	void SetVector(shader_param p, D3DXVECTOR4* v)
	{
		auto handle = this->Params[(int)p].handle;
		if (handle)
		{
			this->D3DEffect->SetVector(handle, v);
		}
	}

	void SetVectorArray(ShaderParam p, D3DXVECTOR4* vs, UINT count)
	{
		auto handle = ShaderParamsMap[(int)this->id].Params[(int)p];
		if (handle)
		{
			this->D3DEffect->SetVectorArray(handle, vs, count);
		}
	}

	void SetFloatArray(ShaderParam p, float* vs, UINT count)
	{
		auto handle = ShaderParamsMap[(int)this->id].Params[(int)p];
		if (handle)
		{
			this->D3DEffect->SetFloatArray(handle, vs, count);
		}
	}

	void SetFloat(ShaderParam p, float v)
	{
		auto handle = ShaderParamsMap[(int)this->id].Params[(int)p];
		if (handle)
		{
			this->D3DEffect->SetFloat(handle, v);
		}
	}

	void SetInt(ShaderParam p, int v)
	{
		auto handle = ShaderParamsMap[(int)this->id].Params[(int)p];
		if (handle)
		{
			this->D3DEffect->SetInt(handle, v);
		}
	}

	void SetFloat(shader_param p, float v)
	{
		auto handle = this->Params[(int)p].handle;
		if (handle)
		{
			this->D3DEffect->SetFloat(handle, v);
		}
	}

	void SetBool(ShaderParam p, bool v)
	{
		auto handle = ShaderParamsMap[(int)this->id].Params[(int)p];
		if (handle)
		{
			this->D3DEffect->SetBool(handle, v);
		}
	}

	bool HasParam(ShaderParam p)
	{
		return ShaderParamsMap[(int)this->id].Params[(int)p] != NULL;
	}

	void SetTechniqueByName(LPCSTR pName)
	{
		auto tech = this->D3DEffect->GetTechniqueByName(pName);
		this->D3DEffect->SetTechnique(tech);
	}

	void DrawFullScreenQuad(IDirect3DTexture9* texture, bool invert = false)
	{
		struct {
			D3DXVECTOR3 position;
			D3DCOLOR color;
			D3DXVECTOR2 uv;
		} vertices[4];

		Game::Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		D3DSURFACE_DESC desc;

		texture->GetLevelDesc(0u, &desc);

		float w = static_cast<float>(desc.Width);
		float h = static_cast<float>(desc.Height);

		if ((desc.Width & 0x80000000) != 0)
		{
			w += 4.2949673e9f;
		}

		if ((desc.Height & 0x80000000) != 0)
		{
			h += 4.2949673e9f;
		}

		float inv_w = 0.5f / w;
		float inv_h = 0.5f / h;

		float uv00 = inv_w;
		float uv10 = inv_w + 1.0f;
		float uv20 = inv_w + 1.0f;
		float uv30 = inv_w;

		float uv11;
		float uv31;
		float uv21;
		float uv01;

		if (invert)
		{
			uv01 = inv_h + 1.0f;
			uv11 = inv_h + 1.0f;
			uv21 = inv_h;
			uv31 = inv_h;
		}
		else
		{
			uv01 = inv_h;
			uv11 = inv_h;
			uv21 = inv_h + 1.0f;
			uv31 = inv_h + 1.0f;
		}

		vertices[0].position.x = -1.0f;
		vertices[0].position.y = +1.0f;
		vertices[0].position.z = +0.0f;
		vertices[0].color = 0xFFFFFFFF;
		vertices[0].uv.x = uv00;
		vertices[0].uv.y = uv01;

		vertices[1].position.x = +1.0f;
		vertices[1].position.y = +1.0f;
		vertices[1].position.z = +0.0f;
		vertices[1].color = 0xFFFFFFFF;
		vertices[1].uv.x = uv10;
		vertices[1].uv.y = uv11;

		vertices[2].position.x = +1.0f;
		vertices[2].position.y = -1.0f;
		vertices[2].position.z = +0.0f;
		vertices[2].color = 0xFFFFFFFF;
		vertices[2].uv.x = uv20;
		vertices[2].uv.y = uv21;

		vertices[3].position.x = -1.0f;
		vertices[3].position.y = -1.0f;
		vertices[3].position.z = +0.0f;
		vertices[3].color = 0xFFFFFFFF;
		vertices[3].uv.x = uv30;
		vertices[3].uv.y = uv31;

		this->SetTexture(shader_param::DiffuseMap, texture);

		this->D3DEffect->CommitChanges();

		Game::Device->DrawPrimitiveUP(::D3DPT_TRIANGLEFAN, 2u, vertices, sizeof(vertices[0]));
	}
};