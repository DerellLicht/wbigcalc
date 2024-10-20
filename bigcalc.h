/*
 *    **************************************************
 *    *                                                *
 *    *                   BIGCALC.H                    *
 *    *                                                *
 *    *          Extended Precision Calculator         *
 *    *                                                *
 *    *     Global Constants & Function Prototypes     *
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
 *    *                   Constants                    *
 *    *                                                *
 *    **************************************************
 */

#define MINPREC 10                        /* Min nbr of digits  */
#define DEFAULTPREC 50                    /* Default precision  */
#define MAXNORM 1075                      /* Max normal digits  */
#define MAXCOMP (MAXNORM + 9)             /* Max compute digits */
#define MAXWORK (MAXCOMP * 2 + 5)         /* Max work digits    */
#define COMPPREC (normprec + 9)           /* Compute digits     */
#define WORKPREC (compprec * 2 + 5)       /* Work digits        */
#define MAXEXDIGITS 9                     /* Max exponent digits */
#define MINEXP -999999999L                /* Min exponent */
#define MAXEXP  999999999L                /* Max exponent */
#define MINFLOATDSP -50L                  /* Min exp float decimal display */
#define MAXFLOATDSP 65L                   /* Max exp float decimal display */
#define SIZEOFSMALL 453                   /* Max size for part screen entry */
#define DISPDECIMAL '.'                   /* Display decimal point */
#define PRINTDECIMAL '*'                  /* Print decimal point */

/*
 *    **************************************************
 *    *                                                *
 *    *                  Data Types                    *
 *    *                                                *
 *    **************************************************
 */

typedef char NORMDIGIT;
typedef int  WORKDIGIT;

typedef struct {
   long exp;
   int sign;
   int digits;
   NORMDIGIT man[MAXNORM];
   } NORMTYPE;

typedef struct {
   long exp;
   int sign;
   int digits;
   WORKDIGIT man[MAXWORK];
   } WORKTYPE;

typedef struct {
   long exp;
   int sign;
   int digits;
   WORKDIGIT man[MAXCOMP];
   } COMPTYPE;

/*
 *    **************************************************
 *    *                                                *
 *    *           External Global Variables            *
 *    *                                                *
 *    **************************************************
 */

extern NORMTYPE
   stack[4],               /* Stack registers  */
   reg[10];                /* Memory registers */

extern WORKTYPE
   work[3];                /* Work registers   */

extern int
   normprec,               /* Normal precision  */
   compprec,               /* Compute precision */
   workprec;               /* Work precision    */
   
extern uint groupsize;              /* Digit group size  */
extern char view_dec_point_char ; //  either '.' or '*'
   // menunbr;                /* Menu number       */
   
// extern long
//    minfloatprn,            /* Min exp for float */
//    maxfloatprn;            /* Max exp for float */

extern bool scinotation;   /* Force sci notation if TRUE   */

/*
 *    **************************************************
 *    *                                                *
 *    *                    Defines                     *
 *    *                                                *
 *    **************************************************
 */

#define GETNORMTEMP(n) (NORMTYPE *) malloc((n) * sizeof(NORMTYPE))
#define GETCOMPTEMP(n) (COMPTYPE *) malloc((n) * sizeof(COMPTYPE))

typedef enum {
   ERROR_UNKNOWN,
   ERROR_OVERFLOW,
   ERROR_DIV_ZERO,
   ERROR_ZERO_ARG,
   ERROR_NEG_ARG,
   ERROR_ARG_NOT_INT,
   ERROR_INV_ARG,
   ERROR_NO_MEMORY,
   ERROR_LIST_END
} error_codes_t ;

void MessageError(error_codes_t ecode);

//  wbigcalc.cpp
void Message(char *msg);
void status_message(char *msgstr);
void status_message(unsigned idx, char *msgstr);
void put_stack(unsigned n, char *msg);
void put_register(unsigned n, char *msg);
void show_hide_buttons(bool show);
void show_hide_all_buttons(bool show, uint except_id);
void view_data_field_full(uint fidx, char *fstr);
void view_norm_reg(NORMTYPE *nptr, char *msg);
void show_hide_view_buttons(bool show);
void enable_paste_field(bool show);
void PasteValue_exec(void);
char *get_paste_str(void);
void show_status_info(void);

//  bigcalc.cpp
void Initialize(void);
void view_stack_or_register(uint target);
void clear_stack_or_register(uint button_code);
void show_hide_view_xchg_buttons(bool show);
void show_hide_view_stor_buttons(bool show);
void show_hide_view_rcall_buttons(bool show);
void ExchangeXReg_exec(uint target);
void StoreX_exec(uint target);
void RecallReg_exec(uint target);
void PasteValueEnable(void);

//  menu jumps
void Power(void);
void SquareRoot(void);
void Square(void);
void Reciprocal(void);
void Factorial(void);
void IntegerPart(void);
void FractionPart(void);
void RecallE(void);
void RecallPi(void);

