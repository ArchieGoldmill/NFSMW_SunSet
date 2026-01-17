#pragma once
#include "WorldModel.h"
#include "Config.h"

void __cdecl CustomMeshesShadowRenderHook(eView* view, int a)
{
	WorldModel::RenderAll(view, a);
	CustomMeshes.Draw(view);
}

void InitCustomMeshes()
{
	injector::MakeCALL(0x006E5110, CustomMeshesShadowRenderHook);
	injector::MakeCALL(0x006DED79, CustomMeshesShadowRenderHook);
}