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
   SPLITBAR.C
  ==========================================================================
  ==========================================================================
   950602 Created with help from BTRBAR.C
  ==========================================================================
   960707 Added support for other controls on client area.
  ==========================================================================
   960714 Changed mouse pointer for horizontal bar.
  ==========================================================================
   960818 Added LMB drag and reworked with no StdWindows.
  ==========================================================================
   980324 Added show/hide.
  ==========================================================================
****************************************************************************/
#define INCL_WIN

#include <os2.h>
#include <stdlib.h>
#include "stlibpmc.h"


MRESULT EXPENTRY SplitBarWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

/*************************************************************************************
  CreateSplitBar: Creates Splitbar.
*************************************************************************************/
HWND CreateSplitBar (HWND hwndParent, BOOL bDirection, PSPLITBAR psb)
     {
     static BOOL        bIsClassRegistered = 0;
     PSZ                pszClass = "STSplitBarClass";


     if (!bIsClassRegistered)
        {
        WinRegisterClass(WinQueryAnchorBlock(hwndParent), pszClass, (PFNWP)SplitBarWndProc,
                         CS_SIZEREDRAW | CS_SYNCPAINT | CS_SAVEBITS, 4);
        bIsClassRegistered = TRUE;
        }

     psb->bDirection = bDirection;
     psb->hwndParent = hwndParent;
     psb->bShow = TRUE;

     psb->hwndSplitBarChild = WinCreateWindow(hwndParent, pszClass, (PSZ)NULL, 0, 0, 0, 0, 0,
                                              hwndParent, HWND_TOP, 0, NULL, NULL);

     WinSetWindowPtr(psb->hwndSplitBarChild, 0, psb);

     return(psb->hwndSplitBarChild);
     }


/*************************************************************************************
  DestroySplitBar: Kills splitbar.
*************************************************************************************/
BOOL DestroySplitBar (PSPLITBAR psb)
     {
     return(WinDestroyWindow(psb->hwndSplitBarChild));
     }


/*************************************************************************************
  SplitBarWndProc: Handle messages to splitbar.
*************************************************************************************/
MRESULT EXPENTRY SplitBarWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
     {
     static     BOOL            bDrag;
     static     HPOINTER        hPointerV, hPointerH;

     POINTL     ptl;
     RECTL      rcl;
     HPS        hps;
     PSPLITBAR  psb;

     switch (msg)
            {
            case WM_CREATE:
                 bDrag = FALSE;
                 hPointerV = WinQuerySysPointer(HWND_DESKTOP, SPTR_SIZEWE, FALSE);
                 hPointerH = WinQuerySysPointer(HWND_DESKTOP, SPTR_SIZENS, FALSE);
                 break;

            case WM_BEGINDRAG:
            case WM_BUTTON1MOTIONSTART:
                 bDrag = TRUE;
                 WinSetCapture(HWND_DESKTOP, hwnd);
                 break;

            case WM_MOUSEMOVE:
                 psb = (PSPLITBAR) WinQueryWindowPtr(hwnd, 0);
                 if (bDrag == TRUE)
                    {
                    WinQueryPointerPos(HWND_DESKTOP, &ptl);
                    WinMapWindowPoints(HWND_DESKTOP, psb->hwndParent, &ptl, 1);
                    switch (psb->bDirection)
                           {
                           case SBP_HORZ:
                                WinSetWindowPos(psb->hwndSplitBarChild, HWND_TOP, psb->iBorderBL, ptl.y, 0, 0,
                                                SWP_MOVE | SWP_SHOW | SWP_ZORDER);
                                break;

                           case SBP_VERT:
                                WinSetWindowPos(psb->hwndSplitBarChild, HWND_TOP, ptl.x, psb->iBorderBL, 0, 0,
                                                SWP_MOVE | SWP_SHOW | SWP_ZORDER);
                                break;

                           }
                    }
                 switch (psb->bDirection)
                        {
                        case SBP_HORZ:
                             WinSetPointer(HWND_DESKTOP, hPointerH);
                             break;

                        case SBP_VERT:
                             WinSetPointer(HWND_DESKTOP, hPointerV);
                             break;
                        }
                 break;

            case WM_ENDDRAG:
            case WM_BUTTON1MOTIONEND:
                 if (bDrag == TRUE)
                    {
                    psb = (PSPLITBAR) WinQueryWindowPtr(hwnd, 0);
                    WinSetCapture(HWND_DESKTOP, NULLHANDLE);
                    WinQueryPointerPos(HWND_DESKTOP, &ptl);
                    WinMapWindowPoints(HWND_DESKTOP, psb->hwndParent, &ptl, 1);
                    bDrag = FALSE;
                    switch (psb->bDirection)
                           {
                           case SBP_VERT:
                                WinPostMsg(psb->hwndParent, WM_SPLITBARMOVED, (PVOID) SHORT1FROMMP(ptl.x), NULL);
                                break;

                           case SBP_HORZ:
                                WinPostMsg(psb->hwndParent, WM_SPLITBARMOVED, (PVOID) SHORT1FROMMP(ptl.y), NULL);
                                break;
                           }
                    }
                 break;

            case WM_PAINT:
                 psb = (PSPLITBAR) WinQueryWindowPtr(hwnd, 0);
                 hps = WinBeginPaint(hwnd, 0L, 0L);
                 WinQueryWindowRect(hwnd, &rcl);
                 WinFillRect(hps, &rcl, CLR_PALEGRAY);

                 switch (psb->bDirection)
                        {
                        case SBP_VERT:
                             WinDrawBorder(hps, &rcl, 1, 0, CLR_DARKGRAY, CLR_PALEGRAY, DB_STANDARD);
                             break;

                        case SBP_HORZ:
                             WinDrawBorder(hps, &rcl, 0, 1, CLR_DARKGRAY, CLR_PALEGRAY, DB_STANDARD);
                             break;
                        }
                 WinEndPaint(hps);
                 break ;

            default:
                 return ((MRESULT) WinDefWindowProc(hwnd, msg, mp1, mp2));

            }
     return (FALSE);
     }


