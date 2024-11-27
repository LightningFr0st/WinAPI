#pragma once

#include <vector>
#include <string>

#include <d2d1.h>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#include <dwrite.h>
#include <vector>
#include <string>


#define AlignPixel(x)  floorf((x)) + 0.5F

#include "Cell.h"
#include "Header.h"

class Table {
private:

	int gen = 1;

	IDWriteTextFormat** pTextFormatDef;

	std::vector<Cell*> editCells;

	HWND parent;
	ID2D1HwndRenderTarget* pRenderTarget;
	ID2D1SolidColorBrush* pBrush;

	FLOAT height, width;
	FLOAT cellH, cellW;

	Header header;
	std::vector<std::vector<Cell*>> cells;


public:
	Table(int N, int M, HWND window, ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush,
		IDWriteTextFormat** Def, IDWriteTextFormat** Header);

	void Draw();

	void AddRow(std::wstring templ);

	int nRows;
	int nCols;
	int rowsDisplay;

	void LMBHandle(int x, int y);
	void CtrlLMBHandle(int x, int y);
	void RMBHandle(int x, int y);
	void DLMBHandle(int x, int y);

	int startInd = 0;
};
