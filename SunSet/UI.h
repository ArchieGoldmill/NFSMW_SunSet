#pragma once
#include <thread>
#include "Game.h"
#include "Config.h"
#include "UIEditor.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace UI
{
	WNDPROC oWNDPROC;
	bool initUI = false;
	bool wndProcHooked = false;
	bool DrawUI = true;

	void Reset()
	{
		if (initUI)
		{
			ImGui_ImplDX9_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();

			initUI = false;
		}
	}

	void HandleKeyDownInput(int wParam)
	{
		if (wParam < 1 || wParam > 0xFF)
		{
			return;
		}

		if (g_Config.HK_ToggleEditor == wParam)
		{
			DrawUI = !DrawUI;
		}
	}

	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		{
			return true;
		}

		switch (msg)
		{
		case WM_KEYDOWN:
			HandleKeyDownInput((int)wParam);
			break;
		}

		return CallWindowProc(oWNDPROC, hWnd, msg, wParam, lParam);
	}

	void InitUI(LPDIRECT3DDEVICE9 pDevice)
	{
		if (!initUI)
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			ImGui::GetIO().FontGlobalScale = g_Config.EditorScale;

			ImGui::StyleColorsDark();

			if (!wndProcHooked)
			{
				oWNDPROC = (WNDPROC)SetWindowLongPtr(Game::Window, GWLP_WNDPROC, (LONG_PTR)WndProc);
				wndProcHooked = true;
			}

			ImGui_ImplWin32_Init(Game::Window);
			ImGui_ImplDX9_Init(pDevice);

			initUI = true;
		}
	}

	IDirect3DDevice9* hookedEndScene()
	{
		InitUI(Game::Device);

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (DrawUI)
		{
			UI::Draw();
		}

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		return Game::Device;
	}

	void __declspec(naked) hookedEndSceneCave()
	{
		static constexpr auto cExit = 0x006E75B1;

		__asm
		{
			SAVE_REGS_EAX;
			call hookedEndScene;
			RESTORE_REGS_EAX;
			jmp cExit;
		}
	}

	void Init()
	{
		if (!g_Config.Editor)
		{
			return;
		}

		if (*(short*)0x006E6CAC != 0x0BE9)
		{
			injector::MakeNOP(0x006E6CAE, 7);
		}

		injector::MakeJMP(0x006E75AC, hookedEndSceneCave);
	}
}