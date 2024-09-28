/*
 *    **************************************************
 *    *                                                *
 *    *                    BIGIO.C                     *
 *    *                                                *
 *    *          Extended Precision Calculator         *
 *    *                                                *
 *    *             Low Level I/O Routines             *
 *    *                                                *
 *    *              Version 5.0 01-01-99              *
 *    *                                                *
 *    *              Judson D. McClendon               *
 *    *              Sun Valley Systems                *
 *    *              4522 Shadow Ridge Pkwy            *
 *    *              Pinson, AL 35126-2192             *
 *    *                 205-680-0460                   *
 *    *                                                *
 *    **************************************************
 */
/*
 *    **************************************************
 *    *                                                *
 *    *                   Includes                     *
 *    *                                                *
 *    **************************************************
 */

#include <windows.h>
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "bigcalc.h"

//lint -esym(715, row, outstr, col) 

/*
 *    **************************************************
 *    *                                                *
 *    *                   Constants                    *
 *    *                                                *
 *    **************************************************
 */

// #define CLOCKFREQ       ((unsigned)1193180L) /* Timer frequency        */
// #define SPEAKERMODE     ((unsigned)0xB6)     /* Set timer for speaker  */
// #define TIMERMODEPORT   ((unsigned)0x43)     /* Timer mode port        */
// #define FREQPORT        ((unsigned)0x42)     /* Frequency-control port */
// #define SPEAKERPORT     ((unsigned)0x61)     /* Speaker port           */
// #define SPEAKERON       ((unsigned)0x03)     /* Speaker-on bits        */
// #define FREQ            ((unsigned)400)      /* A frequency            */
// #define CFREQ           CLOCKFREQ / FREQ     /* Division frequency     */

// #define MONOCOLOR       0x07                 /* Grey on black          */
// #define COLORCOLOR      0x17                 /* White on blue          */
// #define MONOSCREEN      0xB0000000L          /* Mono buffer address    */
// #define COLORSCREEN     0xB8000000L          /* Color buffer address   */

/*
 *    **************************************************
 *    *                                                *
 *    *            Source Local Variables              *
 *    *                                                *
 *    **************************************************
 */

// static char color = 0;                    /* Screen Color    */

//**********************************************************
//  conio.cpp functions
//**********************************************************

//**********************************************************
static void dprints(unsigned row, unsigned col, const char* outstr)
   {
   // dgotoxy(col, row) ;
   // dputs(outstr) ;
   }   

/*
 *    **************************************************
 *    *                                                *
 *    *              Keyboard Routines                 *
 *    *                                                *
 *    **************************************************
 */

/*
 *    **************************************************
 *    *                                                *
 *    *         Get decoded character from kbd         *
 *    *                                                *
 *    **************************************************
 */
extern int GetChar(void)
{
   int chr;

   chr = getch();
   if (chr == 0)
      chr = 1000 + getch();      /* Non ASCII character */
   else
      if (isascii(chr) )
         chr = toupper(chr);

   return(chr);
}

/*
 *    **************************************************
 *    *                                                *
 *    *         Return character if key pressed        *
 Only used to interrupt long math operations,
 But not usable in Windows program
 *    *                                                *
 *    **************************************************
 */
// int KeyPressed(void)
// {
//    if (kbhit())
//       return(GetChar());
//    else
//       return(0);
// }

/*
 *    **************************************************
 *    *         Position Cursor at row, column         *
 //  not actually used in this application
 *    **************************************************
 */
static void CurPos(int row, int col)
{
   // dgotoxy(col-1, row-1) ;
}

//***************************************************************************
#define  MAX_OUTPUT_STR    1100
static char output_str[MAX_OUTPUT_STR+1];
static uint outstr_idx = 0 ;

//***************************************************************************
void reset_output_str(void)
{
   ZeroMemory(output_str, MAX_OUTPUT_STR+1);
   outstr_idx = 0 ;
}

//***********************************************************
char *get_output_str(void)
{
   return output_str ;
}

//lint -esym(714, get_output_str_len)
//lint -esym(759, get_output_str_len)
//lint -esym(765, get_output_str_len)
uint get_output_str_len(void)
{
   return outstr_idx ;
}

/*
 *    **************************************************
 *    *            Write Character to Screen           *
 *    **************************************************
 */
void WChar(int chr)
{
   // dputc((char) chr) ;
   if (outstr_idx >= MAX_OUTPUT_STR) {
      syslog("output string overrun (char)\n");
      return ;
   }
   // syslog("%X [%c]", (unsigned char) chr, (char) chr);
   output_str[outstr_idx++] = (char) chr ;
   output_str[outstr_idx] = 0 ;  // keep string NULL-terminated
   // syslog("WChar: %u: [%s]\n", get_output_str_len(), get_output_str());
}

/*
 *    **************************************************
 *    *             Write String to Screen             *
 *    **************************************************
 */
