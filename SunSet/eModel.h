#pragma once
#include "Utilities.h"
#include "eSolid.h"

struct eModel
{
	eModel* Next = NULL;
	eModel* Prev = NULL;
	Hash NameHash = 0;
	eSolid* pSolid = NULL;
	void* pReplacementTextureTable = NULL;
	int NumReplacementTextures = 0;

	D3DXMATRIX* GetPivot()
	{
		FUNC(0x004FB7D0, D3DXMATRIX*, __cdecl, _GetPivot, eModel*);
		return _GetPivot(this);
	}
};