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
/****************************************************************
                                                      2000-08-12
   Include File: STLIBSTC.H
   Description:  Helper functions for generic programs.
****************************************************************/


/* BTRIEVE FUNCTIONS
***********************************/
unsigned short BTRV(int operation, char *posblock, char *databuf, int *datalen, char *keybuf, int keynum);
unsigned short BTRINIT(char *szInitParam);
unsigned short BTREXIT(void);

/* DATE FUNCTIONS
***********************************/
int DateSerial(char *szDate, int iBase);
char *SerialDate(int iDateSerial, int iBase);
int DayOfWeek(char *szDate);
char *DateToday(int iFlag);
int MonthLen(int iYear, int iMonth);


/* ARRAY FUNCTIONS
***********************************/
void *ArrayNew(unsigned int iItemLen, unsigned int iAntal);
void ArrayFree(char *array);
void *ArrayResize(char *array, unsigned int iItemLen, unsigned int iNewSize);
void *ArrayInsert(char *array, unsigned int iItemLen, unsigned int iMaxValue, unsigned int iPosition);
void *ArrayAppend(char *array, unsigned int iItemLen, unsigned int iMaxValue);
void *ArrayDelete(char *array, unsigned int iItemLen, unsigned int iMaxValue, unsigned int iPosition);
void ArrayCopy(char *array, unsigned int iItemLen, unsigned int iFrom, unsigned int iTo);
void ArraySwap(char *array, unsigned int iItemLen, unsigned int iItem1, unsigned int iItem2);


#ifdef FALSE

/* MEMO FUNCTIONS
***********************************/
typedef struct _MEMOHEADER    /* memoheader */
        {
        char            szMemoFile[4];
        unsigned long   ulMemoId;
        unsigned long   ulMemoActive;
        char            szDate[6];
        } MEMOHEADER;

typedef MEMOHEADER *PMEMOHEADER;


typedef struct _MEMOITEM    /* memoitem */
        {
        unsigned long   ulMemoId;
        unsigned long   ulMemoLen;
        char            sMemoActive;
        char            szDate[6];
        unsigned long   ulNextMemoPos;
        } MEMOITEM;

typedef MEMOITEM *PMEMOITEM;

BOOL MemoInfo(char* szMemoFile, PMEMOHEADER pmh);
char* MemoGet(char* szMemoFile, unsigned long ulMemoPos);
char* MemoGetId(char* szMemoFile, unsigned long ulMemoId);
int MemoQueryId(char* szMemoFile, unsigned long ulMemoPos);
int MemoQueryPos(char* szMemoFile, unsigned long ulMemoId);
int MemoLen(char* szMemoFile, unsigned long ulMemoPos);
int MemoLenId(char* szMemoFile, unsigned long ulMemoId);
int MemoPut(char* szMemoFile, unsigned long ulMemoSize, char* szMemo);
int MemoPutId(char* szMemoFile, unsigned long ulMemoSize, char* szMemo);
BOOL MemoDelete(char* szMemoFile, unsigned long ulMemoPos);
BOOL MemoDeleteId(char* szMemoFile, unsigned long ulMemoId);
#endif

#ifdef INCL_DOS
/* FILE FUNCTIONS
***********************************/
#define FILE_COUNT_MAX_FILES    250

#define FILE_COUNT_MAX_PATH     255

#define FILE_COUNT_NORMAL       0
#define FILE_COUNT_READONLY     1
#define FILE_COUNT_HIDDEN       2
#define FILE_COUNT_SYSTEM       4
#define FILE_COUNT_ARCHIVE      32

#define FILE_COUNT_SORTNONE     0
#define FILE_COUNT_SORTNAME     1
#define FILE_COUNT_SORTEXT      2
#define FILE_COUNT_SORTSIZE     4
#define FILE_COUNT_SORTDATE     8

