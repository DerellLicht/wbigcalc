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
#define XSIGNROW 18                       /* X sign row */
#define ENTRYSIGNROWBIG 4                 /* Sign row for big X */
#define ENTRYSIGNROWSMALL 18              /* Sign row for small X */
#define SIZEOFSMALL 453                   /* Max size for part screen entry */
#define SIGNDISPCOL 3                     /* Sign column for number display */
#define MINDISPCOL 4                      /* Min column for number display */
#define MAXDISPCOL 68                     /* Max column for number display */
#define SIGNPRTCOL 4                      /* Sign column for number printer */
#define MINPRTCOL 5                       /* Min column for number printer */
#define MAXPRTCOL 63                      /* Max column for number printer */
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

//  bigcalc.cpp
void Initialize(void);
int dos_main(u16 inchr);
void view_stack_or_register(uint target);
void clear_stack_or_register(uint button_code);
void show_hide_view_xchg_buttons(bool show);
void show_hide_view_stor_buttons(bool show);
void show_hide_view_rcall_buttons(bool show);
void ExchangeXReg_exec(uint target);
void StoreX_exec(uint target);
void RecallReg_exec(uint target);
void PasteValueEnable(void);

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

#define ESCAPE     27   /* Escape          */
#define BACKSPACE  8    /* Back Space      */
// #define CR         '\r' /* Carriage Return */
// #define LF         '\n' /* Line Feed       */
#define FF         '\f' /* Form Feed       */
#define PGUP       1073 /* PgUp            */
#define PGDN       1081 /* PgDn            */

#define ADD        43   /*  +          */
#define SUBTRACT   45   /*  -          */
#define MULTIPLY   42   /*  *          */
#define DIVIDE     47   /*  /          */
#define LASTX      76   /*  L          */
#define CLEAR      67   /*  C          */
#define SCINOT     70   /*  F          */
#define XCHGXREG   88   /*  X          */
#define CHGSIGN    83   /*  S          */
#define GROUPSIZE  71   /*  G          */
#define VIEWREG    86   /*  V          */
#define STOREX     1073 /* PgUp        */
#define RECALLREG  1081 /* PgDn        */
#define ENTER      13   /* Enter       */
#define XCHGXY1    1075 /* Left Arrow  */
#define XCHGXY2    1077 /* Right Arrow */
#define ROLLDOWN   1080 /* Down Arrow  */
#define ROLLUP     1072 /* Up Arrow    */
#define HELP       1059 /* F1          */
#define POWER      1060 /* F2          */
#define SQUAREROOT 1061 /* F3          */
#define SQUARE     1062 /* F4          */
#define RECIPROCAL 1063 /* F5          */
#define FACTORIAL  1064 /* F6          */
#define INTEGER    1065 /* F7          */
#define FRACTION   1066 /* F8          */
#define RECALLPI   1067 /* F9          */
#define RECALLE    1068 /* F10         */
#define SIN        1094 /* CTRL-F1     */
#define ARCSIN     1095 /* CTRL-F2     */
#define COS        1096 /* CTRL-F3     */
#define ARCCOS     1097 /* CTRL-F4     */
#define TAN        1098 /* CTRL-F5     */
#define ARCTAN     1099 /* CTRL-F6     */
#define LOG        1100 /* CTRL-F7     */
#define EXP10      1101 /* CTRL-F8     */
#define LN         1102 /* CTRL-F9     */
#define EXPE       1103 /* CTRL-F10    */

/*
 *    **************************************************
 *    *                                                *
 *    *        BIGMATH.C Function Declarations         *
 *    *                                                *
 *    **************************************************
 */

extern  int ExtendedAdd(void);
extern  int ExtendedSubtract(void);
extern  int ExtendedMultiply(void);
extern  int ExtendedDivide(void);
extern  int ExtendedSquareRoot(void);
extern  int ExtendedPower(void);
extern  int ExtendedSinCos(int scflag);
extern  int ExtendedTan(void);
extern  int ExtendedArcSinCos(int scflag);
extern  int ExtendedArcTan(void);
extern  int ExtendedLog(void);
extern  int ExtendedExp10(void);
extern  int ExtendedLn(void);
extern  int ExtendedExpE(void);
extern  int ExtendedReciprocal(void);
extern  int ExtendedFactorial(void);



/*
 *    **************************************************
 *    *                                                *
 *    *        BIGMISC.C Function Declarations         *
 *    *                                                *
 *    **************************************************
 */

extern  int Normalize(int w);
extern  int FlipSign(int sign);
extern  int ExtendedRound(int w);
extern  int ExtendedIntegerPart(void);
extern  int ExtendedFractionPart(void);

extern  void ExtendedInitConstants(void);
extern  void ExtendedRecallPi(int dest);
extern  void ExtendedRecallHalfPi(int dest);
extern  void ExtendedRecallE(int dest);
extern  void ExtendedRecallLn10(int dest);
extern  void ExtendedRecallLnP9(int dest);
extern  void ExtendedRecallLnP99(int dest);
extern  void ExtendedRecallLnP999(int dest);
extern  void ExtendedRecallLnP9999(int dest);
extern  void ExtendedRecallLnP99999(int dest);
extern  void ExtendedRecallSinP1(int dest);
extern  void ExtendedRecallSinP01(int dest);
extern  void ExtendedRecallSinP001(int dest);
extern  void ExtendedRecallCosP1(int dest);
extern  void ExtendedRecallCosP01(int dest);
extern  void ExtendedRecallCosP001(int dest);

extern  void ClearStack(int lo,int hi);
extern  void ClearReg(int lo,int hi);
extern  void ClearWork(int w);
extern  void MoveStackWork(int source,int dest);
extern  void MoveWorkStack(int source,int dest);
extern  void MoveWorkWork(int source,int dest);
extern  void MoveWorkTemp(int source,COMPTYPE *dest);
extern  void MoveTempWork(COMPTYPE *source,int dest);
extern  void MoveTempTemp(COMPTYPE *source,COMPTYPE *dest);
extern  void SetWorkInteger(int dest,long integer);
extern  void SetTempInteger(COMPTYPE *dest,long integer);



/*
 *    **************************************************
 *    *                                                *
 *    *       BIGPRINT.C Function Declarations         *
 *    *                                                *
 *    **************************************************
 */

extern  void WriteReg(int lo,int hi);
extern  void WriteStack(int s);
extern  void WriteStack(int lo,int hi);
extern  void Overflow(void);
extern  void DivideByZero(void);
extern  void ZeroArgument(void);
extern  void NegativeArgument(void);
extern  void ArgumentNotInteger(void);
extern  void ArgumentInvalid(void);
extern  void MemoryError(void);

extern  void show_status_info(void);
extern  void WorkScreen(void);
