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
   CONTROL.C
  ==========================================================================

  ==========================================================================
   970427 Created with many other codes as source.
  ==========================================================================
   980310 Added support for resource DLL.
  ==========================================================================
   980324 Added show/hide control window.
  ==========================================================================
****************************************************************************/
#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stlibpmc.h"

void ControlInitItems(PSTCONTROL pStCtrl);
void ControlDrawItem(PSTCONTROL pStCtrl, HPS hps, int iItem);
MRESULT EXPENTRY STControlWindowWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);


/*************************************************************************************
  CreateControl: Creates a control window.
*************************************************************************************/
HWND CreateControl(HWND hwndOwner, PSTCONTROL pStCtrl, int iType, int iItemId, int iCtrlId, int iCreateFlags, BOOL bUseHover, int iItemCount)
     {
     static BOOL        bIsClassRegistered = 0;
     char               szClass[] = "STControlWindowClass";
     char               szControlClass[16];

     ULONG              flFlags, ulFlags;
     BOOL               bNoExtraWindow;


     pStCtrl->hwndOwner = hwndOwner;
     pStCtrl->iItemId = iItemId;
     pStCtrl->iControlId = iCtrlId;
     pStCtrl->iItemCount = iItemCount;
     pStCtrl->iCurrentItem = 0;
     pStCtrl->bUseHover = bUseHover;
     pStCtrl->bShow = TRUE;

     pStCtrl->iType = iType;
     pStCtrl->iCreateFlags = iCreateFlags;

     /* Hover settings */
     if (pStCtrl->bUseHover == TRUE)
        {
        CreateHoverBox(HWND_DESKTOP, &pStCtrl->sthb);
        SetHoverBoxFont(&pStCtrl->sthb, "8.Helv");
        SetHoverBoxColor(&pStCtrl->sthb, CLR_BLACK, CLR_YELLOW);
        SetHoverBoxAlignment(&pStCtrl->sthb, HOVER_CENTER);
        SetHoverBoxMotion(&pStCtrl->sthb, FALSE);
        sprintf(pStCtrl->szFlyOverText1, "");
        sprintf(pStCtrl->szFlyOverText2, "");
        }

     /* Register main window class */
     if (!bIsClassRegistered)
        {
        WinRegisterClass(WinQueryAnchorBlock(hwndOwner), szClass,
                        (PFNWP)STControlWindowWndProc, CS_SIZEREDRAW | CS_SYNCPAINT, 4);
        bIsClassRegistered = TRUE;
        }

     bNoExtraWindow = TRUE;
     ulFlags = 0;
     switch (pStCtrl->iType)
            {
            case IDSTCTRL_TEXT:
                 strcpy(szControlClass, WC_STATIC);
                 ulFlags = SS_TEXT;
                 bNoExtraWindow = FALSE;
                 break;

            case IDSTCTRL_BUTTON:
                 strcpy(szControlClass, WC_BUTTON);
                 ulFlags = BS_PUSHBUTTON;
                 bNoExtraWindow = FALSE;
                 break;

            case IDSTCTRL_ENTRYFIELD:
                 strcpy(szControlClass, WC_ENTRYFIELD);
                 bNoExtraWindow = FALSE;
                 break;

            case IDSTCTRL_CHECK:
                 strcpy(szControlClass, WC_ENTRYFIELD);
                 ulFlags = BS_AUTOCHECKBOX;
                 bNoExtraWindow = FALSE;
                 break;

            case IDSTCTRL_RADIO:
                 strcpy(szControlClass, WC_ENTRYFIELD);
                 ulFlags = BS_AUTORADIOBUTTON;
                 bNoExtraWindow = FALSE;
                 break;

            case IDSTCTRL_COMBO:
                 strcpy(szControlClass, WC_COMBOBOX);
                 bNoExtraWindow = FALSE;
                 break;

            case IDSTCTRL_BITMAP:
                 break;

            case IDSTCTRL_STATE:
                 break;

            case IDSTCTRL_MANYSTATE:
                 ControlInitItems(pStCtrl);
                 break;

            case IDSTCTRL_ANIMATE:
                 ControlInitItems(pStCtrl);
                 break;

            case IDSTCTRL_PROGRESS:
                 break;

            case IDSTCTRL_DATETIME:
                 break;

            case IDSTCTRL_PRINT:
                 break;

            case IDSTCTRL_OWNERDRAW:
                 break;
            }

     /* Create main (background) window */
     flFlags = FCF_NOBYTEALIGN | WS_SYNCPAINT;
     pStCtrl->hwndControl = WinCreateWindow(pStCtrl->hwndOwner, szClass, (PSZ)NULL, flFlags, 0, 0, 0, 0, pStCtrl->hwndOwner, HWND_TOP, 0, NULL, NULL);

     if (bNoExtraWindow == FALSE)
        {
        pStCtrl->hwndIntControl = WinCreateWindow(pStCtrl->hwndControl, szControlClass, (PSZ)NULL,
                                                  ulFlags | pStCtrl->iCreateFlags, 0, 0, 0, 0,
                                                  pStCtrl->hwndControl, HWND_BOTTOM, 0, NULL, NULL);
        }

     WinSetWindowPtr(pStCtrl->hwndControl, 0, pStCtrl);
     return(pStCtrl->hwndControl);
     }


