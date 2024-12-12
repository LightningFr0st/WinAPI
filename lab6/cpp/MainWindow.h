#pragma once

#include "BaseWindow.h"

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <vector>
#include <string>
#include <map>

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")

#define AlignPixel(x)  floorf((x)) + 0.5F
#define WM_USER_NEW_MESSAGE (WM_USER + 1)


class MainWindow : public BaseWindow<MainWindow>
{
	struct SharedMemory {
		int cppNew;
		int csNew;
		wchar_t buffer[100 * 1024];
	};

	WCHAR message[100 * 1024];

	static DWORD WINAPI MessageReaderThread(LPVOID param);

	HWND hwndEdit;
	static LRESULT EditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	WNDPROC OriginalEditProc;

	ID2D1Factory* pFactory;
	ID2D1HwndRenderTarget* pRenderTarget;
	ID2D1SolidColorBrush* pBrush;
	ID2D1SolidColorBrush* pBrushRed;

	IDWriteFactory* pDWriteFactory;

	IDWriteTextFormat* pTextFormat;

	void WriteToSharedMemory(LPWSTR buffer, int length);

	void    CalculateLayout();
	HRESULT CreateGraphicsResources();
	void CreateEditControl();
	void    DiscardGraphicsResources();
	void    OnPaint();
	void    Resize();

	FLOAT xm, ym;
	FLOAT height, width;

	HANDLE hMapFile;
	HANDLE hThread;

	LPCWSTR SHARED_MEMORY_NAME = L"MSG_EXCH";

	SharedMemory* sharedMemory;

public:

	MainWindow() {

		pTextFormat = NULL;
		pDWriteFactory = NULL;
		pBrushRed = NULL;
		pBrush = NULL;
		pRenderTarget = NULL;
		pFactory = NULL;
		OriginalEditProc = NULL;

		hMapFile = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(SharedMemory), SHARED_MEMORY_NAME);

		if (hMapFile == nullptr) {
			std::wcerr << L"Failed to create shared memory: " << GetLastError() << std::endl;
			return;
		}

		sharedMemory = (SharedMemory*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedMemory));
		if (sharedMemory == nullptr) {
			std::wcerr << L"Failed to map shared memory: " << GetLastError() << std::endl;
			CloseHandle(hMapFile);
			return;
		}

	}

	PCWSTR  ClassName() const { return L"Base Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};