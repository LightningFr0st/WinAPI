#pragma once 

#include <d2d1.h>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#include <dwrite.h>
#include <vector>
#include <string>

#include "Cell.h"

#define WM_LMB WM_USER + 3
#define WM_DLMB WM_USER + 4
#define WM_RMB WM_USER + 5
#define WM_CTRLLMB WM_USER + 6
#define WM_LMB_H WM_USER + 7

class Header {
public:

	HWND parent;

	float height, width;

	void LMBHandle(int column);

	std::vector<Cell*> columns;

	Header(int lenght, HWND hwnd, float h, float w, IDWriteTextFormat** textFormat) : parent(hwnd)
		, height(h), width(w) {
		columns.resize(lenght);
		columns[0] = new Cell(0, 0, parent, height, width, textFormat, L"", false);
		for (int i = 1; i < lenght; i++) {
			columns[i] = new Cell(0, i, parent, height, width, textFormat, L"");
		}
	}

	void Draw(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush, FLOAT cellW, FLOAT cellH);
};