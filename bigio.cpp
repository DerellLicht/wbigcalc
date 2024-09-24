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
// #define  VER_NUMBER "6.0"
// char *Version = "BigCalc, Version " VER_NUMBER " " ;

/*
 *    **************************************************
 *    *                                                *
 *    *                   Includes                     *
 *    *                                                *
 *    **************************************************
 */

#include <windows.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "bigcalc.h"
// #include "biggvar.h"
// #include "statbar.h"
// #include "conio32.h"

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

#define  MAXIOSTR    1024
static int iostrlen = 0 ;
static char iostr[MAXIOSTR+1] = "" ;

//*******************************************************************************
//  dummy functions
void dputc(char chr)
{
   //  this needs to be modified to deal with BackSpace, etc...
   if (iostrlen < MAXIOSTR) {
      // wsprintf(tempstr, "%c", chr) ;
      // OutputDebugString(tempstr) ;
      iostr[iostrlen++] = chr ;
      iostr[iostrlen] = 0 ;   //  keep string NULL-term
   }
}

void dputs(char *str)
{
   // OutputDebugString(str) ;
   int slen = strlen(str) ;
   if (iostrlen + slen <= MAXIOSTR) {
      iostrlen += wsprintf(&iostr[iostrlen], str) ;
   }
}

static void dprints(unsigned row, unsigned col, char *str)
{
//lint -esym(715, row, col)
   //  row, col are unused
   dputs(str) ;
}

char *get_iostr(void)
{
   return iostr ;
}

void reset_iostr(void)
{
   iostrlen = 0 ;
   iostr[0] = 0 ;
}

/*
 *    **************************************************
 *    *                                                *
 *    *            Source Local Variables              *
 *    *                                                *
 *    **************************************************
 */

// static char color = 0;                    /* Screen Color    */

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
   return 0;
   // int chr;
   // 
   // chr = getch();
   // if (chr == 0)
   //    chr = 1000 + getch();      /* Non ASCII character */
   // else
   //    if (isascii(chr) )
   //       chr = toupper(chr);
   // 
   // return(chr);
}

/*
 *    **************************************************
 *    *                                                *
 *    *                Screen Routines                 *
 *    *                                                *
 *    **************************************************
 */

/*
 *    **************************************************
 *    *                                                *
 *    *                Clear the Screen                *
 *    *                                                *
 *    **************************************************
 */
void ScrClr(void)
{
   // dclrscr() ;
}

/*
 *    **************************************************
 *    *              Erase to End of Line              *
 *    **************************************************
 */
void EraEol(void)
{
//    dclreol() ;
}

/*
 *    **************************************************
 *    *              Erase to End of Page              *
 *    **************************************************
 */
void EraEop(void)
{
//    dclreos() ;
}

/*
 *    **************************************************
 *    *         Position Cursor at row, column         *
 *    **************************************************
 */
void CurPos(int row, int col)
{
//    dgotoxy(col-1, row-1) ;
}

/*
 *    **************************************************
 *    *            Get Cursor row, column              *
 *    **************************************************
 */
void CurGet(int *row, int *col)
{
//    *row = _where_y() + 1;
//    *col = _where_x() + 1;
   *row = 0 ;
   *col = 0 ;
}

/*
 *    **************************************************
 *    *                                                *
 *    *            Write Character to Screen           *
 *    *                                                *
 *    **************************************************
 */
void WChar(int chr)
{
   dputc((char) chr) ;
}

/*
 *    **************************************************
 *    *                                                *
 *    *             Write String to Screen             *
 *    *                                                *
 *    **************************************************
 */
void WString(char *str)
{
   dputs(str) ;
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
//lint -esym(843, spaces80)  could be declared as const
static char * spaces80 = 
   "                                        "
   "                                        " ;
void WriteCenter(int row, char *msg)
{
   dprints(row-1, 0, spaces80) ;
   dprints(row-1, ((80 - strlen(msg)) / 2), msg) ;
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

   strcpy(tmsg, msg);
   if (*tmsg)
      strcat(tmsg, "  ");
   strcat(tmsg, "(Press a key to continue)");

   Message(tmsg);
   GetChar();
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
 *    *         Display Character at row, col          *
 *    *                                                *
 *    **************************************************
 */
void DisplayChar(int *row, int *col, int chr)
{
   CurPos(*row, *col);
   WChar(chr);

   if (*col < MAXDISPCOL)     /* Find position for next character */
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
   CurPos(exprow, expcol);          /* Locate to beginning of exponent */
   EraEol();

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

