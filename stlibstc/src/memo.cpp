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
   MEMO.C
  ==========================================================================
  int MemoLen(char* szMemoFile, int iMemoPos);
  char* MemoGet(char* szMemoFile, int iMemoPos);
  int MemoPut(char* szMemoFile, int iMemoSize, char* szMemo);
  ==========================================================================
   940706 Converted to CSET/2 from BASIC.
  ==========================================================================
   950809 Kraftig omarbetning med m†nga nya anrop.
  ==========================================================================
****************************************************************************/
#define INCL_NOBASEAPI
#define INCL_NOPMAPI

#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <time.h>
#include "stlibstc.h"


int SearchId(FILE *stream, unsigned long ulMemoId);
void WriteHeader(FILE *stream, PMEMOHEADER pmh, int iFlag);
void ReadHeader(FILE *stream, PMEMOHEADER pmh);


/*------------------------------------------------------------------------------------
  SearchId: Searched memofile for specified memo id.
------------------------------------------------------------------------------------*/
int SearchId(FILE *stream, unsigned long ulMemoId)
     {
     MEMOHEADER         mh;
     MEMOITEM           mi;
     int                iCounter;
     unsigned long      ulCurrentPos;


     ReadHeader(stream, &mh);

     for (iCounter = 0; iCounter < mh.ulMemoId; iCounter++)
         {
         ulCurrentPos = ftell(stream);
         fread(&mi, sizeof(MEMOITEM), 1, stream);
         if (mi.ulMemoId == ulMemoId)
            {
            return (ulCurrentPos);
            }
         fseek(stream, mi.ulNextMemoPos, SEEK_SET);
         }
     return (FALSE);
     }


/*------------------------------------------------------------------------------------
  WriteHeader: Updates memofile header.
------------------------------------------------------------------------------------*/
void WriteHeader(FILE *stream, PMEMOHEADER pmh, int iFlag)
     {
     char               szBuffer[7];
     time_t             ltime;
     struct tm          *curtime;
     int                iCounter;


     time(&ltime);
     curtime = localtime(&ltime);
     sprintf(szBuffer, "%.2d%.2d%.2d", curtime->tm_year, curtime->tm_mon+1, curtime->tm_mday);

     if (iFlag == FALSE)
        {
        fseek(stream, 0, SEEK_SET);
        for (iCounter = 0; iCounter < 6; iCounter++)
            pmh->szDate[iCounter] = szBuffer[iCounter];
        fwrite(pmh, sizeof(MEMOHEADER), 1, stream);
        }
     else
        {
        fseek(stream, 0, SEEK_SET);
        pmh->szMemoFile[0] = 'S';
        pmh->szMemoFile[1] = 'T';
        pmh->szMemoFile[2] = 'M';
        pmh->szMemoFile[3] = 2;
        pmh->ulMemoId = 0;
        pmh->ulMemoActive = 0;
        for (iCounter = 0; iCounter < 6; iCounter++)
            pmh->szDate[iCounter] = szBuffer[iCounter];
        fwrite(pmh, sizeof(MEMOHEADER), 1, stream);
        }
     }


/*------------------------------------------------------------------------------------
  ReadHeader: Gets memofile header.
------------------------------------------------------------------------------------*/
void ReadHeader(FILE *stream, PMEMOHEADER pmh)
     {
     fseek(stream, 0, SEEK_SET);
     fread(pmh, sizeof(MEMOHEADER), 1, stream);
     }


/*------------------------------------------------------------------------------------
  MemoInfo: Retrieve memo info from header.
------------------------------------------------------------------------------------*/
BOOL MemoInfo(char *szMemoFile, PMEMOHEADER pmh)
     {
     FILE       *stream;
     char       *szMemo;

     if ((stream = fopen((char*)szMemoFile, "rb")) != 0)
        {
        ReadHeader(stream, pmh);
        fclose(stream);
        return (TRUE);
        }
     return(FALSE);
     }


/*------------------------------------------------------------------------------------
  MemoGet: Gets memo from memofile.
------------------------------------------------------------------------------------*/
char *MemoGet(char *szMemoFile, unsigned long ulMemoPos)
     {
     FILE               *stream;
     char               *szMemo;
     MEMOITEM           mi;

     if ((stream = fopen((char*)szMemoFile, "rb")) != 0)
        {
        fseek(stream, ulMemoPos, SEEK_SET);
        fread(&mi, sizeof(MEMOITEM), 1, stream);
        szMemo = (char *)malloc(mi.ulMemoLen);
        memset(szMemo, 0, mi.ulMemoLen);
        fread(szMemo, sizeof(char), mi.ulMemoLen, stream);
        fclose(stream);
        return (szMemo);
        }
     return((char*)NULL);
     }


