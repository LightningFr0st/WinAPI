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
#include <set>

#define BTN_OK 100
#define BTN_QUIT 101
#define BTN_ADD 102
#define BTN_BROWSE 103

#include "MainWindow.h"

struct FontData {
	TCHAR filesFont[256];
	FLOAT filesHeight;

	TCHAR paramFont[256];
	FLOAT paramHeight;
	
	TCHAR headersFont[256];
	FLOAT headerHeight;
};


class FontWindow : public BaseWindow<FontWindow>
{
	HWND hwndList_files;
	HWND hwndFileFontSize;

	HWND hwndList_sizes;
	HWND hwndParamFontSize;

	HWND hwndList_headers;
	HWND hwndHeaderFontSize;

	HWND hwndButtonOK;
	HWND hwndButtonQuit;
	HWND hwndButtonAdd;
	HWND hwndButtonBrowse;

	HWND hwndEdit;

	HWND hwndParent;

	ID2D1Factory* pFactory;
	ID2D1HwndRenderTarget* pRenderTarget;
	ID2D1SolidColorBrush* pBrush;

	IDWriteFactory5* pDWriteFactory;

	IDWriteFontCollection1** pFontCollection;
	IDWriteFontSet** pFontSet;

	HRESULT AddFontToCollection(const WCHAR* filePath);

	IDWriteTextFormat* pTextFormat_files;
	IDWriteTextFormat* pTextFormat_size;
	IDWriteTextFormat* pTextFormat_header;

	std::vector<std::wstring> systemFonts;
	std::vector<std::wstring> customFonts;

	HRESULT ChangeFont(const WCHAR* newFont, IDWriteTextFormat** formatToChange, FLOAT height, IDWriteFontCollection* fontCollect = NULL);

	static BOOL CALLBACK EnumFamCallBackEx(const LOGFONT* lplf, const TEXTMETRIC* lpntm, DWORD FontType, LPARAM lParam);
	
	void AddSystemFonts();
	void AddCustomFont(UINT32 ind);

	void    CalculateLayout();
	HRESULT CreateGraphicsResources();
	HRESULT CreateFontResources();

	void CreateButton();
	void CreateDropLists();
	void CreateEditControl();

	void    DiscardGraphicsResources();
	void    OnPaint();
	void    Resize();

	std::set<std::wstring> addedFonts;

	FLOAT xm = 0, ym = 0;
	FLOAT height = 0, width = 0;

public:

	FontWindow() : pFactory(NULL), pRenderTarget(NULL), pBrush(NULL)
	{
		hwndList_files = NULL;
		hwndFileFontSize = NULL;
		hwndList_sizes = NULL;
		hwndParamFontSize = NULL;
		hwndList_headers = NULL;
		hwndHeaderFontSize = NULL;

		hwndButtonOK = NULL;
		hwndButtonQuit = NULL;
		hwndButtonAdd = NULL;
		hwndButtonBrowse = NULL;

		hwndEdit = NULL;

		hwndParent = NULL;

		pDWriteFactory = NULL;

		pTextFormat_files = NULL;
		pTextFormat_size = NULL;
		pTextFormat_header = NULL;
		
		pFontCollection = NULL;
		pFontSet = NULL;

		pFactory = NULL;
		pRenderTarget = NULL;
		pBrush = NULL;

		pDWriteFactory = NULL;
	}

	friend HFONT CreateHFONTFromTextFormat(IDWriteTextFormat* pTextFormat);

	PCWSTR  ClassName() const { return L"Child Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};