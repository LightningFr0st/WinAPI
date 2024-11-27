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
				if (SUCCEEDED(hr)) {
					trimmingOptions.granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;
					trimmingOptions.delimiter = 0;
					trimmingOptions.delimiterCount = 0;

					IDWriteInlineObject* pEllipsis = nullptr;

					hr = pDWriteFactory->CreateEllipsisTrimmingSign(pTextFormatDef, &pEllipsis);

					if (SUCCEEDED(hr)) {
						pTextFormatDef->SetTrimming(&trimmingOptions, pEllipsis);
					}
				}
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

void MainWindow::FullFillLeftTable()
{
	std::vector<std::wstring> temp;
	temp.reserve(fileArr.size());

	std::transform(fileArr.begin(), fileArr.end(), std::back_inserter(temp),
		[](const std::tuple<std::wstring, std::wstring, std::wstring>& t) {
			return std::get<0>(t);
		});

	MainTable->FillColumn(0, temp);

	temp.erase(temp.begin(), temp.end());

	std::transform(fileArr.begin(), fileArr.end(), std::back_inserter(temp),
		[](const std::tuple<std::wstring, std::wstring, std::wstring>& t) {
			return std::get<1>(t);
		});

	MainTable->FillColumn(1, temp);

	temp.erase(temp.begin(), temp.end());

	std::transform(fileArr.begin(), fileArr.end(), std::back_inserter(temp),
		[](const std::tuple<std::wstring, std::wstring, std::wstring>& t) {
			return std::get<2>(t);
		});

	MainTable->FillColumn(2, temp);
}

