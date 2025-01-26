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
   HELP.C
  ==========================================================================
  ==========================================================================
   970727 Created help.c, lots happened.
  ==========================================================================
   980309 Added index/contents.
  ==========================================================================
   980718 Added sizewindow
  ==========================================================================
****************************************************************************/
#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include "stlibpmc.h"

/*************************************************************************************
  CreateHelp: Creates help instance.
*************************************************************************************/
HWND CreateHelp(PSTHELP psth, HWND hwndFrame, int iMainHelpPanelId, char *szWindowTitle, char *szLibraryName, char *szTutorial)
     {
     HWND         hwndHelp;
     HELPINIT     hini;


     psth->hwndOwner = hwndFrame;
     if (szLibraryName != NULL)
        strcpy(psth->szHelpLibrary, szLibraryName);
     else
        psth->szHelpLibrary[0] = '\0';

     if (szWindowTitle != NULL)
        strcpy(psth->szHelpWindowTitle, szWindowTitle);
     else
        psth->szHelpWindowTitle[0] = '\0';

     if (szTutorial != NULL)
        strcpy(psth->szTutorial, szTutorial);
     else
        psth->szTutorial[0] = '\0';

     hini.cb = sizeof(HELPINIT);
     hini.ulReturnCode = 0;
     hini.pszTutorialName = psth->szTutorial;
     hini.phtHelpTable = (PHELPTABLE) MAKELONG(iMainHelpPanelId, 0xFFFF);
     hini.hmodHelpTableModule = 0;
     hini.hmodAccelActionBarModule = 0;
     hini.idAccelTable = 0;
     hini.idActionBar = 0;
     hini.pszHelpWindowTitle = psth->szHelpWindowTitle;
     hini.fShowPanelId = CMIC_HIDE_PANEL_ID;
     hini.pszHelpLibraryName = psth->szHelpLibrary;

     psth->hwndHelpInstance = WinCreateHelpInstance(WinQueryAnchorBlock(psth->hwndOwner), &hini);
     WinAssociateHelpInstance(psth->hwndHelpInstance, psth->hwndOwner);
     return(psth->hwndHelpInstance);
     }


/*************************************************************************************
  DestroyHelp: Destroys help instance.
*************************************************************************************/
BOOL DestroyHelp(PSTHELP psth)
     {
     if (psth->hwndHelpInstance != (HWND) NULL)
        {
        WinDestroyHelpInstance(psth->hwndHelpInstance);
        return (TRUE);
        }
     else
        return (FALSE);
     }


/*************************************************************************************
  DisplayHelp: Call this when you wish to display a help item.
*************************************************************************************/
BOOL DisplayHelp(PSTHELP psth, int iHelpId)
     {
     if (psth->hwndHelpInstance != NULLHANDLE)
        {
        WinSendMsg(psth->hwndHelpInstance, HM_DISPLAY_HELP, MPFROM2SHORT(iHelpId, NULL), MPFROMSHORT(HM_RESOURCEID));
        return (TRUE);
        }
     else
        return (FALSE);
     }


/*************************************************************************************
  DisplayHelpIndex: Call this when you wish to display the help index.
*************************************************************************************/
BOOL DisplayHelpIndex(PSTHELP psth)
     {
     if (psth->hwndHelpInstance != NULLHANDLE)
        {
        WinSendMsg(psth->hwndHelpInstance, HM_HELP_INDEX, NULL, NULL);
        return (TRUE);
        }
     else
        return (FALSE);
     }

/*************************************************************************************
  DisplayHelpContents: Call this when you wish to display the help index.
*************************************************************************************/
BOOL DisplayHelpContents(PSTHELP psth)
     {
     if (psth->hwndHelpInstance != NULLHANDLE)
        {
        WinSendMsg(psth->hwndHelpInstance, HM_HELP_CONTENTS, NULL, NULL);
        return (TRUE);
        }
     else
        return (FALSE);
     }


/*************************************************************************************
  SizeHelpWindow: Sets the size of the help window.
*************************************************************************************/
BOOL SizeHelpWindow(PSTHELP psth, LONG xLeft, LONG yBottom, LONG xRight, LONG yTop)
     {
     RECTL      rcl;


     rcl.xLeft = xLeft;
     rcl.yBottom = yBottom;
     rcl.xRight = xRight;
     rcl.yTop = yTop;
     if (psth->hwndHelpInstance != NULLHANDLE)
        {
        WinSendMsg(psth->hwndHelpInstance, HM_SET_COVERPAGE_SIZE, &rcl, NULL);
        return (TRUE);
        }
     else
        return (FALSE);
     }

