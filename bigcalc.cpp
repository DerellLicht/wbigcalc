/*
 *    **************************************************
 *    *                                                *
 *    *                   BIGCALC.C                    *
 *    *                                                *
 *    *          Extended Precision Calculator         *
 *    *                                                *
 *    *                  Main Module                   *
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
#include <conio.h>   //  getch()
#include <stdio.h>
#include <stdlib.h>
//#include <string.h>

#include "resource.h"
#include "common.h"
#include "bigcalc.h"
#include "keywin32.h"

/*
 *    **************************************************
 *    *        BIGCALC.C Function Declarations         *
 *    **************************************************
 */

static void restore_stack0(void);

static  void Add(void);
static  void Subtract(void);
static  void Multiply(void);
static  void Divide(void);
static  void Power(void);
static  void SquareRoot(void);
static  void Square(void);
static  void Reciprocal(void);
static  void Factorial(void);
static  void IntegerPart(void);
static  void FractionPart(void);
static  void Sin(void);
static  void ArcSin(void);
static  void Cos(void);
static  void ArcCos(void);
static  void Tan(void);
static  void ArcTan(void);
static  void Log(void);
static  void Exp10(void);
static  void Ln(void);
static  void ExpE(void);
static  void RecallPi(void);
static  void RecallE(void);
static  void RecallLastX(void);
static  void ChangeSign(void);
static  void GroupSize(void);
static  void MenuRoll(void);
static  void SciNotation(void);
static  void StoreX(void);
static  void AddXReg(void);
static  void SubtractXReg(void);
static  void MultiplyXReg(void);
static  void DivideXReg(void);
static  void RecallReg(void);
static  void AddRegX(void);
static  void SubtractRegX(void);
static  void MultiplyRegX(void);
static  void DivideRegX(void);
static  void ExchangeXY(void);
static  void ExchangeXReg(void);
static  void RollDown(void);
static  void RollUp(void);
static  void PushStack(void);
static  void DropStack(void);


/*
 *    **************************************************
 *    *                                                *
 *    *           Global Variable Definition           *
 *    *                                                *
 *    **************************************************
 */

NORMTYPE
   stack[4],               /* Stack registers  */
   reg[10];                /* Memory registers */
   
static NORMTYPE
   lastx;                  /* Last X register  */

WORKTYPE work[3];          /* Work registers   */

int
   normprec = 0,           /* Normal precision  */
   compprec = 0,           /* Compute precision */
   workprec = 0,           /* Work precision    */
   groupsize = 0;          /* Digit group size  */
   // menunbr = 0;            /* Menu number       */
   
static int chr = 0;                /* Input Character   */

bool scinotation = false;    /* Force sci notation if TRUE   */

static bool stacklift = true;       /* Lift stack for new X if TRUE */

/*
 *    **************************************************
 *    *                                                *
 *    *              Initialize Variables              *
 *    *                                                *
 *    **************************************************
 */
void Initialize(int argc, char *argv)
{
   // ScrInit();                    /* Initialize screen drivers */

   ExtendedInitConstants();      /* Initialize extended constants */

   if (argc > 1)
      while (*argv) {            /* Convert *argv to number */
         if (isdigit(*argv)) {
            normprec = (normprec * 10) + (*argv - '0');
            if (normprec <= MAXNORM)
               argv++;
               continue;
            }
         normprec = 0;
         break;
         }

   if ( (normprec < MINPREC)
         ||
        (normprec > MAXNORM) )
      normprec = DEFAULTPREC;

   compprec = COMPPREC;
   workprec = WORKPREC;

   ClearStack(0, 3);             /* Initialize work areas */
   ClearReg(0, 9);
   lastx = stack[0];
                                 /* Allow full screen entry for big numbers */
   // if (normprec > SIZEOFSMALL)
   //    entrysignrow = ENTRYSIGNROWBIG;
   // else
   //    entrysignrow = ENTRYSIGNROWSMALL;

   GroupSize();                  /* Toggle group size to 5 & set xxxfloatprn */
   WorkScreen();
}

/*
 *    **************************************************
 *    *                                                *
 *    *         Get decoded character from kbd         *
 *    *                                                *
 *    **************************************************
 */
static int GetChar(void)
{
   int inchr = getch();
   if (inchr == 0)
      inchr = 1000 + getch();      /* Non ASCII character */
   else
      if (isascii(inchr) )
         inchr = toupper(inchr);

   return(inchr);
}

/*
 *    **************************************************
 *    *                                                *
 *    *                Process Routines                *
 *    *                                                *
 *    **************************************************
 */



