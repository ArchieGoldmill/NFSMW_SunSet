#pragma once
#include "Spotlight.h"

#define NUM_SPOTLIGHTS_BUFFER 512
SpotLightModel SpotLightBuffer[NUM_SPOTLIGHTS_BUFFER];
int NumSpotLightBuffer = 0;

void AddSpotLightToBuffer(SpotLight spotLight, SpotLightSource source, FlareModel* flare, float flareIntecity = 1.0f)
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

	// Make sure we dont add the same light twice
	for (int i = 0; i < NumSpotLightBuffer; i++)
	{
		auto s = SpotLightBuffer[i];
		if (s.Light.Position == spotLight.Position)
		{
			return;
		}
	}

	// Check that light is in camera view
	D3DXVECTOR3 min, max;
	spotLight.GetBoundingBox(&min, &max);

	auto visibleState = eView::Player->GetVisibleState(&min, &max);
	if (visibleState == visible_state::outside)
	{
		return;
	}

	int pixel = eView::Player->GetPixelSize(&spotLight.Position, spotLight.Range);
	if (pixel < 4)
	{
		return;
	}

	SpotLightModel spotLightModel;
	spotLightModel.Light = spotLight;
	spotLightModel.Source = source;
	spotLightModel.Flare = flare;
	spotLightModel.FlareIntecity = flareIntecity;
	spotLightModel.Added = false;
	spotLightModel.Distance = 0.0f;

	SpotLightBuffer[NumSpotLightBuffer] = spotLightModel;
	NumSpotLightBuffer++;
}

void ClampLights()
{
	const int MaxLights = g_Config.MaxLights;

	if (NumSpotLightBuffer > MaxLights && MaxLights > 0)
	{
		for (int i = 0; i < NumSpotLightBuffer; i++)
		{
			SpotLightBuffer[i].Distance = GetCameraDistance(SpotLightBuffer[i].Light.Position);
		}

		std::sort(SpotLightBuffer, SpotLightBuffer + NumSpotLightBuffer, [](SpotLightModel& a, SpotLightModel& b) { return a.Distance < b.Distance; });

		NumSpotLightBuffer = MaxLights;
	}
}