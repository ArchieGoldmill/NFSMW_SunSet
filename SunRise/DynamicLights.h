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

D3DXVECTOR4 SP_Position_Range[NUM_SPOTLIGHTS];
D3DXVECTOR4 SP_Direction_OuterCos[NUM_SPOTLIGHTS];
D3DXVECTOR4 SP_Color_InnerCos[NUM_SPOTLIGHTS];
float SP_Specular[NUM_SPOTLIGHTS];

int NumSpotLights;

void PopulateSpotLights()
{
	PopulateWorldSpotLights();

	PopulateFrontEndSpotlights();

	PopulateCarSpotLights();

	ClampLights();

	g_CellBuffer.Clear();
	for (int i = 0; i < NumSpotLightBuffer; i++)
	{
		g_CellBuffer.AssignSpotLightToGrid(SpotLightBuffer + i);
	}
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

		// Reset added field that is set inside grid culler
		lightmodel->Added = false;

		auto& s = lightmodel->Light;
		if (NumSpotLights < NUM_SPOTLIGHTS)
		{
			if (ConeSphereIntersect(&s, meshCenter, radius))
			{
				SP_Position_Range[NumSpotLights] = D3DXVECTOR4(s.Position, s.Range);
				SP_Direction_OuterCos[NumSpotLights] = D3DXVECTOR4(s.Direction, cosf(D3DXToRadian(s.OuterAngle)));
				SP_Color_InnerCos[NumSpotLights] = D3DXVECTOR4(s.Color * s.Intensity, cosf(D3DXToRadian(s.InnerAngle)));
				SP_Specular[NumSpotLights] = s.Specular;

				NumSpotLights++;
			}
		}
	}

	for (int i = NumSpotLights; i < NUM_SPOTLIGHTS; i++)
	{
		SP_Position_Range[i].w = 0;
	}
}

int GetNumSpotLights()
{
	if (NumSpotLights == 0)
	{
		return 0;
	}
	else if (NumSpotLights <= 4)
	{
		return 4;
	}
	else if (NumSpotLights <= 8)
	{
		return 8;
	}
	else if (NumSpotLights <= 16)
	{
		return 16;
	}
	else if (NumSpotLights <= 24)
	{
		return 24;
	}

	return NUM_SPOTLIGHTS;
}

inline void SetDynamicLights(RenderModel* model)
{
	if (DynamicallyLit(model) && NumSpotLights > 0)
	{
		auto effect = model->Effect;

		int num = GetNumSpotLights();
		effect->SetVectorArray(ShaderParam::cvaSpPositionRange, SP_Position_Range, num);
		effect->SetVectorArray(ShaderParam::cvaSpDirectionOuterCos, SP_Direction_OuterCos, num);
		effect->SetVectorArray(ShaderParam::cvaSpColorInnerCos, SP_Color_InnerCos, num);
		effect->SetFloatArray(ShaderParam::cfaSpSpecular, SP_Specular, num);

		if (model->LocalToWorld != Game::IdentityMatrix)
		{
			D3DXMATRIX worldIT;
			D3DXMatrixInverse(&worldIT, nullptr, model->LocalToWorld);
			D3DXMatrixTranspose(&worldIT, &worldIT);

			effect->SetMatrix(ShaderParam::cmWorldIT, &worldIT);
		}
		else
		{
			effect->SetMatrix(ShaderParam::cmWorldIT, model->LocalToWorld);
		}
	}
}

inline bool ApplyEmissive(RenderModel* renderModel)
{
	auto effect = renderModel->Effect;
	if (effect->HasParam(ShaderParam::cvEmissive))
	{
		D3DXVECTOR4 color = { 0, 0, 0, 0 };

		bool prelit = false;
		bool enabled = false;

		auto prelitTex = PrelitTextures.Get(renderModel->DiffuseTextureInfo->NameHash);
		if (prelitTex)
		{
			prelit = prelitTex->Prelit;

			enabled = prelitTex->AlwaysOn || g_Weather.LightsOn();
			if (enabled)
			{
				color = D3DXVECTOR4(prelitTex->Color, 1) * prelitTex->Brightness;

				if (!prelitTex->IgnoreWeather)
				{
					color *= g_Weather.GetTextureLightPower();
				}

				if (effect->id == shader_type::GlossyWindow && g_Config.WindowGlowOverride)
				{
					color = g_Weather.GetWindowGlowColor();
				}

				if (prelit)
				{
					color.w = prelitTex->UseVertexColor ? 1.0 : 0.0;
				}
				else
				{
					effect->SetTexture(ShaderParam::EMISSIVE_TEXTURE, prelitTex->GetMaskTexture());
					color.w = prelitTex->Brightness;
				}
			}
		}

		effect->SetVector(ShaderParam::cvEmissive, &color);

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

	auto effect = renderModel->Effect;

	if (RenderTarget::Current->ViewId == ViewId::Reflection && renderModel->pSolid)
	{
		if (renderModel->pSolid->NameHash == Hashes::XW_FENCEMETALB_1A_00)
		{
			return Technique_Invisible;
		}
	}

	if (RenderTarget::Current->ViewId == ViewId::ShadowMap && effect->HasTechnique(Technique_ShadowMap))
	{
		return Technique_ShadowMap;
	}

	if (renderModel->DiffuseTextureInfo->NameHash == Hashes::ANM_WATERA_ && effect->id == shader_type::WorldShader)
	{
		return Technique_Water;
	}

	if (ApplyEmissive(renderModel))
	{
		return Technique_Prelit;
	}

	if (DynamicallyLit(renderModel))
	{
		if (RenderTarget::Current->ViewId == ViewId::Player1)
		{
			PopulateShaderSpotlights(renderModel);

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
			else if (NumSpotLights <= 24)
			{
				technique = Technique_LitPixel_24;
			}
			else
			{
				technique = Technique_LitPixel_32;
			}
		}
		else
		{
			technique = Technique_Unlit;
		}
	}

	return technique;
}