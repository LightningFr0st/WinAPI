#include <windows.h>
#include <errhandlingapi.h>
#include <iostream>
#include <string>
#include <vector>
#include <strsafe.h>

#include <shellscalingapi.h>
#pragma comment(lib, "shcore.lib")
#include "MainWindow.h"

using namespace std;

/*
D:\font\8bitOperatorPlus8-Regular.ttf
D:\font\Canterbury.ttf
D:\font\heorot.ttf
D:\font\mytype.ttf
D:\font\Ubuntu-Title.ttf
*/

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR pCmdLine, _In_ int nCmdShow)
{
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

	MainWindow win;

	if (!win.Create(L"File Sort", WS_POPUP, 0, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)))
	{
		return 0;
	}

	ShowWindow(win.Window(), nCmdShow);

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}