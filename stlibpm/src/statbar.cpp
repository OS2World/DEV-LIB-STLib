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
   STATBAR.C
  ==========================================================================
  HWND CreateStatusBar (HAB hab, HWND hwnd, int iType)
  BOOL DestroyStatusBar (HWND hwnd)
  ULONG SizeStatusBar (HWND hwnd, ULONG ulBtnLen)
  ==========================================================================
   960528 Started converting btnbar.c
  ==========================================================================
   960529 Fixed as much flicker as possible.
  ==========================================================================
   960818 Fixed redraw (size in WM_PAINT) problem.
  ==========================================================================
   970514 Major restructure.
  ==========================================================================
   970731 Added ownerdraw window support.
  ==========================================================================
   980324 Added show/hide statusbar.
  ==========================================================================
****************************************************************************/
#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stlibpmc.h"


/*-------------------------------------------------*/
BOOL            bIsReg = 0;


MRESULT EXPENTRY StatusBarWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
void ResizeStatusBarClients (PSTSTATUSBAR pstsb);
void CalcOneRect(PSTSTATUSBAR pstsb, PRECTL prcl, PRECTL prcl1);
void CalcTwoRect(PSTSTATUSBAR pstsb, PRECTL pRcl, PRECTL pRcl1, PRECTL pRcl2);
void StatusBar3D (HPS hps, RECTL rcl, int iType, BOOL bFill);

/*************************************************************************************
  CreateButtonBar: Creates buttonbar & loads icons.
*************************************************************************************/
HWND CreateStatusBar (HWND hwnd, PSTSTATUSBAR pstsb, int iType, int iItem1Type, int iItem2Type)
     {
     char       szClass[] = "STStatusBarClass";
     ULONG      flFlags;


     pstsb->hwndOwner = hwnd;
     pstsb->bShow = TRUE;

     if (!bIsReg)
        {
        WinRegisterClass(WinQueryAnchorBlock(pstsb->hwndOwner), szClass, (PFNWP)StatusBarWndProc, CS_SIZEREDRAW | CS_SYNCPAINT, 4);
        bIsReg = TRUE;
        }

     pstsb->iType = iType;

     pstsb->iType1 = iItem1Type;
     pstsb->iType2 = iItem2Type;

     pstsb->hwndStatus = WinCreateWindow(hwnd, szClass, (PSZ)NULL, 0, 0, 0, 0, 0, hwnd, HWND_TOP, 0, NULL, NULL);
     WinSetWindowPtr(pstsb->hwndStatus, 0, pstsb);

     flFlags = FCF_NOBYTEALIGN | WS_SYNCPAINT;

     if (pstsb->iType1 == STATUS_TEXT)
        pstsb->hwndChild1 = WinCreateWindow(pstsb->hwndStatus, WC_STATIC, (PSZ)NULL, SS_TEXT | DT_VCENTER,
                                             0, 0, 0, 0, pstsb->hwndStatus, HWND_BOTTOM, 0, NULL, NULL);
     else
        pstsb->hwndChild1 = WinCreateWindow(pstsb->hwndStatus, WC_BUTTON, (PSZ) NULL, BS_PUSHBUTTON | BS_USERBUTTON | BS_NOPOINTERFOCUS,
                                            0, 0, 0, 0, pstsb->hwndStatus, HWND_BOTTOM, 0, NULL, NULL);

     if (pstsb->iType2 == STATUS_TEXT)
        pstsb->hwndChild2 = WinCreateWindow(pstsb->hwndStatus, WC_STATIC, (PSZ)NULL, SS_TEXT | DT_VCENTER,
                                            0, 0, 0, 0, pstsb->hwndStatus, HWND_BOTTOM, 0, NULL, NULL);
     else
        pstsb->hwndChild2 = WinCreateWindow(pstsb->hwndStatus, WC_BUTTON, (PSZ) NULL, BS_PUSHBUTTON | BS_USERBUTTON | BS_NOPOINTERFOCUS,
                                            0, 0, 0, 0, pstsb->hwndStatus, HWND_BOTTOM, 0, NULL, NULL);

     SetStatusBarText(pstsb, "", "");
     SetStatusBarColor(pstsb, CLR_BLACK, CLR_BLACK);
     SetStatusBarBackColor(pstsb, CLR_PALEGRAY, CLR_PALEGRAY);
     SetStatusBarFont(pstsb, "8.Helv");
     SetStatusBarAlignment(pstsb, STATUS_LEFT, STATUS_LEFT);
     pstsb->iTwoSplit = 5;

     return(pstsb->hwndStatus);
     }


