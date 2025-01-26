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
   FILE.C
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
   980726 Split into file.c and std.c and added count functions
  ==========================================================================
****************************************************************************/
#define INCL_NOPMAPI
#define INCL_DOS
#define INCL_DOSERRORS
#define INCL_DOSFILEMGR
#define INCL_DOSDEVIOCTL


#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <direct.h>

#include "stlibstc.h"

int FileCount (BOOL bFlag, char* szPathWild, int iAttributes);
int FileGet (BOOL bFlag, char* szPathWild, PFILELIST pfl, int iAttributes, int iSortOrder);



//  testcfg$ I/O definitions
// -----------------------------------------------
struct
       {
       USHORT port;
       USHORT size;
       } par;

struct
       {
       ULONG data;
       } dta;

struct {
       USHORT portaddr;
       USHORT size;
       ULONG datawrite;
       } param;


static HFILE        hTestCfg = (HFILE) 0;


/*-----------------------------------------------------------------------------
   DosIn: Reads from a port.
-----------------------------------------------------------------------------*/
int DosIn (int iSize, int iPort)
    {
    ULONG               ulAction;
    ULONG               ulLen;
    APIRET              rc;


    if (hTestCfg == (HFILE) 0)
       {
       rc = DosOpen("/dev/testcfg$", &hTestCfg, &ulAction, 0, FILE_NORMAL, FILE_OPEN, OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYNONE,(PEAOP2)NULL);
       if (rc != NO_ERROR)
          return (-1);
       }


    par.port = iPort;
    switch (iSize)
           {
           case 8:
                par.size = 1;
                break;

           case 16:
                par.size = 2;
                break;

           case 32:
                par.size = 3;
                break;

           default:
                par.size = 3;
                break;
           }

    rc = 0;
    rc = DosDevIOCtl(hTestCfg, 0x80, 0x41, (PULONG)&par, sizeof(par), &ulLen, (PULONG)&dta, sizeof(dta), &ulLen);
    if (rc != NO_ERROR)
       return (-1);

    return (dta.data);
    }


/*-----------------------------------------------------------------------------
   DosOut: Writes to a port.
-----------------------------------------------------------------------------*/
BOOL DosOut (int iSize, int iPort, int iValue)
    {
    ULONG               ulAction;
    ULONG               ulLen;
    APIRET              rc;


    if (hTestCfg == (HFILE) 0)
       {
       rc = DosOpen("/dev/testcfg$", &hTestCfg, &ulAction, 0, FILE_NORMAL, FILE_OPEN, OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYNONE,(PEAOP2)NULL);
       if (rc != NO_ERROR)
          return (FALSE);
       }


    param.portaddr = iPort;
    param.datawrite = iValue;
    switch (iSize)
           {
           case 8:
                param.size = 1;
                break;

           case 16:
                param.size = 2;
                break;

           case 32:
                param.size = 3;
                break;

           default:
                param.size = 3;
                break;
           }

    rc = 0;
    rc = DosDevIOCtl(hTestCfg, 0x80, 0x42, (PULONG)&param, sizeof(param), &ulLen, NULL, 0, &ulLen);
    if (rc != NO_ERROR)
       return (FALSE);
    else
       return (TRUE);
    }


/*-----------------------------------------------------------------------------
   SetExeDirCurrent: Sets the .EXE directory as current directory.
-----------------------------------------------------------------------------*/
void SetExeDirCurrent (void)
     {
     auto APIRET        rc;
     auto PTIB          pTib;
     auto PPIB          pPib;
     static char        szBuffer[256];

     short              sCounter;

     DosGetInfoBlocks(&pTib, &pPib);
     rc = DosQueryModuleName(pPib->pib_hmte, sizeof(szBuffer), szBuffer);

     if (szBuffer[1] == ':')
        DosSetDefaultDisk((ULONG)(toupper(szBuffer[0]) - 'A' + 1));

     sCounter = strlen(szBuffer);

     while ((szBuffer[sCounter] != '\\') && (sCounter > 0))
           sCounter--;

     if (sCounter > 0)
        {
        if (szBuffer[1] == ':' && sCounter == 2)
           {
           szBuffer[sCounter + 1] = '\0';
           DosSetCurrentDir(szBuffer);
           }
        else
           {
           szBuffer[sCounter] = '\0';
           DosSetCurrentDir(szBuffer);
           }
        }
     }


