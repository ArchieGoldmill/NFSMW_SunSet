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
#include "FrontEndRenderingCar.h"
#include "FEGeometryModels.h"

#define NUM_SPOTLIGHTS 24
SpotLightShader SpotLights[NUM_SPOTLIGHTS];
int NumSpotLights;

#define NUM_SPOTLIGHTS_BUFFER 512
SpotLightModel SpotLightBuffer[NUM_SPOTLIGHTS_BUFFER];
int NumSpotLightBuffer = 0;

void AddSpotLightToBuffer(SpotLight spotLight, SpotLightSource source, FlareModel* flare)
{
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

void PopulateCarLight(CarRenderInfo* carRenderInfo, Hash flareHash, SpotLight& temp, float offset, D3DXMATRIX* matrix, SpotLightSource source)
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

		SpotLight spotLight = temp;

		pos.x += offset;
		D3DXVec3TransformCoord(&spotLight.Position, &pos, matrix);

		D3DXVec3TransformNormal(&spotLight.Direction, &spotLight.Direction, matrix);
		D3DXVec3Normalize(&spotLight.Direction, &spotLight.Direction);

		AddSpotLightToBuffer(spotLight, source, NULL);
	}
}

void PopulateCarSpotLights(CarRenderInfo* carRenderInfo, D3DXMATRIX* matrix)
{
	D3DXVECTOR3 pos = { matrix->_41, matrix->_42, matrix->_43 };

	LightFlare* flare = carRenderInfo->LightFlares;

	int renderUsage = carRenderInfo->pRideInfo->mMyCarRenderUsage;
	bool isPlayer = renderUsage == 0;

	auto headlightOn = carRenderInfo->IsLightOn(VehicleFX_HEADLIGHTS);
	if (headlightOn)
	{
		SpotLightSource source = isPlayer ? SpotLightSource::Player_Headlights : SpotLightSource::Headlights;
		PopulateCarLight(carRenderInfo, Hashes::LEFT_HEADLIGHT, CarHeadlighsConfig, 0.3, matrix, source);
		PopulateCarLight(carRenderInfo, Hashes::RIGHT_HEADLIGHT, CarHeadlighsConfig, 0.3, matrix, source);
	}

	auto brakelightOn = carRenderInfo->IsLightOn(VehicleFX_BRAKELIGHTS);
	SpotLight temp = brakelightOn ? CarBrakeLightsOnConfig : CarBrakeLightsOffConfig;
	SpotLightSource source = isPlayer ? SpotLightSource::Player_Breaklights : SpotLightSource::Breaklights;
	PopulateCarLight(carRenderInfo, Hashes::LEFT_BRAKELIGHT, temp, -0.1, matrix, source);
	PopulateCarLight(carRenderInfo, Hashes::RIGHT_BRAKELIGHT, temp, -0.1, matrix, source);
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
					int renderUsage = carRenderInfo->pRideInfo->mMyCarRenderUsage;
					if (renderUsage == 5)
					{
						auto matrix = renderConn->Matrix;
						D3DXVECTOR3 pos = { matrix->_41, matrix->_42, matrix->_43 };
						AddHelicopterLight(pos);
					}
					else
					{
						PopulateCarSpotLights(carRenderInfo, &renderConn->Matrix1);
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

void PopulateFrontEndSpotlights()
{
	if (!g_Weather.WorldLightsOn())
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

void PopulateSpotLights(GrandSceneryCullInfo* cullInfo)
{
	NumSpotLightBuffer = 0;

	if (Game::State == 6)
	{
		PopulateWorldSpotLights(cullInfo);
	}

	if (Game::State == 3)
	{
		PopulateFrontEndSpotlights();
	}

	PopulateCarSpotLights();

	std::sort(SpotLightBuffer, SpotLightBuffer + NumSpotLightBuffer, [](const SpotLightModel& a, const SpotLightModel& b) { return (int)a.Source < (int)b.Source; });

	g_CellBuffer.Clear();
	for (int i = 0; i < NumSpotLightBuffer; i++)
	{
		g_CellBuffer.AssignSpotLightToGrid(SpotLightBuffer + i);
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

	if (!model->pSolid || model->pSolid->Volume > 170, 000.0f)
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
		auto lightmodel = g_CellBuffer.candidateLights[i];
		auto spotlight = &lightmodel->Light;
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

		D3DXVECTOR4 brightness = { 0,0,0,0 };

		bool prelit = false;

		auto prelitTex = PrelitTextures.find(renderModel->DiffuseTextureInfo->NameHash);
		if (prelitTex != PrelitTextures.end())
		{
			brightness = prelitTex->second.Color;
			prelit = true;
		}

		auto alphaTex = AlphaTextures.find(renderModel->DiffuseTextureInfo->NameHash);
		if (alphaTex != AlphaTextures.end())
		{
			brightness = alphaTex->second.Color;
		}

		renderModel->Effect->SetVector(ShaderParam::cvBrightness, &brightness);

		if (prelit)
		{
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