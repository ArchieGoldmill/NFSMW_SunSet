#pragma once
#include "eEffect.h"

struct LightQuery
{
	D3DXVECTOR3 pos;
	float range;
	IDirect3DQuery9* query = nullptr;
	bool active = false;

	void Release()
	{
		if (this->query)
		{
			this->query->Release();
			this->query = nullptr;
		}
	}
};

struct LightQueries
{
	LightQuery lightQueries[200];

	LightQuery* Get(int i)
	{
		return lightQueries + i;
	}

	LightQuery* Get(D3DXVECTOR3 pos)
	{
		for (int i = 0; i < 200; i++)
		{
			auto lightQuery = Get(i);
			if (lightQuery->pos == pos)
			{
				return lightQuery;
			}
		}

		return nullptr;
	}

	LightQuery* GetNew()
	{
		for (int i = 0; i < 200; i++)
		{
			auto lightQuery = Get(i);
			if (lightQuery->pos == D3DXVECTOR3(0, 0, 0))
			{
				return lightQuery;
			}
		}

		return nullptr;
	}

	void Init()
	{
		for (auto& query : lightQueries)
		{
			Game::Device->CreateQuery(D3DQUERYTYPE_OCCLUSION, &query.query);
		}
	}

	void Release()
	{
		for (int i = 0; i < 200; i++)
		{
			lightQueries[i].Release();
		}
	}
};

class QueryCuller
{
private:
	IDirect3DVertexBuffer9* g_pVB = nullptr;
	IDirect3DIndexBuffer9* g_pIB = nullptr;

	LightQueries lightQueries;

public:
	void Clear()
	{
	}

	void Submit(D3DXVECTOR3 pos, float range)
	{
		auto lq = lightQueries.Get(pos);
		if (!lq)
		{
			auto lq = lightQueries.GetNew();

			lq->pos = pos;
			lq->range = range;
			lq->active = false;
		}
	}

	void Render()
	{
		this->InitLightQuery();

		Game::Device->SetDepthStencilSurface(Game::DepthStencilSurface);

		Game::Device->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
		Game::Device->SetRenderState(D3DRS_ZENABLE, TRUE);
		Game::Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		Game::Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

		auto view = eView::Player;
		auto view_proj = view->Pinfo->view_projection_matrix;

		auto effect = eEffect::Get(shader_type::ScreenFilterShader);
		auto pEffect = effect->D3DEffect;

		effect->SetTechniqueByName("LightQuery");
		effect->SetInt(shader_param::CULL_MODE, D3DCULL_NONE);

		UINT numPasses = 0;
		pEffect->Begin(&numPasses, 0);
		pEffect->BeginPass(0);

		Game::Device->SetFVF(D3DFVF_XYZ);
		Game::Device->SetStreamSource(0, g_pVB, 0, sizeof(D3DXVECTOR3));
		Game::Device->SetIndices(g_pIB);

		for (int i = 0; i < 200; i++)
		{
			auto lightQuery = lightQueries.Get(i);
			if (lightQuery->active || lightQuery->pos == D3DXVECTOR3(0, 0, 0))
			{
				continue;
			}

			lightQuery->query->Issue(D3DISSUE_BEGIN);

			D3DXMATRIX scaleMat, transMat, worldMat;

			D3DXVECTOR3 scale(1, 1, 1);
			scale *= lightQuery->range;

			D3DXVECTOR3 position = lightQuery->pos;

			D3DXMatrixScaling(&scaleMat, scale.x, scale.y, scale.z);

			D3DXMatrixTranslation(&transMat, position.x, position.y, position.z);

			worldMat = scaleMat * transMat;

			D3DXMATRIX worldViewProj = worldMat * view_proj;

			effect->SetMatrix(shader_param::WORLDVIEWPROJECTION, &worldViewProj);
			Game::Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

			lightQuery->query->Issue(D3DISSUE_END);

			lightQuery->active = true;
		}

		pEffect->EndPass();
		pEffect->End();

		Game::Device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
	}

	bool IsVisible(D3DXVECTOR3 pos)
	{
		auto lightQuery = lightQueries.Get(pos);
		if (lightQuery)
		{
			DWORD numPixels = 0;
			while (true)
			{
				auto result = lightQuery->query->GetData(&numPixels, sizeof(DWORD), D3DGETDATA_FLUSH);
				if(result == S_OK)
				{
					break;
				}
			}

			lightQuery->active = false;
			lightQuery->pos = D3DXVECTOR3(0, 0, 0);
			return numPixels > 0;
		}

		return true;
	}

	void Release()
	{
		if (g_pVB)
		{
			g_pVB->Release();
			g_pVB = nullptr;
		}

		if (g_pIB)
		{
			g_pIB->Release();
			g_pIB = nullptr;
		}

		this->lightQueries.Release();
	}

private:
	void InitLightQuery()
	{
		if (!this->g_pVB)
		{
			this->lightQueries.Init();
			this->CreateCube();
		}
	}

	void CreateCube()
	{
		D3DXVECTOR3 verts[] =
		{
			{-1,-1,-1}, {1,-1,-1}, {1,1,-1}, {-1,1,-1},
			{-1,-1, 1}, {1,-1, 1}, {1,1, 1}, {-1,1, 1}
		};

		WORD indices[] =
		{
			0,1,2, 0,2,3,
			4,6,5, 4,3,6,
			0,3,4, 4,3,7,
			1,5,2, 5,6,2,
			0,4,1, 1,4,5,
			3,2,7, 7,2,6
		};

		// VB
		Game::Device->CreateVertexBuffer(sizeof(verts), 0, D3DFVF_XYZ, D3DPOOL_MANAGED, &g_pVB, nullptr);
		void* vb;
		g_pVB->Lock(0, 0, &vb, 0);
		memcpy(vb, verts, sizeof(verts));
		g_pVB->Unlock();

		// IB
		Game::Device->CreateIndexBuffer(sizeof(indices), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &g_pIB, nullptr);
		void* ib;
		g_pIB->Lock(0, 0, &ib, 0);
		memcpy(ib, indices, sizeof(indices));
		g_pIB->Unlock();
	}
};

inline QueryCuller g_QueryCuller;