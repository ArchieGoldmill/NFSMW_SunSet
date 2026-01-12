#pragma once
#include <unordered_map>
#include <vector>
#include <cmath>
#include <unordered_set>
#include "Hashes.h"
#include "Game.h"
#include "SpotLight.h"

struct Int3
{
	int x;
	int y;
	int z;

	Int3(int x, int y, int z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Hash GetHash()
	{
		Hash sx = x + 200;
		Hash sy = y + 200;
		Hash sz = z + 200;

		return sx * 73856093u ^ sy * 19349663u ^ sz * 83492791u;
	}
};

struct Cell
{
	static const int NumLights = 128;

	SpotLightModel* Lights[NumLights];
};

inline Int3 WorldToCell(const D3DXVECTOR3& pos)
{
	float s = g_Config.LightCellSize;
	return Int3(
		FastFloor(pos.x / s),
		FastFloor(pos.y / s),
		FastFloor(pos.z / s)
	);
}

struct CellBuffer
{
	FastHashTable<Cell, 1024, Hash> table;

	CellBuffer()
	{
		this->Clear();
	}

	void AssignSpotLightToGrid(SpotLightModel* model)
	{
		auto& light = model->Light;
		D3DXVECTOR3 min, max;
		light.GetBoundingBox(&min, &max);

		Int3 minCell = WorldToCell(min);
		Int3 maxCell = WorldToCell(max);

		for (int x = minCell.x; x <= maxCell.x; ++x)
		{
			for (int y = minCell.y; y <= maxCell.y; ++y)
			{
				for (int z = minCell.z; z <= maxCell.z; ++z)
				{
					this->Add(Int3(x, y, z), model);
				}
			}
		}
	}

	void Clear()
	{
		this->table.clear();
	}

	void Add(Int3 coords, SpotLightModel* light)
	{
		Hash cellHash = coords.GetHash();

		Cell* targetCell = this->table.find(cellHash, true);

		for (int i = 0; i < Cell::NumLights; i++)
		{
			if (targetCell->Lights[i] == nullptr)
			{
				targetCell->Lights[i] = light;
				break;
			}
		}
	}

	Cell* Get(Int3 cell)
	{
		Hash cellHash = cell.GetHash();
		return this->table.find(cellHash);
	}

	SpotLightModel* candidateLights[NUM_SPOTLIGHTS];
	int numCandidateLights = 0;
	void AddCandidateLight(SpotLightModel* candidate)
	{
		if (!candidate->Added)
		{
			if (numCandidateLights == NUM_SPOTLIGHTS && candidate->Source >= candidateLights[NUM_SPOTLIGHTS - 1]->Source)
			{
				return;
			}

			int insertIdx = 0;
			while (insertIdx < numCandidateLights && candidateLights[insertIdx]->Source < candidate->Source)
			{
				++insertIdx;
			}

			int end = min(numCandidateLights, NUM_SPOTLIGHTS - 1);
			for (int j = end; j > insertIdx; --j)
			{
				candidateLights[j] = candidateLights[j - 1];
			}

			if (insertIdx < NUM_SPOTLIGHTS)
			{
				candidateLights[insertIdx] = candidate;
			}

			if (numCandidateLights < NUM_SPOTLIGHTS)
			{
				numCandidateLights++;
			}

			candidate->Added = true;
		}
	}

	void GetLightsForMesh(const D3DXVECTOR3& meshCenter, const float meshRadius)
	{
		D3DXVECTOR3 min = meshCenter - D3DXVECTOR3(meshRadius, meshRadius, meshRadius);
		D3DXVECTOR3 max = meshCenter + D3DXVECTOR3(meshRadius, meshRadius, meshRadius);

		GetLightsForMeshBase(min, max);
	}

	void GetLightsForMesh(const D3DXVECTOR3& min, const D3DXVECTOR3& max)
	{
		GetLightsForMeshBase(min, max);
	}

	void GetLightsForMeshBase(const D3DXVECTOR3& min, const D3DXVECTOR3& max)
	{
		numCandidateLights = 0;

		Int3 minCell = WorldToCell(min);
		Int3 maxCell = WorldToCell(max);

		for (int x = minCell.x; x <= maxCell.x; x++)
		{
			for (int y = minCell.y; y <= maxCell.y; y++)
			{
				for (int z = minCell.z; z <= maxCell.z; z++)
				{
					auto cell = this->Get(Int3(x, y, z));
					if (cell)
					{
						for (int i = 0; i < Cell::NumLights; i++)
						{
							SpotLightModel* light = cell->Lights[i];
							if (!light)
							{
								break;
							}

							this->AddCandidateLight(light);
						}
					}
				}
			}
		}
	}
};

inline CellBuffer g_CellBuffer;