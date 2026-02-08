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
		Game::Device->SetRenderTarget(0, Exposure::Downsample[i + 1].Surface);
		Game::Device->SetDepthStencilSurface(0);

		pEffect->Begin(&passes, 0);
		pEffect->BeginPass(0);

		D3DXVECTOR4 texelSize;
		texelSize.x = 1.0 / Exposure::Downsample[i].width;
		texelSize.y = 1.0 / Exposure::Downsample[i].height;

		effect->SetVector(ShaderParam::cvTexelSize, &texelSize);
		effect->DrawFullScreenQuad(Exposure::Downsample[i].Texture);

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

float logAdaptedLum = logf(0.18f);
float GetExposure()
{
	if (g_Config.Exposure)
	{
		auto exp = g_Weather.GetExposure();

		float key = exp.x;

		float currLum = GetCurrentLuminance();
		float logCurrLum = logf(max(currLum, 0.0001f));

		float adaptationRate = logCurrLum > logAdaptedLum ? 3.0f : 1.5f;

		logAdaptedLum += (logCurrLum - logAdaptedLum) * adaptationRate * Game::DeltaTime;

		float adaptedLum = expf(logAdaptedLum);

		float exposure = key / adaptedLum;

		float minExposure = exp.y;
		float maxExposure = exp.z;

		minExposure = min(minExposure, maxExposure);
		maxExposure = max(minExposure, maxExposure);

		exposure = std::clamp(exposure, minExposure, maxExposure);

		return exposure;
	}

	return 1.0;
}

void ApplyExposure()
{
	if (g_Config.Exposure)
	{
		Exposure::Init();

		ExtractLuminance();
		DownSampleLuminance();
	}
}