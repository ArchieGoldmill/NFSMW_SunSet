#pragma once
#include <d3dx9.h>
#include <vector>
#include "Utilities.h"

struct SpotLight
{
	D3DXVECTOR3 Position;
	float Range;
	D3DXVECTOR3 Direction;
	float Power;
	D3DXVECTOR3 Color;
	float Intensity;
};

struct SolidLights
{
	Hash SolidName;
	std::vector<SpotLight*> Lights;
};

std::vector<SolidLights*> SolidLightsList;