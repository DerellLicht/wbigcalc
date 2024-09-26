/*
 *    **************************************************
 *    *                                                *
 *    *                   BIGMATH.C                    *
 *    *                                                *
 *    *          Extended Precision Calculator         *
 *    *                                                *
 *    *        Extended Precision Math Routines        *
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bigcalc.h"
#include "biggvar.h"

static  int ResolveAngle(void);


/*
 *    **************************************************
 *    *                                                *
 *    *            Extended Math Routines              *
 *    *                                                *
 *    **************************************************
 */



/*
 *    **************************************************
 *    *                                                *
 *    *                 Extended Add                   *
 *    *                                                *
 *    *         work[2] = work[1] + work[0]            *
 *    *                                                *
 *    **************************************************
 */

extern int ExtendedAdd(void)

{

   int a, b;                       /* operand indexes           */
   long shift;                     /* digit allignment shift    */
   WORKDIGIT *ax, *ar, *cx, *ct;   /* ar = a-right, ct = c-temp */


         /* Special Cases */

   if (! work[0].digits) {             /* work[0] zero, return work[1] */
      MoveWorkWork(1, 2);
      return(TRUE);
      }

   if (! work[1].digits) {             /* work[1] zero, return work[0] */
      MoveWorkWork(0, 2);
      return(TRUE);
      }

   if (work[0].sign != work[1].sign) {       /* If signs are different, */
      work[0].sign = FlipSign(work[0].sign); /*  reverse sign           */
      return(ExtendedSubtract());            /*  and subtract           */
      }

   if (work[0].exp <= work[1].exp) {   /* a->smallest, b->largest */
      a = 0;
      b = 1;
      }
   else {
      a = 1;
      b = 0;
      }

   shift = work[b].exp - work[a].exp;
   if (shift > (long)compprec) {       /* a operand is insignificant */
      MoveWorkWork(b, 2);              /*  return b operand          */
      return(TRUE);
      }

                                       /* work[2] = b shifted 1 digit */
   work[2].man[0] = 0;
   memcpy(&work[2].man[1], work[b].man, (work[b].digits * sizeof(WORKDIGIT)));
   memset(&(work[2].man[work[b].digits + 1]), 0, ((workprec - work[b].digits - 1) * sizeof(WORKDIGIT)));

   work[2].exp    = work[b].exp + 1L;
   work[2].sign   = work[b].sign;

   if ((work[a].digits + shift + 1) > (work[b].digits + 1))
      work[2].digits = work[a].digits + (int)shift + 1;
   else
      work[2].digits = work[b].digits + 1;

   ax = &work[a].man[0];
   ar = &work[a].man[work[a].digits - 1];
   cx = &work[2].man[shift + 1];


   /*
    *    **************************************************
    *    *                                                *
    *    *  ADDITION: By Shifted Column Add & Carry       *
    *    *                                                *
    *    **************************************************
    */

   do {
      if ((*(cx++) += *(ax++)) > 9) {
         ct = cx - 1;                  /* Carry */
         do {
            *ct -= 10;
            } while (++(*(--ct)) > 9);
         }

      } while (ax <= ar);

   return(Normalize(2));

}




/*
 *    **************************************************
 *    *                                                *
 *    *               Extended Subtract                *
 *    *                                                *
 *    *         work[2] = work[1] - work[0]            *
 *    *                                                *
 *    **************************************************
 */

extern int ExtendedSubtract(void)

{

   int a, b;                       /* operand indexes               */
   int digits, result;             /* work variables                */
   long shift;                     /* digit allignment shift        */
   WORKDIGIT *al, *ax, *cx, *ct;   /* aleft, aindex, cindex, ctempx */


         /* Special Cases */

   if (! work[0].digits) {             /* work[0] zero, return work[1] */
      MoveWorkWork(1, 2);
      return(TRUE);
      }

   if (! work[1].digits) {             /* work[1] zero, return -work[0] */
      MoveWorkWork(0, 2);
      work[2].sign = FlipSign(work[2].sign);
      return(TRUE);
      }

   if (work[0].sign != work[1].sign) {       /* If signs different, */
      work[0].sign = FlipSign(work[0].sign); /*  reverse sign       */
      return(ExtendedAdd());                 /*  and add            */
      }

   if (work[0].exp < work[1].exp) {    /* compare work[0] : work[1] */

      a = 0;                           /* magnitude work[0] < work[1] */
      b = 1;
      }

   else if (work[0].exp > work[1].exp) {

      a = 1;                           /* Magnitude work[1] > work[0] */
      b = 0;
      work[b].sign = FlipSign(work[b].sign);
      }

   else {                                    /* Can't tell by exponents, */
                                             /*  so compare mantissae    */
      if (work[0].digits >= work[1].digits)
         digits = work[0].digits;
      else
         digits = work[1].digits;

      result = memcmp(work[0].man, work[1].man, (digits * sizeof(WORKDIGIT)));

      if (result < 0) {
         a = 0;                        /* work[0] < work[1] */
         b = 1;
         }

      else if (result > 0) {
         a = 1;                        /* work[0] > work[1] */
         b = 0;
         work[b].sign = FlipSign(work[b].sign);
         }

      else {
         ClearWork(2);                 /* work[0] == work[1], return 0 */
         return(TRUE);
         }

      }  /* End compare */

   MoveWorkWork(b, 2);                 /* Put larger number in work[2] */
                                       /*  so borrow always successful */
   shift = work[b].exp - work[a].exp;
   if (shift > (long)compprec) {       /* a operand is insignificant   */
      return(TRUE);                    /*  return larger nbr as answer */
      }

   if ((work[a].digits + (int)shift) > work[b].digits)
      work[2].digits = work[a].digits + (int)shift;
   else
      work[2].digits = work[b].digits;

   al = &work[a].man[0];
   ax = &work[a].man[work[a].digits - 1];
   cx = &work[2].man[(int)shift + work[a].digits - 1];


   /*
    *    **************************************************
    *    *                                                *
    *    *  SUBTRACTION: By Shifted Column Sub & Borrow   *
    *    *                                                *
    *    **************************************************
    */

   do {
      if ((*(cx--) -= *(ax--)) < 0) {
         ct = cx;                         /* Borrow */
         do {
            *(ct + 1) += 10;              /* Borrow always successful */
            } while (--(*(ct--)) < 0);    /*  because a < c           */
         }

      } while (ax >= al);

   return(Normalize(2));

}




/*
 *    **************************************************
 *    *                                                *
 *    *               Extended Multiply                *
 *    *                                                *
 *    *         work[2] = work[1] * work[0]            *
 *    *                                                *
 *    **************************************************
 */

extern int ExtendedMultiply(void)

{

   WORKDIGIT *ax, *bx, *cx;        /* indexes          */
   WORKDIGIT *al, *bl, *cl;        /* left pointers    */
   WORKDIGIT *ar, *br, *cr;        /* right pointers   */
   WORKDIGIT ad;                   /* a digit          */
   long exponent;                  /* work exponent    */


         /* Special Cases */

   ClearWork(2);

   if ( (! work[0].digits)             /* If either is zero, */
         ||
        (! work[1].digits) ) {
      return(TRUE);                    /*  return zero       */
      }

   exponent = work[0].exp + work[1].exp;

   if (exponent < MINEXP + 2L) {       /* If underflow, */
      return(TRUE);                    /*  return zero  */
      }

   if (exponent > (MAXEXP + 2L)) {
      Overflow();
      return(FALSE);
      }

   work[2].exp = exponent;

   if (work[0].sign == work[1].sign)
      work[2].sign = '+';
   else
      work[2].sign = '-';

   if (work[0].digits <= work[1].digits) {
      work[0].digits = (work[0].digits + 2) / 3;   /* Round up to mod 3  */
      work[0].digits *= 3;
      al = &work[0].man[0];                        /* Multiplier left    */
      ar = &work[0].man[work[0].digits - 1];       /* Multiplier right   */
      bl = &work[1].man[0];                        /* Multiplicand left  */
      br = &work[1].man[work[1].digits - 1];       /* Multiplicand right */
      }
   else {
      work[1].digits = (work[1].digits + 2) / 3;   /* Round up to mod 3  */
      work[1].digits *= 3;
      al = &work[1].man[0];                        /* Multiplier left    */
      ar = &work[1].man[work[1].digits - 1];       /* Multiplier right   */
      bl = &work[0].man[0];                        /* Multiplicand left  */
      br = &work[0].man[work[0].digits - 1];       /* Multiplicand right */
      }

   work[2].digits = work[0].digits + work[1].digits;
   cl = &work[2].man[0];                           /* Product left  */
   cr = &work[2].man[work[2].digits - 1];          /* Product right */


   /*
    *    **************************************************
    *    *                                                *
    *    *  MULTIPLY: By Repeated 3 Group Multiplication  *
    *    *                                                *
    *    *     Where: a = multiplier in groups of 3       *
    *    *            b = multiplicand                    *
    *    *            c = product                         *
    *    *                                                *
    *    *     bbbbbbbbb     bbbbbbbbb     bbbbbbbbb      *
    *    *                                                *
    *    *           ---        ---        ---            *
    *    *   x aaaaaaaaa   x aaaaaaaaa   x aaaaaaaaa      *
    *    *   -----------   -----------   -----------      *
    *    *     ccccccccc     ccccccccc     ccccccccc      *
    *    *                ddddddddd     ddddddddd         *
    *    *                           eeeeeeeee            *
    *    *                                                *
    *    **************************************************
    */

   ax = ar;                                  /* Multiplier (ax) loop */
   do {
      cx = cr;
      cr -= 3;                                  /* Back up each cycle */

                                                /* Multiplier digits */
      ad = *(ax - 2) * 100 + *(ax - 1) * 10 + *ax;

      bx = br;                                  /* Multiplicand (bx) loop */
      do {
         if ((*(cx--) += (*bx * ad)) >= 10000) {   /* Partial carry to      */
            *cx += 1000;                           /*  prevent digit overflow */
            *(cx + 1) -= 10000;
            }
         } while (--bx >= bl);                  /* Multiplicand loop end */

      if (KeyPressed() == ESCAPE) {             /* User abort */
         return(FALSE);
         }

      } while ((ax -= 3) >= al);                /* Multiplier loop end */


   cx = &work[2].man[work[2].digits];        /* Satisfy all carrys */
   while (--cx > cl) {
      if (*cx > 9) {
         *(cx - 1) += *cx / 10;
         *cx %= 10;
         }
      }

   return(Normalize(2));

}




