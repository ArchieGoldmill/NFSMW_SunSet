#pragma once
#include "Injector/injector.hpp"
#include "DirectResources.h"
#include "GrandSceneryCullInfo.h"
#include "DynamicLights.h"
#include "RenderModel.h"
#include "RenderTarget.h"
#include "LightFlares.h"
#include "HeavyRain.h"
#include "Weather.h"
#include "Time.h"
#include "MotionBlur.h"
#include "CarBrakeGlow.h"
#include "CarVinylMask.h"
#include "Reflection.h"
#include "AmbientShadow.h"
#include "UpdateMaterials.h"
#include "CustomMeshes.h"

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
	NumSpotLightBuffer = 0;

	cullInfo->SetuWorldCulling();

	PopulateSpotLights();

	ForceTime();

	CheckReloadShaders();

	UpdateBrakeGLow();

	g_Weather.Update();

	g_Rain.Update();

	UpdateMaterials();

	DoDepthPrePass(cullInfo);
}

TextureInfoSingleton StarsTexture(Hashes::SR_STARS);
void SetSkyTexture(RenderModel* renderModel)
{
	auto effect = renderModel->Effect;
	if (effect->id == shader_type::skyshader)
	{
		renderModel->DiffuseTextureInfo = StarsTexture.Get();
		effect->SetFloat(ShaderParam::cfSaturateSky, RenderTarget::Current->ViewId == ViewId::Player1 ? 0 : 1);
	}
}

