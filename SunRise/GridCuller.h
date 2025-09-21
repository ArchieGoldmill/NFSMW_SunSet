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
	int x, y, z;

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

inline Int3 worldToCell(const D3DXVECTOR3& pos)
{
	float cellSize = g_Config.LightCellSize;
	return
	{
		static_cast<int>(std::floor(pos.x / cellSize)),
		static_cast<int>(std::floor(pos.y / cellSize)),
		static_cast<int>(std::floor(pos.z / cellSize))
	};
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
		auto light = &model->Light;

		float r = light->Range;
		D3DXVECTOR3 range = { r, r, r };

		D3DXVECTOR3 min = light->Position - range;

		if (light->IsLampPost())
		{
			range.z = 0;
		}

		D3DXVECTOR3 max = light->Position + range;

		Int3 minCell = worldToCell(min);
		Int3 maxCell = worldToCell(max);

		for (int x = minCell.x; x <= maxCell.x; ++x)
		{
			for (int y = minCell.y; y <= maxCell.y; ++y)
			{
				for (int z = minCell.z; z <= maxCell.z; ++z)
				{
					Int3 cell = { x, y, z };
					this->Add(cell, model);
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
	void ResetCandidateLigts()
	{
		memset(candidateLights, 0, sizeof(candidateLights));
		numCandidateLights = 0;
	}

	void GetLightsForMesh(const D3DXVECTOR3& meshCenter, float meshRadius)
	{
		D3DXVECTOR3 min = meshCenter - D3DXVECTOR3(meshRadius, meshRadius, meshRadius);
		D3DXVECTOR3 max = meshCenter + D3DXVECTOR3(meshRadius, meshRadius, meshRadius);

		ResetCandidateLigts();
		GetLightsForMeshBase(min, max);
	}

	void GetLightsForMesh(const D3DXVECTOR3& min, const D3DXVECTOR3& max)
	{
		ResetCandidateLigts();
		GetLightsForMeshBase(min, max);
	}

	void GetLightsForMeshBase(const D3DXVECTOR3& min, const D3DXVECTOR3& max)
	{
		Int3 minCell = worldToCell(min);
		Int3 maxCell = worldToCell(max);

		for (int x = minCell.x; x <= maxCell.x; x++)
		{
			for (int y = minCell.y; y <= maxCell.y; y++)
			{
				for (int z = minCell.z; z <= maxCell.z; z++)
				{
					auto cell = this->Get({ x, y, z });
					if (cell)
					{
						for (int i = 0; i < Cell::NumLights; i++)
						{
							SpotLightModel* light = cell->Lights[i];
							if (light && numCandidateLights < 256)
							{
								bool alreadyAdded = false;

								for (int i = 0; i < numCandidateLights; i++)
								{
									auto s = candidateLights[i];
									if (s == light)
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

		std::sort(candidateLights, candidateLights + numCandidateLights, [](SpotLightModel* a, SpotLightModel* b) { return (int)a->Source < (int)b->Source; });
	}
};

inline CellBuffer g_CellBuffer;