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
//**************************************************************************
//
//   Modul:      VIO.CPP, Function library for console applications
//                        with support for Warp.
//
//   Version:    0.077
//
//   Tid:        941206, 000110, 20h
//
//  ---------------------------------------------------------------------
//   941206:  Skapade Vio calls (MA,3h).
//  ---------------------------------------------------------------------
//   960203:  Removed Windows NT support (MA,1h).
//  ---------------------------------------------------------------------
//   960204:  Inserted into STLIB2 (MA,1h).
//  ---------------------------------------------------------------------
//   980128:  Started move to stvio library (MA,2h).
//  ---------------------------------------------------------------------
//   980309:  Fix for numeric keyboard in VioInkey (MA,1h).
//  ---------------------------------------------------------------------
//   000110:  Added VioScroll (MA,1h).
//  ---------------------------------------------------------------------
//**************************************************************************

// Includes files for OS/2
//-------------------------
#define INCL_VIO
#define INCL_KBD
#define INCL_DOSMISC

#include <os2.h>
#include <bsesub.h>
#include <bsedev.h>

// Generic includes
//------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <process.h>
#include <conio.h>

#include "stlibvio.h"

// vio and kbd handles OS/2
//--------------------------
#define VIO_HANDLE       0
#define KBD_HANDLE       0


USHORT VioColor(USHORT usFore, USHORT usBack);


/*-----------------------------------------------------------------------------
   VioInit: Inits Vio environment.
-----------------------------------------------------------------------------*/
void VioInit(int *iOutHandle, int *iInHandle)
     {
     if (iOutHandle != NULL)
        *iOutHandle = VIO_HANDLE;

     if (iInHandle != NULL)
        *iInHandle = KBD_HANDLE;
     return;
     }

/*-----------------------------------------------------------------------------
   VioExit: Exits Vio environment.
-----------------------------------------------------------------------------*/
void VioExit(int iOutHandle, int iInHandle)
     {
     return;
     }


/*-----------------------------------------------------------------------------
   VioGetInfo: Gets info on colors and screensize.
-----------------------------------------------------------------------------*/
void VioGetInfo(int iOutHandle, int *iMaxRows, int *iMaxCols, int *iColors)
     {
     VIOMODEINFO        ModeData;


     ModeData.cb = sizeof(VIOMODEINFO);
     VioGetMode(&ModeData, VIO_HANDLE);

     if (iMaxRows != NULL)
        *iMaxRows = ModeData.row;

     if (iMaxCols != NULL)
        *iMaxCols = ModeData.col;

     if (iColors != NULL)
        *iColors = ModeData.color;
     }


/*-----------------------------------------------------------------------------
   VioGetInfo: Gets info on colors and screensize.
-----------------------------------------------------------------------------*/
BOOL VioSetScreenSize(int iOutHandle, int iMaxRows, int iMaxCols)
     {
     VIOMODEINFO        ModeData;


     ModeData.cb = sizeof(VIOMODEINFO);
     VioGetMode(&ModeData, VIO_HANDLE);

     if (iMaxRows != NULL)
        ModeData.row = iMaxRows;

     if (iMaxCols != NULL)
        ModeData.col = iMaxCols;

     if (VioSetMode(&ModeData, VIO_HANDLE) == 0)
        return (TRUE);
     else
        return (FALSE);
     }



/*-----------------------------------------------------------------------------
   VioCls: Clears screen.
-----------------------------------------------------------------------------*/
void VioCls(int iOutHandle, int iForeColor, int iBackColor)
    {
    int                 iCounter;
    VIOMODEINFO         ModeData;
    USHORT              usFill;


    ModeData.cb = sizeof(VIOMODEINFO);
    VioGetMode(&ModeData, VIO_HANDLE);

    usFill = (VioColor(iForeColor, iBackColor) * 0x100) + 0x20;

    for (iCounter = 0; iCounter <= ModeData.row; iCounter++)
        VioWrtNCell((PBYTE) &usFill, ModeData.col, iCounter, 0, VIO_HANDLE);

    VioSetCurPos(0, 0, VIO_HANDLE);
    }


/*-----------------------------------------------------------------------------
   VioColorPrintString: Print string with color settings.
-----------------------------------------------------------------------------*/
void VioColorPrintString (int iOutHandle, int iRow, int iCol, int iForeColor, int iBackColor, char *szString)
     {
     BYTE              usFill;

     iRow--;
     iCol--;
     usFill = VioColor(iForeColor, iBackColor);
     VioWrtCharStrAtt(szString, strlen(szString), iRow, iCol, &usFill, VIO_HANDLE);
     }


/*-----------------------------------------------------------------------------
   VioPrintString: Print string (dont modify background).
-----------------------------------------------------------------------------*/
void VioPrintString (int iOutHandle, int iRow, int iCol, char *szString)
     {
     iRow--;
     iCol--;
     VioWrtCharStr(szString, strlen(szString), iRow, iCol, VIO_HANDLE);
     }


