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
//     *
//     *  W I N D O W I N G   L I B R A R Y   F O R    S T V I O
//     *  ----------------------------------------------------------
//     *
//     *  Started: 930501     Last: 000106      Timelog: 27h
//     *
//     *********************************************************************

#include "stlibvio.h"

void ValidPos (PSTVIOWINDOW hwnd, int iRow, int iCol, int iLen);
void ZoomWindow (PSTVIO pstv, int iRow1, int iCol1, int iRow2, int iCol2, int iForeColor, int iBackColor, int iFrame);

// ---------------------------------------------------------------------
//  VioInit: Initiate Vio environment.
// ---------------------------------------------------------------------
PSTVIO VioInit (int iSetMaxRow, int iSetMaxCol)
     {
     int        iMaxRows, iMaxCols;
     PSTVIO     pstv;


     // Create this window struct
     pstv = (PSTVIO) calloc(1, sizeof(STVIO));

     // Init any Vio handle
     VioInit(&(pstv->iOutHandle), &(pstv->iInHandle));

     pstv->bFirstTime = TRUE;
     VioGetInfo(pstv->iOutHandle, &iMaxRows, &iMaxCols, (int*) NULL);
     pstv->iOrgMaxRows = iMaxRows;
     pstv->iOrgMaxCols = iMaxCols;

     pstv->iOrgCursorRow = VioQueryCursorRow(pstv->iOutHandle);
     pstv->iOrgCursorCol = VioQueryCursorColumn(pstv->iOutHandle);

     pstv->bOrgCursorVisible = VioQueryCursorVisible(pstv->iOutHandle);

     pstv->pchOrgScreen = VioGetWindow(pstv->iOutHandle, 1, 1, pstv->iOrgMaxRows, pstv->iOrgMaxCols);

     if (iSetMaxRow == -1)
        pstv->iMaxRows = iMaxRows;
     else
        pstv->iMaxRows = iSetMaxRow;

     if (iSetMaxCol == -1)
        pstv->iMaxCols = iMaxCols;
     else
        pstv->iMaxCols = iSetMaxCol;

     if (pstv->iMaxRows != pstv->iOrgMaxRows || pstv->iMaxCols != pstv->iOrgMaxCols)
        VioSetScreenSize(pstv->iOutHandle, pstv->iMaxRows, pstv->iMaxCols);

     pstv->ibfcolor = COLORF_BLACK;
     pstv->ibbcolor = COLORB_WHITE;
     pstv->iwfcolor = COLORF_BLACK;
     pstv->iwbcolor = COLORB_WHITE;
     pstv->iffcolor = COLORF_BLACK;
     pstv->ifbcolor = COLORB_WHITE;
     pstv->iefcolor = COLORF_WHITE;
     pstv->iebcolor = COLORB_BLACK;

     return(pstv);
     }


// ---------------------------------------------------------------------
//  VioExit: Exit Vio environment.
// ---------------------------------------------------------------------
void VioExit(PSTVIO pstv)
     {
     int        iMaxRows, iMaxCols;


     if (pstv->iMaxRows != pstv->iOrgMaxRows || pstv->iMaxCols != pstv->iOrgMaxCols)
        VioSetScreenSize(pstv->iOutHandle, pstv->iOrgMaxRows, pstv->iOrgMaxCols);

     VioPutWindow(pstv->iOutHandle, pstv->pchOrgScreen, 1, 1, pstv->iOrgMaxRows, pstv->iOrgMaxCols);

     VioSetCursorPosition(pstv->iOutHandle, pstv->iOrgCursorRow, pstv->iOrgCursorCol);
     VioShowCursor(pstv->iOutHandle, pstv->bOrgCursorVisible);

     // "Exit" any vio handle;
     VioExit(pstv->iOutHandle, pstv->iInHandle);

     free(pstv);
     }


