// Win32 Tutorial (Dialog Box as main window)
// Alan Baylis 2004
// http://members.net-tech.com.au/alaneb/main_dialog.html

#include <windows.h>
#include <stdio.h>

#include "common.h"
#include "keywin32.h"
#include "resource.h"
#include "bigcalc.h"
#include "statbar.h"
#include "winmsgs.h"

#define BUFFER_SIZE 256

char szText[BUFFER_SIZE];

// static unsigned key_mask = 0;
// HBRUSH g_hbrBackground = CreateSolidBrush (RGB (255, 255, 255));
//  other colors are in winuser.h (line 822 in mingw)
HBRUSH g_hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
// #ifdef   USE_SYS_BG_COLOR  
//    wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
// #else
//    wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH) ;
// #endif

HINSTANCE hInst;
static HWND hwndMain = NULL ;
static HWND hwndMsg = NULL ;

static HWND hwndRegs[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } ;
static HWND hwndStack[4] = { 0, 0, 0, 0 } ;

uint cxClient = 0 ;
uint cyClient = 0 ;

static CStatusBar *MainStatusBar = NULL;
extern int dos_main(unsigned inchr);

//  bigcalc.cpp
void Initialize(int argc, char *argv);

//*************************************************************
static void set_hwnd_values(void)
{
   hwndMsg = GetDlgItem(hwndMain, IDC_MSG) ;
   
   hwndRegs[0] = GetDlgItem(hwndMain, IDC_R0) ;
   hwndRegs[1] = GetDlgItem(hwndMain, IDC_R1) ;
   hwndRegs[2] = GetDlgItem(hwndMain, IDC_R2) ;
   hwndRegs[3] = GetDlgItem(hwndMain, IDC_R3) ;
   hwndRegs[4] = GetDlgItem(hwndMain, IDC_R4) ;
   hwndRegs[5] = GetDlgItem(hwndMain, IDC_R5) ;
   hwndRegs[6] = GetDlgItem(hwndMain, IDC_R6) ;
   hwndRegs[7] = GetDlgItem(hwndMain, IDC_R7) ;
   hwndRegs[8] = GetDlgItem(hwndMain, IDC_R8) ;
   hwndRegs[9] = GetDlgItem(hwndMain, IDC_R9) ;
   
   hwndStack[0] = GetDlgItem(hwndMain, IDC_REG_X) ;
   hwndStack[1] = GetDlgItem(hwndMain, IDC_REG_Y);
   hwndStack[2] = GetDlgItem(hwndMain, IDC_REG_Z) ;
   hwndStack[3] = GetDlgItem(hwndMain, IDC_REG_T) ;
   
}

//*******************************************************************
void status_message(char *msgstr)
{
   MainStatusBar->show_message(msgstr);
}

//*******************************************************************
void status_message(uint idx, char *msgstr)
{
   MainStatusBar->show_message(idx, msgstr);
}

//*************************************************************
void put_stack(unsigned n)
{
   // wsprintf (tempstr, "put_stack %u", n);
   // Statusbar_ShowMessage (tempstr);
   SetWindowText(hwndStack[n], get_iostr()) ;
}

void put_register(unsigned n)
{
   // wsprintf (tempstr, "put_register %u", n);
   // Statusbar_ShowMessage (tempstr);
   SetWindowText(hwndRegs[n], get_iostr()) ;
}

void put_work(void)
{
   // wsprintf (tempstr, "put_register %u", n);
   // Statusbar_ShowMessage (tempstr);
   SetWindowText(hwndStack[0], get_iostr()) ;
}

//*************************************************************
// static int process_keystroke (HWND hwnd, unsigned inchr)
// {
//    char stext[10] ;
//    //  main keyboard handler
//    switch (inchr) {
//    case kCc:
//    case kESC:
//       SendMessage(hwnd, WM_DESTROY, 0, 0) ;
//       break;
// 
//    default:
//       HWND hwndTemp = GetDlgItem(hwnd, IDC_REG_X) ;
//       wsprintf(stext, "%c", inchr) ;
//       SetWindowText(hwndTemp, stext) ;
//       dos_main(inchr) ;
//       // wsprintf (tempstr, "PRESS=0x%04X", inchr);
//       // Statusbar_ShowMessage (tempstr);
//       break;
//    }
//    return 0;
// }

