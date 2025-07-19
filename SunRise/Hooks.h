#pragma once
#include "Injector/injector.hpp"
#include "Game.h"
#include "Console.h"
#include "GrandSceneryCullInfo.h"
#include "DynamicLights.h"
#include "RenderModel.h"
#include "RenderTarget.h"

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

void __stdcall SetCurrentPass(RenderModel* renderModel, eEffect* LastEffect)
{
	auto effect = renderModel->Effect;

	if (LastEffect != effect)
	{
		if (LastEffect)
		{
			LastEffect->D3DEffect->EndPass();
			LastEffect->End();
			LastEffect->D3DEffect->End();
			eEffect::Current = NULL;
		}

		eEffect::Current = effect;
	}

	auto id = effect->id;
	if (id == shader_type::WorldShader || id == shader_type::WorldReflectShader || id == shader_type::WorldNormalMap || id == shader_type::CarShader || id == shader_type::GlossyWindow || id == shader_type::ParticlesShader)
	{
		auto v11 = effect->D3DEffect->GetTechnique(effect->main_technique_number);
		effect->D3DEffect->SetTechnique(v11);
	}

	effect->Start();
	Game::Device->SetVertexDeclaration(effect->VertexDecl);
	effect->D3DEffect->Begin(&effect->PassCount, 0);
	effect->D3DEffect->BeginPass(0);

	SetShaderParams(renderModel);
}

void __declspec(naked) SetCurrentPassHook()
{
	static constexpr auto cExit = 0x006E03C3;

	__asm
	{
		SAVE_REGS_EAX;
		push ebx;
		push esi;
		call SetCurrentPass;
		RESTORE_REGS_EAX;

		xor ebp, ebp;

		jmp cExit;
	}
}

void InitHooks()
{
	injector::MakeCALL(0x006DE3F5, SetuWorldCulling);

	injector::MakeJMP(0x006E0254, SetCurrentPassHook);
}