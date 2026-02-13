#pragma once
#include "UIWeather.h"
#include "UITextures.h"
#include "UISpotlights.h"
#include "UICustomMeshes.h"
#include "UIConfig.h"

namespace UI
{
	int CurrentTab = 0;

	void Draw()
	{
		ImVec2 windowSize(800 * g_Config.EditorScale, 1000 * g_Config.EditorScale);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSizeConstraints(ImVec2(800 * g_Config.EditorScale, 400 * g_Config.EditorScale), ImVec2(800 * g_Config.EditorScale, FLT_MAX));

		if (ImGui::Begin("Sun Set Editor", nullptr))
		{
			if (SelectableButton("Weather", { 150, 30 }, CurrentTab == 0))
			{
				CurrentTab = 0;
			}

			ImGui::SameLine();
			if (SelectableButton("Textures", { 150, 30 }, CurrentTab == 1))
			{
				CurrentTab = 1;
			}

			ImGui::SameLine();
			if (SelectableButton("Spotlights", { 150, 30 }, CurrentTab == 2))
			{
				CurrentTab = 2;
			}

			ImGui::SameLine();
			if (SelectableButton("Meshes", { 150, 30 }, CurrentTab == 3))
			{
				CurrentTab = 3;
			}

			ImGui::SameLine();
			if (SelectableButton("Config", { 150, 30 }, CurrentTab == 4))
			{
				CurrentTab = 4;
			}

			ImGui::Text("");

			if (ImGui::Button("Save", GetSize({ 60, 20 })))
			{
				switch (CurrentTab)
				{
				case 0: SaveWeatherConfig(); break;
				case 1: SaveTextureConfig(); break;
				case 2: SaveSpotLightConfig(); break;
				case 3: SaveCustomMeshes(); break;
				case 4: SaveConfig(); break;
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Reset", GetSize({ 60, 20 })))
			{
				switch (CurrentTab)
				{
				case 0:
					LoadWeatherConfig();
					Weather::Reset();
					break;
				case 1:
					LoadTextureConfig();
					Textures::Reset();
					break;
				case 2:
					LoadSpotLightConfig();
					Spotlights::Reset();
					break;
				case 3:
					LoadCustomMeshes();
					Meshes::Reset();
					break;
				case 4:
					LoadConfig();
					break;
				}
			}

			ImGui::Text("");

			switch (CurrentTab)
			{
			case 0: Weather::Draw(); break;
			case 1: Textures::Draw(); break;
			case 2: Spotlights::Draw(); break;
			case 3: Meshes::Draw(); break;
			case 4: Cfg::Draw(); break;
			}
		}
		ImGui::End();
	}
}