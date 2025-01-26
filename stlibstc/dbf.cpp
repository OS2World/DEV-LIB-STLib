/****************************************************************************
   DBF.C
  ==========================================================================

  ==========================================================================
   971016 Started...
  ==========================================================================
   990129 Lots more, major rewrite with deleted support.
  ==========================================================================
****************************************************************************/
#define  INCL_BASE
#include <os2.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dbf.h"


int WrkIsRecDeleted (PDBF pdbf);
// File management
//============================================================================================================================

/*************************************************************************************
  DbfCreate: Create Database
*************************************************************************************/
int DbfCreate (PDBF* ppdbf, char *pszFilename, unsigned long ulFieldCount, PDBFCREATE pdbfcreate)
    {
    BOOL           fMemo;
    char           ch = HEADEREND;
    char           ch2 = DATAEND;
    DBFFIELD       dbffield;
    PDBF           pdbf;
    PDBFCREATE     pdbfcreate2;
    char*          psz;
    int            rc = RC_OK;
    unsigned long  ulI;

    if (ppdbf)
       {
       if ((pdbf = (PDBF) calloc (sizeof (DBF), 1)) != 0)
          {
          *ppdbf = pdbf;
          if ((pdbf->pfile = fopen (pszFilename, "wb")) != 0)
             {
             pdbf->fChanged = TRUE;
             pdbf->ulLenMemo = 512;
             strcpy (pdbf->szFilename, pszFilename);
             strcpy (pdbf->szFootprint, FOOTPRINT);
             pdbf->dbfheader.chVersion = FILE_DB3;
             pdbf->dbfheader.usLenHeader = (ulFieldCount * sizeof (DBFFIELD)) + sizeof (DBFHEADER) + 1;
             if (fwrite (&pdbf->dbfheader, sizeof (DBFHEADER), 1, pdbf->pfile) != 0)
                {
                fMemo = FALSE;
                pdbf->dbfheader.usLenRecord = 1;
                for (ulI = 0, pdbfcreate2 = pdbfcreate; ulI < ulFieldCount; ulI++, pdbfcreate2++)
                    {
                    memset (&dbffield, 0, sizeof (DBFFIELD));
                    strcpy (dbffield.szName, pdbfcreate2->szName);
                    strupr (dbffield.szName);
                    dbffield.chType = pdbfcreate2->chType;
                    switch (dbffield.chType)
                           {
                           case FIELD_CHARACTER:
                                if (pdbfcreate2->chLength == 0 || pdbfcreate2->chLength > 250)
                                   {
                                   rc = RC_WRONGFIELDLENGTH;
                                   }
                                else
                                   {
                                   dbffield.chLength = pdbfcreate2->chLength;
                                   }
                                break;
                           case FIELD_DATE:
                                dbffield.chLength = 8;
                                break;
                           case FIELD_LOGICAL:
                                dbffield.chLength = 1;
                                break;
                           case FIELD_MEMO:
                                dbffield.chLength = 10;
                                pdbf->dbfheader.chVersion = FILE_DB3MEMO;
                                fMemo = TRUE;
                                break;
                           case FIELD_NUMERIC:
                                if (pdbfcreate2->chLength == 0 || pdbfcreate2->chLength > 20 || pdbfcreate2->chLength < pdbfcreate2->chDecimal + 1)
                                   {
                                   rc = RC_WRONGFIELDLENGTH;
                                   }
                                else
                                   {
                                   dbffield.chLength = pdbfcreate2->chLength;
                                   dbffield.chDecimal = pdbfcreate2->chDecimal;
                                   }
                                break;
                           default:
                                rc = RC_WRONGFIELDTYPE;
                                break;
                           }
                    pdbf->dbfheader.usLenRecord += dbffield.chLength;
                    if (fwrite (&dbffield, sizeof (DBFFIELD), 1, pdbf->pfile) == 0)
                       {
                       rc = RC_FILEWRITE;
                       break;
                       }
                    }
                if (rc == RC_OK)
                   {
                   if (fwrite (&ch, 1, 1, pdbf->pfile) != 0)
                      {
                      if (fwrite (&ch2, 1, 1, pdbf->pfile) != 0)
                         {
                         if (fMemo)
                            {
                            strcpy (pdbf->szMemoFilename, pszFilename);
                            if ((psz = strrchr (pdbf->szMemoFilename, '.')) != 0)
                               {
                               strcpy ((psz + 1), "DBT");
                               }
                            if ((pdbf->pfileMemo = fopen (pdbf->szMemoFilename, "wb")) != 0)
                               {
                               pdbf->memheader.ulNext = 1;
                               if (fwrite (&pdbf->memheader, sizeof (MEMHEADER), 1, pdbf->pfileMemo) == 0)
                                  {
                                  rc = RC_FILEWRITE;
                                  }
                               }
                            else
                               {
                               rc = RC_FILECREATE;
                               }
                            }
                         if ((rc = DbfClose (pdbf)) == RC_OK)
                            {
                            rc = DbfOpen (ppdbf, pszFilename, FALSE);
                            }
                         }
                      else
                         {
                         rc = RC_FILEWRITE;
                         }
                      }
                   else
                      {
                      rc = RC_FILEWRITE;
                      }
                   }
                }
             else
                {
                rc = RC_FILEWRITE;
                }
             }
       else
          {
          rc = RC_FILECREATE;
          }



          }
       else
          {
          rc = RC_MEMORYERROR;
          }
       }
    return rc;
    }


/*************************************************************************************
  DbfStructureInit: Allocates a DBFCREATE array
*************************************************************************************/
PDBFCREATE DbfStructureInit(int iFields)
    {
    PDBFCREATE pdbfcreate;


    pdbfcreate = (PDBFCREATE) malloc( sizeof(DBFCREATE) * iFields);
    memset(pdbfcreate, 0, sizeof(DBFCREATE) * iFields);
    return (pdbfcreate);
    }


/*************************************************************************************
  DbfStructureAdd: Adds field to DBFCREATE array
*************************************************************************************/
int DbfStructureAdd(PDBFCREATE pdbfcreate, int iFieldId, char *szName, char chType, char chLength, char chDecimal)
    {
    strncpy(pdbfcreate[iFieldId].szName, szName, 10);
    pdbfcreate[iFieldId].chType = chType;
    pdbfcreate[iFieldId].chLength = chLength;
    pdbfcreate[iFieldId].chDecimal = chDecimal;
    return(0);
    }


/*************************************************************************************
   DbfOpen: Open database
*************************************************************************************/
int DbfOpen (PDBF* ppdbf, char* pszFilename, BOOL bIgnoreDeleted)
   {
   BOOL  fMemo;
   PDBF  pdbf;
   char*   psz;
   int    rc = RC_OK;
   unsigned long ulFieldPosition = 1;
   unsigned long ulI;

   if   (ppdbf)
        {
        if   ((pdbf = (PDBF)calloc (sizeof (DBF), 1)) != 0)
             {
             *ppdbf = pdbf;
             if   ((pdbf->pfile = fopen (pszFilename, "rb+")) != 0)
                  {
                  if   (fread (&pdbf->dbfheader, sizeof (DBFHEADER), 1, pdbf->pfile) != 0)
                       {
                       if   (pdbf->dbfheader.chVersion == FILE_DB3 || pdbf->dbfheader.chVersion == FILE_DB3MEMO)
                            {
                            pdbf->chType = pdbf->dbfheader.chVersion;
                            pdbf->ulNumFields = (pdbf->dbfheader.usLenHeader - 1 - sizeof (DBFHEADER)) / sizeof (DBFFIELD);
                            if   ((pdbf->pdbffield = (PDBFFIELD) malloc (pdbf->ulNumFields * sizeof (DBFFIELD))) != 0)
                                 {
                                 if   ((pdbf->pszCache = (char*)malloc (pdbf->dbfheader.usLenRecord)) != 0)
                                      {
                                      if   ((pdbf->pszCacheOriginal = (char*)malloc (pdbf->dbfheader.usLenRecord)) != 0)
                                           {
                                           if   (fread (pdbf->pdbffield, sizeof (DBFFIELD), pdbf->ulNumFields, pdbf->pfile) != 0)
                                                {
                                                fMemo = FALSE;
                                                for  (ulI = 0; ulI < pdbf->ulNumFields; ulI++)
                                                     {
                                                     (pdbf->pdbffield + ulI)->pvAddress = pdbf->pszCache + ulFieldPosition;
                                                     ulFieldPosition += (pdbf->pdbffield + ulI)->chLength;
                                                     if   ((pdbf->pdbffield + ulI)->chType == FIELD_MEMO)
                                                          {
                                                          fMemo = TRUE;
                                                          }
                                                     }
                                                if   (fMemo)
                                                     {
                                                     strcpy (pdbf->szMemoFilename, pszFilename);
                                                     if   ((psz = strrchr (pdbf->szMemoFilename, '.')) != 0)
                                                          {
                                                          strcpy ((psz + 1), "DBT");
                                                          }
                                                     if   ((pdbf->pfileMemo = fopen (pdbf->szMemoFilename, "rb+")) == 0)
                                                          {
                                                          rc = RC_FILEOPEN;
                                                          }
                                                     rc = CheckMemo (pdbf);
                                                     }
                                                if   (rc == RC_OK)
                                                     {
                                                     strcpy (pdbf->szFilename, pszFilename);
                                                     strcpy (pdbf->szFootprint, FOOTPRINT);
                                                     if   (pdbf->dbfheader.ulNumRecords)
                                                          {
                                                          if (bIgnoreDeleted == TRUE)
                                                             pdbf->fSkipDeleted = TRUE;
                                                          else
                                                             pdbf->fSkipDeleted = FALSE;
                                                          pdbf->ulCurrentRecord = 1;
                                                          DbfGo (pdbf, pdbf->ulCurrentRecord);
                                                          rc = pdbf->iStatus;
                                                          }
                                                     else
                                                          {
                                                          pdbf->ulCurrentRecord = 0;
                                                          }
                                                     }
                                                }
                                           else
                                                {
                                                rc = RC_FILEREAD;
                                                }
                                           }
                                      else
                                           {
                                           rc = RC_MEMORYERROR;
                                           }
                                      }
                                 else
                                      {
                                      rc = RC_MEMORYERROR;
                                      }
                                 }
                            else
                                 {
                                 rc = RC_MEMORYERROR;
                                 }
                            }
                       else
                            {
                            rc = RC_INVALIDFILE;
                            }
                       }
                  else
                       {
                       rc = RC_FILEREAD;
                       }
                  }
             else
                  {
                  rc = RC_FILEOPEN;
                  }
             }
        else
             {
             rc = RC_MEMORYERROR;
             }
        }

   pdbf->iStatus = rc;

   if (rc == RC_OK)
      return (TRUE);
   else
      return (FALSE);
   }


