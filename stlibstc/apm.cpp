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
/**************************************************************************

   Modul:      APM.CPP, Function library for APM support in eComStation

   Version:    0.001

   Tid:        001210, 001210, 1h

  ---------------------------------------------------------------------
   001210:  Stole code from PC/2 (MA,1h).
  ---------------------------------------------------------------------
**************************************************************************/
#define INCL_NOPMAPI
#define INCL_DOS

#include <os2.h>
#include <bsesub.h>
#include <bsedev.h>


/* Generic includes
  ------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stlibstc.h"


/* APM FUNCTIONS
***********************************/
typedef struct _APMSTRUCT
        {
        HANDLE		handle;

        int		iACStatus
	int		iBatteryLife
        } APMSTRUCT;
typedef APMSTRUCT *PAPMSTRUCT;


BOOL ApmOpen(PAPMSTRUCT papm);
void ApmClose(PAPMSTRUCT papm);


/*-----------------------------------------------------------------------
   ApmOpen: Connects to APM driver.
-----------------------------------------------------------------------*/
BOOL ApmOpen(PAPMSTRUCT papm)
     {
     unsigned long	ulAction;


     if (DosOpen("\\DEV\\APM$", &papm->handle, &ulAction, 0, FILE_NORMAL, OPEN_ACTION_OPEN_IF_EXISTS,
        OPEN_FLAGS_FAIL_ON_ERROR | OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE, NULL) == NO_ERROR)
        return (TRUE);
     else
        return (FALSE);
     }


/*-----------------------------------------------------------------------
   ApmClose: Disconnects from APM driver.
-----------------------------------------------------------------------*/
void ApmClose(PAPMSTRUCT papm)
     {
     DosClose(papm->handle);
     }



//    memset(&powereventAPM, 0, sizeof(powereventAPM));
//    ulReturnCode=0;
//    ----- Set PWR MGMT function
//    powereventAPM.usSubID=(pHP->ulStatusFlag2 & ENABLEAPM ? 0x0003 : 0x0004);
//    ulPacketSize=sizeof(powereventAPM);
//    ulDataSize=sizeof(ulReturnCode);
//    DosDevIOCtl(hfileAPM, IOCTL_POWER, POWER_SENDPOWEREVENT,&powereventAPM, ulPacketSize, &ulPacketSize,
//                &ulReturnCode, ulDataSize, &ulDataSize);

//    ----- if APM is enabled (usPowerFlags!=0) and 
//    ----- if it is a battery capable PC (ucACStatus!=255 and ucBatteryLife!=255) 
//    ----- as it does not make sense just to display line power for line powered only PCs 
//    ----- Get the current APM power status
//    memset(&powerstatusAPM, 0, sizeof(powerstatusAPM));
//    ulReturnCode=0;
//    powerstatusAPM.usParmLength=sizeof(powerstatusAPM);
//    ulPacketSize=sizeof(powerstatusAPM);
//    ulDataSize=sizeof(ulReturnCode);
//    DosDevIOCtl(hfileAPM, IOCTL_POWER, POWER_GETPOWERSTATUS, &powerstatusAPM, ulPacketSize, &ulPacketSize, &ulReturnCode, ulDataSize, &ulDataSize);
//powerstatusAPM.ucACStatus
//powerstatusAPM.ucBatteryLife

