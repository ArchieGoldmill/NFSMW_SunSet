#pragma once
#include <d3dx9.h>
#include "Injector/injector.hpp"
#include "Utilities.h"
#include "Config.h"

HRESULT WINAPI D3DXCreateEffectFromResourceHook(const char* efxPath, LPDIRECT3DDEVICE9 pDevice, HMODULE hSrcModule, LPCSTR pSrcResource, D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors)
{
	HRESULT result;

	char FilenameBuf[256];
#ifdef _DEBUG
	strcpy(FilenameBuf, "D:\\Programming\\NFSMW\\NFSMW_SunSet\\");
	strcpy(FilenameBuf + 34, efxPath);
#else
	strcpy(FilenameBuf, efxPath);
#endif
	char* LastUnderline = strrchr(FilenameBuf, '.');
	LastUnderline[1] = 'f';
	LastUnderline[2] = 'x';
	LastUnderline[3] = '\0';

	char IdiPath[256];
	strcpy(IdiPath, "shaders\\");
	strcpy(IdiPath + 8, pSrcResource);

	if (FileExists(FilenameBuf) && g_Config.ShaderCompiler)
	{
		ID3DXEffectCompiler* pEffectCompiler;
		ID3DXBuffer* pBuffer, * pEffectBuffer;

		result = D3DXCreateEffectCompilerFromFileA(FilenameBuf, NULL, NULL, 0, &pEffectCompiler, &pBuffer);
		if (SUCCEEDED(result))
		{
			result = pEffectCompiler->CompileEffect(0, &pEffectBuffer, &pBuffer);
			if (!SUCCEEDED(result))
			{
				cus_puts(*(char**)(pBuffer + 3));
				cusprintf("HRESULT: %X\n", result);
				return result;
			}

			cusprintf("%s : Compilation successful!\n", FilenameBuf);

			unsigned int CurrentEffectSize = *(unsigned int*)(pEffectBuffer + 2);
			WriteFileFromMemory(IdiPath, *(void**)(pEffectBuffer + 3), CurrentEffectSize);

			result = D3DXCreateEffect(pDevice, *(void**)(pEffectBuffer + 3), *(unsigned int*)(pEffectBuffer + 2), pDefines, pInclude, Flags, pPool, ppEffect, &pBuffer);
			if (!SUCCEEDED(result))
			{
				cusprintf("Effect creation failed: HRESULT: %X\n", result);
				cus_puts(*(char**)(pBuffer + 3));
			}

			pEffectCompiler->Release();
			pEffectBuffer->Release();
			return result;
		}
		else
		{
			cusprintf("Error compiling shader: HRESULT: %X\n", result);
			cus_puts(*(char**)(pBuffer + 3));
		}
	}

	if (FileExists(IdiPath))
	{
		result = D3DXCreateEffectFromFileA(pDevice, IdiPath, pDefines, pInclude, Flags, pPool, ppEffect, ppCompilationErrors);
		if (SUCCEEDED(result) && !g_Config.ShaderCompiler)
		{
			cusprintf("%s : successfully loaded!\n", IdiPath);
		}
	}
	else
	{
		result = D3DXCreateEffectFromResourceA(pDevice, hSrcModule, pSrcResource, pDefines, pInclude, Flags, pPool, ppEffect, ppCompilationErrors);
		//cusprintf("%s : successfully loaded!\n", pSrcResource);
	}

	return result;
}

void __declspec(naked) D3DXCreateEffectFromResourceCave()
{
	static constexpr auto cExit = 0x006C60D7;
	__asm
	{
		mov eax, 0x008F9BE4;
		add eax, edx;
		mov eax, [eax];
		push eax;
		call D3DXCreateEffectFromResourceHook;

		jmp cExit;
	}
}

void InitShaderLoader()
{
	if (g_Config.ShaderLoader)
	{
		injector::MakeJMP(0x006C60D2, D3DXCreateEffectFromResourceCave);
	}
}