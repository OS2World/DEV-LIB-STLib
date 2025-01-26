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
//     *********************************************************************
//     * veditstr.cpp
//     *********************************************************************
//     * 890328: Created
//     *--------------------------------------------------------------------
//     * 900629: Changed for New update of STLIB7
//     *--------------------------------------------------------------------
//     * 900630: Exit with <-/-> //oved. Bugs in EditTime fixed
//     *--------------------------------------------------------------------
//     * 900706: EditString using string, full mouse support
//     *--------------------------------------------------------------------
//     * 900907: EditTime changed to HH/MM (seconds skipped)
//     *--------------------------------------------------------------------
//     * 921127: Mouse support //oved
//     *--------------------------------------------------------------------
//     * 930419: Change for new color settings
//     *--------------------------------------------------------------------
//     * 930426: Fixed ; for print on line 24,25
//     *--------------------------------------------------------------------
//     * 930603: Fixed bug that did NOT allow 1900/2000 (00 year)
//     *--------------------------------------------------------------------
//     * 930610: Fixed colours for new field look and added IDHPSW.
//     *--------------------------------------------------------------------
//     * 931107: Split into three files
//     *--------------------------------------------------------------------
//     * 980302: Ported to C (stlibvio)
//     *********************************************************************
#include "stlibvio.h"


int ESHook(int iHook, int iKey, int iCol, char *szString);
// ---------------------------------------------------------------------
//  VioEditString: Edit a string in a vio window.
// ---------------------------------------------------------------------
int VioEditString (PSTVIOWINDOW hwnd, char *szInString, int iRow, int iColHome, int iLength, int aiRet[], int iRetLen, int iHook)
    {
    BOOL        bRedraw = TRUE;
    BOOL        bQuit = FALSE;
    char        szString[255];
    char        szBlank[255];
    int         iRc;
    int         iCol, iColEnd, iStrPos;
    int         kbd;
    int         iCounter;
    int         iCurRow, iCurCol, iCurShow;


    memset(szBlank, ' ', 255);
    memset(szString, ' ', 255);

    ValidPos(hwnd, &iRow, &iColHome, &iLength);

    VioGetCursor (hwnd, &iCurRow, &iCurCol, &iCurShow);
    VioSetCursor(hwnd, iRow, iColHome, TRUE);

    memcpy(szString, szInString, strlen(szInString));

    szString[iLength] = '\0';
    szBlank[iLength] = '\0';

    iCol = iColHome;
    iColEnd = iColHome + iLength + 1;
    iStrPos = 0;

    while (bQuit == FALSE)
          {
          if (bRedraw == TRUE)
             {
             if (iHook != IDH_PSW )
                VioEditPrint(hwnd, iRow, iColHome, szString);
             else
                VioEditPrint(hwnd, iRow, iColHome, szBlank);
             bRedraw = FALSE;
             }

          VioSetCursor(hwnd, iRow, iCol, TRUE);
          kbd = VioGetKey (hwnd, TRUE, FALSE);
          kbd = ESHook(iHook, kbd, iCol, szString);

          switch (kbd)
                 {
                 case KEY_RARROW:
                      iStrPos++;
                      break;

                 case KEY_LARROW:
                      iStrPos--;
                      break;

                 case KEY_HOME:
                      for (iCounter = 0; iCounter < iLength; iCounter++)
                          {
                          if (szString[iCounter] > 32)
                             break;
                          }
                      if (iCounter < iLength - 1)
                         iStrPos = iCounter;
                      else
                         iStrPos = 0;
                      break;

                 case KEY_END:
                      for (iCounter = iLength - 1; iCounter >= 0; iCounter--)
                          if (szString[iCounter] > 32)
                             break;
                      iStrPos = iCounter + 1;
                      break;

                 case KEY_DEL:
                      if (iStrPos == (iLength - 1))
                         szString[iLength - 1] = ' ';
                      else
                         {
                         for (iCounter = iStrPos; iCounter < (iLength - 1); iCounter++)
                             szString[iCounter] = szString[iCounter + 1];
                         szString[iLength - 1] = ' ';
                         }
                      bRedraw = TRUE;
                      break;

                 case KEY_BKSP:
                      if (iStrPos > 0)
                         {
                         // loop and take away character
                         iStrPos -= 1;
                         for (iCounter = iStrPos; iCounter < (iLength - 1); iCounter++)
                             szString[iCounter] = szString[iCounter + 1];
                         szString[iLength - 1] = ' ';
                         bRedraw = TRUE;
                         }
                      break;

                 default:
                      if (kbd > 31)
                         {
                         for (iCounter = iLength - 1; iCounter > iStrPos; iCounter--)
                             szString[iCounter] = szString[iCounter - 1];
                         szString[iStrPos] = kbd;
                         iStrPos++;
                         bRedraw = TRUE;
                         }
                      else
                         {
                         for (iCounter = 0; iCounter < iRetLen; iCounter++)
                             {
                             if (aiRet[iCounter] == kbd)
                                {
                                iRc = kbd;
                                bQuit = TRUE;
                                break;
                                }
                             }
                         }
                      break;
                 }

          if (iStrPos < 0)
             iStrPos = 0;

          if (iStrPos > (iLength - 1))
             iStrPos = iLength - 1;

          iCol = iColHome + iStrPos;
          }

    for (iCounter = iLength - 1; iCounter >= 0; iCounter--)
        {
        if (szString[iCounter] > 32)
           {
           szString[iCounter + 1] = '\0';
           break;
           }
        }

    strcpy(szInString, szString);

    VioSetCursor(hwnd, iCurRow, iCurCol, iCurShow);

    return(iRc);
    }


