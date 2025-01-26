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
   PMPRINT.C
  ==========================================================================
  ==========================================================================
   930107 Converted to CSET/2
  ==========================================================================
   941123 Added PrtForm for getting form information.
  ==========================================================================
   950125 Added PrtInfoXxx for unspecified information.
  ==========================================================================
   950126 Removed no HDC bug in PrtInfoXxx.
  ==========================================================================
   950729 Added support for printer values already there.
  ==========================================================================
   951213 Fixed a bug that did not select the right paper size in PrtForms.
  ==========================================================================
   960527 Opps, PRINTERINFO exists, now called PMPRINTINFO.
  ==========================================================================
   960528 Added queues/job properties.
  ==========================================================================
   9600827 Fixed bug in PrtForm.
  ==========================================================================
   970517 Fixed yet a bug in PrtForm.
  ==========================================================================
****************************************************************************/
#define INCL_WIN
#define INCL_GPI
#define INCL_DEV
#define INCL_SPL
#define INCL_SPLDOSPRINT


#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stlibpmc.h"


/****************************************************************************
  PrtDevPrinterInfo: Returns info about default printer
****************************************************************************/
BOOL PrtDevPrinterInfo (PSZ szLogAddress, PSZ szDriverName, PSZ szDriver, PSZ szPort)
     {
     USHORT             usSize;
     CHAR               szWrk[256];
     CHAR               szTemp[256];
     PCH                pchScan;

     if ( (usSize = (SHORT)PrfQueryProfileString(HINI_PROFILE,
                                               (PSZ)"PM_SPOOLER",
                                               (PSZ)"PRINTER",
                                               NULL,
                                               (PSZ) szTemp,
                                               (LONG)sizeof(szTemp))) != 0)
        {
        szTemp[usSize-2] = 0;
        if (PrfQueryProfileString(HINI_PROFILE,
                                  (PSZ)"PM_SPOOLER_PRINTER",
                                  (PSZ)szTemp,
                                  NULL,
                                  (PSZ)szWrk,
                                  (LONG)sizeof(szWrk)) )
           {
           pchScan = strchr(szWrk, ';');
           pchScan++;
           pchScan = strchr(pchScan, ';');
           pchScan++;
           *(pchScan + strcspn(pchScan, ".,;")) = 0;
           strcpy(szLogAddress, pchScan);

           pchScan = strchr(szWrk, ';');
           pchScan++;
           *(pchScan + strcspn(pchScan, ";")) = 0;
           strcpy(szDriverName, pchScan);

           pchScan = strchr(szWrk, ';');
           pchScan++;
           *(pchScan + strcspn(pchScan, ".,;")) = 0;
           strcpy(szDriver, pchScan);

           pchScan = szWrk;
           *(pchScan + strcspn(pchScan, ".,;")) = 0;
           strcpy(szPort, pchScan);
           return (TRUE);
           }
        else
           return (FALSE);
        }
     else
        return (FALSE);
     }


/****************************************************************************
  PrtDevStartDoc: Starts a print job.
****************************************************************************/
HDC PrtDevStartDoc(HAB hab, PSZ szDriver, PSZ szLogAddress, PSZ szDocInfo)
     {
     HDC                hdcPrinter;
     DEVOPENSTRUC       DevData;

     memset(&DevData, 0, sizeof(DEVOPENSTRUC));
     DevData.pszLogAddress = szLogAddress;
     DevData.pszDriverName = szDriver;
     DevData.pszDataType = (PSZ)"PM_Q_STD";

     hdcPrinter = DevOpenDC(hab, OD_QUEUED,
                            (PSZ)"*", 4L, (PDEVOPENDATA)&DevData,
                            (HDC)NULL);

     DevEscape(hdcPrinter, DEVESC_STARTDOC,
               strlen(szDocInfo), szDocInfo, 0L, NULL);

     return (hdcPrinter);
     }


