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
   BTNBAR.C
  ==========================================================================
   921211 Converted to CSET/2
  ==========================================================================
   930613 Changed ICON into BITMAP
  ==========================================================================
   960527 Added highlite and enabled code.
  ==========================================================================
   960528 Changed StdWindow to Window so not to steal focus + fancy look.
  ==========================================================================
   960818 Added SyncPaint styles to look better when resize.
  ==========================================================================
   960818 Added Hoverbar and rebuilt using struct.
  ==========================================================================
   970427 Added support for two bitmaps and two flyover texts.
  ==========================================================================
****************************************************************************/
#define INCL_WIN

#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include "stlibpmc.h"

/*************************************************************************************/
MRESULT EXPENTRY STButtonBarWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY STButtonBarSubClassButtonWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
PFNWP DefaultButtonProc;
int ButtonBarPosFromItem(PSTBTNBAR pBtnBar, int iItemId);

/*************************************************************************************/
STHOVERBOX      hb;

/*************************************************************************************
  CreateButtonBar: Creates buttonbar & loads icons.
*************************************************************************************/
HWND CreateButtonBar(HWND hwndOwner, PSTBTNBAR pBtnBar, int iBarType, USHORT usItems)
     {
     static BOOL        bIsClassRegistered = 0;
     PSZ                pszClass = "STButtonBarClass";
     int                iCounter;
     ULONG              flFlags;


     if (!bIsClassRegistered)
        {
        WinRegisterClass(WinQueryAnchorBlock(hwndOwner), pszClass,
                        (PFNWP)STButtonBarWndProc, CS_SIZEREDRAW | CS_SYNCPAINT, 4);
        bIsClassRegistered = TRUE;
        }

     flFlags = FCF_BORDER | FCF_NOBYTEALIGN | WS_SYNCPAINT;

     pBtnBar->hwndButtonBar = WinCreateWindow(hwndOwner, pszClass, (PSZ)NULL, flFlags, 0, 0, 0, 0,
                                              hwndOwner, HWND_TOP, 0, NULL, NULL);

     WinSetWindowPtr(pBtnBar->hwndButtonBar, 0, pBtnBar);

     pBtnBar->hwndOwner = hwndOwner;
     pBtnBar->usItemCount = usItems;
     pBtnBar->ulBarType = iBarType;

     pBtnBar->pBtnItems = (PSTBTNBARITEM) calloc(usItems, sizeof(STBTNBARITEM));
 
     for (iCounter = 0; iCounter < pBtnBar->usItemCount; iCounter++)
         {
         pBtnBar->pBtnItems[iCounter].iItemId = 0;
         pBtnBar->pBtnItems[iCounter].iItemType = BBT_SPACE;
         pBtnBar->pBtnItems[iCounter].ulResId1 = 0;
         pBtnBar->pBtnItems[iCounter].ulResId2 = 0;
         pBtnBar->pBtnItems[iCounter].hResHandle1 = 0;
         pBtnBar->pBtnItems[iCounter].hResHandle2 = 0;
         pBtnBar->pBtnItems[iCounter].usState = BBS_ENABLED;
         sprintf(pBtnBar->pBtnItems[iCounter].szFlyoverText1, "");
         sprintf(pBtnBar->pBtnItems[iCounter].szFlyoverText2, "");
         }

     CreateHoverBox(HWND_DESKTOP, &hb);
     SetHoverBoxFont(&hb, "8.Helv");
     SetHoverBoxColor(&hb, CLR_BLACK, CLR_YELLOW);
     SetHoverBoxAlignment(&hb, HOVER_CENTER);
     SetHoverBoxMotion(&hb, FALSE);

     return(pBtnBar->hwndButtonBar);
     }


