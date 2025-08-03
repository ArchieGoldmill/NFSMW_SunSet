#pragma once
#include "imgui/imgui.h"

namespace UI
{
	void InputFloat(const char* label, float* v)
	{
		ImGui::PushItemWidth(150);
		ImGui::InputFloat(label, v, 0.1, 0.3);
		ImGui::PopItemWidth();
	}
}