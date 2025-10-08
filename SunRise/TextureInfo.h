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
	static inline auto& DefaultAlpha = *(TextureInfo**)0x00916040;

	TexturePlatInfo* PlatInfo;
	TextureInfo* Prev;
	TextureInfo* Next;
	char DebugName[24];
	Hash NameHash;
	Hash ClassNameHash;
	Hash ImageParentHash;
	int32_t ImagePlacement;
	int32_t PalettePlacement;
	int32_t ImageSize;
	int32_t PaletteSize;
	int32_t BaseImageSize;
	int16_t Width;
	int16_t Height;
	int8_t ShiftWidth;
	int8_t ShiftHeight;
	uint8_t ImageCompressionType;
	uint8_t PaletteCompressionType;
	int16_t NumPaletteEntries;
	int8_t NumMipMapLevels;
	int8_t TilableUV;
	int8_t BiasLevel;
	int8_t RenderingOrder;
	int8_t ScrollType;
	int8_t UsedFlag;
	int8_t ApplyAlphaSorting;
	int8_t AlphaUsageType;
	int8_t AlphaBlendType;
	int8_t Flags;
	int8_t MipmapBiasType;

	void* LockImage()
	{
		FUNC(0x006BED80, void*, __thiscall, _LockImage, TextureInfo*, int);
		return _LockImage(this, 1);
	}

	void UnlockImage()
	{
		FUNC(0x006BEDB0, void, __thiscall, _UnlockImage, TextureInfo*, int);
		_UnlockImage(this, 1);
	}

	static TextureInfo* Get(Hash hash, bool defaultIfNotFound, bool includeUnloaded)
	{
		FUNC(0x00503400, TextureInfo*, __cdecl, _Get, Hash, bool, bool);
		return _Get(hash, defaultIfNotFound, includeUnloaded);
	}
};