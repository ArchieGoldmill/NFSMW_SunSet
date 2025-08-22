#pragma once

void SetVinylScale(RenderModel* renderModel)
{
	auto effect = renderModel->Effect;

	if (g_Config.CarVinylPaintFix)
	{
		float vinyl = strstr(renderModel->DiffuseTextureInfo->DebugName, "DUMMY_SKIN") != NULL ? 1 : 0;
		effect->SetFloat(ShaderParam::cfVinylScale, vinyl);
	}
	else
	{
		effect->SetFloat(ShaderParam::cfVinylScale, 0);
	}
}

D3DCOLOR GetBlendColourHook(D3DCOLOR* colors, float* coefs, int count, int blendAlpha)
{
	D3DCOLOR result = Game::GetBlendColour(colors, coefs, count, blendAlpha);

	auto bytes = (BYTE*)&result;
	bytes[3] = coefs[0] * 255;

	return result;
}

void __declspec(naked) SkinColorResetHook()
{
	static constexpr auto cExit = 0x0073B146;

	__asm
	{
		mov eax, [esp + 0x58];
		and eax, 0xFFFFFF;
		sub ecx, ebp;

		jmp cExit;
	}
}

void InitCarVinylMask()
{
	if (g_Config.CarVinylPaintFix)
	{
		injector::MakeCALL(0x0073B2B2, GetBlendColourHook);

		injector::MakeJMP(0x0073B140, SkinColorResetHook);

		injector::MakeNOP(0x0073B234, 6);
	}
}