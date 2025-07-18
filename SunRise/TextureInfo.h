#pragma once
#include <d3dx9.h>
#include "Utilities.h"

struct TexturePlatInfo
{
	int unk[6];
	IDirect3DTexture9* D3DTexture;
};

struct TextureInfo
{
	TexturePlatInfo* PlatInfo;
	int unk[2];
	char name[24];
	unsigned int hash;

	static TextureInfo* Get(Hash hash, bool defaultIfNotFound, bool includeUnloaded)
	{
		FUNC(0x00503400, TextureInfo*, __cdecl, _Get, Hash, bool, bool);
		return _Get(hash, defaultIfNotFound, includeUnloaded);
	}
};