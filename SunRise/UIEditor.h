#pragma once
#include "UIWeather.h"
#include "UITextures.h"

namespace UI
{
	int CurrentTab = 0;

	void Draw()
	{
		ImVec2 windowSize(700, 1000);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

		if (ImGui::Begin("Sun Set Editor", nullptr, ImGuiWindowFlags_NoResize))
		{
			*Game::EnableInput = !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);

			if (SelectableButton("Weather", { 200, 30 }, CurrentTab == 0))
			{
				CurrentTab = 0;
			}

			ImGui::SameLine();
			if (SelectableButton("Textures", { 200, 30 }, CurrentTab == 1))
			{
				CurrentTab = 1;
			}

			//ImGui::SameLine();
			//if (SelectableButton("Spotlights", { 200, 30 }, CurrentTab == 2))
			//{
			//	CurrentTab = 2;
			//}

			ImGui::Text("");

			if (ImGui::Button("Save", { 60, 20 }))
			{
				if (CurrentTab == 0)
				{
					SaveWeatherConfig();
				}

				if (CurrentTab == 1)
				{
					SaveTextureConfig();
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Reset", { 60, 20 }))
			{
				if (CurrentTab == 0)
				{
					CurrentWeather = NULL;
					LoadWeatherConfig();
				}

				if (CurrentTab == 1)
				{
					CurrentTexture = NULL;
					InitTextureConfig();
				}
			}

			if (CurrentTab == 0)
			{
				DrawWeather();
			}

			if (CurrentTab == 1)
			{
				DrawTextures();
			}
		}
		ImGui::End();
	}
}