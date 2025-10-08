#pragma once
#include "RenderTarget.h"
#include "resource.h"

IDirect3DSurface9* DepthRenderTarget = nullptr;
IDirect3DTexture9* DepthTexture = nullptr;
IDirect3DSurface9* DepthSurface = nullptr;
LPDIRECT3DVOLUMETEXTURE9 NoiseTexture = NULL;
IDirect3DCubeTexture9* SkyCubeTexture = nullptr;
IDirect3DVolumeTexture9* FilterTexture = NULL;

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
		HRSRC hRes = FindResource(Game::hModule, MAKEINTRESOURCE(IDR_RCDATA1), RT_RCDATA);
		if (hRes)
		{
			HGLOBAL hResData = LoadResource(Game::hModule, hRes);
			if (hResData)
			{
				void* pData = LockResource(hResData);
				DWORD dataSize = SizeofResource(Game::hModule, hRes);

				D3DXCreateVolumeTextureFromFileInMemory(Game::Device, pData, dataSize, &NoiseTexture);
			}
		}
	}
}

void CreateSkyBoxTexture()
{
	if (SkyCubeTexture)
	{
		return;
	}

	D3DCUBEMAP_FACES faces[6] =
	{
		D3DCUBEMAP_FACE_POSITIVE_X,
		D3DCUBEMAP_FACE_NEGATIVE_X,
		D3DCUBEMAP_FACE_POSITIVE_Y,
		D3DCUBEMAP_FACE_NEGATIVE_Y,
		D3DCUBEMAP_FACE_POSITIVE_Z,
		D3DCUBEMAP_FACE_NEGATIVE_Z
	};

	Hash textures[6] =
	{
		Hashes::SKYBOX_XPOS,
		Hashes::SKYBOX_XNEG,
		Hashes::SKYBOX_YPOS,
		Hashes::SKYBOX_YNEG,
		Hashes::SKYBOX_ZPOS,
		Hashes::SKYBOX_ZNEG,
	};

	auto textureInfo = TextureInfo::Get(textures[0], false, false);
	if (!textureInfo)
	{
		return;
	}

	D3DXCreateCubeTexture(Game::Device, textureInfo->Width, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &SkyCubeTexture);

	for (int i = 0; i < 6; i++)
	{
		LPDIRECT3DSURFACE9 pSurface = nullptr;
		SkyCubeTexture->GetCubeMapSurface(faces[i], 0, &pSurface);

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

	if (SkyCubeTexture)
	{
		SkyCubeTexture->Release();
		SkyCubeTexture = nullptr;
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

	__asm popad;
}

void InitDirectResources()
{
	injector::MakeCALL(0x006BD622, ReleaseDirectResources);
}