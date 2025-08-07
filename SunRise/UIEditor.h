#pragma once
#include "UIWeather.h"
#include "UITextures.h"
#include "UISpotlights.h"

namespace UI
{
	int CurrentTab = 0;

	void Draw()
	{
		ImVec2 windowSize(800, 1000);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

		if (ImGui::Begin("Sun Set Editor", nullptr, ImGuiWindowFlags_NoResize))
		{
			if (SelectableButton("Weather", { 200, 30 }, CurrentTab == 0))
			{
				CurrentTab = 0;
			}

			ImGui::SameLine();
			if (SelectableButton("Textures", { 200, 30 }, CurrentTab == 1))
			{
				CurrentTab = 1;
			}

			ImGui::SameLine();
			if (SelectableButton("Spotlights", { 200, 30 }, CurrentTab == 2))
			{
				CurrentTab = 2;
			}

			ImGui::Text("");

			if (ImGui::Button("Save", { 60, 20 }))
			{
				switch (CurrentTab)
				{
				case 0: SaveWeatherConfig(); break;
				case 1: SaveTextureConfig(); break;
				case 2: SaveSpotLightConfig(); break;
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Reset", { 60, 20 }))
			{
				switch (CurrentTab)
				{
				case 0: 
					LoadWeatherConfig(); 
					CurrentWeather = NULL; 
					break;
				case 1: 
					InitTextureConfig(); 
					CurrentTexture = NULL;
					break;
				case 2: 
					LoadSpotLightConfig(); 
					CurrentSolid = NULL;
					CurrentLight = NULL;
					break;
				}
			}

			ImGui::Text("");

			switch (CurrentTab)
			{
			case 0: DrawWeather(); break;
			case 1: DrawTextures(); break;
			case 2: DrawSpotlights(); break;
			}
		}
		ImGui::End();
	}
}