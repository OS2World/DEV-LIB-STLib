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
   HOVERBOX.C
  ==========================================================================
  ==========================================================================
   960711 Created with help from SPLITBAR.C
  ==========================================================================
   960712 Added thread and Event Semaphore.
  ==========================================================================
   960907 Redesigned using timer.
  ==========================================================================
   960908 Added check for Subclassed hwnd:s (check also id) and fixed
          no motion window placement.
  ==========================================================================
   980222 Moved window to x = 0 when bMotion = FALSE.
  ==========================================================================
****************************************************************************/
#define INCL_WIN
#define INCL_GPI
#define INCL_DOS

#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include "stlibpmc.h"

MRESULT EXPENTRY HoverBoxWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
BOOL IsMouseOnHwnd(HWND hwndHit, int iHitId, int (*fnUserTest)(void),  PPOINTL pptl);

/*************************************************************************************
  CreateHoverBox: Creates HoverBox.
*************************************************************************************/
HWND CreateHoverBox (HWND hwndParent, PSTHOVERBOX phb)
     {
     static BOOL        bIsClassRegistered = 0;
     PSZ                pszClass = "STHoverBoxClass";
     ULONG              flFlags;


     if (!bIsClassRegistered)
        {
        WinRegisterClass(WinQueryAnchorBlock(hwndParent), pszClass, (PFNWP)HoverBoxWndProc,
                         CS_SYNCPAINT | CS_SIZEREDRAW | CS_SAVEBITS, 4);
        bIsClassRegistered = TRUE;
        }

     memset(phb, 0, sizeof(STHOVERBOX));

     phb->hwndParent = hwndParent;

     flFlags = FCF_NOBYTEALIGN | FCF_BORDER;
     phb->hwndHoverBox = WinCreateStdWindow(hwndParent, WS_SYNCPAINT | WS_SAVEBITS, &flFlags, pszClass, (PSZ)NULL,
                                            0, 0, (HMODULE)NULL, &phb->hwndHoverBoxChild);

     WinSetWindowPtr(phb->hwndHoverBoxChild, 0, phb);
     phb->bShow = FALSE;
     phb->bFirstShow = FALSE;
     phb->bMotion = TRUE;
     phb->fnUserTest = NULL;
     phb->idTimer = WinStartTimer(WinQueryAnchorBlock(phb->hwndParent), phb->hwndHoverBoxChild, IDT_HOVERTIMER, HOVER_TIMEOUT);

     return(phb->hwndHoverBox);
     }


/*************************************************************************************
  DestroyHoverBox: Kills HoverBox.
*************************************************************************************/
BOOL DestroyHoverBox (PSTHOVERBOX phb)
     {
     phb->bShow = FALSE;
     WinStopTimer(WinQueryAnchorBlock(phb->hwndHoverBox), phb->hwndHoverBoxChild, phb->idTimer);
     return(WinDestroyWindow(phb->hwndHoverBox));
     }


/*************************************************************************************
  IsMouseOnHwnd: Checks if Mouse is ontop of hwnd .
************************************************************************************/
BOOL IsMouseOnHwnd(HWND hwndHit, int iHitId, int (*fnUserTest)(void),  PPOINTL pptl)
     {
     RECTL              rcl;
     int                iHit;


     // if ((*fnUserTest) != NULL)
     //   {
     //   if ((*fnUserTest) == FALSE)
     //      return (FALSE);
     //   }

     if (iHitId != 0)
        {
        if (WinQueryWindowUShort(hwndHit, QWS_ID) != iHitId)
           return (FALSE);
        }

     WinQueryPointerPos(HWND_DESKTOP, pptl);
     WinMapWindowPoints(HWND_DESKTOP, hwndHit, pptl, 1);
     WinQueryWindowRect(hwndHit, &rcl);
     if (WinPtInRect(WinQueryAnchorBlock(hwndHit), &rcl, pptl))
        return (TRUE);
     else
        return(FALSE);
     }


