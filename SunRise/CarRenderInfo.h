#pragma once
#include "LightFlare.h"
#include "Node.h"
#include "RideInfo.h"

enum VehicleFX
{
	VehicleFX_NONE = 0,
	VehicleFX_LHEAD = 1,
	VehicleFX_RHEAD = 2,
	VehicleFX_CHEAD = 4,
	VehicleFX_HEADLIGHTS = 7,
	VehicleFX_LBRAKE = 8,
	VehicleFX_RBRAKE = 0x10,
	VehicleFX_CBRAKE = 0x20,
	VehicleFX_BRAKELIGHTS = 0x38,
	VehicleFX_LIGHTS = 0x3F,
	VehicleFX_LREVERSE = 0x40,
	VehicleFX_RREVERSE = 0x80,
	VehicleFX_REVERSE = 0xC0,
	VehicleFX_LRSIGNAL = 0x100,
	VehicleFX_RRSIGNAL = 0x200,
	VehicleFX_LFSIGNAL = 0x400,
	VehicleFX_LSIGNAL = 0x5000,
	VehicleFX_RFSIGNAL = 0x800,
	VehicleFX_RSIGNAL = 0xA00,
	VehicleFX_COPRED = 0x1000,
	VehicleFX_COPBLUE = 0x2000,
	VehicleFX_COPWHITE = 0x4000,
	VehicleFX_COPS = 0x7000
};

struct CarRenderInfo
{
	int field_0;
	int field_4;
	int field_8;
	int field_C;
	int field_10;
	int field_14;
	int field_18;
	int field_1C;
	int field_20;
	int field_24;
	int field_28;
	int field_2C;
	int field_30;
	int field_34;
	int field_38;
	int field_3C;
	int field_40;
	int field_44;
	int field_48;
	int field_4C;
	int field_50;
	int field_54;
	int field_58;
	int field_5C;
	int field_60;
	int field_64;
	int field_68;
	int field_6C;
	int field_70;
	int field_74;
	int field_78;
	int field_7C;
	int field_80;
	RideInfo* pRideInfo;
	int field_88;
	int field_8C;
	int field_90;
	int field_94;
	int field_98;
	int field_9C;
	int field_A0;
	int field_A4;
	int field_A8;
	int field_AC;
	int field_B0;
	int field_B4;
	int field_B8;
	int field_BC;
	int field_C0;
	int field_C4;
	int field_C8;
	int field_CC;
	int field_D0;
	int field_D4;
	int field_D8;
	int field_DC;
	int field_E0;
	int field_E4;
	int field_E8;
	int field_EC;
	int field_F0;
	int field_F4;
	int field_F8;
	int field_FC;
	int field_100;
	LightFlare* LightFlares;
	int field_10C;

	bool IsLightOn(int light)
	{
		FUNC(0x007376B0, bool, __thiscall, _IsLightOn, CarRenderInfo*,int);
		return _IsLightOn(this, light);
	}
};
