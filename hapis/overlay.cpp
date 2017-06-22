#include "overlay.h"

Rust::Vector3 RotatePoint(Rust::Vector3 origin, Rust::Vector3 point, float radian) {
	float s = sin(radian);
	float c = cos(radian);

	// translate point back to origin:  
	point.x -= origin.x;
	point.y -= origin.y;

	// rotate point   
	float xnew = point.x * c - point.y * s;
	float ynew = point.x * s + point.y * c;

	// translate point back to global coords:
	Rust::Vector3 TranslatedPoint;
	TranslatedPoint.x = xnew + origin.x;
	TranslatedPoint.y = ynew + origin.y;

	return TranslatedPoint;
}

float wrapAngleTo180(float yaw) {
	if (yaw >= 180.0f) 
		yaw -= 360.0f;
	if (yaw < -180.0f)
		yaw += 360.0f;
	return yaw;
}

float toRadians(float degrees) {
	return degrees * (PI / 180.f);
}

Rust::Vector3 RotatePoint(Rust::Vector3 pointToRotate, Rust::Vector3 centerPoint, float angle, bool angleInRadians = false)
{
	if (!angleInRadians)
		angle = (float)(angle * (PI / 180.f));
	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);
	Rust::Vector3 returnVec = {
		cosTheta * (pointToRotate.x - centerPoint.x) - sinTheta * (pointToRotate.y - centerPoint.y),
		sinTheta * (pointToRotate.x - centerPoint.x) + cosTheta * (pointToRotate.y - centerPoint.y),
		0
	};
	returnVec = returnVec + centerPoint;
	return returnVec;
}

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

	bool showGui = false;

	void render() {
		HWND fHwnd = GetForegroundWindow();
		if (fHwnd == tHwnd || (fHwnd == hwnd && showGui))
		{
			p_Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
			p_Device->BeginScene();

			/* Draw Watermark */
			Drawing::DrawString("hapis.exe", 25, 10, 255, 255, 255, 255, Font);

			if (settings->debug) {
				Drawing::DrawFormattedString("width: %d", 25, 35, 255, 255, 255, 255, Font, width);
				Drawing::DrawFormattedString("height: %d", 25, 60, 255, 255, 255, 255, Font, height);
				Drawing::DrawFormattedString("players: %d", 25, 85, 255, 255, 255, 255, Font, players.size());
				if (localPlayer) Drawing::DrawFormattedString("pos: %f, %f, %f", 25, 110, 255, 255, 255, 255, Font, localPlayer->pos.x, localPlayer->pos.y, localPlayer->pos.z);
				if (localPlayer) Drawing::DrawFormattedString("rot: %f, %f, %f", 25, 135, 255, 255, 255, 255, Font, localPlayer->rot.x, localPlayer->rot.y, localPlayer->rot.z);
			}

			/* Draw Crosshair */
			if (settings->crosshair) {
				Drawing::DrawFilledRectangle(p_Device, (width / 2) - (CROSSHAIR_THICKNESS / 2), (height / 2) - (CROSSHAIR_WIDTH / 2), CROSSHAIR_THICKNESS, CROSSHAIR_WIDTH, 255, 0, 255, 0);
				Drawing::DrawFilledRectangle(p_Device, (width / 2) - (CROSSHAIR_WIDTH / 2), (height / 2) - (CROSSHAIR_THICKNESS / 2), CROSSHAIR_WIDTH, CROSSHAIR_THICKNESS, 255, 0, 255, 0);
			}

			/* Draw ESP Boxes */
			if (settings->esp) {
				Rust::Vector3 pos;
				for (auto player : players) {
					Rust::Vector3 feetPos = player.second;
					Rust::Vector3 headPos = feetPos;
					Rust::Vector3 screenFeetPos = { 0 }, screenHeadPos = { 0 };
			
					headPos.y += 1.6f;

					bool visible = Math::World2Screen(localPlayer->pos, localPlayer->rot, FOV, { 0, 1.5f, 0 }, headPos, screenHeadPos, width, height);
					visible = visible && Math::World2Screen(localPlayer->pos, localPlayer->rot, FOV, { 0, 1.5f, 0 }, feetPos, screenFeetPos, width, height);

					Drawing::DrawString("feet", screenFeetPos.x, screenFeetPos.y, 255, 255, 0, 0, Font);
					Drawing::DrawString("head", screenHeadPos.x, screenHeadPos.y, 255, 255, 0, 0, Font);

					float distance = Math::Get3dDistance(localPlayer->pos, feetPos);
					float h = feetPos.x - headPos.x;
					float w = h / 2.0f;

					if (visible && distance < 963.0f)
						Drawing::DrawBorderBox(p_Device, screenFeetPos.x, screenHeadPos.y, w + 2.0f, h, 2, 255, 255, 255, 255);
				}
			}

			/* Draw Radar */
			if (settings->radar) {
				int boxX = 0, boxY = 0;
				Drawing::DrawFilledRectangle(p_Device, width - 10, 20, 200, 200, 100, 0, 0, 0);
				Drawing::DrawFilledRectangle(p_Device, width - 112, 122, 4, 4, 255, 255, 255, 255);
				
				//Rust::Vector3 myPos = { 0, 0, 0 };
				//Rust::Vector3 theirPos = { 50, 0, -50 };

				for (auto player : players) {
					float xDif = localPlayer->pos.x - player.second.x, zDif = localPlayer->pos.z - player.second.z;
					float plyDistance = sqrt(pow(xDif, 2) + pow(zDif, 2));
					float angleDiff = wrapAngleTo180((localPlayer->rot.y - 90) - ((atan2(xDif, zDif) * 180.0f) / PI));
					float finalX = cos(toRadians(angleDiff)) * plyDistance;
					float finalY = -sin(toRadians(angleDiff)) * plyDistance;
					Drawing::DrawFilledRectangle(p_Device, width - (finalX + 114), finalY + 124, 4, 4, 255, 255, 255, 255);
				}
			}

			/* ImGui */
			ImGui_ImplDX9_NewFrame();

			if (showGui) {
				ImGui::Begin("Hapis", &showGui, ImVec2(300, 250), 0.75f);
				{
					if (ImGui::TreeNodeEx("Visuals", ImGuiTreeNodeFlags_CollapsingHeader)) {
						ImGui::Checkbox("Crosshair", &settings->crosshair);
						ImGui::Checkbox("ESP", &settings->esp);
						ImGui::Checkbox("Radar", &settings->radar);
						if (ImGui::TreeNodeEx("Weather", ImGuiTreeNodeFlags_CollapsingHeader)) {
							if(!settings->weather->freeze_time)
								ImGui::Checkbox("Always Day", &settings->weather->always_day);
							if(!settings->weather->always_day)
								ImGui::Checkbox("Freeze Time", &settings->weather->freeze_time);
							ImGui::Checkbox("No Rain", &settings->weather->no_rain);
							ImGui::Checkbox("No Fog", &settings->weather->no_fog);
							ImGui::Checkbox("No Clouds", &settings->weather->no_clouds);
							ImGui::Checkbox("No Wind", &settings->weather->no_wind);
							ImGui::TreePop();
						}
						ImGui::TreePop();
					}
					if (ImGui::TreeNodeEx("Misc", ImGuiTreeNodeFlags_CollapsingHeader)) {
						ImGui::Checkbox("Debug Mode", &settings->debug);
						ImGui::TreePop();
					}
				}
				ImGui::End(); //End main window
			}

			/* Draw Scene */
			ImGui::Render();
			p_Device->EndScene();
			p_Device->PresentEx(0, 0, 0, 0, 0);
		}

	}


	void ToggleGui() {
		showGui = !showGui;

		long style = GetWindowLong(hwnd, GWL_EXSTYLE);

		if (showGui) {
			style &= ~WS_EX_LAYERED;
			SetWindowLong(hwnd, GWL_EXSTYLE, style);
			SetForegroundWindow(hwnd);
		}
		else {
			style |= WS_EX_LAYERED;
			SetWindowLong(hwnd, GWL_EXSTYLE, style);
			SetForegroundWindow(tHwnd);
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

			if (GetForegroundWindow() == tHwnd || GetForegroundWindow() == hwnd)
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

		ImGui_ImplDX9_Init(hwnd, p_Device);

		// wrapped in brackets so you can minimize it
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Alpha = 1.0;
			style.ChildWindowRounding = 0;
			style.WindowRounding = 0;
			style.GrabRounding = 0;
			style.GrabMinSize = 20;
			style.FrameRounding = 0;


			style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.00f, 0.40f, 0.41f, 1.00f);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 1.00f, 1.00f, 0.65f);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.44f, 0.80f, 0.80f, 0.18f);
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.44f, 0.80f, 0.80f, 0.27f);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.44f, 0.81f, 0.86f, 0.66f);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.18f, 0.21f, 0.73f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.27f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.29f, 0.30f, 0.71f);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.44f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_ComboBg] = ImVec4(0.16f, 0.24f, 0.22f, 0.60f);
			style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 0.68f);
			style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.36f);
			style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
			style.Colors[ImGuiCol_Button] = ImVec4(0.00f, 0.65f, 0.65f, 0.46f);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.01f, 1.00f, 1.00f, 0.43f);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.62f);
			style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 1.00f, 0.33f);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.42f);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
			style.Colors[ImGuiCol_Column] = ImVec4(0.00f, 0.50f, 0.50f, 0.33f);
			style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.00f, 0.50f, 0.50f, 0.47f);
			style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.00f, 0.70f, 0.70f, 1.00f);
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_CloseButton] = ImVec4(0.00f, 0.78f, 0.78f, 0.35f);
			style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.00f, 0.78f, 0.78f, 0.47f);
			style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.00f, 0.78f, 0.78f, 1.00f);
			style.Colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 1.00f, 0.22f);
			style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.04f, 0.10f, 0.09f, 0.51f);
		}
	}

	/* Fuck WinAPI */

	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		ImGuiIO& io = ImGui::GetIO();

		switch (msg)
		{
		case WM_PAINT:
		{
			static bool bKeyPressed = false;
			bool bKeyPressedThisFrame = (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;

			if (bKeyPressedThisFrame != bKeyPressed &&
				bKeyPressedThisFrame)
			{
				ToggleGui();
			}

			bKeyPressed = bKeyPressedThisFrame;

			render();
			break;
		}
		case WM_CREATE:
			DwmExtendFrameIntoClientArea(hWnd, &margin);
			break;

		case WM_DESTROY:
			PostQuitMessage(1);
			return 0;
		case WM_SIZE:
			if (p_Device != NULL && wParam != SIZE_MINIMIZED)
			{
				ImGui_ImplDX9_InvalidateDeviceObjects();
				p_Params.BackBufferWidth = LOWORD(lParam);
				p_Params.BackBufferHeight = HIWORD(lParam);
				HRESULT hr = p_Device->Reset(&p_Params);
				if (hr == D3DERR_INVALIDCALL)
					IM_ASSERT(0);
				ImGui_ImplDX9_CreateDeviceObjects();
			}
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_LBUTTONDOWN:
			io.MouseDown[0] = true;
			break;
		case WM_LBUTTONUP:
			io.MouseDown[0] = false;
			break;
		case WM_RBUTTONDOWN:
			io.MouseDown[1] = true;
			break;
		case WM_RBUTTONUP:
			io.MouseDown[1] = false;
			break;
		case WM_MBUTTONDOWN:
			io.MouseDown[2] = true;
			break;
		case WM_MBUTTONUP:
			io.MouseDown[2] = false;
			break;
		case WM_MOUSEWHEEL:
			io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
			break;
		case WM_MOUSEMOVE:
			io.MousePos.x = (signed short)(lParam);
			io.MousePos.y = (signed short)(lParam >> 16);
			break;
		case WM_KEYDOWN:
			if (wParam < 256)
				io.KeysDown[wParam] = 1;

			break;
		case WM_KEYUP:
			if (wParam < 256)
				io.KeysDown[wParam] = 0;
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
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
			UpdateWindow(hwnd);
			InitDirectX();
		}

		MSG Message;
		while (true)
		{
			if (GetMessage(&Message, hwnd, 0, 0))
			{
				DispatchMessage(&Message);
				TranslateMessage(&Message);
			}
		}
	}
}