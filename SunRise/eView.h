#pragma once
#include <d3dx9.h>
#include "ViewId.h"
#include "eModel.h"

enum class visible_state : std::uint8_t
{
	outside,
	partial,
	inside,
};

enum class clipping_plane_type : std::uint32_t
{
	left,
	right,
	bottom,
	top,
	near_,
	far_,
	count,
};

struct plane
{
	D3DXVECTOR3 normal;
	float distance;
};

struct clipping_planes
{
	plane planes[static_cast<std::uint32_t>(clipping_plane_type::count)];
};

struct eViewPlatInfo
{
	D3DXMATRIX view_matrix;
	D3DXMATRIX projection_matrix;
	D3DXMATRIX view_projection_matrix;
	D3DXMATRIX non_jittered_projection_matrix;
	D3DXMATRIX non_jittered_view_projection_matrix;
	clipping_planes clippingPlanes;
};

struct eView
{
	eViewPlatInfo* Pinfo;
	ViewId Id;

	static inline auto Views = (eView*)0x009195E0;
	static inline auto Player = (eView*)0x00919650;

	visible_state GetVisibleState(D3DXVECTOR3* min, D3DXVECTOR3* max)
	{
		FUNC(0x006CF2B0, visible_state, __thiscall, _GetVisibleState, eView*, D3DXVECTOR3*, D3DXVECTOR3*, void*);
		return _GetVisibleState(this, min, max, 0);
	}

	visible_state GetVisibleState(D3DXVECTOR3* point)
	{
		FUNC(0x006BE8F0, visible_state, __thiscall, _GetVisibleState, eView*, D3DXVECTOR3*, int);
		return _GetVisibleState(this, point, 0);
	}

	void Render(eModel* model, D3DXMATRIX* matrix, void* lightContext, int flags, void* blendMatricies)
	{
		FUNC(0x006DA9B0, void, __thiscall, _Render, eView*, eModel*, D3DXMATRIX*, void*, int, void*);
		_Render(this, model, matrix, lightContext, flags, blendMatricies);
	}
};