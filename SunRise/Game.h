#pragma once
#include "Utilities.h"
#include "LightFlare.h"
#include "eView.h"
#include "Camera.h"

namespace Game
{
	inline bool& ReloadShaders = *(bool*)0x00982B14;
	IDirect3DDevice9*& Device = *(IDirect3DDevice9**)0x00982BDC;
	float& DeltaTime = *(float*)0x009259BC;
	float& FlareRotation = *(float*)0x008F2920;
	bool& LostFocus = *(bool*)0x00982C50;

	INLINE_FUNC(0x00505380, void, __cdecl, eRenderLightFlare, eView* view, LightFlare* flare, D3DXMATRIX* local_world, float intensity_scale, int reflexionAction, int destinationType, float reflectionOverride, D3DCOLOR colourOverRide, float sizescale);
	INLINE_FUNC(0x00460BF0, Hash, __cdecl, bStringHash, const char*);
	INLINE_FUNC(0x0046EB40, Camera*, __cdecl, GetPlayerCamera);
}