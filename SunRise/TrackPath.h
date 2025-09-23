#pragma once

enum class ZoneType
{
	none = -1,
	reset = 0,
	reset_to_point,
	guided_reset,
	tunnel,
	overpass,
	overpass_small,
	streamer_prediction,
	garage,
	hidden,
	traffic_pattern,
	dynamic,
	neighborhood,
	jump_camera,
	no_cop_spawn,
	pursuit_start,
	highway,
	canyon_drop,
	vertigo_camera,
	count,
};

struct TrackPath
{
	ZoneType Type;
	D3DXVECTOR2 position;
	D3DXVECTOR2 direction;
	float elevation;
	char zone_source;
	char cached_index;
	std::int16_t visit_info;
	void* user_data;
	D3DXVECTOR2 bbox_min;
	D3DXVECTOR2 bbox_max;
};