//*********************************************************************************
//  WBigCalc.cpp 
//  Windows wrapper for BigCalc Extended Precision Calculator
//  by Judson D. McClendon (c) 1999
//*********************************************************************************


// #define TITLE "Extended Precision Calculator 6.0"

#define  VER_NUMBER "6.0"
static char const * const Version = "WBigCalc Extended Precision Calculator, Version " VER_NUMBER " " ;

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#include "common.h"
#include "commonw.h"
#include "keywin32.h"
#include "resource.h"
#include "bigcalc.h"
#include "statbar.h"
#include "winmsgs.h"

//lint -esym(537, 'c:\mingw\include\commdlg.h')

// HBRUSH g_hbrBackground = CreateSolidBrush (RGB (255, 255, 255));
//  other colors are in winuser.h (line 822 in mingw)
static HBRUSH const g_hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
// #ifdef   USE_SYS_BG_COLOR  
//    wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
// #else
//    wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH) ;
// #endif

static HINSTANCE hInst;
static HWND hwndMain = NULL ;
static HWND hwndTitle = NULL ;
static HWND hwndMsg = NULL ;
static HWND hwndKbdState = NULL ;

static HWND hwndRegs[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } ;
static HWND hwndStack[4] = { 0, 0, 0, 0 } ;

static uint cxClient = 0 ;
static uint cyClient = 0 ;

static CStatusBar *MainStatusBar = NULL;

//**********************************************************************
#define  FONT_HEIGHT_TITLE    21
// #define  FONT_HEIGHT_MESSAGE  22
// static char ascii_font_name[LF_FULLFACESIZE] = "Wingdings" ;
// static char ascii_font_name[LF_FULLFACESIZE] = "Times New Roman" ;
// static TCHAR font_name_title[LF_FULLFACESIZE] = _T("CaligulaA") ;
static TCHAR font_name_message[LF_FULLFACESIZE] = _T("Times New Roman") ;

static void set_control_font(HWND hwnd, TCHAR *fname, uint fheight, uint flags)
{
   // HFONT hfont = build_font(ascii_font_name, STAT_FONT_HEIGHT, EZ_ATTR_BOLD) ;
   HFONT hfont = build_font(fname, fheight, flags) ;
   if (hfont == 0) {
      syslog("build_font: %s\n", get_system_message()) ;
   } else {
      PostMessage(hwnd, WM_SETFONT, (WPARAM) hfont, (LPARAM) true) ;
   }
}

//**********************************************************************
static void set_hwnd_values(void)
{
   hwndMsg      = GetDlgItem(hwndMain, IDC_MSG) ;
   hwndKbdState = GetDlgItem(hwndMain, IDC_KSTATE) ;
   
   hwndRegs[0]  = GetDlgItem(hwndMain, IDC_R0) ;
   hwndRegs[1]  = GetDlgItem(hwndMain, IDC_R1) ;
   hwndRegs[2]  = GetDlgItem(hwndMain, IDC_R2) ;
   hwndRegs[3]  = GetDlgItem(hwndMain, IDC_R3) ;
   hwndRegs[4]  = GetDlgItem(hwndMain, IDC_R4) ;
   hwndRegs[5]  = GetDlgItem(hwndMain, IDC_R5) ;
   hwndRegs[6]  = GetDlgItem(hwndMain, IDC_R6) ;
   hwndRegs[7]  = GetDlgItem(hwndMain, IDC_R7) ;
   hwndRegs[8]  = GetDlgItem(hwndMain, IDC_R8) ;
   hwndRegs[9]  = GetDlgItem(hwndMain, IDC_R9) ;
   
   hwndStack[0] = GetDlgItem(hwndMain, IDC_REG_X) ;
   hwndStack[1] = GetDlgItem(hwndMain, IDC_REG_Y);
   hwndStack[2] = GetDlgItem(hwndMain, IDC_REG_Z) ;
   hwndStack[3] = GetDlgItem(hwndMain, IDC_REG_T) ;
   
}

//*******************************************************************
//lint -esym(714, status_message)
//lint -esym(759, status_message)
//lint -esym(765, status_message)
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
void show_keyboard_state(char *msg)
{
   SetWindowText(hwndKbdState, msg) ;
}

//*************************************************************
void put_stack(unsigned n, char *msg)
{
   // char tempstr[30];
   // wsprintf (tempstr, "put_stack %u [%s]", n, get_iostr());
   // syslog(tempstr);
   SetWindowText(hwndStack[n], msg) ;
}

void put_register(unsigned n, char *msg)
{
   // wsprintf (tempstr, "put_register %u", n);
   // Statusbar_ShowMessage (tempstr);
   SetWindowText(hwndRegs[n], msg) ;
}

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

