#pragma once
#include "DepthPrePass.h"

int RenderCarsParam1;
int RenderCarsParam2;
void __cdecl RenderCars(int a, int b)
{
	if (!Game::MotionBlurEnable)
	{
		Game::VehicleConn_RenderCars(a, b);
	}
	else
	{
		RenderCarsParam1 = a;
		RenderCarsParam2 = b;
	}
}

void DrawBlur()
{
	Game::DrawMotionBlur();
	Game::VehicleConn_RenderCars(RenderCarsParam1, RenderCarsParam2);
	Game::CommitRenderedModels();
}

void __stdcall CopyBufferForBlur(IDirect3DDevice9* device, IDirect3DSurface9* backBuffer, RECT* pSourceRect, IDirect3DSurface9* filterSurface0, RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
	auto filterSurface1 = (IDirect3DSurface9**)0x0093DE64;
	auto filterTexture1 = (IDirect3DTexture9**)0x0093DEFC;

	// Copy back buffer to filter 1
	Game::Device->StretchRect(backBuffer, nullptr, *filterSurface1, nullptr, D3DTEXF_NONE);

	// Set filter 0 as target
	Game::Device->SetRenderTarget(0, filterSurface0);
	Game::Device->SetDepthStencilSurface(0);

	auto effect = eEffect::Get(shader_type::ScreenFilterShader);
	auto pEffect = effect->D3DEffect;

	UINT passes = 0;
	pEffect->Begin(&passes, 0);
	pEffect->BeginPass(0);

	// Set depth texture
	effect->SetTexture(shader_param::HeightMapTexture, resolvedTex);
	// Set filter 1 texture that holds back buffer
	effect->DrawFullScreenQuad(*filterTexture1);

	pEffect->EndPass();
	pEffect->End();
}

void InitMotionBlur()
{
	injector::MakeCALL(0x006DEE3F, RenderCars);
	injector::MakeCALL(0x006DF1DC, DrawBlur);

	// Restore blur from wide screen fix
	injector::WriteMemory<BYTE>(0x006DF1D2, 0x74);

	// Remove blur lerp
	injector::MakeNOP(0x006DBE28, 2);

	// Remove second sky stuff
	injector::MakeNOP(0x006DF449, 5);

	if (g_Config.DepthPrepass)
	{
		injector::MakeNOP(0x006DBD7C, 6);
		injector::MakeCALL(0x006DBD7C, CopyBufferForBlur);

		injector::WriteMemory<BYTE>(0x006DBE9E, D3DBLEND_INVSRCALPHA);
		injector::WriteMemory<BYTE>(0x006DBEA0, D3DBLEND_SRCALPHA);
	}
}