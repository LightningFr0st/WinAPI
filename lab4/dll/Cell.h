#pragma once 

#ifdef TABLEDLL_EXPORTS
#define TABLEDLL_API __declspec(dllexport)
#else
#define TABLEDLL_API __declspec(dllimport)
#endif

#include <d2d1.h>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#include <dwrite.h>
#include <vector>
#include <string>

#define AlignPixel(x)  floorf((x)) + 0.5F

#define WM_LMB WM_USER + 3
#define WM_DLMB WM_USER + 4
#define WM_RMB WM_USER + 5
#define WM_CTRLLMB WM_USER + 6
#define WM_LMB_H WM_USER + 7

extern HMODULE g_hModule;

extern "C" TABLEDLL_API HFONT CreateHFONTFromTextFormat(IDWriteTextFormat * pTextFormat);

class Cell {
public:

	int row;
	int col;

	FLOAT height, width;

	HWND parent;

	HWND hwndEdit;

	bool isStatic;

	WNDPROC OriginalEditProc;

	std::wstring content;

	IDWriteTextFormat** textFormat;

	Cell(int r, int c, HWND hwndP, FLOAT H, FLOAT W, IDWriteTextFormat** Def, std::wstring str = L"Static", bool pStat = true)
		:row(r), col(c), content(str), isStatic(pStat), parent(hwndP), height(H), width(W), textFormat(Def)  {
	}

	void LMBHandle();
	void CtrlLMBHandle();
	void RMBHandle();
	void DLMBHandle();

	void Draw(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush,
		FLOAT left, FLOAT top, FLOAT right, FLOAT bottom, DWRITE_TEXT_ALIGNMENT align);
};