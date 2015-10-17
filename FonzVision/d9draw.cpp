#include "stdafx.h"
#include "d9draw.h"
#include "d3dx9math.h"

void D9Draw::DrawTextW(const wchar_t* text, unsigned long font, util::Color color, util::Vector2 pos) {
	RECT tpos;
	tpos.left = (long)pos.x;
	tpos.top = (long)pos.y;

	ID3DXFont* nativeFont = reinterpret_cast<ID3DXFont*>(font);
	nativeFont->DrawTextW(0, text, wcslen(text), &tpos, DT_NOCLIP, color.GetD3DColor());
}

void D9Draw::DrawTextA(const char* text, unsigned long font, util::Color color, util::Vector2 pos) {
	RECT tpos;
	tpos.left = (long)pos.x;
	tpos.top = (long)pos.y;

	ID3DXFont* nativeFont = reinterpret_cast<ID3DXFont*>(font);
	nativeFont->DrawTextA(0, text, strlen(text), &tpos, DT_NOCLIP, color.GetD3DColor());
}

int D9Draw::TextWidth(const wchar_t* text, unsigned long font) {
	ID3DXFont* nativeFont = reinterpret_cast<ID3DXFont*>(font);

	RECT sizeRect = { 0, 0, 0, 0 };
	nativeFont->DrawTextW(NULL, text, wcslen(text), &sizeRect, DT_CALCRECT, D3DCOLOR_XRGB(0, 0, 0));
	return sizeRect.right - sizeRect.left;
}

unsigned long D9Draw::CreateFontA(const char* name, int size, int weight) {
	ID3DXFont* nativeFont = 0;
	D3DXCreateFontA(this->device, size, 0, 0, 0, false, 1, OUT_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, name, &nativeFont);
	return reinterpret_cast<long>(nativeFont);
}

void D9Draw::DrawFilledRect(int x1, int y1, int x2, int y2, util::Color color) {
	int w = x2 - x1, h = y2 - y1;

	D3DXVECTOR2 vLine[2];
	this->line->SetWidth(w);

	vLine[0].x = x1 + w / 2;
	vLine[0].y = y1;
	vLine[1].x = x1 + w / 2;
	vLine[1].y = y1 + h;

	this->line->Begin();
	this->line->Draw(vLine, 2, color.GetD3DColor());
	this->line->End();
}

void D9Draw::DrawRect(int x1, int y1, int x2, int y2, util::Color color) {

	D3DXVECTOR2 points[5];
	points[0] = D3DXVECTOR2(x1, y1);
	points[1] = D3DXVECTOR2(x2, y1);
	points[2] = D3DXVECTOR2(x2, y2);
	points[3] = D3DXVECTOR2(x1, y2);
	points[4] = D3DXVECTOR2(x1, y1);
	this->line->SetWidth(1);
	this->line->Draw(points, 5, color.GetD3DColor());
}

void D9Draw::DrawLine(int x1, int y1, int x2, int y2, util::Color color) {
	D3DXVECTOR2 lineVec[2];
	lineVec[0].x = x1;
	lineVec[0].y = y1;

	lineVec[1].x = x2;
	lineVec[1].y = y2;

	this->line->SetWidth(1);
	this->line->Draw(lineVec, 2, color.GetD3DColor());
}
