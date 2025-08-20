#pragma once
#include "Utilities.h"
#include "LightFlare.h"
#include "eView.h"
#include "Camera.h"
#include "MeshEntry.h"
#include "eModel.h"
#include "eEffect.h"

namespace Game
{
	inline HMODULE hModule;

	inline bool& ReloadShaders = *(bool*)0x00982B14;
	inline IDirect3DDevice9*& Device = *(IDirect3DDevice9**)0x00982BDC;
	inline float& DeltaTime = *(float*)0x009259BC;
	inline float& FlareRotation = *(float*)0x008F2920;
	inline bool& LostFocus = *(bool*)0x00982C50;
	inline auto FlareBlink = (int*)0x0091601C;
	inline auto& State = *(int*)0x00925E90;
	inline auto IdentityMatrix = (D3DXMATRIX*)0x00987AB0;
	inline auto& ReflectionTexture = *(IDirect3DTexture9**)0x00982A6C;
	inline auto& X360EffectsEnable = *(int*)0x009017E0;
	inline auto& Window = *(HWND*)0x00982BF4;
	inline auto ForceRain = (bool*)0x009B0A30;
	inline auto& MotionBlurEnable = *(int*)0x009017DC;
	inline auto& SkipFE = *(int*)0x00926064;
	inline auto DeviceParams = (D3DPRESENT_PARAMETERS*)0x0093E82C;

	INLINE_FUNC(0x00505380, void, __cdecl, eRenderLightFlare, eView* view, LightFlare* flare, D3DXMATRIX* local_world, float intensity_scale, int reflexionAction, int destinationType, float reflectionOverride, D3DCOLOR colourOverRide, float sizescale);
	INLINE_FUNC(0x00460BF0, Hash, __cdecl, bStringHash, const char*);
	INLINE_FUNC(0x0046EB40, Camera*, __cdecl, GetPlayerCamera);
	INLINE_FUNC(0x007A1FA0, int*, __cdecl, GarageMainScreen_GetInstance);
	INLINE_FUNC(0x00750B10, void, __cdecl, VehicleConn_RenderCars, int, int);
	INLINE_FUNC(0x006DBB20, void, __cdecl, DrawMotionBlur);
	INLINE_FUNC(0x006E2F50, void, __cdecl, CommitRenderedModels);
	INLINE_FUNC(0x006DE210, void, __cdecl, StuffSky, eView*, float, int);
	INLINE_FUNC(0x0073D3C0, void, __cdecl, SimpleAnimApply, eModel*, eSolid*, D3DXMATRIX*);
}

inline D3DXVECTOR3 GetCameraPos()
{
	auto camera = Game::GetPlayerCamera();
	return camera->Position;
}

inline float GetCameraDistance(D3DXVECTOR3 pos)
{
	auto cameraPos = GetCameraPos();
	D3DXVECTOR3 diff = pos - cameraPos;
	return D3DXVec3Length(&diff);
}