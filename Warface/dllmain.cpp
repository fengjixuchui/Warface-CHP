#include "stdafx.h"
#include "Includes.h"
#include "Functions.h"
#include "Draw.h"
#include <ctime>
#define _CRT_SECURE_NO_WARNINGS

static bool bWasOpen = false;
static bool Create = false;

typedef HRESULT(WINAPI* EndScene_)(LPDIRECT3DDEVICE9 pDevice);
EndScene_ pEndScene;
HMODULE OhModule;
PDIRECT3DDEVICE9 pDevice;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC oWndProc;



void Menu(LPDIRECT3DDEVICE9 pDevice) {

	static bool init = true;
	if (init)
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();

		style.FrameRounding = 10.0f;
		style.GrabRounding = 40.0f;
		io.WantCaptureMouse = true;

		if (GetAsyncKeyState(VK_DELETE) & 1)
			bShow = (!bShow);

		ImGui_ImplWin32_Init(GetForegroundWindow());
		ImGui_ImplDX9_Init(pDevice);

	}
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		DrawMenu();
		ImGui::EndFrame();
		ImGui::Render();

	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());


}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}




HRESULT GenerateTexture(IDirect3DDevice9* pD3Ddev, IDirect3DTexture9** ppD3Dtex, DWORD colour32) {
	if (FAILED(pD3Ddev->CreateTexture(8, 8, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, ppD3Dtex, NULL)))
		return E_FAIL;
	WORD colour16 = ((WORD)((colour32 >> 28) & 0xF) << 12)
		| (WORD)(((colour32 >> 20) & 0xF) << 8)
		| (WORD)(((colour32 >> 12) & 0xF) << 4)
		| (WORD)(((colour32 >> 4) & 0xF) << 0);
	D3DLOCKED_RECT d3dlr;
	(*ppD3Dtex)->LockRect(0, &d3dlr, 0, 0);
	WORD* pDst16 = (WORD*)d3dlr.pBits;
	for (int xy = 0; xy < 8 * 8; xy++)
		*pDst16++ = colour16;
	(*ppD3Dtex)->UnlockRect(0);
	return S_OK;
}

HRESULT WINAPI EndScene(LPDIRECT3DDEVICE9 pDevice)
{

		Menu(pDevice);

	
	
	Y = (Topo + 6);
	total_items = 0;

	if (Gerartexture) {
		GenerateTexture(pDevice, &Vermelho, Red);
		GenerateTexture(pDevice, &amarelo, D3DCOLOR_ARGB(255, 255, 255, 0));
		GenerateTexture(pDevice, &verde, D3DCOLOR_ARGB(255, 0, 255, 0));
		GenerateTexture(pDevice, &azul, D3DCOLOR_ARGB(255, 0, 0, 255));

		Gerartexture = false;
	}

	if (!Create)
	{
		pDevice->GetViewport(&Viewport);
		ScreenWidth = Viewport.Width;
		ScreenHeight = Viewport.Height;
		ScreenCenterX = (ScreenWidth / 2);
		ScreenCenterY = (ScreenHeight / 2);
		Create = true;
	}


	Functions();
	
	return (*pEndScene)(pDevice);
}



DWORD FindDevice(DWORD Len)
{
	DWORD dwObjBase = 0;
	dwObjBase = (DWORD)LoadLibrary("d3d9.dll");
	while (dwObjBase++ < dwObjBase + Len)
	{
		if ((*(WORD*)(dwObjBase + 0x00)) == 0x06C7
			&& (*(WORD*)(dwObjBase + 0x06)) == 0x8689
			&& (*(WORD*)(dwObjBase + 0x0C)) == 0x8689
			) {
			dwObjBase += 2; break;
		}
	}
	return(dwObjBase);
}

DWORD GetDeviceAddress(int VTableIndex)
{
	PDWORD VTable;
	*(DWORD*)&VTable = *(DWORD*)FindDevice(0x128000);
	return VTable[VTableIndex];
}

DWORD WINAPI endhook(LPVOID pThreadParameter)
{
	pEndScene = (EndScene_)GetDeviceAddress(42);

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)pEndScene, (PVOID)EndScene);
	DetourTransactionCommit();
		
	HWND  window = FindWindowA(NULL, XorString("Warface"));

	oWndProc = (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)WndProc);

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		OhModule = hModule;
		DisableThreadLibraryCalls(hModule);
		CloseHandle(CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)endhook, NULL, NULL, NULL));
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
