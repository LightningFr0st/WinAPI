#pragma once
#include "MainWindow.h"
#include "FontWindow.h"

#define AlignPixel(x)  floorf((x)) + 0.5F

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

		MoveWindow(hwndEdit, 1, 1, static_cast<int>(0.75f * xm) - 1, static_cast <int>(0.05f * height) - 1, TRUE);
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
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20.0f, L"en-us", &pTextFormat_files);

		if (SUCCEEDED(hr)) {
			hr = pTextFormat_files->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			if (SUCCEEDED(hr)) {
				hr = pTextFormat_files->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
				if (SUCCEEDED(hr)) {
					trimmingOptions.granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;
					trimmingOptions.delimiter = 0;
					trimmingOptions.delimiterCount = 0;

					IDWriteInlineObject* pEllipsis = nullptr;

					hr = pDWriteFactory->CreateEllipsisTrimmingSign(pTextFormat_files, &pEllipsis);

					if (SUCCEEDED(hr)) {
						pTextFormat_files->SetTrimming(&trimmingOptions, pEllipsis);
					}
				}
			}
		}

		hr = pDWriteFactory->CreateTextFormat(L"Times New Roman", NULL, DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20.0f, L"en-us", &pTextFormat_size);

		if (SUCCEEDED(hr)) {
			hr = pTextFormat_size->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			if (SUCCEEDED(hr)) {
				hr = pTextFormat_size->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			}
		}

		hr = pDWriteFactory->CreateTextFormat(L"Times New Roman", NULL, DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 28.0f, L"en-us", &pTextFormat_header);

		if (SUCCEEDED(hr)) {
			hr = pTextFormat_header->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			if (SUCCEEDED(hr)) {
				hr = pTextFormat_header->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			}
		}
	}


	return hr;
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

		FLOAT newHeight = height - 1, newWidth = width - 1;

		pRenderTarget->DrawLine(D2D1::Point2F(AlignPixel(0), AlignPixel(0)),
			D2D1::Point2F(AlignPixel(0), AlignPixel(newHeight)), pBrush);
		pRenderTarget->DrawLine(D2D1::Point2F(AlignPixel(0), AlignPixel(0)),
			D2D1::Point2F(AlignPixel(newWidth), AlignPixel(0)), pBrush);
		pRenderTarget->DrawLine(D2D1::Point2F(AlignPixel(newWidth), AlignPixel(0)),
			D2D1::Point2F(AlignPixel(newWidth), AlignPixel(newHeight)), pBrush);
		pRenderTarget->DrawLine(D2D1::Point2F(AlignPixel(0), AlignPixel(newHeight)),
			D2D1::Point2F(AlignPixel(newWidth), AlignPixel(newHeight)), pBrush);

		// Draw table

		pRenderTarget->DrawLine(D2D1::Point2F(AlignPixel(xm), AlignPixel(0)),
			D2D1::Point2F(AlignPixel(xm), AlignPixel(height)), pBrush);
		pRenderTarget->DrawLine(D2D1::Point2F(AlignPixel(0.75f * xm), AlignPixel(0)),
			D2D1::Point2F(AlignPixel(0.75f * xm), AlignPixel(height)), pBrush);
		pRenderTarget->DrawLine(D2D1::Point2F(AlignPixel(1.75f * xm), AlignPixel(0)), D2D1::
			Point2F(AlignPixel(1.75f * xm), AlignPixel(height)), pBrush);

		pRenderTarget->DrawLine(D2D1::Point2F(AlignPixel(0), AlignPixel(0.05f * height)),
			D2D1::Point2F(AlignPixel(width), AlignPixel(0.05f * height)), pBrush);

		// Draw Text

		FLOAT k = 0.05f;
		D2D1_RECT_F textLayout;
		const wchar_t* textOut;

		//Headers

		textLayout = D2D1::RectF(0.75f * xm, 0, xm, 0.05f * height);
		pRenderTarget->DrawTextW(L"File size", static_cast<UINT32>(wcslen(L"File size")), pTextFormat_header, textLayout, pBrush);

		textLayout = D2D1::RectF(1.75f * xm, 0, 2 * xm, 0.05f * height);
		pRenderTarget->DrawTextW(L"File size", static_cast<UINT32>(wcslen(L"File size")), pTextFormat_header, textLayout, pBrush);

		std::wstring stringOut = L"File name(" + std::to_wstring(start_arr.size()) + L")";
		textOut = stringOut.c_str();
		textLayout = D2D1::RectF(xm, 0, 1.75f * xm, 0.05f * height);
		pRenderTarget->DrawTextW(textOut, static_cast<UINT32>(wcslen(textOut)), pTextFormat_header, textLayout, pBrush);

		//Table content

		for (int i = start_index; i < min(start_index + 38, start_arr.size()); i++) {

			textOut = start_arr[i].first.c_str();
			textLayout = D2D1::RectF(2, k * height, 0.75f * xm, (k + 0.025f) * height);
			pRenderTarget->DrawTextW(textOut, static_cast<UINT32>(wcslen(textOut)), pTextFormat_files, textLayout, pBrush);

			textOut = start_arr[i].second.c_str();
			textLayout = D2D1::RectF(0.75f * xm, k * height, xm, (k + 0.025f) * height);
			pRenderTarget->DrawTextW(start_arr[i].second.c_str(), static_cast<UINT32>(wcslen(textOut)), pTextFormat_size, textLayout, pBrush);

			textOut = end_arr[i].first.c_str();
			textLayout = D2D1::RectF(xm + 2, k * height, 1.75f * xm, (k + 0.025f) * height);
			pRenderTarget->DrawTextW(textOut, static_cast<UINT32>(wcslen(textOut)), pTextFormat_files, textLayout, pBrush);

			textOut = end_arr[i].second.c_str();
			textLayout = D2D1::RectF(1.75f * xm, k * height, 2 * xm, (k + 0.025f) * height);
			pRenderTarget->DrawTextW(textOut, static_cast<UINT32>(wcslen(textOut)), pTextFormat_size, textLayout, pBrush);



			k += 0.025f;
		}

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

