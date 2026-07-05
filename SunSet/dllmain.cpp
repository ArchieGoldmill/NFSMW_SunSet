#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

//#define EDITOR

#include <fstream>
#include "Console.h"
#include "ShaderLoader.h"
#include "InitConfig.h"
#include "Hooks.h"

#ifdef EDITOR
#include "UI.h"
#endif

void __declspec(naked) Reset()
{
	__asm pushad;

	ReleaseDirectResources();

#ifdef EDITOR
	UI::Reset();
#endif

	__asm popad;
	__asm ret;
}

void Init()
{
	InitConfig();

	Game::SkipFE = g_Config.SkipFE;

	if (g_Config.Console)
	{
		InitConsole();
	}

	InitShaderLoader();
	InitHooks();

#ifdef EDITOR
	UI::Init();
#endif

	injector::MakeJMP(0x006C343B, Reset);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		uintptr_t base = (uintptr_t)GetModuleHandleA(NULL);
		IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)(base);
		IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);

		if ((base + nt->OptionalHeader.AddressOfEntryPoint + (0x400000 - base)) == 0x7C4040) // Check if .exe file is compatible - Thanks to thelink2012 and MWisBest
		{
			Game::hModule = hModule;
			Init();
		}
		else
		{
			MessageBoxA(NULL, "This .exe is not supported.\nPlease use v1.3 English speed.exe (5,75 MB (6.029.312 bytes)).", ModName, MB_ICONERROR);
			return FALSE;
		}
	}

	return TRUE;
}