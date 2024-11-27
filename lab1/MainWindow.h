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

class MainWindow : public BaseWindow<MainWindow>
{
    HWND hwndEdit;
    static LRESULT EditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    WNDPROC OriginalEditProc;

    ID2D1Factory* pFactory;
    ID2D1HwndRenderTarget* pRenderTarget;
    ID2D1SolidColorBrush* pBrush;
    ID2D1SolidColorBrush* pBrushRed;

    IDWriteFactory* pDWriteFactory;

    IDWriteTextFormat* pTextFormat_files;
    IDWriteTextFormat* pTextFormat_size;
    IDWriteTextFormat* pTextFormat_header;
    DWRITE_TRIMMING trimmingOptions = {};

    const wchar_t* wszText;
    UINT32 cTextLength;


    void    CalculateLayout();
    HRESULT CreateGraphicsResources();
    void CreateEditControl();
    void    DiscardGraphicsResources();
    void    OnPaint();
    void    Resize();

    FLOAT xm, ym;
    FLOAT height, width;

    int start_index = 0;

	std::vector<std::pair<std::wstring, std::wstring>> start_arr;
	std::vector<std::pair<std::wstring, std::wstring>> end_arr;

    std::map<std::wstring, std::pair<UINT32,UINT32>> repeatData;

public:

    MainWindow() : pFactory(NULL), pRenderTarget(NULL), pBrush(NULL)
    {
    }
    
    void InitArray(LPCWSTR start_dir);
    int HoarePartition(int p, int r);
    void QuickSort(int start, int end);
    void FinilizeArray();

    friend HFONT CreateHFONTFromTextFormat(IDWriteTextFormat* pTextFormat);

    PCWSTR  ClassName() const { return L"Base Window Class"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};