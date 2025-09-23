#pragma once
#include "TrackPath.h"

struct Rain
{
	inline static Rain*& Instance = *(Rain**)0x009196B8;

	int unk1[0x8F];
	int IsInTunnel;
	int IsUnderOverpass;
	TrackPath* pTrackPath;
	int unk2[0xF];
	int NumParticles;
	eView* View;
	float Intensity;
};