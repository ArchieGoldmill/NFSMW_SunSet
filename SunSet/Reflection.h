#pragma once

void __stdcall CarReflection(int reflection, float* carRenderInfo, float* z)
{
	if (reflection)
	{
		float elev = carRenderInfo[0x45E];

		// from Carbon
		float v33 = *z - elev;
		if (v33 >= 0.1)
		{
			*z = *z - (v33 + v33);
		}
	}
}

void __declspec(naked) CarReflectionHook()
{
	static constexpr auto cExit = 0x007510CD;

	__asm
	{
		pushad;
		mov eax, esp;
		add eax, 0x48;
		push eax;
		push esi;
		push edx;
		call CarReflection;
		popad;

		test byte ptr[edi + 0x000003F8], 04;

		jmp cExit;
	}
}

void DrawFeReflections()
{
	StuffFeScenery();
	Game::CommitRenderedModels();
}

void InitReflection()
{
	injector::MakeJMP(0x007510C6, CarReflectionHook);

	// Real front end reflections
	if (g_Config.RealFeReflections)
	{
		injector::WriteMemory<BYTE>(0x006C5BB0, 0xEB);
		injector::MakeCALL(0x006DE7CA, DrawFeReflections);
		injector::MakeNOP(0x006DE66F, 6);
	}
}