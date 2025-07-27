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
	D3DXMATRIX TireMatrices[4];
	D3DXMATRIX BrakeMatrices[4];
	eModel* OverrideModel;
	int Visible;
	int nPasses;
	int Reflection;
	int LightsOn;
	int CopLightsOn;
	BYTE padding[8];
};