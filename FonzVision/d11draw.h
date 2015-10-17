#include "stdafx.h"
#include "abstractdraw.h"
#include "D3D11.h"
#pragma once

class D11Draw : public AbstractDraw {

private:
	ID3D11Device* device;

public:


	D11Draw(ID3D11Device* device) {
		this->device = device;
	}

	void DrawTextW(const wchar_t* text, unsigned long font, util::Color color, util::Vector2 pos);
	void DrawTextA(const char* text, unsigned long font, util::Color color, util::Vector2 pos);

	int TextWidth(const wchar_t* text, unsigned long font);
	unsigned long CreateFontA(const char* name, int size, int weight);

	void DrawFilledRect(int x1, int y1, int x2, int y2, util::Color color);
	void DrawRect(int x1, int y1, int x2, int y2, util::Color color);

	void DrawLine(int x1, int y1, int x2, int y2, util::Color color);

};
