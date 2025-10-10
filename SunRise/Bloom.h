#pragma once

void ExtractBloom()
{
	Game::Device->SetRenderTarget(0, Game::FilterSurface0);
	Game::Device->SetDepthStencilSurface(0);

	Game::Device->StretchRect(Game::BackBuffer, nullptr, Game::FilterSurface1, nullptr, D3DTEXF_NONE);

	auto effect = eEffect::Get(shader_type::ScreenFilterShader);
	auto pEffect = effect->D3DEffect;

	effect->SetTechniqueByName("ExtractBloom");

	UINT passes = 0;
	Game::Device->SetVertexDeclaration(effect->VertexDecl);
	pEffect->Begin(&passes, 0);
	pEffect->BeginPass(0);

	effect->DrawFullScreenQuad(Game::FilterTexture1);

	pEffect->EndPass();
	pEffect->End();
}

void BlurBloom()
{
	Game::Device->SetRenderTarget(0, Game::FilterSurface1);
	Game::Device->SetDepthStencilSurface(0);

	auto effect = eEffect::Get(shader_type::ScreenFilterShader);
	auto pEffect = effect->D3DEffect;

	effect->SetTechniqueByName("Blur");

	UINT passes = 0;
	Game::Device->SetVertexDeclaration(effect->VertexDecl);
	pEffect->Begin(&passes, 0);

	pEffect->BeginPass(0);
	effect->DrawFullScreenQuad(Game::FilterTexture0);
	pEffect->EndPass();

	pEffect->End();


	Game::Device->SetRenderTarget(0, Game::FilterSurface0);
	Game::Device->SetDepthStencilSurface(0);

	pEffect->Begin(&passes, 0);

	pEffect->BeginPass(1);
	effect->DrawFullScreenQuad(Game::FilterTexture1);
	pEffect->EndPass();

	pEffect->End();
}

void ApplyBloom()
{
	Game::Device->StretchRect(Game::BackBuffer, nullptr, Game::FilterSurface1, nullptr, D3DTEXF_NONE);

	Game::Device->SetRenderTarget(0, Game::BackBuffer);
	Game::Device->SetDepthStencilSurface(0);

	auto effect = eEffect::Get(shader_type::ScreenFilterShader);
	auto pEffect = effect->D3DEffect;

	effect->SetTechniqueByName("ApplyBloom");

	UINT passes = 0;
	Game::Device->SetVertexDeclaration(effect->VertexDecl);
	pEffect->Begin(&passes, 0);

	pEffect->BeginPass(0);
	effect->SetTexture(shader_param::FILTERTEXTURE0, Game::FilterTexture0); // bloom texture
	effect->SetTexture(shader_param::FILTERTEXTURE1, FilterTexture);
	effect->DrawFullScreenQuad(Game::FilterTexture1);
	pEffect->EndPass();

	pEffect->End();
}

void DrawBloom()
{
	auto bloomParams = g_Weather.GetBloom();

	auto effect = eEffect::Get(shader_type::ScreenFilterShader);
	effect->SetVector(ShaderParam::cvBloomParams, &bloomParams);

	D3DXVECTOR4 filterParams;
	filterParams.x = g_Config.BlurDepth;
	filterParams.y = g_Config.FilterPower;
	effect->SetVector(shader_param::FILTERBLEND, &filterParams);

	if (bloomParams.x > 0)
	{
		ExtractBloom();
		BlurBloom();
	}

	ApplyBloom();
}