//************************************************************************
//  hide all functional buttons while keyboard entry mode is in effect
//************************************************************************
static uint const show_hide_button_ids[] = {
IDB_CLEAR_R0, 
IDB_CLEAR_R1, 
IDB_CLEAR_R2, 
IDB_CLEAR_R3, 
IDB_CLEAR_R4, 
IDB_CLEAR_R5, 
IDB_CLEAR_R6, 
IDB_CLEAR_R7, 
IDB_CLEAR_R8, 
IDB_CLEAR_R9, 
IDB_CLEAR_X,  
IDB_CLEAR_Y,  
IDB_CLEAR_Z,  
IDB_CLEAR_T,  
IDB_IDB_CLRS, 
IDB_IDB_CLRR, 
IDB_IDB_FLTSCI,
IDB_IDB_GRP35,
0 } ;

void show_hide_buttons(bool show)
{
   uint idx ;
   for (idx=0; show_hide_button_ids[idx]; idx++) {
      ShowWindow(GetDlgItem(hwndMain, show_hide_button_ids[idx]), (show) ? SW_SHOW : SW_HIDE);
   }
}


//*************************************************************

static BOOL CALLBACK InitProc (HWND hDlgWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   //***************************************************
   //  debug: log all windows messages
   //***************************************************
   // if (dbg_flags & DBG_WINMSGS) {
   if (false) {   //lint !e506 !e774
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

      hwndTitle = GetDlgItem(hwndMain, IDC_TITLE) ;
      set_control_font(hwndTitle, font_name_message, FONT_HEIGHT_TITLE, EZ_ATTR_BOLD);
      SetWindowText(hwndTitle, Version);
      
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
      Initialize(1, NULL);
      show_status_info();
      keyboard_state_set(KBD_STATE_DEFAULT);
      Message("We are ready...");
      }
      return TRUE;

   case WM_COMMAND:
      {  //  create local context
      DWORD cmd = HIWORD (wParam) ;
      DWORD target = LOWORD(wParam) ;

      switch (cmd) {
      case FVIRTKEY:  //  keyboard accelerators: WARNING: same code as CBN_SELCHANGE !!
         //  fall through to BM_CLICKED, which uses same targets
      case BN_CLICKED:
         switch(target) {
            
         case IDB_KBD_0  :    keyboard_state_handler(k0);       break ;
         case IDB_KBD_1  :    keyboard_state_handler(k1);       break ;
         case IDB_KBD_2  :    keyboard_state_handler(k2);       break ;
         case IDB_KBD_3  :    keyboard_state_handler(k3);       break ;
         case IDB_KBD_4  :    keyboard_state_handler(k4);       break ;
         case IDB_KBD_5  :    keyboard_state_handler(k5);       break ;
         case IDB_KBD_6  :    keyboard_state_handler(k6);       break ;
         case IDB_KBD_7  :    keyboard_state_handler(k7);       break ;
         case IDB_KBD_8  :    keyboard_state_handler(k8);       break ;
         case IDB_KBD_9  :    keyboard_state_handler(k9);       break ;
         case IDB_KBD_DOT:    keyboard_state_handler('.');      break ;
         case IDB_KBD_e  :    keyboard_state_handler(ke);       break ;
         case IDB_KBD_s  :    keyboard_state_handler(ks);       break ;
         case IDB_KBD_BKSPC:  keyboard_state_handler(kBSPACE);  break ;
         case IDB_KBD_RETURN: keyboard_state_handler(kENTER);   break ;            
         
         case IDB_KBD_ESCAPE  : 
            switch (keyboard_state_get()) {
            case KBD_STATE_DEFAULT:
               PostMessageA(hDlgWnd, WM_CLOSE, 0, 0);
               break ;
            case KBD_STATE_GETX:
               keyboard_state_handler(kESC);   
               break ;
            }
            break ;
            
         case IDB_KBD_add:
            dos_main(ADD);
            break ;
         case IDB_KBD_subtract:
            dos_main(SUBTRACT);
            break ;
         case IDB_KBD_mult:
            dos_main(MULTIPLY);
            break ;
         case IDB_KBD_divide:
            dos_main(DIVIDE);
            break ;
            
         //  button operations
         case IDB_IDB_FLTSCI:
            dos_main(SCINOT);
            break ;
         
         case IDB_IDB_GRP35:
            dos_main(GROUPSIZE);
            break ;
            
         case IDB_CLEAR_R0:
         case IDB_CLEAR_R1:
         case IDB_CLEAR_R2:
         case IDB_CLEAR_R3:
         case IDB_CLEAR_R4:
         case IDB_CLEAR_R5:
         case IDB_CLEAR_R6:
         case IDB_CLEAR_R7:
         case IDB_CLEAR_R8:
         case IDB_CLEAR_R9:
         case IDB_CLEAR_X:
         case IDB_CLEAR_Y:
         case IDB_CLEAR_Z:
         case IDB_CLEAR_T:
         case IDB_IDB_CLRS:
         case IDB_IDB_CLRR:
            clear_stack_or_register(target);
            break ;
               
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
}  //lint !e715

//*****************************************************************
INT WINAPI WinMain (HINSTANCE hInstance,
   HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateDialog (hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL,
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
}  //lint !e715