/*
 *    **************************************************
 *    *                                                *
 *    *                 Add X = Y + X                  *
 *    *                                                *
 *    **************************************************
 */
static void Add(void)
{
   restore_stack0();
   // dump_norm_reg(&stack[0], "stack0");
   // dump_norm_reg(&stack[1], "stack1");
   MoveStackWork(0, 0);
   MoveStackWork(1, 1);
   // dump_work_reg(&work[0], "work0");
   // dump_work_reg(&work[1], "work1");

   if (ExtendedAdd() ) {
      lastx = stack[0];
      DropStack();
      MoveWorkStack(2, 0);
      WriteStack(0, 2);
      stacklift = true;
   }
}

/*
 *    **************************************************
 *    *                                                *
 *    *              Subtract X = Y - X                *
 *    *                                                *
 *    **************************************************
 */
static void Subtract(void)
{
   restore_stack0();
   MoveStackWork(0, 0);
   MoveStackWork(1, 1);

   if (ExtendedSubtract() ) {
      lastx = stack[0];
      DropStack();
      MoveWorkStack(2, 0);
      WriteStack(0, 2);
      stacklift = true;
   }
}

/*
 *    **************************************************
 *    *                                                *
 *    *              Multiply X = Y * X                *
 *    *                                                *
 *    **************************************************
 */
static void Multiply(void)
{
   MessageEsc("Multiplying");

   restore_stack0();
   MoveStackWork(0, 0);
   MoveStackWork(1, 1);

   if (ExtendedMultiply() ) {
      lastx = stack[0];
      DropStack();
      MoveWorkStack(2, 0);
      WriteStack(0, 2);
      stacklift = true;
   }
}

/*
 *    **************************************************
 *    *                                                *
 *    *               Divide X = Y / X                 *
 *    *                                                *
 *    **************************************************
 */
static void Divide(void)
{
   MessageEsc("Dividing");

   restore_stack0();
   MoveStackWork(0, 0);
   MoveStackWork(1, 1);

   if (ExtendedDivide() ) {
      lastx = stack[0];
      DropStack();
      MoveWorkStack(2, 0);
      WriteStack(0, 2);
      stacklift = true;
   }
}

/*
 *    **************************************************
 *    *                                                *
 *    *                  Power (Y^X)                   *
 *    *                                                *
 *    **************************************************
 */

static void Power(void)

{

   MessageEsc("Computing Y^X");

   MoveStackWork(0, 0);
   MoveStackWork(1, 1);

   if (ExtendedPower() ) {
      if (ExtendedRound(2)) {
         lastx = stack[0];
         DropStack();
         MoveWorkStack(2, 0);
         WriteStack(0, 2);
         stacklift = true;
         }
      }

}




/*
 *    **************************************************
 *    *                                                *
 *    *               Square Root of X                 *
 *    *                                                *
 *    **************************************************
 */

static void SquareRoot(void)

{

   MessageEsc("Computing �X");

   MoveStackWork(0, 0);
   if (ExtendedSquareRoot() ) {
      lastx = stack[0];
      MoveWorkStack(2, 0);
      WriteStack(0, 0);
      stacklift = true;
      }

}




/*
 *    **************************************************
 *    *                                                *
 *    *                 Square of X                    *
 *    *                                                *
 *    **************************************************
 */

static void Square(void)

{

   MessageEsc("Computing X�");

   MoveStackWork(0, 0);
   MoveStackWork(0, 1);

   if (ExtendedMultiply() ) {
      lastx = stack[0];
      MoveWorkStack(2, 0);
      WriteStack(0, 0);
      stacklift = true;
      }

}




/*
 *    **************************************************
 *    *                                                *
 *    *                Reciprocal of X                 *
 *    *                                                *
 *    **************************************************
 */

static void Reciprocal(void)

{

   MessageEsc("Computing 1/X");

   MoveStackWork(0, 0);

   if (ExtendedReciprocal() ) {
      lastx = stack[0];
      MoveWorkStack(2, 0);
      WriteStack(0, 0);
      stacklift = true;
      }

}




/*
 *    **************************************************
 *    *                                                *
 *    *                Factorial of X                  *
 *    *                                                *
 *    **************************************************
 */

static void Factorial(void)

{

   MessageEsc("Computing X!");

   MoveStackWork(0, 0);

   if (ExtendedFactorial() ) {
      lastx = stack[0];
      MoveWorkStack(2, 0);
      WriteStack(0, 0);
      stacklift = true;
      }

}




/*
 *    **************************************************
 *    *                                                *
 *    *           Extract Integer Part of X            *
 *    *                                                *
 *    **************************************************
 */

