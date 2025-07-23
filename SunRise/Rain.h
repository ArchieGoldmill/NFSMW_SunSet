#pragma once

struct Rain
{
	inline static Rain*& Instance = *(Rain**)0x009196B8;

	int unk[0xA3];
	float Intensity;
};