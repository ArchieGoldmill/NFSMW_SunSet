#pragma once

void CustomMeshesRenderHook()
{
	__asm pushad;

	for (auto& customMesh : CustomMeshes)
	{
		customMesh.Model.pSolid = NULL;
	}

	for (auto& customMesh : CustomMeshes)
	{
		if (!customMesh.Model.pSolid)
		{
			customMesh.Model.pSolid = Game::eFindSolid(customMesh.Model.NameHash);
		}

		if (customMesh.Model.pSolid)
		{
			eView::Player->Render(&customMesh.Model, &customMesh.Matrix, NULL, 0, NULL);
		}
	}

	__asm popad;
}

void __cdecl CustomMeshesShadowRenderHook(eView* view, int a)
{
	WorldModel::RenderAll(view, a);

	for (auto& customMesh : CustomMeshes)
	{
		if (customMesh.Model.pSolid)
		{
			view->Render(&customMesh.Model, &customMesh.Matrix, NULL, 0, NULL);
		}
	}
}

void InitCustomMeshes()
{
	injector::MakeCALL(0x006DF22A, CustomMeshesRenderHook);
	injector::MakeCALL(0x006E5110, CustomMeshesShadowRenderHook);
}