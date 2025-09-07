#pragma once
#include <algorithm>

float GetScreenSmooth(const D3DXVECTOR2& sunUV)
{
	float left = sunUV.x;
	float right = 1.0f - sunUV.x;
	float top = sunUV.y;
	float bottom = 1.0f - sunUV.y;

	float fadeSize = 0.3f;

	float minDist = min(min(left, right), min(top, bottom));
	float fade = std::clamp(minDist / fadeSize, 0.0f, 1.0f);

	fade = fade * fade * (3.0f - 2.0f * fade);
	return fade;
}

D3DXVECTOR4 GetSunScreenUV()
{
	auto camPos = GetCameraPos();
	auto sunDir = D3DXVECTOR3(TimeOfDay::Instance->SunDirection);

	D3DXVECTOR3 sunPosWS = camPos + sunDir * 10000.0f;
	if (eView::Player->GetVisibleState(&sunPosWS) == visible_state::outside)
	{
		return D3DXVECTOR4(0, 0, 0, 0);
	}

	D3DXVECTOR4 sunClip;
	auto viewProj = eView::Player->Pinfo->non_jittered_view_projection_matrix;
	D3DXVec3Transform(&sunClip, &sunPosWS, &viewProj);

	D3DXVECTOR2 sunNDC(sunClip.x / sunClip.w, sunClip.y / sunClip.w);

	sunNDC = sunNDC * 0.5f + D3DXVECTOR2(0.5f, 0.5f);
	sunNDC.y = 1.0f - sunNDC.y;

	auto result = D3DXVECTOR4(sunNDC.x, sunNDC.y, 0, 0);
	result.z = Smoothstep(-0.2, 0.0, sunDir.z) * g_Weather.IsDay() * GetScreenSmooth(sunNDC);

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

		IDirect3DSurface9* rtBackup;
		IDirect3DSurface9* ppZStencilSurface;
		Game::Device->GetRenderTarget(0, &rtBackup);
		Game::Device->GetDepthStencilSurface(&ppZStencilSurface);

		Game::Device->SetRenderTarget(0, Game::FilterSurface0);
		Game::Device->SetDepthStencilSurface(0);

		Game::Device->StretchRect(Game::BackBuffer, nullptr, Game::FilterSurface1, nullptr, D3DTEXF_NONE);

		auto effect = eEffect::Get(shader_type::ScreenFilterShader);
		auto pEffect = effect->D3DEffect;

		UINT passes = 0;
		Game::Device->SetVertexDeclaration(effect->VertexDecl);
		pEffect->Begin(&passes, 0);
		pEffect->BeginPass(1);

		effect->SetVector(shader_param::LIGHTDIR, &sunPos);
		effect->SetTexture(shader_param::HeightMapTexture, DepthTexture);
		effect->DrawFullScreenQuad(Game::FilterTexture1);

		pEffect->EndPass();
		pEffect->End();

		Game::Device->StretchRect(Game::FilterSurface0, nullptr, Game::BackBuffer, nullptr, D3DTEXF_NONE);

		Game::Device->SetRenderTarget(0, rtBackup);
		Game::Device->SetDepthStencilSurface(ppZStencilSurface);

	}
}