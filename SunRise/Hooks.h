#pragma once
#include "Injector/injector.hpp"
#include "GrandSceneryCullInfo.h"
#include "DynamicLights.h"
#include "RenderModel.h"
#include "RenderTarget.h"
#include "LightFlares.h"
#include "Weather.h"
#include "Time.h"
#include "MotionBlur.h"
#include "CarBrakeGlow.h"
#include "CarVinylMask.h"

bool ReloadOnFocus = false;
void CheckReloadShaders()
{
	if (g_Config.HK_ShaderReload)
	{
		if (GetAsyncKeyState(g_Config.HK_ShaderReload) & 1)
		{
			cusprintf("\nManual shader reload started:\n");
			Game::ReloadShaders = true;
		}
	}

	if (g_Config.LiveReload)
	{
		if (Game::LostFocus)
		{
			ReloadOnFocus = true;
		}
		else
		{
			if (ReloadOnFocus)
			{
				InitConfig();
				cusprintf("\nConfig reloaded\n");
				ReloadOnFocus = false;
			}
		}
	}
}

void __cdecl SetuWorldCulling(GrandSceneryCullInfo* cullInfo)
{
	cullInfo->SetuWorldCulling();

	ForceTime();

	CheckReloadShaders();

	PopulateSpotLights(cullInfo);

	UpdateBrakeGLow();

	g_Weather.Update();

	if (g_Config.DepthPrepass)
	{
		DoDepthPrePass(cullInfo);
	}
}

TextureInfo* StarsTexture = NULL;
void SetSkyTexture(RenderModel* renderModel)
{
	auto effect = renderModel->Effect;
	if (effect->id == shader_type::skyshader)
	{
		if (!StarsTexture)
		{
			StarsTexture = TextureInfo::Get(Hashes::SR_STARS, false, false);
		}

		renderModel->DiffuseTextureInfo = StarsTexture;
	}
}

TextureInfo* PlainNormalTexture = NULL;
void __stdcall SetShaderParams(RenderModel* renderModel)
{
	SetDynamicLights(renderModel);
	SetSkyTexture(renderModel);

	bool hasNormalMap = renderModel->NormalTextureInfo && 
		renderModel->NormalTextureInfo != renderModel->DiffuseTextureInfo && 
		renderModel->NormalTextureInfo != TextureInfo::DefaultAlpha;

	if (!hasNormalMap)
	{
		if (!PlainNormalTexture)
		{
			PlainNormalTexture = TextureInfo::Get(Hashes::SR_PLAIN_NORMAL, false, false);
		}

		renderModel->NormalTextureInfo = PlainNormalTexture;
	}

	auto effect = renderModel->Effect;
	if (effect->id == shader_type::CarShader)
	{
		effect->SetBool(ShaderParam::cbUseNormalMap, hasNormalMap);

		auto material = renderModel->LightMaterial;
		effect->SetFloat(ShaderParam::cfMetallicScale, material->Metallic);

		SetVinylScale(renderModel);
		ApplyCarBrakeGlow(renderModel);
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
	auto techName = Technique_Invalid;

	if (DepthPrePass)
	{
		techName = Technique_ZPrePass;
	}
	else
	{
		techName = GetTechnique(renderModel);
	}

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

	if (DepthPrePass)
	{
		SetSkyTexture(renderModel);
	}
	else
	{
		SetShaderParams(renderModel);
	}
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
	shaderParams.Techniques[Technique_Prelit] = effect->D3DEffect->GetTechniqueByName("Prelit");
	shaderParams.Techniques[Technique_LitPixel_4] = effect->D3DEffect->GetTechniqueByName("LitPixel_4");
	shaderParams.Techniques[Technique_LitPixel_8] = effect->D3DEffect->GetTechniqueByName("LitPixel_8");
	shaderParams.Techniques[Technique_LitPixel_16] = effect->D3DEffect->GetTechniqueByName("LitPixel_16");
	shaderParams.Techniques[Technique_LitPixel_24] = effect->D3DEffect->GetTechniqueByName("LitPixel_24");
	shaderParams.Techniques[Technique_LitVertex] = effect->D3DEffect->GetTechniqueByName("LitVertex");
	shaderParams.Techniques[Technique_ShadowMap] = effect->D3DEffect->GetTechniqueByName("ShadowMap");
	shaderParams.Techniques[Technique_Water] = effect->D3DEffect->GetTechniqueByName("Water");
	shaderParams.Techniques[Technique_ZPrePass] = effect->D3DEffect->GetTechniqueByName("ZPrePass");

	for (int i = 0; i < (int)ShaderParam::count; i++)
	{
		shaderParams.Params[i] = effect->D3DEffect->GetParameterByName(NULL, ShaderParamNames[i]);
	}

	ShaderParamsMap[effect->id] = shaderParams;
}

void InitHooks()
{
	InitLightFlares();
	InitTime();
	InitMotionBlur();
	InitDepthPrePass();
	InitCarVinylMask();
	InitCustomMeshes();

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

	// Disable vanilla time propagation
	injector::WriteMemory<unsigned short>(0x0076938E, 0x74EB);

	if (g_Config.X360Effects)
	{
		injector::MakeNOP(0x006C1841, 5);
		Game::X360EffectsEnable = true;
	}

	// Improve reflection lods
	injector::WriteMemory<uint32_t>(0x6BFEBD, 0x00006002, true);

	// Remove road reflection blur
	injector::WriteMemory<BYTE>(0x006DE596, 4);

#ifdef _DEBUG
	injector::MakeNOP(0x006C2206, 10);
#endif
}