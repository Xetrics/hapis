#include "drawing.h"

void Drawing::DrawString(char* String, int x, int y, int a, int r, int g, int b, ID3DXFont* font) {
	RECT FontPos;
	FontPos.left = x;
	FontPos.top = y;
	font->DrawTextA(0, String, strlen(String), &FontPos, DT_NOCLIP, D3DCOLOR_ARGB(a, r, g, b));
}

void Drawing::DrawFilledRectangle(IDirect3DDevice9Ex* p_Device, float x, float y, float w, float h, int a, int r, int g, int b) {
	D3DCOLOR color = D3DCOLOR_ARGB(a, r, g, b);
	D3DRECT rect = { x, y, w, h };
	p_Device->Clear(1, &rect, D3DCLEAR_TARGET | D3DCLEAR_TARGET, color, 0, 0);
}


void Drawing::DrawBorderBox(IDirect3DDevice9Ex* p_Device, int x, int y, int w, int h, int thickness, int a, int r, int g, int b)
{
	DrawFilledRectangle(p_Device, x, y, w, y + thickness, a, r, g, b); // x
	DrawFilledRectangle(p_Device, x, y, x + thickness, h, a, r, g, b); // y
	DrawFilledRectangle(p_Device, x, h, w, h + thickness, a, r, g, b); // w
	DrawFilledRectangle(p_Device, w, y, w + thickness, h + thickness, a, r, g, b); // h
}
