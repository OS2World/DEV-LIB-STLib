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
   MENU.C
  ==========================================================================
   HWND MenuHwnd (HWND hwnd)
   void MenuEnableItem (HWND hwndMenu, USHORT usMenuId, BOOL bEnabled)
   void MenuCheckItem (HWND hwndMenu, USHORT usMenuId, BOOL bCheck)
   void MenuSetItemText (HWND hwndMenu, USHORT usMenuId, PSZ szItemText)
   PSZ MenuGetItemText (HWND hwndMenu, USHORT usMenuId)
   void MenuInsertItem(HWND hwndMenu, USHORT usTopMenuId, USHORT usPosition, USHORT usNewMenuId, PSZ szItemText);
   void MenuInsertItem(HWND hwndMenu, USHORT usMenuId);
   BOOL ShowPopupMenu (HWND hwndClient, ULONG ulMenuId)
  ==========================================================================
   930107 Converted to CSET/2
  ==========================================================================
   930930 Bugfixes with Enable etc.
  ==========================================================================
   940425 Bugfixes with Checked.
  ==========================================================================
   940706 Added ShowPopupMenu.
  ==========================================================================
   970514 Added MenuInsertItem, MenuRemoveItem.
  ==========================================================================
****************************************************************************/
#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include "stlibpmc.h"


/****************************************************************************
  MenuHwnd: Gets menu hwnd.
****************************************************************************/
HWND MenuHwnd (HWND hwnd)
     {
     return (WinWindowFromID(WinQueryWindow(hwnd, QW_PARENT), FID_MENU));
     }


/****************************************************************************
  MenuEnableItem: Enable or disable menuitem.
****************************************************************************/
void MenuEnableItem (HWND hwndMenu, USHORT usMenuId, BOOL bEnabled)
     {
     USHORT             usFlag;

     if (!bEnabled)
        usFlag = MIA_DISABLED;
     else
        usFlag = TRUE;

     WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(usMenuId, TRUE), MPFROM2SHORT(MIA_DISABLED, usFlag));
     }


/****************************************************************************
  MenuCheckItem: Check or uncheck menuitem.
****************************************************************************/
void MenuCheckItem (HWND hwndMenu, USHORT usMenuId, BOOL bCheck)
     {
     USHORT             usFlag;

     if (bCheck)
        usFlag = MIA_CHECKED;
     else
        usFlag = TRUE;
     WinSendMsg(hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(usMenuId, TRUE), MPFROM2SHORT(MIA_CHECKED, usFlag));
     }


/****************************************************************************
  MenuSetItemText: Set text of menuitem.
****************************************************************************/
void MenuSetItemText (HWND hwndMenu, USHORT usMenuId, PSZ szItemText)
     {
     WinSendMsg(hwndMenu, MM_SETITEMTEXT, MPFROMSHORT(usMenuId), MPFROMLONG(szItemText));
     }


/****************************************************************************
  MenuGetItemText: Get text of menuitem.
****************************************************************************/
PSZ MenuGetItemText (HWND hwndMenu, USHORT usMenuId)
    {
    ULONG               ulMenuTextLen;
    PSZ                 szMenuText;


    ulMenuTextLen = (ULONG) WinSendMsg(hwndMenu, MM_QUERYITEMTEXTLENGTH, MPFROMSHORT(usMenuId), NULL);
    ulMenuTextLen++;
    szMenuText = (PSZ) malloc(ulMenuTextLen);
    WinSendMsg(hwndMenu, MM_QUERYITEMTEXT, MPFROM2SHORT(usMenuId, 0), MPFROMLONG(szMenuText));
    return (szMenuText);
    }



/****************************************************************************
  MenuInsertItem: Insert/appends a menuitem.
****************************************************************************/
void MenuInsertItem(HWND hwndMenu, USHORT usTopMenuId, USHORT usPosition, USHORT usNewMenuId, PSZ szItemText)
     {
     MENUITEM   mi;
     HWND       hwndSubMenu, hwndPullDown;

     WinSendMsg(hwndMenu, MM_QUERYITEM, MPFROM2SHORT(usTopMenuId, TRUE), (MPARAM) &mi);
     hwndPullDown = mi.hwndSubMenu;

     mi.iPosition = usPosition;
     mi.afStyle = MIS_TEXT;
     mi.afAttribute = 0;
     mi.id = usNewMenuId;
     mi.hwndSubMenu = (HWND) NULL;
     mi.hItem = 0;

     WinSendMsg(hwndPullDown, MM_INSERTITEM, (MPARAM) &mi, (MPARAM) szItemText);
     }


/****************************************************************************
  MenuRemoveItem: Removes a menuitem.
****************************************************************************/
void MenuRemoveItem(HWND hwndMenu, USHORT usMenuId)
     {
     WinSendMsg(hwndMenu, MM_DELETEITEM, MPFROM2SHORT(usMenuId, TRUE), NULL);
     }


/****************************************************************************
  ShowPopupMenu: Shows a popup menu.
****************************************************************************/
BOOL ShowPopupMenu (HWND hwndClient, ULONG ulMenuId)
     {
     HWND               hwndMenu;
     POINTL             ptl;
     ULONG              flOptions;

     WinQueryPointerPos(HWND_DESKTOP, &ptl);
     WinMapWindowPoints(HWND_DESKTOP, hwndClient, &ptl, 1);
     hwndMenu = WinLoadMenu(hwndClient, NULLHANDLE, ulMenuId);
     flOptions = PU_KEYBOARD | PU_MOUSEBUTTON1 | PU_HCONSTRAIN | PU_VCONSTRAIN;
     return (WinPopupMenu(hwndClient, hwndClient, hwndMenu, ptl.x, ptl.y, 0, (USHORT) flOptions));
     }

