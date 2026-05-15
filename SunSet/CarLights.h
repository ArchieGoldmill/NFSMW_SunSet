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
		spotLight.Specular *= std::lerp(0.5, 1, g_Rain.GetRoadWetness());

		D3DXVec3TransformCoord(&spotLight.Position, &spotLight.Position, matrix);

		D3DXVec3TransformNormal(&spotLight.Direction, &spotLight.Direction, matrix);
		D3DXVec3Normalize(&spotLight.Direction, &spotLight.Direction);

		AddSpotLightToBuffer(spotLight, source, NULL);
	}
}

void PopulateCopLight(CarRenderInfo* carRenderInfo, VehicleFX fx, SpotLight& spotlightCfg, D3DXMATRIX* matrix)
{
	if (carRenderInfo->IsLightOn(fx))
	{
		float time = Game::Sim_GetTime();
		time = fmod(time * 3.0f, 1.0f);
		if (fx == VehicleFX_COPBLUE && time > 0.5)
		{
			return;
		}

		if (fx == VehicleFX_COPRED && time < 0.5)
		{
			return;
		}

		SpotLight spotLight = spotlightCfg;

		spotLight.Color *= g_Weather.GetCarLightsPower();

		D3DXVec3TransformCoord(&spotLight.Position, &spotLight.Position, matrix);

		D3DXVec3TransformNormal(&spotLight.Direction, &spotLight.Direction, matrix);
		D3DXVec3Normalize(&spotLight.Direction, &spotLight.Direction);

		AddSpotLightToBuffer(spotLight, SpotLightSource::Blinking, NULL);
	}
}

void PopulateExhaustLights(CarRenderInfo* carRenderInfo, D3DXMATRIX* matrix, VehicleRenderConn* conn, CarRenderUsage renderUsage)
{
	if (renderUsage == CarRenderUsage::Player)
	{
		auto icar = (int**)carRenderInfo;
		auto iconn = (int*)conn;

		bool usingNos = icar[0x45F];
		bool shifting = ((iconn[0xFE] & 0x10) != 0 && iconn[0xF6] != 0) && !usingNos;

		if (!usingNos && !shifting)
		{
			return;
		}

		auto emmiters = (CarEmitter**)(icar + 0x6B);

		auto emmiter = *emmiters;
		while ((void*)emmiter != (void*)emmiters)
		{
			SpotLight spotLight = usingNos && g_Config.NosLight ? NosLightConfig : ExhaustLightConfig;
			spotLight.Position += emmiter->Position;

			spotLight.Color *= g_Weather.GetCarLightsPower();

			D3DXVec3TransformCoord(&spotLight.Position, &spotLight.Position, matrix);

			D3DXVec3TransformNormal(&spotLight.Direction, &spotLight.Direction, matrix);
			D3DXVec3Normalize(&spotLight.Direction, &spotLight.Direction);

			AddSpotLightToBuffer(spotLight, SpotLightSource::Player_Breaklights, NULL);

			emmiter = emmiter->Next;
		}
	}
}

void PopulateCarSpotLights(CarRenderInfo* carRenderInfo, D3DXMATRIX* matrix, bool roadBlock)
{
	if (!roadBlock)
	{
		auto renderUsage = carRenderInfo->pRideInfo->mCarRenderUsage;
		bool isPlayer = renderUsage == CarRenderUsage::Player;

		auto headlightOn = carRenderInfo->IsLightOn(VehicleFX_HEADLIGHTS);
		if (headlightOn)
		{
			auto source = isPlayer ? SpotLightSource::Player_Headlights : SpotLightSource::Headlights;
			auto headlighsConfig = isPlayer ? CarHeadlighsConfig : CarAiHeadlighsConfig;
			PopulateCarLight(carRenderInfo, Hashes::LEFT_HEADLIGHT, headlighsConfig, matrix, source);
			PopulateCarLight(carRenderInfo, Hashes::RIGHT_HEADLIGHT, headlighsConfig, matrix, source);
		}

		auto brakelightOn = carRenderInfo->IsLightOn(VehicleFX_BRAKELIGHTS);
		if (Game::State == 6 || brakelightOn)
		{
			auto brakeLightsConfig = brakelightOn ? CarBrakeLightsOnConfig : CarBrakeLightsOffConfig;
			auto source = isPlayer ? SpotLightSource::Player_Breaklights : SpotLightSource::Breaklights;
			PopulateCarLight(carRenderInfo, Hashes::LEFT_BRAKELIGHT, brakeLightsConfig, matrix, source);
			PopulateCarLight(carRenderInfo, Hashes::RIGHT_BRAKELIGHT, brakeLightsConfig, matrix, source);
		}

		auto reverseOn = carRenderInfo->IsLightOn(VehicleFX_REVERSE);
		if (reverseOn)
		{
			PopulateCarLight(carRenderInfo, Hashes::LEFT_REVERSE, CarReverseConfig, matrix, SpotLightSource::Reverse);
			PopulateCarLight(carRenderInfo, Hashes::RIGHT_REVERSE, CarReverseConfig, matrix, SpotLightSource::Reverse);
		}
	}

	PopulateCopLight(carRenderInfo, VehicleFX_COPBLUE, CopLightBlueConfig, matrix);
	PopulateCopLight(carRenderInfo, VehicleFX_COPRED, CopLightRedConfig, matrix);
}

bool IsRoadBlockCar(VehicleRenderConn* renderConn)
{
	if (!g_Config.RoadblockLights)
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
						PopulateExhaustLights(carRenderInfo, &renderConn->Matrix1, renderConn, renderUsage);
						PopulateCarSpotLights(carRenderInfo, &renderConn->Matrix1, renderUsage == CarRenderUsage::AICop && IsRoadBlockCar(renderConn));
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
			PopulateCarSpotLights(fecar->pCarRenderInfo, &fecar->BodyMatrix, false);
		}
	}
}