/*************************************************************************************
  DestroyButtonBar: Kills buttonbar & unloads icons.
*************************************************************************************/
BOOL DestroyButtonBar(PSTBTNBAR pBtnBar)
     {
     int        iCounter;


     for (iCounter = 0; iCounter < pBtnBar->usItemCount; iCounter++)
         {
         switch (pBtnBar->pBtnItems[iCounter].iItemType)
                {
                case BBT_BITMAP:
                     GpiDeleteBitmap(pBtnBar->pBtnItems[iCounter].hResHandle1);
                     GpiDeleteBitmap(pBtnBar->pBtnItems[iCounter].hResHandle2);
                     break;
                }
         }

     free(pBtnBar->pBtnItems);

     DestroyHoverBox(&hb);

     return(WinDestroyWindow(pBtnBar->hwndButtonBar));
     }


/*************************************************************************************
  SetButtonBarItem: Sets contence of item.
*************************************************************************************/
BOOL SetButtonBarItem(PSTBTNBAR pBtnBar, int iItem, int iItemId, int iItemType, ULONG ulResId1, ULONG ulResId2, USHORT usState, char *szFlyoverText1, char *szFlyoverText2)
     {
     HPS        hps;


     if (iItem + 1 > pBtnBar->usItemCount)
        return (FALSE);

     pBtnBar->pBtnItems[iItem].iItemId = iItemId;
     pBtnBar->pBtnItems[iItem].iItemType = iItemType;
     pBtnBar->pBtnItems[iItem].ulResId1 = ulResId1;
     pBtnBar->pBtnItems[iItem].ulResId2 = ulResId2;
     pBtnBar->pBtnItems[iItem].usState = usState;

     hps = WinGetPS(pBtnBar->hwndOwner);
     switch (pBtnBar->pBtnItems[iItem].iItemType)
            {
            case BBT_BITMAP:
                 pBtnBar->pBtnItems[iItem].hResHandle1 = GpiLoadBitmap(hps, (ULONG) NULL,
                                                                      pBtnBar->pBtnItems[iItem].ulResId1, 0, 0);
                 pBtnBar->pBtnItems[iItem].hResHandle2 = GpiLoadBitmap(hps, (ULONG) NULL,
                                                                      pBtnBar->pBtnItems[iItem].ulResId2, 0, 0);
                 break;

            case BBT_SPACE:
                 pBtnBar->pBtnItems[iItem].hResHandle1 = NULLHANDLE;
                 pBtnBar->pBtnItems[iItem].hResHandle2 = NULLHANDLE;
                 break;
            }
     WinReleasePS(hps);

     if (strlen(szFlyoverText1) < 1)
        pBtnBar->pBtnItems[iItem].szFlyoverText1[0] = '\0';
     else
        sprintf(pBtnBar->pBtnItems[iItem].szFlyoverText1, szFlyoverText1);

     if (strlen(szFlyoverText2) < 1)
        pBtnBar->pBtnItems[iItem].szFlyoverText2[0] = '\0';
     else
        sprintf(pBtnBar->pBtnItems[iItem].szFlyoverText2, szFlyoverText2);

     return(TRUE);
     }


