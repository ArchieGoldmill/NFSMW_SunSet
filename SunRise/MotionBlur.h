#pragma once

int RenderCarsParam1;
int RenderCarsParam2;
void __cdecl RenderCars(int a, int b)
{
	if (!Game::MotionBlurEnable)
	{
		Game::VehicleConn_RenderCars(a, b);
	}
	else
	{
		RenderCarsParam1 = a;
		RenderCarsParam2 = b;
	}
}

void DrawBlur()
{
	Game::DrawMotionBlur();
	Game::VehicleConn_RenderCars(RenderCarsParam1, RenderCarsParam2);
	Game::CommitRenderredModels();
}

void InitMotionBlur()
{
	injector::MakeCALL(0x006DEE3F, RenderCars);
	injector::MakeCALL(0x006DF1DC, DrawBlur);
}