/*************************************************************************************
  ControlInitItems: allocates and initiates items array.
*************************************************************************************/
void ControlInitItems(PSTCONTROL pStCtrl)
     {
     int                iCounter;


     pStCtrl->pStCi = (PSTCONTROLITEM) calloc(pStCtrl->iItemCount, sizeof(STCONTROLITEM));

     for (iCounter = 0; iCounter < pStCtrl->iItemCount; iCounter++)
         {
         pStCtrl->pStCi[iCounter].iItemId = 0;
         pStCtrl->pStCi[iCounter].iItemType = IDSTCTRL_SUB_NONE;
         pStCtrl->pStCi[iCounter].ulResourceId = 0;
         pStCtrl->pStCi[iCounter].hResHandle = NULLHANDLE;
         sprintf(pStCtrl->pStCi[iCounter].szText, "");
         pStCtrl->pStCi[iCounter].iForeColor = CLR_BLACK;
         pStCtrl->pStCi[iCounter].iBackColor = CLR_PALEGRAY;
         sprintf(pStCtrl->pStCi[iCounter].szFlyOverText, "");
         }
     }


/*************************************************************************************
  DestroyControl: Kills of the contol.
*************************************************************************************/
BOOL DestroyControl(PSTCONTROL pStCtrl)
     {
     int        iCounter;


     /* Destroy any subitems */
     if (pStCtrl->iItemCount != 0)
        {
        for (iCounter = 0; iCounter < pStCtrl->iItemCount; iCounter++)
            {
            switch (pStCtrl->pStCi[iCounter].iItemType)
                   {
                   case IDSTCTRL_SUB_BITMAP:
                   case IDSTCTRL_SUB_HBITMAP:
                        GpiDeleteBitmap(pStCtrl->pStCi[iCounter].hResHandle);
                        break;
                   }
            }
        free(pStCtrl->pStCi);
        }

     /* Delete control specific data */
     switch (pStCtrl->iType)
            {
            case IDSTCTRL_BITMAP:
                 GpiDeleteBitmap(pStCtrl->hResHandle1);
                 break;

            case IDSTCTRL_STATE:
                 GpiDeleteBitmap(pStCtrl->hResHandle1);
                 GpiDeleteBitmap(pStCtrl->hResHandle2);
                 break;
            }

     /* Destroy windows */
     WinDestroyWindow(pStCtrl->hwndIntControl);
     return(WinDestroyWindow(pStCtrl->hwndControl));
     }


