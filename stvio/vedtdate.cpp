/***************************************************************************/
/* F r e e C o d e                                                         */
/*                                                                         */
/* THIS CODE IS FREEWARE AND AS SUCH YOU ARE PERMITTED TO DO WHAT YOU WISH */
/* WITH IT. THESE PROGRAMS ARE PROVIDED AS IS WITHOUT ANY WARRANTY,        */
/* EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO FITNESS FOR ANY      */
/* PARTICULAR PURPOSE.                                                     */
/*                                                                         */
/* However, we would appriciate if you shared any enhancements to us       */
/* Please send them to www.jmast.se/free/ and we will include them in      */
/* future distributions.                                                   */
/*                                                                         */
/***************************************************************************/
// *********************************************************************
// * EDITDATE.MOD
// *********************************************************************
// * FUNCTION EditDate%
// *
// *********************************************************************
// * 890328: Created
// *--------------------------------------------------------------------
// * 900629: Changed for New update of STLIB7
// *--------------------------------------------------------------------
// * 900630: Exit with <-/-> removed. Bugs in EditTime fixed
// *--------------------------------------------------------------------
// * 900706: EditString using string, full mouse support
// *--------------------------------------------------------------------
// * 900907: EditTime changed to HH/MM (seconds skipped)
// *--------------------------------------------------------------------
// * 921127: Mouse support removed
// *--------------------------------------------------------------------
// * 930419: Change for new color settings
// *--------------------------------------------------------------------
// * 930426: Fixed ; for print on line 24,25
// *--------------------------------------------------------------------
// * 930603: Fixed bug that did NOT allow 1900/2000 (00 year)
// *--------------------------------------------------------------------
// * 930610: Fixed colours for new field look and added IDHPSW.
// *--------------------------------------------------------------------
// * 931107: Split into three files
// *--------------------------------------------------------------------
// * 980302: Ported to C (stlibvio)
// *********************************************************************
#include "stlibvio.h"

DECLARE FUNCTION ESChkDateRet% (ret%, row%, col%, fieldid%, retur%(), quit%)


// ---------------------------------------------------------------------
//  VioEditDate:
// ---------------------------------------------------------------------
int VioEditDate (PSTVIOWINDOW hwnd, char *szDate, int iRow, int iCol, int aiRet[], int iRetLen)
    {
    int         iCurRow, iCurCol, iCurShow;
    int         iCounter, iLength;
    int         iKey, iRc;
    char        szBuffer[255];
    BOOL        bQuit = FALSE;
    char        szYear[5], szMonth[2], szDay[2];


    iLength = 10;
    ValidPos(hwnd, &iRow, &iCol, &iLength);
    if (iLength != 10)
       return (0);

    year$ = RIGHT$(DATE$, 2)
    month$ = LEFT$(DATE$, 2)
    day$ = MID$(DATE$, 4, 2)
    baddate = FALSE

    if ( strlen(szDate) == 8)
       {
       temp$ = LEFT$(eddate$, 2)
       year$ = temp$
       temp$ = MID$(eddate$, 3, 2)
       IF VAL(temp$) > 0 AND VAL(temp$) < 13 THEN
          month$ = temp$
       END IF
       temp$ = RIGHT$(eddate$, 2)
       IF VAL(temp$) > 0 AND VAL(temp$) < 32 THEN
          day$ = temp$
       END IF
       }
     else
       {
       // dagens datum
       }
            IF StrTrim$(st.iDateDelim) = "" THEN
               st.iDateDelim = "/"
            END IF

            COLOR st.iGloefcolor, st.iGloebcolor

            LOCATE row, col
            PRINT year$; st.iDateDelim; month$; st.iDateDelim; day$;
            REDIM ret(UBOUND(retur) + 3)
            FOR counter = 1 TO UBOUND(retur)
                ret(counter) = retur(counter)
            NEXT counter
            ret(UBOUND(retur) + 1) = KEYXLAST
            ret(UBOUND(retur) + 2) = KEYENTER
            ret(UBOUND(retur) + 3) = KEYTAB
            quit = FALSE
            fieldid = 1
            DO
               SELECT CASE fieldid
                      CASE 1
                           quit1 = FALSE
                           DO
                              old$ = year$
                              ret = EditString(year$, row, col, 2, ret(), IDHDATE)
                              IF VAL(year$) > 0 THEN
                                 quit1 = TRUE
                              ELSE
                                 year$ = old$
                                 BEEP
                              END IF
                           LOOP UNTIL quit1 = TRUE
                      CASE 2
                           quit1 = FALSE
                           DO
                              old$ = month$
                              ret = EditString(month$, row, col + 3, 2, ret(), IDHDATE)
                              IF VAL(month$) > 0 AND VAL(month$) < 13 THEN
                                 quit1 = TRUE
                              ELSE
                                 month$ = old$
                                 BEEP
                              END IF
                           LOOP UNTIL quit1 = TRUE
                      CASE 3
                           quit1 = FALSE
                           DO
                              old$ = day$
                              ret = EditString(day$, row, col + 6, 2, ret(), IDHDATE)
                              IF VAL(day$) > 0 AND VAL(day$) < 32 THEN
                                 quit1 = TRUE
                              ELSE
                                 day$ = old$
                                 BEEP
                              END IF
                           LOOP UNTIL quit1 = TRUE
                      CASE ELSE
               END SELECT

               COLOR st.iGloefcolor, st.iGloebcolor
               LOCATE row, col
               PRINT year$; st.iDateDelim; month$; st.iDateDelim; day$;

               EditDate = ESChkDateRet(ret, row, col, fieldid, retur(), quit)

               IF fieldid > 3 THEN
                  fieldid = 3
               ELSEIF fieldid < 1 THEN
                  fieldid = 1
               END IF
            LOOP WHILE quit = FALSE
            eddate$ = year$ + month$ + day$

            COLOR st.iGloffcolor, st.iGlofbcolor

            LOCATE row, col
            PRINT year$; st.iDateDelim; month$; st.iDateDelim; day$;

        END FUNCTION

