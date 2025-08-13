#pragma once

struct IVehicleAI
{
	void* GetRoadBlock()
	{
		FUNC(0x00431D80, void*, __thiscall, _GetRoadBlock, IVehicleAI*);
		return _GetRoadBlock(this);
	}

	float GetBraking()
	{

	}
};

struct InputPlayer
{
	float unk[8];
	float Braking;
};

struct PVehicle
{
	static inline auto& Player = *(PVehicle**)0x0092CD28;

	IVehicleAI* GetAIVehiclePtr()
	{
		FUNC(0x00688230, IVehicleAI*, __thiscall, _GetAIVehiclePtr, PVehicle*);
		return _GetAIVehiclePtr(this);
	}

	float GetSpeed()
	{
		FUNC(0x006881B0, float, __thiscall, _GetSpeed, PVehicle*);
		return _GetSpeed(this);
	}

	float GetBraking()
	{
		auto input = *(InputPlayer**)((int)this + 0x3C);
		if (input)
		{
			return input->Braking;
		}

		return 0;
	}
};