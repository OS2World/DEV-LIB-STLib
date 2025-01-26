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
   950601 Started to create extended version.
  ==========================================================================
****************************************************************************/
#define INCL_WIN

#include <os2.h>
#include <stdlib.h>
#include "stlibpmc.h"



/**********************************************************
typedef struct _BBITEM     btnbaritem 
        {
        HWND            hwndItem;
        USHORT          usBBItemType;
        RECTL           rclSizePos;
        USHORT          usDraw;
        ULONG           ulItemStyle;
        BOOL            bIdType;
        ULONG           ulItemId;
        ULONG           ulHandle;
        BOOL            bHelpTextType;
        ULONG           ulHelpTextId;
        CHAR            szHelpText[80];
        BOOL            bHelpWavType;
        ULONG           ulHelpWavId;
        ULONG           ulHelpWavHandle;
        HWND            hwndStatus;
        } BBITEM;

typedef BBITEM *PBBITEM;


typedef struct _BBINFO    btnbarinfo 
        {
        HWND            hwndButtonBar;
        USHORT          usItemCount;
        RECTL           rclSizePos;
        ULONG           ulPosition;
        BOOL            bFixedItemSize;
        PBBITEM         pbbItems;
        } BBINFO;

typedef BBINFO *PBBINFO;


#define FLBB_FIXEDSIZE          FALSE
#define FLBB_NOTFIXED           TRUE

#define FLBB_TOP                0x001
#define FLBB_BOTTOM             0x002
#define FLBB_LEFT               0x003
#define FLBB_RIGHT              0x004
#define FLBB_FLOAT              0x005

#define BBRT_RESID              FALSE
#define BBRT_OTHER              TRUE

#define BBIT_ICON               0x001
#define BBIT_BITMAP             0x002
#define BBIT_META               0x003
#define BBIT_TEXT               0x004
#define BBIT_COMBO              0x005
#define BBIT_OWNERCOMBO         0x006
#define BBIT_SPACE              0x007

#define BBIDRAW_ENABLED         0x001
#define BBIDRAW_DISABLED        0x002
#define BBIDRAW_HIGHLITED       0x003

#define WM_BBCLICK              WM_USER + 1100L

**********************************************************/


MRESULT EXPENTRY BBWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

/*************************************************************************************
  BBAllocate: Allocates struct's and registers BB window class.                950601
*************************************************************************************/
PBBINFO BBAllocate (HWND hwndApp, USHORT usItemCount)
     {
     PBBINFO    pbbinfo;
     INT        iCounter;

 
     pbbinfo = (PBBINFO) calloc(1, sizeof(BBINFO));
     
     pbbinfo->usItemCount    = usItemCount;
     pbbinfo->ulPosition     = FLBB_TOP;
     pbbinfo->bFixedItemSize = FLBB_FIXEDSIZE;

     pbbinfo->pbbItems = (PBBINFO) calloc(usItemCount, sizeof(BBITEM));
 
     for (iCounter = 0; iCounter < usItemCount; iCounter++)
         {
         pbbinfo->pbbItems[iCounter]->usBBItemType = BBIT_BITMAP;
         pbbinfo->pbbItems[iCounter]->usDraw       = BBIDRAW_ENABLED;
         }
 
     return (ppinfo);
     }


/*************************************************************************************
  BBDeAllocate: Deallocates struct's and optionally deregisters BB window class.
*************************************************************************************/
BOOL BBDeAllocate (HWND hwndApp, PBBINFO pbbInfo)
     {
     free (pbbinfo->pbbItems);
     free (pbbinfo);
     }


