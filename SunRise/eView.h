#pragma once
#include <d3dx9.h>

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

	visible_state get_visible_state_sb(D3DXVECTOR3 center, D3DXVECTOR3 extent)
	{
		std::uint32_t lod_increment = 1;

		const clipping_planes& clipping = this->clippingPlanes;

		for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(clipping_plane_type::count); ++i)
		{
			const plane& plane = clipping.planes[i];

			float ext_dot =
				::fabs(plane.normal.x) * extent.x +
				::fabs(plane.normal.y) * extent.y +
				::fabs(plane.normal.z) * extent.z;

			float cen_dot =
				plane.normal.x * center.x +
				plane.normal.y * center.y +
				plane.normal.z * center.z +
				plane.distance;

			if (cen_dot + ext_dot < 0.0f)
			{
				return visible_state::outside;
			}

			if (cen_dot - ext_dot < 0.0f)
			{
				lod_increment = 0u;
			}
		}

		return static_cast<visible_state>(lod_increment + 1u);
	}
};

struct eView
{
	eViewPlatInfo* Pinfo;

	static inline auto Views = (eView*)0x009195E0;
	static inline auto PlayerView = (eView*)0x00919650;
};