﻿// ChessGameClient.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "ChessGameClient.h"
#include "WindowPainter.hpp"
#include "Board.hpp"
#include <iostream>

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
WindowPainter windowPainter;                    // 
Board board;
bool isDragging = false;
bool isMemorized = false;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.
    // Start up GDI+.
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CHESSGAMECLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHESSGAMECLIENT));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    GdiplusShutdown(gdiplusToken);

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHESSGAMECLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CHESSGAMECLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

std::wstring GetExePath() {
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buffer, MAX_PATH);
    return std::wstring(buffer).substr(0, std::wstring(buffer).find_last_of(L"\\/"));
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        //window init
        windowPainter.LoadSprites(&board);
        windowPainter.SetWindow(hWnd);    
        windowPainter.CreateBuffer(hWnd);

        //game init
        board.SetUpFigures();
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {   
            //init
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            if (!isMemorized) {
                BitBlt(windowPainter.bufferDC, windowPainter.windowRect.left, windowPainter.windowRect.top, windowPainter.windowRect.right - windowPainter.windowRect.left,
                    windowPainter.windowRect.bottom - windowPainter.windowRect.top, hdc, windowPainter.windowRect.left, windowPainter.windowRect.top, SRCCOPY);
            }
            else {
                BitBlt(windowPainter.bufferDC, windowPainter.windowRect.left, windowPainter.windowRect.top, windowPainter.windowRect.right - windowPainter.windowRect.left,
                    windowPainter.windowRect.bottom - windowPainter.windowRect.top, windowPainter.memoryDC, windowPainter.windowRect.left, windowPainter.windowRect.top, SRCCOPY);
            }
            
            windowPainter.SetHDC(windowPainter.bufferDC);

            // draw
            if (!isMemorized) {
                windowPainter.DrawField(&board);
                windowPainter.DrawFigures(&board);
                if (isDragging && !isMemorized) {
                    BitBlt(windowPainter.memoryDC, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
                        ps.rcPaint.bottom - ps.rcPaint.top, windowPainter.bufferDC, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
                    isMemorized = true;
                }
            }           
            windowPainter.DrawSelectedFigure(&board);

            //finish
            BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
                ps.rcPaint.bottom - ps.rcPaint.top, windowPainter.bufferDC, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_SIZE:
        windowPainter.SetWindow(hWnd);
        break;
    case WM_SIZING:
        windowPainter.SetWindow(hWnd);
        break;
    case WM_MOUSEMOVE:
        if (isDragging) {
            windowPainter.xMousePos = LOWORD(lParam);
            windowPainter.yMousePos = HIWORD(lParam);

            InvalidateRect(hWnd, &windowPainter.windowRect, FALSE);
        }
        break;
    case WM_LBUTTONDOWN: 
        if (!isDragging) {
            if (board.TrySelectFigure(LOWORD(lParam), HIWORD(lParam))) {
                windowPainter.xMousePos = LOWORD(lParam);
                windowPainter.yMousePos = HIWORD(lParam);

                isDragging = true;
                isMemorized = false;
                InvalidateRect(hWnd, &windowPainter.windowRect, FALSE);
            }           
        }
        break;
    case WM_LBUTTONUP:
        if (isDragging) {
            if (board.TrySetFigureInCell(LOWORD(lParam), HIWORD(lParam))) {
                isDragging = false;
                isMemorized = false;
                InvalidateRect(hWnd, &windowPainter.windowRect, FALSE);
            }    
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
