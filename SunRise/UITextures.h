#pragma once

namespace UI
{
	namespace Textures
	{
		PrelitTexture* CurrentTexture = NULL;
		char FilterBuff[128] = { 0 };
		char TextureNameBuff[128] = { 0 };
		char TextureMaskBuffer[128] = { 0 };

		void Reset()
		{
			CurrentTexture = NULL;
		}

		void Draw()
		{
			if (ImGui::Button("Remove", { 60, 20 }))
			{
				if (CurrentTexture)
				{
					PrelitTextures.Remove(CurrentTexture->NameHash);
					PrelitTextures.PopulateUiList();
					CurrentTexture = NULL;
				}
			}

			ImGui::SameLine();
			ImGui::InputText("Texture", TextureNameBuff, 128);

			ImGui::SameLine();
			if (ImGui::Button("Add", { 60, 20 }))
			{
				if (strlen(TextureNameBuff) > 0)
				{
					PrelitTexture newTex;

					newTex.Name = TextureNameBuff;
					newTex.ParseTextureName();

					PrelitTextures.Add(newTex);
					PrelitTextures.Sort();
					PrelitTextures.PopulateUiList();

					CurrentTexture = PrelitTextures.Get(newTex.NameHash);

					TextureNameBuff[0] = 0;
				}
			}

			if (ImGui::BeginTable("WeatherEditorTable", 2, ImGuiTableFlags_BordersInnerV))
			{
				ImGui::TableSetupColumn("Texture name", ImGuiTableColumnFlags_WidthFixed, 220.0f);
				ImGui::TableSetupColumn("Settings", ImGuiTableColumnFlags_WidthStretch);

				ImGui::TableNextRow();
				{
					ImGui::TableSetColumnIndex(0);
					{
						ImGui::InputText("Filter", FilterBuff, 128);
						ImGui::SameLine();
						if (ImGui::Button("x", { 20, 20 }))
						{
							FilterBuff[0] = 0;
						}

						if (ImGui::BeginChild("##TextureList", ImGui::GetContentRegionAvail(), false, 0))
						{
							for (auto texture : PrelitTextures.GetUiList())
							{
								if (FilterBuff[0] && !strstr(texture->Name.c_str(), FilterBuff))
								{
									continue;
								}

								if (SelectableButton(texture->Name.c_str(), { 200, 20 }, CurrentTexture == texture))
								{
									CurrentTexture = texture;
								}
							}

							ImGui::EndChild();
						}
					}

					ImGui::TableSetColumnIndex(1);
					{
						if (CurrentTexture)
						{
							ImGui::Checkbox("Prelit", &CurrentTexture->Prelit);
							ImGui::Checkbox("Ignore weather", &CurrentTexture->IgnoreWeather);

							ImGui::ColorEdit3("Color", (float*)&CurrentTexture->Color, ImGuiColorEditFlags_Float);
							InputFloat("Brightness", &CurrentTexture->Brightness);

							ImGui::Checkbox("Always on", &CurrentTexture->AlwaysOn);
							ImGui::Checkbox("Use vertex color", &CurrentTexture->UseVertexColor);

							ImGui::BeginDisabled(CurrentTexture->Prelit);
							{
								strcpy(TextureMaskBuffer, CurrentTexture->Mask.GetChar());
								if (ImGui::InputText("Mask texture", TextureMaskBuffer, 128))
								{
									CurrentTexture->Mask.SetString(TextureMaskBuffer);
									CurrentTexture->MaskTexture = NULL;
								}
							}
							ImGui::EndDisabled();
						}
					}
				}

				ImGui::EndTable();
			}
		}
	}
}