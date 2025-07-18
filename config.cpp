//****************************************************************************
//  Copyright (c) 2009  Derell Licht
//  config.cpp - manage configuration data file
//
//  Filename will be same as executable, but will have .ini extensions.
//  Config file will be stored in same location as executable file.
//  Comments will begin with '#'
//  First line:
//  device_count=%u
//  Subsequent file will have a section for each device.
//****************************************************************************
#include <windows.h>
#include <stdio.h>   //  fopen, etc
#include <stdlib.h>  //  atoi()

#include "common.h"
#include "bigcalc.h"

//lint -esym(759, show_winmsgs)
//lint -esym(765, show_winmsgs)
//lint -esym(843, show_winmsgs)
bool show_winmsgs = false ;

//  bigmisc.cpp
extern void store_regs_in_ini(FILE *fd);
extern void recall_reg_from_ini(char *instr);

//****************************************************************************
static char ini_name[MAX_PATH_LEN+1] = "" ;

//****************************************************************************
static void strip_comments(char *bfr)
{
   char *hd = strchr(bfr, '#') ;
   if (hd != 0)
      *hd = 0 ;
}

//****************************************************************************
LRESULT set_ini_filename(void)
{
   LRESULT result = derive_filename_from_exec(ini_name, (char *) ".ini") ; //lint !e1773 const
   if (result != 0) {
      syslog("%s open: %s\n", ini_name, get_system_message(result)) ;
   }
   if (show_winmsgs) {
      syslog("ini file: %s\n", ini_name);
   }
   return result;
}

//****************************************************************************
static LRESULT save_default_ini_file(void)
{
   FILE *fd = fopen(ini_name, "wt") ;
   if (fd == 0) {
      LRESULT result = (LRESULT) GetLastError() ;
      syslog("%s open: %s\n", ini_name, get_system_message(result)) ;
      return result;
   }
   //  save any global vars
   fprintf(fd, "normprec=%d\n", normprec) ;
   fprintf(fd, "groupsize=%u\n", groupsize) ;
   fprintf(fd, "vdec_char=%c\n", view_dec_point_char) ;
   fprintf(fd, "scinotation=%u\n", (scinotation) ? 1U : 0U) ;
   fprintf(fd, "winmsgs=%u\n", (show_winmsgs) ? 1U : 0U) ;
   
   store_regs_in_ini(fd);
   fclose(fd) ;
   return ERROR_SUCCESS;
}

//****************************************************************************
LRESULT save_cfg_file(void)
{
   return save_default_ini_file() ;
}

//****************************************************************************
//  - derive ini filename from exe filename
//  - attempt to open file.
//  - if file does not exist, create it, with device_count=0
//    no other data.
//  - if file *does* exist, open/read it, create initial configuration
//****************************************************************************
LRESULT read_config_file(void)
{
   char inpstr[4000] ;
   uint uvalue ;
   FILE *fd = fopen(ini_name, "rt") ;
   if (fd == 0) {
      return save_default_ini_file() ;
   }

   while (fgets(inpstr, sizeof(inpstr), fd) != 0) {
      strip_comments(inpstr) ;
      strip_newlines(inpstr) ;
      if (strlen(inpstr) == 0)
         continue;

      if (strncmp(inpstr, "normprec=", 9) == 0) {
         // syslog("enabling factory mode\n") ;
         normprec = (int) strtoul(&inpstr[9], 0, 0) ;
         compprec = COMPPREC;
         workprec = WORKPREC;
      } else
      if (strncmp(inpstr, "groupsize=", 10) == 0) {
         groupsize = (uint) strtoul(&inpstr[10], 0, 0) ;
      } else
      if (strncmp(inpstr, "vdec_char=", 10) == 0) {
         view_dec_point_char = inpstr[10] ;
      } else
      if (strncmp(inpstr, "scinotation=", 12) == 0) {
         uvalue = (uint) strtoul(&inpstr[12], 0, 0) ;
         scinotation = (uvalue == 0) ? false : true ;
      } else
      if (strncmp(inpstr, "winmsgs=", 8) == 0) {
         uvalue = (uint) strtoul(&inpstr[8], 0, 0) ;
         show_winmsgs = (uvalue == 0) ? false : true ;
      } else
      //  parse register record
      if (inpstr[0] == 'R'  &&  inpstr[2] == ':') {
         recall_reg_from_ini(inpstr);
      } else
      {
         syslog("unknown: [%s]\n", inpstr) ;
      } 
   }
   fclose(fd);
   return 0;
}

