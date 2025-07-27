#pragma once
#include "DynamicLights.h"
#include "Game.h"
#include "eLightFlareParams.h"

FlareModel* CurrentFlare = NULL;
void __stdcall RenderWorldLightFlares()
{
	bool isRoadReflection = RenderTarget::Current->ViewId == ViewId::Reflection;
	CurrentFlare = NULL;
	for (int i = 0; i < NumSpotLightBuffer; i++)
	{
		auto spotlight = SpotLightBuffer[i];

		if (spotlight.Source == SpotLightSource::LampPost && spotlight.Flare)
		{
			auto color = D3DCOLOR_COLORVALUE(spotlight.Light.Color.z, spotlight.Light.Color.y, spotlight.Light.Color.x, 0);
			CurrentFlare = spotlight.Flare;

			LightFlare flare;
			flare.Position = spotlight.Light.Position;
			flare.Direction = { 0, 0, 1 };
			flare.ReflectPosZ = 0;
			flare.Tint = 0;
			flare.NameHash = 0;
			flare.Type = spotlight.Flare->Type;
			flare.Flags = 0;

			float size = spotlight.Flare->Size;
			float intensity = spotlight.Flare->Intensity;
			if (isRoadReflection)
			{
				size *= 0.25f;
				intensity *= 0.25f;
			}

			Game::FlareRotation = 0;
			Game::eRenderLightFlare(eView::PlayerView, &flare, (D3DXMATRIX*)0x00987AB0, intensity, isRoadReflection, isRoadReflection * 2, 0, color, size);
			Game::FlareRotation = 240;
		}
	}
	CurrentFlare = NULL;
}

void __declspec(naked) RenderWorldLightFlaresHook()
{
	__asm
	{
		pushad;
		call RenderWorldLightFlares;
		popad;

		ret;
	}
}

TextureInfo* __stdcall GetFlareTexture(int num)
{
	if (CurrentFlare)
	{
		if (!CurrentFlare->Texture)
		{
			CurrentFlare->Texture = TextureInfo::Get(CurrentFlare->TextureName, false, false);
		}

		return CurrentFlare->Texture;
	}

	auto flareTextures = (TextureInfo**)(0x00915114);
	return flareTextures[num];
}

void __declspec(naked) GetFlareTextureHook()
{
	static constexpr auto cExit = 0x00505A43;

	__asm
	{
		SAVE_REGS_EDI;
		push ecx;
		call GetFlareTexture;
		mov edi, eax;
		RESTORE_REGS_EDI;

		jmp cExit;
	}
}

void InitLightFlares()
{
	injector::MakeJMP(0x00505F71, RenderWorldLightFlaresHook);

	injector::MakeJMP(0x00505A3C, GetFlareTextureHook);
}