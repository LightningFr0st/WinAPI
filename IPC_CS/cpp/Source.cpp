#include <windows.h>
#include <iostream>
#include <string>
#include <vector>


#include <shellscalingapi.h>
#pragma comment(lib, "shcore.lib")

#include "MainWindow.h"

using namespace std;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

    MainWindow win;

    if (!win.Create(L"CPP Client", WS_OVERLAPPEDWINDOW, 0, 0, 0, 600, 480))
    {
        return 0;
    }

    ShowWindow(win.Window(), nCmdShow);

    UpdateWindow(win.Window());

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}