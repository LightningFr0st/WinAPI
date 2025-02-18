#include "Header.h"

void Header::Draw(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush, FLOAT cellW, FLOAT cellH) {

	FLOAT x = 0;
	for (auto str : columns) {
		D2D1_RECT_F textLayout = D2D1::RectF(x, 0, x + cellW, cellH);
		pRenderTarget->DrawTextW(str.c_str(),
			static_cast<UINT32>(str.length()), *pTextFormatHeader, textLayout, pBrush);
		x += cellW;
	}
}

void Header::LMBHandle(int column) {

	SendMessage(parent, WM_LMB_H, column, 0);
}