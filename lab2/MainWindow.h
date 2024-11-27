#pragma once

#include <d2d1.h>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#include <dwrite.h>
#include <dwrite_3.h>
#include <d2d1helper.h>
#include <vector>
#include <string>
#include <map>

#include "BaseWindow.h"

class MainWindow : public BaseWindow<MainWindow>
{
	HWND hwndEdit;
	static LRESULT EditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	WNDPROC OriginalEditProc;

	ID2D1Factory* pFactory;
	ID2D1HwndRenderTarget* pRenderTarget;
	ID2D1SolidColorBrush* pBrush;

	IDWriteFactory5* pDWriteFactory;

	IDWriteTextFormat* pTextFormat_files;
	IDWriteTextFormat* pTextFormat_size;
	IDWriteTextFormat* pTextFormat_header;

	IDWriteFontCollection1* pFontCollection;
	IDWriteFontSet* pFontSet;

	DWRITE_TRIMMING trimmingOptions = {};

	//UINT32 cTextLength;

	HRESULT ChangeFont(const WCHAR* newFont, IDWriteTextFormat** formatToChange, FLOAT height, DWRITE_TEXT_ALIGNMENT align, IDWriteFontCollection* fontCollect = NULL);

	void    CalculateLayout();
	HRESULT CreateGraphicsResources();
	HRESULT CreateFontResources();

	void CreateEditControl();
	void CreateButton();

	void    DiscardGraphicsResources();
	void    RemoveCustomFonts();
	void    OnPaint();
	void    Resize();

	FLOAT xm = 0, ym = 0;
	FLOAT height = 0, width = 0;

	int start_index = 0;

	std::vector<std::pair<std::wstring, std::wstring>> start_arr;
	std::vector<std::pair<std::wstring, std::wstring>> end_arr;

	std::vector<LPWSTR> customFontsPathes;
	std::vector<std::wstring> customFontsNames;

	std::map<std::wstring, std::pair<UINT32, UINT32>> repeatData;

public:

	MainWindow() {
		pFactory = NULL;
		pRenderTarget = NULL;
		pBrush = NULL;

		pDWriteFactory = NULL;

		pTextFormat_files = NULL;
		pTextFormat_size = NULL;
		pTextFormat_header = NULL;

		pFontCollection = NULL;
		pFontSet = NULL;

		hwndEdit = NULL;
		OriginalEditProc = NULL;
	}

	void CenterChildWindow(HWND hwndChild, HWND hwndParent);

	void InitArray(LPCWSTR start_dir);
	size_t HoarePartition(size_t p, size_t r);
	void QuickSort(size_t start, size_t end);
	void FinilizeArray();
	LRESULT SendDefFonts(HWND child);
	friend HFONT CreateHFONTFromTextFormat(IDWriteTextFormat* pTextFormat);

	PCWSTR  ClassName() const { return L"Base Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};