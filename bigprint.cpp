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

#include "common.h"
#include "bigcalc.h"

/*
 *    **************************************************
 *    *                                                *
 *    *            Source Local Variables              *
 *    *                                                *
 *    **************************************************
 */

static int ppos = 0;            /* Current print column */

/*
 *    **************************************************
 *    *                                                *
 *    *               Writing routines                 *
 *    *                                                *
 *    **************************************************
 */

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

   if ( (scinotation) ||
        (exponent < MINFLOATDSP) ||
        (exponent > MAXFLOATDSP) ||
        (exponent > normprec) ) {      /* Scientific Notation */

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
            if (++ppos > 79)                 /* Until end of digits or line */
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
               if (++ppos > 79)
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
 *    *                Overflow Message                *
 *    *                                                *
 *    **************************************************
 */
void Overflow(void)
{
   MessageWait("** Overflow **");
}

/*
 *    **************************************************
 *    *                                                *
 *    *             Divide By Zero Message             *
 *    *                                                *
 *    **************************************************
 */
void DivideByZero(void)
{
   MessageWait("** Divide by zero **");
}

/*
 *    **************************************************
 *    *                                                *
 *    *              Zero Argument Message             *
 *    *                                                *
 *    **************************************************
 */
void ZeroArgument(void)
{
   MessageWait("** Zero Argument **");
}

/*
 *    **************************************************
 *    *                                                *
 *    *            Negative Argument Message           *
 *    *                                                *
 *    **************************************************
 */
void NegativeArgument(void)
{
   MessageWait("** Negative Argument **");
}

/*
 *    **************************************************
 *    *                                                *
 *    *          Argument not Integer Message          *
 *    *                                                *
 *    **************************************************
 */
void ArgumentNotInteger(void)
{
   MessageWait("** Argument not Integer **");
}

/*
 *    **************************************************
 *    *                                                *
 *    *                Argument Invalid                *
 *    *                                                *
 *    **************************************************
 */
void ArgumentInvalid(void)
{
   MessageWait("** Argument Invalid **");
}

/*
 *    **************************************************
 *    *                                                *
 *    *          Insufficient Memory Message           *
 *    *                                                *
 *    **************************************************
 */
void MemoryError(void)
{
   MessageWait("** Insufficient Memory **");
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
 *    *             Screen Header Line 2               *
 *    *                                                *
 *    **************************************************
 */
void show_status_info(void)
{
   char tempstr[20];
   sprintf (tempstr, " Precision: %d", normprec);
   status_message(1, tempstr);
   
   sprintf (tempstr, " dgroup: %d", groupsize);
   status_message(2, tempstr);
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
