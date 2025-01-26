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
   BTRIEVE32.C
  ==========================================================================
   unsigned short BTRV(int operation, char *posblock, char *databuf, int *datalen, char *keybuf, int keynum)
  ==========================================================================
   930925 Converted to CSET/2
  ==========================================================================
****************************************************************************/
#define INCL_NOPMAPI

#include <os2.h>
#include "stlibstc.h"

/* Declare DLL entrypoint */
unsigned short _Far16 _Pascal btrcall(short operation, char * posblock, char * databuf,
                                      short * datalen, char * keybuf, short keylen, char ckeynum);

unsigned short _Far16 _Pascal BTRVINIT(char * szInitParam);

unsigned short _Far16 _Pascal BTRVSTOP(void);

/*************************************************************************************
  BTRV: Thunk procedure for Btrieve 16bit's btrcall.
*************************************************************************************/
unsigned short BTRV(int operation, char *posblock, char *databuf, int *datalen, char *keybuf, int keynum)
     {
     static short	dlen;
     short 		rc;

     if (datalen == NULL)
        dlen = 0;
     else
        dlen = *datalen;

     /* Make call to the dynamic link entry point */
     rc = btrcall(operation, posblock, databuf, &dlen, keybuf, 255, (char) keynum);

     if (datalen != NULL)
        *datalen = dlen;

     return (rc);
     }

/*************************************************************************************
  BTRINIT: Thunk procedure for Btrieve 16bit's BTRVINIT.
*************************************************************************************/
unsigned short BTRINIT(char *szInitParam)
     {
     short 		rc;

     /* Make call to the dynamic link entry point */
     rc = BTRVINIT(szInitParam);
     return (rc);
     }

/*************************************************************************************
  BTRINIT: Thunk procedure for Btrieve 16bit's BTRVINIT.
*************************************************************************************/
unsigned short BTREXIT(void)
     {
     short 		rc;

     /* Make call to the dynamic link entry point */
     rc = BTRVSTOP();
     return (rc);
     }
