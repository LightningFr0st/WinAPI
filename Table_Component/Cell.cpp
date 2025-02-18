#include "Cell.h"
#include "resource2.h"

void Cell::Draw(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush,
	FLOAT left, FLOAT top, FLOAT right, FLOAT bottom, IDWriteTextFormat** Def) {

	D2D1_RECT_F textLayout = D2D1::RectF(left, top, right, bottom);
	pRenderTarget->DrawTextW(content.c_str(),
		static_cast<UINT32>(content.length()), *Def, textLayout, pBrush);

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


void Cell::LMBHandle() {

	if (!isStatic) {
		wcscpy_s(newText, this->content.c_str());
		if (DialogBox(GetModuleHandle(NULL),
			MAKEINTRESOURCE(DLG_CHANGE_TEXT),
			parent,
			(DLGPROC)InputDialogProc) == IDOK)
		{
			this->content = std::wstring(newText);
		}
		return;
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