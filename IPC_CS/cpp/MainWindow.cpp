#pragma once
#include "MainWindow.h"

#include <d2d1_1helper.h>


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

		MoveWindow(hwndEdit, 10, 10, width - 20, 0.1 * height, TRUE);
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

				hr = pDWriteFactory->CreateTextFormat(L"Times New Roman", NULL, DWRITE_FONT_WEIGHT_REGULAR,
					DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 26.0f, L"en-us", &pTextFormat);


				if (SUCCEEDED(hr)) {

					hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

					if (SUCCEEDED(hr)) {
						hr = pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
					}
				}
			}
		}

	}
	return hr;
}

void MainWindow::DiscardGraphicsResources()
{
	CloseHandle(hThread);

	UnmapViewOfFile(sharedMemory);
	CloseHandle(hMapFile);

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

		FLOAT newHeight = height - 10, newWidth = width - 10;

		FLOAT x1 = AlignPixel(10);
		FLOAT x2 = AlignPixel(newWidth);

		FLOAT y1 = AlignPixel(0.1 * height + 60);
		FLOAT y2 = AlignPixel(newHeight);

		D2D1_POINT_2F bottomLeft = D2D1::Point2F(x1, y2);
		D2D1_POINT_2F bottomRight = D2D1::Point2F(x2, y2);

		D2D1_POINT_2F topLeft = D2D1::Point2F(x1, y1);
		D2D1_POINT_2F topRight = D2D1::Point2F(x2, y1);

		D2D1_RECT_F textLayout = D2D1::RectF(x1, y1 - 40, x2, y1);

		pRenderTarget->DrawTextW(L"Message:", static_cast<UINT32>(wcslen(L"Message:")), pTextFormat, textLayout, pBrush);

		pRenderTarget->DrawLine(topLeft, bottomLeft, pBrush);

		pRenderTarget->DrawLine(topRight, bottomRight, pBrush);

		pRenderTarget->DrawLine(bottomLeft, bottomRight, pBrush);

		pRenderTarget->DrawLine(topLeft, topRight, pBrush);


		textLayout = D2D1::RectF(x1, y1, x2, y2);
		pRenderTarget->DrawTextW(message, 100 * 1024, pTextFormat, textLayout, pBrush);

		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}

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

void MainWindow::WriteToSharedMemory(LPWSTR buffer, int length) {
	memset(sharedMemory->buffer, 0, sizeof(sharedMemory->buffer));
	CopyMemory(sharedMemory->buffer, buffer, length * sizeof(WCHAR));

	sharedMemory->cppNew = 1;

	memset(message, 0, sizeof(message));
	CopyMemory(message, buffer, length * sizeof(WCHAR));
}

void MainWindow::CreateEditControl() {

	hwndEdit = CreateWindowExW(
		0, L"EDIT",   // predefined class 
		NULL,         // no window title 
		WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER | ES_MULTILINE,
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
		SendMessage(hwndEdit, WM_SETFONT, (WPARAM)CreateHFONTFromTextFormat(pTextFormat), LPARAM(TRUE));

		hThread = CreateThread(
			NULL,             // Атрибуты безопасности
			0,                // Размер стека (0 = по умолчанию)
			MessageReaderThread,
			this,             // Аргумент для потока
			0,                // Флаги создания
			NULL              // ID потока
		);

		if (!hThread) {
			MessageBox(NULL, L"Failed to create thread!", L"Error", MB_OK | MB_ICONERROR);
		}

		SetFocus(hwndEdit);


		return 0;
	}

	case WM_USER_NEW_MESSAGE: {

		InvalidateRect(m_hwnd, NULL, FALSE);

		return 0;
	}


	case WM_DESTROY: {
		DiscardGraphicsResources();
		SafeRelease(&pFactory);

		// Поток будет работать в фоновом режиме

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

	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

DWORD WINAPI MainWindow::MessageReaderThread(LPVOID param) {
	MainWindow* pThis = static_cast<MainWindow*>(param);

	while (true) {
		if (pThis->sharedMemory->csNew == 1) {
			// Скопировать сообщение из разделяемой памяти
			ZeroMemory(pThis->message, sizeof(message));
			CopyMemory(pThis->message, pThis->sharedMemory->buffer, sizeof(message));

			// Сбросить флаг
			pThis->sharedMemory->csNew = 0;

			// Обновить интерфейс
			PostMessage(pThis->m_hwnd, WM_USER_NEW_MESSAGE, 0, reinterpret_cast<LPARAM>(pThis->message));
		}
		Sleep(100); // Предотвращение активного ожидания
	}

	return 0;
}

LRESULT CALLBACK MainWindow::EditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	MainWindow* pThis = (MainWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (msg) {
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {

			int length = GetWindowTextLengthW(pThis->hwndEdit);
			LPWSTR buffer = new WCHAR[length + 1];
			GetWindowTextW(pThis->hwndEdit, buffer, length + 1);

			pThis->WriteToSharedMemory(buffer, length + 1);

			GetWindowTextW(pThis->hwndEdit, buffer, length + 1);
			InvalidateRect(pThis->m_hwnd, NULL, FALSE);
			
			//SetFocus(pThis->m_hwnd);
			
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