/*
 *    **************************************************
 *    *                                                *
 *    *                Extended Divide                 *
 *    *                                                *
 *    *         work[2] = work[1] / work[0]            *
 *    *                                                *
 *    **************************************************
 */

extern int ExtendedDivide(void)

{

   WORKDIGIT *ax, *bx, *cx;        /* Indexes               */
   WORKDIGIT *al, *bl;             /* left pointers         */
   WORKDIGIT *ar,      *cr;        /* right pointers        */
   WORKDIGIT *bb, *bt;             /* borrow, temp pointers */
   WORKDIGIT factor, val1, val2;   /* subtract factor       */
   BOOLEAN underflow;              /* underflow flag        */
   long exponent;                  /* long prevents oflow   */


         /* Special Cases */

   if (! work[0].digits) {             /* Divide by zero: error */
      DivideByZero();
      return(FALSE);
      }

   ClearWork(2);

   if (! work[1].digits) {             /* Divide into zero: result zero */
      return(TRUE);
      }

   exponent = work[1].exp - work[0].exp + 1L;

   if (exponent < MINEXP + 1L) {       /* If underflow, */
      return(TRUE);                    /*  return zero  */
      }

   if (exponent > (MAXEXP + 2L)) {
      Overflow();
      return(FALSE);
      }

   work[2].exp = exponent;

   if (work[0].sign == work[1].sign)
      work[2].sign = '+';
   else
      work[2].sign = '-';

   work[2].digits = compprec + 2;

   al = &work[0].man[0];                  /* Divisor left pointer   */
   bl = &work[1].man[0];                  /* Dividend left pointer  */

   bb = &work[1].man[0];                  /* Leftmost limit of borrow */

   ar = &work[0].man[work[0].digits - 1]; /* Divisor right pointer  */
   cr = &work[2].man[compprec + 1];       /* Quotient right pointer */

   cx = &work[2].man[0];                  /* Quotient index pointer */

   work[1].man[workprec - 1] = 1;   /* Insignificant digit simplifies scan */
                                    /*  for leftmost digit of dividend     */


   /*
    *    **************************************************
    *    *                                                *
    *    *  DIVIDE:  By Repeated Weighted Subtraction     *
    *    *                                                *
    *    *     Where: a = divisor * factor                *
    *    *            b = dividend                        *
    *    *            f = factor                          *
    *    *                                                *
    *    *     bbbbbbbb      0bbbbbbb      00bbbbbb       *
    *    *   - aaaaaa       - aaaaaa       - aaaaaa       *
    *    *          |              |              |       *
    *    *          v              v              v       *
    *    *          f             ff            fff       *
    *    *                                                *
    *    **************************************************
    */


   do {                    /* Divide by repeated subtraction */
      ax = al;
      bx = bl;
                              /* Calculate subtract factor */
      val1 = *bx * 100 + *(bx + 1) * 10 + *(bx + 2);
      if (bb < bx)
         val1 += *bb * 1000;
      val2 = *ax * 100 + *(ax + 1) * 10 + *(ax + 2);
      factor = val1 / val2;

      if (factor)
         do {                                /* Subtraction Cycle */
            underflow = FALSE;
            do {
               if ((*bx -= (*ax * factor)) < 0) {
                  bt = bx;

                  do {                             /* Begin borrow */
                     if (bt > bb) {
                        *(bt - 1) -= (int)((9 - *bt) / 10);
                        *bt += ((int)((9 - *bt) / 10)) * 10 ;
                        }
                     else {                           /* Borrow failed */

                        while (ax >= al) {               /* Undo column subs */
                           if ((*bx += (*ax * factor)) > 9) {
                              bt = bx;                      /* Undo prev borrow */
                              do {
                                 *(bt - 1) += *bt / 10;
                                 *bt %= 10;
                                 } while (*(--bt) > 9);
                              }                             /* Undo prev borrow end */
                           ax--;
                           bx--;
                           }                             /* Undo column subs end */
                        underflow = TRUE;
                        break;
                        }                             /* Borrow failed end */
                     } while (*(--bt) < 0);
                  }                                /* Borrow end */
               ax++;
               bx++;
               } while ((! underflow) && (ax <= ar));

            factor -= underflow;                /* Subtracts if TRUE (== 1) */

            } while (underflow && factor);   /* Subtract cycle end */

      *cx = factor;           /* Result of subtraction */

      while (! *bb)           /* Find leftmost non zero is      */
         bb++;                /*  always successful (see above) */

      if (bb > bl) {          /* If 1st non zero is right of current shift */
         cx += (bb - bl);     /*  then shift right to it,                  */
         bl = bb;
         }
      else {                  /*  else */
         cx++;                /*  shift one column to the right */
         bl++;
         }

      if (KeyPressed() == ESCAPE) { /* User abort */
         return(FALSE);
         }

      } while (cx <= cr);  /* Divide loop end */


   return(Normalize(2));

}




/*
 *    **************************************************
 *    *                                                *
 *    *             Extended Square Root               *
 *    *                                                *
 *    *            work[2] = Sqrt(work[0])             *
 *    *                                                *
 *    *              (Uses 2 temp areas)               *
 *    *                                                *
 *    **************************************************
 */

extern int ExtendedSquareRoot(void)

{

   long nl, apxl, lapxl;
   int i, j, result;
   int oldprec, cmpsize;
   COMPTYPE *temp;
   COMPTYPE *arg, *apx;


         /* Special Cases */

   if (work[0].sign == '-') {                /* Negative: error */
      NegativeArgument();
      return(FALSE);
      }

   if (! work[0].digits) {                   /* Zero: return zero */
      ClearWork(2);
      return(TRUE);
      }


         /* Set up temp registers */

   if ((temp = GETCOMPTEMP(2)) == NULL) {
      MemoryError();
      return(FALSE);
      }
   arg = temp;
   apx = temp + 1;

   MoveWorkTemp(0, arg);                     /* Argument -> *arg */


         /* Prepare first approximation */

   nl = 0L;                                  /* Extract 1st digits of N */
   j = 8 + (int)(arg->exp & 0x01L);  /* 9 if odd, 8 if even */
   for (i = 0; i < j; i++) {
      nl = nl * 10L + (long)arg->man[i];
      }

   if (j == 8L)                              /* Compute sqrt of digits */
      apxl = 7071L;
   else
      apxl = 22360L;

   for (i = 1; i <= 6; i++) {
      lapxl = apxl;
      apxl = ((lapxl + (nl / lapxl)) >> 1);
      }
   SetWorkInteger(2, apxl);                  /* Put 1st appx in work[2] */

   if (arg->exp > 0L)                        /* Calc 1st approx exponent */
      work[2].exp = (arg->exp + 1L) / 2L;
   else
      work[2].exp = arg->exp / 2L;
   work[2].sign = '+';


   /*
    *    **************************************************
    *    *                                                *
    *    *  SQUARE ROOT:  Using Newton's Approximation    *
    *    *                                                *
    *    *     apx    = ( apx  + ( X / apx ) ) / 2        *
    *    *        i+1        i            i               *
    *    *                                                *
    *    *                                                *
    *    *  *arg = X                                      *
    *    *                                                *
    *    *  *apx = apx                                    *
    *    *            i                                   *
    *    *                                                *
    *    **************************************************
    */

   oldprec = normprec;                 /* Use progressive precision */
   if (normprec > 8) {
      normprec = 8;
      compprec = COMPPREC;
      workprec = WORKPREC;
      }
   cmpsize = compprec * sizeof(WORKDIGIT);


   do {

      MoveWorkTemp(2, apx);               /* Save prev appx */

      MoveTempWork(arg, 1);               /* Divide arg */

      MoveWorkWork(2, 0);                 /*  by prev appx */

      if (! ExtendedDivide()) {
         result = FALSE;
         break;
         }


      MoveWorkWork(2, 1);                 /* Add result to prev appx */
                                          /*  still in work[0]       */
      if (! ExtendedAdd()) {
         result = FALSE;
         break;
         }


      MoveWorkWork(2, 1);                 /* Multiply result */

      ClearWork(2);                       /*  by .5 */
      work[2].exp    = work[1].exp;
      work[2].sign   = work[1].sign;
      work[2].digits = work[1].digits + 1;
      for (i = work[1].digits - 1; i >= 0; i--) {
         if ((work[2].man[i+1] += work[1].man[i] * 5) > 9) {
            work[2].man[i] = work[2].man[i+1] / 10;
            work[2].man[i+1] %= 10;
            }
         }
      if ( (! work[2].man[0])
            ||
           (work[2].digits > compprec) ) {
         Normalize(2);
         }

                                          /* Test for completion */
      if (compprec >= arg->digits) {
         if (! memcmp(work[2].man, apx->man, cmpsize) ) {
            result = TRUE;
            break;
            }
         }

      if (normprec < oldprec) {           /* Increase precision */
         normprec *= 2;
         if (normprec > oldprec)
            normprec = oldprec;
         compprec = COMPPREC;
         workprec = WORKPREC;
         cmpsize = compprec * sizeof(WORKDIGIT);
         }

      } while (TRUE);               /* Loop until precision gained */


   if (normprec < oldprec) {        /* Reset precision if needed */
      normprec = oldprec;
      compprec = COMPPREC;
      workprec = WORKPREC;
      }

   free(temp);

   return(result);

}