/*************************************************************************************
  ControlSetItem: Sets contence of one item.
*************************************************************************************/
BOOL ControlSetItem(PSTCONTROL pStCtrl, int iItem, int iItemId, int iItemType, HMODULE hMod, ULONG ulResId, char *szItemText, int iForeColor, int iBackColor, char *szFlyOverText)
     {
     HPS        hps;


     if (iItem + 1 > pStCtrl->iItemCount)
        return (FALSE);

     pStCtrl->pStCi[iItem].iItemId = iItemId;
     pStCtrl->pStCi[iItem].iItemType = iItemType;
     pStCtrl->pStCi[iItem].hMod = hMod;
     pStCtrl->pStCi[iItem].ulResourceId = ulResId;

     pStCtrl->pStCi[iItem].iForeColor = iForeColor;
     pStCtrl->pStCi[iItem].iBackColor = iBackColor;

     strcpy(pStCtrl->pStCi[iItem].szFlyOverText, szFlyOverText);

     hps = WinGetPS(pStCtrl->hwndOwner);
     switch (pStCtrl->pStCi[iItem].iItemType)
            {
           case IDSTCTRL_SUB_TEXT:
                strcpy(pStCtrl->pStCi[iItem].szText, szItemText);
                break;

            case IDSTCTRL_SUB_BITMAP:
                 pStCtrl->pStCi[iItem].hResHandle = GpiLoadBitmap(hps, pStCtrl->pStCi[iItem].hMod, pStCtrl->pStCi[iItem].ulResourceId, 0, 0);
                 break;

            case IDSTCTRL_SUB_HBITMAP:
                 pStCtrl->pStCi[iItem].hResHandle = pStCtrl->pStCi[iItem].ulResourceId;
                 break;

           case IDSTCTRL_SUB_COLOR:
                pStCtrl->pStCi[iItem].iBackColor = iBackColor;
                break;

           default:
                 break;
            }
     WinReleasePS(hps);

     return(TRUE);
     }


/*************************************************************************************
  ControlItemRealloc: Reallocates control items.
************************************************************************************/
void ControlItemRealloc(PSTCONTROL pStCtrl, int iItemCount)
     {
     int        iCounter;

     /* Destroy any existing subitems */
     if (pStCtrl->iItemCount != 0)
        {
        for (iCounter = 0; iCounter < pStCtrl->iItemCount; iCounter++)
            {
            switch (pStCtrl->pStCi[iCounter].iItemType)
                   {
                   case IDSTCTRL_SUB_BITMAP:
                   case IDSTCTRL_SUB_HBITMAP:
                        GpiDeleteBitmap(pStCtrl->pStCi[iCounter].hResHandle);
                        break;
                   }
            }
        free(pStCtrl->pStCi);
        }

     pStCtrl->iItemCount = iItemCount;
     ControlInitItems(pStCtrl);
     }


/*************************************************************************************
  ControlResize: Sizes the control window.
************************************************************************************/
int ControlResize(PSTCONTROL pStCtrl, int yPos, int xPos, int ySize, int xSize)
    {
    RECTL              prclClient;
    FONTMETRICS        fm;
    ULONG              yFont, xFont;
    HPS                hps;


    if (pStCtrl->bShow == FALSE || pStCtrl->hwndControl == NULLHANDLE)
       return ((int) MPFROM2SHORT(0, 0));

    switch (pStCtrl->iType)
           {
           case IDSTCTRL_TEXT:
           case IDSTCTRL_ENTRYFIELD:
           case IDSTCTRL_COMBO:
                hps = WinGetPS(pStCtrl->hwndControl);
                GpiQueryFontMetrics(hps, sizeof(fm), &fm);
                yFont = fm.lMaxBaselineExt;
                xFont = fm.lEmInc;
                WinReleasePS(hps);
                ySize = yPos + (STBORDERSPACE * 2) + yFont;
                break;

           case IDSTCTRL_DATETIME:
           case IDSTCTRL_CHECK:
           case IDSTCTRL_RADIO:
                hps = WinGetPS(pStCtrl->hwndControl);
                GpiQueryFontMetrics(hps, sizeof(fm), &fm);
                yFont = fm.lMaxBaselineExt;
                xFont = fm.lEmInc;
                WinReleasePS(hps);
                ySize = yPos + (STBORDERSPACE * 2) + yFont;
                xSize = xPos + (STBORDERSPACE * 2) + ( strlen(pStCtrl->szText1) * xFont);
                break;

           case IDSTCTRL_ANIMATE:
           case IDSTCTRL_PROGRESS:
           case IDSTCTRL_PRINT:
                break;

           }

    WinSetWindowPos(pStCtrl->hwndControl, HWND_TOP, xPos, yPos, xSize, ySize, SWP_SIZE | SWP_MOVE | SWP_SHOW | SWP_ZORDER);

    return ((int) MPFROM2SHORT(ySize, xSize));
    }


