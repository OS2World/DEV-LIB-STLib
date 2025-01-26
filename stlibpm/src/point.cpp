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
   POINT.C
  ==========================================================================
  void IconSize (PULONG pxIcon, PULONG pyIcon)
  void PointerNormal()
  void PointerWait()
  ==========================================================================
   930107 Converted to CSET/2
  ==========================================================================
****************************************************************************/
#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include "stlibpmc.h"

/****************************************************************************
  IconSize: Get x and y size of icons.                               930107
****************************************************************************/
void IconSize (PULONG pxIcon, PULONG pyIcon)
     {
     *pxIcon  = WinQuerySysValue(HWND_DESKTOP, SV_CXICON);
     *pyIcon  = WinQuerySysValue(HWND_DESKTOP, SV_CYICON);
     }

/****************************************************************************
  PointerNormal: Set pointer to standard arrow.                      930107
****************************************************************************/
void PointerNormal()
     {
     WinSetPointer(HWND_DESKTOP,WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, (ULONG) NULL));
     }

/****************************************************************************
  PointerWait: Set pointer to stopwatch.                             930107
****************************************************************************/
void PointerWait()
     {
     WinSetPointer(HWND_DESKTOP,WinQuerySysPointer(HWND_DESKTOP, SPTR_WAIT, (ULONG) NULL));
     }
