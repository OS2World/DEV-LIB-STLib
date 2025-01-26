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
/****************************************************************************
   WINUTIL.C
  ==========================================================================
   void SetWindowTitle (HWND hwnd, PSZ szNewTitle)
  ==========================================================================
   921211 Converted to CSET/2
  ==========================================================================
   961211 Added StartProgram
  ==========================================================================
   970511 Added WinCenterWindow, WinSetDlgLen, WinChangeStyle
  ==========================================================================
   970514 Added WinAddToTaskList, WinRemoveFromTaskList
          Removed WinChangeStyle
  ==========================================================================
   970722 Added parameter to StartProgram
  ==========================================================================
   970809 Added Win Max/Min/rest Window
  ==========================================================================
****************************************************************************/
#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <string.h>
#include "stlibpmc.h"

/*-------------------------------------------------------------------------------------------------
   SetWindowTitle: Sets title of windowtitlebar.
-------------------------------------------------------------------------------------------------*/
void SetWindowTitle (HWND hwnd, PSZ szNewTitle)
     {
     HWND       hwndParent, hwndTitle;

     hwndParent = WinQueryWindow(hwnd, QW_PARENT);
     hwndTitle = WinWindowFromID(hwndParent, FID_TITLEBAR);
     WinSetWindowText(hwndTitle, (PSZ)szNewTitle);
     }


/*-------------------------------------------------------------------------------------------------
   EnableWindowUpdate: Sets window 'state'.
-------------------------------------------------------------------------------------------------*/
void EnableWindowUpdate (HWND hwnd, int iFlag)
     {
     if (iFlag == FALSE)
        {
        WinSetPointer(hwnd, WinQuerySysPointer(HWND_DESKTOP, SPTR_WAIT, (ULONG) NULL));
        WinEnableWindowUpdate(hwnd, FALSE);
        }
     else
        {
        WinSetPointer(hwnd, WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, (ULONG) NULL));
        WinEnableWindowUpdate(hwnd, TRUE);
        }
     }


/*-----------------------------------------------------------------------------
   StartProgram: Starts another program with parameter.
-----------------------------------------------------------------------------*/
BOOL StartProgram(HWND hwndNotify, char *szTitle, char *szProg, char *szParam, char *szStartDir)
     {
     PROGDETAILS        Details;
     HAPP               happ;


     Details.Length          = sizeof(PROGDETAILS);
     Details.progt.progc     = PROG_DEFAULT;
     Details.progt.fbVisible = SHE_VISIBLE;
     Details.pszTitle        = szTitle;
     Details.pszExecutable   = szProg;
     Details.pszParameters   = szParam;
     Details.pszStartupDir   = szStartDir;
     Details.pszIcon         = NULL;
     Details.pszEnvironment  = NULL;
     Details.swpInitial.fl   = SWP_ACTIVATE;
     Details.swpInitial.cy   = 0;
     Details.swpInitial.cx   = 0;
     Details.swpInitial.y    = 0;
     Details.swpInitial.x    = 0;
     Details.swpInitial.hwndInsertBehind = HWND_TOP;
     Details.swpInitial.hwnd             = HWND_DESKTOP;
     Details.swpInitial.ulReserved1      = 0;
     Details.swpInitial.ulReserved2      = 0;

     happ = WinStartApp(hwndNotify, &Details, szParam, NULL, 0);

     if (happ == NULLHANDLE)
        return(FALSE);

     return(TRUE);
     }


/*************************************************************************************
  WinPrint: Prints string at logical char coordinates.
*************************************************************************************/
void WinPrint(HWND hwnd, HPS hps, USHORT usRow, USHORT usCol, PSZ szMessage)
     {
     FONTMETRICS        fm;
     ULONG              yLen,xLen;
     RECTL              rcl;
     POINTL             ppointl;

     GpiQueryFontMetrics(hps, sizeof(fm), &fm);
     yLen = fm.lMaxBaselineExt;
     xLen = fm.lEmInc;

     WinQueryWindowRect(hwnd, &rcl);

     ppointl.y = rcl.yTop - ((usRow) * yLen);
     ppointl.x = (usCol) * xLen;

     GpiMove(hps, &ppointl);
     GpiCharString(hps, strlen(szMessage), szMessage);
     }


