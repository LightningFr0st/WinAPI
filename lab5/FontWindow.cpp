#pragma once
#include "FontWindow.h"
#include <CommCtrl.h>
#include <shobjidl.h> 

PWSTR GetFilePath();

std::wstring GetFontName(IDWriteLocalizedStrings** fontNames) {
	HRESULT hr;
	UINT32 index = 0;
	BOOL exists = FALSE;
	wchar_t localeName[LOCALE_NAME_MAX_LENGTH];

	int defaultLocaleSuccess = GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH);
	if (defaultLocaleSuccess)
	{
		hr = (*fontNames)->FindLocaleName(localeName, &index, &exists);
	}
	if (!exists) {
		index = 0;
	}

	UINT32 length = 0;
	hr = (*fontNames)->GetStringLength(index, &length);
	if (FAILED(hr)) {
		return L"";
	}
	std::wstring fontName(length + 1, L'\0');
	hr = (*fontNames)->GetString(index, &fontName[0], length + 1);
	if (FAILED(hr)) {
		return L"";
	}

	return fontName;
}

void FontWindow::CalculateLayout()
{
	if (pRenderTarget != NULL)
	{
		D2D1_SIZE_F size = pRenderTarget->GetSize();
		xm = size.width / 2;
		ym = size.height / 2;
		height = size.height;
		width = size.width;

		MoveWindow(hwndButtonOK, (static_cast<int>(width) / 3) - 50, static_cast<int>(height) - 60, 100, 30, TRUE);
		MoveWindow(hwndButtonQuit, (2 * static_cast<int>(width) / 3) - 50, static_cast<int>(height) - 60, 100, 30, TRUE);
		MoveWindow(hwndButtonAdd, 10, static_cast<int>(height) / 2 + 100, 100, 30, TRUE);
		MoveWindow(hwndButtonBrowse, 120, static_cast<int>(height) / 2 + 100, 100, 30, TRUE);

		MoveWindow(hwndEdit, 10, static_cast<int>(height) / 2 + 50, 450, 30, TRUE);
		MoveWindow(hwndDefFontSize, 420, 50, 50, 30, TRUE);
		MoveWindow(hwndHeaderFontSize, 420, 125, 50, 30, TRUE);
	}
}

HRESULT FontWindow::CreateGraphicsResources()
{
	HRESULT hr = S_OK;
	if (pRenderTarget == NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&pRenderTarget);

		pRenderTarget->SetDpi(96.0f, 96.0f);

		if (SUCCEEDED(hr))
		{
			const D2D1_COLOR_F color = D2D1::ColorF(0, 0, 0);
			hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);

			if (SUCCEEDED(hr))
			{
				CalculateLayout();
			}
		}
	}
	return hr;
}

HRESULT FontWindow::CreateFontResources() {
	HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&pDWriteFactory));

	/*IDWriteFontSetBuilder1* pFontSetBuilder;

	if (SUCCEEDED(hr)) {
		hr = pDWriteFactory->CreateFontSetBuilder(&pFontSetBuilder);
		if (SUCCEEDED(hr))
			hr = pFontSetBuilder->CreateFontSet(pFontSet);
	}*/

	if (SUCCEEDED(hr)) {

		hr = pDWriteFactory->CreateTextFormat(L"Times New Roman", NULL, DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20.0f, L"en-us", &pTextFormat_files);

		hr = pDWriteFactory->CreateTextFormat(L"Times New Roman", NULL, DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20.0f, L"en-us", &pTextFormat_size);

		hr = pDWriteFactory->CreateTextFormat(L"Times New Roman", NULL, DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20.0f, L"en-us", &pTextFormat_header);

		if (SUCCEEDED(hr)) {
			hr = pTextFormat_files->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			hr = pTextFormat_size->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			hr = pTextFormat_header->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		}

		if (SUCCEEDED(hr)) {
			hr = pTextFormat_files->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			hr = pTextFormat_size->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			hr = pTextFormat_header->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}
	}

	//SafeRelease(&pFontSetBuilder);

	return hr;
}

void FontWindow::DiscardGraphicsResources()
{
	SafeRelease(&pDWriteFactory);
	SafeRelease(&pRenderTarget);
	SafeRelease(&pBrush);
}

