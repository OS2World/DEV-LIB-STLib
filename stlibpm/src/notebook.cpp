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
   NOTEBOOK.CPP
  ==========================================================================
  ==========================================================================
   980907 Created NOTEBOOK.cpp, lots happened.
  ==========================================================================
****************************************************************************/
#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include "stlibpmc.h"



// FOR STLIBPMC ---------------------------------------------------------------------------------------------------

/* NOTEBOOK WINDOW
***********************************/
#define WM_NOTEBOOKMSG         WM_USER + 1022

#define STNB_TABTEXT
#define STNB_TABBITMAP



typedef struct _STNOTEBOOKPAGE
        {
        int                     iPageNumber;
        int                     iPageId;
        HWND                    hwndThisPage;
        unsigned short          usPageStyle;
        int                     iTabType;
        char                    szTabText[128];
        char                    szStatusText[128];
        HBM                     hbmTabBitmap;
        void                    *pstOwner;
        } STNOTEBOOKPAGE;

typedef STNOTEBOOKPAGE *PSTNOTEBOOKPAGE;

typedef struct _STNOTEBOOK
        {
        HWND                    hwndOwner;
        HWND                    hwndClient;
        HWND                    hwndNotebook;
        HWND                    hwndPage;
        int                     iIdNoteBook;
        PFNWP                   DefaultNotebookPageProc;
        PFNWP                   SubClassedNotebookPageProc;
        int                     iMaxPages;
        int                     iUsedPages;
        int                     isVisible;

        int                     iCurPage;

        PSTNOTEBOOKPAGE         pstnbPages;
        } STNOTEBOOK;

typedef STNOTEBOOK *PSTNOTEBOOK;


HWND CreateNotebook(HWND hwndOwner, PSTNOTEBOOK pstnb, int iIdNoteBook, int iIdPage, ULONG ulNoteBookStyles, int iMaxPages);

// ------------------------------------------------------------------------------------------------------------------

MRESULT EXPENTRY STNotebookWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY STPageWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
int FindNotebookPageId(PSTNOTEBOOK pstnb, int iPageNr);
int FindNotebookPageNr(PSTNOTEBOOK pstnb, int iPageId);
int FindNotebookPageNr(PSTNOTEBOOK pstnb, int iPage, BOOL bFlag);
/*************************************************************************************
   CreateNotebook:
*************************************************************************************/
HWND CreateNotebook(HWND hwndOwner, PSTNOTEBOOK pstnb, int iIdNoteBook, int iIdPage, ULONG ulNoteBookStyles, int iMaxPages)
     {
     static BOOL        bIsReg = FALSE;
     static BOOL        bIsPageReg = FALSE;
     char               szClass[] = "STNotebookClass";
     char               szPageClass[] = "STNotebookPageClass";

     // Set default values
     pstnb->hwndOwner = hwndOwner;
     pstnb->iIdNoteBook = iIdNoteBook;
     pstnb->iMaxPages = iMaxPages;


     // Register class
     if (!bIsReg)
        {
        WinRegisterClass(WinQueryAnchorBlock(pstnb->hwndOwner), szClass, (PFNWP)STNotebookWndProc, CS_SYNCPAINT | CS_SIZEREDRAW, 4);
        bIsReg = TRUE;
        }

     //  Create client window and save pointer
     pstnb->hwndClient = WinCreateWindow(pstnb->hwndOwner, szClass, (PSZ)NULL, WS_SYNCPAINT, FCF_NOBYTEALIGN,
                                           0, 0, 0, pstnb->hwndOwner, HWND_TOP, 0, NULL, NULL);

     WinSetWindowPtr(pstnb->hwndClient, 0, pstnb);

     // Set notebook styles
     if (ulNotebookStyles == 0)
        {
        ulNotebookStyles = BKS_SPIRALBIND | BKS_BACKPAGESBR | BKS_MAJORTABRIGHT | BKS_SQUARETABS |
                           BKS_TABTEXTLEFT | BKS_STATUSTEXTLEFT | WS_GROUP | WS_TABSTOP | 0x0800;
        }

     // Create notebook window
     pstnb->hwndNotebook = WinCreateWindow(pstnb->hwndClient, WC_NOTEBOOK, (PSZ)NULL, WS_SYNCPAINT, FCF_NOBYTEALIGN,
                                           0, 0, 0, pstnb->hwndClient, HWND_TOP, iIdNoteBook, NULL, NULL);

     // Register page class
     if (!bIsPageReg)
        {
        WinRegisterClass(WinQueryAnchorBlock(pstnb->hwndOwner), szPageClass, (PFNWP)STPageWndProc, CS_SYNCPAINT | CS_SIZEREDRAW, 4);
        bIsPageReg = TRUE;
        }

     // Create page window
     pstnb->hwndPage = WinCreateWindow(pstnb->hwndClient, szPageClass, NULL, 0L, 0, 0, 0, 0, pstnb->hwndClient,
                                       HWND_TOP, pstnb->iIdPage, NULL, NULL);

     WinSetWindowPtr(pstnb->hwndPage, 0, pstnb);


     pstnb->pstnbPages = (PSTNOTEBOOKPAGE) calloc(pstnb->iMaxPages, sizeof(STNOTEBOOKPAGE));

     for (iCounter = 0; iCounter < pstnb->iMaxPages; iCounter++)
         {
         pstnb->pstnbPages[iCounter].pstOwner = pstnb;
         }

     return(pstnb->hwndClient);
     }


