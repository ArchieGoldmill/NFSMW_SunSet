#pragma once
#include "UIWeather.h"
#include "UITextures.h"
#include "UISpotlights.h"
#include "UICustomMeshes.h"
#include "UIConfig.h"
#include "UIAbout.h"

namespace UI
{
	int CurrentTab = 0;

	void Draw()
	{
		int width = 820;

		ImVec2 windowSize(width * g_Config.EditorScale, 1000 * g_Config.EditorScale);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSizeConstraints(ImVec2(width * g_Config.EditorScale, 400 * g_Config.EditorScale), ImVec2(width * g_Config.EditorScale, FLT_MAX));

		if (ImGui::Begin("Sun Set Editor", nullptr))
		{
			ImVec2 buttonSize = { 127, 30 };

			if (SelectableButton("Weather", buttonSize, CurrentTab == 0))
			{
				CurrentTab = 0;
			}

			ImGui::SameLine();
			if (SelectableButton("Textures", buttonSize, CurrentTab == 1))
			{
				CurrentTab = 1;
			}

			ImGui::SameLine();
			if (SelectableButton("Spotlights", buttonSize, CurrentTab == 2))
			{
				CurrentTab = 2;
			}

			ImGui::SameLine();
			if (SelectableButton("Meshes", buttonSize, CurrentTab == 3))
			{
				CurrentTab = 3;
			}

			ImGui::SameLine();
			if (SelectableButton("Config", buttonSize, CurrentTab == 4))
			{
				CurrentTab = 4;
			}

			ImGui::SameLine();
			if (SelectableButton("About", buttonSize, CurrentTab == 5))
			{
				CurrentTab = 5;
			}

			if (CurrentTab != 5)
			{
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
			}

			ImGui::Text("");

			switch (CurrentTab)
			{
			case 0: Weather::Draw(); break;
			case 1: Textures::Draw(); break;
			case 2: Spotlights::Draw(); break;
			case 3: Meshes::Draw(); break;
			case 4: Cfg::Draw(); break;
			case 5: About::Draw(); break;
			}
		}
		ImGui::End();
	}
}