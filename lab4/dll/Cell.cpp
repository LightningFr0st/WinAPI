#include "pch.h"
#include "Cell.h"
#include "resource2.h"

void Cell::Draw(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush,
	FLOAT left, FLOAT top, FLOAT right, FLOAT bottom, DWRITE_TEXT_ALIGNMENT align) {

	D2D1_RECT_F textLayout = D2D1::RectF(left, top, right, bottom);

	(*textFormat)->SetTextAlignment(align);

	pRenderTarget->DrawTextW(content.c_str(),
		static_cast<UINT32>(content.length()), *textFormat, textLayout, pBrush);

}

WCHAR newText[256];

LRESULT CALLBACK InputDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			GetDlgItemText(hDlg, IDC_EDIT_TEXT, newText, 256);

		case IDCANCEL:
			EndDialog(hDlg, wParam);
			return TRUE;
		}
	}
	return FALSE;
}

LRESULT CALLBACK EditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	Cell* pThis = (Cell*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (msg) {
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {

			int length = GetWindowTextLengthW(pThis->hwndEdit);
			LPWSTR buffer = new WCHAR[length + 1];
			GetWindowTextW(pThis->hwndEdit, buffer, length + 1);

			pThis->content = std::wstring(buffer);
			DestroyWindow(pThis->hwndEdit);
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

void Cell::LMBHandle() {

	if (!isStatic) {

		hwndEdit = CreateWindowExW(
			0,
			L"EDIT",
			NULL,
			WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
			col * width + 1,
			row * height + 1,
			width - 2,
			height - 2,
			parent,
			NULL,
			g_hModule,
			NULL);


		SetWindowText(hwndEdit, content.c_str());

		this->content = L"";

		SetWindowLongPtr(hwndEdit, GWLP_USERDATA, (LONG_PTR)this);

		OriginalEditProc = (WNDPROC)SetWindowLongPtr(hwndEdit, GWLP_WNDPROC, (LONG_PTR)EditProc);

		SendMessage(hwndEdit, WM_SETFONT, (WPARAM)CreateHFONTFromTextFormat(*textFormat), LPARAM(TRUE));

		SetFocus(hwndEdit);

		RECT rect;
		SendMessage(hwndEdit, EM_GETRECT, 0, (LPARAM)&rect);
		rect.top = 50;
		rect.bottom = static_cast<LONG>(round(height));
		SendMessage(hwndEdit, EM_SETRECT, 0, (LPARAM)&rect);

		/*
		wcscpy_s(newText, this->content.c_str());
		if (DialogBox(g_hModule,
			MAKEINTRESOURCE(DLG_CHANGE_TEXT),
			parent,
			(DLGPROC)InputDialogProc) == IDOK)
		{
			this->content = std::wstring(newText);
		}
		return;
		*/
	}
	SendMessage(parent, WM_LMB, row, col);

}

void Cell::CtrlLMBHandle() {

	SendMessage(parent, WM_CTRLLMB, row, col);
}

void Cell::RMBHandle() {

	SendMessage(parent, WM_RMB, row, col);
}

void Cell::DLMBHandle() {

	SendMessage(parent, WM_DLMB, row, col);
}