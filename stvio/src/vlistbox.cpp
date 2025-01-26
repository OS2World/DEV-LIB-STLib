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
// *********************************************************************
// * WLISTBOX.MOD
// *********************************************************************
// * FUNCTION ListBox
// *
// *********************************************************************
// * 871004: Created
// *--------------------------------------------------------------------
// * 900629: Changed for New update of STLIB7
// *--------------------------------------------------------------------
// * 900701: Position meter added though not functioning
// *--------------------------------------------------------------------
// * 901228: Changed to use 'callback' and TYPE variable and STATIC
// *         1) Set items in type varibble
// *         2) Call with 0 to show listbox
// *         3) Loop calling with 1 to get keys
// *         4) Clean up by calling with 2
// *--------------------------------------------------------------------
// * 921127: Mouse support //oved
// *--------------------------------------------------------------------
// * 930419: Changed for new color settings
// *--------------------------------------------------------------------
// * 931107: Added WinListBox
// *--------------------------------------------------------------------
// * 980302: Ported to C (stlibvio)
// *********************************************************************
#include "stlibvio.h"


// ---------------------------------------------------------------------
//  ListBox: Puts list on screen to choose from.               901228
// ---------------------------------------------------------------------
int VioListBox (PSTVIOWINDOW hwnd, PSTVIOLISTBOX pli, int iListType, char *szStrPtr[], int aiRet[], int iHook)
    {
    int         iTemp, iCounter;
    int         iCurrent, iScrpos, iWidth, iKeyCode;
    int         iAttrTemp;
    char        szBuffer[256];
    int         kbd;


    switch (iListType)
           {
           case 0:      // First time, setup list

                // Check to se if it fits

                 pli->lvisible = pli->rowTo;
                 if (pli->lvisible > pli->llen)
                    pli->lvisible = pli->llen;

                 iWidth = pli->colTo;

                 if (pli->startat == 0)
                    {
                    pli->offset = 0;
                    pli->curRow = 0;
                    }
                else
                   {
                   if (pli->startat > pli->llen)
                      pli->startat = 0;

                   if (pli->startat <= pli->lvisible)
                      {
                      pli->offset = 0;
                      pli->curRow = pli->startat;
                      }
                   else
                      {
                      if (pli->startat > (pli->llen - pli->lvisible + 1))
                         {
                         pli->offset = pli->startat - pli->lvisible;
                         pli->curRow = pli->startat - pli->offset;
                         }
                      else
                         {
                         pli->offset = pli->startat;
                         pli->curRow = 0;
                         }
                      }
                   }

                for (iCounter = pli->row1; iCounter <= pli->row1 + pli->rowTo - 1; iCounter++)
                    {
                    iTemp = pli->offset + (iCounter - pli->row1);

                    VioFill(hwnd->pstv->iOutHandle, hwnd->iRow1 + iCounter, hwnd->iCol1 + pli->col1, iWidth, hwnd->iebcolor, hwnd->iefcolor);

                    if (iTemp < pli->llen)
                       {
                       memset(szBuffer, 0, 256);
                       strncpy(szBuffer, szStrPtr[iTemp], min(strlen(szStrPtr[iTemp]), iWidth));
                       VioColorPrint(hwnd, iCounter, pli->col1, hwnd->iebcolor, hwnd->iefcolor, szBuffer);
                       }
                    }
                pli->curpos = 1;
                return(1);


//  *************************************************************************************************************
           case 1:      // Poll list
                iCurrent = pli->offset + pli->curRow;
                if (iCurrent > pli->llen)
                   iCurrent = pli->llen;
                iScrpos = hwnd->iRow1 + pli->row1 + pli->curRow;

                iWidth = pli->colTo;

                if (pli->redraw == TRUE)
                   {
                   for (iCounter = pli->row1; iCounter <= pli->row1 + pli->rowTo - 1; iCounter++)
                       {
                       iTemp = pli->offset + (iCounter - pli->row1);
                       VioFill(hwnd->pstv->iOutHandle, hwnd->iRow1 + iCounter, hwnd->iCol1 + pli->col1, iWidth, hwnd->iebcolor, hwnd->iefcolor);

                       if (iTemp < pli->llen)
                          {
                          memset(szBuffer, 0, 256);
                          strncpy(szBuffer, szStrPtr[iTemp], min(strlen(szStrPtr[iTemp]), iWidth));
                          VioColorPrint(hwnd, iCounter, pli->col1, hwnd->iebcolor, hwnd->iefcolor, szBuffer);
                          }
                       }
                   }

                memset(szBuffer, 0, 256);
                strncpy(szBuffer, szStrPtr[iCurrent], min(strlen(szStrPtr[iCurrent]), iWidth));

                VioFill(hwnd->pstv->iOutHandle, iScrpos, hwnd->iCol1 + pli->col1, iWidth, hwnd->iefcolor, hwnd->iebcolor);
                VioColorPrint(hwnd, pli->row1 + pli->curRow, pli->col1, hwnd->iefcolor, hwnd->iebcolor, szBuffer);

                kbd = VioGetKey(hwnd, TRUE, FALSE);

                VioFill(hwnd->pstv->iOutHandle, iScrpos, hwnd->iCol1 + pli->col1, iWidth, hwnd->iebcolor, hwnd->iefcolor);
                VioColorPrint(hwnd, pli->row1 + pli->curRow, pli->col1, hwnd->iebcolor, hwnd->iefcolor, szBuffer);

                switch (kbd)
                       {
                       case KEY_UARROW:
                            pli->curRow--;
                            break;

                       case KEY_DARROW:
                            pli->curRow++;
                            break;

                       case KEY_PAGEDN:
                            pli->offset = pli->offset + pli->lvisible;
                            break;

                       case KEY_PAGEUP:
                            pli->offset = pli->offset - pli->lvisible;
                            break;

                       case KEY_HOME:
                            pli->offset = 0;
                            pli->curRow = 0;
                            break;

                       case KEY_END:
                            pli->offset = pli->llen - pli->lvisible ;
                            pli->curRow = pli->lvisible - 1;
                            break;

                       case KEY_SPACE:
                            if (pli->mark == TRUE)
                               {
                               if (strlen(szStrPtr[iCurrent]) > 1)
                                  {
                                  if (szStrPtr[iCurrent][0] == 'û')
                                     szStrPtr[iCurrent][0] = ' ';
                                  else
                                     szStrPtr[iCurrent][0] = 'û';

                                  memset(szBuffer, 0, 256);
                                  strncpy(szBuffer, szStrPtr[iCurrent], min(strlen(szStrPtr[iCurrent]), iWidth));
                                  VioFill(hwnd->pstv->iOutHandle, iScrpos, hwnd->iCol1 + pli->col1, iWidth, hwnd->iebcolor, hwnd->iefcolor);
                                  VioColorPrint(hwnd, pli->row1 + pli->curRow, pli->col1, hwnd->iebcolor, hwnd->iefcolor, szBuffer);
                                  }
                               pli->curRow = pli->curRow + 1;
                               }
                            break;

                       default:
                            pli->curpos = iCurrent;
                            return (kbd);
                            break;
                       }

                if (pli->lvisible > pli->rowTo)
                   pli->lvisible = pli->rowTo;

                if (pli->offset + pli->curRow >= pli->llen)
                   pli->curRow--;

                if (pli->curRow < 0)
                   {
                   pli->curRow = 0;
                   pli->offset = pli->offset - 1;
                   }
                else if (pli->curRow >= pli->lvisible)
                   {
                   pli->curRow = pli->lvisible - 1;
                   pli->offset = pli->offset + 1;
                   }
                else
                   pli->redraw = FALSE;

                if (pli->offset > pli->llen - pli->lvisible)
                   {
                   pli->offset = pli->llen - pli->lvisible;
                   pli->redraw = FALSE;
                   }

                if (pli->offset < 0)
                   {
                   pli->offset = 0;
                   pli->redraw = FALSE;
                   }

                if (pli->oldoffset != pli->offset)
                   pli->redraw = TRUE;

                pli->oldoffset = pli->offset;

                pli->curpos = pli->offset + pli->curRow - 1;

                return (iKeyCode);


//  *************************************************************************************************************
           case 2:      // Exit Listbox
                return (TRUE);

           default:
                return(TRUE);
           }
    }