/*-----------------------------------------------------------------------------
   GetExeDir: Returns the .EXE directory directory.
-----------------------------------------------------------------------------*/
char* GetExeDir (void)
     {
     static char        szBuffer[CCHMAXPATH];
     APIRET             rc;
     TIB                *pTib;
     PIB                *pPib;
     int                iCounter;


     DosGetInfoBlocks(&pTib, &pPib);
     DosQueryModuleName(pPib->pib_hmte, sizeof(szBuffer), szBuffer);

     iCounter = strlen(szBuffer);

     while ((szBuffer[iCounter] != '\\') && (iCounter > 0))
           iCounter--;

     if (iCounter > 0)
        {
        szBuffer[iCounter + 1] = '\0';
        return (szBuffer);
        }
     else
        return (NULL);
     }


/*------------------------------------------------------------------------------
   GetDLLFullName: Returns full pathname/size for specified .DLL file
-----------------------------------------------------------------------------*/
int GetDLLFullName(char *szDLLName, char *szFullPath)
     {
     HMODULE            hModule;
     HMODULE            hModFile;
     FILE               *handle;
     char               szFileFound[_MAX_PATH];
     int                iFileLength;
     APIRET             rc;
     HDIR               hdirFindHandle = HDIR_CREATE;
     FILEFINDBUF3       FindBuffer     = {0};
     ULONG              ulFindCount    = 1;


     DosLoadModule(NULL, 0, szDLLName, &hModFile);

     rc = DosQueryModuleHandle(szDLLName, &hModule);
     if (rc != NO_ERROR)
       {
       DosFreeModule(hModFile);
       return(-1);
       }

    rc = DosQueryModuleName(hModule, _MAX_PATH, szFileFound);
     if (rc != NO_ERROR)
       {
       DosFreeModule(hModFile);
       return (-1);
       }
    else
       {
       strcpy(szFullPath, szFileFound);
       DosFreeModule(hModFile);
       if (DosFindFirst(szFullPath, &hdirFindHandle, FILE_NORMAL, &FindBuffer, sizeof(FILEFINDBUF3), &ulFindCount, FIL_STANDARD) ==NO_ERROR)
          {
          iFileLength = FindBuffer.cbFile;
          DosFindClose(hdirFindHandle);
          return(iFileLength);
          }
       else
          return(-1);
       }
  }


/*----------------------------------------------------------------------
  CheckForPreviousInstance: Looks for previous instance via shared mem.
-----------------------------------------------------------------------*/
int CheckForPreviousInstance (char *szAppName, int iFirstInstance)
     {
     int        *pvShrObject = NULL;
     char       szMemName[_MAX_PATH];


     sprintf(szMemName, "\\SHAREMEM\\%s", szAppName);

     /* If found, return hwnd of creator */
     if (DosGetNamedSharedMem((PPVOID) &pvShrObject, szMemName, PAG_READ) == NO_ERROR)
        return (*pvShrObject);
     else
        {
        /* If not found, put our hwnd in sharemem */
        if (DosAllocSharedMem((PPVOID) &pvShrObject, szMemName, 4096, PAG_COMMIT | PAG_WRITE ) == NO_ERROR)
           *pvShrObject = iFirstInstance;
        return(0);
        }
     }


/*----------------------------------------------------------------------
  GetInstanceData: Gets data from (previous) instance via shared mem.
-----------------------------------------------------------------------*/
int GetInstanceData(char *szAppName, char *szAppData)
     {
     int        *pvShrObject = NULL;
     char       szMemName[_MAX_PATH];


     sprintf(szMemName, "\\SHAREMEM\\%s", szAppName);

     if (DosGetNamedSharedMem((PPVOID) &pvShrObject, szMemName, PAG_READ) == NO_ERROR)
        {
        memcpy(szAppData, pvShrObject + 8, *(pvShrObject + 4));
        return (*pvShrObject);
        }
     else
        return (0);
     }


/*----------------------------------------------------------------------
  SetInstanceData: Sets data for (previous) instance via shared mem.
-----------------------------------------------------------------------*/
int SetInstanceData(char *szAppName, int iAppDataLen, char *szAppData)
     {
     int        *pvShrObject = NULL;
     char       szMemName[_MAX_PATH];


     sprintf(szMemName, "\\SHAREMEM\\%s", szAppName);

     if (DosGetNamedSharedMem((PPVOID) &pvShrObject, szMemName, PAG_READ | PAG_WRITE) == NO_ERROR)
        {
        *(pvShrObject + 4) = iAppDataLen;
        memcpy(pvShrObject + 8, szAppData, *(pvShrObject + 4));
        return (*pvShrObject);
        }
     else
        return (0);
     }


/*------------------------------------------------------------------------------
   FileGetPath: Get path from full filename.
-----------------------------------------------------------------------------*/
char *FileGetPath (char* szFullFile)
     {
     static char        szReturn[_MAX_DRIVE + _MAX_DIR];
     char               szDrive[_MAX_DRIVE];
     char               szDir[_MAX_DIR];
     char               szName[_MAX_FNAME];
     char               szExt[_MAX_EXT];


     _splitpath(szFullFile, szDrive, szDir, szName, szExt);

     sprintf(szReturn, "%s%s", szDrive, szDir);

     return (szReturn);
     }


