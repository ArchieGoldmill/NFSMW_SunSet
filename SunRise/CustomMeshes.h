#pragma once
#include "WorldModel.h"
#include "Config.h"

struct CustomMesh
{
	std::string Name;

	D3DXVECTOR3 Position = { 0, 0, 0 };
	D3DXVECTOR3 Rotation = { 0, 0, 0 };
	D3DXVECTOR3 Scale = { 1, 1, 1 };
	bool CastShadow;
	float Distance;

	bool PassDistCheck = false;

	D3DXMATRIX Matrix;

	eModel* Model = NULL;

	void SetMatrix()
	{
		D3DXMATRIX scale;
		D3DXMatrixScaling(&scale, Scale.x, Scale.y, Scale.z);

		D3DXMATRIX rot;
		D3DXMatrixRotationYawPitchRoll(&rot, D3DXToRadian(Rotation.x), D3DXToRadian(Rotation.y), D3DXToRadian(Rotation.z));

		D3DXMATRIX pos;
		D3DXMatrixTranslation(&pos, Position.x, Position.y, Position.z);

		Matrix = scale * rot * pos;
	}

	bool IsValid()
	{
		return this->Scale != D3DXVECTOR3(0, 0, 0) && this->Model && this->Model->pSolid && this->PassDistCheck;
	}
};

class CustomMesheContainer
{
private:
	std::vector<CustomMesh> list;
	std::vector<eModel*> models;

public:

	void Add(CustomMesh& mesh)
	{
		this->list.push_back(mesh);

		auto& lastMesh = this->list.back();

		auto nameHash = Game::bStringHash(lastMesh.Name.c_str());
		lastMesh.Model = this->GetModel(nameHash);
		if (!lastMesh.Model)
		{
			auto model = new eModel();
			model->NameHash = nameHash;
			this->models.push_back(model);

			lastMesh.Model = this->models.back();
		}
	}

	void Remove(int num)
	{
		auto mesh = this->list[num];

		bool found = false;
		for (int i = 0; i < this->list.size(); i++)
		{
			if (this->list[i].Model == mesh.Model && i != num)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			for (int i = 0; i < this->models.size(); i++)
			{
				if (this->models[i]->NameHash == mesh.Model->NameHash)
				{
					delete this->models[i];
					this->models.erase(this->models.begin() + i);
					break;
				}
			}
		}

		this->list.erase(this->list.begin() + num);
	}

	CustomMesh& Get(int num)
	{
		return this->list[num];
	}

	void Clear()
	{
		this->list.clear();
	}

	std::vector<CustomMesh>& GetList()
	{
		return this->list;
	}

	void Draw(eView* view)
	{
		for (auto& customMesh : this->list)
		{
			this->DrawCustomMesh(view, customMesh);
		}
	}

	void DrawPrepass(float defaultDist)
	{
		this->ResetSolids();

		for (auto& customMesh : this->list)
		{
			float maxDist = customMesh.Distance;
			if (maxDist <= 0)
			{
				maxDist = defaultDist;
			}
				
			float dist = GetCameraDistance(customMesh.Position);

			customMesh.PassDistCheck = false;
			if (dist < maxDist)
			{
				customMesh.PassDistCheck = true;

				if (!customMesh.Model->pSolid)
				{
					customMesh.Model->pSolid = Game::eFindSolid(customMesh.Model->NameHash);
				}

				DrawCustomMesh(eView::Player, customMesh);
			}
		}
	}

	int Size()
	{
		return this->list.size();
	}

private:

	void ResetSolids()
	{
		for (auto& model : this->models)
		{
			model->pSolid = NULL;
		}
	}

	eModel* GetModel(Hash name)
	{
		for (auto& model : this->models)
		{
			if (model->NameHash == name)
			{
				return model;
			}
		}

		return NULL;
	}

	void DrawCustomMesh(eView* view, CustomMesh& customMesh)
	{
		if (customMesh.IsValid())
		{
			if (view->Id == ViewId::ShadowMap && !customMesh.CastShadow)
			{
				return;
			}

			view->Render(customMesh.Model, &customMesh.Matrix, NULL, 0, NULL);
		}
	}
};

inline CustomMesheContainer CustomMeshes;

void __cdecl CustomMeshesShadowRenderHook(eView* view, int a)
{
	WorldModel::RenderAll(view, a);
	CustomMeshes.Draw(view);
}

void InitCustomMeshes()
{
	injector::MakeCALL(0x006E5110, CustomMeshesShadowRenderHook);
}