/*************************************************************************************
  DrawButtonBar: Positions/sizes all defined items.
*************************************************************************************/
void DrawButtonBar(PSTBTNBAR pBtnBar, int ySize)
     {
     HWND       hwndButton;
     int        iCounter;
     int        iPos;
     RECTL      rcl;

     if (ySize == 0 && pBtnBar->ulBarType == BBP_LEFT)
        {
        WinQueryWindowRect(pBtnBar->hwndOwner, &rcl);
        ySize = rcl.yTop;
        }

     switch (pBtnBar->ulBarType)
            {
            case BBP_LEFT:
                 iPos = ySize - YICO - STBORDERSPACE;
                 for (iCounter = 0; iCounter < pBtnBar->usItemCount; iCounter++)
                     {
                     switch (pBtnBar->pBtnItems[iCounter].iItemType)
                            {
                            case BBT_BITMAP:
                                 hwndButton = WinCreateWindow(pBtnBar->hwndButtonBar, WC_BUTTON, (PSZ) NULL,
                                                              WS_VISIBLE | BS_PUSHBUTTON | BS_USERBUTTON | BS_NOPOINTERFOCUS,
                                                              STBORDERSPACE, iPos, XICO, YICO,
                                                              pBtnBar->hwndButtonBar, HWND_BOTTOM,
                                                              pBtnBar->pBtnItems[iCounter].iItemId, 0, 0);
                                 WinSetWindowPtr(hwndButton, 0, &pBtnBar->pBtnItems[iCounter]);
                                 DefaultButtonProc = WinSubclassWindow(hwndButton, (PFNWP) STButtonBarSubClassButtonWndProc);
                                 iPos = iPos - YICO - STBORDERSPACE;
                                 break;

                            case BBT_SPACE:
                                 iPos = iPos - (YICO / 4) - STBORDERSPACE;
                                 break;
                            }
                     }

            case BBP_TOP:
                 iPos = STBORDERSPACE;
                 for (iCounter = 0; iCounter < pBtnBar->usItemCount; iCounter++)
                     {
                     switch (pBtnBar->pBtnItems[iCounter].iItemType)
                            {
                            case BBT_BITMAP:
                                 hwndButton = WinCreateWindow(pBtnBar->hwndButtonBar, WC_BUTTON, (PSZ) NULL,
                                                              WS_VISIBLE | BS_PUSHBUTTON | BS_USERBUTTON | BS_NOPOINTERFOCUS,
                                                              iPos, STBORDERSPACE, XICO, YICO,
                                                              pBtnBar->hwndButtonBar, HWND_BOTTOM,
                                                              pBtnBar->pBtnItems[iCounter].iItemId, 0, 0);
                                 WinSetWindowPtr(hwndButton, 0, &pBtnBar->pBtnItems[iCounter]);
                                 DefaultButtonProc = WinSubclassWindow(hwndButton, (PFNWP) STButtonBarSubClassButtonWndProc);
                                 iPos = iPos + XICO + STBORDERSPACE;
                                 break;

                            case BBT_SPACE:
                                 iPos = iPos + (XICO / 4) + STBORDERSPACE;
                                 break;
                            }
                     }
            }
     }


/*************************************************************************************
  HiliteButton: Sets state of button.
************************************************************************************/
BOOL HiliteButton(PSTBTNBAR pBtnBar, int iItem, BOOL bHilite)
     {
     HWND       hwndButton;
     int        iOldState, iPos;


     iPos = ButtonBarPosFromItem(pBtnBar, iItem);
     iOldState = pBtnBar->pBtnItems[iPos].usState;

     if (bHilite == TRUE)
         pBtnBar->pBtnItems[iPos].usState = BBS_HIGHLITED;
     else
         pBtnBar->pBtnItems[iPos].usState = BBS_ENABLED;

     WinInvalidateRect(WinWindowFromID(pBtnBar->hwndButtonBar, pBtnBar->pBtnItems[iPos].iItemId),
                       NULL, FALSE);

     return (iOldState);
     }


/*************************************************************************************
  EnableButton: Sets state of button.
************************************************************************************/
BOOL EnableButton(PSTBTNBAR pBtnBar, int iItem, BOOL bEnable)
     {
     HWND       hwndButton;
     int        iOldState, iPos;


     iPos = ButtonBarPosFromItem(pBtnBar, iItem);
     iOldState = pBtnBar->pBtnItems[iPos].usState;

     if (bEnable == TRUE)
         pBtnBar->pBtnItems[iPos].usState = BBS_ENABLED;
     else
         pBtnBar->pBtnItems[iPos].usState = BBS_DISABLED;

     WinInvalidateRect(WinWindowFromID(pBtnBar->hwndButtonBar, pBtnBar->pBtnItems[iPos].iItemId),
                       NULL, FALSE);

     return (iOldState);
     }


