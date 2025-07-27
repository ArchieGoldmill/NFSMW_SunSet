#pragma once
#include "Injector/injector.hpp"
#include "Game.h"
#include "Console.h"
#include "GrandSceneryCullInfo.h"
#include "DynamicLights.h"
#include "RenderModel.h"
#include "RenderTarget.h"
#include "LightFlares.h"
#include "Weather.h"

bool ReloadOnFocus = false;
void CheckReloadShaders()
{
	if (GetAsyncKeyState('F') & 1)
	{
		cusprintf("\nManual shader reload started:\n");
		Game::ReloadShaders = true;

		InitConfig();
	}

	if (Game::LostFocus)
	{
		ReloadOnFocus = true;
	}
	else
	{
		if (ReloadOnFocus)
		{
			InitConfig();
			cusprintf("\nConfig reload on focus\n");
			ReloadOnFocus = false;
		}
	}
}

float GLobalTime = 0.3f;
void __cdecl SetuWorldCulling(GrandSceneryCullInfo* cullInfo)
{
	cullInfo->SetuWorldCulling();

	if (g_Config.ForceTime >= 0.0f)
	{
		TimeOfDay::Instance->CurrentTime = g_Config.ForceTime;
	}
	else
	{
		GLobalTime += Game::DeltaTime * TimeOfDay::Instance->UpdateRate * 0.001;
		if (GLobalTime > 1)
		{
			GLobalTime = 0;
		}

		TimeOfDay::Instance->CurrentTime = GLobalTime;
	}

	CheckReloadShaders();

	PopulateSpotLights(cullInfo);

	g_Weather.Update();
}

TextureInfo* StarsTexture = NULL;
TextureInfo* PlainNormalTexture = NULL;
void __stdcall SetShaderParams(RenderModel* renderModel)
{
	SetDynamicLights(renderModel);

	if (renderModel->Effect->id == shader_type::skyshader)
	{
		if (StarsTexture)
		{
			renderModel->NormalTextureInfo = renderModel->DiffuseTextureInfo;
			renderModel->DiffuseTextureInfo = StarsTexture;
		}
		else
		{
			StarsTexture = TextureInfo::Get(Hashes::SR_STARS, false, false);
		}
	}

	if (renderModel->Effect->id == shader_type::CarShader)
	{
		auto material = (float*)renderModel->LightMatertial;
		renderModel->Effect->SetFloat(ShaderParam::cfMetallicScale, material[40]);
	}

	if (renderModel->DiffuseTextureInfo == renderModel->NormalTextureInfo)
	{
		if (PlainNormalTexture)
		{
			renderModel->NormalTextureInfo = PlainNormalTexture;
		}
		else
		{
			PlainNormalTexture = TextureInfo::Get(Hashes::SR_PLAIN_NORMAL, false, false);
		}
	}
}

TechniqueType LastTechnique = Technique_Invalid;
void __stdcall SetCurrentPass(RenderModel* renderModel, eEffect* LastEffect)
{
	if (LastEffect == NULL)
	{
		LastTechnique = Technique_Invalid;
	}

	auto effect = renderModel->Effect;

	auto techName = GetTechnique(renderModel);

	if (LastEffect != effect || LastTechnique != techName)
	{
		if (LastEffect)
		{
			LastEffect->D3DEffect->EndPass();
			LastEffect->End();
			LastEffect->D3DEffect->End();
			eEffect::Current = NULL;
		}

		if (techName != Technique_Invalid)
		{
			D3DXHANDLE hTech = ShaderParamsMap[effect->id].Techniques[techName];
			effect->D3DEffect->SetTechnique(hTech);
		}
		else
		{
			effect->D3DEffect->SetTechnique(effect->main_technique_handle);
		}

		eEffect::Current = effect;

		LastTechnique = techName;

		effect->Start();
		Game::Device->SetVertexDeclaration(effect->VertexDecl);
		effect->D3DEffect->Begin(&effect->PassCount, 0);
		effect->D3DEffect->BeginPass(0);
	}

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

void __fastcall SetEffectParams(eEffect* effect)
{
	effect->SetParams();

	ShaderParams shaderParams;

	shaderParams.Techniques[Technique_Unlit] = effect->D3DEffect->GetTechniqueByName("Unlit");
	shaderParams.Techniques[Technique_LitPixel_8] = effect->D3DEffect->GetTechniqueByName("LitPixel_8");
	shaderParams.Techniques[Technique_LitPixel_16] = effect->D3DEffect->GetTechniqueByName("LitPixel_16");
	shaderParams.Techniques[Technique_LitPixel_24] = effect->D3DEffect->GetTechniqueByName("LitPixel_24");
	shaderParams.Techniques[Technique_LitVertex] = effect->D3DEffect->GetTechniqueByName("LitVertex");
	shaderParams.Techniques[Technique_ShadowMap] = effect->D3DEffect->GetTechniqueByName("ShadowMap");

	for (int i = 0; i < (int)ShaderParam::count; i++)
	{
		shaderParams.Params[i] = effect->D3DEffect->GetParameterByName(NULL, ShaderParamNames[i]);
	}

	ShaderParamsMap[effect->id] = shaderParams;
}

void InitHooks()
{
	InitLightFlares();

	injector::MakeCALL(0x006DE3F5, SetuWorldCulling);

	injector::MakeJMP(0x006E0254, SetCurrentPassHook);

	injector::MakeCALL(0x006C60D9, SetEffectParams);
	injector::MakeCALL(0x006DB2B0, SetEffectParams);

	// Disable Texture Headlights
	injector::WriteMemory<BYTE>(0x007429E0, 0xEB);

	// Disable sun direction
	injector::MakeNOP(0x0076956E, 5);

	// Disable sun flare
	injector::MakeNOP(0x00504C56, 5);
	injector::MakeNOP(0x006DF57B, 5);
	injector::MakeNOP(0x006DF585, 5);

	// Disable tunnel bloom
	injector::MakeNOP(0x00504C65, 5);
	injector::MakeNOP(0x00504C6F, 5);

	// Disable car flare reflection
	injector::WriteMemory<BYTE>(0x0074322E, 0);
}