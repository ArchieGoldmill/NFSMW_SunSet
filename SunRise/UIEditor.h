#pragma once
#include "UIWeather.h"

namespace UI
{
	int CurrentTab = 0;

	void Draw()
	{
		ImVec2 windowSize(700, 1000);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);

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

			ImGui::SameLine();
			if (SelectableButton("Spotlights", { 200, 30 }, CurrentTab == 2))
			{
				CurrentTab = 2;
			}

			ImGui::Text("");

			if (ImGui::Button("Save"))
			{
				if (CurrentTab == 0)
				{
					SaveWeatherConfig();
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Reset"))
			{
				if (CurrentTab == 0)
				{
					CurrentWeather = NULL;
					LoadWeatherConfig();
				}
			}

			if (CurrentTab == 0)
			{
				DrawWeather();
			}
		}
		ImGui::End();
	}
}