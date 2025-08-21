#pragma once
#include "GarageMainScreen.h"

IDirect3DSurface9* DepthRenderTarget = nullptr;
IDirect3DTexture9* DepthTexture = nullptr;
IDirect3DSurface9* DepthSurface = nullptr;

void InitPrepassTextures()
{
	if (!DepthRenderTarget)
	{
		auto renderTarget = RenderTarget::Player;

		Game::Device->CreateRenderTarget(
			renderTarget->resolution_x,
			renderTarget->resolution_y,
			D3DFMT_R32F,
			Game::DeviceParams->MultiSampleType,
			Game::DeviceParams->MultiSampleQuality,
			FALSE,
			&DepthRenderTarget,
			nullptr
		);

		Game::Device->CreateTexture(
			renderTarget->resolution_x,
			renderTarget->resolution_y,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_R32F,
			D3DPOOL_DEFAULT,
			&DepthTexture,
			nullptr
		);

		DepthTexture->GetSurfaceLevel(0, &DepthSurface);
	}
}

void ResetPrepassTextures()
{
	__asm pushad;

	if (DepthRenderTarget)
	{
		DepthRenderTarget->Release();
		DepthRenderTarget = nullptr;
	}

	if (DepthTexture)
	{
		DepthTexture->Release();
		DepthTexture = nullptr;
	}

	if (DepthSurface)
	{
		DepthSurface->Release();
		DepthSurface = nullptr;
	}

	__asm popad;
}

void SetDepthRenderTarget()
{
	auto renderTarget = RenderTarget::Player;
	Game::Device->SetRenderTarget(0, DepthRenderTarget);
	Game::Device->SetDepthStencilSurface(renderTarget->d3d_depth_stencil);

	Game::Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

	D3DVIEWPORT9 viewport;
	viewport.Height = renderTarget->resolution_y;
	viewport.Width = renderTarget->resolution_x;
	viewport.X = 0;
	viewport.Y = 0;
	viewport.MinZ = 0.0;
	viewport.MaxZ = 1.0;
	Game::Device->SetViewport(&viewport);
}

bool DepthPrePass = false;
void DoDepthPrePass(GrandSceneryCullInfo* cullInfo)
{
	InitPrepassTextures();
	SetDepthRenderTarget();

	DepthPrePass = true;

	Game::StuffSky(eView::Player, 1.0, 0);
	cullInfo->StuffScenery(eView::Player, 0x20);
	WorldModel::RenderAll(eView::Player);
	if (Game::State == 3)
	{
		auto manager = GarageMainScreen::GetInstance();
		if (manager)
		{
			manager->HandleRender();
		}
	}

	Game::CommitRenderedModels();
	DepthPrePass = false;

	Game::Device->StretchRect(DepthRenderTarget, nullptr, DepthSurface, nullptr, D3DTEXF_NONE);
}

void __stdcall SetRenderStateHook(IDirect3DDevice9* device, D3DRENDERSTATETYPE state, DWORD value)
{
	if (RenderTarget::Current->ViewId == ViewId::Reflection && eEffect::Current->id == shader_type::skyshader)
	{
		device->SetRenderState(state, false);
	}
	else if (RenderTarget::Current->ViewId == ViewId::Player1 && eEffect::Current->id != shader_type::CarShader)
	{
		device->SetRenderState(state, DepthPrePass);
	}
	else
	{
		device->SetRenderState(state, value);
	}
}

void __cdecl SimpleAnimApplyHook(eModel* model, eSolid* solid, D3DXMATRIX* matrix)
{
	if (DepthPrePass)
	{
		Game::SimpleAnimApply(model, solid, matrix);
	}
}

void InitDepthPrePass()
{
	if (g_Config.DepthPrepass)
	{
		injector::MakeNOP(0x006C6983, 6);
		injector::MakeCALL(0x006C6983, SetRenderStateHook);

		injector::MakeCALL(0x006DAA2B, SimpleAnimApplyHook);

		injector::MakeCALL(0x006BD622, ResetPrepassTextures);
	}
}