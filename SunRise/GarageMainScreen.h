#pragma once
#include "Utilities.h"

struct GarageMainScreen
{
	static GarageMainScreen* GetInstance()
	{
		FUNC(0x007A1FA0, GarageMainScreen*, __cdecl, _GetInstance);
		return _GetInstance();
	}

	void HandleRender()
	{
		FUNC(0x007A2200, void, __thiscall, _HandleRender, GarageMainScreen*, int);
		_HandleRender(this, 1);
	}
};