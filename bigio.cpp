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
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "bigcalc.h"

//lint -esym(715, row, outstr, col) 

/*
 *    **************************************************
 *    *                                                *
 *    *            Source Local Variables              *
 *    *                                                *
 *    **************************************************
 */

// static char color = 0;                    /* Screen Color    */

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