static void IntegerPart(void)

{

   MoveStackWork(0, 0);

   if (ExtendedIntegerPart() ) {
      lastx = stack[0];
      MoveWorkStack(0, 0);
      WriteStack(0, 0);
      stacklift = true;
      }

}




/*
 *    **************************************************
 *    *                                                *
 *    *          Extract Fraction Part of X            *
 *    *                                                *
 *    **************************************************
 */

static void FractionPart(void)

{

   MoveStackWork(0, 0);

   if (ExtendedFractionPart() ) {
      lastx = stack[0];
      MoveWorkStack(0, 0);
      WriteStack(0, 0);
      stacklift = true;
      }

}




/*
 *    **************************************************
 *    *                                                *
 *    *                   Sine of X                    *
 *    *                                                *
 *    **************************************************
 */

static void Sin(void)

{

   MessageEsc("Computing SinX");

   MoveStackWork(0, 0);

   if (ExtendedSinCos(0) ) {
      lastx = stack[0];
      MoveWorkStack(2, 0);
      WriteStack(0, 0);
      stacklift = true;
      }

}




/*
 *    **************************************************
 *    *                                                *
 *    *                 ArcSine of X                   *
 *    *                                                *
 *    **************************************************
 */

static void ArcSin(void)

{

   MessageEsc("Computing ArcSinX");

   MoveStackWork(0, 0);

   if (ExtendedArcSinCos(0) ) {
      lastx = stack[0];
      MoveWorkStack(2, 0);
      WriteStack(0, 0);
      stacklift = true;
      }

}




/*
 *    **************************************************
 *    *                                                *
 *    *                  Cosine of X                   *
 *    *                                                *
 *    **************************************************
 */

static void Cos(void)

{

   MessageEsc("Computing CosX");

   MoveStackWork(0, 0);

   if (ExtendedSinCos(1) ) {
      lastx = stack[0];
      MoveWorkStack(2, 0);
      WriteStack(0, 0);
      stacklift = true;
      }

}




/*
 *    **************************************************
 *    *                                                *
 *    *                ArcCosine of X                  *
 *    *                                                *
 *    **************************************************
 */

static void ArcCos(void)

{

   MessageEsc("Computing ArcCosX");

   MoveStackWork(0, 0);

   if (ExtendedArcSinCos(1) ) {
      lastx = stack[0];
      MoveWorkStack(2, 0);
      WriteStack(0, 0);
      stacklift = true;
      }

}




/*
 *    **************************************************
 *    *                                                *
 *    *                 Tangent of X                   *
 *    *                                                *
 *    **************************************************
 */

static void Tan(void)

{

   MessageEsc("Computing TanX");

   MoveStackWork(0, 0);

   if (ExtendedTan() ) {
      lastx = stack[0];
      MoveWorkStack(2, 0);
      WriteStack(0, 0);
      stacklift = true;
      }

}




/*
 *    **************************************************
 *    *                                                *
 *    *                ArcTangent of X                 *
 *    *                                                *
 *    **************************************************
 */

static void ArcTan(void)

{

   MessageEsc("Computing ArcTanX");

   MoveStackWork(0, 0);

   if (ExtendedArcTan() ) {
      lastx = stack[0];
      MoveWorkStack(2, 0);
      WriteStack(0, 0);
      stacklift = true;
      }

}




/*
 *    **************************************************
 *    *                                                *
 *    *              Common Logarithm of X             *
 *    *                                                *
 *    **************************************************
 */

static void Log(void)

{

   MessageEsc("Computing LogX");

   MoveStackWork(0, 0);

   if (ExtendedLog() ) {
      lastx = stack[0];
      MoveWorkStack(2, 0);
      WriteStack(0, 0);
      stacklift = true;
      }

}




/*
 *    **************************************************
 *    *                                                *
 *    *           Common Antilogarithm of X            *
 *    *                                                *
 *    **************************************************
 */

static void Exp10(void)

{

   MessageEsc("Computing 10^X");

   MoveStackWork(0, 0);

   if (ExtendedExp10() ) {
      lastx = stack[0];
      MoveWorkStack(2, 0);
      WriteStack(0, 0);
      stacklift = true;
      }

}




/*
 *    **************************************************
 *    *                                                *
 *    *             Natural Logarithm of X             *
 *    *                                                *
 *    **************************************************
 */

static void Ln(void)

{

   MessageEsc("Computing lnX");

   MoveStackWork(0, 0);

   if (ExtendedLn() ) {
      lastx = stack[0];
      MoveWorkStack(2, 0);
      WriteStack(0, 0);
      stacklift = true;
      }

}




