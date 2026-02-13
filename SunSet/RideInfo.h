#pragma once
#include "Utilities.h"

enum class CarRenderUsage
{
	Player = 0,
	RemotePlayer = 1,
	AIRacer = 2,
	AICop = 3,
	AITraffic = 4,
	AIHeli = 5,
	NISCar = 6,
	Ghost = 7,
	Invalid = 8
};

struct RideInfo
{
	int Type;
	char InstanceIndex;
	char HasDash;
	char CanBeVertexDamaged;
	char SkinType;
	int mMinLodLevel;
	int mMaxLodLevel;
	int mMinFELodLevel;
	int mMaxFELodLevel;
	int mMaxLicenseLodLevel;
	int mMinTrafficDiffuseLodLevel;
	int mMinShadowLodLevel;
	int mMaxShadowLodLevel;
	int mMaxTireLodLevel;
	int mMaxBrakeLodLevel;
	int mMaxSpoilerLodLevel;
	int mMaxRoofScoopLodLevel;
	int mMinReflectionLodLevel;
	Hash mCompositeSkinHash;
	Hash mCompositeWheelHash;
	Hash mCompositeSpinnerHash;
	void* mPartsTable[139];
	char mPartsEnabled[139];
	char Padding_2FF[1];
	void* PreviewPart;
	int mMyCarLoaderHandle;
	CarRenderUsage mCarRenderUsage;
	char mSpecialLODBehavior;
	char Padding_30D[3];
};

ASSERT_SIZE(RideInfo, 0x310);