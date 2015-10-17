#include "stdafx.h"
#include "abstractdraw.h"
#include "d3dx9core.h"
#include "d3d9.h"
#pragma once

class D9Draw : public AbstractDraw {

private:
	IDirect3DDevice9Ex* device;
	ID3DXLine* line;

public:


	D9Draw(IDirect3DDevice9Ex* device) {
		this->device = device;
		D3DXCreateLine(device, &line);
	}

	void DrawTextW(const wchar_t* text, unsigned long font, util::Color color, util::Vector2 pos);
	void DrawTextA(const char* text, unsigned long font, util::Color color, util::Vector2 pos);

	int TextWidth(const wchar_t* text, unsigned long font);
	unsigned long CreateFontA(const char* name, int size, int weight);

	void DrawFilledRect(int x1, int y1, int x2, int y2, util::Color color);
	void DrawRect(int x1, int y1, int x2, int y2, util::Color color);

	void DrawLine(int x1, int y1, int x2, int y2, util::Color color);

};