/*
 *    **************************************************
 *    *                                                *
 *    *                Extended Power                  *
 *    *                                                *
 *    *          work[2] = work[1] ^ work[0]           *
 *    *                                                *
 *    *              (Uses 1 temp area)                *
 *    *                                                *
 *    **************************************************
 */

extern int ExtendedPower(void)

{

   int i;
   unsigned long pow;
   COMPTYPE *temp, *x, *y;
   BOOLEAN recip = FALSE;


         /* Special Cases */

   if (! work[1].digits) {                /* Y zero: return 0 */
      ClearWork(2);
      return(TRUE);
      }

   if (! work[0].digits) {                /* X zero: return 1 */
      SetWorkInteger(2, 1L);
      return(TRUE);
      }

   if (work[0].exp > MAXEXDIGITS) {          /* Overflow */
      Overflow();
      return(FALSE);
      }


         /* Loop if X 'small' integer */

   if ( (work[0].exp >= work[0].digits)   /* If X integer */
         &&
        (work[0].exp <= MAXEXDIGITS) ) {  /*  and 'small' */


            /* Set up temp registers */

      if ((temp = GETCOMPTEMP(1)) == NULL) {
         MemoryError();
         return(FALSE);
         }
      y = temp;
      MoveWorkTemp(1, y);                 /* Save Y */

      if (work[0].sign == '-')            /* If X neg, use reciprocal */
         recip = TRUE;


      /*
       *    **************************************************
       *    *                                                *
       *    *  POWER: Y^X Using Square-Multiply algorithm:   *
       *    *                                                *
       *    *                                                *
       *    *     Shift X left 1 bit                         *
       *    *                                                *
       *    *     Set rightmost bit of X to 1 (flag bit)     *
       *    *                                                *
       *    *     Shift X left until high order bit is 1     *
       *    *                                                *
       *    *     Shift X left again                         *
       *    *                                                *
       *    *     Set TEMP = Y                               *
       *    *                                                *
       *    *     While X <> (high order bit 1, rest 0)      *
       *    *                                                *
       *    *        Square TEMP                             *
       *    *                                                *
       *    *        If high order bit of X = 1              *
       *    *                                                *
       *    *           Multiply Y times TEMP                *
       *    *                                                *
       *    *        Shift X left 1 bit                      *
       *    *                                                *
       *    *     Result in TEMP is Y^X                      *
       *    *                                                *
       *    *                                                *
       *    *     pow = X                                    *
       *    *                                                *
       *    **************************************************
       */

      pow = 0;                            /* X -> pow */
      for (i = 0; i < (int)work[0].exp; i++) {
         pow = pow * 10 + work[0].man[i];
         }
      pow = (pow << 1) + 1;               /* Add flag bit & left justify */
      while (pow < (unsigned long)0x80000000L) {
         pow <<= 1;
         }
      pow <<= 1;

      MoveWorkWork(1, 2);                 /* Y -> work[2] (TEMP) */


      while (pow != (unsigned long)0x80000000L) {

         MoveWorkWork(2, 0);              /* Square TEMP */
         MoveWorkWork(2, 1);
         if (! ExtendedMultiply()) {
            free(temp);
            return(FALSE);
            }

         if (! work[2].digits) {          /* Test for underflow */
            free(temp);
            if (recip)
               return(FALSE);             /* Divide by zero */
            else
               return(TRUE);
            }

         if (pow & (unsigned long)0x80000000L) {

            MoveTempWork(y, 0);              /* Multiply by Y */
            MoveWorkWork(2, 1);
            if (! ExtendedMultiply()) {
               free(temp);
               return(FALSE);
               }
            if (! work[2].digits) {          /* Test for underflow */
               free(temp);
               if (recip)
                  return(FALSE);             /* Divide by zero */
               else
                  return(TRUE);
               }
            }

         pow <<= 1;                       /* Shift pow left 1 bit */

         }

      free(temp);

      if (recip) {                        /* X neg: use reciprocal */
         MoveWorkWork(2, 0);
         SetWorkInteger(1, 1L);
         if (! ExtendedDivide()) {
            return(FALSE);
            }
         }

      return(TRUE);

      }  /* if */


   /*
    *    **************************************************
    *    *                                                *
    *    *  POWER: Using logarithms:                      *
    *    *                                                *
    *    *     Y^X = e^(ln(Y^X))                          *
    *    *                                                *
    *    *         = e^(X * ln(Y))                        *
    *    *                                                *
    *    *                                                *
    *    *     *x = X                                     *
    *    *                                                *
    *    **************************************************
    */


         /* Set up temp registers */

   if ((temp = GETCOMPTEMP(1)) == NULL) {
      MemoryError();
      return(FALSE);
      }
   x = temp;


         /* Save X */

   MoveWorkTemp(0, x);


         /* Compute ln(Y) */

   MoveWorkWork(1, 0);              /* Get Y */
   if (! ExtendedLn()) {
      free(temp);
      return(FALSE);
      }

         /* Compute X * ln(Y) */

   MoveWorkWork(2, 1);              /* Get ln(Y) */
   MoveTempWork(x, 0);              /* Get X */
   if (! ExtendedMultiply()) {
      free(temp);
      return(FALSE);
      }

   free(temp);

         /* Compute exp(X * ln(Y) */

   MoveWorkWork(2, 0);              /* Get X * ln(Y) */
   if (! ExtendedExpE()) {
      return(FALSE);
      }

   return(TRUE);

}




/*
 *    **************************************************
 *    *                                                *
 *    *                  Sine/Cosine                   *
 *    *                                                *
 *    *             work[2] = Sin(work[0])             *
 *    *                 if scflag = 0                  *
 *    *                                                *
 *    *             work[2] = Cos(work[0])             *
 *    *                 if scflag = 1                  *
 *    *                                                *
 *    *              (Uses 2 temp areas)               *
 *    *                                                *
 *    **************************************************
 */

extern int ExtendedSinCos(int scflag)

