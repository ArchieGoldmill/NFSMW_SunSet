#pragma once

namespace UI
{
	namespace Textures
	{
		PrelitTexture* CurrentTexture = NULL;
		char FilterBuff[256] = { 0 };
		char TextureNameBuff[256] = { 0 };
		char TextureMaskBuffer[256] = { 0 };

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
					PrelitTextures.erase(CurrentTexture->NameHash);
					CurrentTexture = NULL;
				}
			}

			ImGui::SameLine();
			ImGui::InputText("Texture", TextureNameBuff, 256);

			ImGui::SameLine();
			if (ImGui::Button("Add", { 60, 20 }))
			{
				if (strlen(TextureNameBuff) > 0)
				{
					PrelitTexture newTex;

					newTex.Name = TextureNameBuff;
					newTex.ParseTextureName();

					PrelitTextures[newTex.NameHash] = newTex;

					CurrentTexture = &PrelitTextures[newTex.NameHash];

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
						ImGui::InputText("Filter", FilterBuff, 256);

						if (ImGui::BeginChild("##MaterialList", ImGui::GetContentRegionAvail(), false, 0))
						{
							for (auto& texture : PrelitTextures)
							{
								if (FilterBuff[0] && !strstr(texture.second.Name.c_str(), FilterBuff))
								{
									continue;
								}

								if (SelectableButton(texture.second.Name.c_str(), { 200, 20 }, CurrentTexture == &texture.second))
								{
									CurrentTexture = &texture.second;
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

							ImGui::ColorEdit3("Color", (float*)&CurrentTexture->Color, ImGuiColorEditFlags_Float);
							InputFloat("Brightness", &CurrentTexture->Color.w);

							ImGui::Checkbox("Always on", &CurrentTexture->AlwaysOn);

							ImGui::BeginDisabled(CurrentTexture->Prelit);
							{
								strcpy(TextureMaskBuffer, CurrentTexture->Mask.GetChar());
								if (ImGui::InputText("Mask texture", TextureMaskBuffer, 256))
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