/*------------------------------------------------------------------------------
   FileGetFile: Get file w/o ext from full filename.
-----------------------------------------------------------------------------*/
char *FileGetFile (char* szFullFile)
     {
     static char        szReturn[_MAX_FNAME];
     char               szDrive[_MAX_DRIVE];
     char               szDir[_MAX_DIR];
     char               szName[_MAX_FNAME];
     char               szExt[_MAX_EXT];


     _splitpath(szFullFile, szDrive, szDir, szName, szExt);

     sprintf(szReturn, "%s", szName);

     return (szReturn);
     }


/*------------------------------------------------------------------------------
   FileGetExt: Get ext from full filename.
-----------------------------------------------------------------------------*/
char *FileGetExt (char* szFullFile)
     {
     static char        szReturn[_MAX_EXT];
     char               szDrive[_MAX_DRIVE];
     char               szDir[_MAX_DIR];
     char               szName[_MAX_FNAME];
     char               szExt[_MAX_EXT];


     _splitpath(szFullFile, szDrive, szDir, szName, szExt);

     sprintf(szReturn, "%s", szExt);

     return (szReturn);
     }


/*------------------------------------------------------------------------------
   FileGetFileExt: Get filename + ext from full filename.
-----------------------------------------------------------------------------*/
char *FileGetFileExt (char* szFullFile)
     {
     static char        szReturn[_MAX_EXT];
     char               szDrive[_MAX_DRIVE];
     char               szDir[_MAX_DIR];
     char               szName[_MAX_FNAME];
     char               szExt[_MAX_EXT];


     _splitpath(szFullFile, szDrive, szDir, szName, szExt);

     sprintf(szReturn, "%s%s", szName, szExt);

     return (szReturn);
     }


/*------------------------------------------------------------------------------
   FileSetDir: Set full filename directory/drive as current.
-----------------------------------------------------------------------------*/
void FileSetDir (char* szFullFile)
     {
     char       szDrive[_MAX_DRIVE];
     char       szDir[_MAX_DIR];
     char       szName[_MAX_FNAME];
     char       szExt[_MAX_EXT];


     _splitpath(szFullFile, szDrive, szDir, szName, szExt);

     _chdrive(szDrive[0] - 64);

     szDir[strlen(szDir) - 1] = '\0';
     chdir(szDir);
     }


/*------------------------------------------------------------------------------
   FileCountFiles: Returns amount of files in path.
-----------------------------------------------------------------------------*/
int FileCountFiles (char* szPathWild, int iAttributes)
     {
     return (FileCount(FALSE, szPathWild, iAttributes));
     }


/*------------------------------------------------------------------------------
   FileCountDirectories: Returns amount of directories in path.
-----------------------------------------------------------------------------*/
int FileCountDirectories (char* szPathWild)
     {
     return (FileCount(TRUE, szPathWild, FILE_COUNT_NORMAL));
     }


/*------------------------------------------------------------------------------
   FileCount: Returns amount of dirs/files in path.
-----------------------------------------------------------------------------*/
int FileCount (BOOL bFlag, char* szPathWild, int iAttributes)
     {
     HDIR          hdirFindHandle = HDIR_CREATE;
     FILEFINDBUF3  *pulResultBuf;
     ULONG         ulResultBufLen = sizeof(FILEFINDBUF3) * FILE_COUNT_MAX_FILES;
     ULONG         ulFindCount;
     ULONG         ulFiles = 0;
     APIRET        rc;


     pulResultBuf = (PFILEFINDBUF3) calloc(sizeof(FILEFINDBUF3), FILE_COUNT_MAX_FILES);

     ulFindCount = FILE_COUNT_MAX_FILES;

     if (bFlag == TRUE)
        rc = DosFindFirst( szPathWild, &hdirFindHandle, MUST_HAVE_DIRECTORY, pulResultBuf, ulResultBufLen, &ulFindCount, FIL_STANDARD);
     else
        rc = DosFindFirst( szPathWild, &hdirFindHandle, iAttributes, pulResultBuf, ulResultBufLen, &ulFindCount, FIL_STANDARD);

     switch (rc)
            {
            case NO_ERROR:
                 ulFiles = ulFindCount;
                 break;

            case ERROR_NO_MORE_FILES:
                 DosFindClose(hdirFindHandle);
                 free(pulResultBuf);
                 return(0);
                 break;

            default:
                 DosFindClose(hdirFindHandle);
                 free(pulResultBuf);
                 return (-1);
            }


     while (rc != ERROR_NO_MORE_FILES)
           {
           ulFindCount = FILE_COUNT_MAX_FILES;
           rc = DosFindNext(hdirFindHandle, pulResultBuf, ulResultBufLen, &ulFindCount);

           if (rc != NO_ERROR && rc != ERROR_NO_MORE_FILES)
              {
              DosFindClose(hdirFindHandle);
              free(pulResultBuf);
              return (-1);
              }
           else
              ulFiles += ulFindCount;
           }

     DosFindClose(hdirFindHandle);
     free(pulResultBuf);
     return (ulFiles);
     }


