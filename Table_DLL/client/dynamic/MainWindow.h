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

#define WM_LMB WM_USER + 3
#define WM_DLMB WM_USER + 4
#define WM_RMB WM_USER + 5
#define WM_CTRLLMB WM_USER + 6
#define WM_LMB_H WM_USER + 7


typedef void(*ContructorP)(int N, int M, HWND window, ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush,
	IDWriteTextFormat** Def, IDWriteTextFormat** Header, void** ptr);
typedef void(*DrawP)(void** ptr);
typedef void(*AddRowP)(std::wstring templ, void** ptr);
typedef void(*HadleP)(int x, int y, void** ptr);
typedef int(*Getter)(void** ptr);

#define AlignPixel(x)  floorf((x)) + 0.5F

class MainWindow : public BaseWindow<MainWindow>
{
public:

	bool pendingSingleClick = false;

	void* MainTableV = NULL;

	HINSTANCE hDLL;

	ContructorP pCreateTable;
	DrawP pDrawTable;
	AddRowP pAddRow;
	HadleP pLMBHandle;
	HadleP pCtrlLMBHandle;
	HadleP pRMBHandle;
	HadleP pDLMBHandle;
	Getter pGetStart;

	Getter pGetN;
	Getter pGetD;
	
	Getter pDecr;
	Getter pInc;


	ID2D1Factory* pFactory;
	ID2D1HwndRenderTarget* pRenderTarget;
	ID2D1SolidColorBrush* pBrush;

	IDWriteFactory5* pDWriteFactory;

	IDWriteTextFormat* pTextFormatDef;
	IDWriteTextFormat* pTextFormatHeader;

	IDWriteFontCollection1* pFontCollection;
	IDWriteFontSet* pFontSet;

	DWRITE_TRIMMING trimmingOptions = {};

	HRESULT ChangeFont(const WCHAR* newFont, IDWriteTextFormat** formatToChange, FLOAT height, DWRITE_TEXT_ALIGNMENT align, IDWriteFontCollection* fontCollect = NULL);

	void    CalculateLayout();
	HRESULT CreateGraphicsResources();
	HRESULT CreateFontResources();

	void CreateEditControl();
	void CreateButton();

	void LoadDll();

	void    DiscardGraphicsResources();
	void    RemoveCustomFonts();
	void    OnPaint();
	void    Resize();

	std::vector<LPWSTR> customFontsPathes;
	std::vector<std::wstring> customFontsNames;

	FLOAT xm = 0, ym = 0;
	FLOAT height = 0, width = 0;

	MainWindow() {
		hDLL = NULL;

		pCreateTable = NULL;
		pDrawTable = NULL;
		pAddRow = NULL;
		pLMBHandle = NULL;
		pCtrlLMBHandle = NULL;
		pRMBHandle = NULL;
		pDLMBHandle = NULL;
		pGetStart = NULL;

		pFactory = NULL;
		pRenderTarget = NULL;
		pBrush = NULL;

		pGetN = NULL;
		pGetD = NULL;

		pDecr = NULL;
		pInc = NULL;

		pDWriteFactory = NULL;

		pTextFormatDef = NULL;
		pTextFormatHeader = NULL;

		pFontCollection = NULL;
		pFontSet = NULL;
	}

	void CenterChildWindow(HWND hwndChild, HWND hwndParent);

	void InitArray(LPCWSTR start_dir);
	size_t HoarePartition(size_t p, size_t r);
	void QuickSort(size_t start, size_t end);
	void FinilizeArray();
	LRESULT SendDefFonts(HWND child);

	PCWSTR  ClassName() const { return L"Base Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

HFONT CreateHFONTFromTextFormat(IDWriteTextFormat* pTextFormat);