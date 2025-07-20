#pragma once
#include <d3dx9.h>
#include <vector>
#include <string>
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

enum class SpotLightSource
{
	LampPost,
	Car
};

struct FlareModel
{
	std::string Name;
	float Intensity;
	float Size;
	eLightFlareType Type;
	D3DXVECTOR3 Color;
};

struct SpotLightModel
{
	SpotLight Light;
	SpotLightSource Source;
	FlareModel* Flare;
};

struct SolidLights
{
	Hash Hash;
	std::string Name;
	FlareModel* Flare;
	std::vector<SpotLight> Lights;
};