#pragma once

namespace UI
{
	namespace Cfg
	{
		void Draw()
		{
			ImGui::PushItemWidth(120);

			ImGui::Checkbox("Console", &g_Config.Console);
			ImGui::Checkbox("Shader loader", &g_Config.ShaderLoader);
			ImGui::Checkbox("Shader compiler", &g_Config.ShaderCompiler);
			ImGui::Checkbox("X360 Effects", &g_Config.X360Effects);
			ImGui::Checkbox("Live reload", &g_Config.LiveReload);
			if (ImGui::Checkbox("Editor", &g_Config.Editor))
			{
				if (g_Config.Editor)
				{
					g_Config.LiveReload = false;
				}
			}
			ImGui::Checkbox("Skip FE", &g_Config.SkipFE);
			ImGui::Checkbox("Car vinyl paint fix", &g_Config.CarVinylPaintFix);
			ImGui::Checkbox("Brake glow", &g_Config.BrakeGlow);
			ImGui::Checkbox("God rays", &g_Config.GodRays);
			ImGui::Checkbox("Bloom", &g_Config.Bloom);
			ImGui::Checkbox("Real Front End reflections", &g_Config.RealFeReflections);
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
			{
				ImGui::SetTooltip("Requires game restart");
			}

			ImGui::Text("");
			ImGui::Text("Time");
			if (ImGui::InputFloat("Force time", &g_Config.ForceTime, 0.1, 0.2))
			{
				if (g_Config.ForceTime > 1)
				{
					g_Config.ForceTime = 1;
				}
			}
			if (ImGui::InputFloat("Time update rate", &g_Config.TimeUpdateRate, 0.1, 0.2))
			{
				TimeOfDay::Instance->UpdateRate = g_Config.TimeUpdateRate;
			}
			ImGui::InputFloat("Lights on", &g_Config.LightsOn, 0.1, 0.2);
			ImGui::InputFloat("Lights off", &g_Config.LightsOff, 0.1, 0.2);
			ImGui::InputFloat("Wet time", &g_Config.WetTime, 0.1, 0.2);
			ImGui::InputFloat("Dry time", &g_Config.DryTime, 0.1, 0.2);
			ImGui::Checkbox("Random startup time", &g_Config.RandomStartupTime);
			ImGui::Checkbox("Real time", &g_Config.RealTime);

			ImGui::Text("");
			ImGui::Text("Blur");
			ImGui::InputFloat("Min speed", &g_Config.BlurMinSpeed, 0.1, 0.2);
			ImGui::InputFloat("Max speed", &g_Config.BlurMaxSpeed, 0.1, 0.2);
			ImGui::InputFloat("Depth", &g_Config.BlurDepth, 0.1, 0.2);

			ImGui::PopItemWidth();

			ImGui::Text("");
			ImGui::Text("Default params");
			ImGui::ColorEdit3("Window glow color", (float*)&g_Config.WindowGlowColor, ImGuiColorEditFlags_Float);
			ImGui::PushItemWidth(120);
			ImGui::InputFloat("Window glow power", &g_Config.WindowGlowPower, 0.1, 0.2);
			ImGui::PopItemWidth();

#ifdef _DEBUG
			ImGui::Text("");
			ImGui::Text("Spotlights: %d", NumSpotLightBuffer);
#endif
		}
	}
}