/*************************************************************************************
   DestroyNotebook:
*************************************************************************************/
void DestroyNotebook(PSTNOTEBOOK pstnb)
     {
     WinDestroyWindow (pstnb->hwndPage);
     WinDestroyWindow (pstnb->hwndNotebook);
     WinDestroyWindow (pstnb->hwndClient);
     }


/*************************************************************************************
  InsertNotebookPage: Adds page to notebook.
*************************************************************************************/
BOOL InsertNotebookPage(PSTNOTEBOOK pstnb, unsigned short usPageStyle, int iRelativePage, unsigned short usPageOrder)
     {
     int        iRelativeId;
     int        iCounter;


     if ( pstnb->iUsedPages + 1 > pstnb->iMaxPages)
        return(FALSE);

     if (usPageStyle == 0)
        usPageStyle = BKA_MAJOR | BKA_STATUSTEXTON | BKA_AUTOPAGESIZE;

     if (usPageOrder == 0)
        usPageOrder = BKA_LAST;


     if (iRelativePage != 0)
        iRelativeId = FindNotebookPageId(pstnb, iRelativePage);
     else
        iRelativeId = 0;


     ulPageId = (ULONG) WinSendMsg(pstnb->hwndNotebook, BKM_INSERTPAGE, (MPFROMLONG)iRelativeId,
                                   MPFROM2SHORT(usPageStyle, usPageOrder));

     WinSendMsg(pstnb->hwndNotebook, BKM_SETPAGEWINDOWHWND, MPFROMLONG(ulPageId), MPFROMHWND(pstnb->hwndPage));

     switch(usPageOrder)
           {
           case BKA_FIRST:
                iInsertPos = 0;
                break;

           case BKA_LAST:
                break;

           case BKA_PREV:
                iInsertPos = iRelativePage;
                break;

           case BKA_NEXT:
                iInsertPos = iRelativePage + 1;
                break;
           }

// Flytta om i array !!
     for (iCounter = iInsertPos; iCounter < pstnb->iUsedPages; iCounter++)
         {

         }
     }


/*************************************************************************************
  RemoveNotebookPage: Removes page from notebook.
*************************************************************************************/
BOOL RemoveNotebookPage(PSTNOTEBOOK pstnb, int iPageNumber, int iRemoveFlag)
     {
     WinSendMsg(pstnb->hwndNotebook, BKM_DELETEPAGE,
                MPFROMLONG(FindNotebookPageId(pstnb, iPageNumber)), MPFROMLONG(iRemoveFlag));

// Flytta om i array !!
     }


/*************************************************************************************
   NotebookResize: Resize notebook.
*************************************************************************************/
void NotebookResize(PSTNOTEBOOK pstnb, int ypos, int xpos, int ysize, int xsize)
     {
     WinSetWindowPos(pstnb->hwndClient, HWND_BOTTOM, xpos, ypos, xsize, ysize, SWP_SIZE | SWP_MOVE | pstnb->isVisible);
     WinSetWindowPos(pstnb->hwndNotebook, HWND_BOTTOM, xpos, ypos, xsize, ysize, SWP_SIZE | SWP_MOVE | pstnb->isVisible);
     WinSetWindowPos(pstnb->hwndPage, HWND_BOTTOM, xpos, ypos, xsize, ysize, SWP_SIZE | SWP_MOVE | pstnb->isVisible);
     }


