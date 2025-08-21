#pragma once
#include "imgui/imgui.h"

namespace UI
{
	bool InputFloat(const char* label, float* v)
	{
		ImGui::PushItemWidth(150);
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

		bool clicked = ImGui::Button(label, size_arg);

		if (selected)
		{
			ImGui::PopStyleColor();
		}

		return clicked;
	}
}