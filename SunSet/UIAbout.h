#pragma once

namespace UI
{
	namespace About
	{
		void Draw()
		{
			ImGui::Text(ModName);

			ImGui::Text("");
			ImGui::Text("Credits");
			ImGui::BulletText("Dear ImGui by Omar Cornut (github.com/ocornut/imgui)");
			ImGui::BulletText("MinHook (github.com/TsudaKageyu/minhook)");

			ImGui::Text("");
			ImGui::Text("Please report any bugs to my Discord server or NFSMods page.");

			ImGui::Text("");
			ImGui::Text("Made by ARCHIE");

			ImGui::Text("");
			if (ImGui::Button("DISCORD"))
			{
				ShellExecute(0, 0, L"https://discord.gg/DxC6Fyf", 0, 0, SW_SHOW);
			}
			ImGui::SameLine();
			if (ImGui::Button("NFS MODS"))
			{
				ShellExecute(0, 0, L"https://nfsmods.xyz/mod/6659", 0, 0, SW_SHOW);
			}
		}
	}
}