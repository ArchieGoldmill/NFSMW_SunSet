#pragma once
#include "eSolid.h"
#include "eEffect.h"
#include "MeshEntry.h"

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
	void* DiffuseTextureInfo;
	void* NormalTextureInfo;
	void* HeightTextureInfo;
	void* SpecularTextureInfo;
	void* OpacityTextureInfo;
	int unk2;
	int UseLowLod;
	int unk3;
};