#pragma once
#include "Config.h"
#include "TimeOfDay.h"

void ForceTime()
{
	if (g_Config.ForceTime >= 0.0f)
	{
		TimeOfDay::Instance->CurrentTime = g_Config.ForceTime;
	}
	else
	{
		TimeOfDay::Instance->CurrentTime += Game::DeltaTime * TimeOfDay::Instance->UpdateRate * 0.001;
		if (TimeOfDay::Instance->CurrentTime > 1)
		{
			TimeOfDay::Instance->CurrentTime = 0;
		}
	}
}

TimeOfDay* __fastcall CreateTimeOfDay(TimeOfDay* tod)
{
	tod->Ctor();
	tod->UpdateRate = g_Config.TimeUpdateRate;
	return tod;
}

void InitTime()
{
	injector::MakeCALL(0x006BF2C4, CreateTimeOfDay);
}