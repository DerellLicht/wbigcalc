// Win32 Tutorial (Dialog Boxes)

// Alan Baylis 2004

#include <windows.h>
#include "resource.h"

const char ClassName[] = "WinBigCalc";
const char DialogClassName[] = "DialogClass";
HWND hWndDlgBox;
HWND hDlgButton;
HWND hWndButton;

//************************************************************************
LRESULT CALLBACK DlgProc (HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg) {
   case WM_CREATE:
      hDlgButton = CreateWindowEx (0,
         "BUTTON",
         "Close",
         WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
         10,
         10,
         100,
         20,
         hWndDlg,
         (HMENU) IDB_DLGBUTTON,
         (HINSTANCE) GetWindowLong (hWndDlg, GWL_HINSTANCE), NULL);

      if (!hDlgButton)
         MessageBox (NULL, "Dialog Button Failed.", "Error",
            MB_OK | MB_ICONERROR);

      return TRUE;

   case WM_COMMAND:
      switch (LOWORD (wParam)) {
         case IDB_DLGBUTTON:
            {
               switch (HIWORD (wParam)) {
                  case BN_CLICKED:
                     SendMessage (hWndDlg, WM_CLOSE, 0, 0);
                     break;
               }
            }
            break;
      }
      return 0;

   case WM_CLOSE:
      DestroyWindow (hWndDlg);
      hWndDlgBox = NULL;
      break;

   default:
      return (DefWindowProc (hWndDlg, Msg, wParam, lParam));
	}
}

//************************************************************************
LRESULT CALLBACK WndProc (HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg) {
   case WM_CREATE:
      {
      WNDCLASSEX wc;
      wc.cbSize = sizeof (WNDCLASSEX);
      wc.style = 0;
      wc.lpfnWndProc = (WNDPROC) DlgProc;
      wc.cbClsExtra = 0;
      wc.cbWndExtra = 0;
      wc.hInstance = (HINSTANCE) GetWindowLong (hWnd, GWL_HINSTANCE);
      wc.hIcon = NULL;
      wc.hIconSm = NULL;
      wc.hCursor = LoadCursor (NULL, IDC_ARROW);
      wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
      wc.lpszMenuName = NULL;
      wc.lpszClassName = DialogClassName;

      if (!RegisterClassEx (&wc)) {
         MessageBox (NULL, "Failed To Register The Dialog Class.",
            "Error", MB_OK | MB_ICONERROR);
         return FALSE;
      }

      hWndButton = CreateWindowEx (0,
         "BUTTON",
         "Open Dialog",
         WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
         10,
         10,
         100,
         30,
         hWnd,
         (HMENU) IDB_WNDBUTTON,
         (HINSTANCE) GetWindowLong (hWnd, GWL_HINSTANCE), NULL);

      if (!hWndButton)
         MessageBox (NULL, "Main Window Button Failed.", "Error",
            MB_OK | MB_ICONERROR);
      }
      break;

   case WM_COMMAND:
      switch (LOWORD (wParam)) {
      case IDB_WNDBUTTON:
         switch (HIWORD (wParam)) {
         case BN_CLICKED:
            if (!hWndDlgBox) {
               hWndDlgBox =
                  CreateWindowEx (WS_EX_DLGMODALFRAME |
                  WS_EX_TOPMOST, DialogClassName,
                  "Dialog Box",
                  WS_VISIBLE | WS_POPUP | WS_CAPTION, 100,
                  100, 200, 100, hWnd, NULL,
                  (HINSTANCE) GetWindowLong (hWnd,
                     GWL_HINSTANCE), NULL);

               if (!hWndDlgBox)
                  MessageBox (NULL, "Dialog Box Failed.",
                     "Error", MB_OK | MB_ICONERROR);
            }
            break;
         }
         break;
      }
      return 0;

   case WM_CLOSE:
      DestroyWindow (hWnd);
      break;

   case WM_DESTROY:
      PostQuitMessage (0);
      break;

   default:
      return (DefWindowProc (hWnd, Msg, wParam, lParam));
	}

	return 0;
}

//************************************************************************
INT WINAPI WinMain (HINSTANCE hInstance,
	HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	WNDCLASSEX wc;

	wc.cbSize = sizeof (WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = (WNDPROC) WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_ICON));
	wc.hIconSm = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_ICON));
	wc.hCursor = LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = ClassName;

	if (!RegisterClassEx (&wc)) {
		MessageBox (NULL, "Failed To Register The Window Class.", "Error",
			MB_OK | MB_ICONERROR);
		return 0;
	}

	HWND hWnd;

	hWnd = CreateWindowEx (WS_EX_CLIENTEDGE,
		ClassName,
		"Modeless Dialog Boxes",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 240, 120, NULL, NULL, hInstance, NULL);

	if (!hWnd) {
		MessageBox (NULL, "Window Creation Failed.", "Error",
			MB_OK | MB_ICONERROR);
		return 0;
	}

	ShowWindow (hWnd, SW_SHOW);
	UpdateWindow (hWnd);

	MSG Msg;

	while (GetMessage (&Msg, NULL, 0, 0)) {
		TranslateMessage (&Msg);
		DispatchMessage (&Msg);
	}

	return Msg.wParam;
}
