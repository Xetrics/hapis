#include "overlay.h"
#include "main.h"

namespace Overlay {
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	const MARGINS margin = { 0, 0, width, height };
	HWND hwnd;
	HWND tHwnd;
	RECT tSize;

	IDirect3D9Ex* p_Object;
	IDirect3DDevice9Ex* p_Device;
	D3DPRESENT_PARAMETERS p_Params;
	ID3DXFont* Font;

	void render() {
		if (GetForegroundWindow() == tHwnd)
		{
			p_Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
			p_Device->BeginScene();

			/* Draw Watermark */
			Drawing::DrawString("hapis.exe", 25, 10, 255, 255, 255, 255, Font);
			Drawing::DrawFormattedString("width: %d", 25, 35, 255, 255, 255, 255, Font, width);
			Drawing::DrawFormattedString("height: %d", 25, 60, 255, 255, 255, 255, Font, height);
			Drawing::DrawFormattedString("players: %d", 25, 85, 255, 255, 255, 255, Font, players.size());
			if (localPlayer) Drawing::DrawFormattedString("pos: %f, %f, %f", 25, 110, 255, 255, 255, 255, Font, localPlayer->pos.x, localPlayer->pos.y, localPlayer->pos.z);
			if (localPlayer) Drawing::DrawFormattedString("rot: %f, %f, %f", 25, 135, 255, 255, 255, 255, Font, localPlayer->rot.x, localPlayer->rot.y, localPlayer->rot.z);

			/* Draw Crosshair */
			if (settings->crosshair) {
				Drawing::DrawFilledRectangle(p_Device, (width / 2) - (CROSSHAIR_THICKNESS / 2), (height / 2) - (CROSSHAIR_WIDTH / 2), CROSSHAIR_THICKNESS, CROSSHAIR_WIDTH, 255, 0, 255, 0);
				Drawing::DrawFilledRectangle(p_Device, (width / 2) - (CROSSHAIR_WIDTH / 2), (height / 2) - (CROSSHAIR_THICKNESS / 2), CROSSHAIR_WIDTH, CROSSHAIR_THICKNESS, 255, 0, 255, 0);
			}

			/* Draw ESP Boxes */
			for (auto player : players) {
				Rust::Vector3 pos;
				bool visible = Math::WorldToScreen(player.second, pos, localPlayer->viewMatrix, width, height);
				Drawing::DrawString("player", pos.x, pos.y, 255, 255, 255, 255, Font);
				//if (visible)  Drawing::DrawString("player", pos.x, pos.y, 255, 255, 255, 255, Font);
			}

			/* Draw Scene */
			p_Device->EndScene();
			p_Device->PresentEx(0, 0, 0, 0, 0);
		}
	}

	/* Moves the invisible window to the position and size of Rust */
	void setWindowToTarget() {
		for (;;) {
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
				ShowWindow(hwnd, SW_SHOW);
			}

			if (GetForegroundWindow() == tHwnd)
				ShowWindow(hwnd, SW_SHOW);
			else
				ShowWindow(hwnd, SW_HIDE);

			Sleep(WINDOW_ADJUST_RATE);
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

		D3DXCreateFont(p_Device, 20, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &Font);
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

		while (!FindWindow(FALSE, OVERLAY_TARGET)) { Sleep(20); }

		tHwnd = FindWindow(FALSE, OVERLAY_TARGET);

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
		}
	}
}