/*************************************************************************************
   NotebookShow: Show (or hide) notebook.
*************************************************************************************/
BOOL NotebookShow(PSTNOTEBOOK pstnb, BOOL bShow);
     {
     BOOL       bOldState;


     if (pstnb->isVisible == SWP_SHOW)
        bOldState = TRUE;
     else
        bOldState = FALSE;

     if (bShow == TRUE)
        pstnb->isVisible = SWP_SHOW;
     else
        pstnb->isVisible = SWP_HIDE;

     WinSetWindowPos(pstnb->hwndClient, NULL, 0, 0, 0, 0, pstnb->isVisible);
     WinSetWindowPos(pstnb->hwndNotebook, NULL, 0, 0, 0, 0, pstnb->isVisible);
     WinSetWindowPos(pstnb->hwndPage, NULL, 0, 0, 0, 0, pstnb->isVisible);

     return(bOldState);
     }


/*************************************************************************************
   ShowNotebookPage: .
*************************************************************************************/
BOOL ShowNotebookPage(PSTNOTEBOOK pstnb, int iPageNumber)
     {
     WinSendMsg(pstnb->hwndNotebook, BKM_TURNTOPAGE, MPFROMLONG(FindNotebookPageId(pstnb, iPageNumber)), (MPARAM)NULL);
     }


/*************************************************************************************
   SetNotebookPageStatusText: .
*************************************************************************************/
BOOL SetNotebookPageStatusText(PSTNOTEBOOK pstnb, int iPageNumber, char *szStatusText)
     {
     int        iIndex;


     iIndex = FindNotebookPageNr(pstnb, iPageNumber, FALSE);
     WinSendMsg(pstnb->hwndNotebook, BKM_SETSTATUSLINETEXT, MPFROMLONG(FindNotebookPageId(pstnb, iPageNumber)), MPFROMLONG(szStatusText));
     strcpy(pstnb->pstnbPages[iIndex].szStatusText, szStatusText);
     }


/*************************************************************************************
   SetNotebookTabSize: .
*************************************************************************************/
BOOL SetNotebookTabSize(PSTNOTEBOOK pstnb, int iTabType, short x, short y)
     {
     if (iTabType == 0)
        iTabType = BKA_MAJORTAB;

     WinSendMsg(pstnb->hwndNotebook, BKM_SETDIMENSIONS, MPFROM2SHORT(x, y), MPFROMSHORT(iTabType));
     WinSendMsg(pstnb->hwndNotebook, BKM_INVALIDATETABS, (MPARAM)NULL, (MPARAM)NULL);
     }


/*************************************************************************************
   SetNotebookTabText: .
*************************************************************************************/
BOOL SetNotebookTabText(PSTNOTEBOOK pstnb, int iPageNumber, char *szTabText)
     {
     int        iIndex;


     iIndex = FindNotebookPageNr(pstnb, iPageNumber, FALSE);
     WinSendMsg(pstnb->hwndNotebook, BKM_SETTABTEXT, MPFROMLONG(FindNotebookPageId(pstnb, iPageNumber)), MPFROMLONG(szTabText));
     strcpy(pstnb->pstnbPages[iIndex].szTabText, szTabText);
     }


/*************************************************************************************
   SetNotebookTabColor: .
*************************************************************************************/
// BKM_SETTABCOLOR
//     BKV_AUTOCOLOR
//
/*************************************************************************************
   SetNotebookTabBitmap: .
*************************************************************************************/
// BKM_SETTABBITMAP
BOOL SetNotebookTabBitmap(PSTNOTEBOOK pstnb, int iPageNumber, HBM hbmBitmap)
     {
     int        iIndex;


     iIndex = FindNotebookPageNr(pstnb, iPageNumber, FALSE);
     WinSendMsg(pstnb->hwndNotebook, BKM_SETTABTEXT, MPFROMLONG(FindNotebookPageId(pstnb, iPageNumber)), MPFROMLONG(szTabText));
     pstnb->pstnbPages[iIndex].hbmTabBitmap = hbmBitmap;
     }



/*************************************************************************************
  QueryNotebookFont:
*************************************************************************************/
void QueryNotebookFont(PSTNOTEBOOK pstnb, char* szFont)
     {
     DEBUG("FIX_THIS");
     }


/*************************************************************************************
  SetNotebookFont:
*************************************************************************************/
void SetNotebookFont(PSTNOTEBOOK pstnb, char* szFont)
     {
     DEBUG("FIX_THIS");
     }


/*************************************************************************************
   SubClassNotebookPageWindow: Creates a subclass of notebook 'client' window.
*************************************************************************************/
void SubClassNotebookPageWindow(PSTNOTEBOOK pstnb, PFNWP SubClassedNotebookPageProc)
     {
     DefaultNotebookPageProc = WinSubclassWindow(pstnb->hwndPage, (PFNWP) SubClassedNotebookPageProc);
     }


