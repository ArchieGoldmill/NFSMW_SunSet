#pragma once
#include "CarRenderInfo.h"
#include "SpotLight.h"
#include "Config.h"
#include "VehicleRenderConn.h"
#include "FrontEndRenderingCar.h"
#include "HelicopterLight.h"
#include "Weather.h"
#include "PVehicle.h"

void PopulateCarLight(CarRenderInfo* carRenderInfo, Hash flareHash, SpotLight& spotlightCfg, D3DXMATRIX* matrix, SpotLightSource source)
{
	LightFlare* flares[2];
	int num = 0;

	LightFlare* flare = carRenderInfo->LightFlares;
	while ((void*)flare != &carRenderInfo->LightFlares && num < 2)
	{
		if (flare->NameHash == flareHash)
		{
			flares[num] = flare;
			num++;
		}

		flare = flare->Next;
	}

	if (num > 0)
	{
		D3DXVECTOR3 pos;

		if (num == 1)
		{
			pos = flares[0]->Position;
		}

		if (num == 2)
		{
			pos = (flares[0]->Position + flares[1]->Position) * 0.5;
		}

		SpotLight spotLight = spotlightCfg;
		if (flares[0]->Tint.rgba)
		{
			spotLight.Color.x = flares[0]->Tint.r / 255.0f;
			spotLight.Color.y = flares[0]->Tint.g / 255.0f;
			spotLight.Color.z = flares[0]->Tint.b / 255.0f;
		}

		spotLight.Color *= g_Weather.GetCarLightsPower();
		spotLight.Position += pos;

		D3DXVec3TransformCoord(&spotLight.Position, &spotLight.Position, matrix);

		D3DXVec3TransformNormal(&spotLight.Direction, &spotLight.Direction, matrix);
		D3DXVec3Normalize(&spotLight.Direction, &spotLight.Direction);

		AddSpotLightToBuffer(spotLight, source, NULL);
	}
}

void PopulateCarSpotLights(CarRenderInfo* carRenderInfo, D3DXMATRIX* matrix)
{
	D3DXVECTOR3 pos = { matrix->_41, matrix->_42, matrix->_43 };

	auto renderUsage = carRenderInfo->pRideInfo->mCarRenderUsage;
	bool isPlayer = renderUsage == CarRenderUsage::Player;

	auto headlightOn = carRenderInfo->IsLightOn(VehicleFX_HEADLIGHTS);
	if (headlightOn)
	{
		SpotLightSource source = isPlayer ? SpotLightSource::Player_Headlights : SpotLightSource::Headlights;
		PopulateCarLight(carRenderInfo, Hashes::LEFT_HEADLIGHT, CarHeadlighsConfig, matrix, source);
		PopulateCarLight(carRenderInfo, Hashes::RIGHT_HEADLIGHT, CarHeadlighsConfig, matrix, source);
	}

	auto brakelightOn = carRenderInfo->IsLightOn(VehicleFX_BRAKELIGHTS);
	SpotLight temp = brakelightOn ? CarBrakeLightsOnConfig : CarBrakeLightsOffConfig;
	SpotLightSource source = isPlayer ? SpotLightSource::Player_Breaklights : SpotLightSource::Breaklights;
	PopulateCarLight(carRenderInfo, Hashes::LEFT_BRAKELIGHT, temp, matrix, source);
	PopulateCarLight(carRenderInfo, Hashes::RIGHT_BRAKELIGHT, temp, matrix, source);

	auto reverseOn = carRenderInfo->IsLightOn(VehicleFX_REVERSE);
	if (reverseOn)
	{
		PopulateCarLight(carRenderInfo, Hashes::LEFT_REVERSE, CarReverseConfig, matrix, source);
		PopulateCarLight(carRenderInfo, Hashes::RIGHT_REVERSE, CarReverseConfig, matrix, source);
	}
}

bool IsRoadBlockCar(VehicleRenderConn* renderConn)
{
	auto sim = (int*)renderConn->SimCon;
	if (sim)
	{
		auto pVehicle = (PVehicle*)sim[0x12];
		if (pVehicle)
		{
			auto aiVehicle = pVehicle->GetAIVehiclePtr();
			if (aiVehicle)
			{
				return aiVehicle->GetRoadBlock() != NULL;
			}
		}
	}

	return false;
}

void PopulateCarSpotLights()
{
	if (Game::State == 6)
	{
		for (int i = 0; i < VehicleRenderConn::ListCount; i++)
		{
			auto renderConn = VehicleRenderConn::List[i];
			if (renderConn && !renderConn->Inactive)
			{
				auto carRenderInfo = renderConn->pCarRenderInfo;
				if (carRenderInfo)
				{
					auto renderUsage = carRenderInfo->pRideInfo->mCarRenderUsage;
					if (renderUsage == CarRenderUsage::AIHeli)
					{
						AddHelicopterLight(renderConn->Matrix);
					}
					else
					{
						bool populate = true;
						if (renderUsage == CarRenderUsage::AICop)
						{
							populate = !IsRoadBlockCar(renderConn);
						}

						if (populate)
						{
							PopulateCarSpotLights(carRenderInfo, &renderConn->Matrix1);
						}
					}
				}
			}
		}
	}

	if (Game::State == 3)
	{
		auto fecar = FrontEndRenderingCar::List;
		if (fecar && fecar->pCarRenderInfo && fecar->Visible)
		{
			PopulateCarSpotLights(fecar->pCarRenderInfo, &fecar->BodyMatrix);
		}
	}
}