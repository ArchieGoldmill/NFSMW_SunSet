#pragma once
#include "CarRenderInfo.h"

struct VehicleRenderConn
{
	inline static auto& List = *(VehicleRenderConn***)(0x009B37CC);
	inline static auto& ListCount = *(int*)(0x009B37D4);

	int vTable;
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
	D3DXMATRIX* Matrix;
	int field_38;
	int field_3C;
	int field_40;
	CarRenderInfo* pCarRenderInfo;
	int field_48;
};