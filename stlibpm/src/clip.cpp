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
   CLIP.C
  ==========================================================================
   BOOL ClipQueryText(HAB hab);
   void TextToClip(HAB hab, PSZ szText);
   BOOL ClipQueryBitmap(HAB hab);
   BOOL BitmapToClip(HAB hab, HBITMAP hbm);
   HBITMAP ClipToBitmap(HAB hab);
  ==========================================================================
   921211 Converted to CSET/2
  ==========================================================================
   970809 Added ClipToText, changed hab to WinQueryAnch....
  ==========================================================================
****************************************************************************/
#define INCL_DOS
#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <string.h>
#include "stlibpmc.h"

void BitmapPSandDC(HAB hab, HPS hps, HDC hdc);
HBITMAP CopyOfBitmap(HAB hab, HBITMAP hbmSource);

/*-----------------------------------------------------------------------------------
  ClipQueryText: Is there CF_TEXT in the clipboard.
-----------------------------------------------------------------------------------*/
BOOL ClipQueryText(HWND hwnd)
     {
     BOOL       bIsText;


     if (WinOpenClipbrd (WinQueryAnchorBlock(hwnd)))
        {
        bIsText = WinQueryClipbrdFmtInfo(WinQueryAnchorBlock(hwnd), CF_TEXT, 0L);
        WinCloseClipbrd (WinQueryAnchorBlock(hwnd));
        return (bIsText);
        }
      else
        return(FALSE);
     }


/*-----------------------------------------------------------------------------------
  ClipToText: Copy Text from clipboard.
-----------------------------------------------------------------------------------*/
char* ClipToText(HWND hwnd)
     {
     static char        szClipBoardText[1024];
     PSZ                pszClipText;


     if (WinOpenClipbrd (WinQueryAnchorBlock(hwnd)))
        {
        if ( (pszClipText = (PSZ) WinQueryClipbrdData(WinQueryAnchorBlock(hwnd), CF_TEXT))  != NULL )
           {
           strncpy(szClipBoardText, pszClipText, 1023);
           WinCloseClipbrd (WinQueryAnchorBlock(hwnd));
           return(szClipBoardText);
           }
        WinCloseClipbrd (WinQueryAnchorBlock(hwnd));
        }
     return((char*) NULL);
     }


/*-----------------------------------------------------------------------------------
  TextToClip: Copy text to clipboard.
-----------------------------------------------------------------------------------*/
void TextToClip(HWND hwnd, PSZ szText)
     {
     PBYTE      pb;


     DosAllocSharedMem((PVOID *) &pb, (PSZ) NULL, strlen(szText) + 1, fALLOC | OBJ_GIVEABLE);
     strcpy((char*) pb, szText);

     if (WinOpenClipbrd (WinQueryAnchorBlock(hwnd)) != FALSE)
        {
        WinEmptyClipbrd(WinQueryAnchorBlock(hwnd));
        WinSetClipbrdData (WinQueryAnchorBlock(hwnd), (int) pb, CF_TEXT, CFI_POINTER);
        WinCloseClipbrd (WinQueryAnchorBlock(hwnd));
        }
      }

/*
*************************************************************************************
  ClipQueryBitmap: Is there CF_BITMAP in the clipboard.
*************************************************************************************
BOOL ClipQueryBitmap(HAB hab)
     {
     BOOL       bRetVal;

     if(WinOpenClipBrd(hab))
       {
       bRetVal = WinQueryClipbrdFmtInfo(hab, CF_BITMAP, (ULONG) NULL);
       WinCloseClipbrd (hab);
       return(bRetVal);
       }
     else
       WinCloseClipbrd(hab);

     return(TRUE);
     }


*************************************************************************************
  BitmapToClip: Copy bitmap to clipboard.
*************************************************************************************
BOOL BitmapToClip(HAB hab, HBITMAP hbm)
     {
     BOOL           bRetVal;
     HBITMAP        hbmClip;

     if (WinOpenClipBrd(hab))
        {
        WinSetPointer(HWND_DESKTOP,WinQuerySysPointer(HWND_DESKTOP, SPTR_WAIT, (ULONG) NULL));
        hbmClip = CopyOfBitmap(hab, hbm);
        WinEmptyClipBrd(hab);
        bRetVal = WinSetClipbrdData(hab, hbmClip, CF_BITMAP, CFI_HANDLE);
        WinCloseClipbrd(hab);
        WinSetPointer(HWND_DESKTOP,WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, (ULONG) NULL));
        return(bRetVal);
        }
     else
        return(FALSE);
     }

*************************************************************************************
  ClipToBitmap: Copy bitmap from clipboard.
*************************************************************************************
HBITMAP ClipToBitmap(HAB hab)
     {
     BOOL               bRetVal;
     HBITMAP            hbmClip, hbmTemp;

     if (WinOpenClipBrd(hab))
        {
        hbmClip = WinQueryClipBrdData(hab, CF_BITMAP);
        if (hbmclip)
           {
           WinSetPointer(HWND_DESKTOP,WinQuerySysPointer(HWND_DESKTOP, SPTR_WAIT, (ULONG) NULL));
           hbmTemp = CopyOfBitmap(hab, hbmClip);
           WinSetPointer(HWND_DESKTOP,WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, (ULONG) NULL));
           WinCloseClipbrd(hab);
           return(hbmTemp);
           }
         else
           {
           WinCloseClipbrd(hab);
           return((HBITMAP) NULL);
           }
         }
      else
         return((HBITMAP) NULL);
     }


*************************************************************************************
  CopyOfBitmap: Make a copy of a bitmap.
*************************************************************************************
HBITMAP CopyOfBitmap(HAB hab, HBITMAP hbmSource)
     {
     BITMAPINFOHEADER           bih;
     POINTL                     aptl[2];
     HPS                        hpsSource, hpsTarget;
     HDC                        hdcSource, hdcTarget;
     HBITMAP                    hbmTarget;

     BitmapPSandDC(hab, hpsSource, hdcSource);
     BitmapPSandDC(hab, hpsTarget, hdcTarget);

     GpiQueryBitmapParameters(hbmSource, &bih);
     hbmTarget = GpiCreateBitmap(hpsTarget, &bih, 0L, 0L, 0L);
     GpiSetBitmap(hpsSource, hbmSource);
     GpiSetBitmap(hpsTarget, hbmTarget);

     aptl[0].x = 0L;
     aptl[0].y = 0L;
     aptl[1].x = bih.cx;
     aptl[1].y = bih.cy;
     aptl[2].x = 0L;
     aptl[2].y = 0L;

     GpiBitBlt(hpsTarget, hpsSource, 3, &aptl[0], ROP_SRCCOPY, BBO_AND);
     GpiDestroyPS(hpsSource);
     GpiDestroyPS(hpsTarget);
     DevCloseDC(hdcSource);
     DevCloseDC(hdcTarget);
     return(hbmTarget);
     }


*************************************************************************************
  BitmapPSandDC: Create PS and DC for Bitmap copying.
*************************************************************************************
void BitmapPSandDC(HAB hab, HPS hps, HDC hdc)
     {
     SIZEL              sizel;
     CHAR               szToken[]="*";

     sizl.cx = 0;
     sizl.cy = 0;
     hdc = DevOpenDC(hab, OD_MEMORY,szToken, 0L, 0L, 0L);
     hps = GpiCreatePS(hab, hdc, &sizl, PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);
     }


*/
