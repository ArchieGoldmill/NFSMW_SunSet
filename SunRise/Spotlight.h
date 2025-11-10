#pragma once
#include <d3dx9.h>
#include <vector>
#include <string>
#include "Utilities.h"
#include "eLightFlareParams.h"
#include "TextureInfo.h"
#include "HashField.h"

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

	void GetBoundingBox(D3DXVECTOR3* min, D3DXVECTOR3* max)
	{
		D3DXVECTOR3 range = { this->Range, this->Range, this->Range };
		*min = this->Position - range;

		if (this->IsLampPost())
		{
			range.z = 0;
		}

		*max = this->Position + range;
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
	Reverse,
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
	float Distance;
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

inline bool ConeSphereIntersect(SpotLight* worldLight, const D3DXVECTOR3& sphereCenter, float sphereRadius)
{
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