/*************************************************************************************
  BBCreate: Creates (empty) BB window.                                       950601
*************************************************************************************/
HWND BBCreate (HWND hwndOwner, PBBINFO pbbInfo)
     {
     static BOOL        bIsClassRegistred = FALSE;

     PBBITEM    pbbItem;
     PSZ        pszClassName = "STBBClass";
     ULONG      flFlags;
     HWND       hwndBBClient;
     HPS        hps;


     if (!bIsClassRegistred)
        {
        WinRegisterClass(WinQueryAnchorBlock(hwndOwner), pszClassName,
                         (PFNWP)BBWndProc, CS_SYNCPAINT | CS_SIZEREDRAW, 0);
        bIsClassRegistred = TRUE;
        }

     flFlags = FCF_BORDER | FCF_NOBYTEALIGN;

     pbbInfo->hwndButtonBar = WinCreateStdWindow(hwndOwner, 0L, (PVOID)&flFlags, pszClass,
                                                 (PSZ)NULL, 0L, 0L, (HMODULE)NULL, &hwndBBClient);


     hps = WinGetPS(hwndOwner);
     for (iCounter = 0; iCounter < pbbInfo->usItemCount; iCounter++)
         {
         pbbItem = pbbInfo->pbbItems[iCounter];


         switch (pbbItem->usBBItemType)
                {
                case BBIT_BITMAP:
                     if (pbbItem->->bIdType == FALSE)
                        pbbItem->ulHandle = GpiLoadBitmap(hps, (ULONG) NULL, pbbItem->ulItem, 0L, 0L);
                     else
                        pbbItem->ulHandle = pbbItem->ulItem;
                     break;

                case BBIT_ICON:
                     break;

                case BBIT_META:
                     break;

                case BBIT_TEXT:
                     break;

                case BBIT_COMBO:
                     break;

                case BBIT_OWNERCOMBO:
                     break;

                case BBIT_SPACE:
                     break;

                }
         }
     WinReleasePS(hps);

     return(pbbInfo->hwndButtonBar);
     }


/*************************************************************************************
  BBDestroy: Destroys BB window.                                       950601
*************************************************************************************/
HWND BBDestroy (PBBINFO pbbInfo)
     {
     }


/*************************************************************************************
  CreateButtonBar: Creates buttonbar & loads icons.
*************************************************************************************/
HWND CreateButtonBar (HAB hab, HWND hwnd, ULONG ulBtnArray[], USHORT usButtons, USHORT usBartype)
     {
     HWND       hwndOwner;
     PSZ        pszClass = "STButtonBarClass";
     USHORT     usCounter;
     ULONG      flFlags;
     HPS	hps;

     hwndOwner = hwnd;

     if (!bIsReg)
	{
	WinRegisterClass(hab, pszClass, (PFNWP)ButtonBarWndProc, CS_SIZEREDRAW, 0);
	bIsReg = TRUE;
	}

     usItems = usButtons;

     btnbar = (PBTNBARITEM) malloc(sizeof(BTNBARITEM) * usItems);

     /* Load each of the buttonbar bitmaps into memory and add them to the
        btnbar[] array
	================================================================== */ 
     hps = WinGetPS(hwnd);
     for(usCounter = 0;usCounter < usItems;usCounter++)
	 btnbar[usCounter].ulResId = GpiLoadBitmap(hps, (ULONG) NULL, ulBtnArray[usCounter], 0L, 0L);
     WinReleasePS(hps);

     xico  = 24;
     yico  = 24;
     xbrd  = WinQuerySysValue(HWND_DESKTOP, SV_CXBORDER);
     ybrd  = WinQuerySysValue(HWND_DESKTOP, SV_CYBORDER);

     barpos = usBartype;
     
     ysize = yico * usItems;


     flFlags = FCF_BORDER | FCF_NOBYTEALIGN;

     hwndBtn = WinCreateStdWindow(hwnd, 0L, (PVOID)&flFlags, pszClass, (PSZ)NULL, 0L, 0L, (HMODULE)NULL, &hwndBtnChild);

     return(hwndBtn);
     }


/*************************************************************************************
  DestroyButtonBar: Kills buttonbar & unloads icons.
*************************************************************************************/
BOOL DestroyButtonBar (HWND hwnd)
     {
     USHORT     usCounter;

     for (usCounter = 0;usCounter < usItems;usCounter++)
	 GpiDeleteBitmap(btnbar[usCounter].ulResId);

     return(WinDestroyWindow(hwnd));
     }


/*************************************************************************************
  MakeButton: Create and size button.
************************************************************************************/
HWND MakeButton (HWND hwnd, ULONG xpos, ULONG ypos, ULONG xsize, ULONG ysize, USHORT usButtonid)
     {
     return (WinCreateWindow(hwnd, WC_BUTTON, (PSZ) NULL, WS_VISIBLE | BS_PUSHBUTTON | BS_USERBUTTON | BS_NOPOINTERFOCUS, xpos, ypos, xsize, ysize, hwnd, HWND_BOTTOM, usButtonid, 0L, 0L));
     }


