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
//     *********************************************************************
//     * WPRINT.MOD
//     *********************************************************************
//     * Part of WIN.MOD library after split
//     *
//     *********************************************************************
//     * 931107: Split...
//     *--------------------------------------------------------------------
//     * 980129: Moved to STLIBVIO (C/C++).
//     *--------------------------------------------------------------------
//     *********************************************************************
#include "stlibvio.h"

// ---------------------------------------------------------------------
//  VioColorPrint:
// ---------------------------------------------------------------------
void VioColorPrint (PSTVIOWINDOW hwnd, int iRow, int iCol, int fc, int bc, char *szText)
     {
     char       szString[256];
     int        iLen = strlen(szText);


     ValidPos(hwnd, &iRow, &iCol, &iLen);
     memset(szString, 0, 256);
     strncpy(szString, szText, iLen);

     VioColorPrintString (hwnd->pstv->iOutHandle, hwnd->iRow1 + iRow, hwnd->iCol1 + iCol, fc, bc, szString);
     }


// ---------------------------------------------------------------------
//  VioEditPrint:
// ---------------------------------------------------------------------
void VioEditPrint (PSTVIOWINDOW hwnd, int iRow, int iCol, char *szText)
     {
     char       szString[256];
     int        iLen = strlen(szText);


     ValidPos(hwnd, &iRow, &iCol, &iLen);
     memset(szString, 0, 256);
     strncpy(szString, szText, iLen);

     VioColorPrintString (hwnd->pstv->iOutHandle, hwnd->iRow1 + iRow, hwnd->iCol1 + iCol, hwnd->iefcolor, hwnd->iebcolor, szString);
     }

// ---------------------------------------------------------------------
//  WinFieldPrint:
// ---------------------------------------------------------------------
void VioFieldPrint (PSTVIOWINDOW hwnd, int iRow, int iCol, char *szText)
     {
     char       szString[256];
     int        iLen = strlen(szText);


     ValidPos(hwnd, &iRow, &iCol, &iLen);
     memset(szString, 0, 256);
     strncpy(szString, szText, iLen);

     VioColorPrintString (hwnd->pstv->iOutHandle, hwnd->iRow1 + iRow, hwnd->iCol1 + iCol, hwnd->iffcolor, hwnd->ifbcolor, szString);
     }


// ---------------------------------------------------------------------
//  VioFrame:
// ---------------------------------------------------------------------
void VioFrame (PSTVIOWINDOW hwnd, char *szTitle, int iRow1, int iCol1, int iRowTo, int iColTo, int iFrame)
     {
     int                iCounter;
     char               cUL, cUR, cLL, cLR, cH, cV;
     char               szBuffer[256], szTemp[256];


     if (iFrame == -1)
        iFrame = hwnd->iFrame;

     switch (iFrame)
            {
            case 0:
                 cUL ='Ú';
                 cUR ='¿';
                 cLL ='À';
                 cLR ='Ù';
                 cH ='Ä';
                 cV ='³';
                 break;

            case 255:
                 cUL ='É';
                 cUR ='»';
                 cLL ='È';
                 cLR ='¼';
                 cH ='Í';
                 cV ='º';
                 break;

            default:
                 cUL = cUR = cLL = cLR = cH = cV = hwnd->iFrame;
                 break;
            }

     memset(szTemp, 0, 256);
     memset(szTemp, cH, iColTo - 1);
     sprintf(szBuffer, "%c%s%c", cUL, szTemp, cUR);
     VioPrint(hwnd, iRow1, iCol1, szBuffer);

     for (iCounter = iRow1 + 1; iCounter <= iRow1 + iRowTo - 1; iCounter++)
         {
         sprintf(szBuffer, "%c", cV);
         VioPrint(hwnd, iCounter, iCol1, szBuffer);
         VioPrint(hwnd, iCounter, iCol1 + iColTo, szBuffer);
         }

     memset(szTemp, 0, 256);
     memset(szTemp, cH, iColTo - 1);
     sprintf(szBuffer, "%c%s%c", cLL, szTemp, cLR);
     VioPrint(hwnd, iRow1 + iRowTo, iCol1, szBuffer);

     if (strlen(szTitle) > 0)
        {
        sprintf(szBuffer, " %s ", szTitle);
        VioPrint(hwnd, iRow1, iCol1 + 1, szBuffer);
        }
     }



