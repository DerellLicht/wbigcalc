//****************************************************************************

#include <windows.h>
#include <stdio.h>   //  FILE* 
#include <time.h>

#include "bigcalc.h"
// #include "biggvar.h"
#include "resource.h"
#include "wcommon.h"
#include "statbar.h"
#include "keywin32.h"

#define  Version  "BigCalc for Windows, Version 1.00"

static char const szAppName[] = "WinBigCalc";
static char const szAppDesc[] = Version ;

extern void Initialize(int argc,char *argv);
extern int dos_main(unsigned inchr);

#define  USE_SYS_BG_COLOR  1
//***********************************************************************
static unsigned key_mask = 0;

static char tempstr[1024] ;
//****************************************************************************
static int process_keystroke (HWND hwnd, unsigned inchr);

//****************************************************************************
static LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

//***********************************************************************
static HINSTANCE g_hinst = 0;

//***********************************************************************
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                  PSTR szCmdLine, int iCmdShow)
   {
   HWND        hwnd ;
   MSG         msg ;
   WNDCLASSEX  wndclass ;
   // WNDCLASS  wndclass ;

   g_hinst = hInstance;

   Initialize(0, 0);

   //*******************************************
   //  register main window
   //*******************************************
   wndclass.cbSize        = sizeof (wndclass) ;
   wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
   wndclass.lpfnWndProc   = WndProc ;
   wndclass.cbClsExtra    = 0 ;
   wndclass.cbWndExtra    = 0 ;
   wndclass.hIcon         = LoadIcon (g_hinst, MAKEINTRESOURCE(IDI_ICON));
   // wndclass.hIcon         = (HICON) LoadImage (hInstance, 
   //                               MAKEINTRESOURCE(WWCELTIC), IMAGE_ICON, 
   //                               0, 0, LR_DEFAULTSIZE);
   wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
#ifdef   USE_SYS_BG_COLOR  
   wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
#else
   wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH) ;
#endif
   // wndclass.lpszMenuName  = MAKEINTRESOURCE(IDR_WNDIR_MENU) ;
   wndclass.lpszMenuName  = NULL;
   wndclass.lpszClassName = szAppName ;
   wndclass.hInstance     = hInstance ;
   wndclass.hIconSm       = LoadIcon (g_hinst, MAKEINTRESOURCE(IDI_ICON));
   // wndclass.hIconSm       = (HICON) LoadImage (hInstance, 
   //                               MAKEINTRESOURCE(WWARMOUR), IMAGE_ICON, 
   //                               0, 0, LR_DEFAULTSIZE);

   RegisterClassEx (&wndclass) ;
   // RegisterClass (&wndclass) ;

   //*******************************************
   //  create both windows
   //*******************************************
   hwnd = CreateWindow (szAppName, szAppDesc,
                        // WS_OVERLAPPEDWINDOW,
                        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        // CW_USEDEFAULT, CW_USEDEFAULT,
                        650, 610,
                        NULL, NULL, hInstance, NULL) ;   //lint !e569

   // hwndZot = CreateWindow("ZotWindow","ZotWindow",WS_OVERLAPPEDWINDOW,
   //    200,200,640,540,0,0,hInstance,0);

   //*******************************************
   //  open and handle main window
   //*******************************************
   ShowWindow (hwnd, SW_SHOWNORMAL) ;
   UpdateWindow (hwnd) ;

   while (GetMessage (&msg, NULL, 0, 0)) {
      TranslateMessage (&msg) ;
      DispatchMessage (&msg) ;
   }

   return msg.wParam ;
}  //lint !e715

//**********************************************************************
// #define IDC_STATIC   1020
// #define IDC_MSG_AREA 1021

// HWND hwndRegX = 0 ;
// HWND hwndRegY = 0 ;
// HWND hwndRegZ = 0 ;
// HWND hwndRegT = 0 ;

static HWND hwndRegs[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } ;
static HWND hwndStack[4] = { 0, 0, 0, 0 } ;

// HWND hwndParent = 0 ;
static int cxClient = 0 ;
static int cyClient = 0 ;

#define  FONT_HEIGHT    20
#define  REG_OFFSET     (FONT_HEIGHT + 6)
#define  REG_LEFT       10
#define  REG_WIDTH      200

static LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
   // HDC hdc;
   // HMENU hMenu;
   // PAINTSTRUCT ps;
   // static char szFileName[_MAX_PATH];
   // static char szTitleName[_MAX_PATH];
   // unsigned cx, cy;
   // int result ;
   unsigned dy ;