TextureInfoSingleton PlainNormalTexture(Hashes::SR_PLAIN_NORMAL);
TextureInfoSingleton DamageNormalTexture(Hashes::DAMAGE0_N);
void __stdcall SetShaderParams(RenderModel* renderModel)
{
	SetDynamicLights(renderModel);
	SetSkyTexture(renderModel);

	bool hasNormalMap = renderModel->NormalTextureInfo &&
		renderModel->NormalTextureInfo != renderModel->DiffuseTextureInfo &&
		renderModel->NormalTextureInfo != TextureInfo::DefaultAlpha &&
		renderModel->NormalTextureInfo->NameHash != Hashes::WINDOWREFLECTION &&
		renderModel->NormalTextureInfo != renderModel->OpacityTextureInfo;

	if (!hasNormalMap)
	{
		if (renderModel->DiffuseTextureInfo->NameHash == Hashes::DAMAGE0)
		{
			renderModel->NormalTextureInfo = DamageNormalTexture.Get();
		}
		else
		{
			renderModel->NormalTextureInfo = PlainNormalTexture.Get();
		}
	}

	auto effect = renderModel->Effect;
	if (renderModel->DiffuseTextureInfo)
	{
		auto tilableUV = renderModel->DiffuseTextureInfo->TilableUV;
		effect->SetInt(ShaderParam::BaseAddressU, (tilableUV & 1) ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP);
		effect->SetInt(ShaderParam::BaseAddressV, (tilableUV & 2) ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP);
	}

	if (effect->id == shader_type::CarShader)
	{
		effect->SetBool(ShaderParam::cbUseNormalMap, hasNormalMap);

		auto material = renderModel->LightMaterial;
		effect->SetFloat(ShaderParam::cfMetallicScale, material->Metallic);
		effect->SetFloat(ShaderParam::cfSpecularHotSpot, material->SpecularHotSpot);

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
			D3DXHANDLE hTech = ShaderParamsMap[(int)effect->id].Techniques[techName];
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
	shaderParams.Techniques[Technique_LitPixel_32] = effect->D3DEffect->GetTechniqueByName("LitPixel_32");
	shaderParams.Techniques[Technique_ShadowMap] = effect->D3DEffect->GetTechniqueByName("ShadowMap");
	shaderParams.Techniques[Technique_Water] = effect->D3DEffect->GetTechniqueByName("Water");
	shaderParams.Techniques[Technique_Invisible] = effect->D3DEffect->GetTechniqueByName("Invisible");
	shaderParams.Techniques[Technique_ZPrePass] = effect->D3DEffect->GetTechniqueByName("ZPrePass");

	for (int i = 0; i < (int)ShaderParam::count; i++)
	{
		shaderParams.Params[i] = effect->D3DEffect->GetParameterByName(NULL, ShaderParamNames[i]);
	}

	ShaderParamsMap[(int)effect->id] = shaderParams;
}

void __cdecl NormalizeLightVec(D3DXVECTOR4* dest, D3DXVECTOR4* src)
{
	if (RenderTarget::Current->ViewId == ViewId::Reflection)
	{
		dest->z = -dest->z;
	}

	D3DXVec4Normalize(dest, src);
}

void SetDeviceParams()
{
	if (g_Config.MSAAx8)
	{
		Game::DeviceParams->MultiSampleType = D3DMULTISAMPLE_8_SAMPLES;
		Game::DeviceParams->MultiSampleQuality = 0;
	}
	else
	{
		Game::DeviceParams->MultiSampleType = D3DMULTISAMPLE_NONMASKABLE;
		Game::DeviceParams->MultiSampleQuality = Game::FSAALevel;
	}
}

inline shader_type GetModelId(RenderModel* model)
{
	return model->Effect ? model->Effect->id : shader_type::WorldShader;
}

inline std::uint64_t GetModelSort(RenderLight* rl, RenderModel* model, RenderingOrder* order)
{
	if (!model->Effect)
	{
		return 0;
	}

	if (model->SortOrder > 0x80000000)
	{
		return order->ToInt64();
	}

	return rl->Technique + (int)model->Effect->id * 100;
}

int RenderModelSize = 4096;

void __cdecl SortRenderModels(RenderingOrder* first, RenderingOrder* last, int count)
{
	if (DepthPrePass || RenderTarget::Current->ViewId == ViewId::ShadowMap)
	{
		std::sort(first, last, [](RenderingOrder& a, RenderingOrder& b)
			{
				auto model1 = RenderModel::List + a.model_index;
				auto model2 = RenderModel::List + b.model_index;

				return GetModelId(model1) < GetModelId(model2);
			});
	}
	else if (RenderTarget::Current->ViewId == ViewId::Player1)
	{
		for (int i = 0; i < RenderModelSize; i++)
		{
			RenderLights[i].num = -1;
		}

		std::sort(first, last, [](RenderingOrder& a, RenderingOrder& b)
			{
				auto model1 = RenderModel::List + a.model_index;
				auto model2 = RenderModel::List + b.model_index;

				auto rl1 = RenderLights + a.model_index;
				auto rl2 = RenderLights + b.model_index;

				PopulateTechnique(model1, rl1);
				PopulateTechnique(model2, rl2);

				return GetModelSort(rl1, model1, &a) < GetModelSort(rl2, model2, &b);
			});
	}
	else
	{
		std::sort(first, last);
	}
}

void __cdecl NormalizeSunDirForShadow(D3DXVECTOR3* dest, D3DXVECTOR3* src)
{
	D3DXVECTOR3 dir = *src;
	D3DXVec3Normalize(&dir, &dir);

	dir.z = max(0.3, dir.z);
	D3DXVec3Normalize(&dir, &dir);

	*dest = dir;
}

void __declspec(naked) NosEffectCave()
{
	static constexpr auto cExit = 0x0075634B;

	__asm
	{
		mov al, [ecx + 0x71];
		mov ebp, 0x6B7916BD; // fxcar_nos
		test al, al;
		je NosEffectCave_Exit;
		mov ebp, 0x138f6983; // fxcar_nos_2

	NosEffectCave_Exit:
		jmp cExit;
	}
}

void InitHooks()
{
	InitLightFlares();
	InitTime();
	InitMotionBlur();
	InitDepthPrePass();
	InitCarVinylMask();
	InitCustomMeshes();
	InitReflection();
	InitAmbientShadow();
	InitHeavyRain();

	injector::MakeCALL(0x006DE3F5, SetuWorldCulling);

	injector::MakeCALL(0x00731B63, DrawScenery);

	injector::MakeJMP(0x006E0254, SetCurrentPassHook);

	injector::MakeCALL(0x006C60D9, SetEffectParams);
	injector::MakeCALL(0x006DB2B0, SetEffectParams);

	// Disable Texture Headlights
	injector::WriteMemory<BYTE>(0x007429E0, 0xEB);
	injector::WriteMemory<Hash>(0x00738180, 0x83000CC2);

	// Disable sun direction
	injector::MakeNOP(0x0076956E, 5);

	if (g_Config.DisableSunFlare)
	{
		injector::MakeNOP(0x00504C56, 5);
		injector::MakeNOP(0x006DF57B, 5);
		injector::MakeNOP(0x006DF585, 5);
	}
	else
	{
		injector::MakeNOP(0x006C0167, 4);
		injector::WriteMemory<float*>(0x006C0162, g_Weather.GetSunFlare());
	}

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

	// Remove road reflection blur
	injector::WriteMemory<BYTE>(0x006DE596, 4);

	injector::MakeCALL(0x006C82B9, NormalizeLightVec);

	// MSAA x8
	injector::MakeNOP(0x006BFBB0, 6);
	injector::MakeCALL(0x006BFBB0, SetDeviceParams);

	// Model sorting
	injector::MakeCALL(0x006E2F73, SortRenderModels);

	// Disable ShadowPolyCurrentGen
	injector::WriteMemory<BYTE>(0x006DAC03, 0xEB);
	injector::WriteMemory<BYTE>(0x006E5352, 0xEB);

	// Disable max shadow detail
	injector::MakeNOP(0x006D24C5, 2);

	// Clamp sun dir for shadows
	injector::MakeCALL(0x006E455A, NormalizeSunDirForShadow);

	if (g_Config.NosEmmiter)
	{
		injector::MakeJMP(0x00756346, NosEffectCave);
	}

	auto RenderModelSize = *(int*)(0x006E0AD4);
	RenderLights = new RenderLight[RenderModelSize];

	RenderModel::List = *(RenderModel**)(0x006E0AEC);

#ifdef _DEBUG
	injector::MakeNOP(0x006C2206, 10);
#endif
}