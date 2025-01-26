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
   FONT.C

  ==========================================================================
   921211 Converted to CSET/2
  ==========================================================================
   950729 Added SelectFont and FontSize, renamed DefFontSize to
          QueryFontSize. (MA,1h)
  ==========================================================================
****************************************************************************/
#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <string.h>
#include <memory.h>
#include "stlibpmc.h"


/*------------------------------------------------------------------------------------
  QueryFontSize: Returns font size of current font.
------------------------------------------------------------------------------------*/
void QueryFontSize(HWND hwnd, PULONG pulFontWidth, PULONG pulFontHeight)
     {
     FONTMETRICS                fm;
     HPS                        hps;

     hps = WinGetPS(hwnd);
     GpiQueryFontMetrics(hps, sizeof(fm), &fm);
     *pulFontWidth = fm.lEmInc;
     *pulFontHeight = fm.lMaxBaselineExt;
     WinReleasePS(hps);
     }


/*-----------------------------------------------------------------------------
   SelectFont: Selects or deselects a font in a ps.
-----------------------------------------------------------------------------*/
ULONG SelectFont(BOOL bFlag, HPS hps, char *szFontName, int iFontSize)
     {
     ULONG              lcidFont;
     FATTRS             fat;

     if (bFlag == TRUE)
        {
        lcidFont = 1L;
        memset(&fat, 0, sizeof(fat));
        fat.usRecordLength = sizeof(fat);

        strcpy(fat.szFacename, szFontName);
        ScaleFont (hps, iFontSize);
        GpiCreateLogFont(hps, 0L, lcidFont, &fat);
        GpiSetCharSet(hps, lcidFont);
        return (lcidFont);
        }
     else
        {
        GpiSetCharSet(hps, 0L);
        GpiDeleteSetId(hps, iFontSize);
        return (TRUE);
        }
     }


/*---------------------------------------------------------------------------
  ScaleFont: Sets font size to selected size in points.
---------------------------------------------------------------------------*/
BOOL ScaleFont (HPS hpsPrinter, USHORT usFontSize)
     {
     HDC                hdc;
     LONG               xRes, yRes;
     POINTL             aptl[2];
     SIZEF              sizef;


     hdc = GpiQueryDevice(hpsPrinter);

     /****************************************************************************
       F”r att skala en font s† exakt som m”jligt anv„nder jag f”ljande formel:
           ”nskad_points * pixels_meter * meters_tum * antaltum_punkt
       samt hanterar avrundningsfel med hj„lp av * och /.
       D† GpiSetCharBox kr„ver fixed-v„rden g”rs slutligen en s†dan konvertering.
                                                                    (MA, 941127)
     ****************************************************************************/

     DevQueryCaps(hdc, CAPS_HORIZONTAL_RESOLUTION, 1, &xRes);
     DevQueryCaps(hdc, CAPS_VERTICAL_RESOLUTION,   1, &yRes);

     sizef.cx = MAKEFIXED( (((usFontSize * xRes * 254) / 10000) * 1384 ) / 100000, 0 );
     sizef.cy = MAKEFIXED( (((usFontSize * yRes * 254) / 10000) * 1384 ) / 100000, 0 );

     return GpiSetCharBox(hpsPrinter, &sizef);
     }