{

   int i;
   long n, count = 0;
   int sign = '+';
   COMPTYPE *temp;
   COMPTYPE *x, *xsq, *sum;
   BOOLEAN add = FALSE;


         /* Special Cases */

   if (! work[0].digits) {                /* X zero */
      if (scflag)
         SetWorkInteger(2, 1L);           /*  cos(0) = 1 */
      else
         ClearWork(2);                    /*  sin(0) = 0 */
      return(TRUE);
      }

   if (work[0].exp > MAXEXDIGITS) {       /* Overflow */
      Overflow();
      return(FALSE);
      }


         /* Make Argument Positive */

   if (work[0].sign == '-') {             /* X negative */
      work[0].sign = '+';                 /*  cos(X) = cos(-X) */
      if (! scflag)
         sign = FlipSign(sign);           /*  sin(X) = -sin(-X) */
      }


         /* Set up temp registers */

   if ((temp = GETCOMPTEMP(2)) == NULL) {
      MemoryError();
      return(FALSE);
      }
   x = temp;                              /* x and xsq never used together */
   xsq  = temp;
   sum  = temp + 1;


   /*
    *    **************************************************
    *    *                                                *
    *    *  Scale: 0 < X < pi/4  like this:               *
    *    *                                                *
    *    *                                                *
    *    *     X = X - (pi/2 * int(X / pi/2))             *
    *    *                                                *
    *    *     if (X > pi/4)                              *
    *    *                                                *
    *    *        X = X - pi/2                            *
    *    *                                                *
    *    *                                                *
    *    *     *x = X                                     *
    *    *                                                *
    *    **************************************************
    */


   if (work[0].exp > 1) {              /* Check magnitude of X */

      MoveWorkTemp(0, x);              /* Save X */

      MoveWorkWork(0, 1);              /* Calculate int(X/(pi/2)) */
      ExtendedRecallHalfPi(0);
      if (! ExtendedDivide()) {
         free(temp);
         return(FALSE);
         }
      MoveWorkWork(2, 0);
      if (! ExtendedIntegerPart()) {
         free(temp);
         return(FALSE);
         }
      count = 0;                       /* int(X/(pi/2)) -> count */
      for (i = 0; i < (int)work[0].exp; i++) {
         count = count * 10 + work[0].man[i];
         }

      ExtendedRecallHalfPi(1);         /* Calculate pi/2 * int(X/(pi/2)) */
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }

      MoveWorkWork(2, 0);              /* Calculate X - ((pi/2) * int(X/(pi/2))) */
      MoveTempWork(x, 1);
      if (! ExtendedSubtract()) {
         free(temp);
         return(FALSE);
         }

      MoveWorkWork(2, 0);              /* 0 < X < pi/2 */

      }  /* end pi/2 scale */


   i = work[0].man[0] * 1000 +         /* Get 1st 4 digits of X */
       work[0].man[1] * 100 +
       work[0].man[2] * 10 +
       work[0].man[3];

   while ( (work[0].exp > 0)           /* Check magnitude of X */
           ||
           ( (work[0].exp == 0)
              &&
             (i > 7853) ) ) {

      MoveWorkWork(0, 1);              /* Calculate X - pi/2 */
      ExtendedRecallHalfPi(0);
      if (! ExtendedSubtract()) {
         free(temp);
         return(FALSE);
         }

      count++;
      MoveWorkWork(2, 0);              /* 0 < X < pi/4 */

      i = work[0].man[0] * 1000 +      /* Get 1st 4 digits of X */
          work[0].man[1] * 100 +
          work[0].man[2] * 10 +
          work[0].man[3];

      }  /* end pi/4 scale */



   /*
    *    **************************************************
    *    *                                                *
    *    *  SIN(X): Using the power series:               *
    *    *                                                *
    *    *                                                *
    *    *                  X^3   X^5   X^7       X^n     *
    *    *     sin(X) = X - --- + --- - --- + ... ---     *
    *    *                   3!    5!    7!        n!     *
    *    *                                                *
    *    *  COS(X): Using the power series:               *
    *    *                                                *
    *    *                                                *
    *    *                  X^2   X^4   X^6       X^n     *
    *    *     cos(X) = 1 - --- + --- - --- + ... ---     *
    *    *                   2!    4!    6!        n!     *
    *    *                                                *
    *    *                                                *
    *    *     *xsq = X^2                                 *
    *    *                                                *
    *    *     *sum = Sum of series                       *
    *    *                                                *
    *    *     n    = Term number                         *
    *    *                                                *
    *    **************************************************
    */



   MoveWorkWork(0, 1);              /* Calculate X^2 */
   if (! ExtendedMultiply()) {
      free(temp);
      return(FALSE);
      }
   MoveWorkTemp(2, xsq);

   count += scflag;

   if (count & 2L) {                /* If count & 2: flip sign */
      sign = FlipSign(sign);
      }

   if (count & 1L) {                /* If count & 1: use cos series */
      SetWorkInteger(0, 1L);
      n = 0L;                       /* Even terms    */
      }
   else {
      n = 1L;                       /* Odd terms     */
      }
   MoveWorkTemp(0, sum);            /* X -> sum */


   while (TRUE) {                /* Until term < compprec */

      n += 2;                       /* n = term number */

         /* term *= X */

         /* term still in work[0] */

      MoveTempWork(xsq, 1);         /* Get xsq */
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }


         /* term /= n! */

      SetWorkInteger(0, (n*(n-1))); /* Get n! */
      MoveWorkWork(2, 1);           /* Get term */
      if (! ExtendedDivide()) {
         free(temp);
         return(FALSE);
         }


         /* Add to/subtract from sum */

      MoveWorkWork(2, 0);           /* new term */
      MoveTempWork(sum, 1);         /* Get sum */
      if (add) {
         if (! ExtendedAdd()) {
            free(temp);
            return(FALSE);
            }
         add = FALSE;
         }
      else {
         if (! ExtendedSubtract()) {
            free(temp);
            return(FALSE);
            }
         add = TRUE;
         }


         /* Test for completion */

      if ( (labs(work[2].exp - work[0].exp) >= compprec)
            ||
           (! work[0].digits) ) {
         break;
         }


         /* Save new sum */

      MoveWorkTemp(2, sum);

      }  /* while */

   free(temp);

   if (sign == '-') {
      work[2].sign = FlipSign(work[2].sign);
      }

   return(TRUE);

}




/*
 *    **************************************************
 *    *                                                *
 *    *                   Tangent                      *
 *    *                                                *
 *    *             work[2] = Tan(work[0])             *
 *    *                                                *
 *    **************************************************
 */

extern int ExtendedTan(void)

{

   COMPTYPE *temp;


         /* Special Cases */

   if (! work[0].digits) {                /* X zero: return zero */
      ClearWork(2);
      return(TRUE);
      }


   /*
    *    **************************************************
    *    *                                                *
    *    *  TAN(X) Using these relations:                 *
    *    *                                                *
    *    *     tan(X) = sin(X) / cos(X)                   *
    *    *                                                *
    *    *                                                *
    *    *     *temp = x or sin(X)                        *
    *    *                                                *
    *    **************************************************
    */


         /* Set up temp register */

   if ((temp = GETCOMPTEMP(1)) == NULL) {
      MemoryError();
      return(FALSE);
      }
   temp = temp;

   MoveWorkTemp(0, temp);        /* Save X */

   if (! ExtendedSinCos(0)) {    /* Compute sin(X) */
      free(temp);
      return(FALSE);
      }

   MoveTempWork(temp, 0);        /* Get X */
   MoveWorkTemp(2, temp);        /* Save sin(X) */

   if (! ExtendedSinCos(1)) {    /* Compute cos(X) */
      free(temp);
      return(FALSE);
      }

   free(temp);

   MoveWorkWork(2, 0);           /* Compute sin(X) / cos(X) */
   MoveTempWork(temp, 1);
   if (! ExtendedDivide()) {
      return(FALSE);
      }

   return(TRUE);

}




/*
 *    **************************************************
 *    *                                                *
 *    *                Arc Sine/Cosine                 *
 *    *                                                *
 *    *            work[2] = ArcSin(work[0])           *
 *    *                 if scflag = 0                  *
 *    *                                                *
 *    *            work[2] = ArcCos(work[0])           *
 *    *                 if scflag = 1                  *
 *    *                                                *
 *    *               (uses 1 temp area)               *
 *    *                                                *
 *    **************************************************
 */

extern int ExtendedArcSinCos(int scflag)

{

   int sign = '+';
   COMPTYPE *temp;



         /* Special cases */

   if (work[0].sign == '-') {          /* Convert to positive */
      work[0].sign = '+';
      sign = '-';
      }

   if ( (work[0].exp > 1)                /* Arg > 1: return error */
         ||
        ( (work[0].exp == 1)
           &&
          ( (work[0].man[0] > 1)
             ||
            (work[0].digits > 1) ) ) ) {
      ArgumentInvalid();
      return(FALSE);
      }



/*
 *    **************************************************
 *    *                                                *
 *    *  Calculate arcsin, and convert if arccos       *
 *    *                                                *
 *    **************************************************
 */

   if (! work[0].digits) {             /* Zero: result 0 */
      ClearWork(2);
      }
   else if ( (work[0].exp == 1)        /* One: result = pi/2 */
              &&
             (work[0].man[0] == 1)
              &&
             (work[0].digits == 1) ) {
      ExtendedRecallHalfPi(2);
      }
   else {                              /* Calculate angle */
      if ((temp = GETCOMPTEMP(1)) == NULL) {
         MemoryError();
         return(FALSE);
         }
      MoveWorkTemp(0, temp);           /* Save 'sine' */

      MoveWorkWork(0, 1);              /* Calculate 'cosine' */
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }

      MoveWorkWork(2, 0);
      SetWorkInteger(1, 1L);
      ExtendedSubtract();

      MoveWorkWork(2, 0);
      if (! ExtendedSquareRoot()) {    /* cos = sqrt(1 - sin^2) */
         free(temp);
         return(FALSE);
         }

      MoveTempWork(temp, 0);           /* 'sine' */
      MoveWorkWork(2, 1);              /* 'cosine' */

      free(temp);

      if (! ResolveAngle()) {
         return(FALSE);
         }
      }

   work[2].sign = sign;                /* Correct sign */

   if (scflag) {                       /* Convert to arccos */
      MoveWorkWork(2, 0);
      ExtendedRecallHalfPi(1);
      ExtendedSubtract();
      }

   return(TRUE);

}




/*
 *    **************************************************
 *    *                                                *
 *    *                  ArcTangent                    *
 *    *                                                *
 *    *           work[2] = ArcTan(work[0])            *
 *    *                                                *
 *    **************************************************
 */

extern int ExtendedArcTan(void)

{

   int sign = '+', recip = 0;
   COMPTYPE *temp;


/*
 *    **************************************************
 *    *                                                *
 *    *  Convert tan to sin and arcsin, then resolve   *
 *    *                                                *
 *    **************************************************
 */


         /* Special cases */

   if (work[0].sign == '-') {          /* Convert to positive */
      work[0].sign = '+';
      sign = '-';
      }
                           
   if ( (work[0].exp > 1)              /* Arg > 1: use reciprocal: */
         ||                            /* atan(1/x) = pi/2 - atan(x) */
        ( (work[0].exp == 1)
           &&
          ( (work[0].man[0] > 1)
             ||
            (work[0].digits > 1) ) ) ) {
      recip = 1;
      if (! ExtendedReciprocal())
         return(FALSE);
      MoveWorkWork(2, 0);
      }


         /* Get temp area */

   if ((temp = GETCOMPTEMP(1)) == NULL) {
      MemoryError();
      return(FALSE);
      }
   MoveWorkTemp(0, temp);              /* Save x */


         /* Calculate cos = 1 / sqrt(1 + tan^2) */

   MoveWorkWork(0, 1);
   if (! ExtendedMultiply()) {         /* tan^2 */
      free(temp);
      return(FALSE);
      }
   MoveWorkWork(2, 0);
   SetWorkInteger(1, 1L);
   ExtendedAdd();                      /* 1 + tan^2 */
   MoveWorkWork(2, 0);
   if (! ExtendedSquareRoot()) {       /* sqrt(1 + tan^2) */
      free(temp);
      return(FALSE);
      }
   MoveWorkWork(2, 0);
   SetWorkInteger(1, 1L);
   if (! ExtendedDivide()) {           /* cos = 1 / sqrt(1 + tan^2) */
      free(temp);
      return(FALSE);
      }


         /* Calculate sin = cos * tan */

   MoveWorkWork(2, 1);
   MoveTempWork(temp, 0);
   free(temp);
   if (! ExtendedMultiply())
      return(FALSE);                   /* cos still in work[1] */
   MoveWorkWork(2, 0);                 /* sin -> work[0] */

   if (! ResolveAngle())               /* Resolve angle */
      return(FALSE);

   if (recip) {                        /* Resolve reciprocal: */
      MoveWorkWork(2, 0);              /* atan(1/x) = pi/2 - atan(x) */
      ExtendedRecallHalfPi(1);
      ExtendedSubtract();
      }

   work[2].sign = sign;                /* Correct sign */

   return(TRUE);

}




