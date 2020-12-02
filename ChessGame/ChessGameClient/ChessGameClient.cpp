// ChessGameClient.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "ChessGameClient.h"
#include "WindowPainter.hpp"
#include "Board.hpp"
#include "Game.hpp"
#include "ThreadPool.hpp"
#include <iostream>

#include <CommCtrl.h>
#pragma comment(lib, "ComCtl32.Lib")

#include <uxtheme.h>
#pragma comment(lib, "uxtheme.Lib")

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define MAX_LOADSTRING 100
#define IDC_LISTVIEW 1111
#define IDC_SURRENDERBUTTON 1112
#define IDC_RESETBUTTON 1113
#define IDC_FIGUREINGOPANEL1 1114
#define IDC_FIGUREINGOPANEL2 1115
#define IDC_MOVEGOPANEL 1116

#define IDC_PVPBUTTON 1117
#define IDC_PVEBUTTON 1118
#define IDC_QUITBUTTON 1119

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
WindowPainter windowPainter;                    // 
Game game;
ThreadPool threadPool;
LPRECT cursorClip;
bool isDragging = false;
bool isMemorized = false;

Point listViewPos;
Rect surrenerButtonPos;
Rect resetButtonPos;
Rect figureInfoPanelRect1;
Rect figureInfoPanelRect2;
Rect moveInfoPanelRect;

Rect pvpButtonRect;
Rect pveButtonRect;
Rect quitButtonRect;
const int listViewWidth = 225;

//Controls
int idTimer = -1;
HWND mainWnd;
HWND hWndListView;
HWND hwndSurrenderButton;
HWND hwndResetMoveButton;
Panel* killedFiguresPanel1;
Panel* killedFiguresPanel2;
Panel* moveInfoPanel;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
void ChangePlayerMoveText();
void ChangeControlsVisibility();
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
        Rectangle(hdc, rc.left, rc.top, rc.right, windowPainter.windowRect.bottom - windowPainter.windowRect.top + 100);
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
        WS_CHILD |  LVS_REPORT   | WS_VSCROLL,
        pos.X, pos.Y,
        listViewWidth,
        windowPainter.windowRect.bottom - windowPainter.windowRect.top,
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

LRESULT CALLBACK MyStaticWndProc(HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam)
{
    if (Message == WM_PAINT)
    {
        RECT rc;
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        GetClientRect(hwnd, &rc);
        SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, L"f", _tcslen(L"f"), &rc, DT_CENTER | DT_VCENTER);
        EndPaint(hwnd, &ps);
        return 0;
    }

    //v2 StaticWndProc(hwnd, Message, wparam, lparam);
    return DefSubclassProc(hwnd, Message, wparam, lparam);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

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

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_SYSMENU | WS_CLIPCHILDREN,
      CW_USEDEFAULT, 0, 882, 780, nullptr, nullptr, hInstance, nullptr);

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

void UnLogMove(int moveNum, int side) {
    if (side == 0) {
        ListView_DeleteItem(hWndListView, moveNum);
    }
    ListView_SetItemText(hWndListView, moveNum, 0, const_cast<LPWSTR>(L""));
    ListView_SetItemText(hWndListView, moveNum, side + 1, const_cast<LPWSTR>(L""));
}

void AIMove(HWND hWnd) {
    game.AIMove();
    LogMove(game.logger.log[game.logger.log.size() - 1], (game.logger.log.size() - 1) / 2, (game.logger.log.size() - 1) % 2);   
    ChangePlayerMoveText();
    InvalidateRect(hWnd, &windowPainter.windowRect, FALSE);
}

std::wstring GetExePath() {
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buffer, MAX_PATH);
    return std::wstring(buffer).substr(0, std::wstring(buffer).find_last_of(L"\\/"));
}