/*------------------------------------------------------------------------------
   FileGetFiles: Returns files information.
-----------------------------------------------------------------------------*/
int FileGetFiles (char* szPathWild, PFILELIST pfl, int iAttr, int iSort)
     {
     return (FileGet(FALSE, szPathWild, pfl, iAttr, iSort));
     }


/*------------------------------------------------------------------------------
   FileGetDirectories: Returns directories information.
-----------------------------------------------------------------------------*/
int FileGetDirectories (char* szPathWild, PFILELIST pfl, int iSort)
     {
     return (FileGet(TRUE, szPathWild, pfl, FILE_COUNT_NORMAL, iSort));
     }


/*------------------------------------------------------------------------------
   FileGet: Returns dirs/files in path.
-----------------------------------------------------------------------------*/
int FileGet (BOOL bFlag, char* szPathWild, PFILELIST pfl, int iAttributes, int iSortOrder)
     {
     int           iFiles, iCounter;
     HDIR          hdirFindHandle = HDIR_CREATE;
     char          *pcRes;
     FILEFINDBUF3  *pulResultBuf, *pulRB;
     ULONG         ulResultBufLen;
     ULONG         ulFindCount;
     ULONG         ulFiles = 0;
     APIRET        rc;



     pfl->iFiles = 0;
     pfl->iSizeTotal = 0;
     pfl->iSizeAlloc = 0;
     pfl->pfl = NULL;

     iFiles = FileCount(bFlag, szPathWild, iAttributes);
     if (iFiles == -1)
        return (-1);

     pulRB = pulResultBuf = (PFILEFINDBUF3) calloc(sizeof(FILEFINDBUF3), iFiles);
     ulResultBufLen = sizeof(FILEFINDBUF3) * iFiles;
     ulFindCount = iFiles;

     if (bFlag == TRUE)
        rc = DosFindFirst(szPathWild, &hdirFindHandle, MUST_HAVE_DIRECTORY, pulResultBuf, ulResultBufLen, &ulFindCount, FIL_STANDARD);
     else
        rc = DosFindFirst(szPathWild, &hdirFindHandle, iAttributes, pulResultBuf, ulResultBufLen, &ulFindCount, FIL_STANDARD);

     if (rc != NO_ERROR)
        {
        DosFindClose(hdirFindHandle);
        free(pulResultBuf);
        return (-1);
        }

     if ( bFlag == TRUE && strcmpi(pulResultBuf[0].achName, ".") == 0 )
        {
        pcRes = (char*) pulResultBuf;
        pcRes += pulResultBuf->oNextEntryOffset;
        pulResultBuf = (PFILEFINDBUF3) pcRes;
        iFiles--;
        }

     pfl->pfl = (PFILELISTENTRY) calloc(sizeof(FILELISTENTRY), iFiles);

     for (iCounter = 0; iCounter < iFiles; iCounter++)
         {
         pfl->iSizeTotal += pulResultBuf->cbFile;
         pfl->iSizeAlloc += pulResultBuf->cbFileAlloc;

         strcpy(pfl->pfl[iCounter].szName, pulResultBuf->achName);
         pfl->pfl[iCounter].iFileSize = pulResultBuf->cbFile;
         pfl->pfl[iCounter].iAttrib = pulResultBuf->attrFile;
         pfl->pfl[iCounter].sYear = 1980 + pulResultBuf->fdateLastWrite.year;
         pfl->pfl[iCounter].cMonth = pulResultBuf->fdateLastWrite.month;
         pfl->pfl[iCounter].cDay = pulResultBuf->fdateLastWrite.day;
         pfl->pfl[iCounter].cHour = pulResultBuf->ftimeLastWrite.hours;
         pfl->pfl[iCounter].cMinute = pulResultBuf->ftimeLastWrite.minutes;
         pfl->pfl[iCounter].cSecond = pulResultBuf->ftimeLastWrite.twosecs * 2;

         pcRes = (char*) pulResultBuf;
         pcRes += pulResultBuf->oNextEntryOffset;
         pulResultBuf = (PFILEFINDBUF3) pcRes;
         }

     pfl->iFiles = iFiles;

     DosFindClose(hdirFindHandle);
     free(pulRB);
     return(iFiles);
     }

