/**************************************************************
                                                        990128
   Include File: DBF.H
   Description:  Helper functions for dBASE access.
**************************************************************/

#ifndef _DBASE_H
   #define _DBASE_H

   #define FILE_DB3     0x03
   #define FILE_DB3MEMO 0x83
   #define FILE_DB4MEMO 0x8B
   #define FILE_FPMEMO  0xF5

   #define FIELD_CHARACTER 'C'
   #define FIELD_NUMERIC   'N'
   #define FIELD_LOGICAL   'L'
   #define FIELD_DATE      'D'
   #define FIELD_MEMO      'M'

   #define LOGICALFALSE  'f'
   #define LOGICALFALSE2 'F'
   #define LOGICALFALSE3 'n'
   #define LOGICALFALSE4 'N'
   #define LOGICALTRUE   't'
   #define LOGICALTRUE2  'T'
   #define LOGICALTRUE3  'y'
   #define LOGICALTRUE4  'Y'

   #define FOOTPRINT "DBASE"

   #define HEADEREND 0x0D
   #define DATAEND   0x1A

   #define RECORDDELETED '*'

   typedef enum
      {
      RC_OK = 0,
      RC_FILECOPY,
      RC_FILECREATE,
      RC_FILEOPEN,
      RC_FILEPOSITION,
      RC_FILEREAD,
      RC_FILEWRITE,
      RC_INVALIDDBF,
      RC_INVALIDFILE,
      RC_INVALIDFILEHANDLE,
      RC_MEMORYERROR,
      RC_MISSINGPDBF,
      RC_MISSINGFIELD,
      RC_NOMEMO,
      RC_NOTFOUND,
      RC_RECORDDELETED,
      RC_RECORDNOTDELETED,
      RC_STRINGSOUNDEXNOTOK,
      RC_STRINGSOUNDEXOK,
      RC_WRONGFIELD,
      RC_WRONGFIELDLENGTH,
      RC_WRONGFIELDNUMBER,
      RC_WRONGFIELDTYPE,
      RC_WRONGRECORD
      };

   #pragma pack(1)
   typedef struct
      {
      char szName [11] ;
      char chType ;
      char chLength ;
      char chDecimal ;
      } DBFCREATE, *PDBFCREATE ;

   typedef struct
      {
      char              szName [11] ;
      char              chType ;
      void*             pvAddress ;
      char              chLength ;
      char              chDecimal ;
      unsigned short    usDummy1 ;
      char              chArea ;
      char              achDummy2 [11] ;
      } DBFFIELD, *PDBFFIELD ;

   typedef struct
      {
      char achDummy [20] ;
      } DBFHEADER3, *PDBFHEADER3 ;

   typedef struct
      {
      char chDummy1 [2] ;
      char chPendingTransaction ;
      char chCryption ;
      char achDummy [12] ;
      char chMDX ;
      char achDummy2 [3] ;
      } DBFHEADER4, *PDBFHEADER4 ;

   typedef struct
      {
      char chYear ;
      char chMonth ;
      char chDay ;
      } TDATE, *PTDATE ;

   typedef struct
      {
      unsigned long ulYear ;
      unsigned long ulMonth ;
      unsigned long ulDay ;
      } TDATE2, *PTDATE2 ;

   typedef struct
      {
      char              chVersion ;
      TDATE             tdate ;
      unsigned long     ulNumRecords ;
      unsigned short    usLenHeader ;
      unsigned short    usLenRecord ;
      union
         {
         DBFHEADER3 dbfheader3 ;
         DBFHEADER4 dbfheader4 ;
         } ;
      } DBFHEADER, *PDBFHEADER ;

   typedef struct
      {
      char achDummy [496] ;
      } MEMHEADER3, *PMEMHEADER3 ;

   typedef struct
      {
      char              chVersion ;
      char              achDummy [3] ;
      unsigned short    usLenMemo ;
      char              achDummy2 [490] ;
      } MEMHEADER4, *PMEMHEADER4 ;

   typedef struct
      {
      unsigned long     ulNext ;
      unsigned long     ulDummy ;
      char              szFilename [8] ;
      union
         {
         MEMHEADER3 memheader3 ;
         MEMHEADER4 memheader4 ;
         } ;
      } MEMHEADER, *PMEMHEADER ;

   typedef struct
      {
      BOOL              fChanged;
      BOOL              fExtendedMemo;
      BOOL              fSkipDeleted;
      BOOL              fEOF;
      int               iStatus;
      char              chType;
      char              szFootprint [8];
      char              szFilename [_MAX_PATH];
      char              szMemoFilename [_MAX_PATH];
      DBFHEADER         dbfheader;
      FILE*             pfile;
      FILE*             pfileMemo;
      MEMHEADER         memheader;
      PDBFFIELD         pdbffield;
      char*             pszCache;
      char*             pszCacheOriginal;
      unsigned long     ulCurrentRecord;
      unsigned long     ulLenMemo;
      unsigned long     ulNumFields;
      } DBF, *PDBF ;
   #pragma pack()

   // File management
   int DbfCreate (PDBF*, char*, unsigned long, PDBFCREATE);
   PDBFCREATE DbfStructureInit(int iFields);
   int DbfStructureAdd(PDBFCREATE pdbfcreate, int iFieldId, char *szName, char chType, char chLength, char chDecimal);
   int DbfOpen (PDBF* ppdbf, char* pszFilename, BOOL bIgnoreDeleted);
   int DbfClose (PDBF);
   int DbfType (PDBF, char*);
   int DbfPack (char*);
   int DbfFlush (PDBF);

   // Record management
   int DbfRecCount (PDBF pdbf);
   int DbfAppend (PDBF);
   int DbfUpdate (PDBF, unsigned long);
   int DbfDelete (PDBF, unsigned long);
   int DbfRecall (PDBF, unsigned long);

   // Record positioning
   int DbfEOF(PDBF pdbf);
   int DbfRecNo (PDBF pdbf);
   int DbfGo (PDBF, unsigned long);
   int DbfGoTop (PDBF);
   int DbfGoBottom (PDBF);
   int DbfSkip (PDBF, long);

   // Searching
   int DbfFindChar (PDBF, char*, char*, int);
   int DbfFindDate (PDBF, char*, PTDATE2, int);
   int DbfFindBool (PDBF, char*, BOOL*, int);
   int DbfFindNum (PDBF, char*, double*, int);

   // Field management
   int DbfFldType (PDBF, char*, char*);
   int DbfFldLen (PDBF, char*, unsigned long*);
   char *DbfGetChar (PDBF pdbf, char *pszColumn);
   int DbfGetDate (PDBF, char*, PTDATE2);
   int DbfGetBool (PDBF, char*, BOOL*);
   double DbfGetNum (PDBF pdbf, char* pszColumn);
   int DbfPutChar (PDBF, char*, char*);
   int DbfPutDate (PDBF, char*, PTDATE2);
   int DbfPutBool (PDBF, char*, BOOL*);
   int DbfPutNum (PDBF, char*, double);

   // Memo
   int DbfMemoExist (PDBF, char*);
   int DbfMemoLength (PDBF, char*, unsigned long*);
   int DbfMemoRead (PDBF, char*, char*);
   int DbfMemoWrite (PDBF, char*, char*);
   int DbfMemoDelete (PDBF, char*);

   // Internal routines, dont use !
   int DbfFldCount (PDBF, unsigned long*);
   int DbfFldDecimal (PDBF, char*, unsigned long*);
   int DbfFldName (PDBF, unsigned long, char*);

   int DbfIsRecDeleted (PDBF);
   int DbfRecRefresh (PDBF);
   int DbfUtlCut (char*, unsigned long);
   int DbfUtlSoundex (char*, char*);

   // Field internal
   int DbfFldNGetCharacter (PDBF, unsigned long, char*);
   int DbfFldNGetDate (PDBF, unsigned long, PTDATE2);
   int DbfFldNGetLogical (PDBF, unsigned long, BOOL*);
   int DbfFldNGetNumeric (PDBF, unsigned long, double*);
   int DbfFldNPutCharacter (PDBF, unsigned long, char*);
   int DbfFldNPutDate (PDBF, unsigned long, PTDATE2);
   int DbfFldNPutLogical (PDBF, unsigned long, BOOL*);
   int DbfFldNPutNumeric (PDBF, unsigned long, double*);
   int DbfFldNumber (PDBF, char*, unsigned long*);

   // Memo internal
   int DbfMemNDelete (PDBF, unsigned long);
   int DbfMemNExist (PDBF, unsigned long);
   int DbfMemNLength (PDBF, unsigned long, unsigned long*);
   int DbfMemNRead (PDBF, unsigned long, char*);
   int DbfMemNWrite (PDBF, unsigned long, char*);

   // Searching internal
   int DbfRecNLocateCharacter (PDBF, unsigned long, char*, int);
   int DbfRecNLocateDate (PDBF, unsigned long, PTDATE2, int);
   int DbfRecNLocateLogical (PDBF, unsigned long, PBOOL, int);
   int DbfRecNLocateNumeric (PDBF, unsigned long, double*, int);

   // Misc internal routines
   int FindField (PDBF, char*, unsigned long*);
   int ReadRecord (PDBF, unsigned long);
   int WriteRecord (PDBF, unsigned long);
   int CheckMemo (PDBF);
   int CheckPDBF (PDBF);
   int ClearRecord (PDBF);

#endif /* _DBASE_H */