void MainWindow::FullFillRightTable()
{
	std::vector<std::wstring> temp;
	temp.reserve(fileArr.size());

	std::transform(fileArr.begin(), fileArr.end(), std::back_inserter(temp),
		[](const std::tuple<std::wstring, std::wstring, std::wstring>& t) {
			return std::get<0>(t);
		});

	MainTable->FillColumn(3, temp);

	temp.erase(temp.begin(), temp.end());

	std::transform(fileArr.begin(), fileArr.end(), std::back_inserter(temp),
		[](const std::tuple<std::wstring, std::wstring, std::wstring>& t) {
			return std::get<1>(t);
		});

	MainTable->FillColumn(4, temp);

	temp.erase(temp.begin(), temp.end());

	std::transform(fileArr.begin(), fileArr.end(), std::back_inserter(temp),
		[](const std::tuple<std::wstring, std::wstring, std::wstring>& t) {
			return std::get<2>(t);
		});

	MainTable->FillColumn(5, temp);
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

		MainTable->Draw();

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

struct SharedMemory {
	wchar_t buffer[1024 * 1024 * 10];
	size_t count;
};


void SerializeVectorToMemory(const std::vector<std::tuple<std::wstring, std::wstring, std::wstring>>& vec, SharedMemory* sharedMem) {
	sharedMem->count = vec.size();

	wchar_t* ptr = sharedMem->buffer;
	for (const auto& t : vec) {
		for (const auto& str : { std::get<0>(t), std::get<1>(t), std::get<2>(t) }) {
			wcscpy(ptr, str.c_str());
			ptr += str.size() + 1;
		}
	}

}

std::vector<std::tuple<std::wstring, std::wstring, std::wstring>> DeserializeVectorFromMemory(const SharedMemory* sharedMem) {
	std::vector<std::tuple<std::wstring, std::wstring, std::wstring>> vec(sharedMem->count);
	const wchar_t* ptr = sharedMem->buffer;

	for (auto& t : vec) {
		std::wstring first = ptr;
		ptr += first.size() + 1;
		std::wstring second = ptr;
		ptr += second.size() + 1;
		std::wstring third = ptr;
		ptr += third.size() + 1;
		t = std::make_tuple(first, second, third);
	}

	return vec;
}

void Merge(std::vector<std::tuple<std::wstring, std::wstring, std::wstring>>& vec, int start, int mid, int end, int sortIndex) {
	std::vector<std::tuple<std::wstring, std::wstring, std::wstring>> left(vec.begin() + start, vec.begin() + mid + 1);
	std::vector<std::tuple<std::wstring, std::wstring, std::wstring>> right(vec.begin() + mid + 1, vec.begin() + end + 1);

	int i = 0, j = 0, k = start;
	while (i < left.size() && j < right.size()) {
		if ((sortIndex == 0 && std::get<0>(left[i]) <= std::get<0>(right[j])) ||
			(sortIndex == 1 && std::get<1>(left[i]) <= std::get<1>(right[j])) ||
			(sortIndex == 2 && std::get<2>(left[i]) <= std::get<2>(right[j]))) {
			vec[k++] = left[i++];
		}
		else {
			vec[k++] = right[j++];
		}
	}

	while (i < left.size()) {
		vec[k++] = left[i++];
	}
	while (j < right.size()) {
		vec[k++] = right[j++];
	}
}

void CallChildProcess(int start, int end, int sortIndex) {
	std::wstring args = L"Sorter.exe " + std::to_wstring(start) + L" " + std::to_wstring(end) + L" " + std::to_wstring(sortIndex);

	std::vector<wchar_t> cmdLine(args.begin(), args.end());
	cmdLine.push_back(L'\0');


	STARTUPINFOW si = { sizeof(si) };
	PROCESS_INFORMATION pi = {};

	if (!CreateProcessW(nullptr, cmdLine.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
		std::wcerr << L"Failed to create child process: " << GetLastError() << std::endl;
		return;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

PROCESS_INFORMATION CallChildProcessN(int start, int end, int sortIndex) {
	std::wstring args = L"Sorter.exe " + std::to_wstring(start) + L" " + std::to_wstring(end) + L" " + std::to_wstring(sortIndex);

	std::vector<wchar_t> cmdLine(args.begin(), args.end());
	cmdLine.push_back(L'\0');

	STARTUPINFOW si = { sizeof(si) };
	PROCESS_INFORMATION pi = {};

	if (!CreateProcessW(nullptr, cmdLine.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
		std::wcerr << L"Failed to create child process: " << GetLastError() << std::endl;
		throw std::runtime_error("Failed to create process");
	}

	return pi;
}


void MainWindow::LoadNewProc(int sortIndex) {
	HANDLE hMapFile = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(SharedMemory), SHARED_MEMORY_NAME);
	if (hMapFile == nullptr) {
		std::wcerr << L"Failed to create shared memory: " << GetLastError() << std::endl;
		return;
	}

	SharedMemory* sharedMem = (SharedMemory*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedMemory));
	if (sharedMem == nullptr) {
		std::wcerr << L"Failed to map shared memory: " << GetLastError() << std::endl;
		CloseHandle(hMapFile);
		return;
	}

	SerializeVectorToMemory(fileArr, sharedMem);

	int numProcesses = 4; // Количество процессов
	std::vector<std::pair<int, int>> ranges;

	int chunkSize = fileArr.size() / numProcesses;
	for (int i = 0; i < numProcesses; ++i) {
		int start = i * chunkSize;
		int end = (i == numProcesses - 1) ? (fileArr.size() - 1) : (start + chunkSize - 1);
		ranges.emplace_back(start, end);
		CallChildProcess(start, end, sortIndex);
	}

	auto vec = DeserializeVectorFromMemory(sharedMem);

	for (int step = 1; step < numProcesses; step *= 2) {
		for (int i = 0; i + step < numProcesses; i += 2 * step) {

			int start = ranges[i].first;
			int mid = ranges[i].second;
			int end = ranges[i + step].second;

			Merge(vec, start, mid, end, sortIndex);
			ranges[i].second = end; // Обновляем диапазон объединенной части
		}
	}

	fileArr = vec;

	UnmapViewOfFile(sharedMem);
	CloseHandle(hMapFile);
}

void MainWindow::LoadNewProcN(int sortIndex) {
	HANDLE hMapFile = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(SharedMemory), SHARED_MEMORY_NAME);
	if (hMapFile == nullptr) {
		std::wcerr << L"Failed to create shared memory: " << GetLastError() << std::endl;
		return;
	}

	SharedMemory* sharedMem = (SharedMemory*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedMemory));
	if (sharedMem == nullptr) {
		std::wcerr << L"Failed to map shared memory: " << GetLastError() << std::endl;
		CloseHandle(hMapFile);
		return;
	}

	SerializeVectorToMemory(fileArr, sharedMem);

	int numProcesses = 4;
	std::vector<std::pair<int, int>> ranges;
	std::vector<PROCESS_INFORMATION> processInfos;
	std::vector<HANDLE> processHandles;

	// Разделение на диапазоны
	int chunkSize = fileArr.size() / numProcesses;
	for (int i = 0; i < numProcesses; ++i) {
		int start = i * chunkSize;
		int end = (i == numProcesses - 1) ? (fileArr.size() - 1) : (start + chunkSize - 1);
		ranges.emplace_back(start, end);

		// Запуск дочернего процесса
		try {
			auto pi = CallChildProcessN(start, end, sortIndex);
			processInfos.push_back(pi);
			processHandles.push_back(pi.hProcess);
		}
		catch (const std::exception& e) {
			std::wcerr << L"Error launching child process: " << e.what() << std::endl;
			continue;
		}
	}

	while (processHandles.size() > 1) {
		
		DWORD index1 = WaitForMultipleObjects(processHandles.size(), processHandles.data(), FALSE, INFINITE);
		HANDLE completedProcess1 = processHandles[index1];
		CloseHandle(completedProcess1);

		processHandles.erase(processHandles.begin() + index1);
		DWORD index2 = WaitForMultipleObjects(processHandles.size(), processHandles.data(), FALSE, INFINITE);
		HANDLE completedProcess2 = processHandles[index2];
		CloseHandle(completedProcess2);

		processHandles.erase(processHandles.begin() + index2);

		int rangeIndex1 = index1 < index2 ? index1 : index2 - 1;
		int rangeIndex2 = index1 < index2 ? index2 - 1 : index1;

		int start = ranges[rangeIndex1].first;
		int mid = ranges[rangeIndex1].second;
		int end = ranges[rangeIndex2].second;

		auto vec = DeserializeVectorFromMemory(sharedMem);
		Merge(vec, start, mid, end, sortIndex);

		ranges[rangeIndex1].second = end;
		ranges.erase(ranges.begin() + rangeIndex2);
	}

	fileArr = DeserializeVectorFromMemory(sharedMem);

	UnmapViewOfFile(sharedMem);
	CloseHandle(hMapFile);
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

		MainTable = new Table(20, 6, Window(), pRenderTarget, pBrush, &pTextFormatDef, &pTextFormatHeader);

		MainTable->RenameHeader(headerNames);

		SetCursor(LoadCursor(NULL, IDC_ARROW));

		return 0;
	}

	case WM_DESTROY: {
		RemoveCustomFonts();
		DiscardGraphicsResources();
		SafeRelease(&pFactory);
		PostQuitMessage(0);
		return 0;
	}

	case WM_LBUTTONDBLCLK: {
		pendingSingleClick = false;
		KillTimer(m_hwnd, SINGLE_CLICK_TIMER_ID);

		MainTable->DLMBHandle(LOWORD(lParam), HIWORD(lParam));
		break;
	}

	case WM_RBUTTONDOWN: {

		MainTable->RMBHandle(LOWORD(lParam), HIWORD(lParam));

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
					MainTable->CtrlLMBHandle(clickPoint.x, clickPoint.y);
				}
				else {
					MainTable->LMBHandle(clickPoint.x, clickPoint.y);
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

		/*int row = static_cast<int>(wParam);
		int col = static_cast<int>(lParam);*/



		return 0;
	}

	case WM_DLMB: {
		/*int row = static_cast<int>(wParam);
		int col = static_cast<int>(lParam);

		SYSTEMTIME st;
		GetLocalTime(&st);

		std::wstring time = L"Time: " + std::to_wstring(st.wHour) + L':' + std::to_wstring(st.wMinute) + L':'
			+ std::to_wstring(st.wSecond) + L':' + std::to_wstring(st.wMilliseconds);

		std::wstring result = L"WM_DLMB handled\nCell: (" + std::to_wstring(row) + L"," + std::to_wstring(col) + L")\n"
			+ time;

		MessageBoxW(NULL, result.c_str(), L"Event handled", MB_OK);*/

		return 0;
	}

	case WM_RMB: {
		/*int row = static_cast<int>(wParam);
		int col = static_cast<int>(lParam);

		SYSTEMTIME st;
		GetLocalTime(&st);

		std::wstring time = L"Time: " + std::to_wstring(st.wHour) + L':' + std::to_wstring(st.wMinute) + L':'
			+ std::to_wstring(st.wSecond) + L':' + std::to_wstring(st.wMilliseconds);

		std::wstring result = L"WM_RMB handled\nCell: (" + std::to_wstring(row) + L"," + std::to_wstring(col) + L")\n"
			+ time;

		MessageBoxW(NULL, result.c_str(), L"Event handled", MB_OK);*/
		return 0;
	}

	case WM_CTRLLMB: {
		/*int row = static_cast<int>(wParam);
		int col = static_cast<int>(lParam);
		SYSTEMTIME st;
		GetLocalTime(&st);

		std::wstring time = L"Time: " + std::to_wstring(st.wHour) + L':' + std::to_wstring(st.wMinute) + L':'
			+ std::to_wstring(st.wSecond) + L':' + std::to_wstring(st.wMilliseconds);

		std::wstring result = L"WM_CTRLLMB handled\nCell:	(" + std::to_wstring(row) + L"," + std::to_wstring(col) + L")\n"
			+ time;

		MessageBoxW(NULL, result.c_str(), L"Event handled", MB_OK);*/
		return 0;
	}

	case WM_LMB_H: {
		int col = static_cast<int>(wParam);

		if (col >= 3) {
			LoadNewProc(col - 3);

			FullFillRightTable();
		}


		/*SYSTEMTIME st;
		GetLocalTime(&st);

		std::wstring time = L"Time: " + std::to_wstring(st.wHour) + L':' + std::to_wstring(st.wMinute) + L':'
			+ std::to_wstring(st.wSecond) + L':' + std::to_wstring(st.wMilliseconds);

		std::wstring result = L"WM_LMB_H handled\nHeader: (" + std::to_wstring(col + 1) + L")\n"
			+ time;

		MessageBoxW(NULL, result.c_str(), L"Event handled", MB_OK);*/

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
		else if (wParam == 'S' || wParam == 's') {

			InitArray((MainTable->header->columns[0]->content + L"*").c_str());

			FullFillLeftTable();

			LoadNewProc(0);

			FullFillRightTable();

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
		if (wheelDirection > 0 && MainTable->startInd > 0) {

			MainTable->startInd--;
		}
		else if (wheelDirection < 0 && MainTable->startInd + MainTable->rowsDisplay < MainTable->nRows - 1) {
			MainTable->startInd++;
		}

		InvalidateRect(m_hwnd, NULL, FALSE);

		return 0;
	}

	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
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

					FILETIME* fileTime = &data.ftCreationTime;

					SYSTEMTIME sysTime;

					FileTimeToSystemTime(fileTime, &sysTime);

					TCHAR buf[200];

					GetTimeFormat(LOCALE_NAME_USER_DEFAULT, TIME_NOTIMEMARKER, &sysTime, NULL, buf, 200);

					std::wstring timeCreation = std::wstring(buf) + L":";

					if (sysTime.wMilliseconds < 10) {
						timeCreation += L"00" + std::to_wstring(sysTime.wMilliseconds);
					}
					else if (sysTime.wMilliseconds < 100) {
						timeCreation += L"0" + std::to_wstring(sysTime.wMilliseconds);
					}
					else {
						timeCreation += std::to_wstring(sysTime.wMilliseconds);
					}


					memset(buf, 0, sizeof(buf));

					GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &sysTime, NULL, buf, 200);

					timeCreation = timeCreation + L" " + std::wstring(buf);

					unsigned long long fileSize = (data.nFileSizeHigh * (MAXDWORD + 1ULL)) + data.nFileSizeLow;

					fileArr.push_back(std::make_tuple(fileName,
						std::to_wstring(fileSize), timeCreation));

				}
			}

		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
}