/*************************************************************************************
  STNoteBookWndProc: Handle Notebook messages.
*************************************************************************************/
MRESULT EXPENTRY STNotebookWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
     {
     HPS                        hps;
     RECTL                      rcl;
     PSTNOTEBOOK                pstnb;
     PPAGESELECTNOTIFY          ppsn;
     PDELETENOTIFY              pdn;

     switch (msg)
            {
            case WM_SIZE:
                 pstnb = (PSTNOTEBOOK) WinQueryWindowPtr(hwnd, 0);
                 if (pstnb != NULL)
                    {
                    WinQueryWindowRect(hwnd, &rcl);
                    WinSetWindowPos(pstnb->hwndNotebook, HWND_BOTTOM, rcl.xLeft, rcl.yBottom, rcl.xRight,rcl.yBottom,
                                    SWP_SIZE | SWP_MOVE | SWP_SHOW);
                    WinSetWindowPos(pstnb->hwndPage, HWND_BOTTOM, rcl.xLeft, rcl.yBottom, rcl.xRight,rcl.yBottom,
                                    SWP_SIZE | SWP_MOVE | SWP_SHOW);
                    }
                 break;

            case WM_CONTROL:
                 pstnb = (PSTNOTEBOOK) WinQueryWindowPtr(hwnd, 0);
                 if (SHORT1FROMMP(mp1) == pstnb->iIdNoteBook)
                    {
                    switch (SHORT2FROMMP(mp1))
                           {
                           case BKN_HELP:
                                WinPostMsg(pstnb->hwndOwner, WM_NOTEBOOKMSG, (PVOID)mp1, (MPARAM)mp2);
                                break;

                           case BKN_NEWPAGESIZE:
                                WinPostMsg(pstnb->hwndOwner, WM_NOTEBOOKMSG, (PVOID)mp1, (MPARAM)mp2);
                                break;

                           case BKN_PAGEDELETED:
                                pdn = (PDELETENOTIFY) mp2;
                                WinPostMsg(pstnb->hwndOwner, WM_NOTEBOOKMSG, (PVOID)mp1, (MPARAM)pdn->hwndPage);
                                break;

                           case BKN_PAGESELECTED:
                           case BKN_PAGESELECTEDPENDING:
                                ppsn = (PPAGESELECTNOTIFY) mp2;
                                WinPostMsg(pstnb->hwndOwner, WM_NOTEBOOKMSG, (PVOID)mp1, (MPARAM)ppsn->ulPageIdNew);
                                break;
                           }
                    }
                 break;

            default:
                 return (WinDefWindowProc (hwnd, msg, mp1, mp2));
            }
     return (FALSE);
     }


/*************************************************************************************
   STPageWndProc: Handles messages for the notebook "client" window.
*************************************************************************************/
MRESULT EXPENTRY STPageWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
     {
     HPS        hps;
     RECTL      rcl;


     switch (msg)
          {
          case WM_SIZE:
               break;

          default:
               return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
          }
     return FALSE ;
     }


/*************************************************************************************
  FindNotebookPageId: .
*************************************************************************************/
int FindNotebookPageId(PSTNOTEBOOK pstnb, int iPageNumber)
    {
    for (iCounter = 0; iCounter < pstnb->iUsedPages; iCounter++)
        {
        if (pstnb->pstnbPages[iCounter].iPageNumber == iPageNumber)
           return (pstnb->pstnbPages[iCounter].iPageId);
        }
    return(0);
    }


/*************************************************************************************
  FindNotebookPageNr: .
*************************************************************************************/
int FindNotebookPageNr(PSTNOTEBOOK pstnb, int iPageId)
    {
    for (iCounter = 0; iCounter < pstnb->iUsedPages; iCounter++)
        {
        if (pstnb->pstnbPages[iCounter].iPageId == iPageId)
           return (pstnb->pstnbPages[iCounter].iPageNumber);
        }
    return(0);
    }

/*************************************************************************************
  FindNotebookPageIndex: .
*************************************************************************************/
int FindNotebookPageNr(PSTNOTEBOOK pstnb, int iPage, BOOL bFlag)
    {
    for (iCounter = 0; iCounter < pstnb->iUsedPages; iCounter++)
        {
        if (bFlag == TRUE)
           {
           if (pstnb->pstnbPages[iCounter].iPageId == iPage)
              return (iCounter);
           }
        else
           {
           if (pstnb->pstnbPages[iCounter].iPageNumber == iPage)
              return (iCounter);
           }
        }
    return(0);
    }


