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

HINSTANCE hInst = NULL;
static HWND hwndMain = NULL ;
static HWND hwndTitle = NULL ;
static HWND hwndMsg = NULL ;
static HWND hwndKbdState = NULL ;
static HWND hwndViewFrame = NULL ;

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
   hwndMsg       = GetDlgItem(hwndMain,  IDC_MSG) ;
   hwndKbdState  = GetDlgItem(hwndMain,  IDC_KSTATE) ;
   hwndViewFrame = GetDlgItem(hwndMain, IDC_VIEW_DATA) ;
   
   hwndRegs[0]   = GetDlgItem(hwndMain, IDC_R0) ;
   hwndRegs[1]   = GetDlgItem(hwndMain, IDC_R1) ;
   hwndRegs[2]   = GetDlgItem(hwndMain, IDC_R2) ;
   hwndRegs[3]   = GetDlgItem(hwndMain, IDC_R3) ;
   hwndRegs[4]   = GetDlgItem(hwndMain, IDC_R4) ;
   hwndRegs[5]   = GetDlgItem(hwndMain, IDC_R5) ;
   hwndRegs[6]   = GetDlgItem(hwndMain, IDC_R6) ;
   hwndRegs[7]   = GetDlgItem(hwndMain, IDC_R7) ;
   hwndRegs[8]   = GetDlgItem(hwndMain, IDC_R8) ;
   hwndRegs[9]   = GetDlgItem(hwndMain, IDC_R9) ;
                 
   hwndStack[0]  = GetDlgItem(hwndMain, IDC_REG_X) ;
   hwndStack[1]  = GetDlgItem(hwndMain, IDC_REG_Y);
   hwndStack[2]  = GetDlgItem(hwndMain, IDC_REG_Z) ;
   hwndStack[3]  = GetDlgItem(hwndMain, IDC_REG_T) ;
   
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
static char const stackLtr[5] = "XYZT" ;

