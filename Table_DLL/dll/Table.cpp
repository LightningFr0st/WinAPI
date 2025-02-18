#include "pch.h"
#include "Table.h"


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


Table::Table(int N, int M, HWND window, ID2D1HwndRenderTarget* target, ID2D1SolidColorBrush* brush,
	IDWriteTextFormat** DefText, IDWriteTextFormat** HeaderText)
	: nRows(N), nCols(M), parent(window), pTextFormatDef(DefText), pRenderTarget(target), pBrush(brush) {
	rowsDisplay = N - 1;

	D2D1_SIZE_F size = pRenderTarget->GetSize();

	height = size.height;
	width = size.width;

	cellH = height / N;
	cellW = width / M;

	header = new Header(M, window, cellH, cellW, HeaderText);

	FLOAT x = 0, y = cellH;
	cells.resize(nRows - 1);
	for (int r = 0; r < nRows - 1; ++r) {
		cells[r].resize(nCols);
		x = 0;
		for (int c = 0; c < nCols; ++c) {
			cells[r][c] = new Cell(r, c, parent, cellH, cellW, DefText, L"");
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

	header->Draw(pRenderTarget, pBrush, cellW, cellH);

	x = 0, y = cellH;

	for (int i = startInd; i < min(startInd + rowsDisplay, nRows - 1); i++) {
		x = 0;
		for (int j = 0; j < nCols; j++) {

			cells[i][j]->Draw(pRenderTarget, pBrush, x, y, x + cellW, y + cellH, DWRITE_TEXT_ALIGNMENT_CENTER);


			x += cellW;
		}
		y += cellH;
	}
}


void Table::AddRow(std::wstring templ) {
	std::vector<Cell*> newRow;

	for (int i = 0; i < nCols; i++) {
		newRow.push_back(new Cell(nRows, i + 1, parent, cellH, cellW, pTextFormatDef, templ, true));
	}
	nRows++;
	cells.push_back(newRow);
}

void Table::FillColumn(int start, std::vector<std::wstring> arr)
{
	while (nRows - 1 < arr.size()) {
		AddRow(L"Temp");
	}

	for (int i = 0; i < arr.size(); i++) {
		cells[i][start]->content = arr[i];
	}
}

void Table::LMBHandle(int x, int y) {
	int r = y / (int)cellH;
	int c = x / (int)cellW;
	if (r == 0) {
		header->LMBHandle(c);
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

void Table::RenameHeader(std::vector<std::wstring> names) {
	for (int i = 0; i < names.size(); i++) {
		header->columns[i]->content = names[i];
	}
}

void CreateTable(int N, int M, HWND window, ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush,
	IDWriteTextFormat** Def, IDWriteTextFormat** Header, void** ptr) {
	*ptr = static_cast<void*>(new Table(N, M, window, pRenderTarget, pBrush, Def, Header));
}

void AddRowF(std::wstring templ, void** ptr) {
	static_cast<Table*>(*ptr)->AddRow(templ);
}

void Draw(void** ptr) {
	static_cast<Table*>(*ptr)->Draw();
}

void LMBHandleW(int x, int y, void** ptr) {
	static_cast<Table*>(*ptr)->LMBHandle(x, y);
}
void CtrlLMBHandleW(int x, int y, void** ptr) {
	static_cast<Table*>(*ptr)->CtrlLMBHandle(x, y);
}
void RMBHandleW(int x, int y, void** ptr) {
	static_cast<Table*>(*ptr)->RMBHandle(x, y);
}
void DLMBHandleW(int x, int y, void** ptr) {
	static_cast<Table*>(*ptr)->DLMBHandle(x, y);
}

int getStartInd(void** ptr) {
	return static_cast<Table*>(*ptr)->startInd;
}

int getRows(void** ptr) {
	return static_cast<Table*>(*ptr)->nRows;
}

int getDisp(void** ptr)
{
	return static_cast<Table*>(*ptr)->rowsDisplay;
}

void Decr(void** ptr) {
	static_cast<Table*>(*ptr)->startInd--;
}

void Inc(void** ptr) {
	static_cast<Table*>(*ptr)->startInd++;
}