#pragma once
#include "eSolid.h"
#include "eEffect.h"
#include "MeshEntry.h"
#include "TextureInfo.h"

struct RenderModel
{
	MeshEntry* pMeshEntry;
	int unk;
	eSolid* pSolid;
	int Flags;
	eEffect* Effect;
	void* LightContext;
	void* LightMatertial;
	D3DXMATRIX* LocalToWorld;
	void* BlendMatricies;
	TextureInfo* DiffuseTextureInfo;
	TextureInfo* NormalTextureInfo;
	TextureInfo* HeightTextureInfo;
	TextureInfo* SpecularTextureInfo;
	TextureInfo* OpacityTextureInfo;
	int unk2;
	int UseLowLod;
	int unk3;
};