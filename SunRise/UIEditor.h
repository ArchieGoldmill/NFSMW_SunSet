#pragma once
#include "UIWeather.h"

namespace UI
{
	void Draw()
	{
		ImVec2 windowSize(700, 1000);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Sun Set Editor", nullptr, ImGuiWindowFlags_NoResize))
		{
			Game::EnableInput = !ImGui::IsWindowFocused();

			if (ImGui::Button("Weather", { 200, 30 }))
			{
			}

			ImGui::SameLine();
			if (ImGui::Button("Textures", { 200, 30 }))
			{
			}

			ImGui::SameLine();
			if (ImGui::Button("Spotlights", { 200, 30 }))
			{
			}

			ImGui::Text("");

			if (ImGui::Button("Save"))
			{

			}

			ImGui::SameLine();
			if (ImGui::Button("Reset"))
			{
				CurrentWeather = NULL;
				LoadWeatherConfig();
			}

			DrawWeather();
		}
		ImGui::End();
	}
}