#pragma once
#include "DynamicLights.h"
#include "Game.h"

void RenderWorldLightFlares()
{
	for (int i = 0; i < NumSpotLightBuffer; i++)
	{
		auto spotlight = SpotLightBuffer[i];

		if (spotlight.Source == SpotLightSource::LampPost && spotlight.Flare)
		{
			auto color = D3DCOLOR_COLORVALUE(spotlight.Flare->Color.z, spotlight.Flare->Color.y, spotlight.Flare->Color.x, 0);

			LightFlare flare;
			flare.Position = spotlight.Light.Position;
			flare.Direction = { 1, 0, 0 };
			flare.ReflectPosZ = 0;
			flare.Tint = 0;
			flare.NameHash = 0;
			flare.Type = spotlight.Flare->Type;
			flare.Flags = 0;

			Game::eRenderLightFlare(eView::PlayerView, &flare, (D3DXMATRIX*)0x00987AB0, spotlight.Flare->Intensity, 0, 0, 0.0, color, spotlight.Flare->Size);
		}
	}
}