// TicTacToe_Windows_Desktop_Application.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TicTacToe_Windows_Desktop_Application.h"
#include <windowsx.h>
#include "resource.h"


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
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

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TICTACTOEWINDOWSDESKTOPAPPLICATION, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICTACTOEWINDOWSDESKTOPAPPLICATION));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICTACTOEWINDOWSDESKTOPAPPLICATION));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    //wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TICTACTOEWINDOWSDESKTOPAPPLICATION);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

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

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return

//My Variables
const int cell_size = 100;
HBRUSH hbr1, hbr2;
HICON hIcon1, hIcon2;
int playerturn = 1;
int gameBoard[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int winner = 0;
int wins[3];
int player1_score;
int player2_score;

BOOL GetGameBoard(HWND hwnd, RECT * pRect)
{
	RECT rc;
	if (GetClientRect(hwnd, &rc))
	{
		int width = rc.right - rc.left;
		int hight = rc.bottom - rc.top;

		pRect-> left = (width - cell_size * 3) / 2;
		pRect-> top = (hight - cell_size * 3) / 2;
		pRect-> right = pRect-> left + cell_size * 3;
		pRect-> bottom = pRect-> top + cell_size * 3;
		return TRUE;
	}
		SetRectEmpty(pRect);
		return FALSE;
}
void drawline(HDC hdc, int x1, int y1, int x2, int y2)
{
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}
int GetCellNumber(HWND hwnd, int x, int y)
{
	POINT pt = { x,y };
	RECT rc;
	if (GetGameBoard(hwnd, &rc))
	{
		if (PtInRect(&rc, pt))
		{
			//User clicked in game board
			x = pt.x - rc.left;
			y = pt.y - rc.top;

			int column = x / cell_size;
			int row = y / cell_size;

			//Convert to index
			return column + row * 3;
		}
	}
	return -1; //Outside game board
}
BOOL GetCellRect(HWND hWnd, int index, RECT * pRect)
{
	RECT rcBoard;

	SetRectEmpty(pRect);
	if (index < 0 || index > 8)
		return FALSE;

	if (GetGameBoard(hWnd, &rcBoard))
	{
		int y = index / 3; //Row number
		int x = index % 3; //Column number

		pRect->left = rcBoard.left + x * cell_size +1;
		pRect->top = rcBoard.top + y * cell_size +1;
		pRect->right = pRect->left + cell_size -1;
		pRect->bottom = pRect->top + cell_size -1;
		return TRUE;
	}
	return FALSE;
}
int GetWinner(int wins[3])
{	/*
	Return:
	0 - No winner
	1- Player 1 wins
	2- Player 2 wins
	3- Draw
 
	0,1,2
	3,4,5
	6,7,8
	*/

	int cells[] = { 0,1,2,  3,4,5,  6,7,8,  0,3,6,  1,4,7,  2,5,8,  0,4,8,  2,4,6 };

	for (int i = 0; i < ARRAYSIZE(cells); i += 3)
	{
		if (gameBoard[cells[i]] != 0 && gameBoard[cells[i]] == gameBoard[cells[i + 1]] && gameBoard[cells[i]] == gameBoard[cells[i + 2]])
		{
			//A winner
			wins[0] = cells[i];
			wins[1] = cells[i + 1];
			wins[2] = cells[i + 2];

			return gameBoard[cells[i]];
		}
	}

	//Any cells left??
	for (int i = 0; i < ARRAYSIZE(gameBoard); ++i)
	{
		if (gameBoard[i] == 0)
			return 0; //Continue the game
	}

	//Draw
	for (int i = 0; i < ARRAYSIZE(cells); i += 3)
	{
		if (gameBoard[cells[i]] != 0 && gameBoard[cells[i]] != gameBoard[cells[i + 1]] && gameBoard[cells[i]] != gameBoard[cells[i + 2]])
		{
			return 3;
		}
	}
}
void ShowTurn(HWND hwnd, HDC hdc)
{
	static const WCHAR szTurn1[] = L"Turn: Player 1";
	static const WCHAR szTurn2[] = L"Turn: Player 2";
	const WCHAR * pszTurnText = NULL;
	
	
	switch (winner)
	{
	case 0:
		pszTurnText = (playerturn == 1) ? szTurn1 : szTurn2;
		break;
	case 1:
		pszTurnText = L"Player 1 is the winner";
		break;
	case 2:
		pszTurnText = L"Player 2 is the winner";
		break;
	case 3:
		pszTurnText = L"Draw";
		break;
	}

	RECT rc;

	if (pszTurnText != NULL && GetClientRect(hwnd, &rc))
	{
		rc.top = rc.bottom - 48;
		FillRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		DrawText(hdc, pszTurnText, lstrlen(pszTurnText), &rc, DT_CENTER);
	}
}
void DrawIconCentered(HDC hdc, RECT * pRect, HICON hIcon)
{
	if (pRect != NULL)
	{
		int left = pRect->left + ((pRect -> right - pRect->left) - 32)/2;
		int top = pRect->top + ((pRect->bottom - pRect->top) - 32) / 2;
		DrawIcon(hdc, left, top, hIcon);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		hbr1 = CreateSolidBrush(RGB(255, 0, 0));
		hbr2 = CreateSolidBrush(RGB(0, 0, 255));

		//Load icons
		hIcon1 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
		hIcon2 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON2));

	}
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case ID_FILE_NEWGAME:
			{
				int ret = MessageBox(hWnd, L"Are you sure you want to start a new game?", L"New Game", 
					MB_YESNO | MB_ICONQUESTION);
				if (IDYES == ret)
				{
					//Reset variables
					playerturn = 1;
					winner = 0;
					player1_score = 0;
					player2_score = 0;
					ZeroMemory(gameBoard, sizeof(gameBoard));
					InvalidateRect(hWnd, NULL, TRUE);
					UpdateWindow(hWnd);
					
				}
			}
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
	case WM_LBUTTONDOWN:
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		//Only handle clicks if there's a player turn
		if (playerturn == 0)
			break;

		int index = GetCellNumber(hWnd, xPos, yPos);
		
		HDC hdc = GetDC(hWnd);
		if (NULL != hdc)
		{
			//WCHAR temp[100];
			//wsprintf(temp, L"Index = %d", index);
			//TextOut(hdc, xPos, yPos, temp, lstrlen(temp));

			//Get cell dimensions
			if (index != -1)
			{
				RECT rcCell;
				if ((gameBoard[index] == 0) && GetCellRect(hWnd, index, &rcCell))
				{
					gameBoard[index] = playerturn;
					//FillRect(hdc, &rcCell, (playerturn==1) ? hbr1 : hbr2);
					DrawIconCentered(hdc, &rcCell, (playerturn == 1) ? hIcon1 : hIcon2);
					//DrawIcon(hdc, rcCell.left, rcCell.top, (playerturn == 1) ? hIcon1 : hIcon2);
					
					//Check for a winner
					winner = GetWinner(wins);
					if (winner == 1 || winner == 2)
					{
						//A winner
						MessageBox(hWnd, (winner == 1) ? L"Player 1 is the winner" : L"Player 2 is the winner",
							L"You win", MB_OK | MB_ICONINFORMATION);

						if (winner == 1)
							player1_score += 1;
						else if (winner == 2)
							player2_score += 1;

						playerturn = 1;
						winner = 0;
						ZeroMemory(gameBoard, sizeof(gameBoard));
						InvalidateRect(hWnd, NULL, TRUE);
						UpdateWindow(hWnd);
					}
					else if (winner == 3)
					{
						//Draw
						MessageBox(hWnd, L"Draw", L":(", MB_OK | MB_ICONEXCLAMATION);
						playerturn = 1;
						winner = 0;
						ZeroMemory(gameBoard, sizeof(gameBoard));
						InvalidateRect(hWnd, NULL, TRUE);
						UpdateWindow(hWnd);
					}
					else if (winner == 0)
					{
						//Continue
						playerturn = (playerturn == 1) ? 2 : 1;
					}
					ShowTurn(hWnd, hdc);
				}
			}
			ReleaseDC(hWnd, hdc);
		}
	}
	break;
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO * pMinMax = (MINMAXINFO*)lParam;
		pMinMax->ptMinTrackSize.x = cell_size * 5;
		pMinMax->ptMinTrackSize.y = cell_size * 5;
	}
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			RECT rc;
			if (GetGameBoard(hWnd, &rc))
			{ 
				RECT rcClient;
				if (GetClientRect(hWnd, &rcClient))
				{
					SetBkMode(hdc, TRANSPARENT);
					//Draw Player 1 & 2 text
					TextOut(hdc, 16, 16, L"Player 1  O", 12);
					WCHAR temp[100];
					wsprintf(temp, L"Score: %d", player1_score);
					TextOut(hdc, 16, 50, temp, lstrlen(temp));
					TextOut(hdc, rcClient.right - 72, 16, L"Player 2  X", 12);
					WCHAR temp2[100];
					wsprintf(temp2, L"Score: %d", player2_score);
					TextOut(hdc, rcClient.right - 72, 50, temp2, lstrlen(temp2));
					ShowTurn(hWnd, hdc);
				}

				//Draw game board
				Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
			}

			for (int i = 0; i < 3; i++)
			{
				//Draw vertical lines
				drawline(hdc, rc.left + cell_size * i, rc.top, rc.left + cell_size * i, rc.bottom);
				//Draw horizontal line 
				drawline(hdc, rc.left, rc.top + cell_size * i, rc.right, rc.top + cell_size * i);
			}

			RECT rcCell;
			for (int i = 0; i < 9; ++i)
			{
				if ((gameBoard[i] != 0) && GetCellRect(hWnd, i, &rcCell))
				{
					//FillRect(hdc, &rcCell, (gameBoard[i] == 1) ? hbr1 : hbr2);
					DrawIconCentered(hdc, &rcCell, (gameBoard[i] == 1) ? hIcon1 : hIcon2);

				}

			}
			
	
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		DeleteObject(hbr1);
		DeleteObject(hbr2);
		DestroyIcon(hIcon1);
		DestroyIcon(hIcon2);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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
