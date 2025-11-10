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
		int sx = x + 200;
		int sy = y + 200;
		int sz = z + 200;

		return sx * 400 * 400 + sy * 400 + sz;
	}
};

struct Cell
{
	static const int NumLights = 128;

	Hash Index;
	SpotLightModel* Lights[NumLights];
};

inline Int3 WorldToCell(const D3DXVECTOR3& pos)
{
	float cellSize = g_Config.LightCellSize;
	return Int3(
		(int)std::floor(pos.x / cellSize),
		(int)std::floor(pos.y / cellSize),
		(int)std::floor(pos.z / cellSize)
	);
}

struct CellBuffer
{
	static const int BufferSize = 1024;

	int Count = 0;
	Cell Buffer[BufferSize];

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
		Count = 0;
		memset(Buffer, 0, sizeof(Buffer));
	}

	void Add(Int3 cell, SpotLightModel* light)
	{
		Cell* targetCell = nullptr;
		Hash cellHash = cell.GetHash();

		for (int i = 0; i < this->Count; i++)
		{
			if (Buffer[i].Index == cellHash)
			{
				targetCell = Buffer + i;
				break;
			}
		}

		if (targetCell == nullptr)
		{
			if (this->Count >= BufferSize)
			{
#ifdef _DEBUG
				throw std::runtime_error("Cell buffer is full");
#endif
				return;
			}

			targetCell = Buffer + this->Count;
			targetCell->Index = cellHash;
			this->Count++;
		}

		bool found = false;
		for (int i = 0; i < Cell::NumLights; i++)
		{
			if (targetCell->Lights[i] == nullptr)
			{
				targetCell->Lights[i] = light;
				found = true;
				break;
			}
		}

#ifdef _DEBUG
		if (!found)
		{
			throw std::runtime_error("Cell lights is full");
		}
#endif
	}

	void Sort()
	{
		std::sort(Buffer, Buffer + Count, [](const Cell& a, const Cell& b) { return a.Index < b.Index; });
	}

	Cell* Get(Int3 cell)
	{
		Hash target_hash = cell.GetHash();

		int left = 0;
		int right = Count - 1;
		while (left <= right)
		{
			int mid = left + (right - left) / 2;
			if (Buffer[mid].Index == target_hash)
			{
				return Buffer + mid;
			}

			else if (Buffer[mid].Index < target_hash)
			{
				left = mid + 1;
			}
			else {
				right = mid - 1;
			}
		}

		return NULL;
	}

	SpotLightModel* candidateLights[256];
	int numCandidateLights = 0;

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
							if (light && numCandidateLights < 256)
							{
								bool alreadyAdded = false;

								for (int j = 0; j < numCandidateLights; j++)
								{
									if (candidateLights[j] == light)
									{
										alreadyAdded = true;
										break;
									}
								}

								if (!alreadyAdded)
								{
									candidateLights[numCandidateLights++] = light;
								}
							}
							else
							{
								break;
							}
						}
					}
				}
			}
		}

		if (numCandidateLights > 0)
		{
			std::sort(candidateLights, candidateLights + numCandidateLights, [](SpotLightModel* a, SpotLightModel* b) { return (int)a->Source < (int)b->Source; });
		}
	}
};

inline CellBuffer g_CellBuffer;