/*************************************************************************************
   DbfClose: Close Database
*************************************************************************************/
int DbfClose (PDBF pdbf)
    {
    int         rc = RC_OK;
    time_t      timet;
    struct tm*  tms;


    if ((rc = CheckPDBF (pdbf)) == RC_OK)
       {
       if (pdbf->fChanged)
          {
          timet = time (NULL);
          tms = localtime (&timet);
          pdbf->dbfheader.tdate.chYear = tms->tm_year;
          pdbf->dbfheader.tdate.chMonth = tms->tm_mon + 1;
          pdbf->dbfheader.tdate.chDay = tms->tm_mday;
          if (fseek (pdbf->pfile, 0, SEEK_SET) == 0)
             {
             if (fwrite (&pdbf->dbfheader, sizeof (DBFHEADER), 1, pdbf->pfile) == 0)
                {
                rc = RC_FILEWRITE;
                }
             }
          else
             {
             rc = RC_FILEPOSITION;
             }
          }
       if (pdbf->pdbffield)
          {
          free (pdbf->pdbffield);
          }
       if (pdbf->pszCache)
          {
          free (pdbf->pszCache);
          }
       if (pdbf->pszCacheOriginal)
          {
          free (pdbf->pszCacheOriginal);
          }
       if (pdbf->pfileMemo)
          {
          fclose (pdbf->pfileMemo);
          }
       if (pdbf->pfile)
          {
          fclose (pdbf->pfile);
          }
       memset (pdbf, 0, sizeof (DBF));
       free (pdbf);
       }
    return rc;
    }


/*************************************************************************************
   DBFType: Query database type
*************************************************************************************/
int DbfType (PDBF pdbf, char* pch)
    {
    int         rc = RC_OK;


    if ((rc = CheckPDBF (pdbf)) == RC_OK)
       {
       *pch = pdbf->chType;
       }
    return rc;
    }


/*************************************************************************************
   DbfPack: Pack database
*************************************************************************************/
int DbfPack (char* pszFilename)
   {
   char           ch = HEADEREND;
   char           ch2 = DATAEND;
   char           szFilename [CCHMAXPATH];
   DBFHEADER      dbfheader;
   FILE*          pfileIn;
   FILE*          pfileOut;
   PVOID          pv;
   int            rc = RC_OK;
   unsigned long  ulI;
   unsigned long  ulNumFields;
   unsigned long  ulNumRecords;


   if   ((pfileIn = fopen (pszFilename, "rb+")) != 0)
        {
        strcpy (szFilename, tmpnam (NULL));
        if   ((pfileOut = fopen (szFilename, "wb")) != NULL)
             {
             if   ((pv = malloc (65535)) != 0)
                  {
                  if   (fseek (pfileIn, 0, SEEK_SET) == 0)
                       {
                       if   (fseek (pfileOut, 0, SEEK_SET) == 0)
                            {
                            if   (fread (&dbfheader, sizeof (DBFHEADER), 1, pfileIn) != 0)
                                 {
                                 ulNumFields = (dbfheader.usLenHeader - 1 - sizeof (DBFHEADER)) / sizeof (DBFFIELD);
                                 if   (fwrite (&dbfheader, sizeof (DBFHEADER), 1, pfileOut) != 0)
                                      {
                                      if   (fread (pv, sizeof (DBFFIELD), ulNumFields, pfileIn) != 0)
                                           {
                                           if   (fwrite (pv, sizeof (DBFFIELD), ulNumFields, pfileOut) != 0)
                                                {
                                                if   (fwrite (&ch, 1, 1, pfileOut) != 0)
                                                     {
                                                     if   (dbfheader.ulNumRecords)
                                                          {
                                                          ulNumRecords = dbfheader.ulNumRecords;
                                                          dbfheader.ulNumRecords = 0;
                                                          for  (ulI = 1; ulI <= ulNumRecords; ulI++)
                                                               {
                                                               if   (fseek (pfileIn, dbfheader.usLenHeader + ((ulI - 1) * dbfheader.usLenRecord), SEEK_SET) == 0)
                                                                    {
                                                                    if   (fread (pv, dbfheader.usLenRecord, 1, pfileIn) != 0)
                                                                         {
                                                                         if   (*((PCHAR) pv) == ' ')
                                                                              {
                                                                              if   (fwrite (pv, dbfheader.usLenRecord, 1, pfileOut) != 0)
                                                                                   {
                                                                                   dbfheader.ulNumRecords ++;
                                                                                   }
                                                                              else
                                                                                   {
                                                                                   rc = RC_FILEWRITE;
                                                                                   break;
                                                                                   }
                                                                              }
                                                                         }
                                                                    else
                                                                         {
                                                                         rc = RC_FILEREAD;
                                                                         break;
                                                                         }
                                                                    }
                                                               else
                                                                    {
                                                                    rc = RC_FILEPOSITION;
                                                                    break;
                                                                    }
                                                               }
                                                          if   (rc == RC_OK)
                                                               {
                                                               if   (fwrite (&ch2, 1, 1, pfileOut) != 0)
                                                                    {
                                                                    if   (fseek (pfileOut, 0, SEEK_SET) == 0)
                                                                         {
                                                                         if   (fwrite (&dbfheader, sizeof (DBFHEADER), 1, pfileOut) == 0)
                                                                              {
                                                                              rc = RC_FILEWRITE;
                                                                              }
                                                                         }
                                                                    else
                                                                         {
                                                                         rc = RC_FILEPOSITION;
                                                                         }
                                                                    }
                                                               else
                                                                    {
                                                                    rc = RC_FILEWRITE;
                                                                    }
                                                               }
                                                          }
                                                     }
                                                else
                                                     {
                                                     rc = RC_FILEWRITE;
                                                     }
                                                }
                                           else
                                                {
                                                rc = RC_FILEWRITE;
                                                }
                                           }
                                      else
                                           {
                                           rc = RC_FILEREAD;
                                           }
                                      }
                                 else
                                      {
                                      rc = RC_FILEWRITE;
                                      }
                                 }
                            else
                                 {
                                 rc = RC_FILEREAD;
                                 }
                            }
                       else
                            {
                            rc = RC_FILEPOSITION;
                            }
                       }
                  else
                       {
                       rc = RC_FILEPOSITION;
                       }
                  free (pv);
                  }
             else
                  {
                  rc = RC_MEMORYERROR;
                  }
             fclose (pfileIn);
             fclose (pfileOut);
             if   (rc == RC_OK)
                  {
                  if   (!DosCopy (szFilename,
                                  pszFilename,
                                  DCPY_EXISTING))
                       {
                       rc = RC_FILECOPY;
                       }
                  }
             remove (szFilename);
             }
        else
             {
             fclose (pfileIn);
             rc = RC_FILECREATE;
             }
        }
   else
        {
        rc = RC_FILEOPEN;
        }
   return rc;
   }


/*************************************************************************************
   DbfFlush: Flush database
*************************************************************************************/
int DbfFlush (PDBF pdbf)
   {
   int rc = RC_OK;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        fflush (pdbf->pfile);
        }
   return rc;
   }


// Records
//============================================================================================================================

/*************************************************************************************
   DbfAppend: Append empty record to database
*************************************************************************************/
int DbfAppend (PDBF pdbf)
   {
   char ch = DATAEND;
   int   rc = RC_OK;
   int   ulRecord = 0;


   if ((rc = CheckPDBF(pdbf)) == RC_OK)
      {
      if ((rc = ClearRecord(pdbf)) == RC_OK)
         {
         if (fseek(pdbf->pfile, -sizeof (ch), SEEK_END) == 0)
            {
            if (fwrite(pdbf->pszCache, pdbf->dbfheader.usLenRecord, 1, pdbf->pfile) != 0)
               {
               if (fwrite(&ch, sizeof (ch), 1, pdbf->pfile) != 0)
                  {
                  pdbf->fChanged = TRUE;
                  pdbf->dbfheader.ulNumRecords++;
                  ulRecord = pdbf->dbfheader.ulNumRecords;
                  DbfGo(pdbf, pdbf->dbfheader.ulNumRecords);
                  rc = pdbf->iStatus;
                  }
               else
                  {
                  rc = RC_FILEWRITE;
                  }
               }
            else
               {
               rc = RC_FILEWRITE;
               }
            }
         else
            {
            rc = RC_FILEPOSITION;
            }
         }
      }
   pdbf->iStatus = rc;
   if (rc == RC_OK)
      return(ulRecord);
   else
      return(0);
   }


/*************************************************************************************
   DbfUpdate: Write the record changes for the given record number to disk
*************************************************************************************/
int DbfUpdate (PDBF pdbf, unsigned long ulRecord)
    {
    int         rc = RC_OK;

    if ((rc = CheckPDBF (pdbf)) == RC_OK)
       {
       if (pdbf->fSkipDeleted == TRUE && WrkIsRecDeleted(pdbf) == TRUE)
          rc = RC_RECORDDELETED;
       else
          rc = WriteRecord (pdbf, ulRecord);
       }
    if (rc == RC_OK)
       return TRUE;
    else
       return FALSE;
    }