/*------------------------------------------------------------------------------------
  MemoGetId: Gets memo from memofile by Id.
------------------------------------------------------------------------------------*/
char *MemoGetId(char *szMemoFile, unsigned long ulMemoId)
     {
     FILE               *stream;
     char               *szMemo;
     MEMOITEM           mi;
     unsigned long      ulMemoPos;

     if ((stream = fopen((char*)szMemoFile, "rb")) != 0)
        {
        if ( (ulMemoPos = SearchId(stream, ulMemoId)) != 0)
           {
           fseek(stream, ulMemoPos, SEEK_SET);
           fread(&mi, sizeof(MEMOITEM), 1, stream);
           szMemo = (char *)malloc(mi.ulMemoLen);
           memset(szMemo, 0, mi.ulMemoLen);
           fread(szMemo, sizeof(char), mi.ulMemoLen, stream);
           fclose(stream);
           return (szMemo);
           }
        else
           {
           fclose(stream);
           return((char*)NULL);
           }
        }
     return((char*)NULL);
     }


/*------------------------------------------------------------------------------------
  MemoQueryId: Gets id using position.
------------------------------------------------------------------------------------*/
int MemoQueryId(char *szMemoFile, unsigned long ulMemoPos)
     {
     FILE               *stream;
     MEMOITEM           mi;


     if ((stream = fopen((char*)szMemoFile, "rb")) != 0)
        {
        fseek(stream, ulMemoPos, SEEK_SET);
        fread(&mi, sizeof(MEMOITEM), 1, stream);
        fclose(stream);
        return (mi.ulMemoId);
        }
     return(FALSE);
     }


/*------------------------------------------------------------------------------------
  MemoQueryPos: Gets position using Id.
------------------------------------------------------------------------------------*/
int MemoQueryPos(char *szMemoFile, unsigned long ulMemoId)
     {
     FILE               *stream;
     MEMOITEM           mi;
     unsigned long      ulMemoPos;


     if ((stream = fopen((char*)szMemoFile, "rb")) != 0)
        {
        if ( (ulMemoPos = SearchId(stream, ulMemoId)) != 0)
           {
           fseek(stream, ulMemoPos, SEEK_SET);
           fread(&mi, sizeof(MEMOITEM), 1, stream);
           fclose(stream);
           return (mi.ulMemoId);
           }
        else
           {
           fclose(stream);
           return(FALSE);
           }
        }
     return(FALSE);
     }


/*------------------------------------------------------------------------------------
  MemoLen: Gets memolength from memofile.
------------------------------------------------------------------------------------*/
int MemoLen(char* szMemoFile, unsigned long ulMemoPos)
     {
     FILE               *stream;
     MEMOITEM           mi;


     if ((stream = fopen((char*)szMemoFile, "rb")) != 0)
        {
        fseek(stream, ulMemoPos, SEEK_SET);
        fread(&mi, sizeof(MEMOITEM), 1, stream);
        fclose(stream);
        return (mi.ulMemoLen);
        }
     return(FALSE);
     }


/*------------------------------------------------------------------------------------
  MemoLenId: Gets memolength from memofile using Id.
------------------------------------------------------------------------------------*/
int MemoLenId(char* szMemoFile, unsigned long ulMemoId)
     {
     FILE               *stream;
     MEMOITEM           mi;
     unsigned long      ulMemoPos;


     if ((stream = fopen((char*)szMemoFile, "rb")) != 0)
        {
        if ( (ulMemoPos = SearchId(stream, ulMemoId)) != 0)
           {
           fseek(stream, ulMemoPos, SEEK_SET);
           fread(&mi, sizeof(MEMOITEM), 1, stream);
           fclose(stream);
           return (mi.ulMemoLen);
           }
        else
           {
           fclose(stream);
           return(FALSE);
           }
        }
     return(FALSE);
     }


/*------------------------------------------------------------------------------------
  MemoPut: Saves memo to memofile.
------------------------------------------------------------------------------------*/
int MemoPut(char *szMemoFile, unsigned long ulMemoSize, char *szMemo)
     {
     FILE               *stream;
     MEMOHEADER         mh;
     unsigned long      ulMemoPos;
     MEMOITEM           mi;
     char               szBuffer[7];
     time_t             ltime;
     struct tm          *curtime;
     int                iCounter;


     /* F”rs”k ”ppna filen f”r l„sning */
     if ((stream = fopen((char*)szMemoFile, "r+b")) == 0)
        {
        /* Om inte s† skapar vi den */
        if ((stream = fopen((char*)szMemoFile,"w+b")) == 0)
           return (FALSE);
        else
           WriteHeader(stream, &mh, TRUE);
        }
     /* H„mta headern */
     ReadHeader(stream, &mh);

     /* G† till slutet av filen och spara position */
     fseek(stream, 0, SEEK_END);
     ulMemoPos = ftell(stream);

     /* Skriv memo't i filen */
     mi.ulMemoId = mh.ulMemoId + 1;
     mi.ulMemoLen = ulMemoSize;
     mi.sMemoActive = 1;
     time(&ltime);
     curtime = localtime(&ltime);
     sprintf(szBuffer, "%.2d%.2d%.2d", curtime->tm_year, curtime->tm_mon+1, curtime->tm_mday);
     for (iCounter = 0; iCounter < 6; iCounter++)
         mi.szDate[iCounter] = szBuffer[iCounter];
     mi.ulNextMemoPos = ulMemoPos + sizeof(MEMOITEM) + mi.ulMemoLen - 1;

     fwrite(&mi, sizeof(MEMOITEM), 1, stream);
     fwrite((char*)szMemo, sizeof(char), mi.ulMemoLen, stream);

     mh.ulMemoId++;
     mh.ulMemoActive++;
     WriteHeader(stream, &mh, FALSE);

     /* St„ng filen, †terv„nd */
     fclose(stream);
     return (ulMemoPos);
     }


