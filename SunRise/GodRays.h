#pragma once
#include <algorithm>

D3DXVECTOR4 GetSunScreenUV()
{
	auto camPos = GetCameraPos();
	auto sunDir = D3DXVECTOR3(TimeOfDay::Instance->SunDirection);

	D3DXVECTOR3 sunPosWS = camPos + sunDir * 10000.0f;

	D3DXVECTOR4 sunClip;
	auto viewProj = eView::Player->Pinfo->non_jittered_view_projection_matrix;
	D3DXVec3Transform(&sunClip, &sunPosWS, &viewProj);

	D3DXVECTOR2 sunNDC(sunClip.x / sunClip.w, sunClip.y / sunClip.w);

	sunNDC = sunNDC * 0.5f + D3DXVECTOR2(0.5f, 0.5f);
	sunNDC.y = 1.0f - sunNDC.y;

	auto result = D3DXVECTOR4(sunNDC.x, sunNDC.y, 0, 0);
	result.z = Smoothstep(-0.2, 0.0, sunDir.z);

	return result;
}

void DrawGodRays()
{
	if (Game::State == 6 && g_Config.GodRays)
	{
		auto sunPos = GetSunScreenUV();
		if (sunPos.z == 0)
		{
			return;
		}

		auto color = g_Weather.GetGodRaysColor();
		if (color.x == 0 && color.y == 0 && color.z == 0)
		{
			return;
		}

		Game::Device->SetRenderTarget(0, Game::FilterSurface0);
		Game::Device->SetDepthStencilSurface(0);

		Game::Device->StretchRect(Game::BackBuffer, nullptr, Game::FilterSurface1, nullptr, D3DTEXF_NONE);

		auto effect = eEffect::Get(shader_type::ScreenFilterShader);
		auto pEffect = effect->D3DEffect;
		effect->SetTechniqueByName("GodRays");

		UINT passes = 0;
		Game::Device->SetVertexDeclaration(effect->VertexDecl);
		pEffect->Begin(&passes, 0);
		pEffect->BeginPass(0);

		effect->SetVector(shader_param::LIGHTDIR, &sunPos);
		effect->SetVector(ShaderParam::cvGodRaysColor, &color);
		effect->SetTexture(shader_param::HeightMapTexture, DepthTexture);
		effect->DrawFullScreenQuad(Game::FilterTexture1);

		pEffect->EndPass();
		pEffect->End();

		Game::Device->StretchRect(Game::FilterSurface0, nullptr, Game::BackBuffer, nullptr, D3DTEXF_NONE);
	}
}