/*************************************************************************************
  IsButtonHilited: Querys state of button.
************************************************************************************/
BOOL IsButtonHilited(PSTBTNBAR pBtnBar, int iItem)
     {
     HWND       hwndButton, iPos;


     iPos = ButtonBarPosFromItem(pBtnBar, iItem);

     if (pBtnBar->pBtnItems[iPos].usState == BBS_HIGHLITED)
        return(TRUE);
     else
        return(FALSE);
     }


/*************************************************************************************
  IsButtonEnabled: Querys state of button.
************************************************************************************/
BOOL IsButtonEnabled(PSTBTNBAR pBtnBar, int iItem)
     {
     HWND       hwndButton, iPos;


     iPos = ButtonBarPosFromItem(pBtnBar, iItem);

     if (pBtnBar->pBtnItems[iPos].usState == BBS_DISABLED)
        return(FALSE);
     else
        return(TRUE);
     }


/*************************************************************************************
  STButtonBarWndProc: Handle Button Paint/presses.
*************************************************************************************/
MRESULT EXPENTRY STButtonBarWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
        {
        RECTL              rcl;
        PUSERBUTTON        ubInfo;
        HPS                hps;
        ULONG              y, ulFlag;
        int                iItemId, iPos;
        int                iCounter;
        POINTL             pp;
        PSTBTNBAR          pBtnBar;
        HBITMAP            hbmUse;


        switch (msg)
               {

               case WM_PAINT:
                    hps = WinBeginPaint(hwnd, 0, 0);
                    WinQueryWindowRect(hwnd, &rcl);
                    WinFillRect(hps, &rcl, CLR_PALEGRAY);

                    GpiSetColor(hps, CLR_WHITE);

                    pp.x = 1;
                    pp.y = 0;
                    GpiMove(hps, &pp);
                    pp.x = 1;
                    pp.y = rcl.yTop;
                    GpiLine(hps, &pp);

                    pp.x = 0;
                    pp.y = rcl.yTop - 1;
                    GpiMove(hps, &pp);
                    pp.x = rcl.xRight;
                    pp.y = rcl.yTop - 1;
                    GpiLine(hps, &pp);

                    GpiSetColor(hps, CLR_DARKGRAY);

                    pp.x = 0;
                    pp.y = 0;
                    GpiMove(hps, &pp);
                    pp.x = rcl.xRight;
                    pp.y = 0;
                    GpiLine(hps, &pp);

                    pp.x = rcl.xRight - 1;
                    pp.y = 0;
                    GpiMove(hps, &pp);
                    pp.x = rcl.xRight - 1;
                    pp.y = rcl.yTop;
                    GpiLine(hps, &pp);

                    WinEndPaint(hps);
                    break;

               case WM_CONTROL:
                    pBtnBar = (PSTBTNBAR) WinQueryWindowPtr(hwnd, 0);
                    switch (SHORT2FROMMP(mp1))
                           {
                           case BN_CLICKED:
                                WinPostMsg(pBtnBar->hwndOwner, WM_BUTTONBARSELECT,
                                           (PVOID) SHORT1FROMMP(mp1), NULL);
                                break;

                           case BN_PAINT:
                                iPos = ButtonBarPosFromItem(pBtnBar, SHORT1FROMMP(mp1));
                                y = LONGFROMMP(mp2);
                                ubInfo = (PUSERBUTTON) y;

                                switch (ubInfo->fsState)
                                       {
                                       /* Draw normal, select bitmap (normal/selected) 
                                        ***********************************************/
                                       case BDS_DEFAULT:
                                            switch (pBtnBar->pBtnItems[iPos].usState)
                                                   {
                                                   case BBS_ENABLED:
                                                        hbmUse = pBtnBar->pBtnItems[iPos].hResHandle1;
                                                        ulFlag = DBM_NORMAL;
                                                        break;

                                                   case BBS_HIGHLITED:
                                                        if (pBtnBar->pBtnItems[iPos].hResHandle2 != NULLHANDLE)
                                                           {
                                                           hbmUse = pBtnBar->pBtnItems[iPos].hResHandle2;
                                                           ulFlag = DBM_NORMAL;
                                                           }
                                                        else
                                                           {
                                                           hbmUse = pBtnBar->pBtnItems[iPos].hResHandle1;
                                                           ulFlag = DBM_INVERT;
                                                           }
                                                        break;

                                                   case BBS_DISABLED:
                                                        hbmUse = pBtnBar->pBtnItems[iPos].hResHandle1;
                                                        ulFlag = DBM_HALFTONE;
                                                        break;
                                                   }
                                            pp.x = 0;
                                            pp.y = 0;
                                            WinDrawBitmap(ubInfo->hps, hbmUse, 0, &pp, CLR_NEUTRAL, CLR_BACKGROUND, ulFlag);
                                            return ((MRESULT) TRUE);
                                            break;


                                       /* Draw highlighted (user clicked) , select bitmap (normal/selected) 
                                        *********************************************************************/
                                       case BDS_HILITED:
                                            switch (pBtnBar->pBtnItems[iPos].usState)
                                                   {
                                                   case BBS_ENABLED:
                                                        if (pBtnBar->pBtnItems[iPos].hResHandle2 != NULLHANDLE)
                                                           {
                                                           hbmUse = pBtnBar->pBtnItems[iPos].hResHandle2;
                                                           ulFlag = DBM_NORMAL;
                                                           }
                                                        else
                                                           {
                                                           hbmUse = pBtnBar->pBtnItems[iPos].hResHandle1;
                                                           ulFlag = DBM_INVERT;
                                                           }
                                                        break;

                                                   case BBS_HIGHLITED:
                                                        if (pBtnBar->pBtnItems[iPos].hResHandle2 != NULLHANDLE)
                                                           {
                                                           hbmUse = pBtnBar->pBtnItems[iPos].hResHandle1;
                                                           ulFlag = DBM_NORMAL;
                                                           }
                                                        else
                                                           {
                                                           hbmUse = pBtnBar->pBtnItems[iPos].hResHandle1;
                                                           ulFlag = DBM_NORMAL;
                                                           }
                                                        break;

                                                   case BBS_DISABLED:
                                                        hbmUse = pBtnBar->pBtnItems[iPos].hResHandle1;
                                                        ulFlag = DBM_HALFTONE;
                                                        break;
                                                   }
                                            pp.x = 0;
                                            pp.y = 0;
                                            WinDrawBitmap(ubInfo->hps, hbmUse, 0, &pp, CLR_NEUTRAL, CLR_BACKGROUND, ulFlag);
                                            return ((MRESULT) TRUE);
                                            break;


                                       /* Draw disabled, allways use the first bitmap */
                                       case BDS_DISABLED:
                                            hbmUse = pBtnBar->pBtnItems[iPos].hResHandle1;
                                            pp.x = 0;
                                            pp.y = 0;
                                            WinDrawBitmap(ubInfo->hps, hbmUse, 0, &pp, CLR_NEUTRAL, CLR_BACKGROUND, DBM_HALFTONE);
                                            return ((MRESULT) TRUE);
                                            break;


                                       default:
                                            switch (pBtnBar->pBtnItems[iPos].usState)
                                                   {
                                                   case BBS_ENABLED:
                                                        hbmUse = pBtnBar->pBtnItems[iPos].hResHandle1;
                                                        ulFlag = DBM_NORMAL;
                                                        break;

                                                   case BBS_HIGHLITED:
                                                        if (pBtnBar->pBtnItems[iPos].hResHandle2 != NULLHANDLE)
                                                           {
                                                           hbmUse = pBtnBar->pBtnItems[iPos].hResHandle2;
                                                           ulFlag = DBM_NORMAL;
                                                           }
                                                        else
                                                           {
                                                           hbmUse = pBtnBar->pBtnItems[iPos].hResHandle1;
                                                           ulFlag = DBM_INVERT;
                                                           }
                                                        break;

                                                   case BBS_DISABLED:
                                                        hbmUse = pBtnBar->pBtnItems[iPos].hResHandle1;
                                                        ulFlag = DBM_HALFTONE;
                                                        break;
                                                   }
                                            pp.x = 0;
                                            pp.y = 0;
                                            WinDrawBitmap(ubInfo->hps, hbmUse, 0, &pp, CLR_NEUTRAL, CLR_BACKGROUND, ulFlag);
                                            return ((MRESULT) TRUE);
                                            break;
                                       }

                                break;
                           }
               default:
                    return (WinDefWindowProc(hwnd, msg, mp1, mp2));
               }
        return (FALSE);
        }


