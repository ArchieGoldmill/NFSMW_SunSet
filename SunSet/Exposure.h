#pragma once

inline float g_Exposure = 1.0;

void ExtractLuminance()
{
	Game::Device->StretchRect(Game::BackBuffer, nullptr, Game::FilterSurface0, nullptr, D3DTEXF_NONE);

	Game::Device->SetRenderTarget(0, Exposure::Downsample[0].Surface);
	Game::Device->SetDepthStencilSurface(0);

	auto effect = eEffect::Get(shader_type::ScreenFilterShader);
	auto pEffect = effect->D3DEffect;

	Game::Device->SetVertexDeclaration(effect->VertexDecl);
	effect->SetTechniqueByName("Luminance");

	UINT passes = 0;
	pEffect->Begin(&passes, 0);
	pEffect->BeginPass(0);

	effect->DrawFullScreenQuad(Game::FilterTexture0);

	pEffect->EndPass();
	pEffect->End();
}

void DownSampleLuminance()
{
	auto effect = eEffect::Get(shader_type::ScreenFilterShader);
	auto pEffect = effect->D3DEffect;
	effect->SetTechniqueByName("Exposure");
	Game::Device->SetVertexDeclaration(effect->VertexDecl);
	UINT passes = 0;

	for (int i = 0; i < Exposure::Downsample.size() - 1; i++)
	{
		auto target = Exposure::Downsample[i + 1];
		Game::Device->SetRenderTarget(0, target.Surface);
		Game::Device->SetDepthStencilSurface(0);

		pEffect->Begin(&passes, 0);
		pEffect->BeginPass(0);

		D3DXVECTOR4 texelSize;
		texelSize.x = 1.0 / Exposure::Downsample[i].width;
		texelSize.y = 1.0 / Exposure::Downsample[i].height;

		effect->SetVector(ShaderParam::cvTexelSize, &texelSize);
		effect->DrawFullScreenQuad(Exposure::Downsample[i].Texture, target.width, target.height);

		pEffect->EndPass();
		pEffect->End();
	}
}

float GetCurrentLuminance()
{
	auto tex1x1 = Exposure::Downsample[Exposure::Downsample.size() - 1];

	Game::Device->GetRenderTargetData(tex1x1.Surface, Exposure::SysTex.Surface);

	D3DLOCKED_RECT rect;
	Exposure::SysTex.Texture->LockRect(0, &rect, nullptr, D3DLOCK_READONLY);

	float* pPixel = (float*)rect.pBits;
	float avgLuminance = pPixel[0];

	Exposure::SysTex.Texture->UnlockRect(0);

	return avgLuminance;
}

bool IsExposureEnabled()
{
	if (g_Config.Exposure && Game::State == 6)
	{
		auto exp = g_Weather.GetExposure();
		return exp.y != exp.z;
	}

	return false;
}

float logAdaptedLum = logf(0.18f);
float g_CurrentExposure = 1;

float GetExposure()
{
	if (g_Config.Exposure && Game::State == 6)
	{
		if (!Game::IsPaused())
		{
			auto params = g_Weather.GetExposure();

			float adaptationRate = 1.0f;
			float minExposure = params.y;
			float maxExposure = params.z;
			float key = params.x;

			if (minExposure == maxExposure)
			{
				return minExposure;
			}

			float logCurrLum = GetCurrentLuminance();

			logAdaptedLum += (logCurrLum - logAdaptedLum) * adaptationRate * Game::DeltaTime;

			float sceneLum = exp(logAdaptedLum);

			float exposure = key / sceneLum;

			minExposure = min(minExposure, maxExposure);
			maxExposure = max(minExposure, maxExposure);

			if (exposure > maxExposure)
			{
				MoveTowards(g_CurrentExposure, maxExposure, Game::DeltaTime);
			}
			else if (exposure < minExposure)
			{
				MoveTowards(g_CurrentExposure, minExposure, Game::DeltaTime);
			}
			else
			{
				g_CurrentExposure = exposure;
			}
		}

		return g_CurrentExposure;
	}

	return 1.0;
}

void ApplyExposure()
{
	if (IsExposureEnabled())
	{
		Exposure::Init();

		ExtractLuminance();
		DownSampleLuminance();
	}
}