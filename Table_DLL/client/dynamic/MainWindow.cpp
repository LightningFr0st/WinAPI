#pragma once
#include "MainWindow.h"
#include "FontWindow.h"

#define SINGLE_CLICK_TIMER_ID 1
#define CLICK_DELAY 250

#include <CommCtrl.h>

std::wstring GetFontName(IDWriteTextFormat* pTextFormat) {
	WCHAR fontName[LF_FACESIZE];
	pTextFormat->GetFontFamilyName(fontName, LF_FACESIZE);
	return std::wstring(fontName);
}

HFONT CreateHFONTFromTextFormat(IDWriteTextFormat* pTextFormat) {

	WCHAR fontName[LF_FACESIZE];
	FLOAT fontSize;
	DWRITE_FONT_WEIGHT fontWeight;
	DWRITE_FONT_STYLE fontStyle;

	pTextFormat->GetFontFamilyName(fontName, LF_FACESIZE);
	fontSize = pTextFormat->GetFontSize();
	fontWeight = pTextFormat->GetFontWeight();
	fontStyle = pTextFormat->GetFontStyle();

	LONG lfWeight = (fontWeight >= DWRITE_FONT_WEIGHT_BOLD) ? FW_BOLD : FW_NORMAL;
	BYTE lfItalic = (fontStyle == DWRITE_FONT_STYLE_ITALIC || fontStyle == DWRITE_FONT_STYLE_OBLIQUE) ? TRUE : FALSE;

	LOGFONT lf = {};
	//lf.lfHeight = lfHeight / DPI_SCALING;
	lf.lfHeight = -static_cast<LONG>(fontSize);
	lf.lfWeight = lfWeight;
	lf.lfItalic = lfItalic;
	wcscpy_s(lf.lfFaceName, fontName);
	return CreateFontIndirect(&lf);
}

HRESULT MainWindow::ChangeFont(const WCHAR* newFont, IDWriteTextFormat** formatToChange, FLOAT height, DWRITE_TEXT_ALIGNMENT align, IDWriteFontCollection* fontCollect) {

	(*formatToChange)->Release();
	HRESULT hr = pDWriteFactory->CreateTextFormat(newFont, fontCollect, DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, height, L"en-us", formatToChange);
	if (SUCCEEDED(hr)) {
		hr = (*formatToChange)->SetTextAlignment(align);
	}
	if (SUCCEEDED(hr)) {
		hr = (*formatToChange)->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
	return hr;

}

void MainWindow::CalculateLayout()
{
	if (pRenderTarget != NULL)
	{
		D2D1_SIZE_F size = pRenderTarget->GetSize();
		xm = size.width / 2;
		ym = size.height / 2;
		height = size.height;
		width = size.width;
	}
}

HRESULT MainWindow::CreateGraphicsResources()
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

HRESULT MainWindow::CreateFontResources() {

	HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&pDWriteFactory));

	IDWriteFontSetBuilder1* pFontSetBuilder;

	if (SUCCEEDED(hr)) {
		hr = pDWriteFactory->CreateFontSetBuilder(&pFontSetBuilder);
		if (SUCCEEDED(hr)) {
			hr = pFontSetBuilder->CreateFontSet(&pFontSet);
			if (SUCCEEDED(hr)) {
				hr = pDWriteFactory->CreateFontCollectionFromFontSet(pFontSet, &pFontCollection);
			}
		}
	}

	if (SUCCEEDED(hr)) {

		hr = pDWriteFactory->CreateTextFormat(L"Times New Roman", NULL, DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20.0f, L"en-us", &pTextFormatDef);

		if (SUCCEEDED(hr)) {
			hr = pTextFormatDef->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			if (SUCCEEDED(hr)) {
				hr = pTextFormatDef->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			}
		}

		hr = pDWriteFactory->CreateTextFormat(L"Times New Roman", NULL, DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 30.0f, L"en-us", &pTextFormatHeader);

		if (SUCCEEDED(hr)) {
			hr = pTextFormatHeader->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			if (SUCCEEDED(hr)) {
				hr = pTextFormatHeader->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			}
		}
	}


	return hr;
}

