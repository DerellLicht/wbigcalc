<ins>WBigCalc 6.0 - BigCalc ported to 32-bit Windows utility</ins>

WBigCalc is an extended-precision scientific calculator, supporting numbers up to 1075 digits.  
WBigCalc uses Reverse Polish Notation (RPN), as originally used in HP calculators;  
RPN is an efficient method for handling complex computations,
but is rarely seen in pocket calculators any more.

<hr>

### Notes about large numbers of digits
If number of digits is larger than about 200, the stack displays will only display
first 200 characters; that is all the digits that fit on the display line.
However, all the specified digits will still be displayed in the View window, 
and all computations will be done with the full number of requested digits.

The original MSDOS version of bigcalc, handled data display differently 
if number of digits was greater than SIZEOFSMALL (453).
I haven't attempted to experiment with numbers that large, so this Windows version
of BigCalc isn't going to handle those inputs in a sensible way.

In practice, at this point, manual inputs are limited to about 200 characters
(the size of the X field, in PasteX mode).  Computations can go larger than that,
but past 453 digits, the program won't be able to display the numbers without modification.
Note that the *computations* will probably still work fine, since Judson McClendon's
math functions are unchanged in this program; just displays will need to be revised.

### Program modifications
- Removed all 'print to printer/file' routines from Windows version.
I don't really think those operations are required in a calculator such as this.
Instead, user can copy-and-paste from either X-stack field or View window.

- Removed add/subtract/multiply/divide with register commands, since they 
require <code>GetCh()</code> operations which are awkward to implement in Windows,
and I don't think they are very important.  
They could be added later if someone wants to; use <code>ExchangeXReg()</code> as reference.

### History
I previously ported BigCalc to 32-bit Windows console utility, and it still works great!  
Executable package is available at https://derelllicht.42web.io/bigcalc.html  
Source code is maintained at https://github.com/DerellLicht/bigcalc

### Original author and notice
The original program was written by Judson D. McClendon for MSDOS, ca 1999.  
Neither the author nor the original program appear to be present on the internet any more.

```
 Judson D. McClendon           $20 gets you a disk with the complete C source.  
 Sun Valley Systems  
 4522 Shadow Ridge Pkwy        There is no warranty of any kind.  
 Pinson, AL 35126-2192         The author assumes no responsibility for the  
     205-680-0460              use of this program.  
```

#### Building the application
This application is built using the MinGW toolchain; 
I recommend the [TDM](http://tdm-gcc.tdragon.net/) distribution, 
to avoid certain issues with library accessibility. 
The makefile also requires Gnu version of Make utility.

#### _Revision history_

| Version | Date |description |
| :---: | :---: | --- |
| 6.00 | 10/08/24 |Original release |
| 6.01 | 10/17/24 |- Added Paste option to paste external string into X Stack |
|      |          |- Added About dialog |
|      |          |- Added program info in internal data |
| 6.02 | 10/18/24 |- Update help file with info on special functions and INI file |
|      |          |- "Paste X" button needs to be hidden when other special functions are active |
|      |          |- 'About' dialog: add link to Github page, as well as home page |
| 6.03 | 10/19/24 |- Convert error functions to common error handler |
|      |          |- Change Msg field colors for error messages |
| 6.04 | 10/19/24 |- Increase max number of displayed digits to 150, to fit dialog data fields |
|      |          |- Code optimization: eliminate double-menu jumps in keyboard handler |
| 6.05 | 10/20/24 |Revise some display constants, to allow for wider display fields |
| 6.06 | 10/24/24 |Debug: Display stack/register data in status bar, on View command |
| 6.07 | 10/29/24 |Make naming consistent: wbigcalc, *not* bigcalc |
| 6.08 | 11/03/24 |- Add mechanism to allow reading X from file, for very-large-number handling |
|      |          |- Add groupsize=0 option for Viewing large numbers without gaps |
|      |          |- Fix bug: computations and number displays were truncated to normprec |
|      |          |- Implement test vectors for common functions and operations |
| 6.09 | 11/03/24 |Remove ReadXFile button, superceded by ReadScript |
| 6.10 | 05/22/25 |Update help file to discuss script files |
