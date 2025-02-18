#pragma once
#include "MainWindow.h"
#define DPI_SCALING 1.25
#include "wingdi.h"
#include <objidl.h>
#include <gdiplus.h>
#include <shobjidl.h> 
#include <d2d1_1helper.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

HFONT CreateHFONTFromTextFormat(IDWriteTextFormat* pTextFormat) {

	WCHAR fontName[LF_FACESIZE];
	FLOAT fontSize;
	DWRITE_FONT_WEIGHT fontWeight;
	DWRITE_FONT_STYLE fontStyle;

	pTextFormat->GetFontFamilyName(fontName, LF_FACESIZE);
	fontSize = pTextFormat->GetFontSize();
	fontWeight = pTextFormat->GetFontWeight();
	fontStyle = pTextFormat->GetFontStyle();

	LONG lfHeight = -MulDiv(static_cast<LONG>(fontSize), GetDeviceCaps(GetDC(NULL), LOGPIXELSY), 72);

	LONG lfWeight = (fontWeight >= DWRITE_FONT_WEIGHT_BOLD) ? FW_BOLD : FW_NORMAL;
	LONG lfItalic = (fontStyle == DWRITE_FONT_STYLE_ITALIC || fontStyle == DWRITE_FONT_STYLE_OBLIQUE) ? TRUE : FALSE;

	LOGFONT lf = {};
	lf.lfHeight = lfHeight / 1.5F;
	lf.lfWeight = lfWeight / 1.5F;
	lf.lfItalic = lfItalic;
	wcscpy_s(lf.lfFaceName, fontName);

	return CreateFontIndirect(&lf);
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

		MoveWindow(hwndEdit, 0, 0, xm * 0.75 - 1, 0.05 * height - 1, TRUE);
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

		pRenderTarget->SetDpi(96.0f * 1, 96.0f * 1);

		if (SUCCEEDED(hr))
		{
			const D2D1_COLOR_F color = D2D1::ColorF(0, 0, 0);
			hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);

			if (SUCCEEDED(hr))
			{
				pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(255, 0, 0), &pBrushRed);
				CalculateLayout();
			}

			hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
				reinterpret_cast<IUnknown**>(&pDWriteFactory));
			if (SUCCEEDED(hr)) {
				wszText = L"Test string";
				cTextLength = (UINT32)wcslen(wszText);

				hr = pDWriteFactory->CreateTextFormat(L"Times New Roman", NULL, DWRITE_FONT_WEIGHT_REGULAR,
					DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 18.0f, L"en-us", &pTextFormat_files);

				hr = pDWriteFactory->CreateTextFormat(L"Times New Roman", NULL, DWRITE_FONT_WEIGHT_REGULAR,
					DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 18.0f, L"en-us", &pTextFormat_size);

				hr = pDWriteFactory->CreateTextFormat(L"Times New Roman", NULL, DWRITE_FONT_WEIGHT_REGULAR,
					DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 25.0f, L"en-us", &pTextFormat_header);

				trimmingOptions.granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;
				trimmingOptions.delimiter = 0;
				trimmingOptions.delimiterCount = 0;

				IDWriteInlineObject* pEllipsis = nullptr;

				pDWriteFactory->CreateEllipsisTrimmingSign(pTextFormat_files, &pEllipsis);

				pTextFormat_files->SetTrimming(&trimmingOptions, pEllipsis);

				if (SUCCEEDED(hr)) {
					hr = pTextFormat_files->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
					hr = pTextFormat_size->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
					hr = pTextFormat_header->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				}

				if (SUCCEEDED(hr)) {
					hr = pTextFormat_files->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
					hr = pTextFormat_size->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
					hr = pTextFormat_header->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
				}
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
		HDC hdc = BeginPaint(m_hwnd, &ps);


		pRenderTarget->BeginDraw();
		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		// Draw table
		pRenderTarget->DrawLine(D2D1::Point2F(floorf(xm) + 0.5f, 0), D2D1::Point2F(floorf(xm) + 0.5f, floorf(0.95 * height) + 0.5f), pBrush);
		pRenderTarget->DrawLine(D2D1::Point2F(floorf(0.75 * xm) + 0.5f, 0), D2D1::Point2F(floorf(0.75 * xm) + 0.5f, floorf(0.95 * height) + 0.5f), pBrush);
		pRenderTarget->DrawLine(D2D1::Point2F(floorf(1.75 * xm) + 0.5f, 0), D2D1::Point2F(floorf(1.75 * xm) + 0.5f, floorf(0.95 * height) + 0.5f), pBrush);

		pRenderTarget->DrawLine(D2D1::Point2F(0, floorf(0.05 * height) + 0.5f), D2D1::Point2F(width, floorf(0.05 * height) + 0.5f), pBrush);
		pRenderTarget->DrawLine(D2D1::Point2F(0, floorf(0.95 * height) + 0.5f), D2D1::Point2F(width, floorf(0.95 * height) + 0.5f), pBrush);

		// Draw Text

		FLOAT k = 0.05;
		D2D1_RECT_F textLayout;
		const wchar_t* textOut;

		//Headers

		textLayout = D2D1::RectF(0.75 * xm, 0, xm, 0.05 * height);
		pRenderTarget->DrawTextW(L"File size (bytes)", wcslen(L"File size (bytes)"), pTextFormat_header, textLayout, pBrush);

		textLayout = D2D1::RectF(xm, 0, 1.75 * xm, 0.05 * height);
		pRenderTarget->DrawTextW(L"File name", wcslen(L"File name"), pTextFormat_header, textLayout, pBrush);

		textLayout = D2D1::RectF(1.75 * xm, 0, 2 * xm, 0.05 * height);
		pRenderTarget->DrawTextW(L"File size (bytes)", wcslen(L"File size (bytes)"), pTextFormat_header, textLayout, pBrush);

		std::wstring stringOut = L"Total files: " + std::to_wstring(start_arr.size());
		textOut = stringOut.c_str();
		textLayout = D2D1::RectF(0, 0.95 * height, width, height);
		pRenderTarget->DrawTextW(textOut, wcslen(textOut), pTextFormat_header, textLayout, pBrush);

		//Table content

		for (int i = start_index; i < min(start_index + 36, start_arr.size()); i++) {

			textOut = start_arr[i].first.c_str();
			textLayout = D2D1::RectF(2, k * height, 0.75 * xm, (k + 0.025) * height);
			pRenderTarget->DrawTextW(textOut, wcslen(textOut), pTextFormat_files, textLayout, pBrush);

			textOut = start_arr[i].second.c_str();
			textLayout = D2D1::RectF(0.75 * xm, k * height, xm, (k + 0.025) * height);
			pRenderTarget->DrawTextW(start_arr[i].second.c_str(), wcslen(textOut), pTextFormat_size, textLayout, pBrush);

			textOut = end_arr[i].first.c_str();
			textLayout = D2D1::RectF(xm + 2, k * height, 1.75 * xm, (k + 0.025) * height);
			pRenderTarget->DrawTextW(textOut, wcslen(textOut), pTextFormat_files, textLayout, pBrush);

			textOut = end_arr[i].second.c_str();
			textLayout = D2D1::RectF(1.75 * xm, k * height, 2 * xm, (k + 0.025) * height);
			pRenderTarget->DrawTextW(textOut, wcslen(textOut), pTextFormat_size, textLayout, pBrush);

			k += 0.025;
		}

		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}

		HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		SelectObject(hdc, hPen);

		/*MoveToEx(hdc, 0, 0, NULL);
		LineTo(hdc, 0, height * DPI_SCALING);

		MoveToEx(hdc, 0, height * DPI_SCALING, NULL);
		LineTo(hdc, width * DPI_SCALING, height * DPI_SCALING);

		MoveToEx(hdc, width * DPI_SCALING, height * DPI_SCALING, NULL);
		LineTo(hdc, 0, height * DPI_SCALING);

		MoveToEx(hdc, 0, 0, NULL);
		LineTo(hdc, 0, height * DPI_SCALING);

		MoveToEx(hdc, xm * DPI_SCALING, 0, NULL);
		LineTo(hdc, xm * DPI_SCALING, 0.95 * height * DPI_SCALING);

		MoveToEx(hdc, 0.75 * xm * DPI_SCALING, 0, NULL);
		LineTo(hdc, 0.75 * xm * DPI_SCALING, 0.95 * height * DPI_SCALING);

		MoveToEx(hdc, 1.75 * xm * DPI_SCALING, 0, NULL);
		LineTo(hdc, 1.75 * xm * DPI_SCALING, 0.95 * height * DPI_SCALING);

		MoveToEx(hdc, 0, 0.05 * height * DPI_SCALING, NULL);
		LineTo(hdc, width * DPI_SCALING, 0.05 * height *DPI_SCALING);

		MoveToEx(hdc, 0, 0.95 * height * DPI_SCALING, NULL);
		LineTo(hdc, width * DPI_SCALING, 0.95 * height * DPI_SCALING);*/

		DeleteObject(hPen);

		EndPaint(m_hwnd, &ps);
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
		0, L"EDIT",   // predefined class 
		NULL,         // no window title 
		WS_CHILD | WS_VISIBLE | ES_LEFT | ES_WANTRETURN,
		0, 0, 0, 0,   // set size in WM_SIZE message 
		m_hwnd,         // parent window 
		0,   // edit control ID 
		(HINSTANCE)GetModuleHandle(NULL),
		NULL);        // pointer to window 

	SetWindowLongPtr(hwndEdit, GWLP_USERDATA, (LONG_PTR)this);

	OriginalEditProc = (WNDPROC)SetWindowLongPtr(hwndEdit, GWLP_WNDPROC, (LONG_PTR)EditProc);

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
		SendMessage(hwndEdit, WM_SETFONT, (WPARAM)CreateHFONTFromTextFormat(pTextFormat_header), LPARAM(TRUE));

		return 0;
	}

	case WM_DESTROY: {
		DiscardGraphicsResources();
		SafeRelease(&pFactory);
		PostQuitMessage(0);
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

	case WM_MOUSEWHEEL: {
		if (start_arr.size() != 0) {
			INT16 wheelDirection = HIWORD(wParam);
			if (wheelDirection > 0 && start_index >= 36) {
				start_index -= 36;
			}
			else if (wheelDirection < 0 && start_index + 36 < start_arr.size()) {
				start_index += 36;
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

void MainWindow::QuickSort(int low, int high) {
	if (low < high) {

		int pi = HoarePartition(low, high);

		QuickSort(low, pi);
		QuickSort(pi + 1, high);
	}
}

int MainWindow::HoarePartition(int low, int high) {
	std::wstring size_pivot = end_arr[(low + high) / 2].second;
	std::wstring name_pivot = end_arr[(low + high) / 2].first;

	int l = low;
	int r = high;

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