/*************************************************************************************
  DestroyStatusBar: Kills statusbar.
*************************************************************************************/
BOOL DestroyStatusBar (PSTSTATUSBAR pstsb)
     {
     WinDestroyWindow(pstsb->hwndChild1);
     WinDestroyWindow(pstsb->hwndChild2);

     return(WinDestroyWindow(pstsb->hwndStatus));
     }


/*************************************************************************************
  StatusBarWndProc: Handle Status bar paint.
*************************************************************************************/
MRESULT EXPENTRY StatusBarWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
     {
     RECTL              rcl, rcl1, rcl2, rcl11, rcl21;
     HPS                hps;
     POINTL             pp;
     int                iAlign, fc, bc, iStrLen;
     char               szText1[130], szText2[130];
     PSTSTATUSBAR       pstsb;
     PUSERBUTTON        ubInfo;


     switch (msg)
            {
            case WM_SIZE:
                 WinQueryWindowRect(hwnd, &rcl);
                 if (rcl.xRight == 0)
                    break;
                 else
                    ResizeStatusBarClients((PSTSTATUSBAR) WinQueryWindowPtr(hwnd, 0));
                 break;

            case WM_PAINT:
                 pstsb = (PSTSTATUSBAR) WinQueryWindowPtr(hwnd, 0);

                 hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);
                 WinQueryWindowRect(hwnd, &rcl);

                 StatusBar3D(hps, rcl, 0, TRUE);

                 switch (pstsb->iType)
                        {
                        case STATUS_ONE:
                             CalcOneRect(pstsb, &rcl, &rcl1);
                             StatusBar3D(hps, rcl1, 1, FALSE);
                             break;

                        case STATUS_TWO:
                             CalcTwoRect(pstsb, &rcl, &rcl1, &rcl2);
                             StatusBar3D(hps, rcl1, 1, FALSE);
                             StatusBar3D(hps, rcl2, 1, FALSE);
                             break;
                        }
                 WinEndPaint(hps);
                 break;


            case WM_CONTROL:
                 pstsb = (PSTSTATUSBAR) WinQueryWindowPtr(hwnd, 0);
                 switch (SHORT2FROMMP(mp1))
                        {
                        case BN_PAINT:
                             ubInfo = (PUSERBUTTON) LONGFROMMP(mp2);

                             if (ubInfo->hwnd == pstsb->hwndChild1)
                                pstsb->iItemToRedraw = 1;
                             else
                                pstsb->iItemToRedraw = 2;

                             pstsb->hwndRedraw = ubInfo->hwnd;
                             pstsb->hpsRedraw = ubInfo->hps;
                             WinSendMsg(pstsb->hwndOwner, WM_STATUSREDRAW, MPFROMLONG(pstsb->iStatusId), MPFROMLONG(pstsb->iItemToRedraw));
                        }

            default:
                 return (WinDefWindowProc(hwnd, msg, mp1, mp2));
            }
     return (FALSE);
     }


