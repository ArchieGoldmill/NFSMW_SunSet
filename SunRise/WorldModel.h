#pragma once
#include "Node.h"
#include "eModel.h"

struct SpaceNode
{
	int vTable;
	int Next;
	int Prev;
	int field_C;
	int field_10;
	int field_14;
	int field_18;
	int field_1C;
	D3DXMATRIX Matrix1;
	D3DXMATRIX Matrix2;
};

struct ModelHeirarchyNode
{
	Hash NameHash;
	eModel* pModel;
	int Flags;
	Hash Parent;
};

struct ModelHeirarchy
{
	Hash NameHash;
	int NumNodes;
	int Flags;
	ModelHeirarchyNode Node;
};

struct WorldModel : bTNode<WorldModel>
{
	inline static auto& List = *(bTNode<WorldModel>*)(0x009B0F90);

	eModel* pModel;
	int field_C;
	ModelHeirarchy* Heirarchy;
	int HeirarchyIndex;
	int field_18;
	int field_1C;
	int field_20;
	int field_24;
	int field_28;
	int field_2C;
	SpaceNode* pSpaceNode;
	int field_34;
	int field_38;
	int field_3C;
	D3DXMATRIX Matrix;
	int field_80;
	int field_84;
	int field_88;
	int field_8C;
};