/*
 *    **************************************************
 *    *                                                *
 *    *          Natural Antilogarithm of X            *
 *    *                                                *
 *    **************************************************
 */

static void ExpE(void)

{

   MessageEsc("Computing e^X");

   MoveStackWork(0, 0);

   if (ExtendedExpE() ) {
      lastx = stack[0];
      MoveWorkStack(2, 0);
      WriteStack(0, 0);
      stacklift = true;
      }

}




/*
 *    **************************************************
 *    *                                                *
 *    *                Recall pi to X                  *
 *    *                                                *
 *    **************************************************
 */

static void RecallPi(void)

{

   if (stacklift) {
      PushStack();
      WriteStack(1, 3);
      }

   ExtendedRecallPi(0);
   MoveWorkStack(0, 0);
   WriteStack(0, 0);
   stacklift = true;

}




/*
 *    **************************************************
 *    *                                                *
 *    *                Recall e to X                   *
 *    *                                                *
 *    **************************************************
 */
static void RecallE(void)
{
   if (stacklift) {
      PushStack();
      WriteStack(1, 3);
      }

   ExtendedRecallE(0);
   MoveWorkStack(0, 0);
   WriteStack(0, 0);
   stacklift = true;
}

/*
 *    **************************************************
 *    *                                                *
 *    *                Recall Last X                   *
 *    *                                                *
 *    **************************************************
 */
static void RecallLastX(void)
{
   if (stacklift) {
      PushStack();
      WriteStack(1, 3);
   }

   stack[0] = lastx;
   WriteStack(0);
   stacklift = true;
}

/*
 *    **************************************************
 *    *                                                *
 *    *               Change Sign of X                 *
 *    *                                                *
 *    **************************************************
 */
static void ChangeSign(void)
{
   if (stack[0].digits) {        /* Only if X non zero */
      stack[0].sign = FlipSign(stack[0].sign);
      // CurPos(XSIGNROW, SIGNDISPCOL);
      if (stack[0].sign == '-')
         WChar('-');
      else
         WChar(' ');
      }

   stacklift = true;
}

/*
 *    **************************************************
 *    *                                                *
 *    *             Toggle Group Size (3/5)            *
 *    *                                                *
 *    **************************************************
 */
static void GroupSize(void)
{
   if (groupsize == 5) {
      groupsize = 3;
      // minfloatprn = (long)normprec - 1077L;
      // maxfloatprn = 1080L;
   }
   else {
      groupsize = 5;
      // minfloatprn = (long)normprec - 1195L;
      // maxfloatprn = 1200L;
   }

   show_status_info();
}

/*
 *    **************************************************
 *    *                                                *
 *    *             Roll Function Key Menu             *
 *    *                                                *
 *    **************************************************
 */
static void MenuRoll(void)
{
   // menunbr = 1 - menunbr;     /* Toggle menunbr 0/1 */
   // OnScreenMenu();
}

/*
 *    **************************************************
 *    *                                                *
 *    *          Toggle Scientific Notation            *
 *    *                                                *
 *    **************************************************
 */
static void SciNotation(void)
{
   if (scinotation)
      scinotation = false ;
   else
      scinotation = true;

   WriteReg(0, 9);
   WriteStack(0, 3);
}

//*******************************************************************************
void clear_stack_or_register(uint button_code)
{
   uint r, s ;   
   switch (button_code) {
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
      r = button_code - IDB_CLEAR_R0 ;
      ClearReg(r, r);
      WriteReg(r, r);
      break ;
   
   case IDB_CLEAR_X:
   case IDB_CLEAR_Y:
   case IDB_CLEAR_Z:
   case IDB_CLEAR_T:
      s = button_code - IDB_CLEAR_X ;
      ClearStack(s, s);
      WriteStack(s, s);
      break ;

   case IDB_CLRS:
      ClearStack(0, 3);
      WriteStack(0, 3);
      break ;
      
   case IDB_CLRR:
      ClearReg(0, 9);
      WriteReg(0, 9);
      break ;
      
   default:
      break ;
   }
}

/*
 *    **************************************************
 *    *                                                *
 *    *              Store X in Register               *
 *    *                                                *
 *    *             Allows + - * / as well             *
 *    *                                                *
 *    **************************************************
 */
