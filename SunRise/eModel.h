#pragma once
#include "Utilities.h"
#include "eSolid.h"

struct eModel
{
	eModel* Next;
	eModel* Prev;
	Hash NameHash;
	eSolid* pSolid;

	D3DXMATRIX* GetPivot()
	{
		FUNC(0x004FB7D0, D3DXMATRIX*, __cdecl, _GetPivot, eModel*);
		return _GetPivot(this);
	}
};