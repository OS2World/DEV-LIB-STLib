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
   PROFILE.C
  ==========================================================================
  BOOL ReadProfile (HAB hab, char* szFile, char* szApp, char* szKey, char* szItem, ULONG iItemSize);
  BOOL WriteProfile (HAB hab, char* szFile, char* szApp, char* szKey, char* szItem, ULONG iItemSize);


  ==========================================================================
   940706   Started this code.
  ==========================================================================
****************************************************************************/
#define INCL_WIN
#define INCL_GPI
#define INCL_DOS

#include <os2.h>
#include "stlibpmc.h"


/*-----------------------------------------------------------------------------
   ReadProfile: Reads settings from the application profile.
-----------------------------------------------------------------------------*/
BOOL ReadProfile (HAB hab, char* szFile, char* szApp, char* szKey, char* szItem, ULONG iItemSize)
     {
     ULONG          pulBufferMax;
     HINI           hIni;

     hIni = PrfOpenProfile(hab, szFile);

     if ( PrfQueryProfileData(hIni, (PSZ)szApp, (PSZ)szKey, (PVOID) szItem, (PULONG) &iItemSize) )
        {
        PrfCloseProfile(hIni);
        return (TRUE);
        }
     else
        return (FALSE);
     }


/*-----------------------------------------------------------------------------
   WriteProfile: Writes settings to the application profile.
-----------------------------------------------------------------------------*/
BOOL WriteProfile (HAB hab, char* szFile, char* szApp, char* szKey, char* szItem, ULONG iItemSize)
     {
     HINI               hIni;

     hIni = PrfOpenProfile(hab, szFile);
     if (hIni)
        {
        PrfWriteProfileData(hIni, (PSZ) szApp, (PSZ) szKey, (PVOID) szItem, iItemSize);
        PrfCloseProfile(hIni);
        return (TRUE);
        }
     else
        return (FALSE);
     }

