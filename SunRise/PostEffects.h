#pragma once
#include "GodRays.h"
#include "Bloom.h"

void DrawPostEffects()
{
	IDirect3DSurface9* rtBackup;
	IDirect3DSurface9* ppZStencilSurface;
	Game::Device->GetRenderTarget(0, &rtBackup);
	Game::Device->GetDepthStencilSurface(&ppZStencilSurface);

	DrawBloom();
	DrawGodRays();

	Game::Device->SetRenderTarget(0, rtBackup);
	Game::Device->SetDepthStencilSurface(ppZStencilSurface);
}