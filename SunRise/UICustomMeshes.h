#pragma once
#include "CustomMeshesConfig.h"

namespace UI
{
	namespace Meshes
	{
		CustomMesh* Current = NULL;
		int CurrentNum = -1;
		char NameBuff[256] = { 0 };

		void Reset()
		{
			Current = NULL;
			CurrentNum = -1;
		}

		void Draw()
		{
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

							customMesh.Position = { 0, 0, 0 };
							customMesh.Rotation = { 0, 0, 0 };
							customMesh.Scale = { 1, 1, 1 };
							customMesh.Model.Next = NULL;
							customMesh.Model.Prev = NULL;
							customMesh.Model.pSolid = NULL;

							CustomMeshes.push_back(customMesh);

							CurrentNum = CustomMeshes.size() - 1;
							Current = &CustomMeshes[CurrentNum];
						}

						ImGui::SameLine();
						if (ImGui::Button("Remove##Solid", { 60, 20 }))
						{
							if (Current)
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
								sprintf(nameBuff, "%d. %s", i + 1, customMesh.Name.c_str());

								if (SelectableButton(nameBuff, { 200, 20 }, Current == &customMesh))
								{
									CurrentNum = i;
									Current = &customMesh;
								}
							}

							ImGui::EndChild();
						}
					}

					ImGui::TableSetColumnIndex(1);
					{
						if (Current)
						{
							Current->SetMatrix();

							if (ImGui::Button("Place at camera"))
							{
								Current->Position = GetCameraPos();
							}

							ImGui::Text("");

							strcpy(NameBuff, Current->Name.c_str());
							if (ImGui::InputText("Solid", NameBuff, 256))
							{
								Current->Name = NameBuff;
								Current->Model.NameHash = Game::bStringHash(Current->Name.c_str());
							}

							ImGui::Text("");

							ImGui::PushItemWidth(120);
							{
								ImGui::Text("Position");
								ImGui::InputFloat("##PosX", &Current->Position.x, 0.1, 1.0, "%.2f");
								ImGui::SameLine();
								ImGui::InputFloat("##PosY", &Current->Position.y, 0.1, 1.0, "%.2f");
								ImGui::SameLine();
								ImGui::InputFloat("##PosZ", &Current->Position.z, 0.1, 1.0, "%.2f");

								ImGui::Text("");

								ImGui::Text("Rotation");
								ImGui::InputFloat("##RotX", &Current->Rotation.x, 0.1, 1.0, "%.2f");
								ImGui::SameLine();
								ImGui::InputFloat("##RotY", &Current->Rotation.y, 0.1, 1.0, "%.2f");
								ImGui::SameLine();
								ImGui::InputFloat("##RotZ", &Current->Rotation.z, 0.1, 1.0, "%.2f");

								ImGui::Text("");

								ImGui::Text("Scale");
								ImGui::InputFloat("##ScaleX", &Current->Scale.x, 0.1, 1.0, "%.2f");
								ImGui::SameLine();
								ImGui::InputFloat("##ScaleY", &Current->Scale.y, 0.1, 1.0, "%.2f");
								ImGui::SameLine();
								ImGui::InputFloat("##ScaleZ", &Current->Scale.z, 0.1, 1.0, "%.2f");
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