// ---------------------------------------------------------------------
//  VioSetGlobal: Set global values.
// ---------------------------------------------------------------------
void VioSetGlobal(PSTVIO pstv, BOOL bUseColor, int iFrame, BOOL bShade, BOOL bZoom, int iZoomTime, int bf, int bb, int wf, int wb, int ff, int fb, int ef, int eb)
     {
     if (iFrame == -1)
        pstv->iFrame = 32;
     else
        pstv->iFrame = iFrame;

     pstv->bShade = bShade;
     pstv->bZoom = bZoom;

     if (iZoomTime == -1)
        pstv->iZoomTime = 6;
     else
        pstv->iZoomTime = iZoomTime;

     pstv->bUseColor = bUseColor;
     if (pstv->bUseColor == FALSE)
        {
        pstv->ibfcolor = COLORF_BLACK;
        pstv->ibbcolor = COLORB_WHITE;
        pstv->iwfcolor = COLORF_BLACK;
        pstv->iwbcolor = COLORB_WHITE;
        pstv->iffcolor = COLORF_BLACK;
        pstv->ifbcolor = COLORB_WHITE;
        pstv->iefcolor = COLORF_WHITE;
        pstv->iebcolor = COLORB_BLACK;
        }
     else
        {
        if (bf != -1)
           pstv->ibfcolor = bf;

        if (bb != -1)
           pstv->ibbcolor = bb;

        if (wf != -1)
           pstv->iwfcolor = wf;

        if (wb != -1)
           pstv->iwbcolor = wb;

        if (ff != -1)
           pstv->iffcolor = ff;

        if (fb != -1)
           pstv->ifbcolor = fb;

        if (ef != -1)
           pstv->iefcolor = ef;

        if (eb != -1)
           pstv->iebcolor = eb;
        }

// All other settings...
// delimiters and so on

     pstv->bFirstTime = FALSE;
     }


// ---------------------------------------------------------------------
//  VioOpenStandardWindow
// ---------------------------------------------------------------------
PSTVIOWINDOW VioOpenStandardWindow(PSTVIO pstv, int iRow1, int iCol1, int iRowLen, int iColLen, char *szRubrik)
     {
     PSTVIOWINDOW       hwnd;
     int                iRow2, iCol2;
     char               *wndBuffer;
     int                iRowTo, iColFr, iColTo;


     // Create this window struct
     hwnd = (PSTVIOWINDOW) calloc(1, sizeof(STVIOWINDOW));

     // Save cursor position
     hwnd->iOldRow = VioQueryCursorRow(pstv->iOutHandle);
     hwnd->iOldCol = VioQueryCursorColumn(pstv->iOutHandle);
     hwnd->iOldCursorShow = VioQueryCursorVisible(pstv->iOutHandle);

     // If Vio is not initialized, do so
     if (pstv->bFirstTime == TRUE)
        VioSetGlobal(pstv, TRUE, 32, TRUE, TRUE, 30, -1, -1, -1, -1, -1, -1, -1, -1);

     hwnd->pstv = pstv;
     hwnd->bShade = pstv->bShade;
     hwnd->iFrame = pstv->iFrame;
     hwnd->bZoom = pstv->bZoom;
     hwnd->ibfcolor = pstv->ibfcolor;
     hwnd->ibbcolor = pstv->ibbcolor;
     hwnd->iwfcolor = pstv->iwfcolor;
     hwnd->iwbcolor = pstv->iwbcolor;
     hwnd->iffcolor = pstv->iffcolor;
     hwnd->ifbcolor = pstv->ifbcolor;
     hwnd->iefcolor = pstv->iefcolor;
     hwnd->iebcolor = pstv->iebcolor;

     iRow2 = iRow1 + iRowLen - 1;
     iCol2 = iCol1 + iColLen - 1;

     // Set coordinates
     iRowTo = iRow2;
     iColFr = iCol1;
     iColTo = iCol2;

     // Check coordinates
     if (iRow1 < 1)
        iRow1 = 1;

     if (iRowTo > pstv->iMaxRows)
        iRowTo = pstv->iMaxRows;

     if (iColFr < 1)
        iColFr = 1;

     if (iColTo > pstv->iMaxCols)
        iColTo = pstv->iMaxCols;

     // Set windows positions in hwnd struct
     hwnd->iRow1 = iRow1;
     hwnd->iCol1 = iCol1;
     hwnd->iWndRows = iRow2 - iRow1 - 1;
     hwnd->iWndCols = iCol2 - iCol1 - 1;
     if (hwnd->bShade == TRUE)
        {
        hwnd->iRow2 = iRow2 + 1;
        hwnd->iCol2 = iCol2 + 2;
        }
     else
        {
        hwnd->iRow2 = iRow2;
        hwnd->iCol2 = iCol2;
        }

     // Save background
     hwnd->wndBuffer = VioGetWindow(pstv->iOutHandle, hwnd->iRow1, hwnd->iCol1, hwnd->iRow2, hwnd->iCol2);

     // Zoom if requested
     if (hwnd->bZoom == TRUE)
        ZoomWindow(pstv, iRow1, iCol1, iRow2, iCol2, hwnd->ibfcolor, hwnd->iwbcolor, hwnd->iFrame);

     // Draw shade if requested
     if (hwnd->bShade == TRUE)
        VioShade(pstv->iOutHandle, iRow1, iColFr, iRowTo, iColTo);

     // Draw Window
     VioFrameFill(pstv->iOutHandle, iRow1, iCol1, iRow2, iCol2, hwnd->ibfcolor, hwnd->ibbcolor, hwnd->iFrame);
     VioFrameFill(pstv->iOutHandle, iRow1 + 1, iCol1 + 1, iRow2 - 1, iCol2 - 1, hwnd->iwfcolor, hwnd->iwbcolor, 32);

     // Draw titlebar with titlebar text
     if (strlen(szRubrik) > 0)
        {
        VioFill(pstv->iOutHandle, iRow1, iColFr, iColTo - iColFr + 1, hwnd->ibbcolor, hwnd->ibfcolor);

        // If string is to long, clip it
        if (strlen(szRubrik) > (iCol2 - iCol1 - 3) )
           szRubrik[iCol2 - iCol1 - 3] = '\0';

        VioPrintString(pstv->iOutHandle, iRow1, iCol1 + 2, szRubrik);
        }
     return (hwnd);
     }


