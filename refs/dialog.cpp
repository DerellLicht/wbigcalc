// Win32 Tutorial (Dialog Box as main window)
// Alan Baylis 2004
// http://members.net-tech.com.au/alaneb/main_dialog.html

#include <windows.h>
#include "resource.h"

#define BUFFER_SIZE 256

char szText[BUFFER_SIZE];
HBRUSH g_hbrBackground = CreateSolidBrush (RGB (255, 255, 255));
HINSTANCE hInst;

BOOL CALLBACK InitProc (HWND hDlgWnd,
	UINT Message, WPARAM wParam, LPARAM lParam)
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

		case WM_COMMAND:
			switch (LOWORD (wParam)) {
				case IDCANCEL:
					ExitProcess (0);
					return TRUE;
					break;

				case IDOK:
					MessageBox (hDlgWnd, "Ok", "Success", MB_OK);
					GetDlgItemText (hDlgWnd, IDC_DLG_TEXT, szText, BUFFER_SIZE);
					ExitProcess (0);
					return TRUE;
					break;
			}
			break;
	}
	return FALSE;
}

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