//lint -esym(550, left_drag_state)
   static bool left_drag_state = false;
   // hwndParent = hwnd ;  //  used to receive messages from children

   switch (iMsg) {
   // case WM_CTLCOLORSTATIC:
   //    {
   //       HDC hdcStatic = (HDC) wParam;
   //       HBRUSH g_hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
   //       SetTextColor (hdcStatic, RGB (0, 0, 0));
   //       SetBkMode (hdcStatic, OPAQUE);
   //       SetBkColor(hdcStatic, COLOR_3DFACE+1) ;
   //       return (LONG) g_hbrBackground;
   //    }
   //    break;

   case WM_CREATE:
      cxClient = 0;
      cyClient = 0;

      //  specify initial font
      // set_default_font (); //  init_msg_window tries to do this!!

      // hwndMsg = CreateText(" ", 0, 361, 20, 20, IDC_MSG_AREA, hwnd, g_hinst);
      // init_msg_window(hwnd, 0, 361, 20, 20) ;

      // create_status_windows(hwnd) ;
      dy = 10 ;
      CreateStatic(" Stack ", REG_LEFT, dy, REG_WIDTH, FONT_HEIGHT, IDC_REG_X, hwnd, g_hinst);
      dy += REG_OFFSET ;
      sprintf(tempstr, " T reg %u ", regT) ;
      hwndStack[regT] = CreateText(tempstr, REG_LEFT, dy, REG_WIDTH, FONT_HEIGHT, IDC_REG_T, hwnd, g_hinst);
      dy += REG_OFFSET ;
      sprintf(tempstr, " Z reg %u ", regZ) ;
      hwndStack[regZ] = CreateText(tempstr, REG_LEFT, dy, REG_WIDTH, FONT_HEIGHT, IDC_REG_Z, hwnd, g_hinst);
      dy += REG_OFFSET ;
      sprintf(tempstr, " Y reg %u ", regY) ;
      hwndStack[regY] = CreateText(tempstr, REG_LEFT, dy, REG_WIDTH, FONT_HEIGHT, IDC_REG_Y, hwnd, g_hinst);
      dy += REG_OFFSET ;
      sprintf(tempstr, " X reg %u ", regX) ;
      hwndStack[regX] = CreateText(tempstr, REG_LEFT, dy, REG_WIDTH, FONT_HEIGHT, IDC_REG_X, hwnd, g_hinst);
      dy += REG_OFFSET ;
      dy += REG_OFFSET ;
      CreateStatic(" Registers ", REG_LEFT, dy, REG_WIDTH, FONT_HEIGHT, IDC_REG_X, hwnd, g_hinst);
      for (unsigned j=0; j<10; j++) {
         dy += REG_OFFSET ;
         hwndRegs[j] = CreateText(" 0 ", REG_LEFT, dy, REG_WIDTH, FONT_HEIGHT, IDC_REG_T, hwnd, g_hinst);
      }

      // SetBkMode (hdcStatic, OPAQUE);
      // SetBkColor(hdcStatic, COLOR_3DFACE+1) ;
      // ShowWindow (hwndRegX, SW_SHOW) ;
      // ShowWindow (hwndRegY, SW_SHOW) ;
      // ShowWindow (hwndRegZ, SW_SHOW) ;
      // ShowWindow (hwndRegT, SW_SHOW) ;
      //  create status bar
      InitStatusBar (hwnd);
      break;

   case WM_SIZE:
      {
      int cyStatus;
      int sbparts[3];
      RECT rWindow;
//lint -esym(438, stTop)
      unsigned stTop ;
      cxClient = LOWORD (lParam);
      cyClient = HIWORD (lParam);   //  subtrace height of status bar

      // Adjust status bar size.
      if (IsStatusBarVisible()) {
         GetStatusBarRect (&rWindow);
         cyStatus = rWindow.bottom - rWindow.top;
         stTop = cyClient - cyStatus ;
         MoveStatusBar(0, stTop, cxClient, cyStatus);

         //  re-position status-bar parts
         // sbparts[0] = 6 * cxClient / 10 ;
         // sbparts[1] = 8 * cxClient / 10 ;
         sbparts[0] = cxClient - 200;
         sbparts[1] = cxClient - 100;
         sbparts[2] = -1;
         StatusBar_SetParts(3, &sbparts[0]);

         //  subtract height of status bar
         cyClient -= cyStatus;
      } else {
         stTop = cyClient ;
      }

      //  resize the text window
      // init_msg_window(hwnd, 0, 361, cxClient, stTop-361) ;
      // MoveWindow (hwndMsg,  0, 361, cxClient, stTop-361, TRUE);
      // init_msg_window(hwnd, 0, 361, cxClient, stTop-361) ;
      // 
      // RECT mWindow ;
      // GetClientRect(hwndMsg, &mWindow) ;
      // set_client_area(mWindow.right-mWindow.left, 
      //                 mWindow.bottom-mWindow.top) ;
      // gotoxy(hwndMsg, 0, 0) ;

      // InvalidateRgn (hwnd, NULL, FALSE);
      //  draw area separators
      // hdc = GetDC (hwnd) ;
      // draw_main_window(hdc) ;
      // ReleaseDC (hwnd, hdc) ;
      // set_message_area_font() ;
      }
      return 0;

   case WM_SETFOCUS:
      InvalidateRgn (hwnd, NULL, FALSE);
      return 0;

   case WM_PAINT:
      // hdc = BeginPaint (hwnd, &ps);
      // if (keymap == KEYMAP_INTRO) {
      //    draw_intro_screen(hdc) ;
      // } else {
      //    draw_main_screen(hdc) ;
      //    update_room(hdc) ;
      //    // ShowWindow (hwndMsg, SW_SHOW) ;
      //    wsprintf(tempstr, "you are entering %s", names[player.castle_nbr]) ;
      //    put_message(tempstr) ;
      // }
      // EndPaint (hwnd, &ps);
      return 0;

   //**********************************************
   //  menu handlers
   //**********************************************
   // case WM_COMMAND :
   //    key_mask &= ~kAlt ;  //  just in case this was done by key!!
   //    menu_active = 1 ;
   // 
   //    switch (LOWORD(wParam)) {
   //    case ID_FILE_EXIT:
   //       // destroy_debug_window();
   //       SendMessage(hwnd, WM_DESTROY, 0, 0) ;
   //       break;
   // 
   //    case ID_STUFF_GO:
   //       break;
   // 
   //    case ID_HELP_CMD:
   //       // goto_state(hwnd, ND_SHOW_HELP) ; 
   //       break;
   // 
   //    case ID_ABOUT:
   //       // goto_state(hwnd, ND_SHOW_VER) ; 
   //       break;
   // 
   //    //  if it's not a specific known ID, search the path list
   //    default:
   //       // select_new_path(LOWORD(wParam)) ;
   //       // RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE) ;
   //       break;
   //    }
   // 
   //    return 0;

   //**********************************************
   // Status bar settings
   //**********************************************
   case IDM_STAT_SIZEGRIP:
   case IDM_STAT_TOP:
   case IDM_STAT_BOTTOM:
   case IDM_STAT_NOMOVEY:
   case IDM_STAT_NOPARENTALIGN:
   case IDM_STAT_NORESIZE:
      DestroyStatusBar() ;
      RebuildStatusBar (hwnd, LOWORD (wParam));
      break;

   // Status bar messages
   case IDM_ST_GETBORDERS:
   case IDM_ST_GETPARTS:
   case IDM_ST_SETTEXT:
   case IDM_ST_SIMPLE:
      StatusBarMessage (LOWORD (wParam));
      break;

   // case WM_MENUSELECT:
   //   return Statusbar_MenuSelect (hwnd, wParam, lParam);

   case WM_LBUTTONDOWN:
      // cx = LOWORD (lParam);
      // cy = HIWORD (lParam);
      left_drag_state = true;
      // p = is_point_selected (cx, cy);
      // if (p != 0) {
      //   wsprintf (tempstr, "point %d selected", p->line1->id);
      //   Statusbar_ShowMessage (tempstr);
      // }
      return 0;

   case WM_LBUTTONUP:
      left_drag_state = false;
      Statusbar_ShowMessage ("");
      // InvalidateRect (hwnd, NULL, FALSE) ;
      return 0;

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
         wParam |= key_mask;
         process_keystroke (hwnd, wParam);
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

   case WM_DESTROY:
      // DeleteObject ((HGDIOBJ) hSpriteBitmap);
      PostQuitMessage (0);
      return 0;
   }  //lint !e744 no default
   return DefWindowProc (hwnd, iMsg, wParam, lParam);
}

//*************************************************************
static int process_keystroke (HWND hwnd, unsigned inchr)
{
   // int result ;
   
   // do_idle_tasks() ;

   switch (inchr) {
   case kCc:
   case kESC:
      SendMessage(hwnd, WM_DESTROY, 0, 0) ;
      break;

   default:
      wsprintf (tempstr, "PRESS=0x%04X", inchr);
      Statusbar_ShowMessage (tempstr);
      dos_main(inchr) ;
      break;
   }
   return 0;
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

