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
#include <stdio.h>
#include <stdlib.h>

#include "resource.h"
#include "common.h"
#include "commonw.h"
#include "bigcalc.h"
#include "keywin32.h"

/*
 *    **************************************************
 *    *        BIGCALC.C Function Declarations         *
 *    **************************************************
 */

static void restore_stack0(void);
static void AcceptXstatic(char *instr);
static void Enter(void);

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
   workprec = 0;           /* Work precision    */
   
uint groupsize = 0;          /* Digit group size  */

char view_dec_point_char = '.' ; //  either '.' or '*'

//static int chr = 0;                /* Input Character   */

bool scinotation = false;    /* Force sci notation if TRUE   */

static bool stacklift = true;       /* Lift stack for new X if TRUE */

/*
 *    **************************************************
 *    *                                                *
 *    *              Initialize Variables              *
 *    *                                                *
 *    **************************************************
 */
void Initialize(void)
{
   ExtendedInitConstants();      /* Initialize extended constants */

   //  now handled by config/options modules
   if ( (normprec < MINPREC) ||
        (normprec > MAXNORM) ) {
         normprec = DEFAULTPREC;
        }

   compprec = COMPPREC;
   workprec = WORKPREC;

   ClearStack(0, 3);             /* Initialize work areas */
   ClearReg(0, 9);
   lastx = stack[0];

   read_config_file() ;    //  read current screen position, etc
   
   //  these should be done *after* reading config file
   // GroupSize();    /* Toggle group size to 5 & set xxxfloatprn */
   WorkScreen();
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


/*
 *    **************************************************
 *    *                                                *
 *    *                 Add X = Y + X                  *
 *    *                                                *
 *    **************************************************
 */
void Add(void)
{
   restore_stack0();
   MoveStackWork(0, 0);
   MoveStackWork(1, 1);

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
void Subtract(void)
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
void Multiply(void)
{
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
void Divide(void)
{
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
void Power(void)
{
   restore_stack0();
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
void SquareRoot(void)
{
   restore_stack0();
   MoveStackWork(0, 0);
   if (ExtendedSquareRoot() ) {
      lastx = stack[0];
      MoveWorkStack(2, 0);
      WriteStack(0);
      stacklift = true;
   }
   // show_status_info();
}

/*
 *    **************************************************
 *    *                                                *
 *    *                 Square of X                    *
 *    *                                                *
 *    **************************************************
 */
void Square(void)
{
   restore_stack0();
   MoveStackWork(0, 0);
   MoveStackWork(0, 1);

   if (ExtendedMultiply() ) {
      lastx = stack[0];
      MoveWorkStack(2, 0);
      WriteStack(0);
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
void Reciprocal(void)
{
   restore_stack0();
   MoveStackWork(0, 0);

   if (ExtendedReciprocal() ) {
      lastx = stack[0];
      MoveWorkStack(2, 0);
      WriteStack(0);
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
void Factorial(void)
{
   restore_stack0();
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
void IntegerPart(void)
{
   restore_stack0();
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
void FractionPart(void)
{
   restore_stack0();
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
void Sin(void)
{
   restore_stack0();
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
void ArcSin(void)
{
   restore_stack0();
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
void Cos(void)
{
   restore_stack0();
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
void ArcCos(void)
{
   restore_stack0();
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
void Tan(void)
{
   restore_stack0();
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
void ArcTan(void)
{
   restore_stack0();
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
void Log(void)
{
   restore_stack0();
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
void Exp10(void)
{
   restore_stack0();
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
void Ln(void)
{
   restore_stack0();
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
void ExpE(void)
{
   restore_stack0();
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
void RecallPi(void)
{
   if (stacklift) {
      PushStack();
      WriteStack(1, 3);
      }

   ExtendedRecallPi(0);
   MoveWorkStack(0, 0);
   WriteStack(0);
   stacklift = true;
}

/*
 *    **************************************************
 *    *                                                *
 *    *                Recall e to X                   *
 *    *                                                *
 *    **************************************************
 */
void RecallE(void)
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
void RecallLastX(void)
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
void ChangeSign(void)
{
   if (stack[0].digits) {        /* Only if X non zero */
      stack[0].sign = FlipSign(stack[0].sign);
      WriteStack(0);
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
//lint -esym(714, GroupSize)
//lint -esym(759, GroupSize)
//lint -esym(765, GroupSize)
void GroupSize(void) //  unused
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
   save_cfg_file();
}

/*
 *    **************************************************
 *    *                                                *
 *    *          Toggle Scientific Notation            *
 *    *                                                *
 *    **************************************************
 */
//lint -esym(714, SciNotation)
//lint -esym(759, SciNotation)
//lint -esym(765, SciNotation)
void SciNotation(void)  //  unused
{
   if (scinotation)
      scinotation = false ;
   else
      scinotation = true;

   WriteReg(0, 9);
   WriteStack(0, 3);
   save_cfg_file();
}

//*******************************************************************************
void clear_stack_or_register(uint button_code)
{
   uint r, s ;   
   Message("");
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
      save_cfg_file();
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
      save_cfg_file();
      break ;
      
   default:
      break ;
   }
}

/*
 *    **************************************************
 *    *                                                *
 *    *                Exchange X & Y                  *
 *    *                                                *
 *    **************************************************
 */
void ExchangeXY(void)
{
   NORMTYPE *temp;

   if ((temp = GETNORMTEMP(1)) == NULL) {
      MessageError(ERROR_NO_MEMORY);
      return;
      }

   restore_stack0();
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
static void exit_from_xchg_reg_state(void)
{
   Message("");
   show_hide_view_xchg_buttons(false);
   show_hide_all_buttons(true, IDB_XCHG_X_R);
   keyboard_state_set(KBD_STATE_DEFAULT) ;
}

void ExchangeXReg(void)
{
   if (keyboard_state_get() == KBD_STATE_DEFAULT) {
      Message("Exchange X with Register: Press <Xchg> button by register, or 'XChg X/R'");
      show_hide_view_xchg_buttons(true);
      show_hide_all_buttons(false, IDB_XCHG_X_R);
      keyboard_state_set(KBD_STATE_GETREG) ;
   }
   else {
      exit_from_xchg_reg_state();
   }
}

void ExchangeXReg_exec(uint target)
{
   NORMTYPE *temp;
   uint r = target - IDB_XCHG_R0 ;
   if ((temp = GETNORMTEMP(1)) == NULL) {
      MessageError(ERROR_NO_MEMORY);
      return;
   }

   *temp = stack[0];
   stack[0] = reg[r];
   reg[r] = *temp;
   WriteReg(r, r);
   WriteStack(0, 0);
   stacklift = true;
   free(temp);
   exit_from_xchg_reg_state();
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
static void exit_from_store_reg_state(void)
{
   Message("");
   show_hide_view_stor_buttons(false);
   show_hide_all_buttons(true, IDB_STORE_X);
   keyboard_state_set(KBD_STATE_DEFAULT) ;
}

void StoreX(void)
{
   if (keyboard_state_get() == KBD_STATE_DEFAULT) {
      Message("Store X to Register: Press <Stor> button by register, or 'Store X' to exit");
      show_hide_view_stor_buttons(true);
      show_hide_all_buttons(false, IDB_STORE_X);
      keyboard_state_set(KBD_STATE_GETREG) ;
   }
   else {
      exit_from_store_reg_state();
   }
}

void StoreX_exec(uint target)
{
   Message("");

   uint r = target - IDB_STOR_R0 ;
   reg[r] = stack[0];
   WriteReg(r, r);
   save_cfg_file();
   exit_from_store_reg_state();
   stacklift = true;
}

/*
 *    **************************************************
 *    *                                                *
 *    *             Recall Register to X               *
 *    *                                                *
 *    **************************************************
 */
static void exit_from_recall_reg_state(void)
{
   Message("");
   show_hide_view_rcall_buttons(false);
   show_hide_all_buttons(true, IDB_RECALL_X);
   keyboard_state_set(KBD_STATE_DEFAULT) ;
}

void RecallReg(void)
{
   Message("Recall to X: Press Reg (0-9) or Esc:");
   if (keyboard_state_get() == KBD_STATE_DEFAULT) {
      Message("Recall X from Register: Press <Rcall> button by register, or 'Recall X' to exit");
      show_hide_view_rcall_buttons(true);
      show_hide_all_buttons(false, IDB_RECALL_X);
      keyboard_state_set(KBD_STATE_GETREG) ;
   }
   else {
      exit_from_recall_reg_state();
   }
   return;
}

void RecallReg_exec(uint target)
{
   Message("");

   uint r = target - IDB_RCALL_R0 ;
   // if (stacklift)
   //    PushStack();
   stack[0] = reg[r];
   // if (stacklift)
   //    WriteStack(0, 3);
   // else
      WriteStack(0, 0);
      exit_from_recall_reg_state();
   stacklift = true;
   return;
}

//*********************************************************************
//  Paste value into modified X register
//  Maximum integer input (64-bit): 18,446,744,073,709,551,615  
//                                  18446744073709551615  
//*********************************************************************
static void exit_from_paste_value_state(void)
{
   Message("");
   show_hide_view_buttons(false);
   show_hide_all_buttons(true, IDB_PASTE_X);
   enable_paste_field(false);
   keyboard_state_set(KBD_STATE_DEFAULT) ;
}

void PasteValueEnable(void)
{
   Message("Paste number into X stack field:");
   if (keyboard_state_get() == KBD_STATE_DEFAULT) {
      Message("Recall X from Register: Press <Rcall> button by register, or 'Recall X' to exit");
      show_hide_view_buttons(true);
      show_hide_all_buttons(false, IDB_PASTE_X);
      enable_paste_field(true);
      keyboard_state_set(KBD_STATE_GETREG) ;
   }
   else {
      exit_from_paste_value_state();
   }
}

void PasteValue_exec(void)
{
   Message("");
   
   //  copy data from paste field
   char *tptr = get_paste_str();
   // syslog("[%s]\n", tptr);
   // [18446744073709551615]
   
   //  turn this string into a bigcalc working struct
   AcceptXstatic(tptr);
   
   //  move data into X stack
   move_local_to_work0();
   MoveWorkStack(0, 0);
   exit_from_paste_value_state();
   stacklift = true;
}

//***********************************************************************************
//  process commands from script file
//***********************************************************************************
typedef struct script_cmd_map_s {
   char *cmd_str ;
   void (*func) (void) ;
}  script_cmd_map_t ;

static script_cmd_map_t const script_cmd_map[] = {
   { "rollup", RollUp },
   { "push",   Enter },
   { "+",      Add      },
   { "-",      Subtract },
   { "*",      Multiply },
   { "/",      Divide   },
   { "sqrt",   SquareRoot },
   { "square", Square },
{ NULL, NULL }};

static void process_script_command(char *cmd)
{
   uint idx ;
   int clen = strlen(cmd);
   for (idx=0; script_cmd_map[idx].func != NULL; idx++) {
      if (strncmp(cmd, script_cmd_map[idx].cmd_str, clen) == 0) {
         // syslog("execute cmd [%s]\n", cmd);
         (script_cmd_map[idx].func());
         return ;
      }
   }
   // syslog("script command not found\n");
}

//***********************************************************************************
//  Load script file, with program commands.
//  Initially, this is intended for running test vectors.
//***********************************************************************************
void RunScriptFromFile(HWND hwnd)
{
   char command_filename[MAX_PATH_LEN] = "commands.txt" ;
   char msgstr[120] = "" ;
   if (!select_text_file(hwnd, command_filename)) {
      Message("RunScriptFromFile: operation aborted") ;
      return ;
   }
   
   FILE *fd = fopen(command_filename, "rt") ;
   if (fd == 0) {
      sprintf(msgstr, "%s: fopen failed", command_filename);
      Message(msgstr);
      return ;
   }
   // unsigned lcount = 0 ;
   char inpbfr[MAXNORM+1] = "";
   while (fgets(inpbfr, MAXNORM, fd) != 0) {
      // lcount++ ;
      char *tptr = inpbfr ;
      strip_newlines(tptr);
      if (strlen(tptr) == 0) {
         continue ;
      }
      
      //  process comments
      if (*tptr == ';') {
         //  ignore comment
      } else 
      //  process commands
      if (*tptr == ':') {
         tptr++ ; //  skip the colon
         process_script_command(tptr);
      } else
      {
         // syslog("num: [%s]\n", tptr);
         //  turn this string into a bigcalc working struct
         AcceptXstatic(tptr);
         
         //  move data into X stack
         move_local_to_work0();
         MoveWorkStack(0, 0);
      }
   }
   fclose(fd) ;
}

/*
 *    **************************************************
 *    *                                                *
 *    *                Roll Stack Down                 *
 *    *                                                *
 *    **************************************************
 */
void RollDown(void)
{
   NORMTYPE *temp;

   if ((temp = GETNORMTEMP(1)) == NULL) {
      MessageError(ERROR_NO_MEMORY);
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
void RollUp(void)
{
   NORMTYPE *temp;

   if ((temp = GETNORMTEMP(1)) == NULL) {
      MessageError(ERROR_NO_MEMORY);
      return;
      }

   *temp = stack[3];
   PushStack();
   stack[0] = *temp;

   WriteStack(0, 3);
   stacklift = true;

   free(temp);
}

//**********************************************************************
// DNR: [dump stack 0: 2,+,50: 2,8,9,4,8,2,2,9,6,5,2,2,6,0,2,5,7,0,0,1,5,0,0,1,1,1,3,9,4,3,2,7,8,6,9,1,2,1,3,1,8,2,8,7,8,1,5,4,2,6,]
// slen = sprintf(outmsg, "%s: %ld,%c,%d: ", msg, nptr->exp, nptr->sign, nptr->digits);
// for (idx=0; idx<nptr->digits; idx++) {
//    slen += sprintf(&outmsg[slen], "%d,", nptr->man[idx]);
// }
//  nptr->digits is total number of digits in value
//  nptr->exp is number of digits before decimal point
 //**********************************************************************
static char *form_full_view_str(NORMTYPE *nfield)
{
   static char view_str[1100] = "" ;
   // static char dec_point_char = '*' ;
   uint outidx = 0 ; //  index into outstr
   uint inidx ;  //  index for walking data
   uint slen = nfield->digits ;
   int expdigits = nfield->exp ;
   uint mspan ;
   
   if (slen == 0) {
      sprintf(view_str, "empty");
   }
   //  .1
   // DNR: [dump stack 0: 0,+,1: 1,]
   // X:    0*142 857 142 857 142 857 142 857 142 857 142 857 142 857 142 857 14   
   // [dump stack 0: 0,+,50: 1,4,2,8,5,7,1,4,2,8,5,7,1,4,2,8,5,7,1,4,2,8,5,7,1,4,2,8,5,7,1,4,2,8,5,7,1,4,2,8,5,7,1,4,2,8,5,7,1,4,]
   else if (expdigits == 0) {
      // mspan = (uint) expdigits % groupsize ;
      // if (mspan == 0) {
         mspan = groupsize ;
      // }
      
      
      outidx = sprintf(view_str, "%c0*", nfield->sign);
      for (inidx=0; inidx < slen; inidx++) {
         outidx += sprintf(&view_str[outidx], "%u", nfield->man[inidx]);   //lint !e705
         //  if mspan == 0, don't put any spacing between groups
         if (groupsize != 0) {
            if (--mspan == 0) {
               outidx += sprintf(&view_str[outidx], " ");
               mspan = groupsize ;
            }
         }
         
      }
   }
   //  0.00031...
   // X: 0*00031 04625 89257 99441 16733 93356 10058 98789 19590 18938 217
   //  .0002308...
   // DNR: [dump stack 0: -3,+,50: 2,3,0,8,4,0,2,5,8,5,4,1,0,8,9,5,6,6,0,2,0,3,1,3,9,4,2,7,5,1,6,1,5,8,8,1,8,0,9,7,8,7,6,2,6,9,6,2,1,4,]
   else if (expdigits < 0) {
      uint zeroes = -expdigits ;
      outidx = sprintf(view_str, "%c0*", nfield->sign);
      mspan = groupsize ;
      for (inidx=0; inidx < zeroes; inidx++) {
         outidx += sprintf(&view_str[outidx], "0");   //lint !e705
         if (groupsize != 0) {
            if (--mspan == 0) {
               //   &&  (uint) expdigits != slen
               outidx += sprintf(&view_str[outidx], " ");
               mspan = groupsize ;
            }
         }
      }
      
      for (inidx=0; inidx < slen; inidx++) {
         outidx += sprintf(&view_str[outidx], "%u", nfield->man[inidx]);   //lint !e705
         // syslog("mspan: %u, inidx: %u, expdigits: %u, slen: %u, groupsize: %u\n", 
         //    mspan, inidx, expdigits, slen, groupsize);
         if (groupsize != 0) {
            if (--mspan == 0) {
               outidx += sprintf(&view_str[outidx], " ");
               mspan = groupsize ;
            }
         }
      }
   }
   //  else expdigits > 0
   // [dump stack 0: 2,+,50: 2,8,9,4,8,2,2,9,6,5,2,2,6,0,2,5,7,0,0,1,5,0,0,1,1,1,3,9,4,3,2,7,8,6,9,1,2,1,3,1,8,2,8,7,8,1,5,4,2,6,]
   //  slen = 50
   //  expdigits = 2
   //  mspan = 2
   
   // [dump stack 0: 4,+,50: 2,3,5,6,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,]
   // X:  +2356 87878 78787 87878 78787 87878 78787 87878 78787 87878 7
   //  slen = 50
   //  expdigits = 4
   //  mspan = 4
   
   //  <func>: [<str> [mspan]: expdigits, sign, digits, man... ]
   //  DNR: [ffvs [2]: 14,+,4: 8,8,3,8,]
   else {
      if (groupsize != 0) {
         mspan = (uint) expdigits % groupsize ;
         if (mspan == 0) {
            mspan = groupsize ;
         }
      }
      else {
         mspan = 0 ;
      }
      // char tstr[20] ;
      // sprintf(tstr, "ffvs [%u, %u, %u]", mspan, slen, groupsize);
      // dump_norm_reg(nfield, tstr);
      
      //  if expdigits > digits, then there won't be any decimal point
      //  represented at all; the difference between the two numbers will
      //  be represented with zeroes.
      if (expdigits > nfield->digits) {
         if (nfield->sign != '+') {
            outidx = sprintf(view_str, "%c", nfield->sign);
         }
         for (inidx=0; inidx < (uint) expdigits; inidx++) {
            if (inidx <= (uint) nfield->digits) {
               outidx += sprintf(&view_str[outidx], "%u", nfield->man[inidx]);   //lint !e705
            }
            else {
               outidx += sprintf(&view_str[outidx], "0");   //lint !e705
            }
            if (groupsize != 0) {
               if (--mspan == 0) {
                  outidx += sprintf(&view_str[outidx], " ");
                  mspan = groupsize ;
               }
            }
         }
      }
      //  view X: : exp:29, s:+, d:400
      else {
         if (nfield->sign != '+') {
            outidx = sprintf(view_str, "%c", nfield->sign);
         }
         for (inidx=0; inidx < slen; inidx++) {
            outidx += sprintf(&view_str[outidx], "%u", nfield->man[inidx]);   //lint !e705
            // syslog("mspan: %u, inidx: %u, expdigits: %u, slen: %u, groupsize: %u\n", 
            //    mspan, inidx, expdigits, slen, groupsize);
            if (groupsize != 0) {
               if (--mspan == 0) {
                  if ((inidx+1) == (uint) expdigits) {
                     if (slen != (uint) expdigits) {
                        outidx += sprintf(&view_str[outidx], "%c", view_dec_point_char);
                     }
                  }
                  else {
                     outidx += sprintf(&view_str[outidx], " ");
                  }
                  mspan = groupsize ;
               }
            }
            //  handle decimal-point char for normal floating point number
            else {
               if ((inidx+1) == (uint) expdigits) {
                  if (slen != (uint) expdigits) {
                     outidx += sprintf(&view_str[outidx], "%c", view_dec_point_char);
                  }
               }
            }
         }
      }
   }
   return (view_str);
}

/*
 *    **************************************************
 *    *                                                *
 *    *             View Register on Screen            *
 *    *                                                *
 *    **************************************************
 */
void view_stack_or_register(uint target)
{
   uint r;
   NORMTYPE *nfield ;
   // char msg[20] ;
   char *regstr ;

   Message("");
   switch (target) {
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
      r = target - IDB_VIEW_R0 ;
      // sprintf(msg, "dump register %u", r);
      nfield = &reg[r];
      regstr = form_full_view_str(nfield);
      view_data_field_full(r, regstr);
      // dump_norm_reg(nfield, msg);
      break ;
      
   case IDB_VIEW_X:
   case IDB_VIEW_Y:
   case IDB_VIEW_Z:
   case IDB_VIEW_T:
      r = target - IDB_VIEW_X ;
      // sprintf(msg, "dump stack %u", r);
      nfield = &stack[r];
      regstr = form_full_view_str(nfield);
      view_data_field_full(r+10, regstr);
      // dump_norm_reg(nfield, msg);
      break ;
      
   default:
      return ;
   }
}

//*********************************************************************
//  Exit from GetX state
//  success: false means data entry was aborted
//*********************************************************************
static void ExitGetXState(bool success)
{
   if (normprec > SIZEOFSMALL) {
      if (success) {
          if (stacklift)
             PushStack();
         MoveWorkStack(0, 0);
         stacklift = true;
         }
      WorkScreen();
   }
   else {
      if (success) {
         MoveWorkStack(0, 0);
         stacklift = true;
      }
      WriteStack(0);
   }
   keyboard_state_set(KBD_STATE_DEFAULT);
}

//***********************************************************************
static void restore_stack0(void)
{
   Message("");
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
   if (keyboard_state_get() == KBD_STATE_DEFAULT) {
      Message("");
      PushStack();   //  push X to Y
      WriteStack(0, 3); //  update display fields
      stacklift = true;
   }
   else {
      if (success) {
         Message("Return/Enter received");
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
}

/*
 *    **************************************************
 *    *                                                *
 *    *   Enter - script version                       *
 *    *                                                *
 *    **************************************************
 */
static void Enter(void)
{
   Message("");
   PushStack();   //  push X to Y
   WriteStack(0, 3); //  update display fields
   stacklift = true;
}

/*
 *    **************************************************
 *    *                                                *
 //  Process string into X stack
 *    *                                                *
 *    **************************************************
 */
static void AcceptXstatic(char *instr)
{
   if (instr == NULL) {
      return ;
   }
   init_getx_vars();
   getx_clear_output_str();
   reset_output_str();
   put_stack(0, " ");
   ClearWork(0);
   
   uint instrlen = strlen(instr);
   uint idx ;
   for (idx=0; idx<instrlen; idx++) {
      int result = ExtendedGetX(*instr++); //  process first input char
      if (!result) {
         ExitGetXState(false);
      }
   }
}

/*
 *    **************************************************
 *    *                                                *
 *    *               Accept X from KBD                *
 //  later: enter GetX state
 *    *                                                *
 *    **************************************************
 */
static void SetupGetX(void)
{
   if (stacklift) {
      PushStack();   //  push X to Y
      WriteStack(1, 3); //  update display fields
   }
   
   init_getx_vars();
   getx_clear_output_str();   //  reset the temp data structs
   reset_output_str();        //  reset the output-field temp string
   put_stack(0, " ");
   ClearWork(0);
   Message("Entering X: S=ChgSign, E=Exp, BakSpc=Backup, Other=Complete, ESC=Exit");
   keyboard_state_set(KBD_STATE_GETX);
   
}

//***************************************************************************
//  called from keyboard_state_handler() in default state
//***************************************************************************
// int main(int argc,char *argv[])
static int EnterGetX(u16 inchr)
{
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
      SetupGetX();           
      
      //  process first input char
      if (!ExtendedGetX(inchr)) {
         ExitGetXState(false);
      }
      //  what is this about?
      if (normprec > SIZEOFSMALL) {
         WorkScreen();
      }
      break;                  

   case (kENTER):        
      Enter(true);   
      break;
      
   //  what is done with ESCAPE, depends upon keyboard state!!
   case (kESC):
      Enter(false);  
      break ;
   
   case (kBSPACE):   //  this has no meaning in default state
   default:          //  unexpected key
      break ;  
   }  /* end switch */

   return 0;
}

//*********************************************************************
static keyboard_state_t keyboard_state = KBD_STATE_DEFAULT ;

int keyboard_state_handler(u16 inchr)
{
   int result = TRUE ;
   switch(keyboard_state) {
   case KBD_STATE_DEFAULT:
      EnterGetX(inchr);
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
      
   case KBD_STATE_GETREG:
      break ;
      
   case KBD_STATE_PASTE_X:
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

//*********************************************************************
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
      
   case KBD_STATE_GETREG:
      show_keyboard_state("Select Register");
      keyboard_state = new_kbd_state ;
      return true ;
      
   case KBD_STATE_PASTE_X:
      show_keyboard_state("Paste value into X");
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
