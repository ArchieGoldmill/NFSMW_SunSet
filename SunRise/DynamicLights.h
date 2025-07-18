#pragma once
#include "Spotlight.h"
#include "RenderModel.h"

#define NUM_SPOTLIGHTS 24
SpotLight SpotLights[NUM_SPOTLIGHTS];
int NumSpotLights;

SpotLight SpotLightBuffer[256];
int NumSpotLightBuffer = 0;

void AddSpotLightToBuffer(SpotLight spotLight)
{
	if (NumSpotLightBuffer > 255)
	{
		return;
	}

	// Make sure we dont add the same light twice (TODO: how that happens?)
	for (int i = 0; i < NumSpotLightBuffer; i++)
	{
		auto s = SpotLightBuffer[i];
		if (s.Position == spotLight.Position)
		{
			return;
		}
	}

	SpotLightBuffer[NumSpotLightBuffer] = spotLight;
	NumSpotLightBuffer++;
}

SpotLight CreateSpotLight(SpotLight* pSpotLight, D3DXMATRIX* matrix)
{
	SpotLight spotLight = *pSpotLight;

	D3DXVec3TransformCoord(&spotLight.Position, &spotLight.Position, matrix);
	D3DXVec3TransformNormal(&spotLight.Direction, &spotLight.Direction, matrix);
	D3DXVec3Normalize(&spotLight.Direction, &spotLight.Direction);

	return spotLight;
}

void PopulateWorldSpotLights(GrandSceneryCullInfo* cullInfo)
{
	auto drawInfo = cullInfo->FirstDrawInfo;
	while (drawInfo != cullInfo->CurrentDrawInfo)
	{
		auto model = (eModel*)(drawInfo->pModel & 0xFFFFFFFC);
		auto solid = model->pSolid;
		if (solid)
		{
			for (auto solidLights : SolidLightsList)
			{
				// TODO use binary search
				if (solidLights->SolidName == model->NameHash)
				{
					for (auto pSpotLight : solidLights->Lights)
					{
						auto spotLight = CreateSpotLight(pSpotLight, drawInfo->Matrix);
						AddSpotLightToBuffer(spotLight);
					}
				}
			}
		}

		drawInfo++;
	}
}

void PopulateCarSpotLights()
{
	// TODO
}

void PopulateSpotLights(GrandSceneryCullInfo* cullInfo)
{
	NumSpotLightBuffer = 0;

	PopulateWorldSpotLights(cullInfo);

	PopulateCarSpotLights();
}

bool DynamicallyLit(eEffect* effect)
{
	auto id = effect->id;
	return id == shader_type::WorldShader || id == shader_type::WorldReflectShader || id == shader_type::WorldNormalMap || id == shader_type::GlossyWindow;
}

bool DynamicallyLit(RenderModel* model)
{
	return DynamicallyLit(model->Effect);
}

void PopulateShaderSpotlights(RenderModel* model)
{
	NumSpotLights = 0;
	memset(&SpotLights, 0, sizeof(SpotLights));

	auto bbox_min = model->pMeshEntry->bbox_min;
	auto bbox_max = model->pMeshEntry->bbox_max;

	D3DXVec3TransformCoord(&bbox_min, &bbox_min, model->LocalToWorld);
	D3DXVec3TransformCoord(&bbox_max, &bbox_max, model->LocalToWorld);

	auto meshCenter = (bbox_min + bbox_max) * 0.5f;
	D3DXVECTOR3 halfExtents = (bbox_max - bbox_min) * 0.5f;
	float radius = D3DXVec3Length(&halfExtents);
	if (radius > 500)
	{
		return;
	}

	for (int i = 0; i < NumSpotLightBuffer; i++)
	{
		auto spotlight = SpotLightBuffer[i];

		if (NumSpotLights < NUM_SPOTLIGHTS)
		{
			if (ConeIntersectsSphere(spotlight.Position, spotlight.Direction, D3DXToRadian(90), spotlight.Range, meshCenter, radius))
			{
				SpotLights[NumSpotLights] = spotlight;
				NumSpotLights++;
			}
		}
	}
}

void SetDynamicLights(RenderModel* model)
{
	if (DynamicallyLit(model))
	{
		PopulateShaderSpotlights(model);
		model->Effect->SetValue(shader_param::SPLINE, SpotLights, sizeof(SpotLights));
	}
}