#pragma once
#include "GarageMainScreen.h"

void SetDepthRenderTarget()
{
	auto renderTarget = RenderTarget::Player;
	Game::Device->SetRenderTarget(0, DepthRenderTarget);
	Game::Device->SetDepthStencilSurface(renderTarget->d3d_depth_stencil);

	Game::Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

	D3DVIEWPORT9 viewport;
	viewport.Height = renderTarget->resolution_y;
	viewport.Width = renderTarget->resolution_x;
	viewport.X = 0;
	viewport.Y = 0;
	viewport.MinZ = 0.0;
	viewport.MaxZ = 1.0;
	Game::Device->SetViewport(&viewport);
}

void StuffFeScenery()
{
	if (Game::State == 3)
	{
		auto manager = GarageMainScreen::GetInstance();
		if (manager)
		{
			Game::EnableParticleSystem = false;
			manager->HandleRender();
			Game::EnableParticleSystem = true;
		}
	}
}

bool DepthPrePass = false;
void DoDepthPrePass(GrandSceneryCullInfo* cullInfo)
{
	InitPrepassTextures();
	SetDepthRenderTarget();

	DepthPrePass = true;

	Game::StuffSky(eView::Player, 1.0, 0);
	cullInfo->StuffScenery(eView::Player, 0x20);
	WorldModel::RenderAll(eView::Player);
	CustomMeshes.DrawPrepass(g_Config.CustomMeshDist);
	StuffFeScenery();

	Game::CommitRenderedModels();
	DepthPrePass = false;

	Game::Device->StretchRect(DepthRenderTarget, nullptr, DepthSurface, nullptr, D3DTEXF_NONE);

	eEffect::Get(shader_type::ParticlesShader)->SetTexture(shader_param::HeightMapTexture, DepthTexture);

	auto prelitEffect = eEffect::Get(shader_type::WorldPrelitShader);
	prelitEffect->SetBool(ShaderParam::cbCompressColor, true);
}

void __stdcall SetZWriteEnabledHook(TextureInfo* textureInfo, IDirect3DDevice9* device, D3DRENDERSTATETYPE state, DWORD value)
{
	if(textureInfo->NameHash == Hashes::ARROW_GPS)
	{
		device->SetRenderState(state, value);
		return;
	}

	if (RenderTarget::Current->ViewId == ViewId::Reflection && eEffect::Current->id == shader_type::skyshader)
	{
		device->SetRenderState(state, false);
	}
	else if (RenderTarget::Current->ViewId == ViewId::Player1 && eEffect::Current->id != shader_type::CarShader)
	{
		device->SetRenderState(state, DepthPrePass);
	}
	else
	{
		device->SetRenderState(state, value);
	}
}

void __declspec(naked) SetZWriteEnabledCave()
{
	static constexpr auto cExit = 0x006C6989;

	__asm
	{
		push esi;
		call SetZWriteEnabledHook;

		jmp cExit;
	}
}

void __cdecl SimpleAnimApplyHook(eModel* model, eSolid* solid, D3DXMATRIX* matrix)
{
	if (DepthPrePass || RenderTarget::Current->ViewId == ViewId::Reflection || RenderTarget::Current->ViewId == ViewId::ShadowMap)
	{
		Game::SimpleAnimApply(model, solid, matrix);
	}
}

void __cdecl SetTextureAlphaRefHook(unsigned int alphaTestEnable, unsigned int alphaTestRef)
{
	if (alphaTestEnable == 1 && DepthPrePass)
	{
		alphaTestRef += 10;
	}

	Game::BlendState[0] = alphaTestEnable;
	Game::BlendState[1] = alphaTestRef;
}

void InitDepthPrePass()
{
	injector::MakeNOP(0x006C6983, 6);
	injector::MakeJMP(0x006C6983, SetZWriteEnabledCave);

	injector::MakeCALL(0x006DAA2B, SimpleAnimApplyHook);

	injector::MakeCALL(0x006C6910, SetTextureAlphaRefHook);
}