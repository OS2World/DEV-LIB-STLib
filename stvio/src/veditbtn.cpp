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
// * WEDITBTN.MOD
// *********************************************************************
// * Part of WIN.MOD library after split
// *
// *********************************************************************
// * 931107: Split...
// *--------------------------------------------------------------------
// * 980302: Ported to C (stlibvio)
// *********************************************************************
#include "stlibvio.h"

// ---------------------------------------------------------------------
//  VioEditButton: Button in a vio window.
// ---------------------------------------------------------------------
int VioEditButton(PSTVIOWINDOW hwnd, char *szText, int iRow, int iCol, int iRet, int aiRet[], int iRetLen)
    {
    int         iCurRow, iCurCol, iCurShow;
    int         iCounter, iLength;
    int         iKey, iRc;
    char        szBuffer[255];
    BOOL        bQuit = FALSE;


    sprintf(szBuffer, " < %s > ", szText);
    iLength = strlen(szBuffer);

    ValidPos(hwnd, &iRow, &iCol, &iLength);

    if (strlen(szBuffer) > iLength)
       szBuffer[iLength] = '\0';

    VioGetCursor(hwnd, &iCurRow, &iCurCol, &iCurShow);
    VioSetCursor(hwnd, iRow, iCol + 3, TRUE);

    while (bQuit == FALSE)
          {
          VioFieldPrint(hwnd, iRow, iCol, szBuffer);

          iKey = VioGetKey (hwnd, TRUE, FALSE);

          switch (iKey)
                 {
                 case KEY_SPACE:
                      iRc = iRet;
                      bQuit = TRUE;
                      break;

                 default:
                      for (iCounter = 0; iCounter < iRetLen; iCounter++)
                          {
                          if (aiRet[iCounter] == iKey)
                             {
                             iRc = iKey;
                             bQuit = TRUE;
                             break;
                             }
                          }
                 }
          }
    VioSetCursor(hwnd, iCurRow, iCurCol, iCurShow);
    return(iRc);
    }

