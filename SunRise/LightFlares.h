#pragma once
#include "DynamicLights.h"
#include "Game.h"

void RenderWorldLightFlares()
{
	for (int i = 0; i < NumSpotLightBuffer; i++)
	{
		auto spotlight = SpotLightBuffer[i];

		LightFlare flare;
		flare.Postion = spotlight.Position;
		flare.Direction = { 1,0,0 };
		flare.ReflectPosZ = 0;
		flare.Tint = 0;
		flare.NameHash = 0;
		flare.Type = 0;
		flare.Flags = 0;

		Game::eRenderLightFlare(eView::PlayerView, &flare, (D3DXMATRIX*)0x00987AB0, 1, 0, 0, 0.0, 0, 3);
	}
}