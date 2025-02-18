#include "pch.h"
#include "Header.h"



void Header::Draw(ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush, FLOAT cellW, FLOAT cellH) {

	FLOAT x = 0;
	for (int i = 0; i < columns.size(); i++) {
		if (i == 0) {
			columns[i]->Draw(pRenderTarget, pBrush, x, 0, x + cellW, cellH, DWRITE_TEXT_ALIGNMENT_LEADING);
		}
		else {
			columns[i]->Draw(pRenderTarget, pBrush, x, 0, x + cellW, cellH, DWRITE_TEXT_ALIGNMENT_CENTER);
		}
		x += cellW;
	}
}

void Header::LMBHandle(int column) {

	if (column == 0) {
		columns[0]->LMBHandle();
	}
	SendMessage(parent, WM_LMB_H, column, 0);
}