/*
 *    **************************************************
 *    *                                                *
 *    *                Resolve Angle                   *
 *    *                                                *
 *    *  work[2] = Angle(sin = work[0], cos = work[1]) *
 *    *                                                *
 *    *         with angle in first quadrant           *
 *    *                                                *
 *    *              (uses 4 temp areas)               *
 *    *                                                *
 *    **************************************************
 */

static int ResolveAngle(void)

{

   long dif = 0, n, order;
   COMPTYPE *temp;
   COMPTYPE *sin, *cos, *tsin, *tcos;     /* This group of variables is  */
   COMPTYPE *x, *y, *sum;                 /*  never used with this group */


         /* Set up temp registers */

   if ((temp = GETCOMPTEMP(4)) == NULL) {
      MemoryError();
      return(FALSE);
      }
   sin  = temp;         /* This group used for scaling */
   cos  = temp + 1;
   tsin = temp + 2;
   tcos = temp + 3;

   x    = temp;         /* This group used in Euler series */
   y    = temp + 1;
   sum  = temp + 2;


         /* Save off sin and cos */

   MoveWorkTemp(0, sin);
   MoveWorkTemp(1, cos);


   /*
    *    **************************************************
    *    *                                                *
    *    *  Resolve angle A for which we have             *
    *    *                                                *
    *    *     sin(A) and cos(A):                         *
    *    *                                                *
    *    *                                                *
    *    *  Scale the arguments sin and cos like this:    *
    *    *                                                *
    *    *                                                *
    *    *  Choose angle A such that                      *
    *    *                                                *
    *    *     sin(A) = work[0]  (sin)                    *
    *    *  and                                           *
    *    *     cos(A) = work[1]  (cos)                    *
    *    *                                                *
    *    *                                                *
    *    *  Then use the relations:                       *
    *    *                                                *
    *    *     sin(A - B) = sin(A)cos(B) - cos(A)sin(B)   *
    *    *  and                                           *
    *    *     cos(A - B) = cos(A)cos(B) + sin(A)sin(B)   *
    *    *                                                *
    *    *                                                *
    *    *  Using convenient B (.1, .01 and .001)         *
    *    *                                                *
    *    *     and repeatedly adding each B to DIF,       *
    *    *                                                *
    *    *     the angle (A - DIF) is reduced until       *
    *    *                                                *
    *    *     0 <= sin(A - DIF) <= sin(.001)             *
    *    *                                                *
    *    *                                                *
    *    *  Then the arctangent of sin(A-DIF)/cos(A-DIF)  *
    *    *                                                *
    *    *     is calculated and added to DIF to obtain   *
    *    *                                                *
    *    *     the resolved angle.                        *
    *    *                                                *
    *    *                                                *
    *    *  dif = DIF * 1000                              *
    *    *                                                *
    *    **************************************************
    */


         /* Reduce with B = .1 */

   while (sin->exp > -1) {          /* sin(.1) appx .09 */
      dif += 100L;                  /* Add .1 * 1000    */

      MoveTempWork(sin, 0);         /* Calc sin(A)cos(B) */
      ExtendedRecallCosP1(1);
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }
      MoveWorkTemp(2, tsin);        /* Save it */

      MoveTempWork(cos, 0);         /* Calc cos(A)sin(B) */
      ExtendedRecallSinP1(1);
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }

      MoveWorkWork(2, 0);           /* Calc sin(A)cos(B) - cos(A)sin(B) */
      MoveTempWork(tsin, 1);
      ExtendedSubtract();
      MoveWorkTemp(2, tsin);        /* Save new sin */


      MoveTempWork(cos, 0);         /* Calc cos(A)cos(B) */
      ExtendedRecallCosP1(1);
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }
      MoveWorkTemp(2, tcos);        /* Save it */

      MoveTempWork(sin, 0);         /* Calc sin(A)sin(B) */
      ExtendedRecallSinP1(1);
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }

      MoveWorkWork(2, 0);           /* Calc cos(A)cos(B) + sin(A)sin(B) */
      MoveTempWork(tcos, 1);
      ExtendedAdd();
      MoveWorkTemp(2, cos);         /* Save new cos */
      MoveTempTemp(tsin, sin);      /*  and new sin */
      }


         /* Reduce with B = .01 */

   while (sin->exp > -2) {          /* sin(.1) appx .009 */
      dif += 10L;                   /* Add .01 * 1000    */

      MoveTempWork(sin, 0);         /* Calc sin(A)cos(B) */
      ExtendedRecallCosP01(1);
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }
      MoveWorkTemp(2, tsin);        /* Save it */

      MoveTempWork(cos, 0);         /* Calc cos(A)sin(B) */
      ExtendedRecallSinP01(1);
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }

      MoveWorkWork(2, 0);           /* Calc sin(A)cos(B) - cos(A)sin(B) */
      MoveTempWork(tsin, 1);
      ExtendedSubtract();
      MoveWorkTemp(2, tsin);        /* Save new sin */


      MoveTempWork(cos, 0);         /* Calc cos(A)cos(B) */
      ExtendedRecallCosP01(1);
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }
      MoveWorkTemp(2, tcos);        /* Save it */

      MoveTempWork(sin, 0);         /* Calc sin(A)sin(B) */
      ExtendedRecallSinP01(1);
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }

      MoveWorkWork(2, 0);           /* Calc cos(A)cos(B) + sin(A)sin(B) */
      MoveTempWork(tcos, 1);
      ExtendedAdd();
      MoveWorkTemp(2, cos);         /* Save new cos */
      MoveTempTemp(tsin, sin);      /*  and new sin */
      }


         /* Reduce with B = .001 */

   while (sin->exp > -3) {          /* sin(.1) appx .0009 */
      dif += 1L;                    /* Add .001 * 1000    */

      MoveTempWork(sin, 0);         /* Calc sin(A)cos(B) */
      ExtendedRecallCosP001(1);
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }
      MoveWorkTemp(2, tsin);        /* Save it */

      MoveTempWork(cos, 0);         /* Calc cos(A)sin(B) */
      ExtendedRecallSinP001(1);
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }

      MoveWorkWork(2, 0);           /* Calc sin(A)cos(B) - cos(A)sin(B) */
      MoveTempWork(tsin, 1);
      ExtendedSubtract();
      MoveWorkTemp(2, tsin);        /* Save new sin */


      MoveTempWork(cos, 0);         /* Calc cos(A)cos(B) */
      ExtendedRecallCosP001(1);
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }
      MoveWorkTemp(2, tcos);        /* Save it */

      MoveTempWork(sin, 0);         /* Calc sin(A)sin(B) */
      ExtendedRecallSinP001(1);
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }

      MoveWorkWork(2, 0);           /* Calc cos(A)cos(B) + sin(A)sin(B) */
      MoveTempWork(tcos, 1);
      ExtendedAdd();
      MoveWorkTemp(2, cos);         /* Save new cos */
      MoveTempTemp(tsin, sin);      /*  and new sin */
      }


         /* Calculate tan(A) = sin(A)/cos(A) */

      MoveTempWork(sin, 1);         /* With angle <= .01 */
      MoveTempWork(cos, 0);         /*  cos(A) > 0       */
      if (! ExtendedDivide()) {
         free(temp);
         return(FALSE);
         }
      MoveWorkWork(2, 0);           /* Put in Work[0] */


   /*
    *    *******************************************************
    *    *                                                     *
    *    *  ARCTAN(X): Using Euler's series:                   *
    *    *                                                     *
    *    *                                                     *
    *    *            y        2    2*4      2*4*6             *
    *    *  atan(x) = - * (1 + -y + ---y^2 + -----y^3 + ... )  *
    *    *            x        3    3*5      3*5*7             *
    *    *                                                     *
    *    *                                                     *
    *    *     where = y = x^2 / (1 + x^2)                     *
    *    *                                                     *
    *    *                                                     *
    *    *     *x    = Argument                                *
    *    *                                                     *
    *    *     *y    = x^2 / (1 + x^2)                         *
    *    *                                                     *
    *    *     n     = Term counter                            *
    *    *                                                     *
    *    *******************************************************
    */


   MoveWorkTemp(0, x);        /* Save x */


         /* Calculate y = x^2 / (1 + x^2) */

   MoveWorkWork(0, 1);        /* Calculate x^2 */
   if (! ExtendedMultiply()) {
      free(temp);
      return(FALSE);
      }

   MoveWorkWork(2, 1);        /* Calculate 1 + x^2 */
   SetWorkInteger(0, 1L);
   ExtendedAdd();             /* Doesn't change x^2 in work[1] */

   MoveWorkWork(2, 0);
   if (! ExtendedDivide()) {
      free(temp);
      return(FALSE);
      }
   MoveWorkTemp(2, y);        /* Store y */


   SetTempInteger(sum, 1L);   /* Sum */

   n = 0;                     /* N */

   SetWorkInteger(0, 1L);     /* Term in work[0] */



   while (TRUE) {             /* Until term < compprec */

      n += 2;                       /* n = term number */

         /* term *= n  (term still in work[0]) */

      SetWorkInteger(1, n);         /* Put n in work[1] */
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }


         /* term *= y */

      MoveTempWork(y, 0);           /* Get y */
      MoveWorkWork(2, 1);           /* Get term */
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }


         /* term /= (n + 1) */

      SetWorkInteger(0, (n + 1L));  /* Get n + 1 */
      MoveWorkWork(2, 1);           /* Get term */
      if (! ExtendedDivide()) {
         free(temp);
         return(FALSE);
         }


         /* sum += term */

      MoveWorkWork(2, 0);           /* Get term */
      MoveTempWork(sum, 1);         /* Get sum */
      if (! ExtendedAdd()) {
         free(temp);
         return(FALSE);
         }


         /* Test for completion */

      if ( (labs(work[2].exp - work[0].exp) >= compprec)
            ||
           (! work[0].digits) ) {
         break;
         }


         /* Save new sum */

      MoveWorkTemp(2, sum);

      }  /* while */


         /* Compute angle = sum * y / x */

   MoveWorkWork(2, 0);        /* Get sum */
   MoveTempWork(y, 1);
   if (! ExtendedMultiply()) {
      free(temp);
      return(FALSE);
      }
   MoveWorkWork(2, 1);
   MoveTempWork(x, 0);
   if (! ExtendedDivide()) {
      free(temp);
      return(FALSE);
      }

   free(temp);

         /* Resolve dif scaling */

   MoveWorkWork(2, 0);        /* Get angle */

   work[1].exp  = 1L;         /* Put dif in work[1] */
   work[1].sign = '+';
   work[1].digits = 0;
   order = 1000L;
   while (order > dif) {
      order /= 10;
      work[1].exp--;
      }
   while (order) {
      work[1].man[work[1].digits] = (WORKDIGIT) (dif / order);
      dif %= order;
      order /= 10L;
      work[1].digits++;
      }
   memset(&work[1].man[work[1].digits], 0, 
               ((workprec - work[1].digits) * sizeof(WORKDIGIT)));

   return(ExtendedAdd());

}




