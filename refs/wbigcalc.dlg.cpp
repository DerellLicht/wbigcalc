// Win32 Tutorial (Dialog Box as main window)
// Alan Baylis 2004
// http://members.net-tech.com.au/alaneb/main_dialog.html

#include <windows.h>
#include "keywin32.h"
#include "resource.h"

#define BUFFER_SIZE 256

char szText[BUFFER_SIZE];

static unsigned key_mask = 0;
// HBRUSH g_hbrBackground = CreateSolidBrush (RGB (255, 255, 255));
//  other colors are in winuser.h (line 822 in mingw)
HBRUSH g_hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
// #ifdef   USE_SYS_BG_COLOR  
//    wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
// #else
//    wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH) ;
// #endif

HINSTANCE hInst;

extern int dos_main(unsigned inchr);
//*************************************************************
static int process_keystroke (HWND hwnd, unsigned inchr)
{
   char stext[10] ;
   //  main keyboard handler
   switch (inchr) {
   case kCc:
   case kESC:
      SendMessage(hwnd, WM_DESTROY, 0, 0) ;
      break;

   default:
      HWND hwndTemp = GetDlgItem(hwnd, IDC_REG_X) ;
      wsprintf(stext, "%c", inchr) ;
      SetWindowText(hwndTemp, stext) ;
      dos_main(inchr) ;
      // wsprintf (tempstr, "PRESS=0x%04X", inchr);
      // Statusbar_ShowMessage (tempstr);
      break;
   }
   return 0;
}

//*************************************************************
BOOL CALLBACK InitProc (HWND hDlgWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message) {
   case WM_CTLCOLORDLG:
      return (LONG) g_hbrBackground;

   case WM_CTLCOLORSTATIC:
      {
      HDC hdcStatic = (HDC) wParam;
      SetTextColor (hdcStatic, RGB (0, 0, 0));
      SetBkMode (hdcStatic, TRANSPARENT);
      return (LONG) g_hbrBackground;
      }
      break;


   case WM_INITDIALOG:
      {
      RECT DesktopRect;
      RECT DialogRect;
      GetWindowRect (GetDesktopWindow (), &DesktopRect);
      GetWindowRect (hDlgWnd, &DialogRect);
      SetWindowPos (hDlgWnd,
         HWND_TOP,
         (DesktopRect.right - DialogRect.right) / 2,
         (DesktopRect.bottom - DialogRect.bottom) / 2,
         0, 0, SWP_NOSIZE);

      SendMessage (hDlgWnd, WM_SETICON, ICON_SMALL,
         (LPARAM) LoadIcon (hInst, MAKEINTRESOURCE (IDI_ICON)));
      SendMessage (hDlgWnd, WM_SETICON, ICON_BIG,
         (LPARAM) LoadIcon (hInst, MAKEINTRESOURCE (IDI_ICON)));

      SendDlgItemMessage (hDlgWnd, IDC_DLG_TEXT, EM_SETLIMITTEXT,
         (WPARAM) BUFFER_SIZE - 1, (LPARAM) 0);
      SetDlgItemText (hDlgWnd, IDC_DLG_TEXT, "Enter Text");
      return TRUE;
      }
      break;

   case WM_KEYDOWN:
   case WM_SYSKEYDOWN:
      // ShowKey (hwnd, 0, "WM_KEYDOWN", wParam, lParam) ;
      if (wParam == VK_SHIFT) {
         key_mask |= kShift;
      }
      else if (wParam == VK_CONTROL) {
         key_mask |= kCtrl;
      }
      else if (wParam == VK_MENU) {
         key_mask |= kAlt;
      }
      else {
         OutputDebugString("I am here...\n") ;
         wParam |= key_mask;
         process_keystroke (hDlgWnd, wParam);
      }
      return 0;

   case WM_KEYUP:
   case WM_SYSKEYUP:
      // ShowKey (hwnd, 0, "WM_KEYUP", wParam, lParam) ;
      if (wParam == VK_SHIFT) {
         key_mask &= ~kShift;
      }
      else if (wParam == VK_CONTROL) {
         key_mask &= ~kCtrl;
      }
      else if (wParam == VK_MENU) {
         key_mask &= ~kAlt;
      }
      else {
         // wParam |= key_mask ;
         // process_keystroke(hwnd, wParam) ;
      }
      return 0;

   case WM_COMMAND:
      switch (LOWORD (wParam)) {
      case IDCANCEL:
         ExitProcess (0);
         return TRUE;

      case IDOK:
         MessageBox (hDlgWnd, "Ok", "Success", MB_OK);
         GetDlgItemText (hDlgWnd, IDC_DLG_TEXT, szText, BUFFER_SIZE);
         ExitProcess (0);
         return TRUE;
      }
      break;
	}
	return FALSE;
}

//*****************************************************************
INT WINAPI WinMain (HINSTANCE hInstance,
	HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	hInst = hInstance;

	HWND hWnd = CreateDialog (hInstance,
		(LPCTSTR) "DIALOG_NAME",
		NULL,
		(DLGPROC) InitProc);

	MSG Msg;

	while (GetMessage (&Msg, NULL, 0, 0) == TRUE) {
		if (!IsDialogMessage (hWnd, &Msg)) {
			TranslateMessage (&Msg);
			DispatchMessage (&Msg);
		}
	}
	return Msg.wParam;
}
