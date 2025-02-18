#include "Table.h"



Table::Table(int N, int M, HWND window, ID2D1HwndRenderTarget* target, ID2D1SolidColorBrush* brush,
	IDWriteTextFormat** Def, IDWriteTextFormat** Header)
	: nRows(N), nCols(M), parent(window), header(M, window, Header), pTextFormatDef(Def), pRenderTarget(target), pBrush(brush) {
	rowsDisplay = N - 1;

	D2D1_SIZE_F size = pRenderTarget->GetSize();

	height = size.height;
	width = size.width;

	cellH = height / N;
	cellW = width / M;

	FLOAT x = 0, y = cellH;
	cells.resize(nRows - 1);
	for (int r = 0; r < nRows - 1; ++r) {
		cells[r].resize(nCols);
		x = 0;
		for (int c = 0; c < nCols; ++c) {	
			cells[r][c] = new Cell(r + 1, c + 1, parent, L"Static" + std::to_wstring(gen++));
			x += cellW;
		}
		y += cellH;
	}
}

void Table::Draw() {

	D2D1_SIZE_F size = pRenderTarget->GetSize();

	height = size.height;
	width = size.width;

	cellH = height / (rowsDisplay + 1);
	cellW = width / nCols;

	pRenderTarget->DrawLine(D2D1::Point2F(AlignPixel(0), AlignPixel(0)),
		D2D1::Point2F(AlignPixel(0), AlignPixel(height - 1)), pBrush);
	pRenderTarget->DrawLine(D2D1::Point2F(AlignPixel(0), AlignPixel(0)),
		D2D1::Point2F(AlignPixel(width - 1), AlignPixel(0)), pBrush);
	pRenderTarget->DrawLine(D2D1::Point2F(AlignPixel(width - 1), AlignPixel(0)),
		D2D1::Point2F(AlignPixel(width - 1), AlignPixel(height - 1)), pBrush);
	pRenderTarget->DrawLine(D2D1::Point2F(AlignPixel(0), AlignPixel(height - 1)),
		D2D1::Point2F(AlignPixel(width - 1), AlignPixel(height - 1)), pBrush);

	FLOAT x = cellW, y = cellH;
	for (int i = 0; i < nRows - 1; i++) {
		pRenderTarget->DrawLine(D2D1::Point2F(AlignPixel(0), AlignPixel(y)),
			D2D1::Point2F(AlignPixel(width), AlignPixel(y)), pBrush);
		y += cellH;
	}

	for (int i = 0; i < nCols - 1; i++) {
		pRenderTarget->DrawLine(D2D1::Point2F(AlignPixel(x), AlignPixel(0)),
			D2D1::Point2F(AlignPixel(x), AlignPixel(height)), pBrush);
		x += cellW;
	}

	header.Draw(pRenderTarget, pBrush, cellW, cellH);

	x = 0, y = cellH;

	for (int i = startInd; i < min(startInd + rowsDisplay, nRows - 1); i++) {
		x = 0;
		for (int j = 0; j < nCols; j++) {
			cells[i][j]->Draw(pRenderTarget, pBrush, x, y, x + cellW, y + cellH, pTextFormatDef);
			x += cellW;
		}
		y += cellH;
	}
}


void Table::AddRow(std::wstring templ) {
	std::vector<Cell*> newRow;

	for (int i = 0; i < nCols; i++) {
		newRow.push_back(new Cell(nRows, i + 1, parent, templ + std::to_wstring(gen++), false));
	}
	nRows++;
	cells.push_back(newRow);
}

void Table::LMBHandle(int x, int y) {
	int r = y / (int)cellH;
	int c = x / (int)cellW;
	if (r == 0) {
		header.LMBHandle(c);
	}
	else {
		cells[r + startInd - 1][c]->LMBHandle();
	}
}

void Table::CtrlLMBHandle(int x, int y) {
	int r = y / (int)cellH;
	int c = x / (int)cellW;
	if (r > 0) {
		cells[r + startInd - 1][c]->CtrlLMBHandle();
	}
}

void Table::RMBHandle(int x, int y) {
	int r = y / (int)cellH;
	int c = x / (int)cellW;
	if (r > 0) {
		cells[r + startInd - 1][c]->RMBHandle();
	}
}

void Table::DLMBHandle(int x, int y) {
	int r = y / (int)cellH;
	int c = x / (int)cellW;
	if (r > 0) {
		cells[r + startInd - 1][c]->DLMBHandle();
	}
}
