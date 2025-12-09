#pragma once
#include <d3dx9.h>
#include "Node.h"

enum class eLightFlareType : std::uint8_t
{
	headlight_inner,
	headlight_outer,
	brakelight_inner,
	brakelight_traffic_inner,
	reverselight_inner,
	foglight_inner,
	coplightred_inner,
	coplightblue_inner,
	coplightwhite_inner,
	coplightbrightblue,
	coplightbrightred,
	coplightorange,
	lamppost_inner,
	lamppost_outer,
	catseye_orange,
	catseye_red,
	catseye_blue,
	traffic_light,
	traffic_light_red,
	green_flare,
	hand_flare_red,
	sun_flare,
	count,
};

struct Color
{
	union
	{
		DWORD rgba;
		struct
		{
			BYTE r;
			BYTE g;
			BYTE b;
			BYTE a;
		};
	};
};

struct LightFlareParams
{
	float MinSize;
	float MaxSize;
	D3DXVECTOR4 Color;
	float Power;
	float ZBias;
	float MinScale;
	float MaxScale;
	int TextureIndex;
};

struct LightFlare : bTNode<LightFlare>
{
	int NameHash;
	Color Tint;
	D3DXVECTOR3 Position;
	float ReflectPosZ;
	D3DXVECTOR3 Direction;
	eLightFlareType Type;
	char Flags;
};