/*************************************************************************************
   DbfDelete: Delete the record with a given record number
*************************************************************************************/
int DbfDelete (PDBF pdbf, unsigned long ulRecord)
   {
   int rc = RC_OK;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if (pdbf->fSkipDeleted == TRUE && WrkIsRecDeleted(pdbf) == TRUE)
           rc = RC_RECORDDELETED;
        else
           {
           if ((rc = ReadRecord (pdbf, ulRecord)) == RC_OK)
              {
              memcpy (pdbf->pszCache, pdbf->pszCacheOriginal, pdbf->dbfheader.usLenRecord);
              *pdbf->pszCache = RECORDDELETED;
              rc = WriteRecord (pdbf, ulRecord);
              }
           }
        if (pdbf->fSkipDeleted == TRUE)
           pdbf->fEOF = TRUE;
        else
           pdbf->fEOF = FALSE;
        }
   pdbf->iStatus = rc;
   if (rc == RC_OK)
      return TRUE;
   else
      return FALSE;
   }


/*************************************************************************************
   DbfRecRecall: Undelete a previously deleted record with the given record number
*************************************************************************************/
int DbfRecRecall (PDBF pdbf, unsigned long ulRecord)
   {
   int rc = RC_OK;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   ((rc = ReadRecord (pdbf, ulRecord)) == RC_OK)
             {
             if   (WrkIsRecDeleted (pdbf) == TRUE)
                  {
                  memcpy (pdbf->pszCache, pdbf->pszCacheOriginal, pdbf->dbfheader.usLenRecord);
                  *pdbf->pszCache = ' ';
                  rc = WriteRecord (pdbf, ulRecord);
                  }
             pdbf->fEOF = FALSE;
             }
        }
   return rc;
   }


/*************************************************************************************
   DbfRecNo: Query record number of current record
*************************************************************************************/
int DbfRecNo (PDBF pdbf)
   {
   int   rc = RC_OK;
   int   ul;


   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        pdbf->iStatus = rc;
        return (pdbf->ulCurrentRecord);
        }
   pdbf->iStatus = rc;
   return (0);
   }

/*************************************************************************************
   DbfEOF: Has beginning or end passed ??
*************************************************************************************/
int DbfEOF(PDBF pdbf)
   {
   int   rc = RC_OK;
   int   ul;

   if ((rc = CheckPDBF (pdbf)) == RC_OK)
      {
      if (pdbf->fEOF == TRUE)
         return(TRUE);
      else
         return(FALSE);
      }
   pdbf->iStatus == rc;
   return(TRUE);
   }


/*************************************************************************************
   DbfGo: Go to a given record
*************************************************************************************/
int DbfGo (PDBF pdbf, unsigned long ulRecord)
   {
   int rc = RC_OK;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if (pdbf->fSkipDeleted == TRUE)
           {
           rc = ReadRecord (pdbf, ulRecord);
           if (WrkIsRecDeleted(pdbf) == TRUE)
              rc = RC_RECORDDELETED;
           }
        else
           {
           rc = ReadRecord (pdbf, ulRecord);
           }
        }
   pdbf->iStatus = rc;
   if (rc == RC_OK)
      return (TRUE);
   else
      return (FALSE);
   }


/*************************************************************************************
   DbfGoTop: Go to the first record
*************************************************************************************/
int DbfGoTop (PDBF pdbf)
   {
   int          rc = RC_OK;
   int          iRec = 1;


   pdbf->fEOF = FALSE;
   if ((rc = CheckPDBF (pdbf)) == RC_OK)
      {
      if (pdbf->dbfheader.ulNumRecords == 0)
         {
         rc = pdbf->iStatus;
         pdbf->fEOF = TRUE;
         return (0);
         }

      if (pdbf->fSkipDeleted == TRUE)
         {
         rc = DbfGo(pdbf, iRec);
         while (pdbf->iStatus == RC_RECORDDELETED && iRec <= pdbf->dbfheader.ulNumRecords)
               {
               iRec++;
               rc = DbfGo(pdbf, iRec);
               }
         if (iRec > pdbf->dbfheader.ulNumRecords)
            pdbf->fEOF = TRUE;
         rc = pdbf->iStatus;
         }
      else
         {
         DbfGo(pdbf, iRec);
         rc = pdbf->iStatus;
         }
      }
   pdbf->iStatus = rc;
   if (rc == RC_OK)
      return (iRec);
   else
      return (0);
   }


/*************************************************************************************
   DbfGoBottom: Go to the last record
*************************************************************************************/
int DbfGoBottom (PDBF pdbf)
   {
   int          rc = RC_OK;
   int          iRec;


   pdbf->fEOF = FALSE;
   if ((rc = CheckPDBF (pdbf)) == RC_OK)
      {
      if (pdbf->dbfheader.ulNumRecords == 0)
         {
         rc = pdbf->iStatus;
         pdbf->fEOF = TRUE;
         return (0);
         }

      iRec = pdbf->dbfheader.ulNumRecords;

      if (pdbf->fSkipDeleted == TRUE)
         {
         rc = DbfGo (pdbf, iRec);
         while (pdbf->iStatus == RC_RECORDDELETED && iRec >= 1)
               {
               iRec--;
               rc = DbfGo(pdbf, iRec);
               }
         if (iRec < 1)
            pdbf->fEOF = TRUE;
         rc = pdbf->iStatus;
         }
      else
         {
         DbfGo(pdbf, iRec);
         rc = pdbf->iStatus;
         }
      }
   pdbf->iStatus = rc;
   if (rc == RC_OK)
      return (iRec);
   else
      return (0);
   }


/*************************************************************************************
   DbfSkip: Relatively go to a next record.
*************************************************************************************/
int DbfSkip (PDBF pdbf, long lRecords)
   {
   long         l;
   int          rc = RC_OK;
   int          iDir;


   if ((rc = CheckPDBF (pdbf)) == RC_OK)
      {
      if (pdbf->dbfheader.ulNumRecords == 0)
         {
         rc = pdbf->iStatus;
         pdbf->fEOF = TRUE;
         return (0);
         }

      pdbf->fEOF = FALSE;
      l = pdbf->ulCurrentRecord;
      if (lRecords < 0)
         {
         iDir = '-';
         l = pdbf->ulCurrentRecord - abs(lRecords);
         if (l < 1)
            {
            l = 1;
            pdbf->fEOF = TRUE;
            }
         }
      else
         {
         if (lRecords > 0)
            {
            iDir = '+';
            l = pdbf->ulCurrentRecord + abs(lRecords);
            if (l > pdbf->dbfheader.ulNumRecords)
               {
               l = pdbf->dbfheader.ulNumRecords;
               pdbf->fEOF = TRUE;
               }
            }
         }

      if (pdbf->fSkipDeleted == TRUE)
         {
         DbfGo (pdbf, l);
         rc = pdbf->iStatus;
         if (iDir == '-')
            {
            while (pdbf->iStatus == RC_RECORDDELETED && l >= 1)
                  {
                  l--;
                  DbfGo(pdbf, l);
                  }
            if (l < 1)
               pdbf->fEOF = TRUE;
            rc = pdbf->iStatus;
            }
         else
            {
            while (pdbf->iStatus == RC_RECORDDELETED && l <= pdbf->dbfheader.ulNumRecords)
                  {
                  l++;
                  DbfGo(pdbf, l);
                  }
           if (l > pdbf->dbfheader.ulNumRecords)
              pdbf->fEOF = TRUE;
            rc = pdbf->iStatus;
            }
         }
      else
         {
         DbfGo(pdbf, l);
         rc = pdbf->iStatus;
         }
      }
   pdbf->iStatus = rc;
   if (rc == RC_OK)
      return (l);
   else
      return (0);
   }


// Records - searching
//============================================================================================================================
/*************************************************************************************
   DbfFindChar: Locate a text value in the named field within all records starting with the first record
*************************************************************************************/
int DbfFindChar (PDBF pdbf, char* pszColumn, char* pszCharacter, int iStartAt)
   {
   int    rc = RC_OK;
   unsigned long ulFieldNumber;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   ((rc = FindField (pdbf, pszColumn, &ulFieldNumber)) == RC_OK)
             {
             if (iStartAt > 1)
                iStartAt = 1;
             rc = DbfRecNLocateCharacter (pdbf, ulFieldNumber, pszCharacter, iStartAt);
             }
        }
   return rc;
   }


/*************************************************************************************
   DbfFindDate: Locate a date value in the named field within all records starting with the first record
*************************************************************************************/
int DbfFindDate (PDBF pdbf, char* pszColumn, PTDATE2 ptdate2, int iStartAt)
   {
   int    rc = RC_OK;
   unsigned long ulFieldNumber;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   ((rc = FindField (pdbf, pszColumn, &ulFieldNumber)) == RC_OK)
             {
             if (iStartAt > 1)
                iStartAt = 1;
             rc = DbfRecNLocateDate (pdbf, ulFieldNumber, ptdate2, iStartAt);
             }
        }
   return rc;
   }


/*************************************************************************************
   DbfFindBool: Locate a bool value in the named field within all records starting with the first record
*************************************************************************************/
int DbfFindBool (PDBF pdbf, char* pszColumn, BOOL* pfLogical, int iStartAt)
   {
   int    rc = RC_OK;
   unsigned long ulFieldNumber;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   ((rc = FindField (pdbf, pszColumn, &ulFieldNumber)) == RC_OK)
             {
             if (iStartAt > 1)
                iStartAt = 1;
             rc = DbfRecNLocateLogical (pdbf, ulFieldNumber, pfLogical, iStartAt);
             }
        }
   return rc;
   }


/*************************************************************************************
 DbfFindNum: Locate a numeric value in the named field within all records starting with the first record
*************************************************************************************/
int DbfFindNum (PDBF pdbf, char* pszColumn, double* pdNumeric, int iStartAt)
   {
   int    rc = RC_OK;
   unsigned long ulFieldNumber;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   ((rc = FindField (pdbf, pszColumn, &ulFieldNumber)) == RC_OK)
             {
             if (iStartAt > 1)
                iStartAt = 1;
             rc = DbfRecNLocateNumeric (pdbf, ulFieldNumber, pdNumeric, iStartAt);
             }
        }
   return rc;
   }


