#pragma once
#include <vector>
#include "Spotlight.h"

class SolidLightsContainer
{
private:
	bool sorted = true;
	std::vector<SolidLights> list;
	std::vector<SolidLights*> uilist;

public:

	void Add(SolidLights& s)
	{
		this->list.push_back(s);
		this->sorted = false;
	}

	void Sort()
	{
		std::sort(this->list.begin(), this->list.end(), [](const SolidLights& a, const SolidLights& b) { return a.LodA.hash < b.LodA.hash; });
		this->sorted = true;
	}

	void Clear()
	{
		this->list.clear();
		this->sorted = true;
	}

	SolidLights* Get(Hash targetHash)
	{
		if (!sorted)
		{
			throw "list is not sorted";
		}

		int left = 0;
		int right = static_cast<int>(list.size()) - 1;

		while (left <= right)
		{
			int mid = left + (right - left) / 2;
			SolidLights& midItem = list[mid];

			if (midItem.LodA.hash == targetHash)
			{
				return &midItem;
			}
			else if (midItem.LodA.hash < targetHash)
			{
				left = mid + 1;
			}
			else
			{
				right = mid - 1;
			}
		}

		return nullptr;
	}

	std::vector<SolidLights>& GetList()
	{
		return this->list;
	}

	std::vector<SolidLights*>& GetUiList()
	{
		return this->uilist;
	}

	void PopulateUiList()
	{
		uilist.clear();

		for (int i = 0; i < this->list.size(); i++)
		{
			this->uilist.push_back(&this->list[i]);
		}

		std::sort(uilist.begin(), uilist.end(), [](const SolidLights* a, const SolidLights* b) { return a->LodA.str < b->LodA.str; });
	}

	void Remove(Hash targetHash)
	{
		int idx = -1;
		for (int i = 0; i < list.size(); i++)
		{
			if (list[i].LodA.hash == targetHash)
			{
				idx = i;
				break;
			}
		}

		if (idx != -1)
		{
			list.erase(list.begin() + idx);
		}
	}
};

inline SolidLightsContainer SolidLightsList;