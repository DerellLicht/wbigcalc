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

#define TITLE "Extended Precision Calculator 6.0"

// #define REALMODE                          /* Remove for OS/2 protected mode */

#define MINPREC 3                         /* Min nbr of digits  */
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

//  colors
#define TITLE_ATTR   0x1B
#define HEADER_ATTR  0x0B
#define TEXT_ATTR    3
#define HELP_ATTR    2
#define LOGO_ATTR    6
#define REG_ATTR     2
#define MENU_ATTR    3

// extern char tempstr[1024] ;
/*
 *    **************************************************
 *    *                                                *
 *    *                    Defines                     *
 *    *                                                *
 *    **************************************************
 */

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

//lint -esym(652, BOOLEAN)
#ifndef BOOLEAN
#define BOOLEAN int
#endif

#define GETNORMTEMP(n) (NORMTYPE *) malloc((n) * sizeof(NORMTYPE))
#define GETCOMPTEMP(n) (COMPTYPE *) malloc((n) * sizeof(COMPTYPE))

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
#define PRINT      80   /*  P          */
#define SCINOT     70   /*  F          */
#define XCHGXREG   88   /*  X          */
#define CHGSIGN    83   /*  S          */
#define PRINTDISK  68   /*  D          */
#define GROUPSIZE  71   /*  G          */
#define MENUROLL   77   /*  M          */
#define VIEWREG    86   /*  V          */
#define CLEARX     8    /* Back Space  */
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
 *    *                  Data Types                    *
 *    *                                                *
 *    **************************************************
 */
//lint -esym(1709, NORMDIGIT, WORKDIGIT, NORMTYPE, WORKTYPE, COMPTYPE)

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
 *    *         BIGIO.C Function Declarations          *
 *    *                                                *
 *    **************************************************
 */

int GetChar(void);
// int KeyPressed(void);

void ScrClr(void);
void EraEol(void);
void EraEop(void);
void CurPos(int row,int col);
void CurGet(int *row,int *col);
void WChar(int chr);
void WString(char *str);
void WInteger(long integer);
void WriteAt(int row,int col,char *str);
void WriteCenter(int row,char *msg);
void Message(char *msg);
void MessageWait(char *msg);
void MessageEsc(char *msg);

void DisplayChar(int *row,int *col,int chr);
void BackSpace(int *row,int *col);
void DisplayExpChar(int *row,int *col,int chr);
void BackSpaceExp(int *row,int *col);
void DisplayExp(int *row,int *col,int exprow,int expcol,int expsign,long exponent);

// void PChar(int chr);
// void PString(char *str);
// void PInteger(long integer);
// void NewLine(void);
void NewPage(void);

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

int Normalize(int w);
int FlipSign(int sign);
int ExtendedRound(int w);
int ExtendedIntegerPart(void);
int ExtendedFractionPart(void);
int ExtendedGetX(unsigned chr);

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
void MoveRegWork(int source,int dest);
void MoveWorkStack(int source,int dest);
void MoveWorkReg(int source,int dest);
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

// void PrintHeading(void);
// void PrintReg(int lo,int hi);
// void PrintStack(int lo,int hi);
void DisplayReg(int r);
void DisplayStack(int s);
void DumpStack(void);   //  debug function

void WriteReg(int lo,int hi);
void WriteStack(int lo,int hi);
// int  GetPrompt(void);
void Overflow(void);
void DivideByZero(void);
void ZeroArgument(void);
void NegativeArgument(void);
void ArgumentNotInteger(void);
void ArgumentInvalid(void);
void MemoryError(void);

void OnScreenMenu(void);
void HelpScreen(void);
void Heading2(void);
void WorkScreen(void);

//  wbigcalc.cpp 
// #define  regT  (unsigned) 3
// #define  regZ  (unsigned) 2
// #define  regY  (unsigned) 1
// #define  regX  (unsigned) 0
void put_stack(unsigned n);
void put_register(unsigned n);
void put_work(void);
char *get_iostr(void);
void reset_iostr(void);
void init_work_variables(void);
extern unsigned regX_edit_active ;

