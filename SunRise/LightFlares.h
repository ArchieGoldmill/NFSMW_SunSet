#pragma once
#include "DynamicLights.h"
#include "Game.h"

void RenderWorldLightFlares()
{
	for (int i = 0; i < NumSpotLightBuffer; i++)
	{
		auto spotlight = SpotLightBuffer[i];

		if (spotlight.Source == SpotLightSource::LampPost)
		{
			LightFlare flare;
			flare.Position = spotlight.Light.Position;
			flare.Direction = { 1, 0, 0 };
			flare.ReflectPosZ = 0;
			flare.Tint = 0;
			flare.NameHash = 0;
			flare.Type = eLightFlareType::lamppost;
			flare.Flags = 0;

			Game::eRenderLightFlare(eView::PlayerView, &flare, (D3DXMATRIX*)0x00987AB0, 1, 0, 0, 0.0, 0, 3);
		}
	}
}