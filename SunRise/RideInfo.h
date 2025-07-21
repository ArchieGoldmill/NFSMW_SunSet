#pragma once
#include "Utilities.h"

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
	int mMyCarRenderUsage;
	char mSpecialLODBehavior;
	char Padding_30D[3];
};

ASSERT_SIZE(RideInfo, 0x310);