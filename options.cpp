//**********************************************************************
//  Copyright (c) 2009-2024  Daniel D Miller
//  options.cpp - options dialog
//  
//  Written by:   Daniel D. Miller
//**********************************************************************
#include <windows.h>
#include <time.h>
#include <tchar.h>
#ifdef _lint
#include <stdlib.h>
#endif

#include "resource.h"
#include "common.h"
#include "bigcalc.h"
#include "winmsgs.h"
#include "tooltips.h"

extern HINSTANCE hInst ;

static uint const groupsize_codes[6] = {
   0, 0, 0, 
   IDC_GS_3BYTES, 0, IDC_GS_5BYTES
} ;
//****************************************************************************
//  Options dialog tooltips
//****************************************************************************
static tooltip_data_t const options_tooltips[] = {
{ IDS_NORMPREC,     _T("Set number of digits for working numbers")},
{ IDC_NORMPREC,     _T("Set number of digits for working numbers")},
{ IDG_GROUPSIZE,    _T("Set size of digit groups in View window (3 or 5)" )},
{ IDC_GS_3BYTES,    _T("Set size of digit groups in View window (3 or 5)" )},
{ IDC_GS_5BYTES,    _T("Set size of digit groups in View window (3 or 5)" )},
{ IDC_DC_STAR,      _T("Set View decimal point to * character (bigcalc legacy display)" )},
{ IDC_DC_PERIOD,    _T("Set View decimal point to . character (traditional display)" )},
{ IDM_SCINOT,       _T("Select scientific or floating-point for number display" )},
{ IDM_WINMSGS,      _T("Show WinAPI debug messages in DebugView" )},
{ IDOK,             _T("Close this dialog and accept changes" )},
{ IDCANCEL,         _T("Close this dialog and cancel changes" )},

//  This is how to enter multi-line tooltips:
// { IDS_CP_SERNUM,     _T("The SEND CMD button will send COMMAND to the device with")
//                      _T("this Serial Number.  If Serial Number is 0, COMMAND is sent ")
//                      _T("to the broadcast address on the current port.") },

{ 0, NULL }} ;

static HWND hwndNormPrec = NULL ;

//******************************************************************
static INT_PTR CALLBACK OptionsProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   uint tempEditLength, uvalue ;
   char msgstr[81] ;

   //***************************************************
   //  debug: log all windows messages 
   //***************************************************
   if (show_winmsgs) {
      switch (msg) {
      //  list messages to be ignored
      case WM_NCHITTEST:
      case WM_SETCURSOR:
      case WM_MOUSEMOVE:
      case WM_NCMOUSEMOVE:
         break;
      default:
         syslog("Opt [%s]\n", lookup_winmsg_name(msg)) ;
         break;
      }
   }

   //********************************************************************
   //  Windows message handler for this dialog
   //********************************************************************
   switch(msg) {
   case WM_INITDIALOG:
      
// #ifdef  __x86_64__      
#ifdef _WIN64
      SetClassLongPtr(hwnd, GCLP_HICON,   (LONG_PTR)LoadIcon(hInst, (LPCTSTR)IDI_ICON));
      SetClassLongPtr(hwnd, GCLP_HICONSM, (LONG_PTR)LoadIcon(hInst, (LPCTSTR)IDI_ICON));
#else
      SetClassLong(hwnd, GCL_HICON,   (LONG)LoadIcon(hInst, (LPCTSTR)IDI_ICON));
      SetClassLong(hwnd, GCL_HICONSM, (LONG)LoadIcon(hInst, (LPCTSTR)IDI_ICON));
#endif      

      //  label the dialog
      hwndNormPrec  = GetDlgItem(hwnd, IDC_NORMPREC) ;  // EDITTEXT   

      wsprintf(msgstr, " %u", normprec) ;
      SetWindowText(hwndNormPrec , msgstr);
      
      PostMessage(GetDlgItem(hwnd, IDM_WINMSGS),       BM_SETCHECK, show_winmsgs, 0) ;
      PostMessage(GetDlgItem(hwnd, IDM_SCINOT),        BM_SETCHECK, scinotation, 0) ;
      CheckRadioButton (hwnd, IDC_GS_3BYTES, IDC_GS_5BYTES, groupsize_codes[groupsize]) ;
      CheckRadioButton (hwnd, IDC_DC_STAR, IDC_DC_PERIOD, (view_dec_point_char == '.') ? IDC_DC_PERIOD : IDC_DC_STAR) ;

      create_and_add_tooltips(hwnd, 150, 100, 10000, options_tooltips);
      return TRUE ;

   //********************************************************************
   //  menu/control messages
   //********************************************************************
   case WM_COMMAND:
      if (HIWORD (wParam) == BN_CLICKED) {
         char *tptr ;

         switch(LOWORD(wParam)) {
            
         case IDM_WINMSGS:
            show_winmsgs = (show_winmsgs) ? false : true ;
            return TRUE;

         case IDM_SCINOT:
            scinotation = (scinotation) ? false : true ;
            return TRUE;

         case IDC_GS_3BYTES:
            groupsize = 3 ;
            CheckRadioButton (hwnd, IDC_GS_3BYTES, IDC_GS_5BYTES, groupsize_codes[groupsize]) ;
            return TRUE;
         case IDC_GS_5BYTES:
            groupsize = 5 ;
            CheckRadioButton (hwnd, IDC_GS_3BYTES, IDC_GS_5BYTES, groupsize_codes[groupsize]) ;
            return TRUE;
               
         case IDC_DC_STAR:
            view_dec_point_char = '*' ;
            CheckRadioButton (hwnd, IDC_DC_STAR, IDC_DC_PERIOD, IDC_DC_STAR) ;
            break ;
         case IDC_DC_PERIOD:
            view_dec_point_char = '.' ;
            CheckRadioButton (hwnd, IDC_DC_STAR, IDC_DC_PERIOD, IDC_DC_PERIOD) ;
            break ;
      
         case IDOK: //  take the new settings
            tempEditLength = GetWindowTextLength (GetDlgItem(hwnd, IDC_NORMPREC));
            GetWindowText (hwndNormPrec, msgstr, tempEditLength + 1);
            msgstr[tempEditLength] = 0;
            tptr = strip_leading_spaces(msgstr) ;
            uvalue = (uint) strtoul(tptr, 0, 0) ;
            if (uvalue >= MINPREC  &&
                uvalue <= MAXNORM) {
                normprec = uvalue ;
                compprec = COMPPREC;
                workprec = WORKPREC;
                wsprintf(msgstr, " %u", normprec) ;
                SetWindowText(hwndNormPrec , msgstr);
            }

            DestroyWindow(hwnd);
            return TRUE ;

         case IDCANCEL: //  discard all changes
            // send_commands(this_port) ;
            DestroyWindow(hwnd);
            return TRUE ;

         default:
            return FALSE;
         }
      }
      return FALSE;

   case WM_CLOSE:
      DestroyWindow(hwnd);
      return TRUE ;

   case WM_DESTROY:
      show_status_info();
      save_cfg_file();
      return TRUE ;

   default:
      return FALSE;
   }

   return FALSE;  //lint !e527
}  //lint !e715

//****************************************************************************
void open_options_dialog(HWND hwnd)
{
   // DialogBox(g_hinst, MAKEINTRESOURCE(IDD_OPTIONS), hwnd, OptionsProc) ;
   DialogBox(hInst, MAKEINTRESOURCE(IDD_OPTIONS), 0, OptionsProc) ;
}  //lint !e715  hwnd

