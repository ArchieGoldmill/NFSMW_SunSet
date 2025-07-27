#pragma once
#include "Injector/injector.hpp"
#include "CarRenderInfo.h"
#include "eView.h"

int __stdcall ShouldDrawAmbientShadow(CarRenderInfo* carRenderInfo, eView* view)
{
	if (view->Id != 1 || carRenderInfo->pRideInfo->mMyCarRenderUsage == 5)
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

void InitAmbientShadow()
{
	injector::MakeJMP(0x0074E812, DrawAmbientShadowHook);
}