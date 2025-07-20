#pragma once
#include <d3dx9.h>
#include "Node.h"

enum class eLightFlareType : std::uint8_t
{
	car_headlight,
	car_brakelight,
	car_traffic_brakelight,
	car_reverse_light,
	car_fog_light,
	car_cop_light_red,
	car_cop_light_blue,
	car_cop_light_white,
	car_cop_headlight_right,
	car_cop_headlight_left,
	car_cop_light_bright_red,
	car_cop_light_bright_blue,
	car_cop_light_orange,
	lamppost,
	catseye_orange,
	catseye_red,
	catseye_blue,
	blinking_amber,
	blinking_red,
	blinking_green,
	hand_flare,
	sun_flare,
	generic_1,
	generic_2,
	generic_3,
	generic_4,
	generic_5,
	generic_6,
	generic_7,
	generic_8,
	generic_9,
	generic_10,
	count,
};

struct LightFlare : bTNode<LightFlare>
{
	int NameHash;
	D3DCOLOR Tint;
	D3DXVECTOR3 Position;
	float ReflectPosZ;
	D3DXVECTOR3 Direction;
	eLightFlareType Type;
	char Flags;
};
