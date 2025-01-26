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
//     * EDITINT.MOD
//     *********************************************************************
//     * FUNCTION EditInt%
//     *********************************************************************
//     * 870513: Created
//     *--------------------------------------------------------------------
//     * 900629: Changed for New update of STLIB7
//     *--------------------------------------------------------------------
//     * 900630: Lots of bugs fixed/ ENFormat added
//     *--------------------------------------------------------------------
//     * 900708: Total rewrite. Mouse support added
//     *--------------------------------------------------------------------
//     * 921127: Mouse support //oved
//     *--------------------------------------------------------------------
//     * 930419: Chnged for new color settings
//     *--------------------------------------------------------------------
//     * 930610: Fixed colours for new field look.
//     *--------------------------------------------------------------------
//     * 931107: Split into two files.
//     *--------------------------------------------------------------------
//     * 980302: Ported to C (stlibvio)
//     *********************************************************************
#include "stlibvio.h"


// ---------------------------------------------------------------------
//  VioEditInt: Edit an int in a vio window.
// ---------------------------------------------------------------------
int VioEditInt (PSTVIOWINDOW hwnd, int *iValue, int iRow, int iCol, int iFormLen, int aiRet[], int iRetLen)
    {
    char        szIntString[64];
    char        szBuffer[64];
    int         iStrLen;
    int         iRetKey;

    sprintf(szBuffer, "%%-%dd", iFormLen);
    sprintf(szIntString, szBuffer, *iValue);

    iRetKey = VioEditString (hwnd, szIntString, iRow, iCol, strlen(szIntString), aiRet, iRetLen, IDH_INT);

    *iValue = atoi(szIntString);
    return (iRetKey);
    }


