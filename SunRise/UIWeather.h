#pragma once
#include "imgui/imgui.h"
#include "WeatherConfig.h"
#include "UICommon.h"
#include "TimeOfDay.h"

namespace UI
{
	namespace Weather
	{
		WeatherConfig* CurrentWeather = NULL;
		int WeatherIndex = -1;
		bool isRain = false;
		WeatherData CopyData;

		void Reset()
		{
			CurrentWeather = NULL;
			WeatherIndex = -1;
		}

		void Draw()
		{
			ImGui::SliderFloat("Time", &TimeOfDay::Instance->CurrentTime, 0.0f, 1.0f);
			ImGui::SameLine();
			InputFloat("Update Rate", &TimeOfDay::Instance->UpdateRate);

			if (ImGui::BeginTable("WeatherEditorTable", 2, ImGuiTableFlags_BordersInnerV))
			{
				ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 120.0f);
				ImGui::TableSetupColumn("Data", ImGuiTableColumnFlags_WidthStretch);

				ImGui::TableNextRow();
				{
					ImGui::TableSetColumnIndex(0);
					{
						ImGui::BeginDisabled(CurrentWeather == NULL);
						{
							if (ImGui::Button("Add", { 60, 20 }))
							{
								auto newWeather = new WeatherConfig();
								*newWeather = *WeatherList[WeatherIndex];
								newWeather->Time += 0.01;

								WeatherList.insert(WeatherList.begin() + WeatherIndex + 1, newWeather);
								WeatherIndex++;
								CurrentWeather = WeatherList[WeatherIndex];
							}

							ImGui::SameLine();
							if (ImGui::Button("Remove", { 60, 20 }))
							{
								delete WeatherList[WeatherIndex];
								WeatherList.erase(WeatherList.begin() + WeatherIndex);
								Reset();
							}
						}
						ImGui::EndDisabled();

						for (int i = 0; i < WeatherList.size(); i++)
						{
							auto weather = WeatherList[i];

							char tiemBuffer[32];
							sprintf(tiemBuffer, "%.2f", weather->Time);

							if (SelectableButton(tiemBuffer, { 100, 20 }, CurrentWeather == weather))
							{
								CurrentWeather = weather;
								WeatherIndex = i;
							}
						}
					}

					ImGui::TableSetColumnIndex(1);
					{
						if (CurrentWeather)
						{
							auto weather = isRain ? &CurrentWeather->Rain : &CurrentWeather->Main;

							if (SelectableButton("Main", { 0, 0 }, !isRain))
							{
								isRain = false;
							}

							ImGui::SameLine();
							if (SelectableButton("Rain", { 0, 0 }, isRain))
							{
								isRain = true;
							}

							ImGui::SameLine();
							ImGui::Checkbox("Force rain", Game::ForceRain);

							ImGui::SameLine();
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 50);
							if (ImGui::Button("Copy"))
							{
								CopyData = *weather;
							}

							ImGui::SameLine();
							if (ImGui::Button("Paste"))
							{
								*weather = CopyData;
							}

							ImGui::Text("");

							if (ImGui::BeginChild("##WeatherDataEditor", ImGui::GetContentRegionAvail(), false, 0))
							{
								InputFloat("Time", &CurrentWeather->Time);

								ImGui::SameLine();
								if (ImGui::Button("Set"))
								{
									TimeOfDay::Instance->CurrentTime = CurrentWeather->Time;
									TimeOfDay::Instance->UpdateRate = 0;
								}

								ImGui::Text("");

								ImGui::ColorEdit3("Ambient color", (float*)&weather->AmbientColor, ImGuiColorEditFlags_Float);
								ImGui::ColorEdit3("Diffuse color", (float*)&weather->DiffuseColor, ImGuiColorEditFlags_Float);
								ImGui::ColorEdit3("Specular color", (float*)&weather->SpecularColor, ImGuiColorEditFlags_Float);
								InputFloat("Specular power", &weather->SpecularPower);

								ImGui::Text("");

								InputFloat("Diffuse intensity", &weather->DiffuseIntensity);
								InputFloat("Ambient intensity", &weather->AmbientIntensity);

								ImGui::Text("");

								InputFloat("Car diffuse intensity", &weather->CarDiffuseIntensity);
								InputFloat("Car ambient intensity", &weather->CarAmbientIntensity);

								ImGui::Text("");

								ImGui::ColorEdit4("Cloud color", (float*)&weather->CloudColor, ImGuiColorEditFlags_Float);
								ImGui::ColorEdit3("Water color", (float*)&weather->WaterColor, ImGuiColorEditFlags_Float);
								InputFloat("Water specular power", &weather->WaterSpecularPower);

								ImGui::Text("");

								InputFloat("Car lights power", &weather->CarLightsPower);
								InputFloat("Texture light power", &weather->TextureLightPower);

								ImGui::Text("");

								ImGui::ColorEdit3("Sky Beta", (float*)&weather->SkyBeta, ImGuiColorEditFlags_Float);
								InputFloat("Sky Mie", &weather->SkyMie);
								InputFloat("Sky Rayleigh", &weather->SkyRayleigh);
								InputFloat("Sky Brightness", &weather->SkyBrightness);

								ImGui::Text("");

								InputFloat("Fog start", &weather->FogStart);
								InputFloat("Fog end", &weather->FogEnd);
								InputFloat("Fog power", &weather->FogPower);
								InputFloat("Fog exponent", &weather->FogExponent);
								ImGui::ColorEdit3("Fog color", (float*)&weather->FogColor, ImGuiColorEditFlags_Float);
								ImGui::ColorEdit3("Fog sun color", (float*)&weather->FogSunColor, ImGuiColorEditFlags_Float);

								ImGui::EndChild();
							}
						}
					}
				}

				ImGui::EndTable();
			}
		}
	}
}