void Sin(void);
void ArcSin(void);
void Cos(void);
void ArcCos(void);
void Tan(void);
void ArcTan(void);
void Log(void);
void Exp10(void);
void Ln(void);
void ExpE(void);

//  config.cpp
extern bool show_winmsgs ;
LRESULT set_ini_filename(void);
LRESULT read_config_file(void);
LRESULT save_cfg_file(void);

//  options.cpp
void open_options_dialog(HWND hwnd);

//  bigprint.cpp
void reset_output_str(void);
uint get_output_str_len(void);

//  bigmisc.cpp
void getx_clear_output_str(void);

//  bigmisc.cpp - debug dump functions
void dump_norm_reg(NORMTYPE *nptr, char *msg);
void dump_work_reg(WORKTYPE *nptr, char *msg);

//  about.cpp
BOOL CmdAbout(HWND hwnd);

//*********************************************************************************
//  manage keyboard state machine
//  The DOS/console version of BigCalc, had nested keyboard loops for 
//  entering different data elements.  This cannot be done in a message-driven
//  environment such as Windows; a getch() loop will just stall the message thread.
//  
//  This program will use a state machine to determine which keyboard handler
//  to call for keyboard inputs from the message handler.
//*********************************************************************************
typedef enum {
   KBD_STATE_DEFAULT=0,
   KBD_STATE_GETX,
   KBD_STATE_GETREG,
   KBD_STATE_PASTE_X
} keyboard_state_t ;

int keyboard_state_handler(u16 chr);
bool keyboard_state_set(keyboard_state_t new_kbd_state);
keyboard_state_t keyboard_state_get(void);
void show_keyboard_state(char *msg);

//  GetX functions
void init_getx_vars(void);
bool ExtendedGetX(u16 inchr);
int  move_local_to_work0(void);

/*
 *    **************************************************
 *    *                                                *
 *    *                   Key Codes                    *
 *    *                                                *
 *    **************************************************
 */

void Add(void);
void Subtract(void);
void Multiply(void);
void Divide(void);

void ChangeSign(void);
void StoreX(void);
void RecallReg(void);
void SciNotation(void);
void GroupSize(void);
void RecallLastX(void);
void ExchangeXY(void);
void ExchangeXReg(void);
void RollDown(void);
void RollUp(void);

/*
 *    **************************************************
 *    *                                                *
 *    *        BIGMATH.C Function Declarations         *
 *    *                                                *
 *    **************************************************
 */

int ExtendedAdd(void);
int ExtendedSubtract(void);
int ExtendedMultiply(void);
int ExtendedDivide(void);
int ExtendedSquareRoot(void);
int ExtendedPower(void);
int ExtendedSinCos(int scflag);
int ExtendedTan(void);
int ExtendedArcSinCos(int scflag);
int ExtendedArcTan(void);
int ExtendedLog(void);
int ExtendedExp10(void);
int ExtendedLn(void);
int ExtendedExpE(void);
int ExtendedReciprocal(void);
int ExtendedFactorial(void);

/*
 *    **************************************************
 *    *                                                *
 *    *        BIGMISC.C Function Declarations         *
 *    *                                                *
 *    **************************************************
 */

int  Normalize(int w);
int  FlipSign(int sign);
int  ExtendedRound(int w);
int  ExtendedIntegerPart(void);
int  ExtendedFractionPart(void);

void ExtendedInitConstants(void);
void ExtendedRecallPi(int dest);
void ExtendedRecallHalfPi(int dest);
void ExtendedRecallE(int dest);
void ExtendedRecallLn10(int dest);
void ExtendedRecallLnP9(int dest);
void ExtendedRecallLnP99(int dest);
void ExtendedRecallLnP999(int dest);
void ExtendedRecallLnP9999(int dest);
void ExtendedRecallLnP99999(int dest);
void ExtendedRecallSinP1(int dest);
void ExtendedRecallSinP01(int dest);
void ExtendedRecallSinP001(int dest);
void ExtendedRecallCosP1(int dest);
void ExtendedRecallCosP01(int dest);
void ExtendedRecallCosP001(int dest);

void ClearStack(int lo,int hi);
void ClearReg(int lo,int hi);
void ClearWork(int w);
void MoveStackWork(int source,int dest);
void MoveWorkStack(int source,int dest);
void MoveWorkWork(int source,int dest);
void MoveWorkTemp(int source,COMPTYPE *dest);
void MoveTempWork(COMPTYPE *source,int dest);
void MoveTempTemp(COMPTYPE *source,COMPTYPE *dest);
void SetWorkInteger(int dest,long integer);
void SetTempInteger(COMPTYPE *dest,long integer);

/*
 *    **************************************************
 *    *                                                *
 *    *       BIGPRINT.C Function Declarations         *
 *    *                                                *
 *    **************************************************
 */

void WriteReg(int lo,int hi);
void WriteStack(int s);
void WriteStack(int lo,int hi);
void WorkScreen(void);
