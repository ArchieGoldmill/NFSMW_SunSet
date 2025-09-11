#pragma once

void ExtractLuminance()
{
	Game::Device->StretchRect(Game::BackBuffer, nullptr, Game::FilterSurface0, nullptr, D3DTEXF_NONE);

	Game::Device->SetRenderTarget(0, DownSampleTexures[0].Surface);
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

	for (int i = 0; i < DownSampleTexures.size() - 1; i++)
	{
		Game::Device->SetRenderTarget(0, DownSampleTexures[i + 1].Surface);
		Game::Device->SetDepthStencilSurface(0);

		pEffect->Begin(&passes, 0);
		pEffect->BeginPass(0);

		D3DXVECTOR4 texelSize;
		texelSize.x = 1.0 / DownSampleTexures[i].width;
		texelSize.y = 1.0 / DownSampleTexures[i].height;

		effect->SetVector(ShaderParam::cvTexelSize, &texelSize);
		effect->DrawFullScreenQuad(DownSampleTexures[i].Texture);

		pEffect->EndPass();
		pEffect->End();
	}
}

float GetCurrentLuminance()
{
	auto tex1x1 = DownSampleTexures[DownSampleTexures.size() - 1];

	Game::Device->GetRenderTargetData(tex1x1.Surface, SysTex.Surface);

	D3DLOCKED_RECT rect;
	SysTex.Texture->LockRect(0, &rect, nullptr, D3DLOCK_READONLY);

	float* pPixel = (float*)rect.pBits;
	float avgLuminance = pPixel[0];

	SysTex.Texture->UnlockRect(0);

	return avgLuminance;
}

float logAdaptedLum = logf(0.18f);
float GetExposure()
{
	return 1.0;

	float key = 0.18f;
	float adaptationRate = 1.0f;

	float currLum = GetCurrentLuminance();
	float logCurrLum = logf(max(currLum, 0.0001f));

	logAdaptedLum += (logCurrLum - logAdaptedLum) * adaptationRate * Game::DeltaTime;

	float adaptedLum = expf(logAdaptedLum);

	float minLum = 0.01f;
	adaptedLum = max(adaptedLum, minLum);

	float exposure = key / adaptedLum;

	//float minExposure = 1.0f;
	//float maxExposure = 5.0f;

	//exposure = std::clamp(exposure, minExposure, maxExposure);

	return exposure;
}

void CalculateExposure()
{
	InitDownSampleTextures();
	ExtractLuminance();
	DownSampleLuminance();
}