/*
 *    **************************************************
 *    *                                                *
 *    *           Extended Common Logarithm            *
 *    *                                                *
 *    *             work[2] = Log(work[0])             *
 *    *                                                *
 *    **************************************************
 */

extern int ExtendedLog(void)

{


   /*
    *    **************************************************
    *    *                                                *
    *    *  LOG  (X): Using this relation:                *
    *    *     10                                         *
    *    *                                                *
    *    *              lnX                               *
    *    *     log  X = ---                               *
    *    *        10    ln10                              *
    *    *                                                *
    *    **************************************************
    */

   if (! ExtendedLn()) {         /* Get lnX */
      return(FALSE);
      }
   MoveWorkWork(2, 1);

   ExtendedRecallLn10(0);        /* Get ln10 */

   if (! ExtendedDivide()) {
      return(FALSE);
      }

   return(TRUE);


}




/*
 *    **************************************************
 *    *                                                *
 *    *          Extended Common Antilogarithm         *
 *    *                                                *
 *    *              work[2] = 10^(work[0])            *
 *    *                                                *
 *    **************************************************
 */

extern int ExtendedExp10(void)

{

   int i;
   long pow = 0L;


         /* Special Cases */

   if (! work[0].digits) {                /* Arg zero: return one */
      SetWorkInteger(2, 1L);
      return(TRUE);
      }

   if (! work[0].exp > MAXEXDIGITS) {
      if (! work[0].sign == '-') {        /* Underflow: return zero */
         ClearWork(2);
         return(TRUE);
         }
      else {
         Overflow();                      /* Overflow */
         return(FALSE);
         }
      }



         /* If X integer: X -> exp */

   if (work[0].exp >= work[0].digits) {

      pow = 0;                            /* X -> pow */
      for (i = 0; i < (int)work[0].exp; i++) {
         pow = pow * 10 + work[0].man[i];
         }
      if (work[0].sign == '-') {
         pow = - pow;
         }
      pow++;

      work[2].exp = pow;
      work[2].sign = '+';
      work[2].digits = 1;
      work[2].man[0] = 1;

      return(TRUE);

      }  /* if */


   /*
    *    **************************************************
    *    *                                                *
    *    *  ALOG: 10^X using this:                        *
    *    *                                                *
    *    *     10^X = e^(X*ln10)                          *
    *    *                                                *
    *    **************************************************
    */


         /* Compute X * ln10 */

   ExtendedRecallLn10(1);           /* Get ln10 */
   if (! ExtendedMultiply()) {
      return(FALSE);
      }

         /* Compute e^(X * ln10) */

   MoveWorkWork(2, 0);              /* Get X * ln10 */
   if (! ExtendedExpE()) {
      return(FALSE);
      }

   return(TRUE);

}




/*
 *    **************************************************
 *    *                                                *
 *    *          Extended Natural Logarithm            *
 *    *                                                *
 *    *             work[2] = ln(work[0])              *
 *    *                                                *
 *    *              (Uses 3 temp areas)               *
 *    *                                                *
 *    **************************************************
 */

extern int ExtendedLn(void)