/*-----------------------------------------------------------------------------
   VioQueryCursorRow: Get current cursor row.
-----------------------------------------------------------------------------*/
int VioQueryCursorRow(int iOutHandle)
     {
     USHORT    Row;
     USHORT    Column;


     VioGetCurPos(&Row, &Column, VIO_HANDLE);
     return(++Row);
     }


/*-----------------------------------------------------------------------------
   VioQueryCursorColumn: Get current cursor column.
-----------------------------------------------------------------------------*/
int VioQueryCursorColumn(int iOutHandle)
     {
     USHORT    Row;
     USHORT    Column;


     VioGetCurPos(&Row, &Column, VIO_HANDLE);
     return(++Column);
     }


/*-----------------------------------------------------------------------------
   VioSetCursorPosition: Move cursor.
-----------------------------------------------------------------------------*/
void VioSetCursorPosition(int iOutHandle, int iRow, int iCol)
     {
     iRow--;
     iCol--;
     VioSetCurPos(iRow, iCol, VIO_HANDLE);
     }


/*-----------------------------------------------------------------------------
   VioShowCursor: Show or hide cursor.
-----------------------------------------------------------------------------*/
void VioShowCursor(int iOutHandle, BOOL bFlag)
     {
     VIOCURSORINFO    CursorData;


     VioGetCurType(&CursorData, VIO_HANDLE);
     if (bFlag == FALSE)
        CursorData.attr = -1;
     else
        CursorData.attr = 1;
     VioSetCurType(&CursorData, VIO_HANDLE);
     }

/*-----------------------------------------------------------------------------
   VioQueryCursorVisible: Is cursor shown or hidden.
-----------------------------------------------------------------------------*/
BOOL VioQueryCursorVisible(int iOutHandle)
     {
     VIOCURSORINFO    CursorData;


     VioGetCurType(&CursorData, VIO_HANDLE);
     if (CursorData.attr == -1)
        return(FALSE);
     else
        return(TRUE);
     }


/*-----------------------------------------------------------------------------
   VioColor: Converts fg/bg pair to attribute value.
-----------------------------------------------------------------------------*/
USHORT VioColor(USHORT usFore, USHORT usBack)
     {
     USHORT   usTemp;


     usTemp = (usFore & 16) * 8 + (usBack * 16) + (usFore & 15);
     if (usTemp & 128)
        usTemp = usTemp - 128;

     return (usTemp);
     }


/*-----------------------------------------------------------------------------
   VioGetWindow: Saves part of screen area in a string.
-----------------------------------------------------------------------------*/
char *VioGetWindow(int iOutHandle, int iRow1, int iCol1, int iRow2, int iCol2)
      {
      int               iCounter;
      char              *pbWindow, *pStrPos;
      unsigned short    ulTemp;

      pbWindow = (char*) calloc( ((iRow2 - iRow1 + 1) * 2) * (iCol2 - iCol1 + 1), sizeof(char) );

      pStrPos = pbWindow;

      for (iCounter = iRow1; iCounter <= iRow2; iCounter++)
          {
          ulTemp = (iCol2 - iCol1 + 1) * 2;
          VioReadCellStr(pStrPos, &ulTemp, iCounter - 1, iCol1 - 1, VIO_HANDLE);
          pStrPos += (iCol2 - iCol1 + 1) * 2;
          }
      return (pbWindow);
      }


/*-----------------------------------------------------------------------------
   VioPutWindow: Restores window saved by VioGetWindow.
-----------------------------------------------------------------------------*/
void VioPutWindow(int iOutHandle, char *pbWindow, int iRow1, int iCol1, int iRow2, int iCol2)
      {
      int               iCounter;
      char              *pStrPos;
      unsigned short    ulTemp;

      pStrPos = pbWindow;

      for (iCounter = iRow1; iCounter <= iRow2; iCounter++)
          {
          ulTemp = (iCol2 - iCol1 + 1) * 2;
          VioWrtCellStr(pStrPos, ulTemp, iCounter - 1, iCol1 - 1, VIO_HANDLE);
          pStrPos += (iCol2 - iCol1 + 1) * 2;
          }

      free (pbWindow);
      }


/*-----------------------------------------------------------------------------
   VioFrameFill: Draws a frame and fills contents with attribute.
-----------------------------------------------------------------------------*/
void VioFrameFill(int iOutHandle, int iRow1, int iCol1, int iRow2, int iCol2, int iForeColor, int iBackColor, char cFrameType)
     {
     int                iCounter;
     char               *pStrPos;
     unsigned short     ulTemp;
     unsigned short     usFill;
     char               cUL, cUR, cLL, cLR, cH, cV;


     iRow1--;
     iCol1--;
     iRow2--;
     iCol2--;

     usFill = (VioColor(iForeColor, iBackColor) * 0x100) + 32;

     for (iCounter = iRow1; iCounter <= iRow2; iCounter++)
         VioWrtNCell((PBYTE) &usFill, (iCol2 - iCol1 + 1), iCounter, iCol1, VIO_HANDLE);


      switch (cFrameType)
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
                  cUL = cUR = cLL = cLR = cH = cV = cFrameType;
                  break;

             }

      VioWrtNChar((PBYTE) &cUL, 1, iRow1, iCol1, VIO_HANDLE);
      VioWrtNChar((PBYTE) &cH, iCol2 - iCol1 - 1, iRow1, iCol1 + 1, VIO_HANDLE);
      VioWrtNChar((PBYTE) &cUR, 1, iRow1, iCol2, VIO_HANDLE);

      for (iCounter = iRow1 + 1; iCounter <= iRow2 - 1; iCounter++)
          {
          VioWrtNChar((PBYTE) &cV, 1, iCounter, iCol1, VIO_HANDLE);
          VioWrtNChar((PBYTE) &cV, 1, iCounter, iCol2, VIO_HANDLE);
          }

      VioWrtNChar((PBYTE) &cLL, 1, iRow2, iCol1, VIO_HANDLE);
      VioWrtNChar((PBYTE) &cH, iCol2 - iCol1 - 1, iRow2, iCol1 + 1, VIO_HANDLE);
      VioWrtNChar((PBYTE) &cLR, 1, iRow2, iCol2, VIO_HANDLE);
      }


