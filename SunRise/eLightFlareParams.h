#pragma once

struct eLightFlareParams
{
	inline static int List = 0x008F2868;

	float MinSize;
	float MaxSize;
	D3DXVECTOR4 MaxColor;
	float Power;
	float ZBias;
	float MinScale;
	float MaxScale;
	int TextureID = 0;
};