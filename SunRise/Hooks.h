#pragma once
#include "Injector/injector.hpp"
#include "Game.h"
#include "Console.h"
#include "GrandSceneryCullInfo.h"
#include "DynamicLights.h"
#include "RenderModel.h"

void CheckReloadShaders()
{
#ifdef _DEBUG
	if (GetAsyncKeyState('F') & 1)
	{
		cusprintf("\nManual shader reload started:\n");
		Game::ReloadShaders = true;
	}
#endif 
}

void __cdecl SetuWorldCulling(GrandSceneryCullInfo* cullInfo)
{
	cullInfo->SetuWorldCulling();

	CheckReloadShaders();

	PopulateSpotLights(cullInfo);
}

void __stdcall SetShaderParams(RenderModel* renderModel)
{
	SetDynamicLights(renderModel);
}

void __declspec(naked) SetShaderParamsHook()
{
	static constexpr auto hExit = 0x006E03D8;

	__asm
	{
		pushad;
		push esi;
		call SetShaderParams;
		popad;

		mov edx, [esi + 0x0C];
		mov ecx, [esi + 0x3C];

		jmp hExit;
	}
}

void InitHooks()
{
	injector::MakeCALL(0x006DE3F5, SetuWorldCulling);

	injector::MakeJMP(0x006E03D2, SetShaderParamsHook);
}