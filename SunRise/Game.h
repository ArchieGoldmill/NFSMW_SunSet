#pragma once
#include "Utilities.h"
#include "LightFlare.h"
#include "eView.h"

namespace Game
{
	inline bool& ReloadShaders = *(bool*)0x00982B14;
	IDirect3DDevice9*& Device = *(IDirect3DDevice9**)0x00982BDC;

	INLINE_FUNC(0x00505380, void, __cdecl, eRenderLightFlare, eView* view, LightFlare* flare, D3DXMATRIX* local_world, float intensity_scale, int reflexionAction, int destinationType, float reflectionOverride, D3DCOLOR colourOverRide, float sizescale);
	INLINE_FUNC(0x00460BF0, Hash, __cdecl, bStringHash, const char*);
}