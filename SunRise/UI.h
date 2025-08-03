#pragma once
#include <thread>
#include "Game.h"
#include "Config.h"
#include "UIEditor.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "minhook/MinHook.h"

typedef HRESULT(__stdcall* tEndScene)(LPDIRECT3DDEVICE9);
typedef HRESULT(__stdcall* tReset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);

tEndScene oEndScene;
tReset oReset;
WNDPROC oWNDPROC;
bool initUI = false;
bool wndProcHooked = false;

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

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	return CallWindowProc(oWNDPROC, hWnd, msg, wParam, lParam);
}

void InitUI(LPDIRECT3DDEVICE9 pDevice)
{
	if (!initUI)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
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

	UI::Draw();

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

void InitUI()
{
	if (g_Config.Editor)
	{
		std::thread(SubInitUI).detach();
	}
}