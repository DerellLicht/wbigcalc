// 
//lint -e10   Expecting identifier or other declarator
//lint -e129  declaration expected, identifier 'ICON' ignored
//lint -e525  Negative indentation from line ...
//lint -e539  Did not expect positive indentation from line ...
//lint -e534  Ignoring return value of function ...
//lint -e1065 Symbol not declared as "C" conflicts with line nnn, file whatever
//lint -e1784 Symbol previously declared as "C"
//lint -e818  Pointer parameter could be declared as pointing to const
//lint -e1776 Converting a string literal to char * is not const safe (arg. no. 1)
//lint -e755  global macro not referenced

//lint -esym(552, menucleared)  Symbol not accessed

//  not-yet-completed functions; this should be resolved later
//lint -e522   Highest operation, functions, lacks side-effects

//lint -e845  The right argument to operator '|' is certain to be 0
//lint -e840  Use of nul character in a string literal

//lint -e1051 Symbol 'get_system_message' is both a function and a variable
//lint -e1066 Symbol declared as "C" conflicts with line ..., file ...

//  system-header refs
//lint -esym(36, TCHAR)  redefining the storage class of symbol 'TCHAR' (static data vs. type)
//lint -e18
//lint -esym(31, TCHAR)  

//lint -e713  Loss of precision (arg. no. 3) (unsigned int to int)
//lint -e732  Loss of sign (assignment) (int to unsigned int)
//lint -e737  Loss of sign in promotion from int to unsigned long
