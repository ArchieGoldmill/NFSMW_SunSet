#pragma once
#include <d3dx9.h>
#include "Node.h"
#include "WorldModel.h"

struct SimConnection
{
	D3DXVECTOR4 pad;
};

struct SmackableRenderConn : SimConnection, bTNode<SmackableRenderConn>
{
	inline static auto& List = *(bTNode<SmackableRenderConn>*)(0x009B2A10);

	int field_18;
	ModelHeirarchy* heirarchy;
	int field_20;
	int pMatrix;
	int field_28;
	int field_2C;
	WorldModel* pWorldModel;
	int heirarchyIndex;
	int field_38;
	int field_3C;
	D3DXVECTOR4 field_40;
	D3DXMATRIX Matrix;
};
