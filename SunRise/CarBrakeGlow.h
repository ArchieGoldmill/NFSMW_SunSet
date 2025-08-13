#pragma once
#include "Utilities.h"

float BrakeGlow = 0;
void ApplyCarBrakeGlow(RenderModel* renderModel)
{
	D3DXVECTOR4 col = { 1, 1, 1, 0 };

	if (Game::State == 6)
	{
		if (renderModel->LightMaterial && renderModel->LightMaterial->NameHash == Hashes::BRAKEDISC)
		{
			if (PVehicle::Player)
			{
				auto speed = PVehicle::Player->GetSpeed();
				auto braking = PVehicle::Player->GetBraking();
				if (speed > 0 && braking > 0)
				{
					MoveTowards(BrakeGlow, 6, Game::DeltaTime * 0.5);
				}
				else
				{
					MoveTowards(BrakeGlow, 0, Game::DeltaTime * 0.05);
				}

				float glow = 0;
				if (BrakeGlow > 3)
				{
					glow = ConvertRange(BrakeGlow, 3, 6, 0, 1);
				}

				col = LerpVector(col, { 3.0f, 1.0f, 0, 1 }, glow);
			}
		}
	}

	renderModel->Effect->SetVector(ShaderParam::cvCarEmissive, &col);
}