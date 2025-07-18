#pragma once

struct eSolid
{
	void* Pinfo;
	eSolid* Next;
	eSolid* Prev;
	int field_C;
	int field_10;
	int field_14;
	int field_18;
	int field_1C;
	int field_20;
	int field_24;
	int field_28;
	int textures;
	int field_30;
	int field_34;
	int field_38;
	int material_table;
	int pad[24];
	char name[0x40];
};

ASSERT_SIZE(eSolid, 0xE0);