static void StoreX(void)
{
   int r;

   Message("Store X to: Press Reg (0-9) or Operation (+,-,*,/) then Reg (0-9) or Esc:");

   while ((chr = GetChar()) != ESCAPE) {

      if (isdigit(chr) ) {
         r = chr - '0';
         reg[r] = stack[0];
         WriteReg(r, r);
         stacklift = true;
         return;
         }

      switch (chr) {

         case (ADD):
            AddXReg();
            return;

         case (SUBTRACT):
            SubtractXReg();
            return;

         case (MULTIPLY):
            MultiplyXReg();
            return;

         case (DIVIDE):
            DivideXReg();
            return;

         default:
            ;

         }  /* switch */

      } /* while */
}

/*
 *    **************************************************
 *    *                                                *
 *    *               Add X to Register                *
 *    *                                                *
 *    **************************************************
 */
static void AddXReg(void)
{
   int r;

   Message("Add X to: Reg (0-9) or Esc:");

   while ((chr = GetChar()) != ESCAPE) {

      if (isdigit(chr) ) {
         r = chr - '0';
         MoveStackWork(0, 0);
         MoveRegWork(r, 1);

         if (ExtendedAdd() ) {
            MoveWorkReg(2, r);
            WriteReg(r, r);
            stacklift = true;
            return;
            }
         return;
         }
      else
            ;

      }  /* while */
}

/*
 *    **************************************************
 *    *                                                *
 *    *           Subtract X from Register             *
 *    *                                                *
 *    **************************************************
 */
static void SubtractXReg(void)
{
   int r;

   Message("Subtract X from: Reg (0-9) or Esc:");

   while ((chr = GetChar()) != ESCAPE) {

      if (isdigit(chr) ) {
         r = chr - '0';
         MoveStackWork(0, 0);
         MoveRegWork(r, 1);

         if (ExtendedSubtract() ) {
            MoveWorkReg(2, r);
            WriteReg(r, r);
            stacklift = true;
            return;
            }
         return;
         }
      else
            ;

      }  /* while */
}

/*
 *    **************************************************
 *    *                                                *
 *    *            Multiply X times Register           *
 *    *                                                *
 *    **************************************************
 */
static void MultiplyXReg(void)
{
   int r;

   Message("Multiply X times: Reg (0-9) or Esc:");

   while ((chr = GetChar()) != ESCAPE) {

      if (isdigit(chr) ) {
         MessageEsc("Multiplying");

         r = chr - '0';
         MoveStackWork(0, 0);
         MoveRegWork(r, 1);

         if (ExtendedMultiply() ) {
            MoveWorkReg(2, r);
            WriteReg(r, r);
            stacklift = true;
            return;
            }
         return;
         }
      else
            ;

      }  /* while */
}

/*
 *    **************************************************
 *    *                                                *
 *    *             Divide X into Register             *
 *    *                                                *
 *    **************************************************
 */
static void DivideXReg(void)
{
   int r;

   Message("Divide X into: Reg (0-9) or Esc:");

   while ((chr = GetChar()) != ESCAPE) {

      if (isdigit(chr) ) {
         MessageEsc("Dividing");

         r = chr - '0';
         MoveStackWork(0, 0);
         MoveRegWork(r, 1);

         if (ExtendedDivide() ) {
            MoveWorkReg(2, r);
            WriteReg(r, r);
            stacklift = true;
            return;
            }
         return;
         }
      else
            ;

      }  /* while */
}

/*
 *    **************************************************
 *    *                                                *
 *    *             Recall Register to X               *
 *    *                                                *
 *    **************************************************
 */
static void RecallReg(void)
{
   int r;

   Message("Recall to X: Press Reg (0-9) or Operation (+,-,*,/)"
           " then Reg (0-9) or Esc:");

   while ((chr = GetChar()) != ESCAPE) {

      if (isdigit(chr) ) {
         r = chr - '0';
         if (stacklift)
            PushStack();
         stack[0] = reg[r];
         if (stacklift)
            WriteStack(0, 3);
         else
            WriteStack(0, 0);
         stacklift = true;
         return;
         }

      switch (chr) {

         case (ADD):
            AddRegX();
            return;

         case (SUBTRACT):
            SubtractRegX();
            return;

         case (MULTIPLY):
            MultiplyRegX();
            return;

         case (DIVIDE):
            DivideRegX();
            return;

         default:
            ;
         }  /* switch */
      } /* while */
}

/*
 *    **************************************************
 *    *                                                *
 *    *               Add Register to X                *
 *    *                                                *
 *    **************************************************
 */
static void AddRegX(void)
{
   int r;

   Message("Add to X: Reg (0-9) or Esc:");

   while ((chr = GetChar()) != ESCAPE) {

      if (isdigit(chr) ) {
         r = chr - '0';
         MoveRegWork(r, 0);
         MoveStackWork(0, 1);

         if (ExtendedAdd() ) {
            lastx = stack[0];
            MoveWorkStack(2, 0);
            WriteStack(0, 0);
            stacklift = true;
            return;
            }
         return;
         }
      else
            ;

      }  /* while */
}

