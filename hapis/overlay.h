#pragma once

#include <stdio.h>

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#define OVERLAY_TARGET "Rust" /* window name */

namespace Overlay {
	void DrawFilledRectangle(float x, float y, float w, float h, int a, int r, int g, int b);
	void DrawBorderBox(int x, int y, int w, int h, int thickness, int a, int r, int g, int b);
	void DrawString(char* String, int x, int y, int a, int r, int g, int b, ID3DXFont* font);
	void render();
	void setWindowToTarget();
	void InitDirectX();
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void Init();
}