// ---------------------------------------------------------------------
//  VioOpenWindow
// ---------------------------------------------------------------------
PSTVIOWINDOW VioOpenWindow(PSTVIO pstv, int iRow1, int iCol1, int iRowLen, int iColLen, char *szRubrik, int iFrame, int iShade, int iZoom, int iZoomTime, int bf, int bb, int wf, int wb, int ff, int fb, int ef, int eb)
     {
     PSTVIOWINDOW       hwnd;
     int                iRow2, iCol2;
     char               *wndBuffer;
     int                iRowTo, iColFr, iColTo;


     // Create this window struct
     hwnd = (PSTVIOWINDOW) calloc(1, sizeof(STVIOWINDOW));

     // Save cursor position
     hwnd->iOldRow = VioQueryCursorRow(pstv->iOutHandle);
     hwnd->iOldCol = VioQueryCursorColumn(pstv->iOutHandle);
     hwnd->iOldCursorShow = VioQueryCursorVisible(pstv->iOutHandle);

     // If Vio is not initialized, do so
     if (pstv->bFirstTime == TRUE)
        VioSetGlobal(pstv, TRUE, iFrame, iShade, iZoom, 30, -1, -1, -1, -1, -1, -1, -1, -1);

     hwnd->pstv = pstv;

     // Check for specific shade
     if (iShade == -1)
        hwnd->bShade = pstv->bShade;
     else
        hwnd->bShade = iShade;

     // Check for specific frame
     if (iFrame == -1)
        hwnd->iFrame = pstv->iFrame;
     else
        hwnd->iFrame = iFrame;

     // Check for specific zoom
     if (iZoom == -1)
        hwnd->bZoom = pstv->bZoom;
     else
        hwnd->bZoom = iZoom;

     // Check for specific colors
     if (bf == -1)
        hwnd->ibfcolor = pstv->ibfcolor;

     if (bb == -1)
        hwnd->ibbcolor = pstv->ibbcolor;

     if (wf == -1)
        hwnd->iwfcolor = pstv->iwfcolor;

     if (wb == -1)
        hwnd->iwbcolor = pstv->iwbcolor;

     if (ff == -1)
        hwnd->iffcolor = pstv->iffcolor;

     if (fb == -1)
        hwnd->ifbcolor = pstv->ifbcolor;

     if (ef == -1)
        hwnd->iefcolor = pstv->iefcolor;

     if (eb == -1)
        hwnd->iebcolor = pstv->iebcolor;

     iRow2 = iRow1 + iRowLen - 1;
     iCol2 = iCol1 + iColLen - 1;

     // Set coordinates
     iRowTo = iRow2;
     iColFr = iCol1;
     iColTo = iCol2;

     // Check coordinates
     if (iRow1 < 1)
        iRow1 = 1;

     if (iRowTo > pstv->iMaxRows)
        iRowTo = pstv->iMaxRows;

     if (iColFr < 1)
        iColFr = 1;

     if (iColTo > pstv->iMaxCols)
        iColTo = pstv->iMaxCols;

     // Set windows positions in hwnd struct
     hwnd->iRow1 = iRow1;
     hwnd->iCol1 = iCol1;
     hwnd->iWndRows = iRow2 - iRow1 - 1;
     hwnd->iWndCols = iCol2 - iCol1 - 1;

     if (hwnd->bShade == TRUE)
        {
        hwnd->iRow2 = iRow2 + 1;
        hwnd->iCol2 = iCol2 + 2;
        }
     else
        {
        hwnd->iRow2 = iRow2;
        hwnd->iCol2 = iCol2;
        }

     // Save background
     hwnd->wndBuffer = VioGetWindow(pstv->iOutHandle, hwnd->iRow1, hwnd->iCol1, hwnd->iRow2, hwnd->iCol2);

     // Zoom if requested
     if (hwnd->bZoom == TRUE)
        ZoomWindow(pstv, iRow1, iCol1, iRow2, iCol2, hwnd->ibfcolor, hwnd->iwbcolor, iFrame);

     // Draw shade if requested
     if (hwnd->bShade == TRUE)
        VioShade(pstv->iOutHandle, iRow1, iColFr, iRowTo, iColTo);

     // Draw Window
     VioFrameFill(pstv->iOutHandle, iRow1, iCol1, iRow2, iCol2, hwnd->ibfcolor, hwnd->ibbcolor, iFrame);
     VioFrameFill(pstv->iOutHandle, iRow1 + 1, iCol1 + 1, iRow2 - 1, iCol2 - 1, hwnd->iwfcolor, hwnd->iwbcolor, 32);

     // Draw titlebar with titlebar text
     if (strlen(szRubrik) > 0)
        {
        VioFill(pstv->iOutHandle, iRow1, iColFr, iColTo - iColFr + 1, hwnd->ibbcolor, hwnd->ibfcolor);

        // If string is to long, clip it
        if (strlen(szRubrik) > iCol2 - iCol1 - 3)
           szRubrik[iCol2 - iCol1 - 1] = '\0';

        VioPrintString(pstv->iOutHandle, iRow1, iCol1 + 2, szRubrik);

        }
     return (hwnd);
     }


