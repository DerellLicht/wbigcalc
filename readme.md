<ins>WBigCalc 6.0 - BigCalc ported to 32-bit Windows utility</ins>

BigCalc is an extended-precision scientific calculator, supporting numbers up to 1075 digits.  
BigCalc uses Reverse Polish Notation (RPN), as originally used in HP calculators;  
RPN is an efficient method for handling complex computations,
but is rarely seen in pocket calculators any more.

The original readme file is included as bigcalc.txt

The original program was written by Judson D. McClendon for MSDOS, ca 1999.  
Neither the author nor the original program appear to be present on the internet any more.

<hr>

Status
The Windows version of BigCalc is currently under development.  
The main challenge that I am currently facing, is that  the program uses getch() loops in several places, to manage keyboard acquisition.  This technique is incompatible with Windows programs, which use a message handler for all inputs; calling getch() or any other polling function, just stalls the message handler.  
I will need to re-factor the keyboard handler, to avoid getch(), which will likely be a great deal of work...

I previously ported BigCalc to 32-bit Windows console utility, and it still works great!
It is available at https://github.com/DerellLicht/bigcalc





