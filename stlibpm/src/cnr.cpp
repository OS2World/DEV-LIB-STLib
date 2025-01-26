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
   CNR.C
  ==========================================================================
  ==========================================================================
   970302 Created cnr.c, lots happened.
  ==========================================================================
   970310 Everything rethought, recoded.
  ==========================================================================
   970316 Reworked again, hope this is enough.
  ==========================================================================
   970317 Its getting better, slowly...
  ==========================================================================
   970514 Added support for CN_EMPHASIS.
  ==========================================================================
   970701 Added Lots of functions.
  ==========================================================================
   970809 Added ContainerOpenEdit.
  ==========================================================================
****************************************************************************/
#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include "stlibpmc.h"


MRESULT EXPENTRY ContainerWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
/*************************************************************************************
   CreateContainer:
*************************************************************************************/
HWND CreateContainer(HWND hwndOwner, PSTCONTAINER pCnr, int iIdCnr, int iExtra, int iItems, int iCreateFlags)
     {
     static BOOL        bIsReg = 0;
     char               szClass[] = "STContainerClass";
     HWND               hwndContainer;
     CNRINFO            cnrInfo;


     /* Set default values */
     pCnr->hwndOwner = hwndOwner;
     pCnr->iIdCnr = iIdCnr;
     pCnr->iItems = iItems;
     pCnr->iExtra = iExtra;
     pCnr->iCount = 0;

     if (!bIsReg)
        {
        WinRegisterClass(WinQueryAnchorBlock(pCnr->hwndOwner), szClass, (PFNWP)ContainerWndProc,
                         CS_SYNCPAINT | CS_SIZEREDRAW, 4);
        bIsReg = TRUE;
        }

     pCnr->hwndClient = WinCreateWindow(pCnr->hwndOwner, szClass, (PSZ)NULL, WS_SYNCPAINT, FCF_NOBYTEALIGN,
                                           0, 0, 0, pCnr->hwndOwner, HWND_TOP, 0, NULL, NULL);

     WinSetWindowPtr(pCnr->hwndClient, 0, pCnr);

     pCnr->hwndCnr = WinCreateWindow(pCnr->hwndClient, WC_CONTAINER, NULL, iCreateFlags | WS_SYNCPAINT,
                                     0, 0, 0, 0, pCnr->hwndClient, HWND_BOTTOM, iIdCnr, NULL, NULL);

     pCnr->pList = (PVOID) WinSendMsg(pCnr->hwndCnr, CM_ALLOCRECORD, MPFROMLONG(iExtra), MPFROMLONG(pCnr->iItems));

     return (hwndContainer);
     }