typedef struct _FILELISTENTRY
        {
        char            szName[FILE_COUNT_MAX_PATH];
        int             iFileSize;
        int             iAttrib;
        short           sYear;
        char            cMonth;
        char            cDay;
        char            cHour;
        char            cMinute;
        char            cSecond;
        } FILELISTENTRY;
typedef FILELISTENTRY *PFILELISTENTRY;

typedef struct _FILELIST
        {
        int             iFiles;
        int             iSizeTotal;
        int             iSizeAlloc;
        PFILELISTENTRY  pfl;
        } FILELIST;
typedef FILELIST *PFILELIST;

int FileCountDirectories (char* szPathWild);
int FileCountFiles (char* szPathWild, int iAttributes);
int FileGetFiles (char* szPathWild, PFILELIST pfl, int iAttributes, int iSortOrder);
int FileGetDirectories (char* szPathWild, PFILELIST pfl, int iSortOrder);

int DosIn (int iSize, int iPort);
BOOL DosOut (int iSize, int iPort, int iValue);

char* GetExeDir (void);
void SetExeDirCurrent (void);
char *DosGetProcessPath(PID pid);
int GetDLLFullName(char *szDLLName, char *szFullPath);
int CheckForPreviousInstance (char *szAppName, int iFirstInstance);
int GetInstanceData(char *szAppName, char *szAppData);
int SetInstanceData(char *szAppName, int iAppDataLen, char *szAppData);
char *FileGetPath (char* szFullFile);
char *FileGetFile (char* szFullFile);
char *FileGetExt (char* szFullFile);
char *FileGetFileExt (char* szFullFile);
void FileSetDir (char* szFullFile);

#endif

/* STD FUNCTIONS
***********************************/
char *strxtract(char *szString, int iPos1, int iPos2);
char *strpostrim(char *szString, int iPosition);
#ifndef __stdio_h
#include <stdio.h>
#endif
char* fgetsz(char *, int, FILE *);
void strtrim (char* szString);
char *strltrimn(char *szString, int iPosition);
char *strrtrimn(char *szString, int iPosition);
void strreplace(char *szText, char sLookFor, char sReplaceWith);
void strterminate(char *szText, char sTermChar);
char *OemToAnsi(char *szOemText);
char *AnsiToOem(char *szAnsiText);

#ifdef INCL_DOS

/* SERIAL FUNCTIONS
***********************************/
typedef struct _SERIALSTRUCT
        {
        unsigned short   usPort;
        unsigned short   usBpsRate;
        unsigned short   usDataBit;
        unsigned short   usParity;
        unsigned short   usStopBit;
        unsigned short   usReadTimeOut;
        unsigned short   usWriteTimeOut;
        BOOL             bExitTask;

        char             acRecBuf[2000];
        int              iHead, iTail;

        HFILE            hSerial;
        TID              tidRecieveThread;
        unsigned long    hEvent; /* HEV */
        unsigned long    hMutex; /* HMTX */
        } SERIALSTRUCT;
typedef SERIALSTRUCT *PSERIALSTRUCT;


void _Optlink SerialRecieveThread(PVOID pss);

int SerialInit(PSERIALSTRUCT pss);
int SerialExit(PSERIALSTRUCT pss);
int SerialSetSpeed(PSERIALSTRUCT pss, unsigned short usBpsRate);
int SerialSetTimeout(PSERIALSTRUCT pss, unsigned short usReadTimeOut, unsigned short usWriteTimeOut);
int SerialSetBits(PSERIALSTRUCT pss, USHORT usDataBit, USHORT usParity, USHORT usStopBit);
int SerialGetC(PSERIALSTRUCT pss, unsigned short usTimeout);
int SerialPutC(PSERIALSTRUCT pss, unsigned short usTimeout, char cSendChar);
int SerialRecieveString(PSERIALSTRUCT pss, char *szRcvString, int iMaxLen, unsigned short usTimeout);
int SerialSendString(PSERIALSTRUCT pss, unsigned short usTimeout, char *szSendString);

#endif
