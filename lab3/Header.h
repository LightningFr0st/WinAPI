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


class Header {
public:

	HWND parent;

	IDWriteTextFormat** pTextFormatHeader;

	void LMBHandle(int column);

	std::vector<std::wstring> columns;

	Header(int lenght, HWND hwnd, IDWriteTextFormat** Header) : pTextFormatHeader(Header), parent(hwnd) {
		columns.resize(lenght);
		for (int i = 0; i < lenght; i++) {
			columns[i] = L"Header" + std::to_wstring(i + 1);
		}
	}

	void Draw(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush, FLOAT cellW, FLOAT cellH);
};