/*************************************************************************************
   WinCenterWindow: Centers a window on the screen.
*************************************************************************************/
void WinCenterWindow (HWND hwnd)
     {
     RECTL rclScreen,rclWnd;
     LONG  sWidth,sHeight,sBLCx,sBLCy;

     WinQueryWindowRect(HWND_DESKTOP, &rclScreen);
     WinQueryWindowRect(hwnd, &rclWnd);

     sWidth = (LONG) (rclWnd.xRight - rclWnd.xLeft);
     sHeight = (LONG) (rclWnd.yTop - rclWnd.yBottom);
     sBLCx = ((LONG) rclScreen.xRight - sWidth) / 2;
     sBLCy = ((LONG) rclScreen.yTop - sHeight) / 2;

     WinSetWindowPos(hwnd, HWND_TOP, sBLCx, sBLCy, 0, 0, SWP_MOVE);
     }


/*************************************************************************************
   WinSetDlgLen: Sets length of Dialogitem input.
*************************************************************************************/
void WinSetDlgLen(HWND hwnd, int iItem, int iLen)
     {
     WinSendDlgItemMsg(hwnd, iItem, EM_SETTEXTLIMIT, MPFROM2SHORT(iLen, 0), NULL);
     }


/*************************************************************************************
   WinSelDlgEntry: Selects text of Dialogitem input.
*************************************************************************************/
void WinSelDlgEntry(HWND hwnd, int iItem)
     {
     WinSendDlgItemMsg(hwnd, iItem, EM_SETSEL, MPFROM2SHORT(0, 1024), NULL);
     }


/*************************************************************************************
   WinAddToTaskList: Adds window to the tasklist.
*************************************************************************************/
LHANDLE WinAddToTaskList(HWND hwndFrame, char *szTitle)
     {
     SWCNTRL    swctl;
     PID        pid;


     WinQueryWindowProcess(hwndFrame, &pid, NULL);

     swctl.hwnd = hwndFrame;
     swctl.hwndIcon = NULLHANDLE;
     swctl.hprog = NULLHANDLE;
     swctl.idProcess = pid;
     swctl.idSession = 0;
     swctl.uchVisibility = SWL_VISIBLE;
     swctl.fbJump = SWL_JUMPABLE;
     strcpy(swctl.szSwtitle, szTitle);
     swctl.bProgType = PROG_DEFAULT;

     return (WinAddSwitchEntry(&swctl));
     }


/*************************************************************************************
   WinRemoveFromTaskList: Removes window from the tasklist.
*************************************************************************************/
void WinRemoveFromTaskList(HWND hwndFrame)
     {
     HSWITCH    hswitch;


     hswitch = WinQuerySwitchHandle(hwndFrame, 0);
     WinRemoveSwitchEntry(hswitch);
     }

/******************************************************************************
   WinEmphaseDrag: Emphases drop window in a drag/drop operation.
******************************************************************************/
void WinEmphaseDrag (HWND hwnd, BOOL bFlag)
     {
     HPS                hps;
     POINTL             ptl;
     RECTL              rcl;
     long               lColor;
     static BOOL        bDraw = TRUE;



     if (bFlag == TRUE && bDraw == FALSE)
        return;

     WinQueryWindowRect(hwnd, &rcl);

     hps = DrgGetPS(hwnd);


     if (bFlag == TRUE)
        bDraw = FALSE;
     else
        bDraw = TRUE;

     ptl.x = ptl.y = 5;
     GpiMove(hps, &ptl);

     ptl.x = rcl.xRight - rcl.xLeft - 5;
     ptl.y = rcl.yTop - rcl.yBottom - 5;

     GpiSetColor(hps, CLR_RED);
     GpiSetMix(hps, FM_XOR);
     GpiSetBackMix(hps, BM_LEAVEALONE);

     GpiBox(hps, DRO_OUTLINE, &ptl, 0L, 0L);
     DrgReleasePS(hps);
     }


/*************************************************************************************
   WinMaximizeWindow: Maximizes a window.
*************************************************************************************/
void WinMaximizeWindow (HWND hwnd)
     {
     WinSetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_MAXIMIZE | SWP_SHOW);
     }


/*************************************************************************************
   WinMinimizeWindow: Minimizes a window.
*************************************************************************************/
void WinMinimizeWindow (HWND hwnd)
     {
     WinSetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_MINIMIZE);
     }


/*************************************************************************************
   WinRestoreWindow: Restores a window.
*************************************************************************************/
void WinRestoreWindow (HWND hwnd)
     {
     WinSetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_RESTORE | SWP_SHOW);
     }