// ---------------------------------------------------------------------
//  VioScroll:
// ---------------------------------------------------------------------
void VioScroll (PSTVIOWINDOW hwnd, int iRow1, int iCol1, int iRowTo, int iColTo, int iScroll)
     {
     //ValidPos(hwnd, &iRow1, &iCol1, NULL);
     //ValidPos(hwnd, &iRowTo, &iColTo, NULL);

     VioScrollVert(hwnd->pstv->iOutHandle, hwnd->iRow1 + iRow1, hwnd->iCol1 + iCol1,
                   hwnd->iRow1 + (iRowTo + iRow1) - 1, hwnd->iCol1 + (iColTo + iCol1) - 1,
                   iScroll, hwnd->iwfcolor, hwnd->iwbcolor);
     }


// ---------------------------------------------------------------------
//  VioPrint:
// ---------------------------------------------------------------------
void VioPrint (PSTVIOWINDOW hwnd, int iRow, int iCol, char *szText)
     {
     char       szString[256];
     int        iLen = strlen(szText);


     ValidPos(hwnd, &iRow, &iCol, &iLen);
     memset(szString, 0, 256);
     strncpy(szString, szText, iLen);

     VioPrintString (hwnd->pstv->iOutHandle, hwnd->iRow1 + iRow, hwnd->iCol1 + iCol, szString);
     }


// ---------------------------------------------------------------------
//  VioStatusPrint:
// ---------------------------------------------------------------------
void VioStatusPrint(PSTVIO pstv, int iCol, int iField, BOOL bInverse, char *szText)
     {
     int        iRow;


     if (iField == 0)
        iRow = 1;
     else
        iRow = pstv->iMaxRows;

     if (bInverse == TRUE)
         VioColorPrintString (pstv->iOutHandle, iRow, iCol, pstv->iffcolor, pstv->ifbcolor, szText);
     else
        VioPrintString (pstv->iOutHandle, iRow, iCol, szText);
     }

// ---------------------------------------------------------------------
//  VioStatusPrint:
// ---------------------------------------------------------------------
char *VioFormatString(int iVarType, char *szFormatString, void *variable)
     {
     static char        szRetString[128];
     char               szBuffer[128];


     memset (szRetString, 0, 128);

     switch (iVarType)
            {
            case IDFS_STATIC:
            case IDFS_STRING:
                 memset(szRetString, ' ', 128);
                 memcpy(szRetString, (char*) variable, strlen((char*) variable));
                 szRetString[atoi(szFormatString)] = '\0';
                 break;

            case IDFS_LINE:
                 memset(szRetString, 'Ä', atoi(szFormatString));
                 break;

            case IDFS_FRAME:
                 strcpy(szRetString, "ERROR");
                 break;

            case IDFS_INTEGER:
                 sprintf(szBuffer, "%%-%dd", atoi(szFormatString));
                 sprintf(szRetString, szBuffer, * ((int*) variable));
                 break;

            case IDFS_FLOAT:
                 strcpy(szRetString, "ERROR");
                 break;

            case IDFS_DATE:
                 strcpy(szRetString, "ERROR");
                 break;

            case IDFS_TIME:
                 strcpy(szRetString, "ERROR");
                 break;

            case IDFS_DROP:
                 strcpy(szRetString, "ERROR");
                 break;

            case IDFS_BUTTON:
                 sprintf(szRetString, " < %s > ", szFormatString);
                 break;

            case IDFS_BOOL:
                 if (* ((BOOL*)variable) == FALSE)
                    szRetString[0] = szFormatString[0];
                 else
                    szRetString[0] = szFormatString[1];
                 break;

            case IDFS_CHECK:
                 if (* ((BOOL*)variable) == TRUE)
                    sprintf(szRetString, "[X] %s", szFormatString);
                 else
                    sprintf(szRetString, "[ ] %s", szFormatString);
                 break;

            case IDFS_OPTION:
                 if (* ((BOOL*)variable) == TRUE)
                    sprintf(szRetString, "(%c) %s", 7, szFormatString);
                 else
                    sprintf(szRetString, "( ) %s", szFormatString);
                 break;

            case IDFS_LISTBOX:
                 strcpy(szRetString, "ERROR");
                 break;

            default:
                 strcpy(szRetString, "ERROR");
            }

     return(szRetString);
     }
