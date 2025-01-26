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
   STD.C
  ==========================================================================
   930106 Converted to CSET/2
  ==========================================================================
   930106 Added File and str functions.
  ==========================================================================
   970625 Added strtrim.
  ==========================================================================
   970628 Added strltrimn, strrtrimn
  ==========================================================================
   980718 Added DLLfullname and instance functions, strreplace, strterminate
  ==========================================================================
   980726 Split into file.c and std.c
  ==========================================================================
   991125 Made strreplace much safer.
  ==========================================================================
   991126 Added OemToAnsi and AnsiToOem.
  ==========================================================================
   991205 Fixed bug(s) in OemToAnsi and AnsiToOem.
  ==========================================================================
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <direct.h>

#include "stlibstc.h"

/*------------------------------------------------------------------------------
   strxtract: Extract part of string.
-----------------------------------------------------------------------------*/
char *strxtract(char *szString, int iPos1, int iPos2)
     {
     int            iCounter, iAntal;
     static char    szReturn[_MAX_PATH];


     memset (szReturn, 0, _MAX_PATH);
     iAntal = 0;
     for (iCounter = (iPos1 - 1); iCounter < (iPos1 + iPos2 - 1); iCounter++)
         {
         szReturn[iAntal] = szString[iCounter];
         iAntal++;
         }

     return (szReturn);
     }


/*------------------------------------------------------------------------------
   strpostrim: Trims a string from within string.
-----------------------------------------------------------------------------*/
char *strpostrim(char *szString, int iPosition)
     {
     int        iCounter;


     for (iCounter = iPosition - 1; iCounter > 0; iCounter--)
         {
         if (szString[iCounter] != ' ')
            {
            szString[iCounter + 1] = '\0';
            break;
            }
         }
     return (szString);
     }


/*-----------------------------------------------------------------------------
   fgetsz: fgets that replaced newline with \0.
-----------------------------------------------------------------------------*/
char* fgetsz (char *szBuffer, int iMaxLen, FILE *stream)
     {
     fgets(szBuffer, iMaxLen, stream);

     if (szBuffer[strlen(szBuffer) - 1] == '\n')
        szBuffer[strlen(szBuffer) - 1] = '\0';
     return (szBuffer);
     }


/*------------------------------------------------------------------------------
   strtrim: Trims string (removes blanks in begin and end).
-----------------------------------------------------------------------------*/
void strtrim (char* szString)
     {
     char       szTempString[1024];
     int        iCounter, iStrLen;
     int        iBlanks;


     if ( (iStrLen = strlen(szString)) == 0 )
        return;

     iBlanks = 0;

     for (iCounter = 0; iCounter < iStrLen; iCounter++)
         {
         if (szString[iCounter] == ' ')
            iBlanks++;
         else
            break;
         }

     if (iBlanks > 0)
        strcpy(szTempString, &szString[iBlanks]);
     else
        strcpy(szTempString, szString);

     for (iCounter = strlen(szTempString) - 1; iCounter > -1; iCounter--)
         {
         if (szTempString[iCounter] == ' ')
            szTempString[iCounter] == '\0';
         else
            break;
         }

     strcpy(szString, szTempString);
     }


/*------------------------------------------------------------------------------
   strltrimn: Trims n character from left in string.
-----------------------------------------------------------------------------*/
char *strltrimn(char *szString, int iPosition)
     {
     int        iCounter;

     for (iCounter = 0; iCounter <= strlen(szString) - iPosition; iCounter++)
         {
         szString[iCounter] = szString[iPosition + iCounter];
         }
     szString[iCounter] = '\0';
     return (szString);
     }


/*------------------------------------------------------------------------------
   strrtrimn: Trims n character from right in string.
-----------------------------------------------------------------------------*/
char *strrtrimn(char *szString, int iPosition)
     {
     szString[strlen(szString) - iPosition] = '\0';
     return (szString);
     }

/*-----------------------------------------------------------------------------
   strreplace: Replaces all sLookFor with sReplaceWith.
-----------------------------------------------------------------------------*/
void strreplace(char *szText, char sLookFor, char sReplaceWith)
     {
     char       *pchFound;

     if (szText == NULL)
        return;

     if (strlen(szText) == 0)
        return;

     if (sLookFor == sReplaceWith)
        return;

     /* Change all sLookFor to sReplaceWith ones */
     pchFound = strrchr(szText, sLookFor);
     while (pchFound != 0)
           {
           (*pchFound) = sReplaceWith;
           pchFound = strrchr(szText, sLookFor);
           }
     }


/*-----------------------------------------------------------------------------
   strterminate: Terminates string with sTermChar.
-----------------------------------------------------------------------------*/
void strterminate(char *szText, char sTermChar)
     {
     if (szText[strlen(szText) - 1] != sTermChar)
        {
        szText[strlen(szText)] = sTermChar;
        szText[strlen(szText) + 1] = '\0';
        }
     }


/*-----------------------------------------------------------------------------
   OemToAnsi: Converts from 850 to ANSI.
-----------------------------------------------------------------------------*/
char *OemToAnsi(char *szOemText)
     {
     static char       szAnsiText[1024] = "";


     if (szOemText == NULL)
        return (NULL);

     if (strlen(szOemText) == 0)
        return (szOemText);

     memset(szAnsiText, 0, 1024);
     strcpy(szAnsiText, szOemText);

     strreplace(szAnsiText, '', 'Å');
     strreplace(szAnsiText, 'Ž', 'Ä');
     strreplace(szAnsiText, '™', 'Ö');
     strreplace(szAnsiText, '†', 'å');
     strreplace(szAnsiText, '„', 'ä');
     strreplace(szAnsiText, '”', 'ö');
     strreplace(szAnsiText, 'š', 'Ü');
     strreplace(szAnsiText, '', 'ü');
     strreplace(szAnsiText, '', 'É');
     strreplace(szAnsiText, '‚', 'é');

     return(szAnsiText);
     }


/*-----------------------------------------------------------------------------
   AnsiToOem: Converts from ANSI to 850
-----------------------------------------------------------------------------*/
char *AnsiToOem(char *szAnsiText)
     {
     static char       szOemText[1024] = "";


     if (szAnsiText == NULL)
        return (NULL);

  
     if (strlen(szAnsiText) == 0)
        return (szAnsiText);

     memset(szOemText, 0, 1024);
     strcpy(szOemText, szAnsiText);

     strreplace(szOemText, 'Å', '');
     strreplace(szOemText, 'Ä', 'Ž');
     strreplace(szOemText, 'Ö', '™');
     strreplace(szOemText, 'å', '†');
     strreplace(szOemText, 'ä', '„');
     strreplace(szOemText, 'ö', '”');
     strreplace(szOemText, 'Ü', 'š');
     strreplace(szOemText, 'ü', '');
     strreplace(szOemText, 'É', '');
     strreplace(szOemText, 'é', '‚');

     return(szOemText);
     }
