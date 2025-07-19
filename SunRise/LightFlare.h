#pragma once
#include <d3dx9.h>

struct LightFlare
{
	int Next;
	int Prev;
	int NameHash;
	int Tint;
	D3DXVECTOR3 Postion;
	float ReflectPosZ;
	D3DXVECTOR3 Direction;
	char Type;
	char Flags;
};
