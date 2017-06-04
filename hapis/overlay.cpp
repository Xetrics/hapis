#include "overlay.h"
#include "main.h"

namespace Overlay {
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	const MARGINS margin = { 0, 0, width, height };
	HWND hwnd;
	HWND tHwnd;
	RECT tSize;
	RECT rc;

	IDirect3D9Ex* p_Object;
	IDirect3DDevice9Ex* p_Device;
	D3DPRESENT_PARAMETERS p_Params;

	void render() {
		if (tHwnd == GetForegroundWindow()) {
			char* value = OVERLAY_TARGET;

			HWND newhwnd = FindWindow(NULL, value);
			if (newhwnd != NULL) {
				GetWindowRect(newhwnd, &rc);
			}

			p_Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
			p_Device->BeginScene();

			/* Draw Watermark */
			ID3DXFont* pFont;
			D3DXCreateFont(p_Device, 20, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &pFont);
			Drawing::DrawString("hapis.exe", 25, 10, 255, 255, 255, 255, pFont);

			/* Draw ESP Boxes */
			for (auto player : players) {
				Rust::Vector3 pos;
				bool visible = Math::WorldToScreen(player.second, pos, localPlayer->viewMatrix, width, height);

				if (visible)  Drawing::DrawString("player", pos.x, pos.y, 255, 255, 255, 255, pFont);
			}

			/* Draw Scene */
			p_Device->EndScene();
			p_Device->PresentEx(0, 0, 0, 0, 0);
		}
	}

	/* Moves the invisible window to the position and size of Rust */
	void setWindowToTarget() {
		for(;;) {
			tHwnd = FindWindow(FALSE, OVERLAY_TARGET);
			if (tHwnd)
			{
				GetWindowRect(tHwnd, &tSize);
				width = tSize.right - tSize.left;
				height = tSize.bottom - tSize.top;
				DWORD dwStyle = GetWindowLong(tHwnd, GWL_STYLE);
				if (dwStyle & WS_BORDER)
				{
					tSize.top += 23;
					height -= 23;
				}
				MoveWindow(hwnd, tSize.left, tSize.top, width, height, TRUE);
			}

			Sleep(500);
		}
	}

	void InitDirectX() {
		if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
			printf("[Overlay] Failed to create Direct3DCreate9Ex object, sdk version %d", D3D_SDK_VERSION);
		ZeroMemory(&p_Params, sizeof(p_Params));
		p_Params.Windowed = TRUE;
		p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
		p_Params.hDeviceWindow = hwnd;
		p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
		p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
		p_Params.BackBufferWidth = width;
		p_Params.BackBufferHeight = height;
		p_Params.EnableAutoDepthStencil = TRUE;
		p_Params.AutoDepthStencilFormat = D3DFMT_D16;

		if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device)))
			printf("[Overlay] Failed to create create DX device");
	}

	/* Fuck WinAPI */
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_PAINT:
			render();
			break;

		case WM_CREATE:
			DwmExtendFrameIntoClientArea(hWnd, &margin);
			break;

		case WM_DESTROY:
			PostQuitMessage(1);
			return 0;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
		}
		return 0;
	}

	void Init() {
		CreateThread(FALSE, FALSE, (LPTHREAD_START_ROUTINE)setWindowToTarget, FALSE, FALSE, FALSE);

		WNDCLASSEX wClass;
		wClass.cbClsExtra = NULL;
		wClass.cbSize = sizeof(WNDCLASSEX);
		wClass.cbWndExtra = NULL;
		wClass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
		wClass.hCursor = LoadCursor(0, IDC_ARROW);
		wClass.hIcon = LoadIcon(0, IDI_APPLICATION);
		wClass.hIconSm = LoadIcon(0, IDI_APPLICATION);
		wClass.hInstance = GetModuleHandle(NULL);
		wClass.lpfnWndProc = WndProc;
		wClass.lpszClassName = " ";
		wClass.lpszMenuName = " ";
		wClass.style = CS_VREDRAW | CS_HREDRAW;

		if (!RegisterClassEx(&wClass))
			printf("[Overlay] failed to register window class");

		tHwnd = FindWindow(NULL, OVERLAY_TARGET);

		if (tHwnd) {
			GetWindowRect(tHwnd, &tSize);
			width = tSize.right - tSize.left;
			height = tSize.bottom - tSize.top;
			hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, " ", " ", WS_POPUP, TRUE, TRUE, width, height, NULL, NULL, NULL, NULL);
			SetLayeredWindowAttributes(hwnd, NULL, 1.0f, LWA_ALPHA);
			SetLayeredWindowAttributes(hwnd, NULL, RGB(0, 0, 0), LWA_COLORKEY);
			ShowWindow(hwnd, SW_SHOW);

			InitDirectX();
		}

		MSG Message;
		for (;;) {
			if (PeekMessage(&Message, hwnd, 0, 0, PM_REMOVE)) {
				DispatchMessage(&Message);
				TranslateMessage(&Message);
			}
			Sleep(1);
		}
	}
}