/*************************************************************************************
  SizeButtonBar: Sizes the button bar.
************************************************************************************/
ULONG SizeButtonBar(PSTBTNBAR pBtnBar, ULONG ulBtnLen)
      {
      RECTL     rclOwner;
      int       xpos, xsize, ypos, ysize;


      if (pBtnBar->hwndButtonBar == NULLHANDLE)
         return (0);

      WinQueryWindowRect(pBtnBar->hwndOwner, &rclOwner);
      switch (pBtnBar->ulBarType)
             {
             case BBP_LEFT:
                  xpos = rclOwner.xLeft;
                  xsize = XICO + (STBORDERSPACE * 2);
                  ysize = ulBtnLen;
                  if (ysize == 0)
                     ysize = rclOwner.yTop;
                  ypos = rclOwner.yBottom;
                  WinSetWindowPos(pBtnBar->hwndButtonBar, HWND_TOP,
                                  xpos, ypos, xsize, ysize, 
                                  SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ZORDER);
                  return (xsize);
                  break;

             case BBP_TOP:
                  ypos = rclOwner.yTop - YICO - (STBORDERSPACE * 2);
                  ysize = YICO + (STBORDERSPACE * 2);
                  xsize = ulBtnLen;
                  if (xsize == 0)
                     xsize = rclOwner.xRight;
                  xpos = 0;
                  WinSetWindowPos(pBtnBar->hwndButtonBar, HWND_TOP,
                                  xpos, ypos, xsize, ysize, 
                                  SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_ZORDER);
                  return (ysize);
                  break;
             }
      return (0);
      }