/*************************************************************************************
  ShowHoverBox: Shows/positions the Hover Box.
************************************************************************************/
void ShowHoverBox(PSTHOVERBOX phb, HWND hwndHit, int iHitId, BOOL bShow)
     {
     RECTL              rcl;
     POINTL             ptl;
     int                yPos;
     unsigned long      ulIgnore;
     APIRET             rc;


     if (bShow == FALSE)
        {
        phb->bShow = FALSE;
        WinSetWindowPos(phb->hwndHoverBox, HWND_TOP, 0, 0, 0, 0, SWP_HIDE);
        return;
        }

     phb->hwndHit = hwndHit;

     if (IsMouseOnHwnd(phb->hwndHit, phb->iHitId, phb->fnUserTest, &ptl) == TRUE)
        {
        if (phb->bMotion == FALSE)
           {
           if (phb->bShow == TRUE)
              return;
           }

        if (phb->bMotion == FALSE)
           {
           WinQueryWindowRect(hwndHit, &rcl);
           ptl.x = 0;
           ptl.y = 0;
           WinMapWindowPoints(hwndHit, phb->hwndParent, &ptl, 1);
           WinSetWindowPos(phb->hwndHoverBox, HWND_TOP, ptl.x,
                           ptl.y - rcl.yTop,
                           phb->xSize, phb->ySize,
                           SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ZORDER);
           }
        else
           {
           if (phb->bShow == TRUE)
              {
              WinQueryWindowRect(hwndHit, &rcl);
              WinMapWindowPoints(hwndHit, phb->hwndParent, &ptl, 1);

              WinSetWindowPos(phb->hwndHoverBox, HWND_TOP, ptl.x - (phb->xSize / 2) + WinQuerySysValue(HWND_DESKTOP, SV_CXPOINTER),
                             (ptl.y - phb->ySize) - (WinQuerySysValue(HWND_DESKTOP, SV_CYPOINTER) / 2),
                             phb->xSize, phb->ySize,
                             SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ZORDER);

              if (WinIsWindowShowing(phb->hwndHoverBox) == FALSE)
                 WinSetWindowPos(phb->hwndHoverBox, HWND_TOP, ptl.x - (phb->xSize / 2) + WinQuerySysValue(HWND_DESKTOP, SV_CXPOINTER),
                                 (ptl.y - phb->ySize) - (WinQuerySysValue(HWND_DESKTOP, SV_CYPOINTER) / 2),
                                 phb->xSize, phb->ySize,
                                 SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ZORDER);
              }
           else
              {
              WinQueryWindowRect(hwndHit, &rcl);
              WinMapWindowPoints(hwndHit, phb->hwndParent, &ptl, 1);

              WinSetWindowPos(phb->hwndHoverBox, HWND_TOP, ptl.x - (phb->xSize / 2) + WinQuerySysValue(HWND_DESKTOP, SV_CXPOINTER),
                              (ptl.y - phb->ySize) - (WinQuerySysValue(HWND_DESKTOP, SV_CYPOINTER) / 2),
                              phb->xSize, phb->ySize,
                              SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ZORDER);
              }
           }

        WinStartTimer(WinQueryAnchorBlock(phb->hwndParent), phb->hwndHoverBoxChild, IDT_HOVERTIMER, HOVER_TIMEOUT);
        phb->bShow = TRUE;
        }
     else
        {
        phb->bShow = FALSE;
        WinSetWindowPos(phb->hwndHoverBox, HWND_TOP, 0, 0, 0, 0, SWP_HIDE);
        }
     phb->iHitId = iHitId;
     }


/*************************************************************************************
  HoverBoxWndProc: Handle messages to HoverBox.
*************************************************************************************/
MRESULT EXPENTRY HoverBoxWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
     {
     RECTL              rcl;
     POINTL             ptl;
     HPS                hps;

     PSTHOVERBOX        phb;
     int                iStrLen;
     char               szText[256];


     switch (msg)
            {
            case WM_CREATE:
                 break;

            case WM_TIMER:
                 phb = (PSTHOVERBOX) WinQueryWindowPtr(hwnd, 0);

                 if (phb->bShow == TRUE)
                    {
                    if (SHORT1FROMMP(mp1) == IDT_HOVERTIMER && IsMouseOnHwnd(phb->hwndHit, phb->iHitId, phb->fnUserTest, &ptl) == FALSE)
                       {
                       WinShowWindow(phb->hwndHoverBox, FALSE);
                       phb->bShow = FALSE;
                       }
                    }
                 break;

            case WM_PAINT:
                 phb = (PSTHOVERBOX) WinQueryWindowPtr(hwnd, 0);

                 hps = WinBeginPaint(hwnd, 0L, 0L);
                 WinQueryWindowRect(hwnd, &rcl);

                 szText[0] = ' ';
                 strcpy(szText + 1, phb->szText);
                 iStrLen = strlen(szText);
                 szText[iStrLen] = ' ';
                 szText[iStrLen + 1] = '\0';

                 WinDrawText(hps, -1, szText, &rcl, phb->iColorText, phb->iColorBack,
                             phb->iAlignInt | DT_ERASERECT | DT_VCENTER);

                 WinEndPaint(hps);
                 break ;

            default:
                 return ((MRESULT) WinDefWindowProc(hwnd, msg, mp1, mp2));

            }
     return (FALSE);
     }


