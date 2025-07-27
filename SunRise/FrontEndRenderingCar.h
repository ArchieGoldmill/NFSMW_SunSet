#pragma once
#include "Node.h"
#include "RideInfo.h"
#include "CarRenderInfo.h"

struct FrontEndRenderingCar : bTNode<FrontEndRenderingCar>
{
	inline static auto& List = *(FrontEndRenderingCar**)(0x009B27EC);

	RideInfo mRideInfo;
	CarRenderInfo* pCarRenderInfo;
	int ViewID;
	D3DXVECTOR4 Position;
	D3DXMATRIX BodyMatrix;
};