void MainWindow::LoadDll() {
	hDLL = LoadLibrary(L"TableDll");
	if (NULL != hDLL) {
		pCreateTable = (ContructorP)GetProcAddress(hDLL, "CreateTable");
		pDrawTable = (DrawP)GetProcAddress(hDLL, "Draw");
		pAddRow = (AddRowP)GetProcAddress(hDLL, "AddRowF");

		pLMBHandle = (HadleP)GetProcAddress(hDLL, "LMBHandleW");
		pCtrlLMBHandle = (HadleP)GetProcAddress(hDLL, "CtrlLMBHandleW");
		pRMBHandle = (HadleP)GetProcAddress(hDLL, "RMBHandleW");
		pDLMBHandle = (HadleP)GetProcAddress(hDLL, "DLMBHandleW");

		pGetStart = (Getter)GetProcAddress(hDLL, "getStartInd");
		pGetN = (Getter)GetProcAddress(hDLL, "getRows");
		pGetD = (Getter)GetProcAddress(hDLL, "getDisp");

		pDecr = (Getter)GetProcAddress(hDLL, "Decr");
		pInc = (Getter)GetProcAddress(hDLL, "Inc");
	}
}

void MainWindow::DiscardGraphicsResources()
{
	SafeRelease(&pRenderTarget);
	SafeRelease(&pBrush);
}

void MainWindow::OnPaint()
{
	HRESULT hr = S_OK;
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);
		pRenderTarget->BeginDraw();
		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		// Draw borders

		pDrawTable(&MainTableV);

		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}
		EndPaint(m_hwnd, &ps);
	}
}

void MainWindow::RemoveCustomFonts() {
	for (int i = 0; i < customFontsPathes.size(); i++) {
		RemoveFontResourceEx(customFontsPathes[i], FR_PRIVATE, 0);
	}
}

void MainWindow::Resize()
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

