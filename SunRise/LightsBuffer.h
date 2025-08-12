#pragma once
#include "Spotlight.h"

#define NUM_SPOTLIGHTS_BUFFER 512
SpotLightModel SpotLightBuffer[NUM_SPOTLIGHTS_BUFFER];
int NumSpotLightBuffer = 0;

void AddSpotLightToBuffer(SpotLight spotLight, SpotLightSource source, FlareModel* flare, float flareIntecity = 1.0)
{
	if (spotLight.Color == D3DXVECTOR3(0, 0, 0))
	{
		return;
	}

	if (NumSpotLightBuffer >= NUM_SPOTLIGHTS_BUFFER)
	{
#ifdef _DEBUG
		throw std::runtime_error("Light buffer is full");
#endif
		return;
	}

	// Make sure we dont add the same light twice (TODO: how that happens?)
	for (int i = 0; i < NumSpotLightBuffer; i++)
	{
		auto s = SpotLightBuffer[i];
		if (s.Light.Position == spotLight.Position)
		{
			return;
		}
	}

	// Check that light is in camera view
	auto playerView = eView::PlayerView;

	D3DXVECTOR3 range = { spotLight.Range, spotLight.Range, spotLight.Range };
	D3DXVECTOR3 min = spotLight.Position - range;

	if (spotLight.Direction == D3DXVECTOR3(0, 0, -1) && spotLight.OuterAngle < 180 && spotLight.InnerAngle < 180)
	{
		range.z = 0;
	}

	D3DXVECTOR3 max = spotLight.Position + range;

	auto visibleState = playerView->GetVisibleState(&min, &max);
	if (visibleState == visible_state::outside)
	{
		return;
	}

	SpotLightModel spotLightModel;
	spotLightModel.Light = spotLight;
	spotLightModel.Source = source;
	spotLightModel.Flare = flare;
	spotLightModel.FlareIntecity = flareIntecity;

	SpotLightBuffer[NumSpotLightBuffer] = spotLightModel;
	NumSpotLightBuffer++;
}