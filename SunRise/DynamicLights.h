#pragma once
#include "Spotlight.h"
#include "RenderModel.h"
#include "eView.h"
#include "ViewId.h"
#include "RenderTarget.h"
#include "Config.h"
#include "VehicleRenderConn.h"
#include "Hashes.h"
#include "SmackableRenderConn.h"
#include "Weather.h"
#include "GridCuller.h"

#define NUM_SPOTLIGHTS 24
SpotLightShader SpotLights[NUM_SPOTLIGHTS];
int NumSpotLights;

#define NUM_SPOTLIGHTS_BUFFER 512
SpotLightModel SpotLightBuffer[NUM_SPOTLIGHTS_BUFFER];
int NumSpotLightBuffer = 0;

inline D3DXVECTOR3 GetCameraPos()
{
	auto camera = Game::GetPlayerCamera();
	return camera->Position;
}

inline float GetCameraDistance(D3DXVECTOR3 pos)
{
	auto cameraPos = GetCameraPos();
	D3DXVECTOR3 diff = pos - cameraPos;
	return D3DXVec3Length(&diff);
}

void AddSpotLightToBuffer(SpotLight spotLight, SpotLightSource source, FlareModel* flare)
{
	if (NumSpotLightBuffer >= NUM_SPOTLIGHTS_BUFFER)
	{
		throw std::runtime_error("Light buffer is full");
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
	auto visibleState = playerView->Pinfo->get_visible_state_sb(spotLight.Position, { spotLight.Range, spotLight.Range, spotLight.Range });
	if (visibleState == visible_state::outside)
	{
		return;
	}

	SpotLightModel spotLightModel;
	spotLightModel.Light = spotLight;
	spotLightModel.Source = source;
	spotLightModel.Flare = flare;

	SpotLightBuffer[NumSpotLightBuffer] = spotLightModel;
	NumSpotLightBuffer++;
}

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
					AddSpotLightToBuffer(spotLight, SpotLightSource::LampPost, solidLights.Flare);
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
		// Remove road reflected meshes (TODO: find better way)
		if (drawInfo->Matrix && drawInfo->Matrix->_33 > 0)
		{
			auto model = (eModel*)(drawInfo->pModel & 0xFFFFFFFC);
			PopulateFromModel(model, drawInfo->Matrix);
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

void AddCarHeadlight(CarRenderInfo* carRenderInfo, D3DXMATRIX* matrix, LightFlare* flare, bool isPlayer)
{
	if (flare->Type == eLightFlareType::car_headlight)
	{
		VehicleFX fx = VehicleFX_HEADLIGHTS;
		if (flare->NameHash == Hashes::LEFT_HEADLIGHT)
		{
			fx = VehicleFX_LHEAD;
		}

		if (flare->NameHash == Hashes::RIGHT_HEADLIGHT)
		{
			fx = VehicleFX_RHEAD;
		}

		if (!carRenderInfo->IsLightOn(fx))
		{
			return;
		}

		SpotLight spotLight = CarHeadlighsConfig;

		auto flarePos = flare->Position;
		flarePos.x += 0.3;
		D3DXVec3TransformCoord(&spotLight.Position, &flarePos, matrix);

		D3DXVec3TransformNormal(&spotLight.Direction, &spotLight.Direction, matrix);
		D3DXVec3Normalize(&spotLight.Direction, &spotLight.Direction);

		AddSpotLightToBuffer(spotLight, isPlayer ? SpotLightSource::Player_Headlights : SpotLightSource::Headlights, NULL);
	}
}

void AddCarBrakelight(CarRenderInfo* carRenderInfo, D3DXMATRIX* matrix, LightFlare* flare, bool isPlayer)
{
	if (flare->Type == eLightFlareType::car_brakelight || flare->Type == eLightFlareType::car_traffic_brakelight)
	{
		VehicleFX fx = VehicleFX_BRAKELIGHTS;
		if (flare->NameHash == Hashes::LEFT_HEADLIGHT)
		{
			fx = VehicleFX_LBRAKE;
		}

		if (flare->NameHash == Hashes::RIGHT_HEADLIGHT)
		{
			fx = VehicleFX_RBRAKE;
		}

		SpotLight spotLight = carRenderInfo->IsLightOn(fx) ? CarBrakeLightsOnConfig : CarBrakeLightsOffConfig;

		auto flarePos = flare->Position;
		flarePos.x -= 0.1;
		D3DXVec3TransformCoord(&spotLight.Position, &flarePos, matrix);

		D3DXVec3TransformNormal(&spotLight.Direction, &spotLight.Direction, matrix);
		D3DXVec3Normalize(&spotLight.Direction, &spotLight.Direction);

		AddSpotLightToBuffer(spotLight, isPlayer ? SpotLightSource::Player_Breaklights : SpotLightSource::Breaklights, NULL);
	}
}

D3DXVECTOR3 GetPlayerPos()
{
	for (int i = 0; i < VehicleRenderConn::ListCount; i++)
	{
		auto renderConn = VehicleRenderConn::List[i];
		if (renderConn && !renderConn->Inactive)
		{
			auto carRenderInfo = renderConn->pCarRenderInfo;
			if (carRenderInfo)
			{
				int renderUsage = carRenderInfo->pRideInfo->mMyCarRenderUsage;
				if (renderUsage == 0)
				{
					auto matrix = &renderConn->Matrix1;
					return { matrix->_41, matrix->_42, matrix->_43 };
				}
			}
		}
	}

	return { 0, 0, 0 };
}

void AddHelicopterLight(D3DXVECTOR3 pos)
{
	auto playerPos = GetPlayerPos();
	if (playerPos == D3DXVECTOR3(0, 0, 0))
	{
		return;
	}

	SpotLight spotLight = HelicopterLightConfig;

	spotLight.Direction = playerPos - pos;
	float range = D3DXVec3Length(&spotLight.Direction);
	if (range > spotLight.Range)
	{
		return;
	}

	D3DXVec3Normalize(&spotLight.Direction, &spotLight.Direction);

	spotLight.Position = pos;

	AddSpotLightToBuffer(spotLight, SpotLightSource::Helicopter, NULL);
}

void PopulateCarSpotLights()
{
	for (int i = 0; i < VehicleRenderConn::ListCount; i++)
	{
		auto renderConn = VehicleRenderConn::List[i];
		if (renderConn && !renderConn->Inactive)
		{
			auto carRenderInfo = renderConn->pCarRenderInfo;
			if (carRenderInfo)
			{
				auto matrix = &renderConn->Matrix1;
				D3DXVECTOR3 pos = { matrix->_41, matrix->_42, matrix->_43 };

				int renderUsage = carRenderInfo->pRideInfo->mMyCarRenderUsage;
				if (renderUsage == 5)
				{
					matrix = renderConn->Matrix;
					pos = { matrix->_41, matrix->_42, matrix->_43 };
					AddHelicopterLight(pos);
				}
				else
				{
					LightFlare* flare = carRenderInfo->LightFlares;

					while ((void*)flare != &carRenderInfo->LightFlares)
					{
						bool isPlayer = renderUsage == 0;

						AddCarHeadlight(carRenderInfo, matrix, flare, isPlayer);
						AddCarBrakelight(carRenderInfo, matrix, flare, isPlayer);

						flare = flare->Next;
					}
				}
			}
		}
	}
}

void PopulateSpotLights(GrandSceneryCullInfo* cullInfo)
{
	NumSpotLightBuffer = 0;

	PopulateWorldSpotLights(cullInfo);

	PopulateCarSpotLights();

	std::sort(SpotLightBuffer, SpotLightBuffer + NumSpotLightBuffer, [](const SpotLightModel& a, const SpotLightModel& b) { return (int)a.Source < (int)b.Source; });

	g_CellBuffer.Clear();
	for (int i = 0; i < NumSpotLightBuffer; i++)
	{
		g_CellBuffer.AssignSpotLightToGrid(&SpotLightBuffer[i].Light);
	}
	g_CellBuffer.Sort();
}

inline bool DynamicallyLit(eEffect* effect)
{
	auto id = effect->id;
	return id == shader_type::WorldShader || id == shader_type::WorldReflectShader || id == shader_type::WorldNormalMap || id == shader_type::GlossyWindow || id == shader_type::CarShader || id == shader_type::billboardshader;
}

inline bool DynamicallyLit(RenderModel* model)
{
	return DynamicallyLit(model->Effect);
}

inline void PopulateShaderSpotlights(RenderModel* model)
{
	NumSpotLights = 0;
	memset(&SpotLights, 0, sizeof(SpotLights));

	if (!model->pSolid || model->pSolid->Volume > 170,000.0f)
	{
		return;
	}

	auto bbox_min = model->pMeshEntry->bbox_min;
	auto bbox_max = model->pMeshEntry->bbox_max;

	D3DXVec3TransformCoord(&bbox_min, &bbox_min, model->LocalToWorld);
	D3DXVec3TransformCoord(&bbox_max, &bbox_max, model->LocalToWorld);

	auto meshCenter = (bbox_min + bbox_max) * 0.5f;
	D3DXVECTOR3 diagonal = (bbox_max - bbox_min);
	float radius = D3DXVec3Length(&diagonal) * 0.5f;
	if (radius > 500)
	{
		return;
	}

	g_CellBuffer.GetLightsForMesh(meshCenter, radius);
	for (int i = 0; i < g_CellBuffer.numCandidateLights; i++)
	{
		auto spotlight = g_CellBuffer.candidateLights[i];

		if (spotlight && NumSpotLights < NUM_SPOTLIGHTS)
		{
			if (ConeSphereIntersect(spotlight->Position, spotlight->Direction, D3DXToRadian(spotlight->OuterAngle), spotlight->Range, meshCenter, radius))
			{
				SpotLightShader s;
				s.Position = spotlight->Position;
				s.Direction = spotlight->Direction;
				s.Color = spotlight->Color * spotlight->Intensity;
				s.Range = spotlight->Range;
				s.InnerCos = cosf(D3DXToRadian(spotlight->InnerAngle));
				s.OuterCos = cosf(D3DXToRadian(spotlight->OuterAngle));

				D3DXMATRIX worldToLocal;
				D3DXMatrixInverse(&worldToLocal, NULL, model->LocalToWorld);

				D3DXVec3TransformCoord(&s.Position, &s.Position, &worldToLocal);

				D3DXVec3TransformNormal(&s.Direction, &s.Direction, &worldToLocal);
				D3DXVec3Normalize(&s.Direction, &s.Direction);

				SpotLights[NumSpotLights] = s;
				NumSpotLights++;
			}
		}
	}
}

inline void SetDynamicLights(RenderModel* model)
{
	if (DynamicallyLit(model))
	{
		model->Effect->SetValue(ShaderParam::caSpotLights, SpotLights, sizeof(SpotLights));
	}
}

inline bool UseVertexLighting(RenderModel* model)
{
	auto bbox_min = model->pMeshEntry->bbox_min;
	auto bbox_max = model->pMeshEntry->bbox_max;

	D3DXVec3TransformCoord(&bbox_min, &bbox_min, model->LocalToWorld);
	D3DXVec3TransformCoord(&bbox_max, &bbox_max, model->LocalToWorld);

	float cameraDistance1 = GetCameraDistance(bbox_min);
	float cameraDistance2 = GetCameraDistance(bbox_max);

	return cameraDistance1 > g_Config.LightLodDistance && cameraDistance2 > g_Config.LightLodDistance;
}

TechniqueType GetTechnique(RenderModel* renderModel)
{
	TechniqueType technique = Technique_Invalid;

	if (DynamicallyLit(renderModel))
	{
		if (RenderTarget::Current->ViewId == ViewId::ShadowMap)
		{
			return Technique_ShadowMap;
		}

		if (PrelitTextures.contains(renderModel->DiffuseTextureInfo->NameHash))
		{
			auto brightness = PrelitTextures[renderModel->DiffuseTextureInfo->NameHash].Brightness;
			renderModel->Effect->SetFloat(ShaderParam::cfBrightness, brightness);
			
			return Technique_Prelit;
		}

		if (RenderTarget::Current->ViewId == ViewId::Player1)
		{
			PopulateShaderSpotlights(renderModel);
			bool useVertexLighting = UseVertexLighting(renderModel);
			if (useVertexLighting && NumSpotLights > 0)
			{
				return Technique_LitVertex;
			}

			if (NumSpotLights == 0)
			{
				technique = Technique_Unlit;
			}
			else if (NumSpotLights <= 8)
			{
				technique = Technique_LitPixel_8;
			}
			else if (NumSpotLights <= 16)
			{
				technique = Technique_LitPixel_16;
			}
			else
			{
				technique = Technique_LitPixel_24;
			}
		}
		else
		{
			technique = Technique_Unlit;
		}
	}

	return technique;
}