/*
 *    **************************************************
 *    *                                                *
 *    *           Subtract Register from X             *
 *    *                                                *
 *    **************************************************
 */
static void SubtractRegX(void)
{
   int r;

   Message("Subtract from X: Reg (0-9) or Esc:");

   while ((chr = GetChar()) != ESCAPE) {

      if (isdigit(chr) ) {
         r = chr - '0';
         MoveRegWork(r, 0);
         MoveStackWork(0, 1);

         if (ExtendedSubtract() ) {
            lastx = stack[0];
            MoveWorkStack(2, 0);
            WriteStack(0, 0);
            stacklift = true;
            return;
            }
         return;
         }
      else
            ;
      }  /* while */
}

/*
 *    **************************************************
 *    *                                                *
 *    *            Multiply Register times X           *
 *    *                                                *
 *    **************************************************
 */
static void MultiplyRegX(void)
{
   int r;

   Message("Multiply times X: Reg (0-9) or Esc:");

   while ((chr = GetChar()) != ESCAPE) {

      if (isdigit(chr) ) {
         MessageEsc("Multiplying");

         r = chr - '0';
         MoveRegWork(r, 0);
         MoveStackWork(0, 1);

         if (ExtendedMultiply() ) {
            lastx = stack[0];
            MoveWorkStack(2, 0);
            WriteStack(0, 0);
            stacklift = true;
            return;
            }
         return;
         }
      else
            ;

      } /* while */

}

/*
 *    **************************************************
 *    *                                                *
 *    *             Divide Register into X             *
 *    *                                                *
 *    **************************************************
 */
static void DivideRegX(void)
{
   int r;

   Message("Divide into X: Reg (0-9) or Esc:");

   while ((chr = GetChar()) != ESCAPE) {

      if (isdigit(chr) ) {
         MessageEsc("Dividing");

         r = chr - '0';
         MoveRegWork(r, 0);
         MoveStackWork(0, 1);

         if (ExtendedDivide() ) {
            lastx = stack[0];
            MoveWorkStack(2, 0);
            WriteStack(0, 0);
            stacklift = true;
            return;
            }
         return;
         }
      else
            ;

      }  /* while */
}

/*
 *    **************************************************
 *    *                                                *
 *    *                Exchange X & Y                  *
 *    *                                                *
 *    **************************************************
 */
static void ExchangeXY(void)
{
   NORMTYPE *temp;

   if ((temp = GETNORMTEMP(1)) == NULL) {
      MemoryError();
      return;
      }

   *temp = stack[0];
   stack[0] = stack[1];
   stack[1] = *temp;

   WriteStack(0, 1);
   stacklift = true;

   free(temp);

}

/*
 *    **************************************************
 *    *                                                *
 *    *             Exchange X & Register              *
 *    *                                                *
 *    **************************************************
 */
static void ExchangeXReg(void)
{
   int r;
   NORMTYPE *temp;

   Message("Exchange X with: Press Reg (0-9) or Esc:");

   while ((chr = GetChar()) != ESCAPE) {

      if (isdigit(chr) ) {

      if ((temp = GETNORMTEMP(1)) == NULL) {
         MemoryError();
         return;
         }

         r = chr - '0';
         *temp = stack[0];
         stack[0] = reg[r];
         reg[r] = *temp;
         WriteReg(r, r);
         WriteStack(0, 0);
         stacklift = true;
         free(temp);
         return;
         }

      } /* while */
}

/*
 *    **************************************************
 *    *                                                *
 *    *                Roll Stack Down                 *
 *    *                                                *
 *    **************************************************
 */
static void RollDown(void)
{
   NORMTYPE *temp;

   if ((temp = GETNORMTEMP(1)) == NULL) {
      MemoryError();
      return;
      }

   *temp = stack[0];
   DropStack();
   stack[3] = *temp;

   WriteStack(0, 3);
   stacklift = true;

   free(temp);
}

/*
 *    **************************************************
 *    *                                                *
 *    *                 Roll Stack Up                  *
 *    *                                                *
 *    **************************************************
 */
static void RollUp(void)
{
   NORMTYPE *temp;

   if ((temp = GETNORMTEMP(1)) == NULL) {
      MemoryError();
      return;
      }

   *temp = stack[3];
   PushStack();
   stack[0] = *temp;

   WriteStack(0, 3);
   stacklift = true;

   free(temp);
}

