<ins>WBigCalc 6.0 - BigCalc ported to 32-bit Windows utility</ins>

BigCalc is an extended-precision scientific calculator, supporting numbers up to 1075 digits.  
BigCalc uses Reverse Polish Notation (RPN), as originally used in HP calculators;  
RPN is an efficient method for handling complex computations,
but is rarely seen in pocket calculators any more.

<hr>

### Pending issues

- Store registers in INI file?  Maybe stack as well?

- Although the original BigCalc actually supported numbers up to 1075 digits, 
  this Windows program doesn't really support that many digits, simply because 
  I haven't yet implemented any way to display such numbers.  
  In practice, numbers above 66 digits get converted to scientific notation.  

### Program modifications
**DDM 09/10/24**
Removed all 'print to printer/file' routines from Windows version.
I don't really think those operations are required in a calculator such as this.
Instead, I will try to make it possible to copy-and-paste from any display windows.

**DDM 10/03/24**
Removed add/subtract/multiply/divide with register commands, since they 
require GetCh() operations which are awkward to implement in Windows,
and I don't think they are very important.  
They could be added later if someone wants to; use ExchangeXReg() as reference.

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



