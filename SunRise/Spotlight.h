#pragma once
#include <d3dx9.h>
#include <vector>
#include <string>
#include "Utilities.h"
#include "eLightFlareParams.h"
#include "TextureInfo.h"
#include "HashField.h"

struct SpotLightShader
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Direction;
	D3DXVECTOR3 Color;
	float Range;
	float InnerCos;
	float OuterCos;
	float Specular;
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
	float Specular;

	bool IsLampPost()
	{
		return Direction == D3DXVECTOR3(0, 0, -1) && OuterAngle < 180 && InnerAngle < 180;
	}
};

enum class SpotLightSource
{
	LampPost,
	Player_Headlights,
	Helicopter,
	Headlights,
	Player_Breaklights,
	Breaklights,
	Blinking
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
	float FlareIntecity;
};

struct SolidLights
{
	HashField LodA;
	HashField LodB;

	FlareModel* Flare = NULL;

	std::vector<SpotLight> Lights;

	int Blink;

	bool AlwaysOn = false;
	bool UseFirstLight = false;
};