void InitGameElements(HWND hWnd) {
    listViewPos = Point(game.board.boardInfo.rect.Width, 0);
    surrenerButtonPos = Rect(game.board.boardInfo.rect.Width - 120 * 2 - 5 - game.board.boardImageInfo.leftOffset * game.board.boardInfo.boardSizeMult, game.board.boardInfo.rect.Height + 40, 120, 30);
    resetButtonPos = Rect(game.board.boardInfo.rect.Width - 120 - game.board.boardImageInfo.leftOffset * game.board.boardInfo.boardSizeMult, game.board.boardInfo.rect.Height + 40, 120, 30);
    figureInfoPanelRect1 = Rect(game.board.boardImageInfo.leftOffset * game.board.boardInfo.boardSizeMult - 5, game.board.boardInfo.rect.Height + 40, 360, 36);
    figureInfoPanelRect2 = Rect(game.board.boardImageInfo.leftOffset * game.board.boardInfo.boardSizeMult - 5, 5, 360, 36);
    moveInfoPanelRect = Rect(game.board.boardInfo.rect.Width - 120 * 2 - 5 - game.board.boardImageInfo.leftOffset * game.board.boardInfo.boardSizeMult, 5, 250, 36);

    hWndListView = CreateListView(hWnd, listViewPos);
    windowPainter.CreateButton(surrenerButtonPos, L"Surrender", true, IDC_SURRENDERBUTTON);
    windowPainter.CreateButton(resetButtonPos, L"Reset Move", true, IDC_RESETBUTTON);
    killedFiguresPanel1 = windowPainter.CreatePanel(figureInfoPanelRect1, {}, {}, true, IDC_FIGUREINGOPANEL1, false);
    killedFiguresPanel2 = windowPainter.CreatePanel(figureInfoPanelRect2, {}, {}, true, IDC_FIGUREINGOPANEL2, false);
   // moveInfoPanel = windowPainter.CreatePanel(moveInfoPanelRect, {L"WHITE"}, { Color(255, 255,255,255)}, true, IDC_MOVEGOPANEL, true);

    windowPainter.deadWhiteFiguresRect = figureInfoPanelRect1;
    windowPainter.deadBlackFiguresRect = figureInfoPanelRect2;
}

void ChangeControlsVisibility()
{
    ListView_DeleteAllItems(hWndListView);
    windowPainter.ChangeButtonVisibility(IDC_SURRENDERBUTTON);
    windowPainter.ChangeButtonVisibility(IDC_RESETBUTTON);
    windowPainter.ChangePanelVisibility(IDC_FIGUREINGOPANEL1);
    windowPainter.ChangePanelVisibility(IDC_FIGUREINGOPANEL2);
    windowPainter.ChangePanelVisibility(IDC_MOVEGOPANEL);
    windowPainter.ChangeButtonVisibility(IDC_PVPBUTTON);
    windowPainter.ChangeButtonVisibility(IDC_PVEBUTTON);
    windowPainter.ChangeButtonVisibility(IDC_QUITBUTTON);
}

void InitMenuElements(HWND hWnd) {
    pvpButtonRect = Rect(game.board.boardInfo.rect.Width + listViewWidth / 2 - 170 / 2,
        game.board.boardInfo.rect.Y + game.board.boardImageInfo.topOffset * game.board.boardInfo.boardSizeMult, 170, 40);
    pveButtonRect = Rect(game.board.boardInfo.rect.Width + listViewWidth / 2 - 170 / 2,
        game.board.boardInfo.rect.Y + game.board.boardImageInfo.topOffset * game.board.boardInfo.boardSizeMult + 50, 170, 40);
    quitButtonRect = Rect(game.board.boardInfo.rect.Width + listViewWidth / 2 - 170 / 2,
        game.board.boardInfo.rect.Y + game.board.boardImageInfo.topOffset * game.board.boardInfo.boardSizeMult + 100, 170, 40);

    windowPainter.CreateButton(pvpButtonRect, L"Player vs Player", false, IDC_PVPBUTTON);
    windowPainter.CreateButton(pveButtonRect, L"Player vs AI", false, IDC_PVEBUTTON);
    windowPainter.CreateButton(quitButtonRect, L"Quit", false, IDC_QUITBUTTON);
}

void ShowGameElements(HWND hWnd) {
    ChangeControlsVisibility();
    ShowWindow(hWndListView, SW_SHOWDEFAULT);
}

void ShowMenuElements(HWND hWnd) {
    ChangeControlsVisibility();
    ShowWindow(hWndListView, SW_HIDE);
}