// Fields
//============================================================================================================================
/*************************************************************************************
   DbfFldType: Query field type of a named field
*************************************************************************************/
int DbfFldType (PDBF pdbf, char* pszColumn, PCHAR pchFieldType)
   {
   int    rc = RC_OK;
   unsigned long ulFieldNumber;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   ((rc = FindField (pdbf, pszColumn, &ulFieldNumber)) == RC_OK)
             {
             *pchFieldType = (pdbf->pdbffield + ulFieldNumber - 1)->chType;
             }
        }
   return rc;
   }


/*************************************************************************************
   DbfFldLen: Query field length of a named field
*************************************************************************************/
int DbfFldLen (PDBF pdbf, char* pszColumn, unsigned long* pulFieldLength)
   {
   int    rc = RC_OK;
   unsigned long ulFieldNumber;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   ((rc = FindField (pdbf, pszColumn, &ulFieldNumber)) == RC_OK)
             {
             *pulFieldLength = (pdbf->pdbffield + ulFieldNumber - 1)->chLength;
             }
        }
   return rc;
   }


/*************************************************************************************
   DbfGetChar: Get text content from a named field
*************************************************************************************/
char *DbfGetChar (PDBF pdbf, char *pszColumn)
    {
    int                  rc = RC_OK;
    unsigned long        ulFieldNumber;
    char                 pszCharacter[255];


    if ((rc = CheckPDBF (pdbf)) == RC_OK)
       {
       if ((rc = FindField (pdbf, pszColumn, &ulFieldNumber)) == RC_OK)
          {
          rc = DbfFldNGetCharacter (pdbf, ulFieldNumber, pszCharacter);
          }
       }
    pdbf->iStatus = rc;
    if (rc == RC_OK)
       return (pszCharacter);
    else
       return (NULL);
    }


/*************************************************************************************
   DbfGetDate: Get date content from a named field
*************************************************************************************/
int DbfGetDate (PDBF pdbf, char *pszColumn, PTDATE2 ptdate2)
    {
    int                 rc = RC_OK;
    unsigned long       ulFieldNumber;

    if ((rc = CheckPDBF (pdbf)) == RC_OK)
       {
       if ((rc = FindField (pdbf, pszColumn, &ulFieldNumber)) == RC_OK)
          {
          rc = DbfFldNGetDate (pdbf, ulFieldNumber, ptdate2);
          }
       }
    return rc;
    }


/*************************************************************************************
   DbfGetBool: Get bool content from a named field
*************************************************************************************/
int DbfGetBool (PDBF pdbf, char* pszColumn, BOOL *pfLogical)
    {
    int    rc = RC_OK;
    unsigned long ulFieldNumber;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   ((rc = FindField (pdbf, pszColumn, &ulFieldNumber)) == RC_OK)
             {
             rc = DbfFldNGetLogical (pdbf, ulFieldNumber, pfLogical);
             }
        }
   return rc;
   }


/*************************************************************************************
  DbfGetNum: Get numeric content from a named field
*************************************************************************************/
double DbfGetNum (PDBF pdbf, char* pszColumn)
   {
   int            rc = RC_OK;
   unsigned long  ulFieldNumber;
   double         pdNumeric;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   ((rc = FindField (pdbf, pszColumn, &ulFieldNumber)) == RC_OK)
             {
             rc = DbfFldNGetNumeric (pdbf, ulFieldNumber, &pdNumeric);
             }
        }
    pdbf->iStatus = rc;
    if (rc == RC_OK)
       return (pdNumeric);
    else
       return (NULL);
   }


/*************************************************************************************
   DbfPutChar: Put text value into a named field
*************************************************************************************/
int DbfPutChar (PDBF pdbf, char* pszColumn, char* pszCharacter)
   {
   int    rc = RC_OK;
   unsigned long ulFieldNumber;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   ((rc = FindField (pdbf, pszColumn, &ulFieldNumber)) == RC_OK)
             {
             rc = DbfFldNPutCharacter (pdbf, ulFieldNumber, pszCharacter);
             }
        }
   return rc;
   }


/*************************************************************************************
   DbfPutDate: Put date value into a named field
*************************************************************************************/
int DbfPutDate (PDBF pdbf, char* pszColumn, PTDATE2 ptdate2)
   {
   int    rc = RC_OK;
   unsigned long ulFieldNumber;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   ((rc = FindField (pdbf, pszColumn, &ulFieldNumber)) == RC_OK)
             {
             rc = DbfFldNPutDate (pdbf, ulFieldNumber, ptdate2);
             }
        }
   return rc;
   }


/*************************************************************************************
   DbfPutBool: Put bool value into a named field
*************************************************************************************/
int DbfPutBool (PDBF pdbf, char* pszColumn, BOOL* pfLogical)
   {
   int    rc = RC_OK;
   unsigned long ulFieldNumber;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   ((rc = FindField (pdbf, pszColumn, &ulFieldNumber)) == RC_OK)
             {
             rc = DbfFldNPutLogical (pdbf, ulFieldNumber, pfLogical);
             }
        }
   return rc;
   }


/*************************************************************************************
   DbfPutNum: Put numeric value into a named field
*************************************************************************************/
int DbfPutNum (PDBF pdbf, char* pszColumn, double pdNumeric)
   {
   int    rc = RC_OK;
   unsigned long ulFieldNumber;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   ((rc = FindField (pdbf, pszColumn, &ulFieldNumber)) == RC_OK)
             {
             rc = DbfFldNPutNumeric (pdbf, ulFieldNumber, &pdNumeric);
             }
        }
   return rc;
   }


// Memo
//============================================================================================================================

/*************************************************************************************
   DbfMemoExist: Check for a memo entry of a named field
*************************************************************************************/
int DbfMemoExist (PDBF pdbf, char* pszColumn)
   {
   int    rc = RC_OK;
   unsigned long ulFieldNumber;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   ((rc = FindField (pdbf, pszColumn, &ulFieldNumber)) == RC_OK)
             {
             rc = DbfMemNExist (pdbf, ulFieldNumber);
             }
        }
   return rc;
   }


/*************************************************************************************
   DbfMemoLength: Query length of a memo entry of a named field
*************************************************************************************/
int DbfMemoLength (PDBF pdbf, char* pszColumn, unsigned long* pulFieldLength)
   {
   int    rc = RC_OK;
   unsigned long ulFieldNumber;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   ((rc = FindField (pdbf, pszColumn, &ulFieldNumber)) == RC_OK)
             {
             rc = DbfMemNLength (pdbf, ulFieldNumber, pulFieldLength);
             }
        }
   return rc;
   }


/*************************************************************************************
   DbfMemoRead: Read a memo entry of a named field
*************************************************************************************/
int DbfMemoRead (PDBF pdbf, char* pszColumn, char* pszMemo)
   {
   int    rc = RC_OK;
   unsigned long ulFieldNumber;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   ((rc = FindField (pdbf, pszColumn, &ulFieldNumber)) == RC_OK)
             {
             rc = DbfMemNRead (pdbf, ulFieldNumber, pszMemo);
             }
        }
   return rc;
   }


/*************************************************************************************
   DbfMemoWrite: Write a memo entry of a named field
*************************************************************************************/
int DbfMemoWrite (PDBF pdbf, char* pszColumn, char* pszMemo)
   {
   int    rc = RC_OK;
   unsigned long ulFieldNumber;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   ((rc = FindField (pdbf, pszColumn, &ulFieldNumber)) == RC_OK)
             {
             rc = DbfMemNWrite (pdbf, ulFieldNumber, pszMemo);
             }
        }
   return rc;
   }


/*************************************************************************************
   DbfMemoDelete: Delete a memo entry of a named field
*************************************************************************************/
int DbfMemoDelete (PDBF pdbf, char* pszColumn)
   {
   int    rc = RC_OK;
   unsigned long ulFieldNumber;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   ((rc = FindField (pdbf, pszColumn, &ulFieldNumber)) == RC_OK)
             {
             rc = DbfMemNDelete (pdbf, ulFieldNumber);
             }
        }
   return rc;
   }

// STX
//============================================================================================================================

/*************************************************************************************
   DbfStxCreate:
*************************************************************************************/
BOOL DbfStxCreate(char szDBFName, char *szIndexFile, char *szField1, int iStart1, int iLength, ...)
    {
    return (FALSE);
    }

/*************************************************************************************
   DbfStxReIndex:
*************************************************************************************/
BOOL DbfStxReIndex(char *szIndexFile)
    {
    return (FALSE);
    }

/*************************************************************************************
   DbfStxDelete:
*************************************************************************************/
BOOL DbfStxDelete(char *szIndexFile)
    {
    return (FALSE);
    }

/*************************************************************************************
   DbfStxGetFirst:
*************************************************************************************/
int DbfStxGetFirst(PDBF pdbf, int iIndexId, char *szSearchFor)
    {
    return (0);
    }

/*************************************************************************************
   DbfStxGetLast:
*************************************************************************************/
int DbfStxGetLast(PDBF pdbf, int iIndexId, char *szSearchFor)
    {
    return (0);
    }

/*************************************************************************************
   DbfStxGetNext:
*************************************************************************************/
int DbfStxGetNext(PDBF pdbf, int iIndexId)
    {
    return (0);
    }

/*************************************************************************************
   DbfStxGetPrev:
*************************************************************************************/
int DbfStxGetPrev(PDBF pdbf, int iIndexId)
    {
    return (0);
    }

/*************************************************************************************
   DbfStxFlush:
*************************************************************************************/
BOOL DbfStxFlush(PDBF pdbf)
    {
    return (FALSE);
    }



// Worker routines INTERNAL !!!
//============================================================================================================================