/*************************************************************************************
  ControlShow: Shows (or hides) the control window.
************************************************************************************/
BOOL ControlShow(PSTCONTROL pStCtrl, BOOL bShow)
     {

     if (pStCtrl->hwndControl == NULLHANDLE)
        return (FALSE);

     if (pStCtrl->bShow == bShow)
        return (pStCtrl->bShow);

     if (bShow == TRUE)
        {
        WinSetWindowPos(pStCtrl->hwndControl, 0, 0, 0, 0, 0, SWP_SHOW);
        if (pStCtrl->hwndIntControl != NULLHANDLE)
           WinSetWindowPos(pStCtrl->hwndIntControl, 0, 0, 0, 0, 0, SWP_SHOW);
        }
     else
        {
        WinSetWindowPos(pStCtrl->hwndControl, 0, 0, 0, 0, 0, SWP_HIDE);
        if (pStCtrl->hwndIntControl != NULLHANDLE)
           WinSetWindowPos(pStCtrl->hwndIntControl, 0, 0, 0, 0, 0, SWP_HIDE);
        }

     pStCtrl->bShow = bShow;

     return (!bShow);
     }



/*************************************************************************************
  ControlSetFont: Sets font (used with text display).
************************************************************************************/
void ControlSetFont(PSTCONTROL pStCtrl, char *szFont)
     {
     strcpy(pStCtrl->szFont, szFont);
     WinSetPresParam(pStCtrl->hwndControl, PP_FONTNAMESIZE, (ULONG)strlen(pStCtrl->szFont) + 1, (PVOID)&pStCtrl->szFont);
     WinSetPresParam(pStCtrl->hwndIntControl, PP_FONTNAMESIZE, (ULONG)strlen(pStCtrl->szFont) + 1, (PVOID)&pStCtrl->szFont);
     WinInvalidateRect(pStCtrl->hwndControl, NULL, FALSE);
     }


/*************************************************************************************
  ControlSetFlyOverText: Sets to use with hover (flyover).
************************************************************************************/
void ControlSetFlyOverText(PSTCONTROL pStCtrl, char *szFlyOverText1, char *szFlyOverText2)
     {
     strcpy(pStCtrl->szFlyOverText1, szFlyOverText1);
     strcpy(pStCtrl->szFlyOverText1, szFlyOverText2);
     }


/*************************************************************************************
  ControlSetEffect: Sets effect (2d or 3d).
************************************************************************************/
void ControlSetEffect(PSTCONTROL pStCtrl, int iEffect)
     {
     pStCtrl->iEffect = iEffect;
     WinInvalidateRect(pStCtrl->hwndControl, NULL, FALSE);
     }


/*************************************************************************************
  ControlSetText: Sets text of control window.
************************************************************************************/
void ControlSetText(PSTCONTROL pStCtrl, char *szText1, char *szText2)
     {

     strcpy(pStCtrl->szText1, szText1);
     strcpy(pStCtrl->szText2, szText2);

     switch (pStCtrl->iType)
            {
            case IDSTCTRL_TEXT:
            case IDSTCTRL_BUTTON:
            case IDSTCTRL_ENTRYFIELD:
            case IDSTCTRL_CHECK:
            case IDSTCTRL_RADIO:
            case IDSTCTRL_COMBO:
                 WinSetWindowText(pStCtrl->hwndIntControl, pStCtrl->szText1);
                 break;

            case IDSTCTRL_STATE:
                 WinInvalidateRect(pStCtrl->hwndControl, NULL, FALSE);
                 break;
            }
      }