void ChangePlayerMoveText()
{
   // moveInfoPanel->texts[0] = !game.CurrentActiveSide ? L"WHITE" : L"BLACK";
   // moveInfoPanel->textsColor[0] = !game.CurrentActiveSide ? Color(255, 255, 255, 255) : Color(255, 0, 0, 0);
   // moveInfoPanel->dark = !game.CurrentActiveSide;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        //window init
        threadPool.InitThreadPool();

        windowPainter.SetBoard(&game.board);
        windowPainter.LoadSprites();
        windowPainter.SetWindow(hWnd, 40, 40);
        windowPainter.CreateBuffer(hWnd);

        InitMenuElements(hWnd);
        InitGameElements(hWnd);

        ShowMenuElements(hWnd);

        break;
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
            case IDC_RESETBUTTON:
                if ((game.Player1 != game.Player2) && ((game.CurrentActiveSide == 1 && game.Player2 == User) || (game.CurrentActiveSide == 0 && game.Player1 == User))) {
                    UnLogMove((game.logger.log.size() - 1) / 2, (game.logger.log.size() - 1) % 2);
                    game.ResetMove(false);
                    UnLogMove((game.logger.log.size() - 1) / 2, (game.logger.log.size() - 1) % 2);
                    game.ResetMove(true);
                }
                else if (game.Player1 == game.Player2) {
                    UnLogMove((game.logger.log.size() - 1) / 2, (game.logger.log.size() - 1) % 2);
                    game.ResetMove(true);
                }
                break;
                
            case IDC_SURRENDERBUTTON:
                ShowMenuElements(hWnd);
                game.CurrentGameState = MoveState::InMenu;
                break;
            case IDC_PVPBUTTON:
                ShowGameElements(hWnd);
                game.InitGame(PlayerType::User, PlayerType::User);
                break;
            case IDC_PVEBUTTON:
                ShowGameElements(hWnd);
                game.InitGame(PlayerType::User, PlayerType::AI);
                break;
            case IDC_QUITBUTTON:
                SendMessage(hWnd, WM_DESTROY, 0, 0);
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
            if (game.CurrentGameState == MoveState::InMenu) {              
                windowPainter.DrawField();
                windowPainter.DrawMenu();
                windowPainter.DrawButtons();
                windowPainter.DrawFigures();
                windowPainter.DrawDangerHints(game.CurrentActiveSide);
            }
            else {
                if (!isMemorized) {
                    windowPainter.DrawField();
                    windowPainter.DrawPanels();
                    windowPainter.DrawDeadFigures();                
                    windowPainter.DrawFigures();
                    windowPainter.DrawDangerHints(game.CurrentActiveSide);

                    if (isDragging && !isMemorized) {
                        windowPainter.DrawHintMoves();

                        BitBlt(windowPainter.memoryDC, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
                            ps.rcPaint.bottom - ps.rcPaint.top, windowPainter.bufferDC, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
                        isMemorized = true;
                    }
                }
                if (isDragging) {
                    windowPainter.DrawSelectedFigure();
                    windowPainter.DrawCurrentMoveCell();
                }
                windowPainter.DrawButtons();
            }
            

            //finish
            BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
                ps.rcPaint.bottom - ps.rcPaint.top, windowPainter.bufferDC, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_SIZE:
        listViewPos = Point(game.board.boardInfo.rect.Width, 0);
        windowPainter.SetWindow(hWnd, 40, 40);     
        SetWindowPos(
            hWndListView,
            NULL,
            listViewPos.X, listViewPos.Y,
            listViewWidth, game.board.boardInfo.rect.Height,
            SWP_NOZORDER
        );

        break;
    case WM_MOUSEMOVE:
        windowPainter.xMousePos = LOWORD(lParam);
        windowPainter.yMousePos = HIWORD(lParam);
        if (isDragging) {            
            InvalidateRect(hWnd, &windowPainter.windowRect, FALSE);
        }
        else if (windowPainter.NeedInvalidation()) {
            InvalidateRect(hWnd, &windowPainter.windowRect, FALSE);
        }
        break;
    case WM_LBUTTONDOWN: 
        windowPainter.clickedXMousePos = LOWORD(lParam);
        windowPainter.clickedYMousePos = HIWORD(lParam);
        if (!isDragging) {         
            if (game.TrySelectFigure(Point(LOWORD(lParam), HIWORD(lParam)))) {             
                isDragging = true;
                isMemorized = false;
                
            }           
        }
        InvalidateRect(hWnd, &windowPainter.windowRect, FALSE);
        break;
    case WM_LBUTTONUP:
        windowPainter.clickedXMousePos = 0;
        windowPainter.clickedYMousePos = 0;
        if (isDragging) {
            if (game.TryMove(Point(LOWORD(lParam), HIWORD(lParam)))) {
                LogMove(game.logger.log[game.logger.log.size() - 1], (game.logger.log.size() - 1) / 2, (game.logger.log.size() - 1) % 2);
                ChangePlayerMoveText();
            }
            isDragging = false;
            isMemorized = false;           

            if ((game.CurrentActiveSide == 1 && game.Player2 == AI) || (game.CurrentActiveSide == 0 && game.Player1 == AI)) {
                threadPool.AddJob(std::bind(AIMove, hWnd));
            }
            else {
                // wait
            }         
        }
        else {
            SendMessage(hWnd, WM_COMMAND, windowPainter.ClickButton(), 0);
        }
        InvalidateRect(hWnd, &windowPainter.windowRect, FALSE);
        break;      
    case WM_DESTROY:
        threadPool.Shutdown();
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
