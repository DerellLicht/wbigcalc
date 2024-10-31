//*********************************************************************************************
//  This data should be updated in readme.md as well
//  
//  version    changes
//   =======   ======================================
//    6.00     Initial release
//    6.01     - Added Paste option to paste external string into X Stack
//             - Added About dialog
//             - Added program info in internal data
//    6.02     - Update help file with info on special functions and INI file
//             - "Paste X" button needs to be hidden when other special functions are active
//             - 'About' dialog: add link to Github page, as well as home page
//    6.03     - Convert error functions to common error handler
//             - Change Msg field colors for error messages
//    6.04     - Increase max number of displayed digits to 150, to fit dialog data fields
//             - Code optimization: eliminate double-menu jumps in keyboard handler
//    6.05     Revise some display constants, to allow for wider display fields
//    6.06     Debug: Display stack/register data in status bar, on View command
//    6.07     Make naming consistent: wbigcalc, *not* bigcalc
//    6.08     Add mechanism to allow reading X from file, for very-large-number handling
//*********************************************************************************************
#define VerNum    "V6.08"