/*************************************************************************************
  CalcOneRect: Calcs rect for one fields.
************************************************************************************/
void CalcOneRect(PSTSTATUSBAR pstsb, PRECTL prcl, PRECTL prcl1)
     {
     prcl1->yBottom = prcl->yBottom + STBORDERSPACE;
     prcl1->yTop = prcl->yTop - STBORDERSPACE;
     prcl1->xLeft = prcl->xLeft + STBORDERSPACE;
     prcl1->xRight = (prcl->xRight - 1) - STBORDERSPACE;
     }


/*************************************************************************************
  CalcTwoRect: Calcs rect for two fields.
************************************************************************************/
void CalcTwoRect(PSTSTATUSBAR pstsb, PRECTL prcl, PRECTL prcl1, PRECTL prcl2)
     {
     prcl1->yBottom = prcl->yBottom + STBORDERSPACE;
     prcl1->yTop = prcl->yTop - STBORDERSPACE;
     prcl1->xLeft = prcl->xLeft + STBORDERSPACE;
     prcl1->xRight = (((prcl->xRight - prcl->xLeft) / 10)  * pstsb->iTwoSplit) - STBORDERSPACE;

     prcl2->yBottom = prcl->yBottom + STBORDERSPACE;
     prcl2->yTop = prcl->yTop - STBORDERSPACE;
     prcl2->xLeft =  prcl1->xRight + STBORDERSPACE;
     prcl2->xRight = (prcl->xRight - 1) - STBORDERSPACE;
     }


/*************************************************************************************
  ResizeStatusBarClients: Sizes clients.
************************************************************************************/
void ResizeStatusBarClients (PSTSTATUSBAR pstsb)
     {
     RECTL              rcl, rcl1, rcl2;


     WinQueryWindowRect(pstsb->hwndStatus, &rcl);
     switch (pstsb->iType)
            {
            case STATUS_ONE:
                 CalcOneRect(pstsb, &rcl, &rcl1);
                 WinSetWindowPos(pstsb->hwndChild1, HWND_BOTTOM,
                                 rcl1.xLeft + 1, rcl1.yBottom + 1,
                                 (rcl1.xRight - 1) - (rcl1.xLeft + 1), (rcl1.yTop - 1) - (rcl1.yBottom + 1),
                                 SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ZORDER);
                 break;

            case STATUS_TWO:
                 CalcTwoRect(pstsb, &rcl, &rcl1, &rcl2);
                 WinSetWindowPos(pstsb->hwndChild1, HWND_BOTTOM,
                                 rcl1.xLeft + 1, rcl1.yBottom + 1,
                                 (rcl1.xRight - 1) - (rcl1.xLeft + 1), (rcl1.yTop - 1) - (rcl1.yBottom + 1),
                                 SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ZORDER);

                 WinSetWindowPos(pstsb->hwndChild2, HWND_BOTTOM,
                                 rcl2.xLeft + 1, rcl2.yBottom + 1,
                                 (rcl2.xRight - 1) - (rcl2.xLeft + 1), (rcl2.yTop - 1) - (rcl2.yBottom + 1),
                                 SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ZORDER);
                 break;
            }
     }


/*************************************************************************************
  UpdateStatusBarItem: Redraws ownerdraw item.
************************************************************************************/
void UpdateStatusBarItem(PSTSTATUSBAR pstsb, int iItem)
     {
     if (iItem == 1)
        WinInvalidateRect(pstsb->hwndChild1, NULL, TRUE);
     else
        WinInvalidateRect(pstsb->hwndChild2, NULL, TRUE);
     }