{

   int mulp9 = 0, mulp99 = 0, mulp999 = 0, mulp9999 = 0, mulp99999 = 0;
   int dig, bor;
   long den, pow10 = 0;
   WORKDIGIT *pl, *pr;
   COMPTYPE *temp;
   COMPTYPE *ln, *ysq, *num;


         /* Special Cases */

   if (! work[0].digits) {          /* Zero: error */
      ZeroArgument();
      return(FALSE);
      }

   if (work[0].sign == '-') {       /* Negative: error */
      NegativeArgument();
      return(FALSE);
      }

   if ( (work[0].digits == 1)       /* One: return zero */
         &&
        (work[0].exp == 1L)
         &&
        (work[0].man[0] == 1) ) {
      ClearWork(2);
      return(TRUE);
      }


   /*
    *    **************************************************
    *    *                                                *
    *    *  Scale: 1 <= X < 10  like this:                *
    *    *                                                *
    *    *     M = Mantissa, EX = Exponent                *
    *    *                                                *
    *    *     ln(X) = ln(M * 10^EX)                      *
    *    *                                                *
    *    *           = ln(M) + ln(10^EX)                  *
    *    *                                                *
    *    *           = ln(M) + EX * ln(10)                *
    *    *                                                *
    *    *                                                *
    *    *     pow10 = EX - 1  (so 1 <= M < 10)           *
    *    *                                                *
    *    **************************************************
    */

   if (work[0].exp != 1L) {
      pow10 = work[0].exp - 1;
      work[0].exp = 1;
      }


   /*
    *    **************************************************
    *    *                                                *
    *    *  Scale: 1 <= X <= 1.001 like this:             *
    *    *                                                *
    *    *     ln(X) = ln(X*F/F)                          *
    *    *                                                *
    *    *           = ln(X*F) - ln(F)                    *
    *    *                                                *
    *    *                                                *
    *    *     mulpF = nbr times X*F for X <= 1.00001     *
    *    *                                                *
    *    **************************************************
    */



   /*
    *    **************************************************
    *    *                                                *
    *    *  Calculate X * .9, .99, .999, etc. like this:  *
    *    *                                                *
    *    *     X * .9 = X - X/10                          *
    *    *                                                *
    *    *     X * .99 = X - X/100                        *
    *    *                                                *
    *    *     X * .999 = X - X/1000                      *
    *    *                                                *
    *    *     X * .9999 = X - X/10000                    *
    *    *                                                *
    *    *     X * .99999 = X - X/100000                  *
    *    *                                                *
    *    *                                                *
    *    *  The subtractions are done in place            *
    *    *                                                *
    *    **************************************************
    */

            /* F = .9 */

   while ( (work[0].man[0] > 1)        /* While X > 1.1 */
            ||
           (work[0].man[1] > 1) ) {
      pl = &work[0].man[0];
      pr = &work[0].man[work[0].digits];
      if (work[0].digits < compprec)
         work[0].digits += 1;
      dig = 0;
      bor = 0;
      while (--pr >= pl) {
         dig = *(pr + 1) - *pr - bor;
         if (dig < 0) {
            bor = 1;
            dig += 10;
            }
         else {
            bor = 0;
            }
         *(pr + 1) = dig;
         }
      work[0].man[0] -= bor;
      mulp9++;
      }


            /* F = .99 */

   while ( (work[0].man[1] > 0)        /* While X > 1.01 */
            ||
           (work[0].man[2] > 1) ) {
      pl = &work[0].man[0];
      pr = &work[0].man[work[0].digits];
      if (work[0].digits < compprec)
         work[0].digits += 2;
      dig = 0;
      bor = 0;
      while (--pr >= pl) {
         dig = *(pr + 2) - *pr - bor;
         if (dig < 0) {
            bor = 1;
            dig += 10;
            }
         else {
            bor = 0;
            }
         *(pr + 2) = dig;
         }
      work[0].man[1] -= bor;
      mulp99++;
      }


            /* F = .999 */

   while ( (work[0].man[2] > 0)        /* While X > 1.001 */
            ||
           (work[0].man[3] > 1) ) {
      pl = &work[0].man[0];
      pr = &work[0].man[work[0].digits];
      if (work[0].digits < compprec)
         work[0].digits += 3;
      dig = 0;
      bor = 0;
      while (--pr >= pl) {
         dig = *(pr + 3) - *pr - bor;
         if (dig < 0) {
            bor = 1;
            dig += 10;
            }
         else {
            bor = 0;
            }
         *(pr + 3) = dig;
         }
      work[0].man[2] -= bor;
      mulp999++;
      }


            /* F = .9999 */

   while ( (work[0].man[3] > 0)        /* While X > 1.0001 */
            ||
           (work[0].man[4] > 1) ) {
      pl = &work[0].man[0];
      pr = &work[0].man[work[0].digits];
      if (work[0].digits < compprec)
         work[0].digits += 4;
      dig = 0;
      bor = 0;
      while (--pr >= pl) {
         dig = *(pr + 4) - *pr - bor;
         if (dig < 0) {
            bor = 1;
            dig += 10;
            }
         else {
            bor = 0;
            }
         *(pr + 4) = dig;
         }
      work[0].man[3] -= bor;
      mulp9999++;
      }


            /* F = .99999 */

   while ( (work[0].man[4] > 0)        /* While X > 1.00001 */
            ||
           (work[0].man[5] > 1) ) {
      pl = &work[0].man[0];
      pr = &work[0].man[work[0].digits];
      if (work[0].digits < compprec)
         work[0].digits += 5;
      dig = 0;
      bor = 0;
      while (--pr >= pl) {
         dig = *(pr + 5) - *pr - bor;
         if (dig < 0) {
            bor = 1;
            dig += 10;
            }
         else {
            bor = 0;
            }
         *(pr + 5) = dig;
         }
      work[0].man[4] -= bor;
      mulp99999++;
      }


         /* Set up temp registers */

   if ((temp = GETCOMPTEMP(3)) == NULL) {
      MemoryError();
      return(FALSE);
      }
   ln  = temp;
   ysq = temp + 1;
   num = temp + 2;



   /*
    *    **************************************************
    *    *                                                *
    *    *  LOG (X): Using the power series:              *
    *    *     e                                          *
    *    *                                                *
    *    *                X-1          1+Y                *
    *    *  We choose Y = --- then X = --- so ln X =      *
    *    *                X+1          1-Y                *
    *    *                                                *
    *    *                                                *
    *    *     1+Y             Y^3   Y^5   Y^7            *
    *    *  ln --- = 2 * ( Y + --- + --- + --- + ... )    *
    *    *     1-Y              3     5     7             *
    *    *                                                *
    *    *                                                *
    *    *  *ln  = Sum of series                          *
    *    *                                                *
    *    *  *ysq = y^2                                    *
    *    *                                                *
    *    *  *num = term numerator                         *
    *    *                                                *
    *    **************************************************
    */


         /* Compute Y = (X - 1) / (X + 1) */

   MoveWorkWork(0, 1);              /* X -> work[1] */
   SetWorkInteger(0, 1L);           /* 1 -> work[0] */
   if (! ExtendedSubtract()) {      /* Compute (X - 1) */
      free(temp);
      return(FALSE);
      }
   MoveWorkTemp(2, ysq);            /* Save (X - 1) */


   if (! ExtendedAdd()) {           /* Compute (X + 1) */
      free(temp);
      return(FALSE);
      }
   MoveWorkWork(2, 0);              /* (X + 1) -> work[0] */
   MoveTempWork(ysq, 1);            /* (X - 1) -> work[1] */
   if (! ExtendedDivide()) {        /* Compute (X - 1) / (X + 1) */
      free(temp);
      return(FALSE);
      }

   MoveWorkTemp(2, num);            /* Y -> num */
   MoveWorkTemp(2, ln);             /* Y -> ln */


         /* Compute Y^2 */

   MoveWorkWork(2, 0);              /* Get Y */
   MoveWorkWork(2, 1);
   if (! ExtendedMultiply()) {
      free(temp);                   /* Compute Y^2 */
      return(FALSE);
      }
   MoveWorkTemp(2, ysq);            /* Save Y^2 */


   den = 1L;                        /* den = 1 */


   /*
    *    **************************************************
    *    *                                                *
    *    *   LN (X): Where X = (1 + Y) / (1 - Y):         *
    *    *     e                                          *
    *    *                                                *
    *    *      1+Y             Y^3   Y^5   Y^7           *
    *    *   ln --- = 2 * ( Y + --- + --- + --- + ... )   *
    *    *      1-Y              3     5     7            *
    *    *                                                *
    *    **************************************************
    */

   while (TRUE) {             /* Until term < compprec */


            /* num *= Y^2 */

      MoveTempWork(num, 0);
      MoveTempWork(ysq, 1);
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }
      MoveWorkTemp(2, num);            /* Save new num */


            /* den += 2 */

      den += 2;


            /* num / den */

      SetWorkInteger(0, den);          /* Get den */
      MoveWorkWork(2, 1);              /* Get num */
      if (! ExtendedDivide()) {
         free(temp);
         return(FALSE);
         }

            /* ln + num / den */

      MoveWorkWork(2, 0);
      MoveTempWork(ln, 1);
      if (! ExtendedAdd()) {
         free(temp);
         return(FALSE);
         }

            /* Test for completion */

      if ( (labs(work[2].exp - work[0].exp) >= compprec)
            ||
           (! work[2].digits) ) {
         break;
         }

      MoveWorkTemp(2, ln);             /* Save new ln */

      }  /* while */


            /* Calculate 2 * ( ... ) */

   MoveWorkWork(2, 0);
   MoveWorkWork(2, 0);
   if (! ExtendedAdd()) {
      free(temp);
      return(FALSE);
      }


            /* Resolve .99999 scaling */

   if (mulp99999) {
      if (mulp99999 == 1) {
         ExtendedRecallLnP99999(0);       /* ln(.99999) -> work[0] */
         MoveWorkWork(2, 1);
         }
      else {
         MoveWorkTemp(2, ln);             /* Save ln */
         SetWorkInteger(0, (long)mulp99999);
         ExtendedRecallLnP99999(1);       /* ln(.99999) -> work[1] */
         if (! ExtendedMultiply()) {
            free(temp);
            return(FALSE);
            }
         MoveWorkWork(2, 0);
         MoveTempWork(ln, 1);
         }
      if (! ExtendedSubtract()) {
         free(temp);
         return(FALSE);
         }
      }


            /* Resolve .9999 scaling */

   if (mulp9999) {
      if (mulp9999 == 1) {
         ExtendedRecallLnP9999(0);        /* ln(.9999) -> work[0] */
         MoveWorkWork(2, 1);
         }
      else {
         MoveWorkTemp(2, ln);             /* Save ln */
         SetWorkInteger(0, (long)mulp9999);
         ExtendedRecallLnP9999(1);        /* ln(.9999) -> work[1] */
         if (! ExtendedMultiply()) {
            free(temp);
            return(FALSE);
            }
         MoveWorkWork(2, 0);
         MoveTempWork(ln, 1);
         }
      if (! ExtendedSubtract()) {
         free(temp);
         return(FALSE);
         }
      }


            /* Resolve .999 scaling */

   if (mulp999) {
      if (mulp999 == 1) {
         ExtendedRecallLnP999(0);         /* ln(.999) -> work[0] */
         MoveWorkWork(2, 1);
         }
      else {
         MoveWorkTemp(2, ln);             /* Save ln */
         SetWorkInteger(0, (long)mulp999);
         ExtendedRecallLnP999(1);         /* ln(.999) -> work[1] */
         if (! ExtendedMultiply()) {
            free(temp);
            return(FALSE);
            }
         MoveWorkWork(2, 0);
         MoveTempWork(ln, 1);
         }
      if (! ExtendedSubtract()) {
         free(temp);
         return(FALSE);
         }
      }


            /* Resolve .99 scaling */

   if (mulp99) {
      if (mulp99 == 1) {
         ExtendedRecallLnP99(0);          /* ln(.99) -> work[0] */
         MoveWorkWork(2, 1);
         }
      else {
         MoveWorkTemp(2, ln);             /* Save ln */
         SetWorkInteger(0, (long)mulp99);
         ExtendedRecallLnP99(1);          /* ln(.99) -> work[1] */
         if (! ExtendedMultiply()) {
            free(temp);
            return(FALSE);
            }
         MoveWorkWork(2, 0);
         MoveTempWork(ln, 1);
         }
      if (! ExtendedSubtract()) {
         free(temp);
         return(FALSE);
         }
      }


            /* Resolve .9 scaling */

   if (mulp9) {
      if (mulp9 == 1) {
         ExtendedRecallLnP9(0);           /* ln(.9) -> work[0] */
         MoveWorkWork(2, 1);
         }
      else {
         MoveWorkTemp(2, ln);             /* Save ln */
         SetWorkInteger(0, (long)mulp9);
         ExtendedRecallLnP9(1);           /* ln(.9) -> work[1] */
         if (! ExtendedMultiply()) {
            free(temp);
            return(FALSE);
            }
         MoveWorkWork(2, 0);
         MoveTempWork(ln, 1);
         }
      if (! ExtendedSubtract()) {
         free(temp);
         return(FALSE);
         }
      }


            /* Resolve pow10 scaling */

   if (pow10) {
      MoveWorkTemp(2, ln);             /* Save ln */
      ExtendedRecallLn10(0);           /* ln(10) -> work[0] */
      SetWorkInteger(1, pow10);        /* pow10 -> work[1] */
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }
      MoveTempWork(ln, 0);             /* ln -> work[0] */
      MoveWorkWork(2, 1);              /* (ln(10) * pow10) -> work[1] */
      if (! ExtendedAdd()) {
         free(temp);
         return(FALSE);
         }
      }

   free(temp);

   return(TRUE);


}