void view_data_field_full(uint fidx, char *fstr)
{
   uint r ;
   char view_str[1100] = "" ;
   if (fidx >= 10) {
      r = fidx - 10 ;   //  stack index
      sprintf(view_str, "%c:  %s", stackLtr[r], fstr);
   }
   else {
      r = fidx ;
      sprintf(view_str, "R%u:  %s", r, fstr);
   }
   SetWindowText(hwndViewFrame, view_str) ;
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
 *    *      Write Message and Wait for keystroke      *
 *    *                                                *
 *    **************************************************
 */
void MessageWait(char *msg)
{
   char tmsg[81];
   if (strlen(msg) == 0) {
      return ;
   }
   // syslog("%u [%s]\n", strlen(msg), msg);

   strcpy(tmsg, msg);
   if (*tmsg)
      strcat(tmsg, "  ");

   MessageBox(NULL, tmsg, NULL, MB_OK | MB_ICONEXCLAMATION);
}

/*
 *    **************************************************
 *    *                                                *
 *    *      Write Message and Prompt for Escape       *
 *    *                                                *
 *    **************************************************
 */
void MessageEsc(char *msg)
{
   char tmsg[81];

   strcpy(tmsg, msg);
   if (*tmsg)
      strcat(tmsg, "... ");

   Message(tmsg);
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
IDB_CLRS, 
IDB_CLRR, 
IDB_FLTSCI,
IDB_GRP35,
IDB_KBD_UP,
IDB_KBD_DOWN,
IDC_KEYS,
IDS_KEYS,
IDB_LAST_X,
IDB_XCHG_X_R,
IDB_HELP  ,
IDB_OPTIONS,
IDB_PI,
IDB_ECONST,
IDB_CHG_SIGN,
0 } ;

void show_hide_buttons(bool show)
{
   uint idx ;
   for (idx=0; show_hide_button_ids[idx]; idx++) {
      ShowWindow(GetDlgItem(hwndMain, show_hide_button_ids[idx]), (show) ? SW_SHOW : SW_HIDE);
   }
}

//************************************************************************
static uint const show_hide_all_button_ids[] = {
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
IDB_CLRS, 
IDB_CLRR, 
IDB_FLTSCI,
IDB_GRP35,
IDB_KBD_UP,
IDB_KBD_DOWN,
IDC_KEYS,
IDS_KEYS,
IDB_LAST_X,
IDB_XCHG_X_Y,
IDB_HELP  ,
IDB_OPTIONS,
IDB_Y2X   ,
IDB_SQRT  ,
IDB_XSQRD ,
IDB_INVX  ,
IDB_XFACT ,
IDB_INT   ,
IDB_FRAC  ,
IDB_PI    ,
IDB_ECONST,
IDB_SINX  ,
IDB_ASINX ,
IDB_COSX  ,
IDB_ACOSX ,
IDB_TANX  ,
IDB_ATANX ,
IDB_LOGX  ,
IDB_102X  ,
IDB_LNX   ,
IDB_E2X   ,
IDB_CHG_SIGN,
IDB_XCHG_X_R,
IDB_STORE_X,
IDB_RECALL_X,
0 } ;

void show_hide_all_buttons(bool show, uint except_id)
{
   uint idx ;
   for (idx=0; show_hide_all_button_ids[idx]; idx++) {
      
      if (show_hide_all_button_ids[idx] != except_id) {
         ShowWindow(GetDlgItem(hwndMain, show_hide_all_button_ids[idx]), (show) ? SW_SHOW : SW_HIDE);
      }
   }
}


//************************************************************************
//  Hide VIEW buttons, show XCHG buttons, for XCHG_REG function
//************************************************************************
static uint const view_button_ids[] = {
IDB_VIEW_R0,
IDB_VIEW_R1,
IDB_VIEW_R2,
IDB_VIEW_R3,
IDB_VIEW_R4,
IDB_VIEW_R5,
IDB_VIEW_R6,
IDB_VIEW_R7,
IDB_VIEW_R8,
IDB_VIEW_R9,
IDB_VIEW_X,
IDB_VIEW_Y,
IDB_VIEW_Z,
IDB_VIEW_T,
0 } ;

static uint const xchg_button_ids[] = {
IDB_XCHG_R0,
IDB_XCHG_R1,
IDB_XCHG_R2,
IDB_XCHG_R3,
IDB_XCHG_R4,
IDB_XCHG_R5,
IDB_XCHG_R6,
IDB_XCHG_R7,
IDB_XCHG_R8,
IDB_XCHG_R9,
0 } ;

static uint const stor_button_ids[] = {
IDB_STOR_R0,
IDB_STOR_R1,
IDB_STOR_R2,
IDB_STOR_R3,
IDB_STOR_R4,
IDB_STOR_R5,
IDB_STOR_R6,
IDB_STOR_R7,
IDB_STOR_R8,
IDB_STOR_R9,
0 } ;

static uint const rcall_button_ids[] = {
IDB_RCALL_R0,
IDB_RCALL_R1,
IDB_RCALL_R2,
IDB_RCALL_R3,
IDB_RCALL_R4,
IDB_RCALL_R5,
IDB_RCALL_R6,
IDB_RCALL_R7,
IDB_RCALL_R8,
IDB_RCALL_R9,
0 } ;

void show_hide_view_xchg_buttons(bool show_xchg)
{
   uint idx ;
   if (show_xchg) {
      for (idx=0; view_button_ids[idx]; idx++) {
         ShowWindow(GetDlgItem(hwndMain, view_button_ids[idx]), SW_HIDE);
      }
      for (idx=0; xchg_button_ids[idx]; idx++) {
         ShowWindow(GetDlgItem(hwndMain, xchg_button_ids[idx]), SW_SHOW);
      }
   }
   else {
      for (idx=0; xchg_button_ids[idx]; idx++) {
         ShowWindow(GetDlgItem(hwndMain, xchg_button_ids[idx]), SW_HIDE);
      }
      for (idx=0; view_button_ids[idx]; idx++) {
         ShowWindow(GetDlgItem(hwndMain, view_button_ids[idx]), SW_SHOW);
      }
   }
}

void show_hide_view_stor_buttons(bool show)
{
   uint idx ;
   if (show) {
      for (idx=0; view_button_ids[idx]; idx++) {
         ShowWindow(GetDlgItem(hwndMain, view_button_ids[idx]), SW_HIDE);
      }
      for (idx=0; stor_button_ids[idx]; idx++) {
         ShowWindow(GetDlgItem(hwndMain, stor_button_ids[idx]), SW_SHOW);
      }
   }
   else {
      for (idx=0; stor_button_ids[idx]; idx++) {
         ShowWindow(GetDlgItem(hwndMain, stor_button_ids[idx]), SW_HIDE);
      }
      for (idx=0; view_button_ids[idx]; idx++) {
         ShowWindow(GetDlgItem(hwndMain, view_button_ids[idx]), SW_SHOW);
      }
   }
}

void show_hide_view_rcall_buttons(bool show)
{
   uint idx ;
   if (show) {
      for (idx=0; view_button_ids[idx]; idx++) {
         ShowWindow(GetDlgItem(hwndMain, view_button_ids[idx]), SW_HIDE);
      }
      for (idx=0; rcall_button_ids[idx]; idx++) {
         ShowWindow(GetDlgItem(hwndMain, rcall_button_ids[idx]), SW_SHOW);
      }
   }
   else {
      for (idx=0; rcall_button_ids[idx]; idx++) {
         ShowWindow(GetDlgItem(hwndMain, rcall_button_ids[idx]), SW_HIDE);
      }
      for (idx=0; view_button_ids[idx]; idx++) {
         ShowWindow(GetDlgItem(hwndMain, view_button_ids[idx]), SW_SHOW);
      }
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
      Initialize();
      read_config_file() ;    //  read current screen position, etc
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
            case KBD_STATE_GETREG:
               keyboard_state_handler(kESC);   
               break ;
            }
            break ;
            
         case IDB_KBD_add:       dos_main(ADD); break ;
         case IDB_KBD_subtract:  dos_main(SUBTRACT); break ;
         case IDB_KBD_mult:      dos_main(MULTIPLY); break ;
         case IDB_KBD_divide:    dos_main(DIVIDE); break ;
            
         //**************************************************
         //  button operations
         //**************************************************
         //  button row 1
         case IDB_FLTSCI:    dos_main(SCINOT); break ;
         case IDB_GRP35:     dos_main(GROUPSIZE); break ;
         case IDB_KBD_UP:    dos_main(ROLLUP); break ;
         case IDB_KBD_DOWN:  dos_main(ROLLDOWN); break ;
         case IDB_LAST_X:    dos_main(LASTX); break ;
         case IDB_CHG_SIGN:  dos_main(CHGSIGN); break ;
         case IDB_XCHG_X_Y:  dos_main(XCHGXY1); break ;
         case IDB_XCHG_X_R:  dos_main(XCHGXREG); break ;
         case IDB_STORE_X:   dos_main(STOREX); break ;
         case IDB_RECALL_X:  dos_main(RECALLREG); break ;
                                        
         //  button row 2
         case IDB_Y2X   :    dos_main(POWER); break ;
         case IDB_SQRT  :    dos_main(SQUAREROOT); break ;
         case IDB_XSQRD :    dos_main(SQUARE); break ;
         case IDB_INVX  :    dos_main(RECIPROCAL); break ;
         case IDB_XFACT :    dos_main(FACTORIAL); break ;
         case IDB_INT   :    dos_main(INTEGER); break ;
         case IDB_FRAC  :    dos_main(FRACTION); break ;
         case IDB_PI    :    dos_main(RECALLPI); break ;
         case IDB_ECONST:    dos_main(RECALLE); break ;
         case IDB_HELP  :    dos_main(HELP); break ;
         case IDB_OPTIONS:   open_options_dialog(hDlgWnd); break ;
            
         //  button row 3
         case IDB_SINX :    dos_main(SIN   ); break ;
         case IDB_ASINX:    dos_main(ARCSIN); break ;
         case IDB_COSX :    dos_main(COS   ); break ;
         case IDB_ACOSX:    dos_main(ARCCOS); break ;
         case IDB_TANX :    dos_main(TAN   ); break ;
         case IDB_ATANX:    dos_main(ARCTAN); break ;
         case IDB_LOGX :    dos_main(LOG   ); break ;
         case IDB_102X :    dos_main(EXP10 ); break ;
         case IDB_LNX  :    dos_main(LN    ); break ;
         case IDB_E2X  :    dos_main(EXPE  ); break ;
            
         //**************************************************
         //  clear data fields
         //**************************************************
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
         case IDB_CLRS:
         case IDB_CLRR:
            clear_stack_or_register(target);
            break ;
               
         //**************************************************
         //  view data field with all digits
         //**************************************************
         case IDB_VIEW_R0:
         case IDB_VIEW_R1:
         case IDB_VIEW_R2:
         case IDB_VIEW_R3:
         case IDB_VIEW_R4:
         case IDB_VIEW_R5:
         case IDB_VIEW_R6:
         case IDB_VIEW_R7:
         case IDB_VIEW_R8:
         case IDB_VIEW_R9:
         case IDB_VIEW_X:
         case IDB_VIEW_Y:
         case IDB_VIEW_Z:
         case IDB_VIEW_T:
            view_stack_or_register(target);
            break ;

         //  only active Exchange X with Register state
         case IDB_XCHG_R0:
         case IDB_XCHG_R1:
         case IDB_XCHG_R2:
         case IDB_XCHG_R3:
         case IDB_XCHG_R4:
         case IDB_XCHG_R5:
         case IDB_XCHG_R6:
         case IDB_XCHG_R7:
         case IDB_XCHG_R8:
         case IDB_XCHG_R9:
            ExchangeXReg_exec(target);
            break ;
         
         //  only active Exchange X with Register state
         case IDB_STOR_R0:
         case IDB_STOR_R1:
         case IDB_STOR_R2:
         case IDB_STOR_R3:
         case IDB_STOR_R4:
         case IDB_STOR_R5:
         case IDB_STOR_R6:
         case IDB_STOR_R7:
         case IDB_STOR_R8:
         case IDB_STOR_R9:
            StoreX_exec(target);
            break ;
         
         //  only active Exchange X with Register state
         case IDB_RCALL_R0:
         case IDB_RCALL_R1:
         case IDB_RCALL_R2:
         case IDB_RCALL_R3:
         case IDB_RCALL_R4:
         case IDB_RCALL_R5:
         case IDB_RCALL_R6:
         case IDB_RCALL_R7:
         case IDB_RCALL_R8:
         case IDB_RCALL_R9:
            RecallReg_exec(target);
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
   load_exec_filename() ;  //  get our executable name

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

