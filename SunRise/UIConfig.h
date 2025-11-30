#pragma once

namespace UI
{
	namespace Cfg
	{
		char FilterBuffer[128] = { 0 };

		void Draw()
		{
			if (ImGui::BeginChild("##ConfigScroller", ImGui::GetContentRegionAvail(), false, 0))
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

				ImGui::Checkbox("World vertex color", &g_Config.WorldVertexColor);
				ImGui::Checkbox("Car vertex color", &g_Config.CarVertexColor);
				ImGui::Checkbox("Tunnel wetness fix", &g_Config.TunnelWetnessFix);
				ImGui::Checkbox("Disable night shadows", &g_Config.DisableNightShadows);
				ImGui::Checkbox("Tonemapping", &g_Config.Tonemapping);
				if (ImGui::Checkbox("MSAA 8x", &g_Config.MSAAx8))
				{
					Game::ReloadDevice = true;
				}

				ImGui::InputFloat("Custom mesh dist", &g_Config.CustomMeshDist, 0.1, 0.2);
				ImGui::InputInt("Max lights", &g_Config.MaxLights, 1, 3);

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
				if (ImGui::SliderFloat("Sun azimuth", &g_Config.SunAzimuth, 0.0f, 360.0f, "%.1f"))
				{
					TimeOfDay::Instance->SunAzimuth = D3DXToRadian(g_Config.SunAzimuth);
				}
				ImGui::Checkbox("Random startup time", &g_Config.RandomStartupTime);
				ImGui::Checkbox("Real time", &g_Config.RealTime);

				ImGui::Text("");
				ImGui::Text("Blur");
				ImGui::InputFloat("Min speed", &g_Config.BlurMinSpeed, 0.1, 0.2);
				ImGui::InputFloat("Max speed", &g_Config.BlurMaxSpeed, 0.1, 0.2);
				ImGui::InputFloat("Depth", &g_Config.BlurDepth, 0.1, 0.2);

				ImGui::Text("");
				ImGui::Text("Rain");
				ImGui::InputFloat("Dry time ", &g_Config.Rain.DryTime, 0.1, 0.2);
				ImGui::InputFloat("Dry time random", &g_Config.Rain.DryTimeRandom, 0.1, 0.2);
				ImGui::InputFloat("Light time ", &g_Config.Rain.LightTime, 0.1, 0.2);
				ImGui::InputFloat("Light time random", &g_Config.Rain.LightTimeRandom, 0.1, 0.2);
				ImGui::InputFloat("Heavy time ", &g_Config.Rain.HeavyTime, 0.1, 0.2);
				ImGui::InputFloat("Heavy time random", &g_Config.Rain.HeavyTimeRandom, 0.1, 0.2);
				ImGui::InputFloat("Lightning time out", &g_Config.Rain.LightningTimeOut, 0.1, 0.2);
				ImGui::Checkbox("Rain clouds texture", &g_Config.Rain.RainCloudsTexture);

				ImGui::PopItemWidth();

				ImGui::Text("");
				ImGui::Text("Window glow");
				ImGui::ColorEdit3("Color", (float*)&g_Config.WindowGlowColor, ImGuiColorEditFlags_Float);
				ImGui::PushItemWidth(120);
				ImGui::InputFloat("Power", &g_Config.WindowGlowPower, 0.1, 0.2);
				ImGui::PopItemWidth();
				ImGui::Checkbox("Override", &g_Config.WindowGlowOverride);

				ImGui::Text("");
				strcpy(FilterBuffer, g_Config.Filter.GetChar());
				if (ImGui::InputText("Filter", FilterBuffer, 128))
				{
					g_Config.Filter.SetString(FilterBuffer);

					if (g_Config.Filter.str.length() == 0)
					{
						g_Config.Filter.hash = Game::bStringHash("FILTER_DEFAULT");
					}

					auto filterTexture = TextureInfo::Get(g_Config.Filter.hash, false, false);
					if (filterTexture)
					{
						ReleaseFilterTexture();
					}
				}

				ImGui::InputFloat("Filter power", &g_Config.FilterPower, 0.1, 0.2);

#ifdef _DEBUG
				ImGui::Text("");
				ImGui::Text("Spotlights: %d", NumSpotLightBuffer);
#endif
				ImGui::EndChild();
			}
		}
	}
}