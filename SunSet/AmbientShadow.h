#pragma once
#include "Injector/injector.hpp"
#include "CarRenderInfo.h"
#include "eView.h"

int __stdcall ShouldDrawAmbientShadow(CarRenderInfo* carRenderInfo, eView* view)
{
	if (view->Id != ViewId::Player1 || carRenderInfo->pRideInfo->mCarRenderUsage == CarRenderUsage::AIHeli)
	{
		return 0x0074E843;
	}

	return 0x0074E818;
}

void __declspec(naked) DrawAmbientShadowHook()
{
	__asm
	{
		SAVE_REGS_EAX;
		push edi;
		push esi;
		call ShouldDrawAmbientShadow;
		RESTORE_REGS_EAX;

		jmp eax;
	}
}

void __cdecl NormalizeShadowShift(D3DXVECTOR3* dest, D3DXVECTOR3* v)
{
	dest->x = 0;
	dest->y = 0;
	dest->z = 1;
}

void __declspec(naked) DrawAmbientShadowMap()
{
	static constexpr auto cExit = 0x006E51CF;

	__asm
	{
		mov dword ptr ds:[0x00903328], 0;

		jmp cExit;
	}
}

TimeOfDay* fakeTod = new TimeOfDay();
void InitAmbientShadow()
{
	// Disable helicopter ambient shadow
	injector::MakeJMP(0x0074E812, DrawAmbientShadowHook);

	// Disable shadow shift
	injector::MakeCALL(0x00744084, NormalizeShadowShift);

	// Disable ambient shadow from shadow map
	injector::MakeJMP(0x006E51C9, DrawAmbientShadowMap);
}