/*************************************************************************************
   Delete a memo entry of an indexed field
*************************************************************************************/
/* Delete a memo entry of an indexed field */
int DbfMemNDelete (PDBF pdbf, unsigned long ulFieldNumber)
   {
   char   sz [CCHMAXPATH];
   double dNumeric;
   int     rc = RC_OK;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   (ulFieldNumber && ulFieldNumber <= pdbf->ulNumFields)
             {
             if   ((pdbf->pdbffield + ulFieldNumber - 1)->chType == FIELD_MEMO)
                  {
                  memcpy (sz, (pdbf->pdbffield + ulFieldNumber - 1)->pvAddress, (pdbf->pdbffield + ulFieldNumber - 1)->chLength);
                  sz [(pdbf->pdbffield + ulFieldNumber - 1)->chLength] = 0;
                  dNumeric = 0;
                  sscanf (sz, " %lf", &dNumeric);
                  if   (dNumeric)
                       {
                       memset ((pdbf->pdbffield + ulFieldNumber - 1)->pvAddress, ' ', (pdbf->pdbffield + ulFieldNumber - 1)->chLength);
                       }
                  else
                       {
                       rc = RC_NOMEMO;
                       }
                  }
             else
                  {
                  rc = RC_WRONGFIELDTYPE;
                  }
             }
        else
             {
             rc = RC_WRONGFIELDNUMBER;
             }
        }
   return rc;
   }

/*************************************************************************************
   Check for a memo entry of an indexed field
*************************************************************************************/
/* Check for a memo entry of an indexed field */
int DbfMemNExist (PDBF pdbf, unsigned long ulFieldNumber)
   {
   char   sz [CCHMAXPATH];
   double dNumeric;
   int     rc = RC_OK;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   (ulFieldNumber && ulFieldNumber <= pdbf->ulNumFields)
             {
             if   ((pdbf->pdbffield + ulFieldNumber - 1)->chType == FIELD_MEMO)
                  {
                  memcpy (sz, (pdbf->pdbffield + ulFieldNumber - 1)->pvAddress, (pdbf->pdbffield + ulFieldNumber - 1)->chLength);
                  sz [(pdbf->pdbffield + ulFieldNumber - 1)->chLength] = 0;
                  dNumeric = 0;
                  sscanf (sz, " %lf", &dNumeric);
                  if   (!dNumeric)
                       {
                       rc = RC_NOMEMO;
                       }
                  }
             else
                  {
                  rc = RC_WRONGFIELDTYPE;
                  }
             }
        else
             {
             rc = RC_WRONGFIELDNUMBER;
             }
        }
   return rc;
   }

/*************************************************************************************
   Query length of a memo entry of an indexed field
*************************************************************************************/
/* Query length of a memo entry of an indexed field */
int DbfMemNLength (PDBF pdbf, unsigned long ulFieldNumber, unsigned long* pulFieldLength)
   {
   char   sz [CCHMAXPATH];
   double dNumeric;
   char*    psz;
   int     rc = RC_OK;
   unsigned long  ul;
   unsigned long  ulI;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   (ulFieldNumber && ulFieldNumber <= pdbf->ulNumFields)
             {
             if   ((pdbf->pdbffield + ulFieldNumber - 1)->chType == FIELD_MEMO)
                  {
                  memcpy (sz, (pdbf->pdbffield + ulFieldNumber - 1)->pvAddress, (pdbf->pdbffield + ulFieldNumber - 1)->chLength);
                  sz [(pdbf->pdbffield + ulFieldNumber - 1)->chLength] = 0;
                  dNumeric = 0;
                  sscanf (sz, " %lf", &dNumeric);
                  if   (dNumeric)
                       {
                       ul = dNumeric * pdbf->ulLenMemo;
                       if   (pdbf->fExtendedMemo)
                            {
                            ul += 8;
                            }
                       if   (fseek (pdbf->pfileMemo, ul, SEEK_SET) == 0)
                            {
                            if   ((psz = (char*) calloc (65535, 1)) != 0)
                                 {
                                 if   (fread (psz, 65535, 1, pdbf->pfileMemo) == 0)
                                      {
                                      if   (!feof (pdbf->pfileMemo))
                                           {
                                           rc = RC_FILEREAD;
                                           }
                                      }
                                 if   (rc == RC_OK)
                                      {
                                      *pulFieldLength = 0;
                                      for  (ulI = 0; ulI < 65534; ulI++)
                                           {
                                           if   ((*(psz + ulI) == 0x1A && *(psz + ulI + 1) == 0x1A) || !*(psz + ulI))
                                                {
                                                break;
                                                }
                                           (*pulFieldLength)++;
                                           }
                                      }
                                 free (psz);
                                 }
                            else
                                 {
                                 rc = RC_MEMORYERROR;
                                 }
                            }
                       else
                            {
                            rc = RC_FILEPOSITION;
                            }
                       }
                  else
                       {
                       rc = RC_NOMEMO;
                       }
                  }
             else
                  {
                  rc = RC_WRONGFIELDTYPE;
                  }
             }
        else
             {
             rc = RC_WRONGFIELDNUMBER;
             }
        }
   return rc;
   }

/*************************************************************************************
   Read a memo entry of an indexed field
*************************************************************************************/
/* Read a memo entry of an indexed field */
int DbfMemNRead (PDBF pdbf, unsigned long ulFieldNumber, char* pszMemo)
   {
   char   sz [CCHMAXPATH];
   double dNumeric;
   char*    psz;
   int     rc = RC_OK;
   unsigned long  ul;
   unsigned long  ulI;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   (ulFieldNumber && ulFieldNumber <= pdbf->ulNumFields)
             {
             if   ((pdbf->pdbffield + ulFieldNumber - 1)->chType == FIELD_MEMO)
                  {
                  memcpy (sz, (pdbf->pdbffield + ulFieldNumber - 1)->pvAddress, (pdbf->pdbffield + ulFieldNumber - 1)->chLength);
                  sz [(pdbf->pdbffield + ulFieldNumber - 1)->chLength] = 0;
                  dNumeric = 0;
                  sscanf (sz, " %lf", &dNumeric);
                  if   (dNumeric)
                       {
                       ul = dNumeric * pdbf->ulLenMemo;
                       if   (pdbf->fExtendedMemo)
                            {
                            ul += 8;
                            }
                       if   (fseek (pdbf->pfileMemo, ul, SEEK_SET) == 0)
                            {
                            if   ((psz = (char*)calloc (65535, 1)) != 0)
                                 {
                                 if   (fread (psz, 65535, 1, pdbf->pfileMemo) == 0)
                                      {
                                      if   (!feof (pdbf->pfileMemo))
                                           {
                                           rc = RC_FILEREAD;
                                           }
                                      }
                                 if   (rc == RC_OK)
                                      {
                                      ul = 0;
                                      for  (ulI = 0; ulI < 65534; ulI++)
                                           {
                                           if   ((*(psz + ulI) == 0x1A && *(psz + ulI + 1) == 0x1A) || !*(psz + ulI))
                                                {
                                                break;
                                                }
                                           ul++;
                                           }
                                      memcpy (pszMemo, psz, ul);
                                      *(pszMemo + ul) = 0;
                                      }
                                 free (psz);
                                 }
                            else
                                 {
                                 rc = RC_MEMORYERROR;
                                 }
                            }
                       else
                            {
                            rc = RC_FILEPOSITION;
                            }
                       }
                  else
                       {
                       rc = RC_NOMEMO;
                       }
                  }
             else
                  {
                  rc = RC_WRONGFIELDTYPE;
                  }
             }
        else
             {
             rc = RC_WRONGFIELDNUMBER;
             }
        }
   return rc;
   }

/*************************************************************************************
   Write a memo entry of an indexed field
*************************************************************************************/
/* Write a memo entry of an indexed field */
int DbfMemNWrite (PDBF pdbf, unsigned long ulFieldNumber, char* pszMemo)
   {
   char   ch = 0;
   char   sz [4];
   double dNumeric;
   int     rc = RC_OK;
   unsigned long  ul;
   unsigned long  ul2;
   unsigned long  ul3;
   unsigned long  ulI;
   unsigned long  ulLength;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   (ulFieldNumber && ulFieldNumber <= pdbf->ulNumFields)
             {
             if   ((pdbf->pdbffield + ulFieldNumber - 1)->chType == FIELD_MEMO)
                  {
                  if   (fseek (pdbf->pfileMemo, 0, SEEK_END) == 0)
                       {
                       dNumeric = pdbf->memheader.ulNext;
                       ulLength = strlen (pszMemo);
                       if   (pdbf->fExtendedMemo)
                            {
                            ul3 = ulLength / pdbf->ulLenMemo;
                            if   (ulLength % pdbf->ulLenMemo)
                                 {
                                 ul3++;
                                 }
                            }
                       else
                            {
                            ul = ulLength + 2;
                            ul3 = ul / pdbf->ulLenMemo;
                            if   (ul % pdbf->ulLenMemo)
                                 {
                                 ul3++;
                                 }
                            }
                       pdbf->memheader.ulNext += ul3;
                       ul = ftell (pdbf->pfileMemo);
                       ul2 = ul / pdbf->ulLenMemo;
                       ul2++;
                       if   (ul % pdbf->ulLenMemo)
                            {
                            ul2 *= pdbf->ulLenMemo;
                            ul2 -= ul;
                            for  (ulI = 0; ulI < ul2; ulI++)
                                 {
                                 if   (fwrite (&ch, 1, 1, pdbf->pfileMemo) == 0)
                                      {
                                      rc = RC_FILEWRITE;
                                      break;
                                      }
                                 }
                            }
                       if   (rc == RC_OK)
                            {
                            if   (pdbf->fExtendedMemo)
                                 {
                                 sz [0] = 0xFF;
                                 sz [1] = 0xFF;
                                 sz [2] = 0x08;
                                 sz [3] = 0x00;
                                 if   (fwrite (sz, 4, 1, pdbf->pfileMemo) != 0)
                                      {
                                      if   (fwrite (&ulLength, sizeof (unsigned long), 1, pdbf->pfileMemo) == 0)
                                           {
                                           rc = RC_FILEWRITE;
                                           }
                                      }
                                 else
                                      {
                                      rc = RC_FILEWRITE;
                                      }
                                 }
                            }
                       if   (rc == RC_OK)
                            {
                            if   (fwrite (pszMemo, ulLength, 1, pdbf->pfileMemo) == 0)
                                 {
                                 rc = RC_FILEWRITE;
                                 }
                            }
                       if   (rc == RC_OK)
                            {
                            if   (!pdbf->fExtendedMemo)
                                 {
                                 sz [0] = 0x1A;
                                 sz [1] = 0x1A;
                                 if   (fwrite (sz, 2, 1, pdbf->pfileMemo) == 0)
                                      {
                                      rc = RC_FILEWRITE;
                                      }
                                 }
                            }
                       if   (rc == RC_OK)
                            {
                            if   (fseek (pdbf->pfileMemo, 0, SEEK_SET) == 0)
                                 {
                                 if   (fwrite (&pdbf->memheader, sizeof (MEMHEADER), 1, pdbf->pfileMemo) != 0)
                                      {
                                      rc = DbfFldNPutNumeric (pdbf, ulFieldNumber, &dNumeric);
                                      }
                                 else
                                      {
                                      rc = RC_FILEWRITE;
                                      }
                                 }
                            else
                                 {
                                 rc = RC_FILEPOSITION;
                                 }
                            }
                       }
                  else
                       {
                       rc = RC_FILEPOSITION;
                       }
                  }
             else
                  {
                  rc = RC_WRONGFIELDTYPE;
                  }
             }
        else
             {
             rc = RC_WRONGFIELDNUMBER;
             }
        }
   return rc;
   }


