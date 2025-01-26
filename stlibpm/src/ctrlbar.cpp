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
   CTRLBAR.C
  ==========================================================================
  ==========================================================================
   960808 Started converting statbar.c, lots happened.
  ==========================================================================
   961008 Fixed problems with controls outside the controlbar (combos).
  ==========================================================================
****************************************************************************/
#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include "stlibpmc.h"


MRESULT EXPENTRY ControlBarWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
void ControlBar3D (HPS hps, RECTL rcl);

/*************************************************************************************
  CreateControlBar: Creates controlbar & creates control.
*************************************************************************************/
HWND CreateControlBar (HWND hwnd, PSTCONTROLBAR pcb, char *szType, int iStyles, int iId)
     {
     static BOOL            bIsReg = 0;
     char                   szClass[] = "STControlBarClass";


     if (!bIsReg)
        {
        WinRegisterClass(WinQueryAnchorBlock(hwnd), szClass, (PFNWP)ControlBarWndProc,
                         CS_SYNCPAINT | CS_SIZEREDRAW, 4);
        bIsReg = TRUE;
        }

     memset(pcb, 0, sizeof(STCONTROLBAR));

     pcb->hwndParent = hwnd;
     pcb->bShow = TRUE;

     pcb->hwndControlBar = WinCreateWindow(hwnd, szClass, (PSZ)NULL, 0, FCF_NOBYTEALIGN,
                                           0, 0, 0, hwnd, HWND_TOP, 0, NULL, NULL);

     WinSetWindowPtr(pcb->hwndControlBar, 0, pcb);

     pcb->iControlId = iId;
     pcb->hwndControl = WinCreateWindow(pcb->hwndControlBar, szType, (PSZ) NULL, iStyles,
                                        0, 0, 0, 0, pcb->hwndControlBar, HWND_TOP,
                                        pcb->iControlId, 0, 0);
     return(pcb->hwndControlBar);
     }


/*************************************************************************************
  DestroyControlBar: Kills controlbar.
*************************************************************************************/
BOOL DestroyControlBar (PSTCONTROLBAR pcb)
     {
     return(WinDestroyWindow(pcb->hwndControlBar));
     }


/*************************************************************************************
  ControlBarWndProc: Handle Control bar messages.
*************************************************************************************/
MRESULT EXPENTRY ControlBarWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
     {
     RECTL              rcl, rcl1, rcl2, rcl11, rcl21;
     HPS                hps;
     POINTL             pp;

     PSTCONTROLBAR      pcb;



     switch (msg)
            {
            case WM_PAINT:
                 pcb = (PSTCONTROLBAR) WinQueryWindowPtr(hwnd, 0);

                 hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);
                 WinQueryWindowRect(hwnd, &rcl);
                 WinFillRect(hps, &rcl, CLR_PALEGRAY);
                 ControlBar3D(hps, rcl);
                 WinEndPaint(hps);
                 break;

            case WM_CONTROL:
                 pcb = (PSTCONTROLBAR) WinQueryWindowPtr(hwnd, 0);

                 if (SHORT1FROMMP(mp1) == pcb->iControlId)
                    WinPostMsg(pcb->hwndParent, WM_CONTROLBARMSG, (PVOID)SHORT2FROMMP(mp1), (PVOID)mp2);
                 break;

            default:
                 return (WinDefWindowProc(hwnd, msg, mp1, mp2));
            }
     return (FALSE);
     }


