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
   MLE.C
==========================================================================
 950608 Moved code from MNOTE.C
==========================================================================
****************************************************************************/
#define INCL_DOS
#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <stdlib.h>
#include <memory.h>
#include "stlibpmc.h"


/*-----------------------------------------------------------------------------
   MLEImport: Imports text into MLE (more than 64k supported).
-----------------------------------------------------------------------------*/
BOOL MLEImport(HWND hwndMLE, char *pszText, ULONG ulTextLength)
     {
     CHAR       *pszCurPos;
     void       *pBuffer;
     IPT        ipt = 0;
     ULONG      ulBytesCopied, ulToCopy;
     ULONG      ulBufSize = 65535;


     DosAllocMem ((PPVOID) &pBuffer, (ULONG) ulBufSize, PAG_COMMIT | PAG_READ | PAG_WRITE);

     WinSendMsg (hwndMLE, MLM_SETIMPORTEXPORT, MPFROMP (pBuffer), MPFROMLONG (ulBufSize));

     pszCurPos = pszText;

     do
        {
         ulToCopy = min(ulBufSize, ulTextLength);
         memcpy(pBuffer, pszCurPos, ulToCopy);
         ulBytesCopied = (ULONG) WinSendMsg (hwndMLE, MLM_IMPORT, MPFROMP(&ipt), MPFROMLONG(ulToCopy));
         pszCurPos += ulToCopy;
         ulTextLength -= ulToCopy;
        }
     while (ulTextLength > 0);

     DosFreeMem (pBuffer);
     return (TRUE);
     }


/*-----------------------------------------------------------------------------
   MLEExportLength: Length of (export) text in MLE.
-----------------------------------------------------------------------------*/
ULONG MLEExportLength(HWND hwndMLE)
     {
     IPT        ipt = 0;


     return (ULONG) WinSendMsg(hwndMLE, MLM_QUERYFORMATTEXTLENGTH,
                               MPFROMLONG(ipt), MPFROMLONG((-1)));

     }


/*-----------------------------------------------------------------------------
   MLEExport: Exports text from MLE (more than 64k supported).
-----------------------------------------------------------------------------*/
BOOL MLEExport(HWND hwndMLE, char *pszText, ULONG ulTextLength)
     {
     CHAR       *pszCurPos;
     void       *pBuffer;
     IPT        ipt = 0;
     ULONG      ulBufSize = 32768;
     ULONG      ulCopyLen, ulExported;


     DosAllocMem((PPVOID) &pBuffer, (ULONG) ulBufSize, PAG_COMMIT | PAG_READ | PAG_WRITE);

     pszCurPos = pszText;
     WinSendMsg(hwndMLE, MLM_SETIMPORTEXPORT, MPFROMP(pBuffer), MPFROMLONG(ulBufSize));
     do
        {
        ulCopyLen = min(ulBufSize, ulTextLength);
        ulExported = (ULONG) WinSendMsg(hwndMLE, MLM_EXPORT, MPFROMP(&ipt), MPFROMP(&ulCopyLen));
        memcpy(pszCurPos, pBuffer, ulExported);
        ulTextLength -= ulExported;
        pszCurPos += ulExported;
        }
     while (ulTextLength > 0);

     DosFreeMem(pBuffer);
     return (TRUE);
     }


