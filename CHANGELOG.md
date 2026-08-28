# wbigcalc Changelog

## [6.11] - 2026-08-27
- Include wbigcalc.ini in distribution, to provide register-value examples

## [6.10] - 2026-05-22
- Update help file to discuss script files

## [6.09] - 2024-11-03
- Remove ReadXFile button, superceded by ReadScript

## [6.08] - 2024-10-31
- Add mechanism to allow reading X from file, for very-large-number handling
- Add groupsize=0 option for Viewing large numbers without gaps
- Fix bug: computations and number displays were truncated to normprec
- Implement test vectors for common functions and operations

## [6.07] - 2024-10-29
- Make naming consistent: wbigcalc, *not* bigcalc

## [6.06] - 2024-10-24
- Debug: Display stack/register data in status bar, on View command

## [6.05] - 2024-10-24
- Revise some display constants, to allow for wider display fields

## [6.04] - 2024-10-19
- Increase max number of displayed digits to 150, to fit dialog data fields
- Code optimization: eliminate double-menu jumps in keyboard handler

## [6.03] - 2024-10-19
- Convert error functions to common error handler
- Change Msg field colors for error messages

## [6.02] - 2024-10-18
- Update help file with info on special functions and INI file
- "Paste X" button needs to be hidden when other special functions are active
- 'About' dialog: add link to Github page, as well as home page

## [6.01] - 2024-10-17
- Added Paste option to paste external string into X Stack
- Added About dialog
- Added program info in internal data

## [6.00] - 2024-09-24
- Initial release