/*************************************************************************************
   DbfRecNLocateCharacter: Locate a text value in an indexed field within all records starting with the first record
*************************************************************************************/
/* Locate a text value in an indexed field within all records starting with the first record */
int DbfRecNLocateCharacter (PDBF pdbf, unsigned long ulFieldNumber, char* pszCharacter, int iStartAt)
   {
   char                 sz [261];
   int                  rc;
   unsigned long        ulCount;
   unsigned long        ulI;


   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        rc = RC_NOTFOUND;
        if   (ulFieldNumber && ulFieldNumber <= pdbf->ulNumFields)
             {
             if   ((pdbf->pdbffield + ulFieldNumber - 1)->chType == FIELD_CHARACTER)
                  {
                  if   ((ulCount = DbfRecCount (pdbf)) > 0)
                       {
                       for  (ulI = iStartAt; ulI <= ulCount; ulI++)
                            {
                            DbfGo (pdbf, ulI);
                            if (pdbf->iStatus == RC_OK)
                               {
                               if (WrkIsRecDeleted(pdbf) == FALSE)
                                  {
                                  memcpy (sz, (pdbf->pdbffield + ulFieldNumber - 1)->pvAddress, (pdbf->pdbffield + ulFieldNumber - 1)->chLength);
                                  DbfUtlCut (sz, (pdbf->pdbffield + ulFieldNumber - 1)->chLength);
                                  if (strcmp (sz, pszCharacter) == 0)
                                     {
                                     rc = RC_OK;
                                     break;
                                     }
                                  }
                               }
                            else
                               {
                               break;
                               }
                            }
                       }
                  }
             else
                  {
                  rc = RC_WRONGFIELDTYPE;
                  }
             }
        else
             {
             rc = RC_WRONGFIELDNUMBER;
             }
        }
   return rc;
   }

/*************************************************************************************
   Locate a date value in an indexed field within all records starting with the first record
*************************************************************************************/
/* Locate a date value in an indexed field within all records starting with the first record */
int DbfRecNLocateDate (PDBF pdbf, unsigned long ulFieldNumber, PTDATE2 ptdate2, int iStartAt)
   {
   char   sz [CCHMAXPATH];
   TDATE2 tdate2;
   int     rc;
   unsigned long  ulCount;
   unsigned long  ulI;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        rc = RC_NOTFOUND;
        if   (ulFieldNumber && ulFieldNumber <= pdbf->ulNumFields)
             {
             if   ((pdbf->pdbffield + ulFieldNumber - 1)->chType == FIELD_DATE)
                  {
                  if   ((ulCount = DbfRecCount (pdbf)) > 0)
                       {
                       for  (ulI = iStartAt; ulI <= ulCount; ulI++)
                            {
                            DbfGo (pdbf, ulI);
                            if   (pdbf->iStatus == RC_OK)
                                 {
                                 if   (WrkIsRecDeleted (pdbf) == FALSE)
                                      {
                                      memcpy (sz, (pdbf->pdbffield + ulFieldNumber - 1)->pvAddress, 8);
                                      sz [8] = 0;
                                      sscanf (sz, " %4u%2u%2u", &tdate2.ulYear, &tdate2.ulMonth, &tdate2.ulDay);
                                      if   ((tdate2.ulDay == ptdate2->ulDay) &&
                                            (tdate2.ulMonth == ptdate2->ulMonth) &&
                                            (tdate2.ulYear == ptdate2->ulYear))
                                           {
                                           rc = RC_OK;
                                           break;
                                           }
                                      }
                                 }
                            else
                                 {
                                 break;
                                 }
                            }
                       }
                  }
             else
                  {
                  rc = RC_WRONGFIELDTYPE;
                  }
             }
        else
             {
             rc = RC_WRONGFIELDNUMBER;
             }
        }
   return rc;
   }

/*************************************************************************************
   Locate a bool value in an indexed field within all records starting with the first record
*************************************************************************************/
/* Locate a bool value in an indexed field within all records starting with the first record */
int DbfRecNLocateLogical (PDBF pdbf, unsigned long ulFieldNumber, BOOL* pfLogical, int iStartAt)
   {
   BOOL  fLogical;
   char  ch;
   int    rc;
   unsigned long ulCount;
   unsigned long ulI;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        rc = RC_NOTFOUND;
        if   (ulFieldNumber && ulFieldNumber <= pdbf->ulNumFields)
             {
             if   ((pdbf->pdbffield + ulFieldNumber - 1)->chType == FIELD_LOGICAL)
                  {
                  if   ((ulCount = DbfRecCount (pdbf)) > 0)
                       {
                       for  (ulI = iStartAt; ulI <= ulCount; ulI++)
                            {
                            DbfGo(pdbf, ulI);
                            if   (pdbf->iStatus == RC_OK)
                                 {
                                 if   (WrkIsRecDeleted (pdbf) == FALSE)
                                      {
                                      ch = *((char*) (pdbf->pdbffield + ulFieldNumber - 1)->pvAddress);
                                      fLogical = (ch == LOGICALFALSE || ch == LOGICALFALSE2 || ch == LOGICALFALSE3 || ch == LOGICALFALSE4 || ch == ' ') ? FALSE : TRUE;
                                      if   (fLogical == *pfLogical)
                                           {
                                           rc = RC_OK;
                                           break;
                                           }
                                      }
                                 }
                            else
                                 {
                                 break;
                                 }
                            }
                       }
                  }
             else
                  {
                  rc = RC_WRONGFIELDTYPE;
                  }
             }
        else
             {
             rc = RC_WRONGFIELDNUMBER;
             }
        }
   return rc;
   }

/*************************************************************************************
   Locate a numeric value in an indexed field within all records starting with the first record
*************************************************************************************/
/* Locate a numeric value in an indexed field within all records starting with the first record */
int DbfRecNLocateNumeric (PDBF pdbf, unsigned long ulFieldNumber, double* pdNumeric, int iStartAt)
   {
   char   sz [CCHMAXPATH];
   double dNumeric;
   int     rc = RC_NOTFOUND;
   unsigned long  ulCount;
   unsigned long  ulI;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        rc = RC_NOTFOUND;
        if   (ulFieldNumber && ulFieldNumber <= pdbf->ulNumFields)
             {
             if   ((pdbf->pdbffield + ulFieldNumber - 1)->chType == FIELD_LOGICAL)
                  {
                  if   ((ulCount = DbfRecCount (pdbf)) > 0)
                       {
                       for  (ulI = iStartAt; ulI <= ulCount; ulI++)
                            {
                            DbfGo (pdbf, ulI);
                            if   (pdbf->iStatus == RC_OK)
                                 {
                                 if   (WrkIsRecDeleted (pdbf) == FALSE)
                                      {
                                      memcpy (sz, (pdbf->pdbffield + ulFieldNumber - 1)->pvAddress, (pdbf->pdbffield + ulFieldNumber - 1)->chLength);
                                      sz [(pdbf->pdbffield + ulFieldNumber - 1)->chLength] = 0;
                                      dNumeric = 0;
                                      sscanf (sz, " %lf", &dNumeric);
                                      if   (dNumeric == *pdNumeric)
                                           {
                                           rc = RC_OK;
                                           break;
                                           }
                                      }
                                 }
                            else
                                 {
                                 break;
                                 }
                            }
                       }
                  }
             else
                  {
                  rc = RC_WRONGFIELDTYPE;
                  }
             }
        else
             {
             rc = RC_WRONGFIELDNUMBER;
             }
        }
   return rc;
   }


/*************************************************************************************
   DbfUtlCut: Cut trailing spaces from a text value
*************************************************************************************/
int DbfUtlCut (char* psz, unsigned long ulFieldLength)
    {
    int                  rc = RC_OK;
    unsigned long        ulI;


    ulI = ulFieldLength - 1;
    if (ulI >= 0)
       {
       while (psz [ulI] == ' ' && ulI >= 0)
             {
             psz [ulI--] = 0;
             }
       }
    return rc;
    }


