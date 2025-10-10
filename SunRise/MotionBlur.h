#pragma once
#include "UIWidgetMenu.h"
#include "PostEffects.h"
#include "DepthPrePass.h"

void __cdecl RenderCars(eView* view, int b)
{
	if (!Game::MotionBlurEnable)
	{
		Game::VehicleConn_RenderCars(view, b);
	}
}

void DrawBlur()
{
	if (Game::MotionBlurEnable)
	{
		Game::DrawMotionBlur();
		Game::VehicleConn_RenderCars(eView::Player, 0);
		Game::CommitRenderedModels();
	}

	DrawPostEffects();
}

void __stdcall CopyBufferForBlur(IDirect3DDevice9* device, IDirect3DSurface9* backBuffer, RECT* pSourceRect, IDirect3DSurface9* filterSurface0, RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
	if (Game::State == 3)
	{
		device->StretchRect(backBuffer, pSourceRect, filterSurface0, pDestRect, Filter);
		return;
	}

	// Copy back buffer to filter 1
	Game::Device->StretchRect(backBuffer, nullptr, Game::FilterSurface1, nullptr, D3DTEXF_NONE);

	// Set filter 0 as target
	Game::Device->SetRenderTarget(0, filterSurface0);
	Game::Device->SetDepthStencilSurface(0);

	auto effect = eEffect::Get(shader_type::ScreenFilterShader);
	auto pEffect = effect->D3DEffect;

	Game::Device->SetVertexDeclaration(effect->VertexDecl);
	effect->SetTechniqueByName("Depth");

	UINT passes = 0;
	pEffect->Begin(&passes, 0);
	pEffect->BeginPass(0);

	// Set depth texture
	effect->SetTexture(shader_param::HeightMapTexture, DepthTexture);
	// Set filter 1 texture that holds back buffer
	effect->DrawFullScreenQuad(Game::FilterTexture1);

	pEffect->EndPass();
	pEffect->End();
}

unsigned int BlurToggleWidgetVT[15];
void __fastcall AddToggleOptionHook(UIWidgetMenu* widgetMenu, int, FEToggleWidget* toggleWidget, bool a4)
{
	widgetMenu->AddToggleOption(toggleWidget, a4);

	toggleWidget = (FEToggleWidget*)Game::malloc(0x5C);
	toggleWidget->Ctor(true);
	toggleWidget->vTable = BlurToggleWidgetVT;
	widgetMenu->AddToggleOption(toggleWidget, a4);
}

void __declspec(naked) DisableBlurResetHook()
{
	static constexpr auto cExit1 = 0x00470288;
	static constexpr auto cExit2 = 0x0047069E;

	__asm
	{
		mov eax, 0x0091CAE4;
		mov eax, [eax];
		test eax, eax;
		je updateEnabled;
		jmp cExit2;

	updateEnabled:
		fld dword ptr ds : [0x0089096C] ;
		jmp cExit1;
	}
}

void InitMotionBlur()
{
	injector::MakeCALL(0x006DEE3F, RenderCars);
	injector::MakeCALL(0x006DF1DC, DrawBlur);

	injector::WriteMemory(0x008F9B10, g_Config.BlurMinSpeed);
	injector::WriteMemory(0x008F9B14, g_Config.BlurMaxSpeed);

	// Disable blur check
	injector::MakeNOP(0x006DF1D2, 2);

	// Remove blur lerp
	injector::MakeNOP(0x006DBE28, 2);

	// Remove second sky stuff
	injector::MakeNOP(0x006DF449, 5);

	if (g_Config.DepthPrepass)
	{
		injector::MakeNOP(0x006DBD7C, 6);
		injector::MakeCALL(0x006DBD7C, CopyBufferForBlur);

		injector::WriteMemory<BYTE>(0x006DBE9E, D3DBLEND_INVSRCALPHA);
		injector::WriteMemory<BYTE>(0x006DBEA0, D3DBLEND_SRCALPHA);
	}

	// Swap rain to blur
	injector::MakeCALL(0x00529B29, AddToggleOptionHook);
	for (int i = 0; i < 15; i++)
	{
		BlurToggleWidgetVT[i] = ((unsigned int*)0x0089BDB0)[i];
	}

	BlurToggleWidgetVT[1] = 0x0050FC00;
	BlurToggleWidgetVT[3] = 0x0051BB80;

	injector::WriteMemory<BYTE>(0x0050FC20, 0x2C);
	injector::WriteMemory<BYTE>(0x0050FC2B, 0x2C);
	injector::WriteMemory<BYTE>(0x0051BBC1, 0x2C);
	injector::WriteMemory<Hash>(0x0051BB95, 0x1C7D9D8D);

	// Disable blur reset
	injector::MakeJMP(0x00470282, DisableBlurResetHook);
}