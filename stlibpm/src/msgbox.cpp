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
   MSGBOX.C
  ==========================================================================
   BOOL MessageBox (HWND hwnd, USHORT usBoxType, PSZ szTitle, PSZ szMessage)
   void DEBUG (PSZ szMessage)
  ==========================================================================
   921211 Converted to CSET/2
  ==========================================================================
   950523 Fixed a bug in EXTDEBUG
  ==========================================================================
   950605 Fixed the bug that occured when fixing bug in EXTDEBUG.
  ==========================================================================
****************************************************************************/
#define INCL_WIN

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "stlibpmc.h"

/*************************************************************************************
  MessageBox: Simplified messagebox.
*************************************************************************************/
BOOL MessageBox (HWND hwnd, USHORT usBoxType, PSZ szTitle, PSZ szMessage)
     {
     ULONG      ulTmp;

     if (hwnd == 0)
        hwnd = HWND_DESKTOP;

     switch (usBoxType)
            {
            case 0:
                 WinMessageBox(HWND_DESKTOP, hwnd, szMessage, szTitle, 0, MB_CUANOTIFICATION | MB_APPLMODAL | MB_MOVEABLE);
                 return (TRUE);
                 break;
            case 1:
            case 2:
                 if (usBoxType == 1)
                    ulTmp = MB_DEFBUTTON1;
                 else
                    ulTmp = MB_DEFBUTTON2;
                 if (WinMessageBox(HWND_DESKTOP, hwnd, szMessage, szTitle, 0, MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL | MB_MOVEABLE | ulTmp) == MBID_YES)
                    return (TRUE);
                 else
                    return (FALSE);
                 break;
            case 3:
                 break;
            default:
                 break;
            }
     return (TRUE);
     }


/*************************************************************************************
  EXTDEBUG: Shows debug-message/WinLastError.
*************************************************************************************/
void EXTDEBUG (HAB hab, ULONG ulRetCode, PSZ szMessage)
     {
     ERRORID            erridLast;
     char               szErrNumber[256], szSevere[50];
     int                iSeverity = 0, iErrorNo = 0;


     erridLast = WinGetLastError(hab);

     iSeverity = ERRORIDSEV(erridLast);
     iErrorNo = ERRORIDERROR(erridLast);

      switch (iSeverity)
             {
             case SEVERITY_NOERROR:
                  sprintf(szSevere, "SEVERITY_NOERROR");
                  break;

             case SEVERITY_WARNING:
                  sprintf(szSevere, "SEVERITY_WARNING");
                  break;

             case SEVERITY_ERROR:
                  sprintf(szSevere, "SEVERITY_ERROR");
                  break;

             case SEVERITY_SEVERE:
                  sprintf(szSevere, "SEVERITY_SEVERE");
                  break;

             case SEVERITY_UNRECOVERABLE:
                  sprintf(szSevere, "SEVERITY_UNRECOVERABLE");
                  break;
             }
 
     sprintf(szErrNumber, "%s\nYour retcode => %d\nSeverity = %s \nErrorcode = %x", szMessage, ulRetCode, szSevere, iErrorNo);

     WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, (PSZ)szErrNumber, (PSZ)"*** EXT_DEBUG ***",
                   0, MB_CUANOTIFICATION | MB_APPLMODAL | MB_MOVEABLE);
     }


/*************************************************************************************
  DEBUG: Shows debug-message using HWND_DESKTOP only.
*************************************************************************************/
void DEBUG (PSZ szMessage)
     {
     WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, (PSZ)szMessage, (PSZ)"*** DEBUG ***",
                   0, MB_CUANOTIFICATION | MB_APPLMODAL | MB_MOVEABLE);
     }

