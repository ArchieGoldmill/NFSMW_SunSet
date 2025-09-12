#pragma once
#include "RenderTarget.h"
#include "resource.h"

IDirect3DSurface9* DepthRenderTarget = nullptr;
IDirect3DTexture9* DepthTexture = nullptr;
IDirect3DSurface9* DepthSurface = nullptr;
LPDIRECT3DVOLUMETEXTURE9 NoiseTexture = NULL;
IDirect3DCubeTexture9* SkyCubeTexture = nullptr;

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

	__asm popad;
}

void InitDirectResources()
{
	injector::MakeCALL(0x006BD622, ReleaseDirectResources);
}