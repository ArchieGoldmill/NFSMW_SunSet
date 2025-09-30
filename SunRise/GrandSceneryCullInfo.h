#pragma once
#include "Utilities.h"
#include "eModel.h"
#include "eView.h"

struct Packed3x3
{
	__int16 m11;
	__int16 m12;
	__int16 m13;
	__int16 m21;
	__int16 m22;
	__int16 m23;
	__int16 m31;
	__int16 m32;
	__int16 m33;
};

struct SceneryInstance
{
	D3DXVECTOR3 bbox_min;
	D3DXVECTOR3 bbox_max;
	uint32_t flags;
	std::int16_t preculler_info_index;
	std::int16_t lighting_context_number;
	D3DXVECTOR3 position;
	Packed3x3 rotation;
	__int16 SceneryInfoNumber;

	void SetRotation(D3DXMATRIX* m)
	{
		FUNC(0x006BEEB0, D3DXMATRIX*, __thiscall, _SetRotation, SceneryInstance*, D3DXMATRIX*);
		_SetRotation(this, m);
	}

	void SetPosition(D3DXMATRIX* m)
	{
		FUNC(0x006BEE80, D3DXMATRIX*, __thiscall, _SetRotation, SceneryInstance*, float*);
		_SetRotation(this, &m->_41);
	}
};

struct SceneryInfo
{
	std::uint8_t debug_name[0x18];
	std::uint32_t solid_keys[4];
	eModel* models[4];
	float radius;
	std::uint32_t flags;
	std::uint32_t hierarchy_key;
	void* hierarchy;
};

struct SceneryDrawInfo
{
	int pModel;
	D3DXMATRIX* Matrix;
	SceneryInstance* pScenery;
};

struct SceneryCullInfo
{
	int field_0;
	int field_4;
	int field_8;
	int field_C;
	int field_10;
	int field_14;
	int field_18;
	int field_1C;
	int field_20;
	int field_24;
	int field_28;
	int field_2C;
	int field_30;
	int field_34;
	int field_38;
	int field_3C;
	int field_40;
	int field_44;
	int field_48;
	int field_4C;
	int field_50;
	int field_54;
	int field_58;
	int field_5C;
	int field_60;
	int field_64;
	int field_68;
	int field_6C;
	int field_70;
	int field_74;
	int field_78;
	int field_7C;
	eView* View;
	uint32_t Flags;
	SceneryDrawInfo* FirstDrawInfo;
	SceneryDrawInfo* CurrentDrawInfo;
	SceneryDrawInfo* TopDrawInfo;
	int field_94;
	int field_98;
	int field_9C;
	int field_A0;
	int field_A4;
	int field_A8;
	int field_AC;
	int field_B0;
	int field_B4;
	int field_B8;
	int field_BC;
	int field_C0;
	int field_C4;
	int field_C8;
	int field_CC;
};

struct GrandSceneryCullInfo
{
	SceneryCullInfo CullInfos[12];
	int CullInfoCount;
	SceneryDrawInfo* FirstDrawInfo;
	SceneryDrawInfo* CurrentDrawInfo;
	SceneryDrawInfo* TopDrawInfo;

	void SetuWorldCulling()
	{
		FUNC(0x006BFDD0, void, __cdecl, _SetuWorldCulling, GrandSceneryCullInfo*);
		_SetuWorldCulling(this);
	}

	void StuffScenery(eView* view, int flags)
	{
		FUNC(0x00723FA0, void, __thiscall, _SetuWorldCulling, GrandSceneryCullInfo*, eView*, int);
		_SetuWorldCulling(this, view, flags);
	}
};

struct SceneryPack
{
	SceneryPack* Next;
	SceneryPack* Prev;
	std::uint32_t chunks_loaded;
	std::uint16_t section_number;
	std::uint32_t polygon_in_memory_count;
	std::uint32_t polygon_in_world_count;
	SceneryInfo* infos;
	int infoCount;
	SceneryInstance* instances;
	int instCount;

	void DrawScenery(SceneryCullInfo* cullInfo)
	{
		FUNC(0x00729640, D3DXMATRIX*, __thiscall, _DrawScenery, SceneryPack*, SceneryCullInfo*);
		_DrawScenery(this, cullInfo);
	}
};

ASSERT_SIZE(GrandSceneryCullInfo, 0x9D0);
ASSERT_SIZE(SceneryInstance, 0x40);
ASSERT_SIZE(SceneryInfo, 0x48);