/*------------------------------------------------------------------------------------
  MemoPutId: Saves memo to memofile returning Id.
------------------------------------------------------------------------------------*/
int MemoPutId(char *szMemoFile, unsigned long ulMemoSize, char *szMemo)
     {
     FILE               *stream;
     MEMOHEADER         mh;
     unsigned long      ulMemoPos;
     MEMOITEM           mi;
     char               szBuffer[7];
     time_t             ltime;
     struct tm          *curtime;
     int                iCounter;


     /* F”rs”k ”ppna filen f”r l„sning */
     if ((stream = fopen((char*)szMemoFile, "r+b")) == 0)
        {
        /* Om inte s† skapar vi den */
        if ((stream = fopen((char*)szMemoFile,"w+b")) == 0)
           return (FALSE);
        else
           WriteHeader(stream, &mh, TRUE);
        }
     /* H„mta headern */
     ReadHeader(stream, &mh);

     /* G† till slutet av filen och spara position */
     fseek(stream, 0, SEEK_END);
     ulMemoPos = ftell(stream);

     /* Skriv memo't i filen */
     mi.ulMemoId = mh.ulMemoId + 1;
     mi.ulMemoLen = ulMemoSize;
     mi.sMemoActive = 1;
     time(&ltime);
     curtime = localtime(&ltime);
     sprintf(szBuffer, "%.2d%.2d%.2d", curtime->tm_year, curtime->tm_mon+1, curtime->tm_mday);
     for (iCounter = 0; iCounter < 6; iCounter++)
         mi.szDate[iCounter] = szBuffer[iCounter];
     mi.ulNextMemoPos = ulMemoPos + sizeof(MEMOITEM) + mi.ulMemoLen - 1;

     fwrite(&mi, sizeof(MEMOITEM), 1, stream);
     fwrite((char*)szMemo, sizeof(char), mi.ulMemoLen, stream);

     mh.ulMemoId++;
     mh.ulMemoActive++;
     WriteHeader(stream, &mh, FALSE);

     /* St„ng filen, †terv„nd */
     fclose(stream);
     return (mi.ulMemoId);
     }


/*------------------------------------------------------------------------------------
  MemoDelete: Flags memo as deleted.
------------------------------------------------------------------------------------*/
BOOL MemoDelete(char *szMemoFile, unsigned long ulMemoPos)
     {
     FILE               *stream;
     MEMOITEM           mi;
     MEMOHEADER         mh;


     if ((stream = fopen((char*)szMemoFile, "r+b")) != 0)
        {
        /* Uppdatera sMemoActive */
        fseek(stream, ulMemoPos, SEEK_SET);
        fread(&mi, sizeof(MEMOITEM), 1, stream);
        mi.sMemoActive = 0;
        fseek(stream, ulMemoPos, SEEK_SET);
        fwrite(&mi, sizeof(MEMOITEM), 1, stream);

        /* Minska antalet i headern */
        ReadHeader(stream, &mh);
        mh.ulMemoActive--;
        WriteHeader(stream, &mh, FALSE);

        fclose(stream);
        return (TRUE);
        }
     return(FALSE);
     }


/*------------------------------------------------------------------------------------
  MemoDeleteId: Flags memo as deleted using Id.
------------------------------------------------------------------------------------*/
BOOL MemoDeleteId(char *szMemoFile, unsigned long ulMemoId)
     {
     FILE               *stream;
     MEMOITEM           mi;
     MEMOHEADER         mh;
     unsigned long      ulMemoPos;


     if ((stream = fopen((char*)szMemoFile, "r+b")) != 0)
        {
        if ( (ulMemoPos = SearchId(stream, ulMemoId)) != 0)
           {

           /* Uppdatera sMemoActive */
           fseek(stream, ulMemoPos, SEEK_SET);
           fread(&mi, sizeof(MEMOITEM), 1, stream);
           mi.sMemoActive = 0;
           fseek(stream, ulMemoPos, SEEK_SET);
           fwrite(&mi, sizeof(MEMOITEM), 1, stream);

           /* Minska antalet i headern */
           ReadHeader(stream, &mh);
           mh.ulMemoActive--;
           WriteHeader(stream, &mh, FALSE);

           fclose(stream);
           return (TRUE);
           }
        else
           {
           fclose(stream);
           return(FALSE);
           }
        }
     return(FALSE);
     }

