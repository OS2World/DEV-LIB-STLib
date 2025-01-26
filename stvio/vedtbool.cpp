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
//     * VEDTBOOL.CPP
//     *********************************************************************
//     * Part of WIN.MOD library after split
//     *
//     *********************************************************************
//     * 931107: Split...
//     *--------------------------------------------------------------------
//     * 980302: Ported to C (stlibvio)
//     *********************************************************************
#include "stlibvio.h"

// ---------------------------------------------------------------------
//  WinEditBool:
// ---------------------------------------------------------------------
int VioEditBool(PSTVIOWINDOW hwnd, int *iCurVal, int iRow, int iCol, char *szSelect, int aiRet[], int iRetLen)
    {
    char        sFirst, sLast;
    char        szFirst[2], szLast[2];
    BOOL        bQuit = FALSE;
    int         iCurRow, iCurCol, iCurShow;
    int         iKey, iRc, iCounter;


    VioGetCursor (hwnd, &iCurRow, &iCurCol, &iCurShow);
    VioSetCursor(hwnd, iRow, iCol, TRUE);

    sFirst = szSelect[0];
    szFirst[0] = sFirst;
    szFirst[1] = '\0';
    sLast = szSelect[strlen(szSelect) - 1];
    szLast[0] = sLast;
    szLast[1] = '\0';

    while (bQuit == FALSE)
          {
          if (*iCurVal == FALSE)
             VioEditPrint(hwnd, iRow, iCol, szFirst);
          else
             VioEditPrint(hwnd, iRow, iCol, szLast);

          iKey = VioGetKey (hwnd, TRUE, FALSE);


          if (toupper(iKey) == toupper(sFirst))
             *iCurVal = FALSE;

          if (toupper(iKey) == toupper(sLast))
             *iCurVal = TRUE;

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
    VioSetCursor (hwnd, iCurRow, iCurCol, iCurShow);
    return (iRc);
    }


// ---------------------------------------------------------------------
//  VioEditCheck:
// ---------------------------------------------------------------------
int VioEditCheck (PSTVIOWINDOW hwnd, int *iCurVal, int iRow, int iCol, char *szText, int aiRet[], int iRetLen)
    {
    BOOL        bQuit = FALSE;
    int         iCurRow, iCurCol, iCurShow;
    int         iKey, iRc, iCounter;


    VioGetCursor (hwnd, &iCurRow, &iCurCol, &iCurShow);
    VioSetCursor(hwnd, iRow, iCol + 1, TRUE);

    while (bQuit == FALSE)
          {
          if (*iCurVal == TRUE)
             {
             VioFieldPrint (hwnd, iRow, iCol, "[X] ");
             VioFieldPrint (hwnd, iRow, iCol + 4, szText);
             }
          else
             {
             VioFieldPrint (hwnd, iRow, iCol, "[ ] ");
             VioFieldPrint (hwnd, iRow, iCol + 4, szText);
             }

          iKey = VioGetKey (hwnd, TRUE, FALSE);

          switch (iKey)
                 {
                 case KEY_SPACE:
                      if (*iCurVal == TRUE)
                         *iCurVal = FALSE;
                      else
                         *iCurVal = TRUE;
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
    VioSetCursor (hwnd, iCurRow, iCurCol, iCurShow);
    return (iRc);
    }

// ---------------------------------------------------------------------
//  WinEditOption:
// ---------------------------------------------------------------------
int VioEditOption (PSTVIOWINDOW hwnd, int *iCurVal, int iRow, int iCol, char *szText, int aiRet[], int iRetLen)
    {
    BOOL        bQuit = FALSE;
    int         iCurRow, iCurCol, iCurShow;
    int         iKey, iRc, iCounter;
    char        szBuffer[255];


    VioGetCursor (hwnd, &iCurRow, &iCurCol, &iCurShow);
    VioSetCursor(hwnd, iRow, iCol+ 1, TRUE);

    while (bQuit == FALSE)
          {
          if (*iCurVal == TRUE)
             {
             sprintf(szBuffer, "(%c) %s", 7, szText);
             VioFieldPrint (hwnd, iRow, iCol, szBuffer);
             }
          else
             {
             sprintf(szBuffer, "( ) %s", szText);
             VioFieldPrint (hwnd, iRow, iCol, szBuffer);
             }

          iKey = VioGetKey (hwnd, TRUE, FALSE);

          switch (iKey)
                 {
                 case KEY_SPACE:
                      if (*iCurVal == TRUE)
                         *iCurVal = FALSE;
                      else
                         *iCurVal = TRUE;
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
    VioSetCursor (hwnd, iCurRow, iCurCol, iCurShow);
    return (iRc);
    }

