#pragma once
#include "GodRays.h"
#include "Bloom.h"

void DrawPostEffects()
{
	__asm pushad;

	DrawBloom();
	DrawGodRays();

	__asm popad;
}

void InitPostEffects()
{
	injector::WriteMemory<BYTE>(0x006DF81D, 0xEB);

	injector::MakeNOP(0x006DF817, 6);
	injector::MakeCALL(0x006DF817, DrawPostEffects);
}