/*************************************************************************************
  BBWndProc: Handle Button Paint/presses.
*************************************************************************************/
MRESULT EXPENTRY BBWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
	{ 

	HWND		   hwndTmp;
	RECTL              rcl;
	PUSERBUTTON        ubInfo;
	HPS                hps;
	ULONG              y, ulFlag;
	USHORT             usCounter, iPos;
	POINTL		   pp;

	switch (msg)
	       {
	       case WM_CREATE:
		    switch (barpos)
			   {
			   case BP_TOP:
				for (usCounter = 0;usCounter < usItems;usCounter++)
				    hwndTmp = MakeButton (hwnd, (usCounter) * xico, 0, xico, yico, usCounter);
				break;

			   case BP_LEFT:
				y = ysize - yico;
				for (usCounter = 0;usCounter < usItems;usCounter++)
				    {
				    hwndTmp = MakeButton (hwnd, 0, y, xico, yico, usCounter);
				    y = y - yico;
				    }
				break;
			   }
		    break;

	       case WM_PAINT:
		    hps = WinBeginPaint(hwnd, 0, 0);
		    WinQueryWindowRect(hwnd, &rcl);
		    WinFillRect(hps, &rcl, CLR_PALEGRAY);
		    WinEndPaint(hps);
		    break;

	       case WM_CONTROL:
		    switch (SHORT2FROMMP(mp1))
			   {
			   case BN_CLICKED:
				if (bWait == FALSE)
				   WinPostMsg(hwndOwner, WM_BUTTONBARSELECT, (PVOID) SHORT1FROMMP(mp1), NULL);
				break;

			   case BN_PAINT:
				y = LONGFROMMP(mp2);
				ubInfo = (PUSERBUTTON) y;
				ulFlag = DBM_NORMAL;
				switch (ubInfo->fsState)
				       {
				       case BDS_DEFAULT:
					    ulFlag = DBM_NORMAL;
					    break;

				       case BDS_HILITED:
					    ulFlag = DBM_INVERT;
					    break;

				       case BDS_DISABLED:
					    ulFlag = DBM_HALFTONE;
					    break;
				       }
				iPos = SHORT1FROMMP(mp1);
				pp.x = 0;
				pp.y = 0;
				WinDrawBitmap(ubInfo->hps, btnbar[iPos].ulResId, 0L, &pp, CLR_NEUTRAL, CLR_BACKGROUND, ulFlag);
				return ((MRESULT) TRUE);      
				break;
			   }
	       default:
		    return (WinDefWindowProc(hwnd, msg, mp1, mp2));
	       }
	return (FALSE);
	}


/*************************************************************************************
  PaintButtonBar: "Paints" the button bar.
************************************************************************************/
VOID PaintButtonBar(HWND hwnd)
     {
     USHORT             usCounter;

     if (hwndBtn == FALSE)
	return;
     WinInvalidateRect(hwndBtn, NULL, FALSE);
     WinInvalidateRect(hwndBtnChild, NULL, FALSE);

     for (usCounter = 0;usCounter < usItems;usCounter++)
	 WinInvalidateRect(WinWindowFromID(hwndBtnChild, usCounter), NULL, FALSE);
     }

/*************************************************************************************
  SizeButtonBar: Sizes the button bar.
************************************************************************************/
ULONG SizeButtonBar (HWND hwnd, ULONG ulBtnLen)
      {
      RECTL     prclClient;

      if (hwndBtn == 0)
	 return (0);
      WinQueryWindowRect(hwnd, &prclClient);

      switch (barpos)
	     {
	     case BP_LEFT:
		  xpos = prclClient.xLeft;
		  xsize = xico + (xbrd * 2);
		  ysize = ulBtnLen;
		  if (ysize == 0)
		     ysize = prclClient.yTop;
		  ypos = prclClient.yBottom;
		  WinSetWindowPos(hwndBtn, HWND_BOTTOM, xpos, ypos, xsize, ysize, SWP_SHOW | SWP_SIZE | SWP_MOVE);
		  return (xsize);
		  break;

	     case BP_TOP:
		  ypos = prclClient.yTop - yico - (ybrd * 2);
		  ysize = yico + (ybrd * 2);
		  xsize = ulBtnLen;
		  if (xsize == 0)
		     xsize = prclClient.xRight;
		  xpos = 0;
		  WinSetWindowPos(hwndBtn, HWND_BOTTOM, xpos, ypos, xsize, ysize, SWP_SHOW | SWP_SIZE | SWP_MOVE);
		  return (ysize);
		  break;
	     }
      }

/*************************************************************************************
  EnableButtonBar: Enables (or disables) ButtonBar.
************************************************************************************/
VOID EnableButtonBar(BOOL bFlag)
     {
     if (bFlag)
	bWait = FALSE;
     else
	bWait = TRUE;
     }
