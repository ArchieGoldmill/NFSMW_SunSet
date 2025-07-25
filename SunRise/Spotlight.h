#pragma once
#include <d3dx9.h>
#include <vector>
#include <string>
#include "Utilities.h"
#include "eLightFlareParams.h"
#include "TextureInfo.h"

struct SpotLightShader
{
	D3DXVECTOR3 Position;
	float Range;
	D3DXVECTOR3 Direction;
	float OuterCos;
	D3DXVECTOR3 Color;
	float InnerCos;
};

struct SpotLight
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Direction;
	D3DXVECTOR3 Color;
	float Range;
	float Intensity;
	float InnerAngle;
	float OuterAngle;
};

enum class SpotLightSource
{
	LampPost,
	Player_Headlights,
	Helicopter,
	Headlights,
	Player_Breaklights,
	Breaklights
};

struct FlareModel
{
	std::string Name;
	float Intensity;
	float Size;
	eLightFlareType Type;

	Hash TextureName;
	TextureInfo* Texture = NULL;
};

struct SpotLightModel
{
	SpotLight Light;
	SpotLightSource Source;
	FlareModel* Flare;
};

struct SolidLights
{
	Hash HashA = 0;
	Hash HashB = 0;
	std::string Name;
	FlareModel* Flare = NULL;
	std::vector<SpotLight> Lights;
};