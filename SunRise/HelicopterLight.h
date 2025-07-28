#pragma once
#include <d3dx9.h>
#include "VehicleRenderConn.h"
#include "Spotlight.h"
#include "Config.h"
#include "LightsBuffer.h"

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