/*************************************************************************************
  SetHoverBoxText: Sets text of HoverBox.
*************************************************************************************/
void SetHoverBoxText(PSTHOVERBOX phb, char* szText)
     {
     HPS                hps;
     RECTL              rcl;
     char               szTmpText[256];
     int                iStrLen;


     if (strcmp(szText, phb->szText) == 0)
        return;

     strcpy(phb->szText, szText);

     memset(&rcl, 0, sizeof(RECTL));
     rcl.yTop = 100;
     rcl.xRight = 100;

     szTmpText[0] = ' ';
     strcpy(szTmpText + 1, phb->szText);
     iStrLen = strlen(szTmpText);
     szTmpText[iStrLen] = ' ';
     szTmpText[iStrLen + 1] = '\0';

     hps = WinGetPS(phb->hwndHoverBoxChild);
     WinDrawText(hps, -1, szTmpText, &rcl, 0, 0, DT_QUERYEXTENT);
     WinReleasePS(hps);

     phb->xSize = (STBORDERSPACE * 2) + (rcl.xRight- rcl.xLeft);
     phb->ySize = (STBORDERSPACE * 2) + (rcl.yTop - rcl.yBottom);

     WinInvalidateRect(phb->hwndHoverBox, NULL, FALSE);
     }


/*************************************************************************************
  SetHoverBoxMotion: Move or not on window.
*************************************************************************************/
void SetHoverBoxMotion (PSTHOVERBOX phb, BOOL bMotion)
     {
     phb->bMotion = bMotion;
     }


/*************************************************************************************
  SetHoverBoxFont: Sets font of HoverBox.
*************************************************************************************/
void SetHoverBoxFont(PSTHOVERBOX phb, char* szFont)
     {
     strcpy(phb->szFont, szFont);
     WinSetPresParam(phb->hwndHoverBoxChild, PP_FONTNAMESIZE, (ULONG)strlen(phb->szFont) + 1, &phb->szFont);

     WinInvalidateRect(phb->hwndHoverBox, NULL, FALSE);
     }


/*************************************************************************************
  SetHoverBoxColor: Sets color of text/background in HoverBox.
*************************************************************************************/
void SetHoverBoxColor(PSTHOVERBOX phb, int iColorText, int iColorBack)
     {
     phb->iColorText = iColorText;
     phb->iColorBack = iColorBack;

     WinInvalidateRect(phb->hwndHoverBox, NULL, FALSE);
     }


/*************************************************************************************
  SetHoverBoxAlignment: Sets aligmnent of HoverBox text.
*************************************************************************************/
void SetHoverBoxAlignment (PSTHOVERBOX phb, int iAlign)
     {
     phb->iAlign = iAlign;

     switch (phb->iAlign)
            {
            case HOVER_LEFT:
                 phb->iAlignInt = DT_LEFT;
                 break;

            case HOVER_CENTER:
                 phb->iAlignInt = DT_CENTER;
                 break;

            case HOVER_RIGHT:
                 phb->iAlignInt = DT_RIGHT;
                 break;
            }

     WinInvalidateRect(phb->hwndHoverBox, NULL, FALSE);
     }

/*************************************************************************************
  SetHoverCallBack: Sets callback function for check if on window.
*************************************************************************************/
void SetHoverCallBack (PSTHOVERBOX phb, int (*fnUserTest)(void))
     {
     phb->fnUserTest = fnUserTest;
     }