/*************************************************************************************
  SizeControlBar: Sizes the control bar.
************************************************************************************/
int SizeControlBar(PSTCONTROLBAR pcb, int iVertTop, int iMaxLen)
     {
     RECTL              prclClient;
     FONTMETRICS        fm;
     ULONG              xpos, ypos, ysize, xsize;
     ULONG              xposC, yposC, ysizeC, xsizeC, yReturn;
     ULONG              yFont, xFont;
     HPS                hps;


     if (pcb->hwndControlBar == NULLHANDLE)
        return (0);

     if (pcb->bShow == FALSE)
        return (0);

     WinQueryWindowRect(pcb->hwndParent, &prclClient);

     ysize = (STBORDERSPACE * 2) + pcb->iControlHeight;

     xsize = iMaxLen;
     if (xsize == 0)
        xsize = prclClient.xRight;

     ypos = iVertTop - ysize;
     xpos = prclClient.xLeft;

     /* Size the control */
     xposC = xpos + STBORDERSPACE;

     if (pcb->iControlWidth == 0)
        xsizeC = xsize - (STBORDERSPACE * 2);
     else
        xsizeC = pcb->iControlWidth;

     if (pcb->iControlDecend > 0)
        {
        yposC = STBORDERSPACE - pcb->iControlDecend;
        ysizeC = pcb->iControlHeight + pcb->iControlDecend;
        }
     else
        {
        yposC = STBORDERSPACE;
        ysizeC = pcb->iControlHeight;
        }

     /* Draw the windows */
     WinSetWindowPos(pcb->hwndControlBar, HWND_TOP, xpos, ypos, xsize, ysize,
                     SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ZORDER);
     yReturn = ysize;

     WinSetWindowPos(pcb->hwndControl, HWND_BOTTOM, xposC, yposC, xsizeC, ysizeC,
                     SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ZORDER);

     return (yReturn);
     }


/*************************************************************************************
  ShowControlBar: Shows (or hides) the control bar.
************************************************************************************/
BOOL ShowControlBar(PSTCONTROLBAR pcb, BOOL bShow)
     {
     if (pcb->hwndControlBar == NULLHANDLE)
        return (FALSE);

     if (pcb->bShow == bShow)
        return (pcb->bShow);

     if (bShow == TRUE)
        WinSetWindowPos(pcb->hwndControlBar, 0, 0, 0, 0, 0, SWP_SHOW);
     else
        WinSetWindowPos(pcb->hwndControlBar, 0, 0, 0, 0, 0, SWP_HIDE);

     pcb->bShow = bShow;

     return (!bShow);
     }


/*************************************************************************************
  EnableControlBar: Enable or disable controlbar.
*************************************************************************************/
void EnableControlBar(PSTCONTROLBAR pcb, BOOL bEnable)
     {
     WinEnableWindow(pcb->hwndControl, bEnable);
     }


/*************************************************************************************
  SizeCtrlControlBar: Set the size of the control in the bar.
*************************************************************************************/
void SizeCtrlControlBar(PSTCONTROLBAR pcb, int iControlHeight, int iControlWidth, int iDecend)
     {
     FONTMETRICS        fm;
     ULONG              yFont, xFont;
     HPS                hps;


     pcb->iControlHeight = iControlHeight;
     pcb->iControlWidth = iControlWidth;
     pcb->iControlDecend = iDecend;

     hps = WinGetPS(pcb->hwndControlBar);
     GpiQueryFontMetrics(hps, sizeof(fm), &fm);
     WinReleasePS(hps);

     if (pcb->iControlHeight == 0)
        pcb->iControlHeight = (fm.lMaxBaselineExt * 15) / 10;

     if (iDecend > 0)
        pcb->iControlDecend = fm.lMaxBaselineExt * iDecend;
     }


/*************************************************************************************
  ControlBar3D: Draws 3D look in specified RECTL.
*************************************************************************************/
void ControlBar3D (HPS hps, RECTL rcl)
     {
     POINTL             pp;

     GpiSetColor(hps, CLR_WHITE);

     pp.x = rcl.xLeft;
     pp.y = rcl.yBottom;
     GpiMove(hps, &pp);
     pp.x = rcl.xLeft;
     pp.y = rcl.yTop;
     GpiLine(hps, &pp);

     pp.x = rcl.xLeft;
     pp.y = rcl.yTop;
     GpiMove(hps, &pp);
     pp.x = rcl.xRight;
     pp.y = rcl.yTop;
     GpiLine(hps, &pp);

     GpiSetColor(hps, CLR_DARKGRAY);

     pp.x = rcl.xLeft + 1;
     pp.y = rcl.yBottom;
     GpiMove(hps, &pp);
     pp.x = rcl.xRight;
     pp.y = rcl.yBottom;
     GpiLine(hps, &pp);

     pp.x = rcl.xRight - 1;
     pp.y = rcl.yBottom;
     GpiMove(hps, &pp);
     pp.x = rcl.xRight - 1;
     pp.y = rcl.yTop - 1;
     GpiLine(hps, &pp);
     }

