#pragma once
#include "GrandSceneryCullInfo.h"
#include "Config.h"
#include "Weather.h"
#include "LightsBuffer.h"
#include "WorldModel.h"

SpotLight CreateSpotLight(SpotLight& pSpotLight, D3DXMATRIX* matrix)
{
	SpotLight spotLight = pSpotLight;

	D3DXVec3TransformCoord(&spotLight.Position, &spotLight.Position, matrix);
	D3DXVec3TransformNormal(&spotLight.Direction, &spotLight.Direction, matrix);
	D3DXVec3Normalize(&spotLight.Direction, &spotLight.Direction);
	spotLight.Color *= g_Weather.GetLightIntensity();

	return spotLight;
}

void PopulateFromModel(eModel* model, D3DXMATRIX* matrix)
{
	auto solid = model->pSolid;
	if (solid)
	{
		for (auto& solidLights : SolidLightsList)
		{
			// TODO use binary search
			if (solidLights.HashA == model->NameHash || solidLights.HashB == model->NameHash)
			{
				if (solidLights.Blink)
				{
					int blink = Game::FlareBlink[solidLights.Blink - 1];
					if (blink != -1)
					{
						continue;
					}
				}

				for (auto& pSpotLight : solidLights.Lights)
				{
					auto spotLight = CreateSpotLight(pSpotLight, matrix);
					AddSpotLightToBuffer(spotLight, solidLights.Blink ? SpotLightSource::Blinking : SpotLightSource::LampPost, solidLights.Flare);
				}
			}
		}
	}
}

void PopulateWorldSpotLights(GrandSceneryCullInfo* cullInfo)
{
	if (!g_Weather.WorldLightsOn())
	{
		return;
	}

	auto drawInfo = cullInfo->FirstDrawInfo;
	while (drawInfo != cullInfo->CurrentDrawInfo)
	{
		auto matrix = drawInfo->Matrix;
		if (!matrix)
		{
			matrix = Game::IdentityMatrix;
		}

		// Remove road reflected meshes (TODO: find better way)
		if (matrix->_33 > 0)
		{
			auto model = (eModel*)(drawInfo->pModel & 0xFFFFFFFC);
			PopulateFromModel(model, matrix);
		}

		drawInfo++;
	}

	auto worldModel = WorldModel::List.Next;
	while ((int)worldModel != (int)&WorldModel::List)
	{
		auto spaceNode = worldModel->pSpaceNode;
		D3DXMATRIX* matrix = spaceNode ? &spaceNode->Matrix1 : &worldModel->Matrix;

		eModel* model = worldModel->pModel;
		if (!model && worldModel->Heirarchy)
		{
			auto nodes = &worldModel->Heirarchy->Node;
			model = nodes[worldModel->HeirarchyIndex].pModel;
		}

		if (model && model->pSolid && matrix)
		{
			PopulateFromModel(model, matrix);
		}

		worldModel = worldModel->Next;
	}
}

void PopulateFrontEndSpotlights()
{
	if (!g_Weather.WorldLightsOn())
	{
		return;
	}

	auto manager = Game::GarageMainScreen_GetInstance();
	if (!manager || manager[27])
	{
		return;
	}

	auto garageTypePtr = (int**)(0x0091CAE0);
	auto garageType = (*garageTypePtr)[6];

	if (FrontEndLights.contains(garageType))
	{
		auto& lights = FrontEndLights[garageType].Lights;
		for (auto& spotLight : lights)
		{
			auto newSpotLight = spotLight;
			newSpotLight.Color *= g_Weather.GetLightIntensity();
			AddSpotLightToBuffer(newSpotLight, SpotLightSource::LampPost, FrontEndLights[garageType].Flare);
		}
	}
}