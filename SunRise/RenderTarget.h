#pragma once
#include <d3dx9.h>
#include <cstdint>

struct RenderTarget
{
	int Id;
	int ViewId;
	IDirect3DSurface9* d3d_target;
	IDirect3DSurface9* d3d_depth_stencil;
	bool active;
	__declspec(align(0x04)) std::uint32_t resolution_x;
	__declspec(align(0x04)) std::uint32_t resolution_y;

	static inline RenderTarget*& Current = *reinterpret_cast<RenderTarget**>(0x00982A20);
};