void MainWindow::CreateEditControl() {

	hwndEdit = CreateWindowExW(
		0,
		L"EDIT",
		NULL,
		WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		m_hwnd,
		NULL,
		(HINSTANCE)GetModuleHandle(NULL),
		NULL);

	SetWindowLongPtr(hwndEdit, GWLP_USERDATA, (LONG_PTR)this);

	OriginalEditProc = (WNDPROC)SetWindowLongPtr(hwndEdit, GWLP_WNDPROC, (LONG_PTR)EditProc);
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

	structToSend->filesHeight = pTextFormat_files->GetFontSize();
	pTextFormat_files->GetFontFamilyName(fontName, LF_FACESIZE);
	wcscpy_s(structToSend->filesFont, fontName);

	structToSend->paramHeight = pTextFormat_size->GetFontSize();
	pTextFormat_size->GetFontFamilyName(fontName, LF_FACESIZE);
	wcscpy_s(structToSend->paramFont, fontName);

	structToSend->headerHeight = pTextFormat_header->GetFontSize();
	pTextFormat_header->GetFontFamilyName(fontName, LF_FACESIZE);
	wcscpy_s(structToSend->headersFont, fontName);

	return SendMessage(child, WM_SETDEF, (WPARAM)0, (LPARAM)structToSend);
}


LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE: {
		if (FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
		{
			return -1;
		}
		CreateEditControl();
		CreateGraphicsResources();
		CreateFontResources();

		RECT rect;
		SendMessage(hwndEdit, EM_GETRECT, 0, (LPARAM)&rect);
		rect.top = 10;
		rect.bottom = static_cast<LONG>(round(0.05 * height)) - 1;
		SendMessage(hwndEdit, WM_SETFONT, (WPARAM)CreateHFONTFromTextFormat(pTextFormat_header), LPARAM(TRUE));
		SendMessage(hwndEdit, EM_SETRECT, 0, (LPARAM)&rect);

		return 0;
	}

	case WM_DESTROY: {
		RemoveCustomFonts();
		DiscardGraphicsResources();
		SafeRelease(&pFactory);
		PostQuitMessage(0);
		return 0;
	}

	case WM_RBUTTONDOWN: {

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

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			DestroyWindow(m_hwnd);
		}
		break;

	case WM_GETCOLLECTION:
	{
		FontData* pData = (FontData*)lParam;

		std::wstring findStr = std::wstring(pData->filesFont);

		if (GetFontName(pTextFormat_files) != findStr || pTextFormat_files->GetFontSize() != pData->filesHeight) {
			if (find(customFontsNames.begin(), customFontsNames.end(), findStr)
				!= customFontsNames.end()) {
				ChangeFont(pData->filesFont, &pTextFormat_files, pData->filesHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
					pFontCollection);
			}
			else {
				ChangeFont(pData->filesFont, &pTextFormat_files, pData->filesHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
					NULL);
			}
		}

		findStr = std::wstring(pData->paramFont);
		if (GetFontName(pTextFormat_size) != findStr || pTextFormat_size->GetFontSize() != pData->paramHeight) {
			if (find(customFontsNames.begin(), customFontsNames.end(), findStr)
				!= customFontsNames.end()) {
				ChangeFont(pData->paramFont, &pTextFormat_size, pData->paramHeight, DWRITE_TEXT_ALIGNMENT_CENTER,
					pFontCollection);
			}
			else {
				ChangeFont(pData->paramFont, &pTextFormat_size, pData->paramHeight, DWRITE_TEXT_ALIGNMENT_CENTER,
					NULL);
			}
		}

		findStr = std::wstring(pData->headersFont);
		if (GetFontName(pTextFormat_header) != findStr || pTextFormat_header->GetFontSize() != pData->headerHeight) {
			if (find(customFontsNames.begin(), customFontsNames.end(), findStr)
				!= customFontsNames.end()) {
				ChangeFont(pData->headersFont, &pTextFormat_header, pData->headerHeight, DWRITE_TEXT_ALIGNMENT_CENTER,
					pFontCollection);
				SendMessage(hwndEdit, WM_SETFONT, (WPARAM)CreateHFONTFromTextFormat(pTextFormat_header), LPARAM(TRUE));
			}
			else {
				ChangeFont(pData->headersFont, &pTextFormat_header, pData->headerHeight, DWRITE_TEXT_ALIGNMENT_CENTER,
					NULL);
				SendMessage(hwndEdit, WM_SETFONT, (WPARAM)CreateHFONTFromTextFormat(pTextFormat_header), LPARAM(TRUE));
			}
		}

		RECT rect;
		SendMessage(hwndEdit, EM_GETRECT, 0, (LPARAM)&rect);
		rect.top = 10;
		rect.bottom = static_cast<LONG>(round(0.05 * height)) - 1;
		SendMessage(hwndEdit, EM_SETRECT, 0, (LPARAM)&rect);

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
		if (start_arr.size() != 0) {
			INT16 wheelDirection = HIWORD(wParam);
			if (wheelDirection > 0 && start_index >= 38) {
				start_index -= 38;
			}
			else if (wheelDirection < 0 && start_index + 38 < start_arr.size()) {
				start_index += 38;
			}
			InvalidateRect(m_hwnd, NULL, FALSE);
		}
		return 0;
	}

	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MainWindow::EditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	MainWindow* pThis = (MainWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (msg) {
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {

			int length = GetWindowTextLengthW(pThis->hwndEdit);
			LPWSTR buffer = new WCHAR[length + 1];
			GetWindowTextW(pThis->hwndEdit, buffer, length + 1);

			// C:\Program Files\

			pThis->InitArray((std::wstring(buffer) + L"*").c_str());
			pThis->QuickSort(0, pThis->end_arr.size() - 1);
			pThis->FinilizeArray();
			InvalidateRect(pThis->m_hwnd, NULL, FALSE);
			SetFocus(pThis->m_hwnd);
			return 0;
		}
		else if (wParam == VK_ESCAPE) {
			DestroyWindow(pThis->m_hwnd);
		}
		break;

	case WM_CHAR:
		if (wParam == VK_RETURN) {
			return 0;
		}
		break;

	default:
		break;
	}

	if (pThis) {
		return CallWindowProc(pThis->OriginalEditProc, hwnd, msg, wParam, lParam);
	}
	else {
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

}

void MainWindow::InitArray(LPCWSTR start_dir) {
	//wcout << "Func started my dir: " << start_dir << '\n';
	WIN32_FIND_DATA data;
	HANDLE hFind = FindFirstFile(start_dir, &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (wcscmp(data.cFileName, L".") != 0 && wcscmp(data.cFileName, L"..") != 0) {
				if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					std::wstring next_dir(start_dir);
					next_dir.erase(wcslen(start_dir) - 1, 1);
					next_dir += data.cFileName;
					next_dir += L"\\*";
					InitArray(next_dir.c_str());
				}
				else {
					std::wstring fileName = std::wstring(data.cFileName);
					repeatData[fileName].first++;
					repeatData[fileName].second++;

					start_arr.push_back(std::make_pair(fileName,
						std::to_wstring((data.nFileSizeHigh * (MAXDWORD + 1ULL)) + data.nFileSizeLow)));

					end_arr.push_back(std::make_pair(fileName,
						std::to_wstring((data.nFileSizeHigh * (MAXDWORD + 1ULL)) + data.nFileSizeLow)));

				}
			}

		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
}

void MainWindow::QuickSort(size_t low, size_t high) {
	if (low < high) {
		size_t pi = HoarePartition(low, high);

		QuickSort(low, pi);
		QuickSort(pi + 1, high);
	}
}

size_t MainWindow::HoarePartition(size_t low, size_t high) {
	std::wstring size_pivot = end_arr[(low + high) / 2].second;
	std::wstring name_pivot = end_arr[(low + high) / 2].first;

	size_t l = low;
	size_t r = high;

	while (true) {
		while (end_arr[l].second < size_pivot ||
			(end_arr[l].second == size_pivot) && (end_arr[l].first < name_pivot))
		{
			l++;
		}

		while (end_arr[r].second > size_pivot ||
			(end_arr[r].second == size_pivot) && (end_arr[r].first > name_pivot))
		{
			r--;
		}

		if (r <= l) return r;
		swap(end_arr[l++], end_arr[r--]);
	}
}

void MainWindow::FinilizeArray() {
	std::wstring fileName;
	for (int i = 0; i < end_arr.size(); i++) {
		fileName = end_arr[i].first;
		if (repeatData[fileName].first > 1) {
			UINT32 num = repeatData[fileName].second - repeatData[fileName].first--;
			if (num != 0) {
				end_arr[i].first = fileName + L"["
					+ std::to_wstring(num) + L"]";
			}
		}
	}
}