/*************************************************************************************
  SizeStatusBar: Sizes the status bar.
************************************************************************************/
int SizeStatusBar (PSTSTATUSBAR pstsb, int iMaxLen)
     {
     RECTL              prclClient;
     FONTMETRICS        fm;
     ULONG              xpos, ypos, ysize, xsize;
     ULONG              yFont, xFont;
     HPS                hps;

     if (pstsb->hwndStatus == NULLHANDLE)
         return (0);

     if (pstsb->bShow == FALSE)
         return (0);

     WinQueryWindowRect(pstsb->hwndOwner, &prclClient);

     ypos = prclClient.yBottom;
     xpos = prclClient.xLeft;

     hps = WinGetPS(pstsb->hwndStatus);
     GpiQueryFontMetrics(hps, sizeof(fm), &fm);
     yFont = fm.lMaxBaselineExt;
     xFont = fm.lEmInc;
     WinReleasePS(hps);

     ysize = prclClient.yBottom + (STBORDERSPACE * 2) + yFont + 4;

     xsize = iMaxLen;
     if (xsize == 0)
        xsize = prclClient.xRight;

     WinSetWindowPos(pstsb->hwndStatus, HWND_TOP, xpos, ypos, xsize, ysize,
                     SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ZORDER);
     return (ysize);
     }


/*************************************************************************************
  ShowStatusBar: Shows (or hides) the status bar.
************************************************************************************/
BOOL ShowStatusBar (PSTSTATUSBAR pstsb, BOOL bShow)
     {

     if (pstsb->hwndStatus == NULLHANDLE)
        return (FALSE);

     if (pstsb->bShow == bShow)
        return (pstsb->bShow);

     if (bShow == TRUE)
        WinSetWindowPos(pstsb->hwndStatus, 0, 0, 0, 0, 0, SWP_SHOW);
     else
        WinSetWindowPos(pstsb->hwndStatus, 0, 0, 0, 0, 0, SWP_HIDE);

     pstsb->bShow = bShow;

     return (!bShow);
     }


/*************************************************************************************
  SetStatusBarText: Sets text of statusbar.
*************************************************************************************/
void SetStatusBarText (PSTSTATUSBAR pstsb, char* szText1, char* szText2)
     {
     char       szTemp1[128], szTemp2[128];


     strncpy(szTemp1, szText1, 128);
     strncpy(szTemp2, szText2, 128);

     if (strcmp(szTemp1, pstsb->szText1) != 0)
        {
        sprintf(pstsb->szText1, " %s", szTemp1);
        WinSetWindowText(pstsb->hwndChild1, pstsb->szText1);
        }

     if (strcmp(szTemp2, pstsb->szText2) != 0)
        {
        sprintf(pstsb->szText2, " %s", szTemp2);
        WinSetWindowText(pstsb->hwndChild2, pstsb->szText2);
        }
     }


/*************************************************************************************
  SetStatusBarColor: Sets color of text in statusbar.
*************************************************************************************/
void SetStatusBarColor (PSTSTATUSBAR pstsb, int iColor1, int iColor2)
     {
     pstsb->iColor1 = iColor1;
     pstsb->iColor2 = iColor2;

     WinSetPresParam(pstsb->hwndChild1, PP_FOREGROUNDCOLORINDEX, (ULONG)sizeof(long), (PVOID)&iColor1);
     WinSetPresParam(pstsb->hwndChild2, PP_FOREGROUNDCOLORINDEX, (ULONG)sizeof(long), (PVOID)&iColor2);
     }


/*************************************************************************************
  SetStatusBarBackColor: Sets color of text in statusbar.
*************************************************************************************/
void SetStatusBarBackColor (PSTSTATUSBAR pstsb, int iColor1, int iColor2)
     {
     WinSetPresParam(pstsb->hwndChild1, PP_BACKGROUNDCOLORINDEX, (ULONG)sizeof(long), (PVOID)&iColor1);
     WinSetPresParam(pstsb->hwndChild2, PP_BACKGROUNDCOLORINDEX, (ULONG)sizeof(long), (PVOID)&iColor2);
     }


/*************************************************************************************
  SetStatusBarFont: Sets font of statusbar.
*************************************************************************************/
void SetStatusBarFont (PSTSTATUSBAR pstsb, char* szFont)
     {
     strcpy(pstsb->szFont, szFont);
     WinSetPresParam(pstsb->hwndChild1, PP_FONTNAMESIZE, (ULONG)strlen(pstsb->szFont) + 1, (PVOID)&pstsb->szFont);
     WinSetPresParam(pstsb->hwndChild2, PP_FONTNAMESIZE, (ULONG)strlen(pstsb->szFont) + 1, (PVOID)&pstsb->szFont);
     }


