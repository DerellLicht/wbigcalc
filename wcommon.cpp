//*****************************************************************
//  extract common code for standard derelict Windows operations.
//  
//  - Find INI filename
//  - Create addon objects for Windows applications
//  
//  Written by:   Daniel D. Miller
//  
//  Last Update:  07/11/03 12:57
//*****************************************************************

#include <windows.h>
#include "wcommon.h"

//lint -esym(714, CreateButton, CreateCheck, CreateEdit, CreateText)
//lint -esym(759, CreateButton, CreateCheck, CreateEdit, CreateText)
//lint -esym(765, CreateButton, CreateCheck, CreateEdit, CreateText)
//lint -esym(714, get_system_message)
//lint -esym(759, get_system_message)
//lint -esym(765, get_system_message)

//*************************************************************
//  each subsequent call to this function overwrites 
//  the previous report.
//*************************************************************
char *get_system_message(void)
{
   static char msg[261] ;
   int slen ;

   LPVOID lpMsgBuf;
   FormatMessage( 
      FORMAT_MESSAGE_ALLOCATE_BUFFER | 
      FORMAT_MESSAGE_FROM_SYSTEM | 
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      GetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &lpMsgBuf,
      0, 0);
   // Process any inserts in lpMsgBuf.
   // ...
   // Display the string.
   strncpy(msg, (char *) lpMsgBuf, 260) ;

   // Free the buffer.
   LocalFree( lpMsgBuf );

   //  trim the newline off the message before copying it...
   slen = strlen(msg) ;
   if (msg[slen-1] == 10  ||  msg[slen-1] == 13) {
      msg[slen-1] = 0 ;
   }

   return msg;
}

//********************************************************
// A function to set a control's text to
//     the default font
//lint -esym(714, SetDefaultFont)
//lint -esym(759, SetDefaultFont)
//lint -esym(765, SetDefaultFont)
int SetDefaultFont (int identifier, HWND hwnd)
{
   SendDlgItemMessage (hwnd,
      identifier,
      WM_SETFONT,
      (WPARAM) GetStockObject (DEFAULT_GUI_FONT), MAKELPARAM (TRUE, 0));   //lint !e845
   return 0;
}

// A function to create static text
HWND CreateStatic (char *tempText, int x, int y, int width, int height,
   int identifier, HWND hwnd, HINSTANCE g_hInst)
{
   HWND hStaticTemp;

   hStaticTemp = CreateWindowEx (
      0,
      // WS_EX_STATICEDGE,
      "STATIC",
      tempText,
      WS_CHILD | WS_VISIBLE,
      x, y, width, height, hwnd, (HMENU) identifier, g_hInst, NULL);

   return hStaticTemp;
}

// A function to create a textarea
HWND CreateEdit (char *tempText, int x, int y, int width, int height,
   int identifier, HWND hwnd, HINSTANCE g_hInst)
{
   HWND hEditTemp;

   hEditTemp = CreateWindowEx (WS_EX_CLIENTEDGE,
      "EDIT",
      tempText,
      WS_CHILD | WS_VISIBLE,
      x, y, width, height, hwnd, (HMENU) identifier, g_hInst, NULL);

   return hEditTemp;
}

// A function to create a textarea
HWND CreateText(char *tempText, int x, int y, int width, int height,
   int identifier, HWND hwnd, HINSTANCE g_hInst)
{
   HWND hEditTemp;

   hEditTemp = CreateWindowEx (
      // WS_EX_STATICEDGE,
      WS_EX_OVERLAPPEDWINDOW | WS_EX_TOPMOST,
      "STATIC",
      tempText,
      WS_CHILD | WS_VISIBLE,
      x, y, width, height, hwnd, (HMENU) identifier, g_hInst, NULL);

   return hEditTemp;
}

HWND CreateText(char *tempText, int x, int y, int width, int height, unsigned color,
   int identifier, HWND hwnd, HINSTANCE g_hInst)
{
   HWND hEditTemp;

   hEditTemp = CreateWindowEx (
      // WS_EX_STATICEDGE,
      WS_EX_OVERLAPPEDWINDOW | WS_EX_TOPMOST,
      "STATIC",
      tempText,
      WS_CHILD | WS_VISIBLE,
      x, y, width, height, hwnd, (HMENU) identifier, g_hInst, NULL);

   return hEditTemp;
}  //lint !e715  color

// A function to create a button
HWND CreateButton (char *tempText, int x, int y, int width, int height,
   int identifier, HWND hwnd, HINSTANCE g_hInst)
{
   HWND hButtonTemp;

   hButtonTemp = CreateWindowEx (0,
      "BUTTON",
      tempText,
      WS_CHILD | WS_VISIBLE,
      x, y, width, height, hwnd, (HMENU) identifier, g_hInst, NULL);

   return hButtonTemp;
}

// A function to create a checkbox
HWND CreateCheck (char *tempText, int x, int y, int width, int height,
   int identifier, HWND hwnd, HINSTANCE g_hInst)
{
   HWND hCheckTemp;

   hCheckTemp = CreateWindowEx (0,
      "BUTTON",
      tempText,
      WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
      x, y, width, height, hwnd, (HMENU) identifier, g_hInst, NULL);

   return hCheckTemp;
}