/*************************************************************************************
  ControlGetText: Gets text of control window.
************************************************************************************/
char *ControlGetText(PSTCONTROL pStCtrl)
     {
     static char        szText[128];


     switch (pStCtrl->iType)
            {
            case IDSTCTRL_TEXT:
            case IDSTCTRL_BUTTON:
            case IDSTCTRL_ENTRYFIELD:
            case IDSTCTRL_CHECK:
            case IDSTCTRL_RADIO:
            case IDSTCTRL_COMBO:
                 WinQueryWindowText(pStCtrl->hwndIntControl, 127, szText);
                 return (szText);
                 break;
            }
     return ((char *) NULL);
     }


/*************************************************************************************
  ControlClear: Clears control.
************************************************************************************/
void ControlClear(PSTCONTROL pStCtrl)
     {

     switch (pStCtrl->iType)
            {
            case IDSTCTRL_TEXT:
            case IDSTCTRL_BUTTON:
            case IDSTCTRL_ENTRYFIELD:
                 strcpy(pStCtrl->szText1, "");
                 WinSetWindowText(pStCtrl->hwndIntControl, pStCtrl->szText1);
                 break;

            case IDSTCTRL_CHECK:
                 break;

            case IDSTCTRL_RADIO:
                 break;

            case IDSTCTRL_COMBO:
                 break;
            }
     }


/*************************************************************************************
  AnimateSetTimerId: Sets timer Id.
************************************************************************************/
void AnimateSetTimerId(PSTCONTROL pStCtrl, int iTimerId)
     {
     pStCtrl->iTimerId = iTimerId;
     }


/*************************************************************************************
  AnimateSetTimerTicks: Sets animation timer.
************************************************************************************/
void AnimateSetTimerTicks(PSTCONTROL pStCtrl, int iTimer)
     {
     pStCtrl->iTimerTicks = iTimer;
     }


/*************************************************************************************
  AnimateStart: Starts animation.
************************************************************************************/
BOOL AnimateStart(PSTCONTROL pStCtrl)
     {
     pStCtrl->iCurrentItem = 0;
     WinStartTimer(WinQueryAnchorBlock(pStCtrl->hwndControl), pStCtrl->hwndControl, pStCtrl->iTimerId, pStCtrl->iTimerTicks);
     return (TRUE);
     }


/*************************************************************************************
  AnimateStop: Stops animation.
************************************************************************************/
void AnimateStop(PSTCONTROL pStCtrl)
     {
     WinStopTimer(WinQueryAnchorBlock(pStCtrl->hwndControl), pStCtrl->hwndControl, pStCtrl->iTimerId);
     pStCtrl->iCurrentItem = 0;
     ControlDrawItem(pStCtrl, NULLHANDLE, pStCtrl->iCurrentItem);
     }


/*************************************************************************************
  ControlDrawItem: Draws specified item.
************************************************************************************/
void ControlDrawItem(PSTCONTROL pStCtrl, HPS hps, int iItem)
     {
     RECTL              rcl;
     BOOL               bOwnHps;


     if (hps == NULLHANDLE)
        bOwnHps = TRUE;
     else
        bOwnHps = FALSE;

     if (bOwnHps == TRUE)
        hps = WinGetPS(pStCtrl->hwndControl);

     WinQueryWindowRect(pStCtrl->hwndControl, &rcl);

     switch (pStCtrl->iType)
            {
            case IDSTCTRL_MANYSTATE:
            case IDSTCTRL_ANIMATE:
                 switch (pStCtrl->pStCi[iItem].iItemType)
                        {
                        case IDSTCTRL_SUB_BITMAP:
                        case IDSTCTRL_SUB_HBITMAP:
                             WinDrawBitmap(hps, (HBITMAP) pStCtrl->pStCi[iItem].hResHandle, 0, (PPOINTL)&rcl, CLR_NEUTRAL, CLR_BACKGROUND, DBM_NORMAL | DBM_STRETCH);
                             break;

                        case IDSTCTRL_SUB_TEXT:
                             WinDrawText(hps, -1, pStCtrl->pStCi[iItem].szText, &rcl,
                                         pStCtrl->pStCi[iItem].iForeColor, pStCtrl->pStCi[iItem].iBackColor,
                                         DT_CENTER | DT_ERASERECT | DT_VCENTER);
                             break;

                        case IDSTCTRL_SUB_COLOR:
                             WinFillRect(hps, &rcl, pStCtrl->pStCi[iItem].iBackColor);
                             break;

                        case IDSTCTRL_SUB_OWNERDRAW:
                             WinPostMsg(pStCtrl->hwndOwner, WM_CTRLMESSAGE, MPFROMLONG(pStCtrl), MPFROMLONG(WM_PAINT));
                             break;
                        }
            }

     if (bOwnHps == TRUE)
        WinReleasePS(hps);
     }


