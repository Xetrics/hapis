#include "drawing.h"
#include <cstdio>

#define PI 3.14159265

void Drawing::DrawString(char* String, int x, int y, int a, int r, int g, int b, ID3DXFont* font) {
	RECT FontPos;
	FontPos.left = x;
	FontPos.top = y;
	font->DrawTextA(0, String, strlen(String), &FontPos, DT_NOCLIP, D3DCOLOR_ARGB(a, r, g, b));
}

void Drawing::DrawFormattedString(const char* format, int x, int y, int a, int r, int g, int b, ID3DXFont* font, ...)
{
	char buffer[1000] = { 0 };
	va_list vl;
	va_start(vl, font);
	vsnprintf_s(buffer, sizeof(buffer), format, vl);
	va_end(vl);

	DrawString(buffer, x, y, a, r, g, b, font);
}

void Drawing::DrawFilledRectangle(IDirect3DDevice9Ex* p_Device, float x, float y, float w, float h, int a, int r, int g, int b) {
	D3DRECT rec = { x, y, x + w, y + h };
	p_Device->Clear(1, &rec, D3DCLEAR_TARGET | D3DCLEAR_TARGET, D3DCOLOR_ARGB(a, r, g, b), 0, 0);
}

void Drawing::DrawBorderBox(IDirect3DDevice9Ex* p_Device, int x, int y, int w, int h, int thickness, int a, int r, int g, int b)
{

	DrawFilledRectangle(p_Device, x, y, w, thickness, a, r, g, b);
	DrawFilledRectangle(p_Device, x, y, thickness, h, a, r, g, b);
	DrawFilledRectangle(p_Device, (x + w), y, thickness, h, a, r, g, b);
	DrawFilledRectangle(p_Device, x, y + h, w + thickness, thickness, a, r, g, b);
}

void Drawing::DrawLine(IDirect3DDevice9Ex* p_Device, int bx, int by, int bw, D3DCOLOR color)
{
	D3DRECT rec = { bx - bw, by, bx + bw, by + 1 };
	p_Device->Clear(1, &rec, D3DCLEAR_TARGET, color, 0, 0);
}