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
//     * WDESKTOP.MOD
//     *********************************************************************
//     * Part of WIN.MOD library after split
//     *
//     *********************************************************************
//     * 931107: Split...
//     *--------------------------------------------------------------------
//     * 980129: Moved to STLIBVIO (C/C++ library)
//     *--------------------------------------------------------------------
//     *********************************************************************
#include "stlibvio.h"

// ---------------------------------------------------------------------
//  VioDesktop:
// ---------------------------------------------------------------------
void VioDesktop(PSTVIO pstv, char *szRubrik, int iFillChar, BOOL bStatus)
     {
     int	iCounter;
     char	*szFillStr;

     VioSetCursorPosition(pstv->iOutHandle, 1, 1);
     VioShowCursor(pstv->iOutHandle, FALSE);

     VioFrameFill(pstv->iOutHandle, 1, 1, pstv->iMaxRows, pstv->iMaxCols, pstv->iwfcolor, pstv->iwbcolor, 32);

     if (iFillChar == -1)
        iFillChar = 177;

     szFillStr = (char*) calloc(pstv->iMaxCols + 1, sizeof(char));
     memset(szFillStr, iFillChar, pstv->iMaxCols);
     szFillStr[pstv->iMaxCols + 1] = '\0';
     for (iCounter = 1; iCounter <= pstv->iMaxRows; iCounter++)
         VioPrintString(pstv->iOutHandle, iCounter, 1, szFillStr);
     free(szFillStr);

     if (strlen(szRubrik) > 0)
        {
        VioFill(pstv->iOutHandle, 1, 1, pstv->iMaxCols, pstv->ibbcolor, pstv->ibfcolor);
        if (strlen(szRubrik) > pstv->iMaxCols - 4)
           szRubrik[pstv->iMaxCols - 3] = '\0';
        VioPrintString(pstv->iOutHandle, 1, 2, szRubrik);
        }

     if (bStatus == TRUE)
        VioFill(pstv->iOutHandle, pstv->iMaxRows, 1, pstv->iMaxCols, pstv->ibbcolor, pstv->ibfcolor);
     }

