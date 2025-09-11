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

void LoadSkyBoxTexture()
{
	if (!SkyCubeTexture)
	{
		D3DXCreateCubeTextureFromFileA(Game::Device, "scripts\\SunSetData\\Textures\\SKYBOX.dds", &SkyCubeTexture);
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
	if (!SkyCubeTexture)
	{
		D3DXCreateCubeTexture(Game::Device, 1024, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &SkyCubeTexture);

		D3DCUBEMAP_FACES faces[6] =
		{
			D3DCUBEMAP_FACE_POSITIVE_X,
			D3DCUBEMAP_FACE_NEGATIVE_X,
			D3DCUBEMAP_FACE_POSITIVE_Y,
			D3DCUBEMAP_FACE_NEGATIVE_Y,
			D3DCUBEMAP_FACE_POSITIVE_Z,
			D3DCUBEMAP_FACE_NEGATIVE_Z
		};

		const char* filenames[6] =
		{
			"D:\\x+.jpg",
			"D:\\x-.jpg",
			"D:\\y+.jpg",
			"D:\\y-.jpg",
			"D:\\z+.jpg",
			"D:\\z-.jpg",
		};

		for (int i = 0; i < 6; i++)
		{
			LPDIRECT3DSURFACE9 pSurface = nullptr;
			SkyCubeTexture->GetCubeMapSurface(faces[i], 0, &pSurface);
			D3DXLoadSurfaceFromFileA(pSurface, NULL, NULL, filenames[i], NULL, D3DX_DEFAULT, 0, NULL);
			pSurface->Release();
		}

		D3DXSaveTextureToFileA("D:\\SKYBOX.dds", D3DXIFF_DDS, SkyCubeTexture, NULL);
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