#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "Console.h"
#include "ShaderLoader.h"
#include "Hooks.h"

void PopulateSolidLights()
{
	auto solidLights = new SolidLights();
	solidLights->SolidName = 0x2FC6951B;

	auto spotLight = new SpotLight();
	spotLight->Position = { 0, 0, 10 };
	spotLight->Direction = { 0.0f, 0.0f, -1.0f };
	spotLight->Range = 30.0f;
	spotLight->Intensity = 5.0f;
	spotLight->Power = 2.9f;
	spotLight->Color = { 1,1,1 };

	solidLights->Lights.push_back(spotLight);
	SolidLightsList.push_back(solidLights);
}

void Init()
{
#ifdef _DEBUG
	auto skipFE = (int*)0x00926064;
	*skipFE = 1;
#endif 

	InitConsole();
	InitShaderLoader();
	InitHooks();

	PopulateSolidLights();
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
			Init();
		}
		else
		{
			MessageBoxA(NULL, "This .exe is not supported.\nPlease use v1.3 English speed.exe (5,75 MB (6.029.312 bytes)).", "NFSMW - Shader Loader", MB_ICONERROR);
			return FALSE;
		}
	}

	return TRUE;
}