/*************************************************************************************
   DbfUtlSoundex: Compare two text values with the soundex algorithm
*************************************************************************************/
int DbfUtlSoundex (char* psz1, char* psz2)
   {
   char ch1;
   char ch2;
   char szValue [] = "01230120022455012623010202";
   char*  psz;
   char*  pszNew1;
   char*  pszNew1Save;
   char*  pszNew2;
   char*  pszNew2Save;
   int   rc = RC_STRINGSOUNDEXNOTOK;

   if   (isascii (*psz1) &&
         isascii (*psz2))
        {
        pszNew1 = (char*)calloc (2,
                          strlen (psz1));
        pszNew1Save = pszNew1;
        strupr (pszNew1);
        pszNew2 = (char*)calloc (2,
                          strlen (psz2));
        pszNew2Save = pszNew2;
        strupr (pszNew2);
        ch1 = *pszNew1++;
        psz = pszNew1;
        while ( (ch2 = *pszNew1++) )
           {
           if   (isascii (ch2))
                {
                if   (szValue [ch2 - 'A'] != szValue [ch1 - 'A'])
                     {
                     if   (szValue [ch2 - 'A'] != '0')
                          {
                          ch1 = ch2;
                          *psz = szValue [ch2 - 'A'];
                          psz++;
                          }
                     }
                }
           }
        *psz = 0;
        ch1 = *pszNew2++;
        psz = pszNew2;
        while ((ch2 = *pszNew2++))
           {
           if   (isascii (ch2))
                {
                if   (szValue [ch2 - 'A'] != szValue [ch1 - 'A'])
                     {
                     if   (szValue [ch2 - 'A'] != '0')
                          {
                          ch1 = ch2;
                          *psz = szValue [ch2 - 'A'];
                          psz++;
                          }
                     }
                }
           }
        *psz = 0;
        if   (strcmp (pszNew1Save,
                      pszNew2Save) == 0)
             {
             rc = RC_STRINGSOUNDEXOK;
             }
        free (pszNew1Save);
        free (pszNew2Save);
        }
   return rc;
   }

   // Stx internal routines
/*************************************************************************************
   StxOpen:
*************************************************************************************/
BOOL StxOpen(PDBF pdbf, char *szIndexFile, int iIndexId)
    {
    return (FALSE);
    }

/*************************************************************************************
   StxIndex:
*************************************************************************************/
BOOL StxIndex(PDBF pdbf, int iIndexId)
    {
    return (FALSE);
    }

/*************************************************************************************
   StxAppend:
*************************************************************************************/
int StxAppend(PDBF pdbf, int iIndexId)
    {
    return (0);
    }

/*************************************************************************************
   StxUpdate:
*************************************************************************************/
int StxUpdate(PDBF pdbf, int iIndexId)
    {
    return (0);
    }

/*************************************************************************************
   StxDelete:
*************************************************************************************/
int StxDelete(PDBF pdbf, int iIndexId)
    {
    return (0);
    }

/*************************************************************************************
   StxClose:
*************************************************************************************/
BOOL StxClose(PDBF pdbf, int iIndexId)
    {
    return (FALSE);
    }


/*************************************************************************************
   FindField: Query the index of a field with the help of it's name
*************************************************************************************/
int FindField (PDBF pdbf, char* pszColumn, unsigned long* pulFieldNumber)
    {
    BOOL                f = FALSE;
    PDBFFIELD           pdbffield;
    int                 rc = RC_OK;
    unsigned long       ulI;


    if (pszColumn)
       {
       for (ulI = 0, pdbffield = pdbf->pdbffield; ulI < pdbf->ulNumFields; ulI++, pdbffield++)
           {
           if (stricmp (pdbffield->szName, pszColumn) == 0)
              {
              f = TRUE;
              *pulFieldNumber = ulI + 1;
              break;
              }
           }
       }
    if (!f)
       {
       rc = RC_WRONGFIELD;
       }
    return rc;
    }


/*************************************************************************************
   ReadRecord: Read a given record
*************************************************************************************/
int ReadRecord (PDBF pdbf, unsigned long ulRecord)
    {
    int         rc = RC_OK;


    if (ulRecord > 0 && ulRecord <= pdbf->dbfheader.ulNumRecords)
       {
       if (fseek (pdbf->pfile, pdbf->dbfheader.usLenHeader + ((ulRecord - 1) * pdbf->dbfheader.usLenRecord), SEEK_SET) == 0)
          {
          if (fread (pdbf->pszCache, pdbf->dbfheader.usLenRecord, 1, pdbf->pfile) != 0)
             {
             pdbf->ulCurrentRecord = ulRecord;
             memcpy (pdbf->pszCacheOriginal, pdbf->pszCache, pdbf->dbfheader.usLenRecord);
             }
          else
             {
             rc = RC_FILEREAD;
             }
          }
       else
          {
          rc = RC_FILEPOSITION;
          }
       }
    else
       {
       rc = RC_WRONGRECORD;
       }
    return rc;
    }


/*************************************************************************************
   WriteRecord: Write a given record
*************************************************************************************/
int WriteRecord (PDBF pdbf, unsigned long ulRecord)
    {
    int          rc = RC_OK;


    if (ulRecord > 0 && ulRecord <= pdbf->dbfheader.ulNumRecords)
       {
       if (fseek (pdbf->pfile, pdbf->dbfheader.usLenHeader + ((ulRecord - 1) * pdbf->dbfheader.usLenRecord), SEEK_SET) == 0)
          {
          if (fwrite (pdbf->pszCache, pdbf->dbfheader.usLenRecord, 1, pdbf->pfile) != 0)
             {
             pdbf->fChanged = TRUE;
             pdbf->ulCurrentRecord = ulRecord;
             memcpy (pdbf->pszCacheOriginal, pdbf->pszCache, pdbf->dbfheader.usLenRecord);
             }
          else
             {
             rc = RC_FILEWRITE;
             }
          }
       else
          {
          rc = RC_FILEPOSITION;
          }
       }
    else
       {
       rc = RC_WRONGRECORD;
       }
    return rc;
    }

/*************************************************************************************
   CheckMemo: Query memo file information
*************************************************************************************/
int CheckMemo(PDBF pdbf)
    {
    BYTE        ab[4];
    int         rc = RC_OK;


    if (fseek (pdbf->pfileMemo, 0, SEEK_SET) == 0)
       {
       if (fread (&pdbf->memheader, sizeof (MEMHEADER), 1, pdbf->pfileMemo) != 0)
          {
          pdbf->ulLenMemo = 512;
          if (pdbf->memheader.ulNext > 1)
             {
             if (fseek (pdbf->pfileMemo, pdbf->ulLenMemo, SEEK_SET) == 0)
                {
                if (fread (ab, sizeof (ab), 1, pdbf->pfileMemo) != 0)
                   {
                   if ((ab [0] == 0xFF) && (ab [1] == 0xFF) && (ab [2] == 0x08) && (ab [3] == 0x00))
                      {
                      pdbf->fExtendedMemo = TRUE;
                      pdbf->ulLenMemo = pdbf->memheader.memheader4.usLenMemo;
                      }
                   }
                else
                   {
                   rc = RC_FILEREAD;
                   }
                }
             else
                {
                rc = RC_FILEPOSITION;
                }
             }
          }
       else
          {
          rc = RC_FILEREAD;
          }
       }
    else
       {
       rc = RC_FILEPOSITION;
       }
    return rc;
    }


/*************************************************************************************
   CheckPDBF: Check the pdbf parameter
*************************************************************************************/
int CheckPDBF(PDBF pdbf)
    {
    int         rc = RC_OK;


    if (pdbf)
       {
       if (strcmp (pdbf->szFootprint, FOOTPRINT) == 0)
          {
          if (!pdbf->pfile)
             {
             rc = RC_INVALIDFILEHANDLE;
             }
          }
       else
          {
          rc = RC_INVALIDDBF;
          }
       }
    else
       {
       rc = RC_MISSINGPDBF;
       }
    return rc;
    }


/*************************************************************************************
   ClearRecord: Empty the record buffer
*************************************************************************************/
int ClearRecord (PDBF pdbf)
    {
    int         rc = RC_OK;


    memset (pdbf->pszCache, ' ', pdbf->dbfheader.usLenRecord);
    return rc;
    }
/*************************************************************************************
   DbfRecCount: Query count of record (includes deleted records)
*************************************************************************************/

int DbfRecCount (PDBF pdbf)
   {
   int iRecCount = 0;
   int rc = RC_OK;


   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        iRecCount = pdbf->dbfheader.ulNumRecords;
        }
   pdbf->iStatus = rc;
   return(iRecCount);
   }


/*************************************************************************************
   WrkIsRecDeleted: Is the current record deleted?
*************************************************************************************/
int WrkIsRecDeleted (PDBF pdbf)
   {
   int rc = RC_OK;

   if ((rc = CheckPDBF (pdbf)) == RC_OK)
      {
      pdbf->iStatus = rc;
      if (*pdbf->pszCache == RECORDDELETED)
         return (TRUE);
      else
         return (FALSE);
      }
   pdbf->iStatus = rc;
   return (TRUE);
   }


/*************************************************************************************
   DbfRecRefresh: Undo changes to the current record, use original values
*************************************************************************************/
int DbfRecRefresh (PDBF pdbf)
   {
   int rc = RC_OK;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        memcpy (pdbf->pszCache, pdbf->pszCacheOriginal, pdbf->dbfheader.usLenRecord);
        }
   return rc;
   }

/*************************************************************************************
   DbfFldCount: Query Number of Fields in Database
*************************************************************************************/
int DbfFldCount (PDBF pdbf, unsigned long *pul)
    {
    int         rc = RC_OK;


    if ((rc = CheckPDBF (pdbf)) == RC_OK)
       {
       *pul = pdbf->ulNumFields;
       }
    return rc;
    }


/*************************************************************************************
   DbfFldDecimal: Query Number of decimal digits for a named field
*************************************************************************************/
int DbfFldDecimal (PDBF pdbf, char *pszColumn, unsigned long *pulFieldDecimal)
    {
    int                  rc = RC_OK;
    unsigned long        ulFieldNumber;


    if ((rc = CheckPDBF (pdbf)) == RC_OK)
       {
       if ((rc = FindField (pdbf, pszColumn, &ulFieldNumber)) == RC_OK)
          {
          *pulFieldDecimal = (pdbf->pdbffield + ulFieldNumber - 1)->chDecimal;
          }
       }
    return rc;
    }



