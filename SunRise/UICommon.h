#pragma once
#include "imgui/imgui.h"

namespace UI
{
	ImVec2 GetSize(const ImVec2& size_arg)
	{
		ImVec2 size = size_arg;

		size.x *= g_Config.EditorScale;
		size.y *= g_Config.EditorScale;

		return size;
	}

	bool InputFloat(const char* label, float* v)
	{
		ImGui::PushItemWidth(150 * g_Config.EditorScale);
		bool result = ImGui::InputFloat(label, v, 0.1, 0.3);
		ImGui::PopItemWidth();
		return result;
	}

	bool SelectableButton(const char* label, const ImVec2& size_arg, bool selected)
	{
		if (selected)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 1.0f, 1.0f));
		}

		ImVec2 size = size_arg;

		size.x *= g_Config.EditorScale;
		size.y *= g_Config.EditorScale;

		bool clicked = ImGui::Button(label, size);

		if (selected)
		{
			ImGui::PopStyleColor();
		}

		return clicked;
	}
}