/*
 *    **************************************************
 *    *                                                *
 *    *                  Push Stack                    *
 *    *                                                *
 *    **************************************************
 */
static void PushStack(void)
{
   int s;

   for (s = 3; s >= 1; s--)
      stack[s] = stack[s - 1];
}

/*
 *    **************************************************
 *    *                                                *
 *    *                  Drop Stack                    *
 *    *                                                *
 *    **************************************************
 */
static void DropStack(void)
{
   int s;

   for (s = 0; s <= 2; s++)
      stack[s] = stack[s + 1];
}

//*********************************************************************
//  Exit from GetX state
//  success: false means data entry was aborted
//*********************************************************************
static void ExitGetXState(bool success)
{
   if (normprec > SIZEOFSMALL) {
      // result = ExtendedGetX();
      if (success) {
          if (stacklift)
             PushStack();
         MoveWorkStack(0, 0);
         stacklift = true;
         }
         // else {
         //    ExitGetXState();
         // }
      WorkScreen();
   }
   else {
      // result = ExtendedGetX();
      if (success) {
         MoveWorkStack(0, 0);
         stacklift = true;
      }
      // else {
         // ExitGetXState();
      // }
      WriteStack(0);
   }
   keyboard_state_set(KBD_STATE_DEFAULT);
}

//***********************************************************************
static void restore_stack0(void)
{
   if (keyboard_state_get() == KBD_STATE_GETX) {
      move_local_to_work0();
      MoveWorkStack(0, 0);
      Message("");
      keyboard_state_set(KBD_STATE_DEFAULT);
   }
}

/*
 *    **************************************************
 *    *                                                *
 *    *                    Enter                       *
 *    *                                                *
 *    **************************************************
 */
static void Enter(bool success)
{
   if (success) {
      Message("Return/Enter received");
      // syslog("Enter: %u: [%s]\n", get_output_str_len(), get_output_str());
      move_local_to_work0();
      MoveWorkStack(0, 0);
      PushStack();   //  push X to Y
      WriteStack(0, 3); //  update display fields
      stacklift = true;
   }
   else {
      Message("Data entry aborted by user");
      WriteStack(0);
   }
   stacklift = false;
   keyboard_state_set(KBD_STATE_DEFAULT);
   // ExitGetXState(success);
}

/*
 *    **************************************************
 *    *                                                *
 *    *               Accept X from KBD                *
 //  later: enter GetX state
 *    *                                                *
 *    **************************************************
 */
static void AcceptX(u16 inchr)
{
   int result ;
   if (stacklift) {
      PushStack();   //  push X to Y
      WriteStack(1, 3); //  update display fields
   }
   
   if (keyboard_state_get() == KBD_STATE_GETX) {
      result = ExtendedGetX(inchr);
      if (!result) {
         ExitGetXState(false);
      }
      return ;      
   }

   init_getx_vars();
   getx_clear_output_str();
   reset_output_str();
   put_stack(0, " ");
   ClearWork(0);
   Message("Entering X: S=ChgSign, E=Exp, BakSpc=Backup, Other=Complete, ESC=Exit");
   keyboard_state_set(KBD_STATE_GETX);
   
   result = ExtendedGetX(inchr); //  process first input char
   if (!result) {
      ExitGetXState(false);
   }
   if (normprec > SIZEOFSMALL) {
      //  initial call to GetX function
      WorkScreen();
   }
}