/*************************************************************************************
  ButtonBarPosFromItem: Get button bar pos (in array).
************************************************************************************/
int ButtonBarPosFromItem(PSTBTNBAR pBtnBar, int iItemId)
      {
      int       iCounter;


      for (iCounter = 0; iCounter < pBtnBar->usItemCount; iCounter++)
          {
          if (pBtnBar->pBtnItems[iCounter].iItemId == iItemId)
             {
             return(iCounter);
             break;
             }
          }
      return(-1);
      }


/*-----------------------------------------------------------------------------
   STButtonBarSubClassButtonWndProc: Subclassed button WndProc.
-----------------------------------------------------------------------------*/
MRESULT EXPENTRY STButtonBarSubClassButtonWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
     {
     PSTBTNBARITEM      pBtnBarItem;
     char               szUseText[64];


     switch (msg)
            {
            case WM_MOUSEMOVE:
                 pBtnBarItem = (PSTBTNBARITEM) WinQueryWindowPtr(hwnd, 0);

                 switch (pBtnBarItem->usState)
                        {
                        case BBS_HIGHLITED:
                             strcpy(szUseText, pBtnBarItem->szFlyoverText2);
                             break;

                        default:
                             strcpy(szUseText, pBtnBarItem->szFlyoverText1);
                             break;
                        }

                 if (strlen(szUseText) > 0)
                    {
                    SetHoverBoxText(&hb, szUseText);
                    ShowHoverBox(&hb, hwnd, pBtnBarItem->iItemId, TRUE);
                    }
                 return WinDefWindowProc (hwnd, msg, mp1, mp2);

            default:
                 return ((MRESULT) DefaultButtonProc(hwnd, msg, mp1, mp2));
            }
     return (FALSE);
     }