/*-----------------------------------------------------------------------------
   VioScrollVert: Scroll window vertical.
-----------------------------------------------------------------------------*/
void VioScrollVert(int iOutHandle, int iRow1, int iCol1, int iRow2, int iCol2, int iRows, int iForeColor, int iBackColor)
     {
     char	szPair[2];


     szPair[0] = ' ';
     szPair[1] = VioColor(iForeColor, iBackColor);

     if (iRows > 0) 
        {
        VioScrollDn(iRow1, iCol1, iRow2, iCol2, iRows, (PBYTE)szPair, (HVIO)iOutHandle);
        }       
     else 
        {
        VioScrollUp(iRow1, iCol1, iRow2, iCol2, -iRows, (PBYTE)szPair, (HVIO)iOutHandle);
        }
     }


/*-----------------------------------------------------------------------------
   VioFill: Fills line with blank and color.
-----------------------------------------------------------------------------*/
void VioFill(int iOutHandle, int iRow, int iCol, int iLen, int iForeColor, int iBackColor)
     {
     USHORT     usFill;

     iRow--;
     iCol--;
     usFill = (VioColor(iForeColor, iBackColor) * 0x100) + 32;
     VioWrtNCell((PBYTE) &usFill, iLen, iRow, iCol, VIO_HANDLE);
     }


/*-----------------------------------------------------------------------------
   VioFillChar: Fills line with character.
-----------------------------------------------------------------------------*/
void VioFillChar(int iOutHandle, int iRow, int iCol, int iLen, char cChar)
     {
     USHORT     usFill;

     iRow--;
     iCol--;
     VioWrtNChar((PBYTE) &cChar, iLen, iRow, iCol, VIO_HANDLE);
     }


/*-----------------------------------------------------------------------------
   VioShade: Draws shade for specified triangle.
-----------------------------------------------------------------------------*/
void VioShade(int iOutHandle, int iRow1, int iCol1, int iRow2, int iCol2)
     {
     int                iCounter;
     USHORT             usFill;


     iRow1--;
     iCol1--;
     iRow2--;
     iCol2--;

     usFill = VioColor(COLOR_GRAY, COLOR_BLACK);
     VioWrtNAttr((PBYTE) &usFill, iCol2 - iCol1 + 1, iRow2 + 1, iCol1 + 1, VIO_HANDLE);

     for (iCounter = iRow1 + 1; iCounter <= iRow2 + 1; iCounter++)
         {
         VioWrtNAttr((PBYTE) &usFill, 1, iCounter, iCol2 + 1, VIO_HANDLE);
         VioWrtNAttr((PBYTE) &usFill, 1, iCounter, iCol2 + 2, VIO_HANDLE);
         }
     }


/*-----------------------------------------------------------------------------
   VioInkey: Get keypress.
-----------------------------------------------------------------------------*/
int VioInkey(int iInHandle, BOOL bWaitFlag, int *iShiftstate)
     {
     KBDKEYINFO         CharData;


     if (bWaitFlag == 0)
        KbdCharIn(&CharData, 1, KBD_HANDLE);
     else
        KbdCharIn(&CharData, 0, KBD_HANDLE);

     if (iShiftstate != NULL)
        *iShiftstate = CharData.fsState;

     if (CharData.chScan == 224)
        {
        return(CharData.chChar);
        }
     else
        {
        if (CharData.fbStatus == 66)
           return(-CharData.chScan);
        else
           return(CharData.chChar);
        }
     }


/*-----------------------------------------------------------------------------
   VioTimer: Returns milliseconds since system start.
-----------------------------------------------------------------------------*/
int VioTimer(void)
    {
    int         iMilliSec;


    DosQuerySysInfo(QSV_MS_COUNT, QSV_MS_COUNT, &iMilliSec, 4);
    return(iMilliSec);
    }


/*-----------------------------------------------------------------------------
   strcenter: Center string within positions.
-----------------------------------------------------------------------------*/
int strcenter(int x1, int x2, char *szString)
    {
    if (strlen(szString) >= x2 - x1 + 1)
       return (x1);
    else
       return ((x2 - x1 - strlen(szString) + x1) / 2 + 1);
    }


