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
   BITMAP.C
  ==========================================================================
   961211 Started with this.
  ==========================================================================
****************************************************************************/
#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <string.h>
#include "stlibpmc.h"


/*-----------------------------------------------------------------------------------
  LoadBitmapFromFile: Copys bitmap from file to hbm.
-----------------------------------------------------------------------------------*/
HBITMAP LoadBitmapFromFile(HWND hwnd, char *szFileName)
     {
     APIRET              rc;                     /* API return code */
     BOOL                fRet = FALSE;           /* Function return code. */
     HFILE               hfile;
     ULONG               ulAction;
     FILESTATUS          fsts;
     PBITMAPFILEHEADER2  pbfh2;                  /* can address any file types */
     PBITMAPINFOHEADER2  pbmp2;                  /* address any info headers */
     PBYTE               pFileBegin = NULL;      /* beginning of bitmap file data */
     ULONG               cbRead;                 /* Number of bytes read by DosRead. */
     ULONG               cScans, cScansRet;      /* number of scan lines in bitmap (cy) */    
 
     HAB                 hab;
     HDC                 hdc;
     HPS                 hps;
     SIZEL               sizel;
     CHAR                szToken[]="*";

     HBITMAP             hbm;
     BOOL                bIgnore = FALSE;
    

     hab = WinQueryAnchorBlock(hwnd);

     /**********************************************************************
       Use Loop to avoid duplicate cleanup code.  If any errors, a break
       statement will jump directly to error cleanup code.
      **********************************************************************/
     do
        {
        /* Read the bitmap from file 
        ****************************/
        rc = DosOpen(szFileName, &hfile, &ulAction, 0, FILE_NORMAL, FILE_OPEN, OPEN_ACCESS_READONLY | OPEN_SHARE_DENYREADWRITE, NULL);
        if (rc)
           break;

        rc = DosQueryFileInfo(hfile, 1, &fsts, sizeof(fsts));
        if (rc)
           break;

        rc = DosAllocMem((PPVOID)&pFileBegin, (ULONG)fsts.cbFile, (ULONG) PAG_READ | PAG_WRITE | PAG_COMMIT);
        if (rc)
           break;

        if (DosRead( hfile, (PVOID)pFileBegin, fsts.cbFile, &cbRead))
           break;

        /**********************************************************************
          If it's a bitmap-array, point to common file header.  Otherwise,
          point to beginning of file.
        ***********************************************************************/
        pbfh2 = (PBITMAPFILEHEADER2) pFileBegin;
        pbmp2 = NULL;

        switch (pbfh2->usType)
               {
               case BFT_BITMAPARRAY:
                    /************************************************************
                      If this is a Bitmap-Array, adjust pointer to the normal
                      file header.  We'll just use the first bitmap in the
                      array and ignore other device forms.
                    *************************************************************/
                    pbfh2 = &(((PBITMAPARRAYFILEHEADER2) pFileBegin)->bfh2);
                    pbmp2 = &pbfh2->bmp2;    /* pointer to info header (readability) */
                    break;

               case BFT_BMAP:
                    pbmp2 = &pbfh2->bmp2;    /* pointer to info header (readability) */
                    break;

               default:      /* these formats aren't supported; don't set any ptrs */
                    case BFT_ICON:
                    case BFT_POINTER:
                    case BFT_COLORICON:
                    case BFT_COLORPOINTER:
                    break;
               }

        if (pbmp2 == NULL)
           break;        /* File format NOT SUPPORTED: break out to error code */

        /************************************************************************
          Check to see if BMP file has an old structure, a new structure, or
          Windows structure.  Capture the common data and treat all bitmaps
          generically with pointer to new format.  API's will determine format
          using cbFixed field.

          Windows bitmaps have the new format, but with less data fields
          than PM.  The old strucuture has some different size fields,
          though the field names are the same.
     
     
          NOTE: bitmap data is located by offsetting the beginning of the file
                by the offset contained in pbfh2->offBits.  This value is in
                the same relatie location for different format bitmap files.
         ***********************************************************************/

        if (pbmp2->cbFix == sizeof(BITMAPINFOHEADER))           /* old format? */
           cScans = (ULONG) ((PBITMAPINFOHEADER)pbmp2)->cy;        
        else                               /* new PM format, Windows, or other */
           cScans = pbmp2->cy;

        /* Create memory device context and make a hps.
         ***********************************************/
        sizel.cx = 0;
        sizel.cy = 0;
        hdc = DevOpenDC(hab, OD_MEMORY, szToken, 0L, 0L, 0L);
        hps = GpiCreatePS(hab, hdc, &sizel, PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);

        hbm = GpiCreateBitmap(hps, pbmp2, 0L, NULL, NULL);

        if (!hbm)
           break;

        if (GpiSetBitmap(hps, hbm) == (HBITMAP)BMB_ERROR)
           break;

        /*********************************************************************
          Tell GPI to put the bits into the thread's PS. The function returns
          the number of scan lines of the bitmap that were copied.  We want
          all of them at once.
         *********************************************************************/
        cScansRet = GpiSetBitmapBits(hps, 0L, cScans, pFileBegin + pbfh2->offBits, (PBITMAPINFO2) pbmp2);

        if (cScansRet != cScans)
           break;

        GpiDestroyPS(hps);
        DevCloseDC(hdc);
        DosFreeMem(pFileBegin);
        DosClose(hfile);
        return (hbm);
        } 
     while (bIgnore == FALSE);
    
     /* Error return) 
     *********************/
     if (pFileBegin != NULL)
        DosFreeMem(pFileBegin);
     if (hps != (HPS) NULL)
        GpiDestroyPS(hps);
     if (hdc != (HDC) NULL)
        DevCloseDC(hdc);
     DosClose( hfile);
     return ((HBITMAP) NULL);
     }