//void MainWindow::QuickSort(size_t low, size_t high) {
//	if (low < high) {
//		size_t pi = HoarePartition(low, high);
//
//		QuickSort(low, pi);
//		QuickSort(pi + 1, high);
//	}
//}


//size_t MainWindow::HoarePartition(size_t low, size_t high) {
//	std::wstring size_pivot = std::get<1>(end_arr[(low + high) / 2]);
//	std::wstring name_pivot = std::get<0>(end_arr[(low + high) / 2]);
//
//	size_t l = low;
//	size_t r = high;
//
//	while (true) {
//		while (end_arr[l].second < size_pivot ||
//			(end_arr[l].second == size_pivot) && (end_arr[l].first < name_pivot))
//		{
//			l++;
//		}
//
//		while (end_arr[r].second > size_pivot ||
//			(end_arr[r].second == size_pivot) && (end_arr[r].first > name_pivot))
//		{
//			r--;
//		}
//
//		if (r <= l) return r;
//		swap(end_arr[l++], end_arr[r--]);
//	}
//}


//void MainWindow::FinilizeArray() {
//	std::wstring fileName;
//	for (int i = 0; i < end_arr.size(); i++) {
//		fileName = end_arr[i].first;
//		if (repeatData[fileName].first > 1) {
//			UINT32 num = repeatData[fileName].second - repeatData[fileName].first--;
//			if (num != 0) {
//				end_arr[i].first = fileName + L"["
//					+ std::to_wstring(num) + L"]";
//			}
//		}
//	}
//}
