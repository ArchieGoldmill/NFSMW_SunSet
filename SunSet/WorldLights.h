#pragma once
#include "GrandSceneryCullInfo.h"
#include "Config.h"
#include "Weather.h"
#include "LightsBuffer.h"
#include "WorldModel.h"
#include "GarageMainScreen.h"

SpotLight CreateSpotLight(SpotLight& pSpotLight, D3DXMATRIX* matrix)
{
	SpotLight spotLight = pSpotLight;

	D3DXVec3TransformCoord(&spotLight.Position, &spotLight.Position, matrix);
	D3DXVec3TransformNormal(&spotLight.Direction, &spotLight.Direction, matrix);
	D3DXVec3Normalize(&spotLight.Direction, &spotLight.Direction);

	return spotLight;
}

void PopulateFromModel(eModel* model, D3DXMATRIX* matrix)
{
	auto solid = model->pSolid;
	if (solid)
	{
		auto solidLights = SolidLightsList.Get(model->NameHash);
		if (solidLights)
		{
			for (auto& pSpotLight : solidLights->Lights)
			{
				auto spotLight = CreateSpotLight(pSpotLight, matrix);
				if (solidLights->UseFirstLight)
				{
					spotLight.Color = solidLights->Lights[0].Color;
					spotLight.Intensity = solidLights->Lights[0].Intensity;
					spotLight.InnerAngle = solidLights->Lights[0].InnerAngle;
					spotLight.OuterAngle = solidLights->Lights[0].OuterAngle;
					spotLight.Range = solidLights->Lights[0].Range;
					spotLight.Specular = solidLights->Lights[0].Specular;
				}

				float flareIntecity = 1.0f;
				auto source = SpotLightSource::LampPost;

				if (!solidLights->AlwaysOn)
				{
					flareIntecity = g_Weather.GetLightIntensity();
					spotLight.Color *= flareIntecity;
				}

				if (solidLights->Blink)
				{
					source = SpotLightSource::Blinking;
					int blink = Game::FlareBlink[solidLights->Blink - 1];
					if (blink != -1)
					{
						spotLight.Color *= 0.0001;
						flareIntecity *= 0.0001;
					}
				}

				AddSpotLightToBuffer(spotLight, source, solidLights->Flare, flareIntecity);
			}
		}
	}
}

void PopulateCustomMeshSpotLights()
{
	for (auto& customMesh : CustomMeshes.GetList())
	{
		if (customMesh.IsValid())
		{
			PopulateFromModel(customMesh.Model, &customMesh.Matrix);
		}
	}
}

void PopulateWorldSpotLights()
{
	if (Game::State == 6)
	{
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

		PopulateCustomMeshSpotLights();
	}
}

void __fastcall DrawScenery(SceneryPack* pack, int, SceneryCullInfo* cull_info)
{
	pack->DrawScenery(cull_info);

	if (Game::State == 6 && cull_info->View->Id == ViewId::Player1)
	{
		for (int i = 0; i < pack->instCount; i++)
		{
			auto& instance = pack->instances[i];
			auto flags = instance.flags;

			if ((flags & 0x100) == 0)
			{
				if ((flags & 0x8000000) != 0 || (flags & 0x40) != 0)
				{
					flags |= 0x8000040u;
				}

				if ((cull_info->Flags & (flags ^ 0xFFFFFF60) & 0x80000FF) == 0)
				{
					auto& info = pack->infos[instance.SceneryInfoNumber];

					for (int j = 0; j < 4; j++)
					{
						auto model = info.models[j];
						if (model)
						{
							D3DXMATRIX matrix;

							instance.SetRotation(&matrix);
							instance.SetPosition(&matrix);

							PopulateFromModel(model, &matrix);

							break;
						}
					}
				}
			}
		}
	}
}

void PopulateFrontEndSpotlights()
{
	if (Game::State == 3 && g_Weather.WorldLightsOn())
	{
		auto garageType = GarageMainScreen::GetType();
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
}