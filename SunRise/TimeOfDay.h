#pragma once
#include <d3dx9.h>
#include "Utilities.h"

struct TimeOfDay
{
	static inline TimeOfDay*& Instance = *reinterpret_cast<TimeOfDay**>(0x009B392C);

	float UpdateRate;
	int UpdateDirection;
	float CurrentTime;

	int pad[85];

	D3DXVECTOR4 SunPosition;
	D3DXVECTOR4 SunDirection;
};

ASSERT_SIZE(TimeOfDay, 0x180);