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

	FlareModel* Flare = NULL;

	std::vector<SpotLight> Lights;

	int Blink;

	bool AlwaysOn = false;
	bool UseFirstLight = false;
};

inline bool ConeSphereIntersect(const SpotLightShader& localLight, SpotLight* worldLight, MeshEntry* mesh, const D3DXVECTOR3& sphereCenter, float sphereRadius)
{
	auto localApex = localLight.Position;

	if ((localApex.x >= mesh->bbox_min.x && localApex.x <= mesh->bbox_max.x) &&
		(localApex.y >= mesh->bbox_min.y && localApex.y <= mesh->bbox_max.y) &&
		(localApex.z >= mesh->bbox_min.z && localApex.z <= mesh->bbox_max.z))
	{
		return true;
	}

	auto v = sphereCenter - worldLight->Position;
	float vlen = D3DXVec3Length(&v);

	if (vlen > worldLight->Range + sphereRadius)
	{
		return false;
	}

	if (worldLight->OuterAngle < 180)
	{
		float sinA = sphereRadius / vlen;
		auto vnorm = v / vlen;
		float cosv = D3DXVec3Dot(&vnorm, &worldLight->Direction);

		float ac = acos(cosv);
		float as = asin(sinA);

		float diff = ac - as;

		if (D3DXToRadian(worldLight->OuterAngle) < diff)
		{
			return false;
		}
	}

	return true;
}