/*************************************************************************************
   Query field name of an indexed field
*************************************************************************************/
int DbfFldName (PDBF pdbf, unsigned long ulFieldNumber, char* pszColumn)
   {
   int rc = RC_OK;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   (ulFieldNumber && ulFieldNumber <= pdbf->ulNumFields)
             {
             strcpy (pszColumn, (pdbf->pdbffield + ulFieldNumber - 1)->szName);
             }
        else
             {
             rc = RC_WRONGFIELDNUMBER;
             }
        }
   return rc;
   }

/*************************************************************************************
   Get text content from an indexed field
*************************************************************************************/
int DbfFldNGetCharacter (PDBF pdbf, unsigned long ulFieldNumber, char* pszCharacter)
   {
   int rc = RC_OK;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   (ulFieldNumber && ulFieldNumber <= pdbf->ulNumFields)
             {
             if   ((pdbf->pdbffield + ulFieldNumber - 1)->chType == FIELD_CHARACTER)
                  {
                  memcpy (pszCharacter, (pdbf->pdbffield + ulFieldNumber - 1)->pvAddress, (pdbf->pdbffield + ulFieldNumber - 1)->chLength);
                  DbfUtlCut (pszCharacter, (pdbf->pdbffield + ulFieldNumber - 1)->chLength);
                  }
             else
                  {
                  rc = RC_WRONGFIELDTYPE;
                  }
             }
        else
             {
             rc = RC_WRONGFIELDNUMBER;
             }
        }
   return rc;
   }

/*************************************************************************************
   Get date content from an indexed field
*************************************************************************************/
int DbfFldNGetDate (PDBF pdbf, unsigned long ulFieldNumber, PTDATE2 ptdate2)
   {
   char sz [CCHMAXPATH];
   int   rc = RC_OK;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   (ulFieldNumber && ulFieldNumber <= pdbf->ulNumFields)
             {
             if   ((pdbf->pdbffield + ulFieldNumber - 1)->chType == FIELD_DATE)
                  {
                  memcpy (sz, (pdbf->pdbffield + ulFieldNumber - 1)->pvAddress, 8);
                  sz [8] = 0;
                  ptdate2->ulDay = 1;
                  ptdate2->ulMonth = 1;
                  ptdate2->ulYear = 1900;
                  sscanf (sz, " %4u%2u%2u", &ptdate2->ulYear, &ptdate2->ulMonth, &ptdate2->ulDay);
                  }
             else
                  {
                  rc = RC_WRONGFIELDTYPE;
                  }
             }
        else
             {
             rc = RC_WRONGFIELDNUMBER;
             }
        }
   return rc;
   }

/*************************************************************************************
   Get bool content from an indexed field
*************************************************************************************/
int DbfFldNGetLogical (PDBF pdbf, unsigned long ulFieldNumber, BOOL* pfLogical)
   {
   char ch;
   int   rc = RC_OK;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   (ulFieldNumber && ulFieldNumber <= pdbf->ulNumFields)
             {
             if   ((pdbf->pdbffield + ulFieldNumber - 1)->chType == FIELD_LOGICAL)
                  {
                  ch = *((char*) (pdbf->pdbffield + ulFieldNumber - 1)->pvAddress);
                  *pfLogical = (ch == LOGICALFALSE || ch == LOGICALFALSE2 || ch == LOGICALFALSE3 || ch == LOGICALFALSE4 || ch == ' ') ? FALSE : TRUE;
                  }
             else
                  {
                  rc = RC_WRONGFIELDTYPE;
                  }
             }
        else
             {
             rc = RC_WRONGFIELDNUMBER;
             }
        }
   return rc;
   }

/*************************************************************************************
   Get numeric content from an indexed field
*************************************************************************************/
int DbfFldNGetNumeric (PDBF pdbf, unsigned long ulFieldNumber, double* pdNumeric)
   {
   char sz [CCHMAXPATH];
   int   rc = RC_OK;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   (ulFieldNumber && ulFieldNumber <= pdbf->ulNumFields)
             {
             if   ((pdbf->pdbffield + ulFieldNumber - 1)->chType == FIELD_NUMERIC)
                  {
                  memcpy (sz, (pdbf->pdbffield + ulFieldNumber - 1)->pvAddress, (pdbf->pdbffield + ulFieldNumber - 1)->chLength);
                  sz [(pdbf->pdbffield + ulFieldNumber - 1)->chLength] = 0;
                  *pdNumeric = 0;
                  sscanf (sz, " %lf", pdNumeric);
                  }
             else
                  {
                  rc = RC_WRONGFIELDTYPE;
                  }
             }
        else
             {
             rc = RC_WRONGFIELDNUMBER;
             }
        }
   return rc;
   }

/*************************************************************************************
   Put text value into an indexed field
*************************************************************************************/
/* Put text value into an indexed field */
int DbfFldNPutCharacter (PDBF pdbf, unsigned long ulFieldNumber, char* pszCharacter)
   {
   int    rc = RC_OK;
   unsigned long ul;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   (ulFieldNumber && ulFieldNumber <= pdbf->ulNumFields)
             {
             if   ((pdbf->pdbffield + ulFieldNumber - 1)->chType == FIELD_CHARACTER)
                  {
                  ul = strlen (pszCharacter);
                  if   (ul <= (pdbf->pdbffield + ulFieldNumber - 1)->chLength)
                       {
                       memset((pdbf->pdbffield + ulFieldNumber - 1)->pvAddress, ' ', (pdbf->pdbffield + ulFieldNumber - 1)->chLength);
                       memcpy ((pdbf->pdbffield + ulFieldNumber - 1)->pvAddress, pszCharacter, ul);
                       }
                  else
                       {
                       rc = RC_WRONGFIELDLENGTH;
                       }
                  }
             else
                  {
                  rc = RC_WRONGFIELDTYPE;
                  }
             }
        else
             {
             rc = RC_WRONGFIELDNUMBER;
             }
        }
   return rc;
   }

/*************************************************************************************
   Put date value into an indexed field
*************************************************************************************/
int DbfFldNPutDate (PDBF pdbf, unsigned long ulFieldNumber, PTDATE2 ptdate2)
   {
   char szValue [261];
   int   rc = RC_OK;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   (ulFieldNumber && ulFieldNumber <= pdbf->ulNumFields)
             {
             if   ((pdbf->pdbffield + ulFieldNumber - 1)->chType == FIELD_DATE)
                  {
                  sprintf (szValue, "%04u%02u%02u", ptdate2->ulYear, ptdate2->ulMonth, ptdate2->ulDay);
                  memcpy ((pdbf->pdbffield + ulFieldNumber - 1)->pvAddress, szValue, 8);
                  }
             else
                  {
                  rc = RC_WRONGFIELDTYPE;
                  }
             }
        else
             {
             rc = RC_WRONGFIELDNUMBER;
             }
        }
   return rc;
   }

/*************************************************************************************
   Put bool value into an indexed field
*************************************************************************************/
int DbfFldNPutLogical (PDBF pdbf, unsigned long ulFieldNumber, BOOL* pfLogical)
   {
   int rc = RC_OK;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   (ulFieldNumber && ulFieldNumber <= pdbf->ulNumFields)
             {
             if   ((pdbf->pdbffield + ulFieldNumber - 1)->chType == FIELD_LOGICAL)
                  {
                  *((char*) (pdbf->pdbffield + ulFieldNumber - 1)->pvAddress) = (*pfLogical) ? LOGICALTRUE4 : LOGICALFALSE4;
                  }
             else
                  {
                  rc = RC_WRONGFIELDTYPE;
                  }
             }
        else
             {
             rc = RC_WRONGFIELDNUMBER;
             }
        }
   return rc;
   }

/*************************************************************************************
   Put numeric value into an indexed field
*************************************************************************************/
int DbfFldNPutNumeric (PDBF pdbf, unsigned long ulFieldNumber, double* pdNumeric)
   {
   char szFormat [32];
   char szValue [261];
   int   rc = RC_OK;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        if   (ulFieldNumber && ulFieldNumber <= pdbf->ulNumFields)
             {
             if   ((pdbf->pdbffield + ulFieldNumber - 1)->chType == FIELD_NUMERIC || (pdbf->pdbffield + ulFieldNumber - 1)->chType == FIELD_MEMO)
                  {
                  if   ((pdbf->pdbffield + ulFieldNumber - 1)->chDecimal)
                       {
                       sprintf (szFormat, "%% %u.%uf", (pdbf->pdbffield + ulFieldNumber - 1)->chLength, (pdbf->pdbffield + ulFieldNumber - 1)->chDecimal);
                       sprintf (szValue, szFormat, *pdNumeric);
                       }
                  else
                       {
                       sprintf (szFormat, "%% %uu", (pdbf->pdbffield + ulFieldNumber - 1)->chLength);
                       sprintf (szValue, szFormat, (unsigned long) *pdNumeric);
                       }
                  memcpy ((pdbf->pdbffield + ulFieldNumber - 1)->pvAddress, szValue, strlen (szValue));
                  }
             else
                  {
                  rc = RC_WRONGFIELDTYPE;
                  }
             }
        else
             {
             rc = RC_WRONGFIELDNUMBER;
             }
        }
   return rc;
   }


/*************************************************************************************
   Query field index of a named field
*************************************************************************************/
int DbfFldNumber (PDBF pdbf, char* pszColumn, unsigned long* pulFieldNumber)
   {
   int rc = RC_OK;

   if   ((rc = CheckPDBF (pdbf)) == RC_OK)
        {
        rc = FindField (pdbf, pszColumn, pulFieldNumber);
        }
   return rc;
   }


/*************************************************************************************
   WrkGo: Go to a given record
*************************************************************************************/
int WrkGo (PDBF pdbf, unsigned long ulRecord)
   {
   int rc = RC_OK;


   if ((rc = CheckPDBF (pdbf)) == RC_OK)
      rc = ReadRecord (pdbf, ulRecord);

   pdbf->iStatus = rc;
   if (rc == RC_OK)
      return (TRUE);
   else
      return (FALSE);
   }

