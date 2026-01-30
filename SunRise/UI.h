#pragma once
#include <thread>
#include "Game.h"
#include "Config.h"
#include "UIEditor.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "minhook/MinHook.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace UI
{
	typedef HRESULT(__stdcall* tEndScene)(LPDIRECT3DDEVICE9);
	typedef HRESULT(__stdcall* tReset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);

	tEndScene oEndScene;
	tReset oReset;
	WNDPROC oWNDPROC;
	bool initUI = false;
	bool wndProcHooked = false;
	bool DrawUI = true;

	HRESULT __stdcall hookedReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		if (initUI)
		{
			ImGui_ImplDX9_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();

			initUI = false;
		}

		return oReset(pDevice, pPresentationParameters);
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

	HRESULT __stdcall hookedEndScene(LPDIRECT3DDEVICE9 pDevice)
	{
		InitUI(pDevice);

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

		return oEndScene(pDevice);
	}

	void SubInitUI()
	{
		LPDIRECT3DDEVICE9 pDevice = NULL;
		while (pDevice == NULL)
		{
			pDevice = Game::Device;
			Sleep(1000);
		}

		auto vt = *(void***)pDevice;

		MH_Initialize();

		MH_CreateHook(vt[16], hookedReset, (LPVOID*)&oReset);
		MH_CreateHook(vt[42], hookedEndScene, (LPVOID*)&oEndScene);

		MH_EnableHook(MH_ALL_HOOKS);
	}

	void Init()
	{
		if (!g_Config.Editor)
		{
			return;
		}

		injector::MakeNOP(0x006E6CAE, 7);

		std::thread(SubInitUI).detach();
	}
}