/*
 *    **************************************************
 *    *                                                *
 *    *         Extended Natural Antilogarithm         *
 *    *                                                *
 *    *              work[2] = e^(work[0])             *
 *    *                                                *
 *    *               (Uses 2 temp areas)              *
 *    *                                                *
 *    **************************************************
 */

extern int ExtendedExpE(void)

{

   int i, j;
   long n, pow2 = 1, powe = 0;
   COMPTYPE *temp;
   COMPTYPE *arg, *exp;
   BOOLEAN recip = FALSE;


         /* Special Cases */

   if (! work[0].digits) {                   /* X zero: return 1 */
      SetWorkInteger(2, 1L);
      return(TRUE);
      }

   if (work[0].exp > MAXEXDIGITS) {          /* Overflow */
      Overflow();
      return(FALSE);
      }


   if ( (work[0].exp >= work[0].digits)      /* If X integer */
         &&
        (work[0].exp <= MAXEXDIGITS) ) {     /*  and 'small' */
      ExtendedRecallE(1);
      return(ExtendedPower());               /*  compute directly */
      }


         /* Process neg arg as pos & take recip at exit */

   if (work[0].sign == '-') {
      recip = TRUE;
      work[0].sign = '+';
      }


   /*
    *    **************************************************
    *    *                                                *
    *    *  Scale X like this:                            *
    *    *                                                *
    *    *     e^X = e^(iii.ddd)                          *
    *    *                                                *
    *    *         = e^(iii + .ddd)                       *
    *    *                                                *
    *    *         = e^(iii) * e^(.ddd)                   *
    *    *                                                *
    *    *                                                *
    *    *     powe = iii                                 *
    *    *                                                *
    *    *     *arg = .ddd                                *
    *    *                                                *
    *    **************************************************
    */

   if (work[0].exp > 0) {                 /* Test for iii part */

      j = (int)work[0].exp;               /* Extract iii -> powe */
      for (i = 0; i < j; i++) {
         powe = powe * 10 + work[0].man[i];
         work[0].man[i] = 0;
         }

      Normalize(0);                       /* Normalize .ddd */
      }

   /*
    *    **************************************************
    *    *                                                *
    *    *  Scale X like this:                            *
    *    *                                                *
    *    *     e^X = e^(X/n * n)                          *
    *    *                                                *
    *    *         = (e^(X/n))^n                          *
    *    *                                                *
    *    *                                                *
    *    *     pow2 = n (powers of 2)                     *
    *    *                                                *
    *    **************************************************
    */

   while (work[0].exp > -7L) {            /* Scale to X < .0000001 */

      ClearWork(2);                       /* Multiply X by .5 */
      work[2].exp    = work[0].exp;
      work[2].sign   = work[0].sign;
      work[2].digits = work[0].digits + 1;
      for (i = work[0].digits - 1; i >= 0; i--) {
         if ((work[2].man[i+1] += work[0].man[i] * 5) > 9) {
            work[2].man[i] = work[2].man[i+1] / 10;
            work[2].man[i+1] %= 10;
            }
         }
      if ( (! work[2].man[0])
            ||
           (work[2].digits > compprec) ) {
         Normalize(2);
         }

      MoveWorkWork(2, 0);                 /* Save new arg */
      pow2 *= 2;
      }


         /* Set up temp registers */

   if ((temp = GETCOMPTEMP(2)) == NULL) {
      MemoryError();
      return(FALSE);
      }
   arg  = temp;
   exp  = temp + 1;

   MoveWorkTemp(0, arg);                  /* Save arg */


         /* Calculate exp to first 2 terms */

   SetWorkInteger(1, 1L);                 /* Compute sum of terms 0 & 1: */
   if (! ExtendedAdd()) {                 /*  1 + X   (work[0] == X) */
      free(temp);
      return(FALSE);
      }
   MoveWorkTemp(2, exp);                  /*  and -> exp */

   n = 1L;                                /* Terms 0 & 1 done */


   /*
    *    **************************************************
    *    *                                                *
    *    *  EXPONENT (e^X): Using the power series:       *
    *    *                                                *
    *    *                                                *
    *    *                   X^2   X^3   X^4              *
    *    *     e^X = 1 + X + --- + --- + --- + ...        *
    *    *                    2!    3!    4!              *
    *    *                                                *
    *    *                                                *
    *    *     *arg = X                                   *
    *    *                                                *
    *    *     *exp = Sum of series                       *
    *    *                                                *
    *    *     n    = Term number                         *
    *    *                                                *
    *    **************************************************
    */


   while (TRUE) {             /* Until term < compprec */

      n++;                          /* n = term number */

         /* term *= X */

         /* term still in work[0] */

      MoveTempWork(arg, 1);         /* Get arg */
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }


         /* term /= n */

      SetWorkInteger(0, n);         /* Get n */
      MoveWorkWork(2, 1);           /* Get term */
      if (! ExtendedDivide()) {
         free(temp);
         return(FALSE);
         }


         /* Add to exp */

      MoveWorkWork(2, 0);           /* new term */
      MoveTempWork(exp, 1);         /* Get exp */
      if (! ExtendedAdd()) {
         free(temp);
         return(FALSE);
         }


         /* Test for completion */

      if ( (labs(work[2].exp - work[0].exp) >= compprec)
            ||
           (! work[0].digits) ) {
         break;
         }


         /* Save new exp */

      MoveWorkTemp(2, exp);

      }  /* while */



         /* Resolve pow2 scaling */

   if (pow2 > 1) {
      SetWorkInteger(0, pow2);
      MoveWorkWork(2, 1);
      if (! ExtendedPower()) {
         free(temp);
         return(FALSE);
         }
      }


         /* Resolve powe scaling */

   if (powe) {
      MoveWorkTemp(2, arg);            /* Save e^(.ddd) part */

      SetWorkInteger(0, powe);         /* Calculate e^(iii) */
      ExtendedRecallE(1);
      if (! ExtendedPower()) {
         free(temp);
         return(FALSE);
         }

      MoveWorkWork(2, 0);              /* Calculate e^(iii) * e^(.ddd) */
      MoveTempWork(arg, 1);
      if (! ExtendedMultiply()) {
         free(temp);
         return(FALSE);
         }
      }

   free(temp);

         /* Resolve reciprocal */

   if (recip) {
      MoveWorkWork(2, 0);
      SetWorkInteger(1, 1L);
      if (! ExtendedDivide()) {
         return(FALSE);
         }
      }

   return(TRUE);

}




/*
 *    **************************************************
 *    *                                                *
 *    *             Extended Reciprocal                *
 *    *                                                *
 *    *            work[2] = 1 / work[0]               *
 *    *                                                *
 *    **************************************************
 */

extern int ExtendedReciprocal(void)

{

   SetWorkInteger(1, 1L);        /* 1 -> work[1] */

   return(ExtendedDivide());

}




/*
 *    **************************************************
 *    *                                                *
 *    *              Extended Factorial                *
 *    *                                                *
 *    *              work[2] = work[0]!                *
 *    *                                                *
 *    **************************************************
 */

extern int ExtendedFactorial(void)

{

   int i;
   long x, n;


         /* Special Cases */

   if (! work[0].digits) {                /* Test for zero (0! == 1) */
      SetWorkInteger(2, 1L);
      return(TRUE);
      }

   if (work[0].sign == '-') {             /* Test for negative: error */
      NegativeArgument();
      return(FALSE);
      }

   if (work[0].exp < work[0].digits) {    /* Test for non integer: error */
      ArgumentNotInteger();
      return(FALSE);
      }

   if ( (work[0].exp == 1)                /* if X = 1 or 2: X! = X */
         &&
        (work[0].man[0] <= 2) ) {
      MoveWorkWork(0, 2);
      return(TRUE);
      }

   if (work[0].exp > 6) {                 /* Rough test for overflow */
      Overflow();
      return(FALSE);
      }


   /*
    *    **************************************************
    *    *                                                *
    *    *  FACTORIAL:  X! = 2 * 3 * ... * (X-1) * X      *
    *    *                                                *
    *    **************************************************
    */

   x = 0;                              /* X -> x */
   for (i = 0; i < (int)work[0].exp; i++) {
      x = x * 10 + work[0].man[i];
      }

   SetWorkInteger(2, 6L);              /* work[2] = N! (3!) */

   for (n = 4L; n <= x; n++) {         /* N = 4, ..., X */
      SetWorkInteger(0, n);            /* work[0] = N */
      MoveWorkWork(2, 1);              /* work[1] = (N - 1)! */
      if (! ExtendedMultiply()) {      /* work[2] = N * (N - 1) = N! */
         return(FALSE);
         }
      }

   return(TRUE);

}
