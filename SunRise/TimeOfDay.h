#pragma once
#include <d3dx9.h>
#include "Utilities.h"

struct TimeOfDay
{
	static inline TimeOfDay*& Instance = *reinterpret_cast<TimeOfDay**>(0x009B392C);

	float UpdateRate;
	int UpdateDirection;
	float CurrentTime;
	float SunAzimuth;

	int pad[88];

	D3DXVECTOR4 SunPosition;
	D3DXVECTOR4 SunDirection;

	void Ctor()
	{
		FUNC(0x00770D70, void, __thiscall, _Ctor, TimeOfDay*);
		_Ctor(this);
	}
};

ASSERT_SIZE(TimeOfDay, 0x190);