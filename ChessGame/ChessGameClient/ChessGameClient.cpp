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

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")  

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

Point listViewPos;
const int listViewWidth = 225;

//Controls
HWND hWndListView;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK ListViewProc(HWND hwnd,
    UINT msg, WPARAM wp, LPARAM lp, UINT_PTR, DWORD_PTR)
{
    switch (msg)
    {
    case WM_NCPAINT:
    {
        RECT rc;
        GetWindowRect(hwnd, &rc);
        OffsetRect(&rc, -rc.left, -rc.top);
        auto hdc = GetWindowDC(hwnd);
        auto hpen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
        auto oldpen = SelectObject(hdc, hpen);
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);//draw red frame
        SelectObject(hdc, oldpen);
        DeleteObject(oldpen);
        ReleaseDC(hwnd, hdc);


        return 0;
    }

    case WM_NCDESTROY:
        RemoveWindowSubclass(hwnd, ListViewProc, 0);
        break;
    }

    return DefSubclassProc(hwnd, msg, wp, lp);
}

HWND CreateListView(HWND hwndParent, Point pos)
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
        WS_CHILD |  LVS_REPORT  | LVS_EX_TRANSPARENTBKGND | WS_VSCROLL,
        pos.X, pos.Y,
        listViewWidth,
        rcClient.bottom - rcClient.top,
        hwndParent,
        (HMENU)IDC_LISTVIEW,
        GetModuleHandle(NULL),
        NULL);
    SetWindowSubclass(hWndListView, ListViewProc, 0, NULL);

    LVCOLUMN lvc;
    lvc.mask = LVCF_WIDTH | LVCF_TEXT;
    lvc.cxMin = 40;
    lvc.cx = 40;
    lvc.pszText = const_cast<LPWSTR>(L"№");
    ListView_InsertColumn(hWndListView, 0, &lvc);

    lvc.cx = 90;
    lvc.pszText = const_cast<LPWSTR>(L"White");
    ListView_InsertColumn(hWndListView, 1, &lvc);

    lvc.pszText = const_cast<LPWSTR>(L"Black");
    ListView_InsertColumn(hWndListView, 2, &lvc);

    return (hWndListView);
}

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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_SYSMENU | WS_CLIPCHILDREN,
      CW_USEDEFAULT, 0, 882, 700, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void LogMove(std::wstring move, int moveNum, int side) {
    if (side == 0) {
        LVITEM lvI;
        lvI.pszText = LPSTR_TEXTCALLBACK;
        lvI.mask = LVIF_TEXT;
        lvI.iSubItem = 0;
        lvI.iItem = moveNum;
        ListView_InsertItem(hWndListView, &lvI);
    }

    wchar_t buffer[256];
    wsprintfW(buffer, L"%d", moveNum + 1);

    ListView_SetItemText(hWndListView, moveNum, 0, buffer);
    ListView_SetItemText(hWndListView, moveNum, side + 1, const_cast<LPWSTR>(move.c_str()));
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
        windowPainter.LoadSprites(&game.board);
        windowPainter.SetWindow(hWnd, &game.board);
        windowPainter.CreateBuffer(hWnd);

        listViewPos = Point(game.board.boardInfo.rect.Width, 0);
        hWndListView = CreateListView(hWnd, listViewPos);
        ShowWindow(hWndListView, SW_SHOWDEFAULT);

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
        listViewPos = Point(game.board.boardInfo.rect.Width, 0);
        windowPainter.SetWindow(hWnd, &game.board);     
        SetWindowPos(
            hWndListView,
            NULL,
            listViewPos.X, listViewPos.Y,
            listViewWidth, game.board.boardInfo.rect.Height,
            SWP_NOZORDER
        );

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
            if (game.TrySelectFigure(Point(LOWORD(lParam), HIWORD(lParam)))) {
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
            game.TryMove(Point(LOWORD(lParam), HIWORD(lParam)));
            LogMove(game.logger.log[game.logger.log.size() - 1], (game.logger.log.size() - 1) / 2, (game.logger.log.size() - 1) % 2);
            isDragging = false;
            isMemorized = false;
            InvalidateRect(hWnd, &windowPainter.windowRect, FALSE);

            if ((game.CurrentActiveSide == 1 && game.Player2 == AI) || (game.CurrentActiveSide == 0 && game.Player1 == AI)) {
                game.AIMove();
                LogMove(game.logger.log[game.logger.log.size() - 1], (game.logger.log.size() - 1) / 2, (game.logger.log.size() - 1) % 2);
            }
            else {
                // wait
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
