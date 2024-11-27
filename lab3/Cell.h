#include <d2d1.h>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#include <dwrite.h>
#include <vector>
#include <string>

#define WM_LMB WM_USER + 3
#define WM_DLMB WM_USER + 4
#define WM_RMB WM_USER + 5
#define WM_CTRLLMB WM_USER + 6
#define WM_LMB_H WM_USER + 7

class Cell {
public:

	int row;
	int col;

	HWND parent;

	bool isStatic;

	std::wstring content;

	Cell(int r, int c, HWND hwndP, std::wstring str = L"Static", bool pStat = true)
		:row(r), col(c), content(str), isStatic(pStat), parent(hwndP) {
	}

	void LMBHandle();
	void CtrlLMBHandle();
	void RMBHandle();
	void DLMBHandle();

	void Draw(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush,
		FLOAT left, FLOAT top, FLOAT right, FLOAT bottom, IDWriteTextFormat** Def);
};