// ---------------------------------------------------------------------
//  VioCloseWindow:
// ---------------------------------------------------------------------
void VioCloseWindow (PSTVIOWINDOW hwnd)
     {
     VioPutWindow(hwnd->pstv->iOutHandle, hwnd->wndBuffer, hwnd->iRow1, hwnd->iCol1, hwnd->iRow2, hwnd->iCol2);
     VioSetCursorPosition(hwnd->pstv->iOutHandle, hwnd->iOldRow, hwnd->iOldCol);
     VioShowCursor(hwnd->pstv->iOutHandle, hwnd->iOldCursorShow);
     free(hwnd);
     }


// ---------------------------------------------------------------------
//  VioGetKey:
// ---------------------------------------------------------------------
int VioGetKey(PSTVIOWINDOW hwnd, BOOL bWaitForKey, BOOL bEcho)
    {
    int         iKeyPress;
    char        szBuffer[8];


    if (bWaitForKey == TRUE)
       iKeyPress = VioInkey(hwnd->pstv->iInHandle, TRUE, NULL);
    else
       iKeyPress = VioInkey(hwnd->pstv->iInHandle, FALSE, NULL);

    if (bEcho == TRUE && iKeyPress > 0)
       {
       sprintf(szBuffer, "%c", iKeyPress);
       VioPrint(hwnd, hwnd->iCursorRow, hwnd->iCursorCol, szBuffer);
       }
    return(iKeyPress);
    }


