#pragma once
#include "Utilities.h"
#include "eSolid.h"

struct eModel
{
	eModel* Next;
	eModel* Prev;
	Hash NameHash;
	eSolid* pSolid;
};