void MainWindow::CenterChildWindow(HWND hwndChild, HWND hwndParent) {
	RECT parentRect;
	GetClientRect(hwndParent, &parentRect);

	int parentWidth = parentRect.right - parentRect.left;
	int parentHeight = parentRect.bottom - parentRect.top;

	RECT childRect;
	GetWindowRect(hwndChild, &childRect);

	int childWidth = childRect.right - childRect.left;
	int childHeight = childRect.bottom - childRect.top;

	int xPos = (parentWidth - childWidth) / 2;
	int yPos = (parentHeight - childHeight) / 2;

	SetWindowPos(hwndChild, NULL, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

LRESULT MainWindow::SendDefFonts(HWND child) {
	FontData* structToSend = new FontData();

	WCHAR fontName[LF_FACESIZE];

	structToSend->defHeight = pTextFormatDef->GetFontSize();
	pTextFormatDef->GetFontFamilyName(fontName, LF_FACESIZE);
	wcscpy_s(structToSend->defFont, fontName);

	structToSend->headerHeight = pTextFormatHeader->GetFontSize();
	pTextFormatHeader->GetFontFamilyName(fontName, LF_FACESIZE);
	wcscpy_s(structToSend->headerFont, fontName);

	return SendMessage(child, WM_SETDEF, (WPARAM)0, (LPARAM)structToSend);
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static POINT clickPoint = { 0, 0 };
	static bool clickCtrl = false;
	switch (uMsg)
	{
	case WM_CREATE: {
		if (FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
		{
			return -1;
		}
		CreateGraphicsResources();
		CreateFontResources();

		LoadDll();
		
		pCreateTable(10, 10, Window(), pRenderTarget, pBrush, &pTextFormatDef, &pTextFormatHeader, &MainTableV);

		for (int i = 0; i < 4; i++) {
			pAddRow(L"Editable", &MainTableV);
		}

		SetCursor(LoadCursor(NULL, IDC_ARROW));

		return 0;
	}

	case WM_DESTROY: {
		RemoveCustomFonts();
		DiscardGraphicsResources();
		SafeRelease(&pFactory);
		FreeLibrary(hDLL);
		PostQuitMessage(0);
		return 0;
	}

	case WM_LBUTTONDBLCLK: {
		pendingSingleClick = false;
		KillTimer(m_hwnd, SINGLE_CLICK_TIMER_ID);


		pDLMBHandle(LOWORD(lParam), HIWORD(lParam), &MainTableV);
		
		break;
	}

	case WM_RBUTTONDOWN: {

		pRMBHandle(LOWORD(lParam), HIWORD(lParam), &MainTableV);
		
		break;
	}

	case WM_LBUTTONDOWN: {
		pendingSingleClick = true;

		if (wParam & MK_CONTROL) {
			clickCtrl = true;
		}
		else {
			clickCtrl = false;
		}

		clickPoint.x = LOWORD(lParam);
		clickPoint.y = HIWORD(lParam);

		SetTimer(m_hwnd, SINGLE_CLICK_TIMER_ID, CLICK_DELAY, NULL);
		break;
	}

	case WM_TIMER: {
		if (wParam == SINGLE_CLICK_TIMER_ID) {
			KillTimer(m_hwnd, SINGLE_CLICK_TIMER_ID);
			if (pendingSingleClick) {
				if (clickCtrl) {
					pCtrlLMBHandle(clickPoint.x, clickPoint.y, &MainTableV);
									}
				else {
					pLMBHandle(clickPoint.x, clickPoint.y, &MainTableV);
					InvalidateRect(m_hwnd, NULL, FALSE);
				}

				pendingSingleClick = false;
			}
		}
		return 0;
	}

	case WM_KEYDOWN: {
		if (wParam == VK_ESCAPE) {
			DestroyWindow(m_hwnd);
		}
		return 0;
	}

	case WM_LMB: {

		int row = static_cast<int>(wParam);
		int col = static_cast<int>(lParam);

		SYSTEMTIME st;
		GetLocalTime(&st);

		std::wstring time = L"Time: " + std::to_wstring(st.wHour) + L':' + std::to_wstring(st.wMinute) + L':'
			+ std::to_wstring(st.wSecond) + L':' + std::to_wstring(st.wMilliseconds);

		std::wstring result = L"WM_LMB handled\nCell: (" + std::to_wstring(row) + L"," + std::to_wstring(col) + L")\n"
			+ time;

		MessageBoxW(NULL, result.c_str(), L"Event handled", MB_OK);

		return 0;
	}

	case WM_DLMB: {
		int row = static_cast<int>(wParam);
		int col = static_cast<int>(lParam);

		SYSTEMTIME st;
		GetLocalTime(&st);

		std::wstring time = L"Time: " + std::to_wstring(st.wHour) + L':' + std::to_wstring(st.wMinute) + L':'
			+ std::to_wstring(st.wSecond) + L':' + std::to_wstring(st.wMilliseconds);

		std::wstring result = L"WM_DLMB handled\nCell: (" + std::to_wstring(row) + L"," + std::to_wstring(col) + L")\n"
			+ time;

		MessageBoxW(NULL, result.c_str(), L"Event handled", MB_OK);
		return 0;
	}

	case WM_RMB: {
		int row = static_cast<int>(wParam);
		int col = static_cast<int>(lParam);

		SYSTEMTIME st;
		GetLocalTime(&st);

		std::wstring time = L"Time: " + std::to_wstring(st.wHour) + L':' + std::to_wstring(st.wMinute) + L':'
			+ std::to_wstring(st.wSecond) + L':' + std::to_wstring(st.wMilliseconds);

		std::wstring result = L"WM_RMB handled\nCell: (" + std::to_wstring(row) + L"," + std::to_wstring(col) + L")\n"
			+ time;

		MessageBoxW(NULL, result.c_str(), L"Event handled", MB_OK);
		return 0;
	}

	case WM_CTRLLMB: {
		int row = static_cast<int>(wParam);
		int col = static_cast<int>(lParam);
		SYSTEMTIME st;
		GetLocalTime(&st);

		std::wstring time = L"Time: " + std::to_wstring(st.wHour) + L':' + std::to_wstring(st.wMinute) + L':'
			+ std::to_wstring(st.wSecond) + L':' + std::to_wstring(st.wMilliseconds);

		std::wstring result = L"WM_CTRLLMB handled\nCell: (" + std::to_wstring(row) + L"," + std::to_wstring(col) + L")\n"
			+ time;

		MessageBoxW(NULL, result.c_str(), L"Event handled", MB_OK);
		return 0;
	}

	case WM_LMB_H: {
		int col = static_cast<int>(wParam);

		SYSTEMTIME st;
		GetLocalTime(&st);

		std::wstring time = L"Time: " + std::to_wstring(st.wHour) + L':' + std::to_wstring(st.wMinute) + L':'
			+ std::to_wstring(st.wSecond) + L':' + std::to_wstring(st.wMilliseconds);

		std::wstring result = L"WM_LMB_H handled\nHeader: (" + std::to_wstring(col + 1) + L")\n"
			+ time;

		MessageBoxW(NULL, result.c_str(), L"Event handled", MB_OK);
		return 0;
	}

	case WM_CHAR: {
		if (wParam == 'F' || wParam == 'f') {
			FontWindow winChild;

			if (!winChild.Create(L"Font Select", WS_CHILD | WS_BORDER, 0, 0, 0, 500, 500, m_hwnd))
			{
				return 0;
			}

			ShowWindow(winChild.Window(), SW_SHOW);

			CenterChildWindow(winChild.Window(), m_hwnd);

			SetFocus(winChild.Window());

			MSG msg = { };

			SendMessage(winChild.Window(), WM_GETCOLLECTION, (WPARAM)&pFontSet, (LPARAM)&pFontCollection);
			SendDefFonts(winChild.Window());

			while (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			InvalidateRect(m_hwnd, NULL, FALSE);
			return 0;
		}
		break;
	}
	case WM_GETCOLLECTION:
	{
		FontData* pData = (FontData*)lParam;

		std::wstring findStr = std::wstring(pData->defFont);

		if (GetFontName(pTextFormatDef) != findStr || pTextFormatDef->GetFontSize() != pData->defHeight) {
			if (find(customFontsNames.begin(), customFontsNames.end(), findStr)
				!= customFontsNames.end()) {
				ChangeFont(pData->defFont, &pTextFormatDef, pData->defHeight, DWRITE_TEXT_ALIGNMENT_CENTER,
					pFontCollection);
			}
			else {
				ChangeFont(pData->defFont, &pTextFormatDef, pData->defHeight, DWRITE_TEXT_ALIGNMENT_CENTER,
					NULL);
			}
		}

		findStr = std::wstring(pData->headerFont);
		if (GetFontName(pTextFormatHeader) != findStr || pTextFormatHeader->GetFontSize() != pData->headerHeight) {
			if (find(customFontsNames.begin(), customFontsNames.end(), findStr)
				!= customFontsNames.end()) {
				ChangeFont(pData->headerFont, &pTextFormatHeader, pData->headerHeight, DWRITE_TEXT_ALIGNMENT_CENTER,
					pFontCollection);
			}
			else {
				ChangeFont(pData->headerFont, &pTextFormatHeader, pData->headerHeight, DWRITE_TEXT_ALIGNMENT_CENTER,
					NULL);
			}
		}

		InvalidateRect(m_hwnd, NULL, FALSE);

		return 0;
	}

	case WM_PAINT: {
		OnPaint();
		return 0;
	}

	case WM_FONTPATH: {
		LPWSTR fontPath = (LPWSTR)wParam;
		customFontsPathes.push_back(fontPath);
		int fontCount = AddFontResourceEx(fontPath, FR_PRIVATE, 0);
		if (fontCount == 0) {
			return -1;
		}

		LPWSTR fontName = (LPWSTR)lParam;
		customFontsNames.push_back(std::wstring(fontName));
		return 0;
	}

	case WM_SIZE: {
		Resize();
		return 0;
	}

	case WM_MOUSEWHEEL: {
		INT16 wheelDirection = HIWORD(wParam);
		
		int rowsDisplay = pGetD(&MainTableV);
		int nRows = pGetN(&MainTableV);
		if (wheelDirection > 0 && pGetStart(&MainTableV) > 0) {

			pDecr(&MainTableV);
		}
		else if (wheelDirection < 0 && pGetStart(&MainTableV) + rowsDisplay < nRows - 1) {
			pInc(&MainTableV);
		}

		InvalidateRect(m_hwnd, NULL, FALSE);

		return 0;
	}

	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}