/*************************************************************************************
  STControlWindowWndProc: Handle Control window messages.
*************************************************************************************/
MRESULT EXPENTRY STControlWindowWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
        {
        PSTCONTROL         pStCtrl;
        HPS                hps;

        switch (msg)
               {
               case WM_PAINT:
                    pStCtrl = (PSTCONTROL) WinQueryWindowPtr(hwnd, 0);

                    hps = WinBeginPaint(hwnd, 0, 0);

                    switch (pStCtrl->iType)
                           {
                           case IDSTCTRL_TEXT:
                                break;

                           case IDSTCTRL_BITMAP:
                                break;

                           case IDSTCTRL_STATE:
                                break;

                           case IDSTCTRL_MANYSTATE:
                           case IDSTCTRL_ANIMATE:
                                ControlDrawItem(pStCtrl, hps, pStCtrl->iCurrentItem);
                                break;

                           case IDSTCTRL_PROGRESS:
                                break;

                           case IDSTCTRL_DATETIME:
                                break;

                           case IDSTCTRL_PRINT:
                                break;

                           case IDSTCTRL_OWNERDRAW:
                                pStCtrl->mp1 = mp1;
                                pStCtrl->mp2 = mp2;
                                pStCtrl->hpsPaint = hps;
                                WinSendMsg(pStCtrl->hwndOwner, WM_CTRLMESSAGE, MPFROMLONG(pStCtrl->iControlId), MPFROMLONG(msg));
                                break;
                     }
                    WinEndPaint(hps);
                    break;


               case WM_BUTTON1CLICK:
               case WM_BUTTON2CLICK:
               case WM_BUTTON3CLICK:
               case WM_BUTTON1DBLCLK:
               case WM_BUTTON2DBLCLK:
               case WM_BUTTON3DBLCLK:
                    pStCtrl = (PSTCONTROL) WinQueryWindowPtr(hwnd, 0);
                    pStCtrl->mp1 = mp1;
                    pStCtrl->mp2 = mp2;
                    WinSendMsg(pStCtrl->hwndOwner, WM_CTRLMESSAGE, MPFROMLONG(pStCtrl->iControlId), MPFROMLONG(msg));
                    return (WinDefWindowProc(hwnd, msg, mp1, mp2));
                    break;


               case WM_SIZE:
                    pStCtrl = (PSTCONTROL) WinQueryWindowPtr(hwnd, 0);
                    break;


               case WM_TIMER:
                    pStCtrl = (PSTCONTROL) WinQueryWindowPtr(hwnd, 0);

                    if (LONGFROMMP(mp1) == pStCtrl->iTimerId)
                       {
                       if (pStCtrl->iCurrentItem >= pStCtrl->iItemCount)
                          pStCtrl->iCurrentItem = 0;

                       WinInvalidateRect(pStCtrl->hwndControl, NULL, FALSE);
                       pStCtrl->iCurrentItem++;
                       }
                    break;


               case WM_MOUSEMOVE:
                    pStCtrl = (PSTCONTROL) WinQueryWindowPtr(hwnd, 0);

                    if (pStCtrl->bUseHover != FALSE)
                       {
                       SetHoverBoxText(&pStCtrl->sthb, pStCtrl->szFlyOverText1);
                       ShowHoverBox(&pStCtrl->sthb, hwnd, 0, TRUE);
                       }
                    return WinDefWindowProc (hwnd, msg, mp1, mp2);
                    break;


               default:
                    return (WinDefWindowProc(hwnd, msg, mp1, mp2));
               }
        return (FALSE);
        }


