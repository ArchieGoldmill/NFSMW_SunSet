#pragma once
#include "Config.h"
#include "TimeOfDay.h"

void ForceTime()
{
	if (g_Config.ForceTime >= 0.0f)
	{
		TimeOfDay::Instance->CurrentTime = g_Config.ForceTime;
		return;
	}

	if (g_Config.RealTime)
	{
		std::time_t now = std::time(nullptr);
		std::tm* localTime = std::localtime(&now);

		int seconds = localTime->tm_hour * 60 * 60 + localTime->tm_min * 60 + localTime->tm_sec;
		const float maxSeconds = 24 * 60.0f * 60.0f;
		TimeOfDay::Instance->CurrentTime = seconds / maxSeconds;

		return;
	}

	TimeOfDay::Instance->CurrentTime += Game::DeltaTime * 0.016666668 * TimeOfDay::Instance->UpdateRate * 0.050000001;
	if (TimeOfDay::Instance->CurrentTime > 1)
	{
		TimeOfDay::Instance->CurrentTime = 0;
	}
}

TimeOfDay* __fastcall CreateTimeOfDay(TimeOfDay* tod)
{
	tod->Ctor();
	tod->UpdateRate = g_Config.TimeUpdateRate;
	return tod;
}

void TimeSetting()
{
	// VOTimeOfDay::Act(char const *,uint)
	injector::WriteMemory<DWORD>(0x89BB44, 0x50F7A0, true); // change code location
	injector::MakeRangedNOP(0x50F7AE, 0x50F7B6, true); // cleanup

	char LoadToEcx[] = { 0x8B, 0x0D, 0x2C, 0x39, 0x9B, 0x00 }; // mov ecx,[009B392C]
	injector::WriteMemoryRaw(0x50F7AE, LoadToEcx, sizeof(LoadToEcx), true);
	injector::WriteMemory<unsigned char>(0x50F7BE, 0x08, true); // mov [ecx+8], edx

	// VOTimeOfDay::Draw(void)
	injector::WriteMemory<DWORD>(0x89BB4C, 0x51B5E0, true);
	injector::WriteMemory<DWORD>(0x51B5F5, 0x999D1383, true); // "Time of Day"
	char LoadToEcx2[] = { 0x8B, 0x0D, 0x2C, 0x39, 0x9B, 0x00, 0x90, 0x90, 0x90, 0x8B, 0x49, 0x08 }; // mov ecx,[009B392C]; mov ecx,[ecx+08]
	injector::WriteMemoryRaw(0x51B617, LoadToEcx2, sizeof(LoadToEcx2), true);

	// VOTimeOfDay::SetInitialValues(void)
	char LoadTimeOfDay[] = { 0x8B, 0x15, 0x2C, 0x39, 0x9B, 0x00, 0x8B, 0x52, 0x08 }; // mov edx,[009B392C]; mov edx,[edx+08]
	injector::WriteMemoryRaw(0x50F9E0, LoadTimeOfDay, sizeof(LoadTimeOfDay), true);

	injector::MakeRangedNOP(0x50F9F5, 0x50F9F8, true); // nop fstp
	injector::WriteMemory<unsigned char>(0x0050F9F4, 0x52, true); // push edx

	injector::WriteMemory<float>(0x50F9F9, 0.01f, true); // Step Size
	injector::WriteMemory<float>(0x50F9FE, 0.95f, true); // Max Value
	injector::WriteMemory<float>(0x50FA03, 0.05f, true); // Min Value
}

void InitTime()
{
	TimeSetting();
	injector::MakeCALL(0x006BF2C4, CreateTimeOfDay);

	if (g_Config.RandomStartupTime)
	{
		srand(time(NULL));
		float r = (rand() % 10) / 10.0f;
		injector::WriteMemory<float>(0x00770F25, r);
	}
}