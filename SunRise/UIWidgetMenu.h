#pragma once
#include "Utilities.h"

struct FEToggleWidget
{
	void* vTable;

	void Ctor(bool enabled)
	{
		FUNC(0x00589300, void, __thiscall, _Ctor, FEToggleWidget*,bool);
		_Ctor(this, enabled);
	}
};

struct UIWidgetMenu
{
	void AddToggleOption(FEToggleWidget* toggleWidget, bool a4)
	{
		FUNC(0x00588570, void, __thiscall, _AddToggleOption, UIWidgetMenu*, FEToggleWidget*, bool);
		_AddToggleOption(this, toggleWidget, a4);
	}
};