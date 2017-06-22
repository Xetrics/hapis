#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

namespace Drawing {
	void DrawString(char* String, int x, int y, int a, int r, int g, int b, ID3DXFont* font);
	void DrawFormattedString(const char* format, int x, int y, int a, int r, int g, int b, ID3DXFont* font, ...);
	void DrawFilledRectangle(IDirect3DDevice9Ex* p_Device, float x, float y, float w, float h, int a, int r, int g, int b);
	void DrawBorderBox(IDirect3DDevice9Ex* p_Device, int x, int y, int w, int h, int thickness, int a, int r, int g, int b);
	void DrawLine(IDirect3DDevice9Ex* p_Device, int bx, int by, int bw, D3DCOLOR color);
}