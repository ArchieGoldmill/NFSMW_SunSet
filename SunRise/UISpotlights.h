#pragma once
#include "SpotLightConfig.h"

namespace UI
{
	namespace Spotlights
	{
		SolidLights* CurrentSolid = NULL;
		int CurrentSolidNum = -1;
		SpotLight* CurrentLight = NULL;
		int CurrentLightNum = -1;

		char SolidLodBufferA[256] = { 0 };
		char SolidLodBufferB[256] = { 0 };

		void Reset()
		{
			CurrentSolid = NULL;
			CurrentSolidNum = -1;
			CurrentLight = NULL;
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
							solidLights.LodB.SetString("");
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

							solidLights.Lights.push_back(spotLight);
							SolidLightsList.push_back(solidLights);
						}

						ImGui::SameLine();
						if (ImGui::Button("Remove##Solid", { 60, 20 }))
						{
							if (CurrentSolid)
							{
								SolidLightsList.erase(SolidLightsList.begin() + CurrentSolidNum);
								CurrentSolid = NULL;
							}
						}

						if (ImGui::BeginChild("##SolidList", ImGui::GetContentRegionAvail(), false, 0))
						{
							for (int i = 0; i < SolidLightsList.size(); i++)
							{
								auto& solidLights = SolidLightsList[i];
								if (SelectableButton(solidLights.LodA.GetChar(), { 200, 20 }, CurrentSolid == &solidLights))
								{
									CurrentSolidNum = i;
									CurrentSolid = &solidLights;
									CurrentLight = NULL;
								}
							}

							ImGui::EndChild();
						}
					}

					ImGui::TableSetColumnIndex(1);
					{
						if (CurrentSolid)
						{
							if (ImGui::Button("Add##Light", { 40, 20 }))
							{
								SpotLight spotlight = CurrentSolid->Lights[0];
								CurrentSolid->Lights.push_back(spotlight);
							}

							ImGui::SameLine();
							if (ImGui::Button("Remove##Light", { 60, 20 }))
							{
								if (CurrentSolid->Lights.size() > 1)
								{
									CurrentSolid->Lights.erase(CurrentSolid->Lights.begin() + CurrentLightNum);
									CurrentLight = NULL;
								}
							}

							char lightNameBuff[32];
							for (int i = 0; i < CurrentSolid->Lights.size(); i++)
							{
								auto& ligt = CurrentSolid->Lights[i];
								sprintf(lightNameBuff, "Light #%d", i + 1);
								if (SelectableButton(lightNameBuff, { 100, 20 }, CurrentLight == &ligt))
								{
									CurrentLight = &ligt;
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
							if (ImGui::InputText("Lod A", SolidLodBufferA, 256))
							{
								CurrentSolid->LodA.SetString(SolidLodBufferA);
							}

							strcpy(SolidLodBufferB, CurrentSolid->LodB.GetChar());
							if (ImGui::InputText("Lod B", SolidLodBufferB, 256))
							{
								CurrentSolid->LodB.SetString(SolidLodBufferB);
							}

							ImGui::Text("");
							const char* BlinkItems[] = { "None", "1", "2", "3" };
							ImGui::Combo("Blink", &CurrentSolid->Blink, BlinkItems, 4);

							ImGui::Text("");
							ImGui::Checkbox("Always on", &CurrentSolid->AlwaysOn);
						}

						if (CurrentLight)
						{
							ImGui::Text("");

							if (ImGui::Button("Place at camera"))
							{
								CurrentLight->Position = GetCameraPos();
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

							ImGui::ColorEdit3("Color", (float*)&CurrentLight->Color, ImGuiColorEditFlags_Float);

							ImGui::Text("");

							InputFloat("Range", &CurrentLight->Range);
							InputFloat("Intensity", &CurrentLight->Intensity);
							InputFloat("Inner angle", &CurrentLight->InnerAngle);
							InputFloat("Outer angle", &CurrentLight->OuterAngle);
						}
					}
				}

				ImGui::EndTable();
			}
		}
	}
}