// ---------------------------------------------------------------------
//  VioGetKeyBoardShift:
// ---------------------------------------------------------------------
int VioGetKeyBoardShift(PSTVIOWINDOW hwnd)
    {
    int         iShiftstate;


    VioInkey(hwnd->pstv->iInHandle, FALSE, &iShiftstate);
    return(iShiftstate);
    }


// ---------------------------------------------------------------------
//  VioSetCursor:
// ---------------------------------------------------------------------
void VioSetCursor (PSTVIOWINDOW hwnd, int iRow, int iCol, int iShow)
    {
    int         iShiftstate;


    ValidPos (hwnd, &iRow, &iCol, NULL);
    hwnd->iCursorRow = iRow;
    hwnd->iCursorCol = iCol;
    if (iShow != -1)
       hwnd->bCursorShow = iShow;

    VioSetCursorPosition(hwnd->pstv->iOutHandle, hwnd->iRow1 + hwnd->iCursorRow, hwnd->iCol1 + hwnd->iCursorCol);
    VioShowCursor(hwnd->pstv->iOutHandle, hwnd->bCursorShow);
    }


// ---------------------------------------------------------------------
//  VioGetCursor:
// ---------------------------------------------------------------------
void VioGetCursor (PSTVIOWINDOW hwnd, int *iRow, int *iCol, int *iShow)
    {
    int         iShiftstate;

// This code is incorrect !!!!!!!!!!!!!
    if (iRow != NULL)
       *iRow = VioQueryCursorRow(hwnd->pstv->iOutHandle);

    if (iRow != NULL)
       *iCol = VioQueryCursorColumn(hwnd->pstv->iOutHandle);

    if (iRow != NULL)
       *iShow = VioQueryCursorVisible(hwnd->pstv->iOutHandle);
    }


// ---------------------------------------------------------------------
//  ValidPos: Validates positions (to be within window).
// ---------------------------------------------------------------------
void ValidPos (PSTVIOWINDOW hwnd, int *iRow, int *iCol, int *iLen)
     {
     if (*iRow > hwnd->iWndRows)
        *iRow = hwnd->iWndRows;

     if (*iCol >= hwnd->iWndCols)
        *iCol = hwnd->iWndCols;

     if (iLen !=NULL)
        {
        if (*iCol + *iLen > hwnd->iWndCols)
           *iLen = hwnd->iWndCols - *iCol;
        }
     }


// ---------------------------------------------------------------------
//  ZoomWindow: Zooms window.
// ---------------------------------------------------------------------
void ZoomWindow (PSTVIO pstv, int iRow1, int iCol1, int iRow2, int iCol2, int iForeColor, int iBackColor, int iFrame)
     {
     int        iRowMitt, iColMitt, iColStep, iRowStep, iRow, iCol;
     int        iCounter, iTimer;


     iRowMitt = iRow1 + ((iRow2 - iRow1) / 2);
     iColMitt = iCol1 + ((iCol2 - iCol1) / 2);

     if (iCol2 - iCol1 > iRow2 - iRow1)
        {
        iColStep = (iCol2 - iCol1) / (iRow2 - iRow1);
        for (iCounter = 1; iCounter <= (iRow2 - iRow1) / 2; iCounter++)
            {
            iCol = iColStep * iCounter;
            VioFrameFill(pstv->iOutHandle, iRowMitt - iCounter, iColMitt - iCol, iRowMitt + iCounter, iColMitt + iCol, iForeColor, iBackColor, iFrame);
            iTimer = VioTimer();
            while (iTimer + pstv->iZoomTime > VioTimer())
                  ;
            }
        }
     else
        {
        iRowStep = (iRow2 - iRow1) / (iCol2 - iCol1);
        for (iCounter = 1; iCounter <= (iCol2 - iCol1) / 2; iCounter++)
            {
            iRow = iRowStep * iCounter;
            VioFrameFill(pstv->iOutHandle, iRowMitt - iRow, iColMitt - iCounter, iRowMitt + iRow, iColMitt + iCounter, iForeColor, iBackColor, iFrame);
            while (iTimer + pstv->iZoomTime > VioTimer())
                  ;
            }
        }
     }

