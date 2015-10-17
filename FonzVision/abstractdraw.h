#include "stdafx.h"
#include "math.h"
#pragma once

class AbstractDraw {

public:
	virtual void DrawTextW(const wchar_t* text, unsigned long font, util::Color color, util::Vector2 pos) = 0;
	virtual void DrawTextA(const char* text, unsigned long font, util::Color color, util::Vector2 pos) = 0;

	virtual int TextWidth(const wchar_t* text, unsigned long font) = 0;
	virtual unsigned long CreateFontA(const char* name, int size, int weight) = 0;

	virtual void DrawFilledRect(int x1, int y1, int x2, int y2, util::Color color) = 0;
	virtual void DrawRect(int x1, int y1, int x2, int y2, util::Color color) = 0;

	virtual void DrawLine(int x1, int y1, int x2, int y2, util::Color color) = 0;

};