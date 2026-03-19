#pragma once
#include "RenderTarget.h"
#include "Config.h"

IDirect3DSurface9* DepthRenderTarget = nullptr;
IDirect3DTexture9* DepthTexture = nullptr;
IDirect3DSurface9* DepthSurface = nullptr;
LPDIRECT3DVOLUMETEXTURE9 NoiseTexture = nullptr;
IDirect3DVolumeTexture9* FilterTexture = nullptr;

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

void LoadVolumeTexture()
{
	if (!NoiseTexture)
	{
		D3DXCreateVolumeTextureFromFileA(Game::Device, "scripts\\SunSetData\\volume.dds", &NoiseTexture);
	}
}

void CreateSkyBoxTexture(Hash* textures, IDirect3DCubeTexture9*& skyCubeTexture)
{
	D3DCUBEMAP_FACES faces[6] =
	{
		D3DCUBEMAP_FACE_POSITIVE_X,
		D3DCUBEMAP_FACE_NEGATIVE_X,
		D3DCUBEMAP_FACE_POSITIVE_Y,
		D3DCUBEMAP_FACE_NEGATIVE_Y,
		D3DCUBEMAP_FACE_POSITIVE_Z,
		D3DCUBEMAP_FACE_NEGATIVE_Z
	};

	auto textureInfo = TextureInfo::Get(textures[0], false, false);
	if (!textureInfo)
	{
		return;
	}

	D3DXCreateCubeTexture(Game::Device, textureInfo->Width, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &skyCubeTexture);

	for (int i = 0; i < 6; i++)
	{
		LPDIRECT3DSURFACE9 pSurface = nullptr;
		skyCubeTexture->GetCubeMapSurface(faces[i], 0, &pSurface);

		auto textureInfo = TextureInfo::Get(textures[i], false, false);
		auto texture = textureInfo->PlatInfo->D3DTexture;

		LPDIRECT3DSURFACE9 pSrcSurface = nullptr;
		texture->GetSurfaceLevel(0, &pSrcSurface);

		D3DXLoadSurfaceFromSurface(pSurface, NULL, NULL, pSrcSurface, NULL, NULL, D3DX_DEFAULT, 0);

		pSurface->Release();
		pSrcSurface->Release();
	}
}

struct VolumeFillData
{
	int Width;
	BYTE* Data;
};

VOID WINAPI FillVolume(D3DXVECTOR4* pOut, const D3DXVECTOR3* pTexCoord, const D3DXVECTOR3* pTexelSize, LPVOID pData)
{
	auto fillData = (VolumeFillData*)pData;

	auto size = fillData->Width;

	int x = (int)(pTexCoord->x * size);
	int y = (int)(pTexCoord->y * size);
	int z = (int)(pTexCoord->z * size);

	int index = x + y * (int)size + z * (int)(size * size);
	BYTE* pPixel = fillData->Data + index * 4;

	const float one255 = 1.0f / 255.0f;

	pOut->x = pPixel[2] * one255;
	pOut->y = pPixel[1] * one255;
	pOut->z = pPixel[0] * one255;
	pOut->w = 1.0f;
}

void CreateFilterTexture()
{
	if (!FilterTexture)
	{
		auto textureInfo = TextureInfo::Get(g_Config.Filter.hash, false, false);

		if (textureInfo)
		{
			auto width = textureInfo->Width;
			Game::Device->CreateVolumeTexture(width, width, width, 1u, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &FilterTexture, 0);

			VolumeFillData fillData;
			fillData.Data = (BYTE*)textureInfo->LockImage();
			fillData.Width = width;

			D3DXFillVolumeTexture(FilterTexture, FillVolume, &fillData);

			textureInfo->UnlockImage();
		}
	}
}

namespace Exposure
{
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

	std::vector<DownSampleTex> Downsample;
	DownSampleTex SysTex;

	void Init()
	{
		if (Downsample.size() > 0)
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

			Downsample.push_back(dst);

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

	void Release()
	{
		for (auto dst : Downsample)
		{
			dst.Release();
		}

		Downsample.clear();

		SysTex.Release();
	}
}

void ReleaseFilterTexture()
{
	if (FilterTexture)
	{
		FilterTexture->Release();
		FilterTexture = nullptr;
	}
}

void ReleaseDirectResources()
{
	__asm pushad;

	if (NoiseTexture)
	{
		NoiseTexture->Release();
		NoiseTexture = nullptr;
	}

	if (DepthRenderTarget)
	{
		DepthRenderTarget->Release();
		DepthRenderTarget = nullptr;
	}

	if (DepthSurface)
	{
		DepthSurface->Release();
		DepthSurface = nullptr;
	}

	if (DepthTexture)
	{
		DepthTexture->Release();
		DepthTexture = nullptr;
	}

	ReleaseFilterTexture();

	Exposure::Release();

	__asm popad;
}