#pragma once
#include "CustomMeshesConfig.h"

namespace UI
{
	namespace Meshes
	{
		int CurrentNum = -1;
		char NameBuff[256] = { 0 };

		void Reset()
		{
			CurrentNum = -1;
		}

		CustomMesh* GetCurrent()
		{
			return CurrentNum >= 0 ? &CustomMeshes[CurrentNum] : NULL;
		}

		void Draw()
		{
			auto current = GetCurrent();

			if (ImGui::BeginTable("CustomMeshesEditorTable", 2, ImGuiTableFlags_BordersInnerV))
			{
				ImGui::TableSetupColumn("Solids", ImGuiTableColumnFlags_WidthFixed, 220.0f);
				ImGui::TableSetupColumn("Settings", ImGuiTableColumnFlags_WidthStretch);

				ImGui::TableNextRow();
				{
					ImGui::TableSetColumnIndex(0);
					{
						if (ImGui::Button("Add", { 60, 20 }))
						{
							CustomMesh customMesh;

							if (current)
							{
								customMesh = *current;
							}
							else
							{
								customMesh.Position = { 0, 0, 0 };
								customMesh.Rotation = { 0, 0, 0 };
								customMesh.Scale = { 1, 1, 1 };
							}

							CustomMeshes.push_back(customMesh);

							CurrentNum = CustomMeshes.size() - 1;
							current = GetCurrent();
						}

						ImGui::SameLine();
						if (ImGui::Button("Remove", { 60, 20 }))
						{
							if (current)
							{
								CustomMeshes.erase(CustomMeshes.begin() + CurrentNum);
								Reset();
							}
						}

						if (ImGui::BeginChild("##CustomMeshesList", ImGui::GetContentRegionAvail(), false, 0))
						{
							for (int i = 0; i < CustomMeshes.size(); i++)
							{
								auto& customMesh = CustomMeshes[i];

								char nameBuff[128];
								memset(nameBuff, ' ', 128);
								sprintf(nameBuff, "%d. %s", i + 1, customMesh.Name.c_str());
								int len = strlen(nameBuff);
								nameBuff[len] = ' ';
								nameBuff[127] = 0;

								if (SelectableButton(nameBuff, { 200, 20 }, current == &customMesh))
								{
									CurrentNum = i;
								}
							}

							ImGui::EndChild();
						}
					}

					ImGui::TableSetColumnIndex(1);
					{
						if (current)
						{
							current->SetMatrix();

							if (ImGui::Button("Place at camera"))
							{
								current->Position = GetCameraPos();
							}

							ImGui::Text("");

							strcpy(NameBuff, current->Name.c_str());
							if (ImGui::InputText("Solid", NameBuff, 256))
							{
								current->Name = NameBuff;
								current->Model.NameHash = Game::bStringHash(current->Name.c_str());
							}

							ImGui::Text("");

							ImGui::PushItemWidth(120);
							{
								ImGui::Text("Position");
								ImGui::InputFloat("##PosX", &current->Position.x, 0.1, 1.0, "%.2f");
								ImGui::SameLine();
								ImGui::InputFloat("##PosY", &current->Position.y, 0.1, 1.0, "%.2f");
								ImGui::SameLine();
								ImGui::InputFloat("##PosZ", &current->Position.z, 0.1, 1.0, "%.2f");

								ImGui::Text("");

								ImGui::Text("Rotation");
								ImGui::InputFloat("##RotX", &current->Rotation.x, 1.0, 2.0, "%.2f");
								ImGui::SameLine();
								ImGui::InputFloat("##RotY", &current->Rotation.y, 1.0, 2.0, "%.2f");
								ImGui::SameLine();
								ImGui::InputFloat("##RotZ", &current->Rotation.z, 1.0, 2.0, "%.2f");

								ImGui::Text("");

								ImGui::Text("Scale");
								ImGui::InputFloat("##ScaleX", &current->Scale.x, 0.1, 1.0, "%.2f");
								ImGui::SameLine();
								ImGui::InputFloat("##ScaleY", &current->Scale.y, 0.1, 1.0, "%.2f");
								ImGui::SameLine();
								ImGui::InputFloat("##ScaleZ", &current->Scale.z, 0.1, 1.0, "%.2f");
							}
							ImGui::PopItemWidth();
						}
					}
				}

				ImGui::EndTable();
			}
		}
	}
}