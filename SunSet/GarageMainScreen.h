#pragma once
#include "Utilities.h"

struct GarageMainScreen
{
	static GarageMainScreen* GetInstance()
	{
		FUNC(0x007A1FA0, GarageMainScreen*, __cdecl, _GetInstance);
		return _GetInstance();
	}

	static int GetType()
	{
		auto manager = (int*)GarageMainScreen::GetInstance();
		if (!manager || manager[27])
		{
			return -1;
		}

		auto garageTypePtr = (int**)(0x0091CAE0);
		auto garageType = (*garageTypePtr)[6];
		return garageType;
	}

	void HandleRender()
	{
		FUNC(0x007A2200, void, __thiscall, _HandleRender, GarageMainScreen*, int);
		_HandleRender(this, 1);
	}
};