// ---------------------------------------------------------------------
//  ESHook: Here programmer can add hooks to EditString.       880420
// ---------------------------------------------------------------------
int ESHook(int iHook, int iKey, int iCol, char *szString)
    {
    int         iRetKey;


    if (iKey < KEY_SPACE )
       return (iKey);

    iRetKey = iKey;

    switch (iHook)
           {
           case IDH_UCASE:
                if (iKey >= 'a' && iKey <='z')
                   iRetKey = iKey - 32;

                switch (iKey)
                       {
                       case '†':
                            iRetKey = '';
                            break;

                       case '„':
                            iRetKey = 'Ž';
                            break;

                       case '”':
                            iRetKey = '™';
                            break;

                       case '':
                            iRetKey = 'š';
                            break;
                       }
                break;

           case IDH_DATE:
           case IDH_TIME:
                if ( !(iKey >= '0' && iKey <= '9') )
                   iRetKey = 0;
                break;

           case IDH_NUM: /* Fix to support - at the beginning */
           case IDH_INT:
                if ( !(iKey >= '0' && iKey <= '9') )
                   iRetKey = 0;
                break;
           }
    return (iRetKey);
    }


/************************************************************************************
       ---------------------------------------------------------------------
        ESChkRet:
       ---------------------------------------------------------------------
        FUNCTION ESChkRet (ret, row, col, fieldid, retur(), quit)
            SELECT CASE ret
                   CASE KEYENTER
                        fieldid = fieldid + 1
                        IF fieldid > 3 THEN
                           FOR counter = 1 TO UBOUND(retur)
                               IF retur(counter) = KEYENTER THEN
                                  quit = TRUE
                                  ESChkRet = KEYENTER
                               END IF
                           NEXT counter
                        END IF
                   CASE KEYTAB
                        fieldid = fieldid + 1
                        IF fieldid > 3 THEN
                           FOR counter = 1 TO UBOUND(retur)
                               IF retur(counter) = KEYTAB THEN
                                  quit = TRUE
                                  ESChkRet = KEYTAB
                               END IF
                           NEXT counter
                        END IF
                   CASE ELSE
                        FOR counter = 1 TO UBOUND(retur)
                            IF retur(counter) = ret THEN
                               quit = TRUE
                               ESChkRet = ret
                            END IF
                        NEXT counter
               END SELECT
        END FUNCTION
************************************************************************************/


