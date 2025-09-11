#pragma once
#include "RenderTarget.h"

struct DownSampleTex
{
	LPDIRECT3DTEXTURE9 Texture = nullptr;
	LPDIRECT3DSURFACE9 Surface = nullptr;
	float width = 0;
	float height = 0;

	void Release()
	{
		if (this->Texture)
		{
			this->Texture->Release();
			this->Texture = nullptr;
		}

		if (this->Surface)
		{
			this->Surface->Release();
			this->Surface = nullptr;
		}
	}
};

std::vector<DownSampleTex> DownSampleTexures;
DownSampleTex SysTex;

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

void InitDownSampleTextures()
{
	if (DownSampleTexures.size() > 0)
	{
		return;
	}

	auto renderTarget = RenderTarget::Player;

	int width = renderTarget->resolution_x;
	int height = renderTarget->resolution_y;

	bool flag = false;
	while (true)
	{
		DownSampleTex dst;

		dst.width = width;
		dst.height = height;

		Game::Device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &dst.Texture, NULL);
		dst.Texture->GetSurfaceLevel(0, &dst.Surface);

		DownSampleTexures.push_back(dst);

		if (width == 1 && height == 1)
		{
			break;
		}

		width = max(1, width / 2);
		height = max(1, height / 2);
	}

	Game::Device->CreateTexture(1, 1, 1, 0, D3DFMT_R32F, D3DPOOL_SYSTEMMEM, &SysTex.Texture, nullptr);
	SysTex.Texture->GetSurfaceLevel(0, &SysTex.Surface);
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

	for (auto dst : DownSampleTexures)
	{
		dst.Release();
	}

	DownSampleTexures.clear();

	SysTex.Release();

	__asm popad;
}

void InitDirectResources()
{
	injector::MakeCALL(0x006BD622, ResetPrepassTextures);
}