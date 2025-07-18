//***************************************************************************
//  bigprint.cpp - copyright (c) Derell Licht
//  The functions in this file are used to update the register and stack fields.
//  They functions provide the abilities to translate the bigcalc data structs 
//  into meaningful output in the target field.
//  They have *no* knowledge of the bigcalc data-field structs.
//  
//***************************************************************************
//  In Judson's original code, this file also contained all the code for
//  printing to file or printer.  In the Windows implementation, I don't
//  feel that effort has any value, so I have removed it.
//  However, it is still possible to copy-and-paste from either the
//  X-register field or the View window, and use the data as desired.
//***************************************************************************
/*
 *    **************************************************
 *    *                                                *
 *    *                  BIGPRINT.C                    *
 *    *                                                *
 *    *          Extended Precision Calculator         *
 *    *                                                *
 *    *               Printing Routines                *
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

#include <windows.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "bigcalc.h"

static int ppos = 0;    //  current write position in display field

//***************************************************************************
//  output_str is used to update the register and stack fields.
//  It has *no* knowledge of the bigcalc data-field structs.
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
static char *get_output_str(void)
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
static void WChar(int chr)
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
static void WString(char *str)
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
 *    *             Write Integer to Screen            *
 *    *                                                *
 *    **************************************************
 */
static void WInteger(long integer)
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
 *    *            Write Exponent on Screen            *
 *    *                                                *
 *    **************************************************
 */
static void WExponent(long exponent)
{
   WString(" e");
   WInteger(exponent);
}

/*
 *    **************************************************
 *    *                                                *
 *    *        Write Number [*number] on Screen        *
 //  idx 0-9 are registers
 //   idx 10 = regX
 //   idx 11 = regY
 //   idx 12 = regZ
 //   idx 13 = regT
 *    *                                                *
 *    **************************************************
 */
// #define MAX_DISP_LEN   79
#define MAX_DISP_LEN   150
 
#define MINDISPCOL 4                      /* Min column for number display */
#define MAXDISPCOL 68                     /* Max column for number display */

static void WriteNumber(NORMTYPE *nbr)
{
   long exponent;
   int i, digits;
   
   reset_output_str();

   if (nbr->digits == 0) {
      WChar(' ');
      WChar('0');
      return;
   }

   if (nbr->sign == '-')
      WChar('-');
   else
      WChar(' ');
   ppos = MINDISPCOL;

   digits = nbr->digits;
   exponent = nbr->exp;

//3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594
   if ( (scinotation) ||
        (exponent < MINFLOATDSP) ||
        (exponent > MAXFLOATDSP) 
        //  DDM: do not conv to scinot if exp > normprec...
        //       we want to display full results
        // ||  (exponent > normprec) 
        ||  (exponent > DISPPREC)
        ) {      /* Scientific Notation */

      // [1288] WN: SN:0, exp:58, MIN:-100, MAX:135, np:50
      // syslog("WN: SN:%u, exp:%ld, MIN:%d, MAX:%d, np:%u\n", 
      //   (uint) (scinotation), exponent, MINFLOATDSP, MAXFLOATDSP, normprec) ;

      WChar(nbr->man[0]+ '0');         /* First digit and decimal point */
      WChar(DISPDECIMAL);
      ppos += 2;

      if (digits <= 1) {                  /* If no more digits, */
         WChar('0');                      /*  print one zero    */
         ppos++;
         }
      else
         for (i = 1; i < digits; i++) {   /*  print remaining digits */
            WChar(nbr->man[i]+ '0');
            if (++ppos >= MAXDISPCOL)
               break;
            }
      WExponent(nbr->exp - 1L);           /* Write exponent */
   }                             /* Scientific Notation end */
   else {                             /* Floating Decimal */
      if (exponent <= 0L) {                             /* Number < 1 */

         WChar(DISPDECIMAL);                 /* Decimal point */
         ppos++;

         for (i = 0; i > (int)exponent; i--) {  /* Zeros to left of number */
            WChar('0');
            ppos++;
            }

         for (i = 0; i < digits; i++) {      /* Write number digits */
            WChar(nbr->man[i] + '0');
            if (++ppos > MAX_DISP_LEN)                 /* Until end of digits or line */
               break;
         }
      }                             /* Number < 1 end */

      else {                             /* Number >= 1 */
         for (i = 0; i < (int)exponent; i++) {  /* Digits to left of decimal */
            if (i < digits)
               WChar(nbr->man[i] + '0');  /* Mantissa digits while they last */
            else
               WChar('0');                /* Zeros if mantissa exhausted */
            ppos++;
            }

         if (i < digits) {                /* Write digits to right of decimal */
            WChar(DISPDECIMAL);
            ppos++;
            for (i = (int)exponent; i < digits; i++) {
               WChar(nbr->man[i] + '0');
               if (++ppos > MAX_DISP_LEN)
                  break;
               }
            }
         }                             /* Number >= 1 end */

      }                             /* Floating Decimal end */

   // syslog("%u [%s]\n", get_output_str_len(), get_output_str());
}

/*
 *    **************************************************
 *    *                                                *
 *    *          Write Registers [lo to hi]            *
 *    *                                                *
 *    **************************************************
 */
void WriteReg(int lo, int hi)
{
   int r;
   for (r = lo; r <= hi; r++) {
      WriteNumber(&reg[r]);
      put_register(r, get_output_str());
   }
}

/*
 *    **************************************************
 *    *                                                *
 *    *            Write Stack [lo to hi]              *
 *    *                                                *
 *    **************************************************
 */
void WriteStack(int lo, int hi)
{
   int s;
   for (s = hi; s >= lo; s--) {
      WriteNumber(&stack[s]);
      put_stack(s, get_output_str());
   }
}

void WriteStack(int s)
{
   WriteNumber(&stack[s]);
   put_stack(s, get_output_str());
}

/*
 *    **************************************************
 *    *                                                *
 *    *              Display Work Screen               *
 *    *                                                *
 *    **************************************************
 */
void WorkScreen(void)
{
   int r, s;

   show_status_info();

   for (r = 0; r <= 9; r++) {
      WriteNumber(&reg[r]);
      put_register(r, get_output_str());
   }

   for (s = 3; s >= 0; s--) {
      WriteNumber(&stack[s]);
      put_stack(s, get_output_str());
   }
}
