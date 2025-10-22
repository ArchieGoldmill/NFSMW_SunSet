#pragma once
#include "SpotLightConfig.h"

namespace UI
{
	namespace Spotlights
	{
		SolidLights* CurrentSolid = NULL;
		int CurrentLightNum = -1;

		char SolidLodBufferA[128] = { 0 };
		char FlareNameBuffer[128] = { 0 };
		char FilterBuff[128] = { 0 };

		void Reset()
		{
			CurrentSolid = NULL;
			CurrentLightNum = -1;
		}

		void Draw()
		{
			if (ImGui::BeginTable("SpotlightEditorTable", 3, ImGuiTableFlags_BordersInnerV))
			{
				ImGui::TableSetupColumn("Solid", ImGuiTableColumnFlags_WidthFixed, 220.0f);
				ImGui::TableSetupColumn("Lights", ImGuiTableColumnFlags_WidthFixed, 110.0f);
				ImGui::TableSetupColumn("Settings", ImGuiTableColumnFlags_WidthStretch);

				ImGui::TableNextRow();
				{
					ImGui::TableSetColumnIndex(0);
					{
						if (ImGui::Button("Add##Solid", { 60, 20 }))
						{
							SolidLights solidLights;
							solidLights.LodA.SetString("SOLID_A");
							solidLights.Blink = 0;
							solidLights.Flare = GetFlareModel("LampPost");

							SpotLight spotLight;
							spotLight.Position = { 0, 0, 0 };
							spotLight.Direction = { 0, 0, -1 };
							spotLight.Color = { 0, 0, 0 };
							spotLight.Range = 30;
							spotLight.Intensity = 5;
							spotLight.InnerAngle = 15;
							spotLight.OuterAngle = 45;
							spotLight.Specular = 1;

							solidLights.Lights.push_back(spotLight);
							SolidLightsList.Add(solidLights);
							SolidLightsList.Sort();
							SolidLightsList.PopulateUiList();

							CurrentSolid = SolidLightsList.Get(solidLights.LodA.hash);
							CurrentLightNum = 0;
						}

						ImGui::SameLine();
						if (ImGui::Button("Remove##Solid", { 60, 20 }))
						{
							if (CurrentSolid)
							{
								SolidLightsList.Remove(CurrentSolid->LodA.hash);
								SolidLightsList.PopulateUiList();
								CurrentSolid = NULL;
							}
						}

						ImGui::InputText("Filter", FilterBuff, 128);
						ImGui::SameLine();
						if (ImGui::Button("x", { 20, 20 }))
						{
							FilterBuff[0] = 0;
						}

						if (ImGui::BeginChild("##SolidList", ImGui::GetContentRegionAvail(), false, 0))
						{
							for (auto solidLights : SolidLightsList.GetUiList())
							{
								if (FilterBuff[0] && !strstr(solidLights->LodA.GetChar(), FilterBuff))
								{
									continue;
								}

								if (SelectableButton(solidLights->LodA.GetChar(), { 200, 20 }, CurrentSolid == solidLights))
								{
									CurrentSolid = solidLights;
									CurrentLightNum = 0;
								}
							}

							ImGui::EndChild();
						}
					}

					ImGui::TableSetColumnIndex(1);
					{
						if (CurrentSolid)
						{
							auto& lights = CurrentSolid->Lights;

							if (ImGui::Button("Add##Light", { 40, 20 }))
							{
								lights.push_back(lights[0]);
								CurrentLightNum = lights.size() - 1;
							}

							ImGui::SameLine();
							if (ImGui::Button("Remove##Light", { 60, 20 }))
							{
								if (lights.size() > 1)
								{
									lights.erase(lights.begin() + CurrentLightNum);
									CurrentLightNum = -1;
								}
							}

							char lightNameBuff[32];
							for (int i = 0; i < lights.size(); i++)
							{
								auto& ligt = lights[i];
								sprintf(lightNameBuff, "Light #%d", i + 1);
								if (SelectableButton(lightNameBuff, { 100, 20 }, CurrentLightNum == i))
								{
									CurrentLightNum = i;
								}
							}
						}
					}

					ImGui::TableSetColumnIndex(2);
					{
						if (CurrentSolid)
						{
							strcpy(SolidLodBufferA, CurrentSolid->LodA.GetChar());
							if (ImGui::InputText("Lod A", SolidLodBufferA, 128))
							{
								CurrentSolid->LodA.SetString(SolidLodBufferA);
								SolidLightsList.Sort();
								SolidLightsList.PopulateUiList();
								CurrentSolid = SolidLightsList.Get(Game::bStringHash(SolidLodBufferA));
							}

							ImGui::Text("");
							const char* BlinkItems[] = { "None", "1", "2", "3" };
							ImGui::Combo("Blink", &CurrentSolid->Blink, BlinkItems, 4);

							ImGui::Text("");
							if (CurrentSolid->Flare)
							{
								strcpy(FlareNameBuffer, CurrentSolid->Flare->Name.c_str());
							}
							else
							{
								FlareNameBuffer[0] = 0;
							}
							if (ImGui::InputText("Flare", FlareNameBuffer, 128))
							{
								CurrentSolid->Flare = GetFlareModel(FlareNameBuffer);
							}

							ImGui::Text("");
							ImGui::Checkbox("Always on", &CurrentSolid->AlwaysOn);
							ImGui::Checkbox("Use first light", &CurrentSolid->UseFirstLight);

							if (CurrentLightNum >= 0)
							{
								auto CurrentLight = &CurrentSolid->Lights[CurrentLightNum];

								ImGui::Text("");

								if (ImGui::Button("Place at camera"))
								{
									CurrentLight->Position = GetCameraPos();
								}

								ImGui::SameLine();
								if (ImGui::Button("Move camera"))
								{
									*Game::DebugCameraPos = CurrentLight->Position;
								}
								if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
								{
									ImGui::SetTooltip("Requires debug camera to be enabled, make sure light is in world space");
								}

								ImGui::PushItemWidth(120);
								{
									ImGui::Text("Position");
									ImGui::InputFloat("##LightPosX", &CurrentLight->Position.x, 0.1, 1.0, "%.2f");
									ImGui::SameLine();
									ImGui::InputFloat("##LightPosY", &CurrentLight->Position.y, 0.1, 1.0, "%.2f");
									ImGui::SameLine();
									ImGui::InputFloat("##LightPosZ", &CurrentLight->Position.z, 0.1, 1.0, "%.2f");

									ImGui::Text("");

									ImGui::Text("Direction");
									ImGui::InputFloat("##LightDirX", &CurrentLight->Direction.x, 0.1, 1.0, "%.2f");
									ImGui::SameLine();
									ImGui::InputFloat("##LightDirY", &CurrentLight->Direction.y, 0.1, 1.0, "%.2f");
									ImGui::SameLine();
									ImGui::InputFloat("##LightDirZ", &CurrentLight->Direction.z, 0.1, 1.0, "%.2f");
								}
								ImGui::PopItemWidth();

								ImGui::Text("");

								ImGui::BeginDisabled(CurrentLightNum != 0 && CurrentSolid->UseFirstLight);
								ImGui::ColorEdit3("Color", (float*)&CurrentLight->Color, ImGuiColorEditFlags_Float);

								ImGui::Text("");

								InputFloat("Range", &CurrentLight->Range);
								InputFloat("Intensity", &CurrentLight->Intensity);
								InputFloat("Inner angle", &CurrentLight->InnerAngle);
								InputFloat("Outer angle", &CurrentLight->OuterAngle);
								InputFloat("Specular", &CurrentLight->Specular);
								ImGui::EndDisabled();
							}
						}
					}
				}

				ImGui::EndTable();
			}
		}
	}
}