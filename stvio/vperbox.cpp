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
//     * WPERBOX.MOD
//     *********************************************************************
//     * Part of Vio.MOD library after split
//     *
//     *********************************************************************
//     * 931107: Split...
//     *--------------------------------------------------------------------
//     *********************************************************************
#include "stlibvio.h"


// --------------------------------------------------------------------
//  VioProgressBox: To use in Vio for longer jobs.           890801
// --------------------------------------------------------------------
PSTVIOWINDOW VioProgressBox (PSTVIO pstv, PSTVIOWINDOW hwnd, int iFlag, char *szRubrik, int iRow, int iCol, int iStart, int iStop)
     {
     int                iLen, iStrLen, iPercent;
     char               szBuffer[80];


     switch (iFlag)
            {
            case 0:
                 hwnd = VioOpenStandardWindow(pstv, iRow, iCol, 7, 31, szRubrik);
                 hwnd->iProgressStart = iStart;
                 hwnd->iProgressStep = iStop - iStart;
                 hwnd->iProgressStop = iStop;

                 VioFill(hwnd->pstv->iOutHandle, hwnd->iRow1 + 3, hwnd->iCol1 + 3, 25, hwnd->iefcolor, hwnd->iebcolor);
                 VioColorPrint(hwnd, 5, 15, hwnd->iwfcolor, hwnd->iwbcolor, "0%");
                 return (hwnd);
                 break;

            case 1:
                 iStrLen = ((iStop * 100) / hwnd->iProgressStep) / 4;
                 iPercent = (iStop * 100) / hwnd->iProgressStep;
                 VioFillChar(hwnd->pstv->iOutHandle, hwnd->iRow1 + 3, hwnd->iCol1 + 3, iStrLen, '±');
                 sprintf(szBuffer, "%d%%", iPercent);
                 VioColorPrint(hwnd, 5, 15, hwnd->iwfcolor, hwnd->iwbcolor, szBuffer);
                 return (NULL);
                 break;

            case 2:
                 VioCloseWindow(hwnd);
                 return (NULL);
                 break;
            }
     return (NULL);
     }