void WString(char *str)
{
   // dputs(str) ;
   uint slen = strlen(str);
   if ((outstr_idx + slen) >= MAX_OUTPUT_STR) {
      syslog("output string overrun (string)\n");
      return ;
   }
   // syslog("%u <%s>\n", slen, str);
   uint idx ;
   for (idx=0; idx<slen; idx++) {
      WChar(*(str+idx));
   }
}

/*
 *    **************************************************
 *    *                                                *
 *    *         Display Character at row, col          *
 *    *                                                *
 *    **************************************************
 */
void DisplayChar(int *row, int *col, int chr)
{
   // CurPos(*row, *col);
   WChar(chr);

   // if (*col < MAXDISPCOL)     /* Find position for next character */
   //    (*col)++;
   // else {
   //    *col = MINDISPCOL;
   //    (*row)++;
   //    }
   // CurPos(*row, *col);
}

/*
 *    **************************************************
 *    *                                                *
 *    *             Write Integer to Screen            *
 *    *                                                *
 *    **************************************************
 */
void WInteger(long integer)
{
   long order;

   if (integer) {
      if (integer < 0L) {
         WChar('-');
         integer = - integer;
         }
      order = 1000000000L;
      while (order > integer) {
         order /= 10L;
         }
      while (order) {
         WChar((int)((integer / order) + (long)'0'));
         integer %= order;
         order /= 10L;
         }
      }
   else
      WChar('0');
}

/*
 *    **************************************************
 *    *                                                *
 *    *       Write String at row, col to Screen       *
 *    *                                                *
 *    **************************************************
 */

void WriteAt(int row, int col, char *str)
{
   dprints(row-1, col-1, str) ;
}

/*
 *    **************************************************
 *    *                                                *
 *    *     Display Message centered on row line       *
 *    *                                                *
 *    **************************************************
 */
// static char *spaces80 = 
//    "                                        "
//    "                                        " ;
// void WriteCenter(int row, char *msg)
// {
//    dprints(row-1, 0, spaces80) ;
//    dprints(row-1, ((80 - strlen(msg)) / 2), msg) ;
// }

/*
 *    **************************************************
 *    *                                                *
 *    *      Display Message centered on 25th line     *
 *    *                                                *
 *    **************************************************
 */
// void Message(char *msg)
// {
//    WriteCenter(25, msg);
// }

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
   strcat(tmsg, "(Press a key to continue)");

   MessageBox(NULL, tmsg, NULL, MB_OK | MB_ICONEXCLAMATION);
   // GetChar();
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
   strcat(tmsg, "(Press Esc to abort)");

   Message(tmsg);
}

/*
 *    **************************************************
 *    *                                                *
 *    *         Number Entry Support Routines          *
 *    *                                                *
 *    **************************************************
 */


/*
 *    **************************************************
 *    *                                                *
 *    *               Backspace Character              *
 *    *                                                *
 *    **************************************************
 */
void BackSpace(int *row, int *col)
{
   if (*col > MINDISPCOL)     /* find previous character position */
      (*col)--;
   else {
      *col = MAXDISPCOL;
      (*row)--;
      }

   CurPos(*row, *col);
   WChar(' ');
   CurPos(*row, *col);
}

/*
 *    **************************************************
 *    *                                                *
 *    *     Display Exponent Character at row, col     *
 *    *                                                *
 *    **************************************************
 */
void DisplayExpChar(int *row, int *col, int chr)
{
   CurPos(*row, *col);
   WChar(chr);

   if (*col < 80)             /* Find position for next character */
      (*col)++;
   else {
      *col = MINDISPCOL;
      (*row)++;
      }
   CurPos(*row, *col);
}

/*
 *    **************************************************
 *    *                                                *
 *    *          Backspace Exponent Character          *
 *    *                                                *
 *    **************************************************
 */
void BackSpaceExp(int *row, int *col)
{
   if (*col > MINDISPCOL)     /* find previous character position */
      (*col)--;
   else {
      *col = 80;
      (*row)--;
      }

   CurPos(*row, *col);
   WChar(' ');
   CurPos(*row, *col);
}

/*
 *    **************************************************
 *    *                                                *
 *    *          Display Exponent at row, col          *
 *    *                                                *
 *    **************************************************
 */
void DisplayExp(int *row, int *col, int exprow, int expcol, int expsign, long exponent)
{
   long order;

   *row = exprow;
   *col = expcol;
   // CurPos(exprow, expcol);          /* Locate to beginning of exponent */
   // EraEol();

   if (expsign == '-')
      DisplayExpChar(row, col, '-');

   if (exponent) {                  /* Write exponent value */
      if (exponent < 0L) {
         DisplayExpChar(row, col, '-');
         exponent = - exponent;
         }
      order = 1000000000L;
      while (order > exponent)
         order /= 10L;
      while (order) {
         DisplayExpChar(row, col, (int)((exponent / order) + (long) '0'));
         exponent %= order;
         order /= 10L;
         }
      }
}

