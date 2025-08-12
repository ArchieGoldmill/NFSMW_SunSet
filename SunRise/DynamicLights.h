#pragma once
#include "RenderModel.h"
#include "eView.h"
#include "ViewId.h"
#include "RenderTarget.h"
#include "Config.h"
#include "Hashes.h"
#include "SmackableRenderConn.h"
#include "GridCuller.h"
#include "CarLights.h"
#include "WorldLights.h"

#define NUM_SPOTLIGHTS 24
SpotLightShader SpotLights[NUM_SPOTLIGHTS];
int NumSpotLights;

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
	return effect->HasParam(ShaderParam::caSpotLights);
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

		if (renderModel->DiffuseTextureInfo->NameHash == Hashes::ANM_WATERA_)
		{
			return Technique_Water;
		}

		auto effect = renderModel->Effect;

		if (effect->HasParam(ShaderParam::cvEmissive))
		{
			D3DXVECTOR4 brightness = { 0, 0, 0, 0 };
			bool prelit = false;
			bool enabled = false;

			auto prelitTex = PrelitTextures.find(renderModel->DiffuseTextureInfo->NameHash);
			if (prelitTex != PrelitTextures.end() && prelitTex->second.Mask.GetHash())
			{
				prelit = prelitTex->second.Prelit;
				enabled = prelitTex->second.AlwaysOn ? true : g_Weather.LightsOn();
				if (enabled)
				{
					effect->SetTexture(ShaderParam::EMISSIVE_TEXTURE, prelitTex->second.GetMaskTexture());

					brightness = prelitTex->second.Color;
					brightness *= brightness.w;
					if (prelit)
					{
						brightness *= g_Weather.GetTextureLightPower();
					}

					brightness.w = 1;
				}
			}

			effect->SetVector(ShaderParam::cvEmissive, &brightness);

			if (prelit && enabled)
			{
				return Technique_Prelit;
			}
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
			else if (NumSpotLights <= 4)
			{
				technique = Technique_LitPixel_4;
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