/****************************************************************************
  PrtDevClip: Returns the left/bottom margins (cliping) if any.
****************************************************************************/
void PrtDevClip(HDC hdcPrinter, PULONG pulBottomClip, PULONG pulLeftClip)
     {
     HCINFO             hc;
     ULONG              ulForms;
     ULONG              ulCounter;

     ulForms = DevQueryHardcopyCaps(hdcPrinter, 1L, 0L, &hc);

     if(ulForms == DQHC_ERROR)
       {
       *pulLeftClip   = 0;
       *pulBottomClip = 0;
       }
     else
       {
       for(ulCounter = 0; ulCounter <= (USHORT) ulForms; ulCounter++)
          {
          DevQueryHardcopyCaps(hdcPrinter, ulCounter, 1L, &hc);
          if (hc.flAttributes == HCAPS_CURRENT + HCAPS_SELECTABLE)
             break;
          }
       *pulLeftClip   = hc.xLeftClip;
       *pulBottomClip = hc.yBottomClip;
       }
     }


/****************************************************************************
  PrtDevCreatePS: Creates a presentation space.
****************************************************************************/
HPS PrtDevCreatePS(HAB hab, HDC hdcPrinter, ULONG ulPsUnit)
     {
     SIZEL              szl;

     szl.cx = 0;
     szl.cy = 0;

    return (GpiCreatePS(hab, hdcPrinter, &szl, ulPsUnit | GPIF_DEFAULT | GPIT_NORMAL | GPIA_ASSOC));
    }


/****************************************************************************
  PrtDevCharPos: Prints string.
****************************************************************************/
void PrtDevCharPos(HPS hps, ULONG ulyPos, ULONG ulxPos, PSZ szText)
     {
     POINTL             pointl;

     pointl.y = ulyPos;
     pointl.x = ulxPos;

     GpiMove(hps, &pointl);
     GpiCharString(hps, strlen(szText), szText);
     }


/****************************************************************************
  PrtDevCharAt: Prints string at logical char coordinates.
****************************************************************************/
void PrtDevCharAt(HPS hps, USHORT usRow, USHORT usCol, PSZ szText)
     {
     FONTMETRICS        fm;
     SIZEL              szl;
     POINTL             pointl;
     ULONG              yLen, xLen;


     GpiQueryFontMetrics(hps, sizeof(fm), &fm);
     yLen = fm.lMaxBaselineExt;
     xLen = fm.lAveCharWidth;

     GpiQueryPS(hps, &szl);

     pointl.y = szl.cy - ((usRow - 1) * yLen);
     pointl.x = (usCol - 1) * xLen;

     GpiMove(hps, &pointl);
     GpiCharString(hps, strlen(szText), szText);
     }

/****************************************************************************
  PrtDevNewPage: Tells GPI to do a formfeed.
****************************************************************************/
void PrtDevNewPage (HDC hdcPrinter)
     {
     DevEscape(hdcPrinter, DEVESC_NEWFRAME, 0L, (ULONG) NULL, 0L, (ULONG) NULL);
     }


/****************************************************************************
  PrtDevDestroyPS: Destroys a presentation space.
****************************************************************************/
void PrtDevDestroyPS (HPS hpsPrinter)
     {
     GpiAssociate(hpsPrinter, 0);
     GpiDestroyPS(hpsPrinter);
     }


/****************************************************************************
  PrtDevEndDoc: Ends a print job
****************************************************************************/
USHORT PrtDevEndDoc (HDC hdcPrinter)
     {
     CHAR               szJobId[1];
     LONG               lBufferLen = 1;

     DevEscape(hdcPrinter, DEVESC_ENDDOC, 0L, 0L, &lBufferLen, szJobId);
     DevCloseDC(hdcPrinter);
     return (0);
     }


/****************************************************************************
  PrtStart: Simplified start document.
****************************************************************************/
BOOL PrtStart(HAB hab, PSZ szDocInfo, ULONG ulPsUnit, PPMPRINTINFO pprt, PPRINTDEST ppDest)
     {
     HDC                hdcPrinter;
     DEVOPENSTRUC       DevData;
     PDEVOPENSTRUC      pDevData;

     if (ppDest == NULL)
        {
        PrtDevPrinterInfo(pprt->szLogAddress, pprt->szDriverName, pprt->szDriver, pprt->szPort);

        pprt->hdcPrinter = PrtDevStartDoc(hab, pprt->szDriver, pprt->szLogAddress, szDocInfo);
        }
     else
        {
        pDevData = (PDEVOPENSTRUC)ppDest->pdopData;
        sprintf(pprt->szLogAddress, pDevData->pszLogAddress);
        sprintf(pprt->szDriverName, pDevData->pszDriverName);

        pprt->hdcPrinter = DevOpenDC(hab, ppDest->lType, ppDest->pszToken, ppDest->lCount,
                                     ppDest->pdopData,(HDC)NULL);

        DevEscape(pprt->hdcPrinter, DEVESC_STARTDOC,
                  strlen(szDocInfo), szDocInfo, 0L, NULL);
        }

     if (ulPsUnit == 0)
        ulPsUnit = PU_LOMETRIC;

     pprt->hpsPrinter = PrtDevCreatePS(hab, pprt->hdcPrinter, ulPsUnit);

     /* Get forms information of default form */
     PrtForm(pprt);

     if (pprt->hdcPrinter == 0 || pprt->hpsPrinter == 0)
        return (FALSE);
     else
        return (TRUE);
     }