/*************************************************************************************
  PositionSplitBar: Places the splitbar at specified coordinates.
************************************************************************************/
ULONG PositionSplitBar (PSPLITBAR psb, int iBorderBL, int iBorderTR, ULONG ulSplitBarPos)
      {
      RECTL     rclParent;
      ULONG     lBorderWidth;
      ULONG     ypos, xpos, ysize, xsize;


      if (psb->hwndSplitBarChild == NULLHANDLE)
         return (0);

      if (psb->bShow == FALSE)
         return (0);

      WinQueryWindowRect(psb->hwndParent, &rclParent);

      xpos  = rclParent.xLeft;
      xsize = WinQuerySysValue(HWND_DESKTOP, SV_CXSIZEBORDER);
      ypos  = rclParent.yBottom;
      ysize = WinQuerySysValue(HWND_DESKTOP, SV_CYSIZEBORDER);


      psb->iBorderBL = iBorderBL;
      psb->iBorderTR = iBorderTR;

      switch (psb->bDirection)
             {
             case SBP_HORZ:
                  ypos = ulSplitBarPos;
                  xpos = xpos + iBorderBL;
                  xsize = rclParent.xRight - (iBorderTR + iBorderBL);
                  break;

             case SBP_VERT:
                  xpos  = ulSplitBarPos;
                  ypos = ypos + iBorderBL;
                  ysize = rclParent.yTop - (iBorderTR + iBorderBL);
                  break;
             }

      WinSetWindowPos(psb->hwndSplitBarChild, HWND_TOP, xpos, ypos, xsize, ysize,
                      SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ZORDER);

      if (psb->bDirection == SBP_VERT)
         return(xsize);
      else
         return(ysize);

      }


/*************************************************************************************
  ShowSplitBar: Shows (or hides) the split bar.
************************************************************************************/
BOOL ShowSplitBar(PSPLITBAR psb, BOOL bShow)
     {
     if (psb->hwndSplitBarChild == NULLHANDLE)
        return (FALSE);

     if (psb->bShow == bShow)
        return (psb->bShow);

     if (bShow == TRUE)
        WinSetWindowPos(psb->hwndSplitBarChild, 0, 0, 0, 0, 0, SWP_SHOW);
     else
        WinSetWindowPos(psb->hwndSplitBarChild, 0, 0, 0, 0, 0, SWP_HIDE);

     psb->bShow = bShow;

     return (!bShow);
     }

