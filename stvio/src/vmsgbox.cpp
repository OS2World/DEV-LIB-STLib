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
//     * WMSGBOX.MOD
//     *********************************************************************
//     * Part of WIN.MOD library after split
//     *
//     *********************************************************************
//     * 931107: Split...
//     *--------------------------------------------------------------------
//     *********************************************************************
#include "stlibvio.h"

// ---------------------------------------------------------------------
//  VioMessageBox:
// ---------------------------------------------------------------------
int VioMessageBox (PSTVIO pstv, int iBoxType, char *szRubrik, char *szRad1, char *szRad2, char *szRad3)
    {
    PSTVIOWINDOW		hwnd;
    int				iWinMessageBox;
    BOOL			bQuit = FALSE;
    int				iFieldId = 0;
    int				iScan;


// Base coordinates on screen size !!
    hwnd = VioOpenStandardWindow(pstv, 9, 20, 8, 40, szRubrik);

    VioPrint(hwnd, 2, strcenter(1, 38, szRad1), szRad1);
    VioPrint(hwnd, 3, strcenter(1, 38, szRad2), szRad2);
    VioPrint(hwnd, 4, strcenter(1, 38, szRad3), szRad3);

    switch (iBoxType)
           {
           case 0:
                VioColorPrint(hwnd, 7, 16, hwnd->iefcolor, hwnd->iebcolor, " < OK > ");
                VioSetCursor(hwnd, 6, 19, TRUE);
                while (bQuit == FALSE)
                      {
                      iScan = VioGetKey(hwnd, TRUE, FALSE);
                      switch (iScan)
                             {
                             case KEY_ENTER:
                             case KEY_SPACE:
                             case KEY_ESC:
                                  bQuit = TRUE;
                                  break;
                             }
                       }
                iWinMessageBox = TRUE;
                break;

           case 1:
                VioColorPrint(hwnd, 7, 10, hwnd->iefcolor, hwnd->iebcolor, " < Yes > ");
                VioColorPrint(hwnd, 7, 22, hwnd->iefcolor, hwnd->iebcolor, " < No > ");

                while (bQuit == FALSE)
                      {
                      if (iFieldId == 0)
                         VioSetCursor(hwnd, 6, 13, TRUE);
                      else
                         VioSetCursor(hwnd, 6, 25, TRUE);

                      iScan = VioGetKey(hwnd, TRUE, FALSE);
                      switch (iScan)
                             {
                             case KEY_ENTER:
                             case KEY_SPACE:
                                  bQuit = TRUE;
                                  break;

                             case KEY_RARROW:
                             case 'N':
                             case 'n':
                                  iFieldId = 1;
                                  break;

                             case KEY_LARROW:
                             case 'Y':
                             case 'y':
                                  iFieldId = 0;
                                  break;

                             case KEY_TAB:
                                  iFieldId = !iFieldId;
                                  break;
                             }
                      }
                if (iFieldId == 1)
                   iWinMessageBox = FALSE;
                else
                   iWinMessageBox = TRUE;
                break;

           case 2:
                VioColorPrint(hwnd, 7, 10, hwnd->iefcolor, hwnd->iebcolor, " < No > ");
                VioColorPrint(hwnd, 7, 21, hwnd->iefcolor, hwnd->iebcolor, " < Yes > ");

                while (bQuit == FALSE)
                      {
                      if (iFieldId == 0)
                         VioSetCursor(hwnd, 6, 13, TRUE);
                      else
                         VioSetCursor(hwnd, 6, 24, TRUE);

                      iScan = VioGetKey(hwnd, TRUE, FALSE);
                      switch (iScan)
                             {
                             case KEY_ENTER:
                             case KEY_SPACE:
                                  bQuit = TRUE;
                                  break;

                             case KEY_LARROW:
                             case 'N':
                             case 'n':
                                  iFieldId = 0;
                                  break;

                             case KEY_RARROW:
                             case 'Y':
                             case 'y':
                                  iFieldId = 1;
                                  break;

                             case KEY_TAB:
                                  iFieldId = !iFieldId;
                                  break;
                             }
                      }
                if (iFieldId == 1)
                   iWinMessageBox = TRUE;
                else
                   iWinMessageBox = FALSE;
                break;
           }
    VioSetCursor(hwnd, -1, -1, FALSE);
    VioCloseWindow(hwnd);
    return(iWinMessageBox);
    }