/****************************************************************************
  PrtForm: Get information of currently selected form.
****************************************************************************/
BOOL PrtForm(PPMPRINTINFO pprt)
     {
     HCINFO             hc;
     ULONG              ulForms;
     ULONG              ulCounter;

     ulForms = DevQueryHardcopyCaps(pprt->hdcPrinter, 1L, 0L, &hc);

     if (ulForms != DQHC_ERROR)
        {
        for(ulCounter = 0; ulCounter <= (USHORT) ulForms; ulCounter++)
           {
           DevQueryHardcopyCaps(pprt->hdcPrinter, ulCounter, 1L, &hc);
           if (hc.flAttributes == HCAPS_CURRENT + HCAPS_SELECTABLE)
              break;
           }
        sprintf(pprt->szFormName, hc.szFormname);
        pprt->lFormWidth = hc.cx;
        pprt->lFormHeight = hc.cy;
        pprt->xLeftClip = hc.xLeftClip;
        pprt->yBottomClip = hc.yBottomClip;
        pprt->xRightClip = hc.xRightClip;
        pprt->yTopClip = hc.yTopClip;
        pprt->lWidthPels = hc.xPels;
        pprt->lHeightPels = hc.yPels;
        return (TRUE);
        }
     else
        return (FALSE);
     }


/****************************************************************************
  PrtEnd: Ends a print job
****************************************************************************/
USHORT PrtEnd(PPMPRINTINFO pprt)
     {
     PrtDevDestroyPS (pprt->hpsPrinter);
     PrtDevEndDoc(pprt->hdcPrinter);
     return (0);
     }


/****************************************************************************
  PrtInfoOpen: Open printer DC and PS.
****************************************************************************/
BOOL PrtInfoOpen(HAB hab, ULONG ulPsUnit, PPMPRINTINFO pprt, PPRINTDEST ppDest)
     {
     DEVOPENSTRUC       DevData;
     PDEVOPENSTRUC      pDevData;



     if (ppDest == NULL)
        {
        PrtDevPrinterInfo(pprt->szLogAddress, pprt->szDriverName, pprt->szDriver, pprt->szPort);
        /* Prepare for DevOpenDC */
        memset(&DevData, 0, sizeof(DEVOPENSTRUC));
        DevData.pszLogAddress = pprt->szLogAddress;
        DevData.pszDriverName = pprt->szDriver;
        DevData.pszDataType = (PSZ)"PM_Q_STD";
        pprt->hdcPrinter = DevOpenDC(hab, OD_QUEUED,(PSZ)"*", 4L,
                                     (PDEVOPENDATA)&DevData,(HDC)NULL);
        }
     else
        {
        pDevData = (PDEVOPENSTRUC)ppDest->pdopData;
        sprintf(pprt->szLogAddress, pDevData->pszLogAddress);
        sprintf(pprt->szDriverName, pDevData->pszDriverName);

        pprt->hdcPrinter = DevOpenDC(hab, ppDest->lType, ppDest->pszToken, ppDest->lCount,
                                     ppDest->pdopData,(HDC)NULL);
        }

     if (ulPsUnit == 0)
        ulPsUnit = PU_LOMETRIC;

     pprt->hpsPrinter = PrtDevCreatePS(hab, pprt->hdcPrinter, ulPsUnit);

     /* Get forms information of default form */
     PrtForm(pprt);

     if (pprt->hdcPrinter == 0 || pprt->hpsPrinter == 0)
        return (FALSE);
     else
        return (TRUE);
     }


/****************************************************************************
  PrtInfoClose: Ends a DC and PS info.
****************************************************************************/
USHORT PrtInfoClose(PPMPRINTINFO pprt)
     {
     PrtDevDestroyPS (pprt->hpsPrinter);
     DevCloseDC(pprt->hdcPrinter);
     return (0);
     }


