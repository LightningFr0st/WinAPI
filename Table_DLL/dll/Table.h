#pragma once

#include "Cell.h"
#include "Header.h"
#include <tuple>

class Table {
public:

	int gen = 1;

	IDWriteTextFormat** pTextFormatDef;

	HWND parent;
	ID2D1HwndRenderTarget* pRenderTarget;
	ID2D1SolidColorBrush* pBrush;

	FLOAT height, width;
	FLOAT cellH, cellW;

	Header* header;
	std::vector<std::vector<Cell*>> cells;


	TABLEDLL_API Table(int N, int M, HWND window, ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush,
		IDWriteTextFormat** Def, IDWriteTextFormat** Header);

	TABLEDLL_API void Draw();

	TABLEDLL_API void AddRow(std::wstring templ);

	TABLEDLL_API void FillColumn(int start, std::vector<std::wstring> arr);

	int nRows;
	int nCols;
	int rowsDisplay;

	TABLEDLL_API void LMBHandle(int x, int y);
	TABLEDLL_API void CtrlLMBHandle(int x, int y);
	TABLEDLL_API void RMBHandle(int x, int y);
	TABLEDLL_API void DLMBHandle(int x, int y);

	TABLEDLL_API void RenameHeader(std::vector<std::wstring> names);

	int startInd = 0;
};


extern "C" TABLEDLL_API void CreateTable(int N, int M, HWND window, ID2D1HwndRenderTarget * pRenderTarget, ID2D1SolidColorBrush * pBrush,
	IDWriteTextFormat * *Def, IDWriteTextFormat * *Header, void** ptr);

extern "C" TABLEDLL_API void AddRowF(std::wstring templ, void** ptr);

extern "C" TABLEDLL_API void Draw(void** ptr);

extern "C" TABLEDLL_API void LMBHandleW(int x, int y, void** ptr);
extern "C" TABLEDLL_API void CtrlLMBHandleW(int x, int y, void** ptr);
extern "C" TABLEDLL_API void RMBHandleW(int x, int y, void** ptr);
extern "C" TABLEDLL_API void DLMBHandleW(int x, int y, void** ptr);

extern "C" TABLEDLL_API int getStartInd(void** ptr);
extern "C" TABLEDLL_API int getRows(void** ptr);
extern "C" TABLEDLL_API int getDisp(void** ptr);

extern "C" TABLEDLL_API void Decr(void** ptr);
extern "C" TABLEDLL_API void Inc(void** ptr);	