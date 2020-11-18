// ChessGameClient.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "ChessGameClient.h"
#include "WindowPainter.hpp"
#include "Board.hpp"
#include "Game.hpp"
#include <iostream>

#include <CommCtrl.h>
#pragma comment(lib, "ComCtl32.Lib")

#define MAX_LOADSTRING 100
#define IDC_LISTVIEW 1111

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
WindowPainter windowPainter;                    // 
Game game;
LPRECT cursorClip;
bool isDragging = false;
bool isMemorized = false;

//Controls
HWND hWndListView;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

HWND CreateListView(HWND hwndParent)
{
    INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    RECT rcClient;                       // The parent window's client area.

    GetClientRect(hwndParent, &rcClient);

    // Create the list-view window in report view with label editing enabled.
    HWND hWndListView = CreateWindowEx(NULL,
        WC_LISTVIEW,
        L"",
        WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
        800, 0,
        rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top,
        hwndParent,
        (HMENU)IDC_LISTVIEW,
        GetModuleHandle(NULL),
        NULL);

    LVCOLUMN lvc;
    lvc.mask = LVCF_WIDTH | LVCF_TEXT;
    lvc.cx = 100;

    lvc.pszText = const_cast<LPWSTR>(L"White");
    ListView_InsertColumn(hWndListView, 0, &lvc);

    lvc.pszText = const_cast<LPWSTR>(L"Black");
    ListView_InsertColumn(hWndListView, 1, &lvc);

    return (hWndListView);
}

// SetView: Sets a list-view's window style to change the view.
// hWndListView: A handle to the list-view control. 
// dwView:       A value specifying the new view style.
//
VOID SetView(HWND hWndListView, DWORD dwView)
{
    // Retrieve the current window style. 
    DWORD dwStyle = GetWindowLong(hWndListView, GWL_STYLE);

    // Set the window style only if the view bits changed.
    if ((dwStyle & LVS_TYPEMASK) != dwView)
    {
        SetWindowLong(hWndListView,
            GWL_STYLE,
            (dwStyle & ~LVS_TYPEMASK) | dwView);
    }               // Logical OR'ing of dwView with the result of 
}                   // a bitwise AND between dwStyle and 
                    // the Unary complenent of LVS_TYPEMASK.


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
        hWndListView = CreateListView(hWnd);
        SetView(hWndListView, );
        ShowWindow(hWndListView, SW_SHOWDEFAULT);

        //window init
        windowPainter.LoadSprites(&game.board);
        windowPainter.SetWindow(hWnd, &game.board);
        windowPainter.CreateBuffer(hWnd);

        //game init
        game.InitGame();

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
                windowPainter.DrawLoggerWindow(Rect(game.board.boardImageInfo.width * game.board.boardInfo.boardSizeMult, 0, 300, game.board.boardImageInfo.height * game.board.boardInfo.boardSizeMult));

                windowPainter.DrawField(&game.board);
                windowPainter.DrawFigures(&game.board);
                windowPainter.DrawDangerHints(&game.board, game.CurrentActiveSide);

                if (isDragging && !isMemorized) {
                    windowPainter.DrawHintMoves(&game.board);

                    BitBlt(windowPainter.memoryDC, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
                        ps.rcPaint.bottom - ps.rcPaint.top, windowPainter.bufferDC, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
                    isMemorized = true;
                }
            }           
            windowPainter.DrawSelectedFigure(&game.board);

            //finish
            BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
                ps.rcPaint.bottom - ps.rcPaint.top, windowPainter.bufferDC, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_SIZE:
        windowPainter.SetWindow(hWnd, &game.board);
        break;
    case WM_SIZING:
        windowPainter.SetWindow(hWnd, &game.board);
        break;
    case WM_MOUSEMOVE:
        if (isDragging) {
            if (ClipCursor(&game.board.boardInfo.tagRect)) {
                windowPainter.xMousePos = LOWORD(lParam);
                windowPainter.yMousePos = HIWORD(lParam);

                InvalidateRect(hWnd, &windowPainter.windowRect, FALSE);
            }           
        }
        break;
    case WM_LBUTTONDOWN: 
        if (!isDragging) {
            if (game.TrySelectFigure(Point(LOWORD(lParam), HIWORD(lParam)))) {
                windowPainter.xMousePos = LOWORD(lParam);
                windowPainter.yMousePos = HIWORD(lParam);

                GetClipCursor(cursorClip);
                isDragging = true;
                isMemorized = false;
                InvalidateRect(hWnd, &windowPainter.windowRect, FALSE);
            }           
        }
        break;
    case WM_LBUTTONUP:
        if (isDragging) {
            game.TryMove(Point(LOWORD(lParam), HIWORD(lParam)));
            ClipCursor(cursorClip);
            isDragging = false;
            isMemorized = false;
            InvalidateRect(hWnd, &windowPainter.windowRect, FALSE);

            if ((game.CurrentActiveSide == 1 && game.Player2 == AI) || (game.CurrentActiveSide == 0 && game.Player1 == AI)) {
                game.AIMove();
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