/****************************************************************************
   PrtGetQueueList: Gets list of queues.
****************************************************************************/
int PrtQueryQueues(void)
     {
     int                iReturned, iTotal, iNeeded;

     /* Get number of queues */
     SplEnumQueue(NULL, 3, NULL, 0, (unsigned long *)&iReturned, (unsigned long *)&iTotal, (unsigned long *)&iNeeded, NULL);
     return (iTotal);
     }


/****************************************************************************
   PrtGetQueues: Fill array with list of queues.
****************************************************************************/
BOOL PrtGetQueues(PPMQUEUEINFO pPmQueueArray, int iAntal)
     {
     int                iReturned, iTotal, iNeeded;
     int                iCounter;
     PPRQINFO3          pQueueInfo;


     /* Get number of queues */
     SplEnumQueue(NULL, 3, NULL, 0, (unsigned long *)&iReturned, (unsigned long *)&iTotal, (unsigned long *)&iNeeded, NULL);
     /* Return if changed */
     if (iTotal != iAntal)
        return (FALSE);

     /* Get queues */
     pQueueInfo = (PPRQINFO3) malloc(iNeeded);
     SplEnumQueue(NULL, 3, pQueueInfo, iNeeded, (unsigned long *)&iReturned, (unsigned long *)&iTotal, (unsigned long *)&iNeeded, NULL);

     /* Move data into PMQUEUEINFO array */
     for (iCounter = 0; iCounter < iReturned; iCounter++)
         {
         strcpy(pPmQueueArray[iCounter].szName, pQueueInfo[iCounter].pszName);
         strcpy(pPmQueueArray[iCounter].szComment, pQueueInfo[iCounter].pszComment);
         strcpy(pPmQueueArray[iCounter].szDriverName, pQueueInfo[iCounter].pszDriverName);
         strcpy(pPmQueueArray[iCounter].szPrinters, pQueueInfo[iCounter].pszPrinters);

         if (pQueueInfo[iCounter].pDriverData->cb < 1024)
            {
            memcpy(pPmQueueArray[iCounter].szDrivData, pQueueInfo[iCounter].pDriverData, pQueueInfo[iCounter].pDriverData->cb);
            pPmQueueArray[iCounter].iDrivDataLen = pQueueInfo[iCounter].pDriverData->cb;
            }
         else
            {
            memcpy(pPmQueueArray[iCounter].szDrivData, pQueueInfo[iCounter].pDriverData, 1024);
            pPmQueueArray[iCounter].iDrivDataLen = 1024;
            }

         pPmQueueArray[iCounter].bDefault = FALSE;
         if (pQueueInfo[iCounter].fsType & PRQ3_TYPE_APPDEFAULT)
            pPmQueueArray[iCounter].bDefault = TRUE;
         }
     free(pQueueInfo);
     return (TRUE);
     }


/****************************************************************************
   PrtQueryJobProperties: Allows user to set job properties.
****************************************************************************/
BOOL PrtQueryJobProperties(HAB hab, PPMQUEUEINFO pPmQueue)
     {
     char       szDriverName[128];
     char       szDeviceName[128];
     int        iPos;
     PSZ        pszTemp;


     /* Separate driver and device name
       ---------------------------------*/
     iPos = strcspn(pPmQueue->szDriverName, ".");
     if (iPos > 0)
        {
        strncpy(szDriverName, pPmQueue->szDriverName, iPos);
        szDriverName[iPos] = '\0';
        strcpy(szDeviceName, &pPmQueue->szDriverName[iPos + 1]);
        }
     else
        {
        strcpy(szDriverName, pPmQueue->szDriverName);
        *szDeviceName = '\0';
        }

     /* Get first printer for this queue
       ----------------------------------*/
     pszTemp = strchr(pPmQueue->szPrinters, ',');
     if (pszTemp != NULL)
        {
        *pszTemp = '\0' ;
        }

     /* Post the job properties dialog
       --------------------------------*/
     if (DevPostDeviceModes(hab, (PDRIVDATA) pPmQueue->szDrivData, szDriverName, szDeviceName, pPmQueue->szPrinters, DPDM_POSTJOBPROP) == DEV_OK)
        return (TRUE);
     else
        return(FALSE);
     }



