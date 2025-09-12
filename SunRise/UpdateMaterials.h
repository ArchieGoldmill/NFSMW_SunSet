#pragma once

bool MaterialsUpdated = false;


bool IsPaintMaterial(eLightMaterial* mat)
{
	const char* PaintMaterials[] = { "WINDSHIELD", "WINDOWMASK", "METPAINT", "REGPAINT", "PEARL" };

	for (auto name : PaintMaterials)
	{
		if (strstr(mat->Name, name) != NULL)
		{
			return true;
		}
	}

	return false;
}

void UpdateMaterials()
{
	if (MaterialsUpdated)
	{
		return;
	}

	auto ptr = eLightMaterial::List.Next;
	while (true)
	{
		auto pMat = (eLightMaterial*)(((int*)ptr) - 1);

		if (IsPaintMaterial(pMat))
		{
			pMat->SpecularHotSpot = 1;
		}
		else
		{
			pMat->SpecularHotSpot = 0;
		}

		ptr = ptr->Next;
		if (ptr == &eLightMaterial::List)
		{
			break;
		}
	}

	MaterialsUpdated = true;
}