/*************************************************************************************
  ContainerWndProc: Handle Container messages.
*************************************************************************************/
MRESULT EXPENTRY ContainerWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
     {
     HPS                        hps;
     RECTL                      rcl;
     PSTCONTAINER               pCnr;
     PNOTIFYRECORDENTER         pNre;
     PNOTIFYRECORDEMPHASIS      pNreEmph;
     PCNREDITDATA               pCnrEd;
     PRECORDCORE                prc;
     PCNRDRAGINIT               pcdi;
     int                        iItem;


     switch (msg)
            {
            case WM_PAINT:
                 hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);
                 WinQueryWindowRect(hwnd, &rcl);
                 WinDrawBorder(hps, &rcl, STBORDERWIDTH, STBORDERWIDTH, CLR_DARKGRAY, CLR_DARKGRAY, DB_PATCOPY);
                 rcl.xLeft = rcl.xLeft + STBORDERWIDTH;
                 rcl.yTop = rcl.yTop - STBORDERWIDTH;
                 WinDrawBorder(hps, &rcl, STBORDERWIDTH, STBORDERWIDTH, CLR_WHITE, CLR_WHITE, DB_PATCOPY);
                 WinEndPaint(hps);
                 break;

            case WM_SIZE:
                 pCnr = (PSTCONTAINER) WinQueryWindowPtr(hwnd, 0);
                 if (pCnr != NULL)
                    {
                    WinQueryWindowRect(hwnd, &rcl);
                    WinSetWindowPos(pCnr->hwndCnr, HWND_BOTTOM, rcl.xLeft + STBORDERWIDTH, rcl.yBottom + STBORDERWIDTH,
                                    (rcl.xRight - rcl.xLeft) - (STBORDERWIDTH * 2), (rcl.yTop - rcl.yBottom) - (STBORDERWIDTH * 2),
                                    SWP_SIZE | SWP_MOVE | SWP_SHOW);
                    }
                 break;

            case WM_CONTROL:
                 pCnr = (PSTCONTAINER) WinQueryWindowPtr(hwnd, 0);
                 if (SHORT1FROMMP(mp1) == pCnr->iIdCnr)
                    {
                    switch (SHORT2FROMMP(mp1))
                           {
                           case CN_EMPHASIS:
                                pNreEmph = (PNOTIFYRECORDEMPHASIS) mp2;
                                WinPostMsg(pCnr->hwndOwner, WM_CONTAINERMSG, (PVOID)mp1, (MPARAM)pNreEmph->pRecord);
                                break;

                           case CN_ENTER:
                                pNre = (PNOTIFYRECORDENTER) mp2;
                                WinPostMsg(pCnr->hwndOwner, WM_CONTAINERMSG, (PVOID)mp1, (MPARAM)pNre->pRecord);
                                break;

                           case CN_REALLOCPSZ:
                                return ((MRESULT)TRUE);
                                break;

                           case CN_ENDEDIT:
                                pCnrEd = (PCNREDITDATA) mp2;
                                WinPostMsg(pCnr->hwndOwner, WM_CONTAINERMSG, (PVOID)mp1, (MPARAM)pCnrEd->pRecord);
                                break;

                           case CN_CONTEXTMENU:
                                prc = (PRECORDCORE) mp2;
                                WinPostMsg(pCnr->hwndOwner, WM_CONTAINERMSG, (PVOID)mp1, (MPARAM)prc);
                                break;

                           case CN_INITDRAG:
                                pcdi = (PCNRDRAGINIT) mp2;
                                WinPostMsg(pCnr->hwndOwner, WM_CONTAINERMSG, (PVOID)mp1, (MPARAM)pcdi->pRecord);
                                break;

                           case CN_DRAGOVER:
                                return(WinSendMsg(pCnr->hwndOwner, WM_CONTAINERMSG, (PVOID)mp1, (MPARAM)mp2));
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
   DestroyContainer:
*************************************************************************************/
void DestroyContainer(PSTCONTAINER pCnr)
     {
     WinDestroyWindow (pCnr->hwndCnr);
     WinDestroyWindow (pCnr->hwndClient);

     if (pCnr->hbmFolder != NULLHANDLE)
        GpiDeleteBitmap(pCnr->hbmFolder);

     if (pCnr->hbmFile != NULLHANDLE)
        GpiDeleteBitmap(pCnr->hbmFile);

     if (pCnr->hbmXtra1 != NULLHANDLE)
        GpiDeleteBitmap(pCnr->hbmXtra1);

     if (pCnr->hbmXtra2 != NULLHANDLE)
        GpiDeleteBitmap(pCnr->hbmXtra2);

     if (pCnr->hbmXtra3 != NULLHANDLE)
        GpiDeleteBitmap(pCnr->hbmXtra3);

     if (pCnr->hbmXtra4 != NULLHANDLE)
        GpiDeleteBitmap(pCnr->hbmXtra4);

     if (pCnr->hbmPlus != NULLHANDLE)
        GpiDeleteBitmap(pCnr->hbmPlus);

     if (pCnr->hbmMinus != NULLHANDLE)
        GpiDeleteBitmap(pCnr->hbmMinus);

     WinSendMsg(pCnr->hwndCnr, CM_FREERECORD, MPFROMP(pCnr->pList), MPFROMLONG(pCnr->iItems));
     }


/*************************************************************************************
   ContainerExpandTree:
*************************************************************************************/
BOOL ContainerExpandTree(PSTCONTAINER pCnr, PVOID pItem)
     {
     return ( (BOOL) WinSendMsg(pCnr->hwndCnr, CM_EXPANDTREE, MPFROMP(pItem), NULL));
     }


/*************************************************************************************
   QueryContainerSelected:
*************************************************************************************/
PVOID QueryContainerSelected(PSTCONTAINER pCnr)
     {
     PRECORDCORE        prc;


     prc = (PRECORDCORE) WinSendMsg(pCnr->hwndCnr, CM_QUERYRECORDEMPHASIS, (PRECORDCORE)CMA_FIRST, MPFROMSHORT(CRA_SELECTED));

     return ((PVOID) prc);
     }


/*************************************************************************************
   ContainerSelect:
*************************************************************************************/
BOOL ContainerSelect(PSTCONTAINER pCnr, PVOID pItem)
     {
     return ( (BOOL) WinSendMsg(pCnr->hwndCnr, CM_SETRECORDEMPHASIS, MPFROMP(pItem), MPFROM2SHORT(TRUE, CRA_SELECTED)));
     }


/*************************************************************************************
   QueryContainerFolder:
*************************************************************************************/
BOOL QueryContainerFolder(PVOID prc)
     {

     /* Folders always have hbmMiniBitmap equal to 0.
      ************************************************/
     if ( ((PRECORDCORE)prc)->hbmMiniBitmap == 0 )
        return (TRUE);
     else
        return (FALSE);
     }


/*************************************************************************************
   ContainerQueryRecord:
*************************************************************************************/
PVOID ContainerQueryRecord(PSTCONTAINER pCnr, int iCommand, PVOID prcItem, PVOID prcParent)
     {
     PVOID              prc;

     prc = (PVOID) WinSendMsg(pCnr->hwndCnr, CM_QUERYRECORD, prcItem, MPFROM2SHORT(iCommand, CMA_ITEMORDER));

     if (prc == NULL || prc == (PVOID) -1)
        return ((PVOID) NULL);
     else
        {
        prcParent = (PVOID) WinSendMsg(pCnr->hwndCnr, CM_QUERYRECORD, prc, MPFROM2SHORT(CMA_PARENT, CMA_ITEMORDER));
        if (prc == NULL || prc == (PVOID) -1)
           return ((PVOID) NULL);
        }

     return ((PVOID) prc);
     }


/*************************************************************************************
   InsertContainerFolder:
*************************************************************************************/
PVOID InsertContainerFolder(PSTCONTAINER pCnr, PVOID prcAfter, PVOID prcAskedParent)
     {
     RECORDINSERT       recsIn;
     PRECORDCORE        pIn;
     PRECORDCORE        prcParent;
     int                iPosition;
     BOOL               bSkip;


     if (pCnr->iCount > pCnr->iItems)
        return (NULL);

     prcParent = (PRECORDCORE) prcAskedParent;

     recsIn.cb = sizeof(RECORDINSERT);

     /* Does the caller ask for CMA_FIRST or CMA_END ?? */
     if (prcAfter == (PVOID) CMA_FIRST || prcAfter == (PVOID) CMA_END)
        {
        /* Place where asked for (depends on whats in prcParent */
        recsIn.pRecordOrder = (PRECORDCORE)prcAfter;
        recsIn.pRecordParent = (PRECORDCORE)prcParent;
        }
     else
        {
        bSkip = FALSE;
        if ((PRECORDCORE)prcParent == NULL)
           {
           /* Parent = Selected */
           prcParent = (PRECORDCORE) WinSendMsg(pCnr->hwndCnr, CM_QUERYRECORDEMPHASIS, (PRECORDCORE)CMA_FIRST, MPFROMSHORT(CRA_SELECTED));
           if (prcParent == NULL)
              {
              if (prcAfter == NULL)
                 prcAfter = (PVOID) CMA_FIRST;
              recsIn.pRecordOrder = (PRECORDCORE)prcAfter;
              recsIn.pRecordParent = (PRECORDCORE)NULL;
              bSkip = TRUE;
              }
           }

        if (bSkip == FALSE)
           {
           /* Check for parent type */
           if ( ((PRECORDCORE)prcParent)->hbmMiniBitmap == 0 )
              {
              /* Parent is a folder */
              recsIn.pRecordOrder = (PRECORDCORE)CMA_END;
              recsIn.pRecordParent = (PRECORDCORE)prcParent;
              }
           else
              {
              /* Parent is a file, put after "parent" */
              recsIn.pRecordOrder = (PRECORDCORE)prcAfter;
              recsIn.pRecordParent = (PRECORDCORE)NULL;
              }
           }
        }

     recsIn.zOrder = (USHORT)CMA_TOP;
     recsIn.fInvalidateRecord = TRUE;
     recsIn.cRecordsInsert = 1;

     if (pCnr->iCount == 0)
        pIn = (PRECORDCORE) pCnr->pList;
     else
        pIn = (PRECORDCORE) pCnr->prcNext;

     pIn->cb = sizeof(RECORDCORE);
     pIn->hbmMiniBitmap = 0;

     WinSendMsg(pCnr->hwndCnr, CM_INSERTRECORD, pIn, &recsIn);

     pCnr->prcNext = pIn->preccNextRecord;
     pCnr->iCount++;
     return (pIn);
     }


/*************************************************************************************
   InsertContainerFile:
*************************************************************************************/
PVOID InsertContainerFile(PSTCONTAINER pCnr, PVOID prcAfter, PVOID prcAskedParent)
     {
     RECORDINSERT       recsIn;
     PRECORDCORE        pIn;
     PRECORDCORE        prcParent;
     int                iPosition;
     BOOL               bSkip;


     prcParent = (PRECORDCORE) prcAskedParent;

     if (pCnr->iCount > pCnr->iItems)
        return (NULL);

     recsIn.cb = sizeof(RECORDINSERT);


     /* Does the caller ask for CMA_FIRST or CMA_END ?? */
     if (prcAfter == (PVOID) CMA_FIRST || prcAfter == (PVOID) CMA_END)
        {
        /* Place where asked for (depends on whats in prcParent */
        recsIn.pRecordOrder = (PRECORDCORE)prcAfter;
        recsIn.pRecordParent = (PRECORDCORE)prcParent;
        }
     else
        {
        bSkip = FALSE;
        if ((PRECORDCORE)prcParent == NULL)
           {
           /* Parent = Selected */
           prcParent = (PRECORDCORE) WinSendMsg(pCnr->hwndCnr, CM_QUERYRECORDEMPHASIS, (PRECORDCORE)CMA_FIRST, MPFROMSHORT(CRA_SELECTED));
           if (prcParent == NULL)
              return(NULL);
           }

        /* Check for parent type */
        if ( ((PRECORDCORE)prcParent)->hbmMiniBitmap == 0 )
           {
           /* Parent is a folder */
           recsIn.pRecordOrder = (PRECORDCORE)CMA_END;
           recsIn.pRecordParent = (PRECORDCORE)prcParent;
           }
        else
           {
           /* Parent is a file, put after "previous" */
           recsIn.pRecordOrder = (PRECORDCORE)prcAfter;
           recsIn.pRecordParent = (PRECORDCORE)NULL;
           }
        }

     recsIn.zOrder = (USHORT)CMA_TOP;
     recsIn.fInvalidateRecord = TRUE;
     recsIn.cRecordsInsert = 1;

     if (pCnr->iCount == 0)
        pIn = (PRECORDCORE) pCnr->pList;
     else
        pIn = (PRECORDCORE) pCnr->prcNext;

     pIn->cb = sizeof(RECORDCORE);
     pIn->hbmMiniBitmap = 1;

     WinSendMsg(pCnr->hwndCnr, CM_INSERTRECORD, pIn, &recsIn);

     pCnr->prcNext = pIn->preccNextRecord;
     pCnr->iCount++;
     return (pIn);
     }


/*************************************************************************************
   DeleteContainerItem:
*************************************************************************************/
BOOL DeleteContainerItem(PSTCONTAINER pCnr, PVOID pItem)
     {

     if (pCnr->iCount <= 0)
        return (FALSE);

     WinSendMsg(pCnr->hwndCnr, CM_REMOVERECORD, &pItem, MPFROM2SHORT(1, CMA_INVALIDATE));

     WinSendMsg(pCnr->hwndCnr, CM_INVALIDATERECORD, MPFROMP(pItem),MPFROM2SHORT(0, CMA_REPOSITION));

     pCnr->iCount--;
     return (TRUE);
     }


/*************************************************************************************
   SetContainerItem:
*************************************************************************************/
BOOL SetContainerItem(PSTCONTAINER pCnr, PVOID pItem, ULONG flRecordAttr, HMODULE hMod, HBITMAP hbmShow, char *szItemText)
     {

     if (pItem == NULL)
        return (FALSE);

     /* Check to see if hbm is valid, if not
        set folder or file.
      ***************************************/
     switch (hbmShow)
            {
            case IDSTCNR_HBMFOLDER:
                 ((PRECORDCORE)pItem)->hbmBitmap = pCnr->hbmFolder;
                 break;

            case IDSTCNR_HBMFILE:
                 ((PRECORDCORE)pItem)->hbmBitmap = pCnr->hbmFile;
                 break;

            case IDSTCNR_HBMOWNER1:
                 ((PRECORDCORE)pItem)->hbmBitmap = pCnr->hbmXtra1;
                 break;

            case IDSTCNR_HBMOWNER2:
                 ((PRECORDCORE)pItem)->hbmBitmap = pCnr->hbmXtra2;
                 break;

            case IDSTCNR_HBMOWNER3:
                 ((PRECORDCORE)pItem)->hbmBitmap = pCnr->hbmXtra3;
                 break;

            case IDSTCNR_HBMOWNER4:
                 ((PRECORDCORE)pItem)->hbmBitmap = pCnr->hbmXtra4;
                 break;

            default:
                 ((PRECORDCORE)pItem)->hbmBitmap = hbmShow;
                 break;
            }

     ((PRECORDCORE)pItem)->pszIcon = szItemText;
     ((PRECORDCORE)pItem)->pszText = szItemText;
     ((PRECORDCORE)pItem)->pszName = szItemText;
     ((PRECORDCORE)pItem)->pszTree = szItemText;

     ((PRECORDCORE)pItem)->flRecordAttr = ((PRECORDCORE)pItem)->flRecordAttr | flRecordAttr;

     WinSendMsg(pCnr->hwndCnr, CM_INVALIDATERECORD, MPFROMP(&pItem), MPFROM2SHORT(1, CMA_REPOSITION));

     return (TRUE);
     }


/*************************************************************************************
   EmptyContainer:
*************************************************************************************/
BOOL EmptyContainer(PSTCONTAINER pCnr)
     {

     if (pCnr->iCount <= 0)
        return (FALSE);

     WinSendMsg(pCnr->hwndCnr, CM_REMOVERECORD, (MPARAM)NULL, MPFROM2SHORT(0, CMA_INVALIDATE));

     pCnr->iCount = 0;
     return (TRUE);
     }


/*************************************************************************************
   ContainerOpenEdit: Opens the text edit of the selected container item.
*************************************************************************************/
void ContainerOpenEdit(PSTCONTAINER pCnr, PVOID pItem)
     {
     CNREDITDATA        cnrEditData;
     APIRET             rc;


     WinSendMsg(pCnr->hwndCnr, CM_SETRECORDEMPHASIS, MPFROMP(pItem), MPFROM2SHORT(TRUE, CRA_SELECTED));

     cnrEditData.cb = sizeof(CNREDITDATA);
     cnrEditData.hwndCnr = pCnr->hwndCnr;
     cnrEditData.pRecord = (PRECORDCORE) pItem;
     cnrEditData.pFieldInfo = NULL;
     cnrEditData.ppszText = NULL;
     cnrEditData.cbText = 0;
     cnrEditData.id = CID_CNRTITLEWND;

     rc = (APIRET) WinSendMsg(pCnr->hwndCnr, CM_OPENEDIT, (MPARAM)&cnrEditData, NULL);
     }


/*************************************************************************************
  QueryContainerColor:
*************************************************************************************/
void QueryContainerColor(PSTCONTAINER pCnr, int *iColorFore, int *iColorBack)
     {
     DEBUG("FIX_THIS");
     }


/*************************************************************************************
  SetContainerColor:
*************************************************************************************/
void SetContainerColor(PSTCONTAINER pCnr, int iColorFore, int iColorBack)
     {
     pCnr->iColorFore = iColorFore;
     pCnr->iColorBack = iColorBack;

     WinSetPresParam(pCnr->hwndCnr, PP_FOREGROUNDCOLORINDEX, (ULONG)sizeof(long), (PVOID)&iColorFore);
     WinSetPresParam(pCnr->hwndCnr, PP_BACKGROUNDCOLORINDEX, (ULONG)sizeof(long), (PVOID)&iColorBack);
     WinSetPresParam(pCnr->hwndClient, PP_BACKGROUNDCOLORINDEX, (ULONG)sizeof(long), (PVOID)&iColorBack);
     }


/*************************************************************************************
  QueryContainerFont:
*************************************************************************************/
void QueryContainerFont(PSTCONTAINER pCnr, char* szFont)
     {
     DEBUG("FIX_THIS");
     }


/*************************************************************************************
  SetContainerFont:
*************************************************************************************/
void SetContainerFont(PSTCONTAINER pCnr, char* szFont)
     {
     strcpy(pCnr->szFont, szFont);
     WinSetPresParam(pCnr->hwndCnr, PP_FONTNAMESIZE, (ULONG)strlen(pCnr->szFont) + 1, (PVOID)&pCnr->szFont);
     }


/*************************************************************************************
  SetContainerBitmap:
*************************************************************************************/
void SetContainerBitmap(PSTCONTAINER pCnr, HMODULE hMod, ULONG ulFolderRes, ULONG ulFileRes, ULONG ulXtra1, ULONG ulXtra2, ULONG ulXtra3, ULONG ulXtra4)
     {
     HPS        hps;

     hps = WinGetPS(pCnr->hwndOwner);
     pCnr->hbmFolder = GpiLoadBitmap(hps, hMod, ulFolderRes, 0, 0);
     pCnr->hbmFile = GpiLoadBitmap(hps, hMod, ulFileRes, 0, 0);

     if (ulXtra1 != 0)
        pCnr->hbmXtra1 = GpiLoadBitmap(hps, hMod, ulXtra1, 0, 0);
     else
        pCnr->hbmXtra1 = (HBITMAP) NULL;

     if (ulXtra2 != 0)
        pCnr->hbmXtra2 = GpiLoadBitmap(hps, hMod, ulXtra2, 0, 0);
     else
        pCnr->hbmXtra2 = (HBITMAP) NULL;

     if (ulXtra3 != 0)
        pCnr->hbmXtra3 = GpiLoadBitmap(hps, hMod, ulXtra3, 0, 0);
     else
        pCnr->hbmXtra3 = (HBITMAP) NULL;

     if (ulXtra4 != 0)
        pCnr->hbmXtra4 = GpiLoadBitmap(hps, hMod, ulXtra4, 0, 0);
     else
        pCnr->hbmXtra4 = (HBITMAP) NULL;

     WinReleasePS(hps);
     }


/*************************************************************************************
  SetContainerBitmapSize:
*************************************************************************************/
void SetContainerBitmapSize(PSTCONTAINER pCnr, int iSizeX, int iSizeY)
     {
     CNRINFO            cnrInfo;

     WinSendMsg(pCnr->hwndCnr, CM_QUERYCNRINFO, (PCNRINFO)&cnrInfo, NULL);
     cnrInfo.slBitmapOrIcon.cy = iSizeY;
     cnrInfo.slBitmapOrIcon.cx = iSizeX;
     WinSendMsg(pCnr->hwndCnr, CM_SETCNRINFO, (PCNRINFO)&cnrInfo, MPFROMLONG(CMA_SLBITMAPORICON));
     }


/*************************************************************************************
  SetContainerTreeBitmap:
*************************************************************************************/
void SetContainerTreeBitmap(PSTCONTAINER pCnr, HMODULE hMod, ULONG ulPlusRes, ULONG ulMinusRes)
     {
     CNRINFO            cnrInfo;
     HPS                hps;


     WinSendMsg(pCnr->hwndCnr, CM_QUERYCNRINFO, (PCNRINFO)&cnrInfo, NULL);

     hps = WinGetPS(pCnr->hwndOwner);
     pCnr->hbmPlus = GpiLoadBitmap(hps, hMod, ulPlusRes, 0, 0);
     pCnr->hbmMinus = GpiLoadBitmap(hps, hMod, ulMinusRes, 0, 0);
     WinReleasePS(hps);

     cnrInfo.hbmExpanded = pCnr->hbmPlus;
     cnrInfo.hbmCollapsed = pCnr->hbmMinus;

     WinSendMsg(pCnr->hwndCnr, CM_SETCNRINFO, (PCNRINFO)&cnrInfo, MPFROMLONG(CMA_TREEBITMAP));
     }


/*************************************************************************************
  SetContainerTreeBitmapSize:
*************************************************************************************/
void SetContainerTreeBitmapSize(PSTCONTAINER pCnr, int iSizeX, int iSizeY)
     {
     CNRINFO            cnrInfo;

     WinSendMsg(pCnr->hwndCnr, CM_QUERYCNRINFO, (PCNRINFO)&cnrInfo, NULL);
     cnrInfo.slTreeBitmapOrIcon.cy = iSizeY;
     cnrInfo.slTreeBitmapOrIcon.cx = iSizeX;
     WinSendMsg(pCnr->hwndCnr, CM_SETCNRINFO, (PCNRINFO)&cnrInfo, MPFROMLONG(CMA_SLTREEBITMAPORICON));
     }


/*************************************************************************************
  QueryContainerTreeLineLen:
*************************************************************************************/
int QueryContainerTreeLineLen(PSTCONTAINER pCnr)
     {
     CNRINFO            cnrInfo;

     WinSendMsg(pCnr->hwndCnr, CM_QUERYCNRINFO, (PCNRINFO)&cnrInfo, NULL);
     return(cnrInfo.cxTreeIndent);
     }


/*************************************************************************************
  SetContainerTreeLineLen:
*************************************************************************************/
void SetContainerTreeLineLen(PSTCONTAINER pCnr, int iTreeLineLen)
     {
     CNRINFO            cnrInfo;

     WinSendMsg(pCnr->hwndCnr, CM_QUERYCNRINFO, (PCNRINFO)&cnrInfo, NULL);
     cnrInfo.cxTreeIndent = iTreeLineLen;
     WinSendMsg(pCnr->hwndCnr, CM_SETCNRINFO, (PCNRINFO)&cnrInfo, MPFROMLONG(CMA_CXTREEINDENT));
     }


/*************************************************************************************
  SetContainerWindowAttr:
*************************************************************************************/
void SetContainerWindowAttr(PSTCONTAINER pCnr, ULONG flWindowAttr)
     {
     CNRINFO            cnrInfo;


     WinSendMsg(pCnr->hwndCnr, CM_QUERYCNRINFO, (PCNRINFO)&cnrInfo, NULL);
     cnrInfo.flWindowAttr = cnrInfo.flWindowAttr | flWindowAttr;
     WinSendMsg(pCnr->hwndCnr, CM_SETCNRINFO, (PCNRINFO)&cnrInfo, MPFROMLONG(CMA_FLWINDOWATTR));
     }


