#pragma once
#include "DynamicLights.h"
#include "Game.h"
#include "eLightFlareParams.h"

FlareModel* CurrentFlare = NULL;
FlareModel RainFlare;
void __stdcall RenderWorldLightFlares()
{
	auto viewId = RenderTarget::Current->ViewId;
	bool isRoadReflection = viewId == ViewId::Reflection;
	auto view = eView::Get(viewId);

	CurrentFlare = NULL;
	for (int i = 0; i < NumSpotLightBuffer; i++)
	{
		auto spotlight = SpotLightBuffer[i];
		if (spotlight.Flare && (spotlight.Source == SpotLightSource::LampPost || spotlight.Source == SpotLightSource::Helicopter))
		{
			auto color = D3DCOLOR_COLORVALUE(spotlight.Light.Color.z, spotlight.Light.Color.y, spotlight.Light.Color.x, 0);
			CurrentFlare = spotlight.Flare;

			LightFlare flare;
			flare.Position = spotlight.Light.Position;
			flare.Direction = { 0, 0, 1 };
			flare.ReflectPosZ = 0;
			flare.Tint.rgba = 0;
			flare.NameHash = 0;
			flare.Type = spotlight.Flare->Type;
			flare.Flags = 0;

			float size = spotlight.Flare->Size;
			float intensity = spotlight.Flare->Intensity;
			if (isRoadReflection)
			{
				size *= 0.5f;
			}

			intensity *= spotlight.FlareIntecity;

			float flareRotation = Game::FlareRotation;
			Game::FlareRotation = 0;
			{
				Game::eRenderLightFlare(view, &flare, Game::IdentityMatrix, intensity, isRoadReflection, isRoadReflection * 2, 0, color, size);

				if (viewId == ViewId::Player1 && spotlight.Flare->NameHash == Hashes::LampPost)
				{
					auto cameraDist = GetCameraDistance(flare.Position);
					intensity *= Smoothstep(1, 100, cameraDist) * g_Rain.GetRain() * 0.1;
					if (intensity > 0)
					{
						RainFlare.TextureName = Game::bStringHash1("_RAIN", spotlight.Flare->TextureName);
						CurrentFlare = &RainFlare;
						Game::eRenderLightFlare(view, &flare, Game::IdentityMatrix, intensity, 0, 0, 0, color, 15);
					}
				}
			}
			Game::FlareRotation = flareRotation;
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

	injector::MakeNOP(0x00742CB9, 8);

	injector::WriteMemory(&Game::FlareDistanceEnd, g_Config.FlareDistance * g_Config.FlareDistance);
	injector::WriteMemory(&Game::FlareDistanceStart, Game::FlareDistanceEnd - 8000.0f);
}