/*
 *    **************************************************
 *    *                                                *
 *    *      Display Message centered on 25th line     *
 *    *                                                *
 *    **************************************************
 */
void Message(char *msg)
{
   SetWindowText(hwndMsg, msg);
}

//*************************************************************

BOOL CALLBACK InitProc (HWND hDlgWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   //***************************************************
   //  debug: log all windows messages
   //***************************************************
   // if (dbg_flags & DBG_WINMSGS) {
   if (false) {
      switch (msg) {
      //  list messages to be ignored
      // case WM_CTLCOLORBTN:
      // case WM_CTLCOLORSTATIC:
      // case WM_CTLCOLOREDIT:
      // case WM_CTLCOLORLISTBOX:
      // case WM_MOUSEMOVE:
      // case WM_NCMOUSEMOVE:
      // case WM_NCHITTEST:
      // case WM_SETCURSOR:
      // case WM_TIMER:
      // case WM_NOTIFY:
      case WM_COMMAND:  //  prints its own msgs below
         break;
      default:
         syslog("TOP [%s]\n", lookup_winmsg_name(msg)) ;
         break;
      }
   }
   
   switch (msg) {
   case WM_INITDIALOG:
      {
      SendMessage (hDlgWnd, WM_SETICON, ICON_SMALL,(LPARAM) LoadIcon (hInst, MAKEINTRESOURCE (IDI_ICON)));
      SendMessage (hDlgWnd, WM_SETICON, ICON_BIG,  (LPARAM) LoadIcon (hInst, MAKEINTRESOURCE (IDI_ICON)));
      
      hwndMain = hDlgWnd ;
      //  center the dialog on desktop
      RECT DesktopRect;
      RECT DialogRect;
      GetWindowRect (GetDesktopWindow (), &DesktopRect);
      GetWindowRect (hDlgWnd, &DialogRect);
      SetWindowPos (hDlgWnd,
         HWND_TOP,
         (DesktopRect.right  - DialogRect.right) / 2,
         (DesktopRect.bottom - DialogRect.bottom) / 2,
         0, 0, SWP_NOSIZE);

      
      // RECT rWindow;
      // unsigned stTop ;
      RECT myRect ;
      // GetWindowRect(hwnd, &myRect) ;
      GetClientRect(hDlgWnd, &myRect) ;
      cxClient = (myRect.right - myRect.left) ;
      cyClient = (myRect.bottom - myRect.top) ;

      //****************************************************************
      //  create/configure status bar
      //****************************************************************
      MainStatusBar = new CStatusBar(hDlgWnd) ;
      MainStatusBar->MoveToBottom(cxClient, cyClient) ;
      //  re-position status-bar parts
      {
      int sbparts[3];
      sbparts[0] = (int) (6 * cxClient / 10) ;
      sbparts[1] = (int) (8 * cxClient / 10) ;
      sbparts[2] = -1;
      MainStatusBar->SetParts(3, &sbparts[0]);
      }

      set_hwnd_values();
      // SendDlgItemMessage (hDlgWnd, IDC_DLG_TEXT, EM_SETLIMITTEXT, (WPARAM) BUFFER_SIZE - 1, (LPARAM) 0);
      // SetDlgItemText (hDlgWnd, IDC_DLG_TEXT, "Enter Text");
      Heading2();
      Message("We are ready...");
      return TRUE;
      }
      break;

   // case WM_KEYDOWN:
   // case WM_SYSKEYDOWN:
   //    // ShowKey (hwnd, 0, "WM_KEYDOWN", wParam, lParam) ;
   //    if (wParam == VK_SHIFT) {
   //       key_mask |= kShift;
   //    }
   //    else if (wParam == VK_CONTROL) {
   //       key_mask |= kCtrl;
   //    }
   //    else if (wParam == VK_MENU) {
   //       key_mask |= kAlt;
   //    }
   //    else {
   //       syslog("I am here (DOWN)...\n") ;
   //       wParam |= key_mask;
   //       process_keystroke (hDlgWnd, wParam);
   //    }
   //    return 0;

   // case WM_KEYUP:
   // case WM_SYSKEYUP:
   //    // ShowKey (hwnd, 0, "WM_KEYUP", wParam, lParam) ;
   //    if (wParam == VK_SHIFT) {
   //       key_mask &= ~kShift;
   //    }
   //    else if (wParam == VK_CONTROL) {
   //       key_mask &= ~kCtrl;
   //    }
   //    else if (wParam == VK_MENU) {
   //       key_mask &= ~kAlt;
   //    }
   //    else {
   //       syslog("I am here (UP)...\n") ;
   //       // wParam |= key_mask ;
   //       // process_keystroke(hwnd, wParam) ;
   //    }
   //    return 0;

   case WM_COMMAND:
      {  //  create local context
      DWORD cmd = HIWORD (wParam) ;
      DWORD target = LOWORD(wParam) ;
      // char tempstr[20];
      // sprintf (tempstr, " cmd: %u, target: %u", (uint) cmd, (uint) target);
      // status_message(tempstr);

      switch (cmd) {
      case FVIRTKEY:  //  keyboard accelerators: WARNING: same code as CBN_SELCHANGE !!
         //  fall through to BM_CLICKED, which uses same targets
      case BN_CLICKED:
         switch(target) {
            
         case IDB_KBD_0  : dos_main(k0);   break ;
         case IDB_KBD_1  : dos_main(k1);   break ;
         case IDB_KBD_2  : dos_main(k2);   break ;
         case IDB_KBD_3  : dos_main(k3);   break ;
         case IDB_KBD_4  : dos_main(k4);   break ;
         case IDB_KBD_5  : dos_main(k5);   break ;
         case IDB_KBD_6  : dos_main(k6);   break ;
         case IDB_KBD_7  : dos_main(k7);   break ;
         case IDB_KBD_8  : dos_main(k8);   break ;
         case IDB_KBD_9  : dos_main(k9);   break ;
         case IDB_KBD_DOT: dos_main(kPeriod);  break ;
         case IDB_KBD_e  : dos_main(ke);   break ;
            
         case IDB_KBD_RETURN:
            dos_main(ENTER);
            Message("Return/Enter received");
            break ;            
         // case IDB_HELP:
         //    queryout("Terminal keyboard shortcuts") ;
         //    infoout("Alt-s = send command (i.e., print command in terminal)") ;
         //    infoout("Alt-h = show this help screen") ;
         //    infoout("Alt-c = Close this program") ;
         //    break;
            
         case IDB_CLOSE:
            PostMessageA(hDlgWnd, WM_CLOSE, 0, 0);
            break;
         } //lint !e744  switch target
         return true;
      } //lint !e744  switch cmd
      break;
      }  //lint !e438 !e10  end local context
         
   case WM_CTLCOLORDLG:
      return (LONG) g_hbrBackground;

   case WM_CTLCOLORSTATIC:
      {
      HDC hdcStatic = (HDC) wParam;
      SetTextColor (hdcStatic, RGB (0, 0, 0));
      SetBkMode (hdcStatic, TRANSPARENT);
      return (LONG) g_hbrBackground;
      }

   //********************************************************************
   //  application shutdown handlers
   //********************************************************************
   case WM_CLOSE:
      DestroyWindow(hDlgWnd);
      break;

   case WM_DESTROY:
      PostQuitMessage(0);
      break;

   default:
      return FALSE ;
   }
   return FALSE;
}

//*****************************************************************
INT WINAPI WinMain (HINSTANCE hInstance,
   HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
   hInst = hInstance;

   Initialize(0, 0);
   HWND hWnd = CreateDialog (hInstance, 
      MAKEINTRESOURCE(IDD_MAIN_DIALOG), 
      NULL,
      (DLGPROC) InitProc);
   HACCEL hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATOR1));  

   MSG Msg;
   while (GetMessage (&Msg, NULL, 0, 0) == TRUE) {
      // if (!IsDialogMessage (hWnd, &Msg)) {
      if(!TranslateAccelerator(hWnd, hAccel, &Msg)  &&  !IsDialogMessage(hWnd, &Msg)) {
         TranslateMessage (&Msg);
         DispatchMessage (&Msg);
      }
   }
   return Msg.wParam;
}

