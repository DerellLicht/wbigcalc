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
if number of digits was greater than <code>SIZEOFSMALL</code> (453 digits).
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

- Removed add/subtract/multiply/divide-with-register commands, since they 
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

<hr>

#### building the application
This application is built using the MinGW toolchain; 
I recommend the [TDM](http://tdm-gcc.tdragon.net/) distribution, 
to avoid certain issues with library accessibility. 
The makefile also requires certain Cygwin tools (rm, make, etc).

#### NOTE: this program requires my ```der_libs``` submodule
If you clone the repository without the --recursive flag, 
you can recover the submodule later, with this command:

```git submodule update --init --recursive```

<hr>

This project is licensed under _Creative Commons CC0 1.0 Universal_   

See the [LICENSE](LICENSE.txt) file for detailed information about this license  
  
<hr>

See [Changelog](CHANGELOG.md) for the full revision history.
  
