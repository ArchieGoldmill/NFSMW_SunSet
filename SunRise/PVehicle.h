#pragma once

struct IVehicleAI
{
	void* GetRoadBlock()
	{
		FUNC(0x00431D80, void*, __thiscall, _GetRoadBlock, IVehicleAI*);
		return _GetRoadBlock(this);
	}
};

struct PVehicle
{
	IVehicleAI* GetAIVehiclePtr()
	{
		FUNC(0x00688230, IVehicleAI*, __thiscall, _GetAIVehiclePtr, PVehicle*);
		return _GetAIVehiclePtr(this);
	}
};