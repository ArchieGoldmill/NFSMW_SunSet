#pragma once
#include "eSolid.h"
#include "eEffect.h"
#include "MeshEntry.h"
#include "TextureInfo.h"
#include "eLightMaterial.h"

struct RenderModel
{
	static inline auto List = (RenderModel*)0x0093E878;

	MeshEntry* pMeshEntry;
	int unk;
	eSolid* pSolid;
	int Flags;
	eEffect* Effect;
	void* LightContext;
	eLightMaterial* LightMaterial;
	D3DXMATRIX* LocalToWorld;
	void* BlendMatricies;
	TextureInfo* DiffuseTextureInfo;
	TextureInfo* NormalTextureInfo;
	TextureInfo* HeightTextureInfo;
	TextureInfo* SpecularTextureInfo;
	TextureInfo* OpacityTextureInfo;
	unsigned int SortOrder;
	int UseLowLod;
	float unk3;
};

struct RenderingOrder
{
	unsigned int model_index;
	int sort_flags;

	inline bool operator < (const RenderingOrder& other) const
	{
		return this->ToInt64() < other.ToInt64();
	}

	inline const std::uint64_t ToInt64() const
	{
		return *reinterpret_cast<const std::uint64_t*>(this);
	}
};