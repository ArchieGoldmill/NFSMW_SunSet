#pragma once

IDirect3DSurface9* msaaRT = nullptr;
IDirect3DTexture9* resolvedTex = nullptr;
IDirect3DSurface9* resolvedSurf = nullptr;

void InitPrepassTextures()
{
	if (!msaaRT)
	{
		auto renderTarget = RenderTarget::Player;

		Game::Device->CreateRenderTarget(
			renderTarget->resolution_x,
			renderTarget->resolution_y,
			D3DFMT_R32F,
			Game::DeviceParams->MultiSampleType,
			Game::DeviceParams->MultiSampleQuality,
			FALSE,
			&msaaRT,
			nullptr
		);

		Game::Device->CreateTexture(
			renderTarget->resolution_x,
			renderTarget->resolution_y,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_R32F,
			D3DPOOL_DEFAULT,
			&resolvedTex,
			nullptr
		);

		resolvedTex->GetSurfaceLevel(0, &resolvedSurf);
	}
}

void SetDepthRenderTarget()
{
	auto renderTarget = RenderTarget::Player;
	Game::Device->SetRenderTarget(0, msaaRT);
	Game::Device->SetDepthStencilSurface(renderTarget->d3d_depth_stencil);

	D3DVIEWPORT9 viewport;
	viewport.Height = renderTarget->resolution_y;
	viewport.Width = renderTarget->resolution_x;
	viewport.X = 0;
	viewport.Y = 0;
	viewport.MinZ = 0.0;
	viewport.MaxZ = 1.0;
	Game::Device->SetViewport(&viewport);
}

bool DepthPrePasss = false;
void DoDepthPrePass(GrandSceneryCullInfo* cullInfo)
{
	InitPrepassTextures();
	SetDepthRenderTarget();

	DepthPrePasss = true;

	Game::StuffSky(eView::Player, 1.0, 0);
	cullInfo->StuffScenery(eView::Player, 0x20);
	WorldModel::RenderAll(eView::Player);

	Game::CommitRenderedModels();
	DepthPrePasss = false;

	Game::Device->StretchRect(msaaRT, nullptr, resolvedSurf, nullptr, D3DTEXF_NONE);

	auto filterEffect = eEffect::Get(shader_type::FilterShader);
	filterEffect->SetTexture(shader_param::HeightMapTexture, resolvedTex);
}

void __stdcall SetRenderStateHook(IDirect3DDevice9* device, D3DRENDERSTATETYPE state, DWORD value)
{
	if (RenderTarget::Current->ViewId == ViewId::Player1 && eEffect::Current->id != shader_type::CarShader)
	{
		device->SetRenderState(state, DepthPrePasss);
	}
	else
	{
		device->SetRenderState(state, value);
	}
}

void __cdecl SimpleAnimApplyHook(eModel* model, eSolid* solid, D3DXMATRIX* matrix)
{
	if (DepthPrePasss)
	{
		Game::SimpleAnimApply(model, solid, matrix);
	}
}

void InitDepthPrePass()
{
	injector::MakeNOP(0x006C6983, 6);
	injector::MakeCALL(0x006C6983, SetRenderStateHook);

	injector::MakeCALL(0x006DAA2B, SimpleAnimApplyHook);
}