/*************************************************************************************
  SetStatusBarAlignment: Sets aligmnent of statusbar text(s).
*************************************************************************************/
void SetStatusBarAlignment (PSTSTATUSBAR pstsb, int iAlign1, int iAlign2)
     {
     ULONG              ulValue;

     pstsb->iAlign1 = iAlign1;
     pstsb->iAlign2 = iAlign2;

     switch (pstsb->iAlign1)
            {
            case STATUS_LEFT:
                 pstsb->iAlignInt1 = DT_LEFT;
                 break;

            case STATUS_CENTER:
                 pstsb->iAlignInt1 = DT_CENTER;
                 break;

            case STATUS_RIGHT:
                 pstsb->iAlignInt1 = DT_RIGHT;
                 break;
            }

     switch (pstsb->iAlign2)
            {
            case STATUS_LEFT:
                 pstsb->iAlignInt2 = DT_LEFT;
                 break;

            case STATUS_CENTER:
                 pstsb->iAlignInt2 = DT_CENTER;
                 break;

            case STATUS_RIGHT:
                 pstsb->iAlignInt2 = DT_RIGHT;
                 break;
            }

     ulValue = WinQueryWindowULong(pstsb->hwndChild1, QWL_STYLE);
     ulValue |= pstsb->iAlignInt1;
     WinSetWindowULong(pstsb->hwndChild1, QWL_STYLE, ulValue);

     ulValue = WinQueryWindowULong(pstsb->hwndChild2, QWL_STYLE);
     ulValue |= pstsb->iAlignInt2;
     WinSetWindowULong(pstsb->hwndChild2, QWL_STYLE, ulValue);
     }


/*************************************************************************************
  SetStatusBarSplit: Where split the bar (if double).
*************************************************************************************/
void SetStatusBarSplit(PSTSTATUSBAR pstsb, int iSplit)
     {
     pstsb->iTwoSplit = iSplit;
     WinInvalidateRect(pstsb->hwndStatus, NULL, TRUE);
     }


/*************************************************************************************
  StatusBar3D: Draws 3D look in specified RECTL.
*************************************************************************************/
void StatusBar3D (HPS hps, RECTL rcl, int iType, BOOL bFill)
     {
     POINTL             pp;


     /* All around */
     if (iType == 1)
        GpiSetColor(hps, CLR_DARKGRAY);
     else
        GpiSetColor(hps, CLR_WHITE);

     pp.x = rcl.xLeft;
     pp.y = rcl.yTop - 1;
     GpiMove(hps, &pp);

     pp.x = rcl.xRight;
     pp.y = rcl.yBottom;
     GpiBox(hps, DRO_OUTLINE, &pp, 0, 0);


     /* Bottom/right */
     if (iType == 1)
        GpiSetColor(hps, CLR_WHITE);
     else
        GpiSetColor(hps, CLR_DARKGRAY);

     pp.x = rcl.xRight;
     pp.y = rcl.yBottom;
     GpiMove(hps, &pp);

     pp.x = rcl.xLeft + 1;
     pp.y = rcl.yTop - 2;
     GpiBox(hps, DRO_OUTLINE, &pp, 0, 0);


     /* Fill contents */
     if (bFill == TRUE)
        {
        GpiSetColor(hps, CLR_PALEGRAY);

        pp.x = rcl.xLeft + 1;
        pp.y = rcl.yBottom + 1;
        GpiMove(hps, &pp);

        pp.x = rcl.xRight - 1;
        pp.y = rcl.yTop - 2;
        GpiBox(hps, DRO_FILL, &pp, 0, 0);
        }
     }