//***************************************************************************
//  called from keyboard_state_handler() in GetX state
//***************************************************************************
// int main(int argc,char *argv[])
int dos_main(u16 inchr)
{
   // Initialize(argc, argv[1]);

   //  chr is a global var, used by ExtendedGetX() et al
   // chr = GetPrompt();
   chr = inchr ;  

   // while (chr != ESCAPE) {

      switch (inchr) {
         case ('0'):
         case ('1'):
         case ('2'):
         case ('3'):
         case ('4'):
         case ('5'):
         case ('6'):
         case ('7'):
         case ('8'):
         case ('9'):
         case ('.'):
         case ('E'):
            /* Accept new X from keyboard with first */
            /*  character passed to AcceptX routine */
            //  This enters GetX state if no errors occur
            AcceptX(inchr);           
            break;                  

         case (kBSPACE):        
            if (!ExtendedGetX(inchr)) {
               ExitGetXState(false);
            }
            break ;
            
         case (kENTER):        
            Enter(true);    break;            //  executed from dos_main()
            
         //  what is done with ESCAPE, depends upon keyboard state!!
         case (kESC):
            Enter(false);   break ; //  executed from dos_main()
         
         case (ADD):          Add();      break;   /* Add Y + X */
         case (SUBTRACT):     Subtract(); break;   /* Subtract Y - X */
         case (MULTIPLY):     Multiply(); break;   /* Multiply Y * X */
         case (DIVIDE):       Divide();   break;   /* Divide Y / X */
         
         case (SCINOT):       SciNotation();  break;     /* Use Scientific Notation */
         case (GROUPSIZE):    GroupSize();  break;       /* Toggle Group Size (3/5) */
         case (ROLLUP):       RollUp();  break;          /* Roll stack up */
         case (ROLLDOWN):     RollDown();  break;        /* Roll stack down */
         case (LASTX):        RecallLastX(); break;      /* Recall Last X */

         //  button row 2         
         case (HELP):   
            MessageBox(NULL, "Help file is not yet ready", NULL, MB_OK | MB_ICONEXCLAMATION);
            //  eventually, this will load a Windows help file
            break;
         case (POWER):        Power();       break;         /* Power (Y^X) */
         case (SQUAREROOT):   SquareRoot();  break;        /* Square Root X */
         case (SQUARE):       Square();      break;      /* Square X */
         case (RECIPROCAL):   Reciprocal();  break;        /* Reciprocal X */
         case (FACTORIAL):    Factorial();   break;         /* Factorial X */
         case (INTEGER):      IntegerPart(); break;       /* Integer Part X */
         case (FRACTION):     FractionPart();   break;      /* Fraction Part X */
         case (RECALLPI):     RecallPi(); break;         /* Recall pi */
         case (RECALLE):      RecallE();  break;         /* Recall e */

         //  button row 3         
         case (SIN   ):       Sin();      break;   /* Sine X */
         case (ARCSIN):       ArcSin();   break;            /* ArcSine X */
         case (COS   ):       Cos();    break;             /* Cosine X */
         case (ARCCOS):       ArcCos(); break;           /* ArcCosine X */
         case (TAN   ):       Tan();    break;           /* Tangent X */
         case (ARCTAN):       ArcTan(); break;           /* ArcTangent X */
         case (LOG   ):       Log(); break;              /* Common Log X */
         case (EXP10 ):       Exp10();  break;           /* Exponent 10^X */
         case (LN    ):       Ln();  break;              /* Natural Log X */
         case (EXPE  ):       ExpE(); break;             /* Exponent e^X */
         
         case (CHGSIGN):      ChangeSign();  break;      /* Change sign X */
         case (MENUROLL):     MenuRoll();   break;       /* Roll Function Key Menu */
         case (STOREX):       StoreX();  break;          /* Store X in register (prompt for which) */
         case (RECALLREG):    RecallReg(); break;        /* Recall register to X (prompt for which) */
         case (XCHGXY1):
         case (XCHGXY2):      ExchangeXY();  break;      /* Exchange X and Y */
         case (XCHGXREG):     ExchangeXReg(); break;     /* Exchange X and Reg (prompt for which) */
         default:
            ;              /* Unknown key */

         }  /* end switch */

      // flushall();

      // chr = GetPrompt();
      // }  /* while */
   // ScrTerm();

   return 0;
}

//*********************************************************************
static keyboard_state_t keyboard_state = KBD_STATE_DEFAULT ;

int keyboard_state_handler(u16 inchr)
{
   int result = TRUE ;
   switch(keyboard_state) {
   case KBD_STATE_DEFAULT:
      dos_main(inchr);
      result = TRUE ;
      break ;

   case KBD_STATE_GETX:
      if (inchr == kENTER) {
         Enter(true);   //  executed from GetX state
      }
      else {
         result = ExtendedGetX(inchr);
         if (!result) {
            // ExitGetXState(false);
            Enter(false);
         }
      }
      break ;
      
   default:
      {
      char msg[30];
      sprintf(msg, "Invalid kbd state: %u", (uint) keyboard_state);
      MessageBox(NULL, msg, NULL, MB_OK | MB_ICONERROR);
      }
      keyboard_state = KBD_STATE_DEFAULT ;
      result = FALSE ;
      break ;
   }
   return result;
}

bool keyboard_state_set(keyboard_state_t new_kbd_state)
{
   switch(new_kbd_state) {
   case KBD_STATE_DEFAULT:
      show_hide_buttons(true);
      show_keyboard_state("default");
      keyboard_state = new_kbd_state ;
      return true ;
   case KBD_STATE_GETX:
      show_hide_buttons(false);
      show_keyboard_state("Input X value");
      keyboard_state = new_kbd_state ;
      return true ;
      
   default:
      return false ;
   }
}

keyboard_state_t keyboard_state_get(void)
{
   return keyboard_state ;
}

