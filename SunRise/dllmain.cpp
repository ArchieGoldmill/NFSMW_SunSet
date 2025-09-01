#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <fstream>
#include "Console.h"
#include "ShaderLoader.h"
#include "Hooks.h"
#include "Config.h"
#include "AmbientShadow.h"
#include "UI.h"

bool CheckFiles()
{
	if (FileExists("XNFS-ShaderLoader-MW.asi"))
	{
		MessageBoxA(NULL, "XNFS-ShaderLoader-MW.asi detected, please remove it for proper shader loading.", ModName, MB_ICONEXCLAMATION);
		return false;
	}

	if (!FileExists("TexWizard.asi"))
	{
		MessageBoxA(NULL, "TexWizard.asi not found, please follow the install guide to fix the issue.", ModName, MB_ICONEXCLAMATION);
		return false;
	}

	if (!FileExists("TexWizard.json"))
	{
		MessageBoxA(NULL, "TexWizard.json not found, please follow the install guide to fix the issue.", ModName, MB_ICONEXCLAMATION);
		return false;
	}

	if (!FileExists("SunSetData\\Textures\\SKYBOX.dds"))
	{
		MessageBoxA(NULL, "SKYBOX.dds not found, please follow the install guide to fix the issue.", ModName, MB_ICONEXCLAMATION);
		return false;
	}

	std::ifstream file("TexWizard.json");
	std::ostringstream buffer;
	buffer << file.rdbuf();
	std::string content = buffer.str();
	std::string keyword = "TexturePacks\\\\SunSet";
	if (content.find(keyword) == std::string::npos)
	{
		MessageBoxA(NULL, "SunSet texture pack not found in TexWizard.json, please follow the install guide to fix the issue.", ModName, MB_ICONEXCLAMATION);
		return false;
	}

	return true;
}

void Init()
{
	if (!CheckFiles())
	{
		return;
	}

	InitConfig();

	if (g_Config.SkipFE)
	{
		Game::SkipFE = 1;
	}

	if (g_Config.Console)
	{
		InitConsole();
	}

	InitShaderLoader();
	InitHooks();
	InitAmbientShadow();
	UI::Init();
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