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
D3DXVECTOR4 SP_Position_Range[NUM_SPOTLIGHTS];
D3DXVECTOR4 SP_Direction_OuterCos[NUM_SPOTLIGHTS];
D3DXVECTOR4 SP_Color_InnerCos[NUM_SPOTLIGHTS];
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

	g_CellBuffer.Clear();
	for (int i = 0; i < NumSpotLightBuffer; i++)
	{
		g_CellBuffer.AssignSpotLightToGrid(SpotLightBuffer + i);
	}
	g_CellBuffer.Sort();
}

inline bool DynamicallyLit(eEffect* effect)
{
	return effect->HasParam(ShaderParam::cvaSpPositionRange);
}

inline bool DynamicallyLit(RenderModel* model)
{
	return DynamicallyLit(model->Effect);
}

inline void PopulateShaderSpotlights(RenderModel* model)
{
	NumSpotLights = 0;
	memset(&SP_Position_Range, 0, sizeof(SP_Position_Range));
	memset(&SP_Direction_OuterCos, 0, sizeof(SP_Direction_OuterCos));
	memset(&SP_Color_InnerCos, 0, sizeof(SP_Color_InnerCos));

	if (!model->pSolid || model->pSolid->Volume > 170, 000.0f)
	{
		return;
	}

	auto bbox_min = model->pMeshEntry->bbox_min;
	auto bbox_max = model->pMeshEntry->bbox_max;

	bool worldSpace = true;
	if (model->LocalToWorld != Game::IdentityMatrix)
	{
		D3DXVec3TransformCoord(&bbox_min, &bbox_min, model->LocalToWorld);
		D3DXVec3TransformCoord(&bbox_max, &bbox_max, model->LocalToWorld);
		worldSpace = false;
	}

	auto meshCenter = (bbox_min + bbox_max) * 0.5f;
	D3DXVECTOR3 diagonal = (bbox_max - bbox_min);
	float radius = D3DXVec3Length(&diagonal) * 0.5f;
	if (radius > 500)
	{
		return;
	}

	if (worldSpace)
	{
		g_CellBuffer.GetLightsForMesh(bbox_min, bbox_max);
	}
	else
	{
		g_CellBuffer.GetLightsForMesh(meshCenter, radius);
	}

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

				SP_Position_Range[NumSpotLights] = D3DXVECTOR4(s.Position, s.Range);
				SP_Direction_OuterCos[NumSpotLights] = D3DXVECTOR4(s.Direction, s.OuterCos);
				SP_Color_InnerCos[NumSpotLights] = D3DXVECTOR4(s.Color, s.InnerCos);

				NumSpotLights++;
			}
		}
	}
}

inline void SetDynamicLights(RenderModel* model)
{
	if (DynamicallyLit(model) && NumSpotLights > 0)
	{
		auto effect = model->Effect;
		effect->SetVectorArray(ShaderParam::cvaSpPositionRange, SP_Position_Range, 24);
		effect->SetVectorArray(ShaderParam::cvaSpDirectionOuterCos, SP_Direction_OuterCos, 24);
		effect->SetVectorArray(ShaderParam::cvaSpColorInnerCos, SP_Color_InnerCos, 24);
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

	return cameraDistance1 > g_Config.LightLodDistance && cameraDistance2 > g_Config.LightLodDistance && NumSpotLights > 0;
}

inline bool ApplyEmissive(RenderModel* renderModel)
{
	auto effect = renderModel->Effect;
	if (effect->HasParam(ShaderParam::cvEmissive))
	{
		D3DXVECTOR4 brightness = { 0, 0, 0, 0 };
		bool prelit = false;
		bool enabled = false;

		auto prelitTex = PrelitTextures.Get(renderModel->DiffuseTextureInfo->NameHash);
		if (prelitTex)
		{
			prelit = prelitTex->Prelit;
			enabled = prelitTex->AlwaysOn ? true : g_Weather.LightsOn();
			if (enabled)
			{
				effect->SetTexture(ShaderParam::EMISSIVE_TEXTURE, prelitTex->GetMaskTexture());

				brightness = prelitTex->Color;
				brightness *= brightness.w;
				if (!prelitTex->IgnoreWeather)
				{
					brightness *= g_Weather.GetTextureLightPower();
				}

				if (effect->id == shader_type::GlossyWindow && g_Config.WindowGlowOverride)
				{
					brightness = g_Weather.GetWindowGlowColor();
				}

				brightness.w = 1;
			}
		}

		effect->SetVector(ShaderParam::cvEmissive, &brightness);

		if (prelit && enabled)
		{
			return true;
		}
	}

	return false;
}

TechniqueType GetTechnique(RenderModel* renderModel)
{
	TechniqueType technique = Technique_Invalid;

	if (RenderTarget::Current->ViewId == ViewId::Reflection && renderModel->pSolid)
	{
		if (renderModel->pSolid->NameHash == Hashes::XW_FENCEMETALB_1A_00)
		{
			return Technique_Invisible;
		}
	}

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

		if (ApplyEmissive(renderModel))
		{
			return Technique_Prelit;
		}

		if (RenderTarget::Current->ViewId == ViewId::Player1)
		{
			PopulateShaderSpotlights(renderModel);

			if (UseVertexLighting(renderModel))
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