void FontWindow::OnPaint()
{
	HRESULT hr = S_OK;
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);
		pRenderTarget->BeginDraw();
		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::AntiqueWhite));

		/*D2D1_RECT_F text123 = D2D1::RectF(300, 300, 10, 10);
		std::wstring qwe = std::to_wstring(temp);
		pRenderTarget->DrawTextW(qwe.c_str(), wcslen(qwe.c_str()), pTextFormat_files, text123, pBrush);*/

		D2D1_RECT_F textLayout = D2D1::RectF(10, 40, 110, 90);
		pRenderTarget->DrawTextW(L"Cell font:", static_cast<UINT32>(wcslen(L"Cell font:")), pTextFormat_files, textLayout, pBrush);

		textLayout = D2D1::RectF(10, 115, 120, 165);
		pRenderTarget->DrawTextW(L"Header font:", static_cast<UINT32>(wcslen(L"Header font:")), pTextFormat_header, textLayout, pBrush);

		textLayout = D2D1::RectF(10, 250, 130, 300);
		pRenderTarget->DrawTextW(L"Add font", static_cast<UINT32>(wcslen(L"Add font")), pTextFormat_header, textLayout, pBrush);

		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}
		EndPaint(m_hwnd, &ps);
	}
}

void FontWindow::Resize()
{
	if (pRenderTarget != NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		pRenderTarget->Resize(size);
		CalculateLayout();
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
}

void FontWindow::CreateButton() {
	hwndButtonOK = CreateWindow(
		L"BUTTON",
		L"Apply",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		m_hwnd,
		(HMENU)BTN_OK,
		(HINSTANCE)GetModuleHandle(NULL),
		NULL);

	hwndButtonQuit = CreateWindow(
		L"BUTTON",
		L"Discard",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		m_hwnd,
		(HMENU)BTN_QUIT,
		(HINSTANCE)GetModuleHandle(NULL),
		NULL);

	hwndButtonAdd = CreateWindow(
		L"BUTTON",
		L"Add",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		m_hwnd,
		(HMENU)BTN_ADD,
		(HINSTANCE)GetModuleHandle(NULL),
		NULL);

	hwndButtonBrowse = CreateWindow(L"BUTTON", L"Browse", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hwnd, (HMENU)BTN_BROWSE, GetModuleHandle(NULL), NULL);

}

void FontWindow::CreateDropLists() {
	hwndListDef = CreateWindow(WC_COMBOBOX, TEXT(""),
		CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL,
		150, 50, 250, 300, m_hwnd, NULL, (HINSTANCE)GetModuleHandle(NULL),
		NULL);

	hwndListHeader = CreateWindow(WC_COMBOBOX, TEXT(""),
		CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL,
		150, 125, 250, 300, m_hwnd, NULL, (HINSTANCE)GetModuleHandle(NULL),
		NULL);
}

void FontWindow::CreateEditControl() {

	hwndEdit = CreateWindowExW(
		0,
		L"EDIT",
		NULL,
		WS_CHILD | WS_VISIBLE | ES_LEFT | ES_WANTRETURN | WS_BORDER,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		m_hwnd,
		NULL,
		(HINSTANCE)GetModuleHandle(NULL),
		NULL);

	hwndDefFontSize = CreateWindowExW(
		0,
		L"EDIT",
		NULL,
		WS_CHILD | WS_VISIBLE | ES_LEFT | ES_WANTRETURN | WS_BORDER,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		m_hwnd,
		NULL,
		(HINSTANCE)GetModuleHandle(NULL),
		NULL);

	hwndHeaderFontSize = CreateWindowExW(
		0,
		L"EDIT",
		NULL,
		WS_CHILD | WS_VISIBLE | ES_LEFT | ES_WANTRETURN | WS_BORDER,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		m_hwnd,
		NULL,
		(HINSTANCE)GetModuleHandle(NULL),
		NULL);


	SetWindowLongPtr(hwndEdit, GWLP_USERDATA, (LONG_PTR)this);
}

void FontWindow::AddSystemFonts() {
	for (int i = 0; i < systemFonts.size(); i++) {
		SendMessage(hwndListDef, CB_ADDSTRING, 0, (LPARAM)systemFonts[i].c_str());
		SendMessage(hwndListHeader, CB_ADDSTRING, 0, (LPARAM)systemFonts[i].c_str());
	}
}

void FontWindow::AddCustomFont(UINT32 ind) {

	IDWriteFontFaceReference* pFontFaceReference;
	HRESULT hr = (*pFontSet)->GetFontFaceReference(ind, &pFontFaceReference);

	if (SUCCEEDED(hr)) {
		IDWriteFontFace3* pFontFace;
		hr = pFontFaceReference->CreateFontFace(&pFontFace);
		if (SUCCEEDED(hr)) {
			IDWriteLocalizedStrings* pFontNames;
			hr = pFontFace->GetFamilyNames(&pFontNames);
			if (SUCCEEDED(hr)) {
				std::wstring wFontName = GetFontName(&pFontNames);
				customFonts.push_back(wFontName);
				SendMessage(hwndListDef, CB_ADDSTRING, 0, (LPARAM)wFontName.c_str());
				SendMessage(hwndListHeader, CB_ADDSTRING, 0, (LPARAM)wFontName.c_str());
			}
		}
	}
}

HRESULT FontWindow::AddFontToCollection(const WCHAR* filePath)
{
	IDWriteFontSetBuilder1* pFontSetBuilder;
	HRESULT hr = pDWriteFactory->CreateFontSetBuilder(&pFontSetBuilder);

	if (SUCCEEDED(hr)) {
		if (pFontSet != NULL) {
			UINT32 fontCount = (*pFontSet)->GetFontCount();
			for (UINT32 i = 0; i < fontCount; ++i) {
				IDWriteFontFaceReference* pFontFaceReference;
				hr = (*pFontSet)->GetFontFaceReference(i, &pFontFaceReference);
				if (SUCCEEDED(hr)) {
					hr = pFontSetBuilder->AddFontFaceReference(pFontFaceReference);
					if (FAILED(hr)) {
						return hr;
					}
				}
			}
		}

		IDWriteFontFile* pFontFile;
		HRESULT hr = pDWriteFactory->CreateFontFileReference(filePath, nullptr, &pFontFile);
		if (SUCCEEDED(hr)) {
			hr = pFontSetBuilder->AddFontFile(pFontFile);
		}

		IDWriteFontSet* pNewFontSet;
		hr = pFontSetBuilder->CreateFontSet(&pNewFontSet);
		if (SUCCEEDED(hr)) {
			SafeRelease(pFontSet);
			if (pFontSet != NULL) {
				*pFontSet = pNewFontSet;
				IDWriteFontCollection1* pNewFontCollection;
				hr = pDWriteFactory->CreateFontCollectionFromFontSet(pNewFontSet, &pNewFontCollection);
				if (SUCCEEDED(hr)) {
					SafeRelease(pFontCollection);

					*pFontCollection = pNewFontCollection;
				}
			}
		}
	}
	if (pFontSet != nullptr) {
		AddCustomFont((*pFontSet)->GetFontCount() - 1);
	}

	return hr;
}



HRESULT FontWindow::ChangeFont(const WCHAR* newFont, IDWriteTextFormat** formatToChange, FLOAT height, IDWriteFontCollection* fontCollect) {
	(*formatToChange)->Release();
	HRESULT hr = pDWriteFactory->CreateTextFormat(newFont, fontCollect, DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, height, L"en-us", formatToChange);
	if (SUCCEEDED(hr)) {
		hr = (*formatToChange)->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	}
	if (SUCCEEDED(hr)) {
		hr = (*formatToChange)->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
	return hr;

}

LRESULT FontWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE: {
		if (FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
		{
			return -1;
		}

		CreateButton();
		CreateEditControl();
		CreateDropLists();

		CreateGraphicsResources();
		CreateFontResources();

		EnumFontFamilies(GetDC(m_hwnd), (LPCTSTR)NULL,
			(FONTENUMPROC)EnumFamCallBackEx, (LPARAM)this);

		return 0;
	}

	case WM_DESTROY: {

		DiscardGraphicsResources();
		SafeRelease(&pFactory);
		PostQuitMessage(0);
		return 0;
	}

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {

			DestroyWindow(m_hwnd);
		}
		break;

	case WM_COMMAND: {
		if (HIWORD(wParam) == BN_CLICKED) {
			if (LOWORD(wParam) == BTN_ADD) {
				int length = GetWindowTextLengthW(hwndEdit);
				LPWSTR buffer = new WCHAR[length + 1];
				GetWindowTextW(hwndEdit, buffer, length + 1);

				AddFontToCollection(buffer);
				//AddCustomFont((*pFontSet)->GetFontCount() - 1);
				WCHAR tempBuffer[256];
				wcscpy_s(tempBuffer, customFonts[customFonts.size() - 1].c_str());
				SendMessageW(GetParent(m_hwnd), WM_FONTPATH, (WPARAM)buffer, (LPARAM)tempBuffer);
			}
			else if (LOWORD(wParam) == BTN_OK) {
				FontData* structToSend = new FontData();
				std::wstring findStr;
				TCHAR NewFont[256];

				GetWindowTextW(hwndDefFontSize, NewFont, 256);
				structToSend->defHeight = _wtoi(NewFont) / 0.75f;
				LRESULT ind = SendMessage(hwndListDef, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				SendMessage(hwndListDef, CB_GETLBTEXT, (WPARAM)ind, (LPARAM)NewFont);
				wcscpy_s(structToSend->defFont, NewFont);

				GetWindowTextW(hwndHeaderFontSize, NewFont, 256);
				structToSend->headerHeight = _wtoi(NewFont) / 0.75f;
				ind = SendMessage(hwndListHeader, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				SendMessage(hwndListHeader, CB_GETLBTEXT, (WPARAM)ind, (LPARAM)NewFont);
				wcscpy_s(structToSend->headerFont, NewFont);

				SendMessage(GetParent(m_hwnd), WM_GETCOLLECTION, (WPARAM)0, (LPARAM)structToSend);

				DestroyWindow(m_hwnd);
			}
			else if (LOWORD(wParam) == BTN_BROWSE) {
				PWSTR filePath = GetFilePath();
				SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM)filePath);
			}
			else if (LOWORD(wParam) == BTN_QUIT) {
				DestroyWindow(m_hwnd);
			}
		}
		return 0;
	}

	case WM_GETCOLLECTION:
	{
		pFontCollection = (IDWriteFontCollection1**)lParam;
		pFontSet = (IDWriteFontSet**)wParam;

		/*if ((*pFontSet)->GetFontCount() == 0) {
			WCHAR buffer[38] = L"D:\\font\\8bitOperatorPlus8-Regular.ttf";
			AddFontToCollection();
			AddFontToCollection(L"D:\\font\\Canterbury.ttf");
			AddFontToCollection(L"D:\\font\\heorot.ttf");
			AddFontToCollection(L"D:\\font\\Ubuntu-Title.ttf");
		}
		else {
			UINT32 fontCount = (*pFontSet)->GetFontCount();
			for (UINT32 i = 0; i < fontCount; ++i) {
				AddCustomFont(i);
			}
		}*/


		AddSystemFonts();

		return 0;
	}

	case WM_SETDEF: {
		FontData* pData = (FontData*)lParam;

		SendMessage(hwndListDef, CB_SELECTSTRING, -1, (LPARAM)pData->defFont);
		TCHAR buffer[256] = { 0 };
		errno_t result = _itow_s(static_cast<int>(round(pData->defHeight * 0.75f)), buffer, 10);
		if (!result)
			SetWindowText(hwndDefFontSize, buffer);

		SendMessage(hwndListHeader, CB_SELECTSTRING, -1, (LPARAM)pData->headerFont);
		result = _itow_s(static_cast<int>(round(pData->headerHeight * 0.75f)), buffer, 10);
		if (!result)
			SetWindowText(hwndHeaderFontSize, buffer);
		return 0;
	}

	case WM_PAINT: {
		OnPaint();
		return 0;
	}

	case WM_SIZE: {
		Resize();
		return 0;
	}

	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}


BOOL CALLBACK FontWindow::EnumFamCallBackEx(const LOGFONT* lplf, const TEXTMETRIC* lpntm, DWORD FontType, LPARAM lParam)
{
	if (lplf->lfFaceName[0] == L'@') {
		return TRUE;
	}
	FontWindow* thisWind = reinterpret_cast<FontWindow*>(lParam);
	if (FontType & TRUETYPE_FONTTYPE) {
		std::wstring currFont = std::wstring(lplf->lfFaceName);
		thisWind->systemFonts.push_back(currFont);
	}

	return TRUE;
}

PWSTR GetFilePath() {
	IFileOpenDialog* pFileOpen = NULL;

	PWSTR pszFilePath = NULL;

	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
		IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

	if (SUCCEEDED(hr))
	{
		IShellItem* folderItem;
		hr = SHCreateItemFromParsingName(L"D:\\University\\5\\lab\\wp\\FontSelect\\font", 
			NULL, IID_PPV_ARGS(&folderItem));
		if (SUCCEEDED(hr)) {
			pFileOpen->SetFolder(folderItem);
			hr = pFileOpen->Show(NULL);
			if (SUCCEEDED(hr))
			{
				IShellItem* pItem = NULL;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		folderItem->Release();
	}
	return pszFilePath;
}