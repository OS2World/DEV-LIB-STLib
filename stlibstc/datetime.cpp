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
/***************************************************************************/
/*                                                                         */
/*   Module:     DATETIME.C, date/time functions in STLIBPMC.LIB           */
/*                                                                         */
/*   Version:    0.005                                                     */
/*                                                                         */
/*   Tid:        901001, 000218, 10h                                       */
/*                                                                         */
/*  ---------------------------------------------------------------------  */
/*   901001:     Created in BASIC. (MA)                                    */
/*   920420:     OS/2 (BASIC) version with WeekDay bug fixed. (MA)         */
/*   931120:     Converted for C/SET2 (32bit OS/2). (MA)                   */
/*   940123:     More conversions for C. (MA)                              */
/*   940220:     Added DateToday. (MA)                                     */
/*   950729:     Changed malloc to static char's. (MA)                     */
/*   950804:     Made monthlen global. (MA)                                */
/*   951222:     Fixed to short string bug inn DateSerial. (MA, 1h)        */
/*   970701:     Added full year (4 char) to DateToday. (MA, 1h)           */
/*   990108:     Y2K and 1999 fixes. (MA, 1h)                              */
/*   000218:     Added more formats to SerialDate. (MA, 1h)                */
/***************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "stlibstc.h"

/* -------------------------------------- */
#define isleap(year) ((((year%4)==0) && ((year%100)!=0)) || ((year%400)==0))


/* -------------------------------------- */
int             aiMonthLen[13];


/*-----------------------------------------------------------------------------
  MonthLen: Return correct lenght of specified year's months.
-----------------------------------------------------------------------------*/
int MonthLen(int iYear, int iMonth)
     {
     int                iCounter;

     for (iCounter = 1; iCounter <= 12; iCounter++)
         aiMonthLen[iCounter] = 31;

     aiMonthLen[2] = 28;
     aiMonthLen[4] = 30;
     aiMonthLen[6] = 30;
     aiMonthLen[9] = 30;
     aiMonthLen[11] = 30;

     if (isleap(iYear))
        aiMonthLen[2] = 29;

     return (aiMonthLen[iMonth]);
     }


/*-----------------------------------------------------------------------------
  DateSerial: Converts datestring to serial number.
-----------------------------------------------------------------------------*/
int DateSerial(char *szDate, int iBase)
     {
     char               szTmpDate[16];

     time_t             ltime;
     struct tm          *curtime;

     int                iYear, iMonth, iDay;
     int                iCounter;
     int                iDays;


     if (strlen(szDate) == 0)
        {
        time(&ltime);
        curtime = localtime(&ltime);
        if (iBase == 2)
           sprintf(szTmpDate, "%2.2d%.2d%.2d", (1900 + curtime->tm_year) % 100, curtime->tm_mon+1, curtime->tm_mday);
        else
           sprintf(szTmpDate, "%.4d%.2d%.2d", 1900 + curtime->tm_year, curtime->tm_mon+1, curtime->tm_mday);
        }
     else
        strcpy(szTmpDate, szDate);

     if (iBase == 2)
        sscanf(szTmpDate, "%2d%2d%2d", &iYear, &iMonth, &iDay);
     else
        sscanf(szTmpDate, "%4d%2d%2d", &iYear, &iMonth, &iDay);


     // Assume < 80 is 2000
     if (iYear < 80)
        iYear = iYear + 100;

     iDays = 0;
     for (iCounter = 1;iCounter < (iYear - 79); iCounter++)
         {
         if (isleap(1979 + iCounter))
            iDays = iDays + 366;
         else
            iDays = iDays + 365;
         }

     MonthLen(1980 + iYear, 1);

     for (iCounter = 1;iCounter < iMonth; iCounter++)
         {
         iDays = iDays + aiMonthLen[iCounter];
         }

     return (iDays + iDay);

     }


/*-----------------------------------------------------------------------------
  SerialDate: Converts a serial number to a datestring.
-----------------------------------------------------------------------------*/
char *SerialDate(int iDateSerial, int iBase)
     {
     static char        szTmpDate[16];

     char               szYear[6], szMonth[6], szDay[6];
     char               szTmpMonth[6], szTmpDay[6];

     int                iYear, iMonth, iDay;
     int                iCounter;
     int                iDays, iRest;
     int                iLast;


     iDays = 0;
     for (iCounter = 1; iCounter < 120; iCounter++)
         {
         if (isleap(1979 + iCounter))
            {
            iDays = iDays + 366;
            iLast = 366;
            }
         else
            {
            iDays = iDays + 365;
            iLast = 365;
            }

         if (iDays >= iDateSerial)
            {
            iYear = 1979 + iCounter;
            if (iDays == iDateSerial)
               {
               iRest = 0;
               iMonth = 12;
               iDay = 31;
               }
            else
               {
               iRest = iDateSerial - (iDays - iLast);
               }

            break;
            }
         }


     if (iRest != 0)
        {
        MonthLen(1900 + iYear, 1);
        for (iCounter = 1; iCounter <= 12; iCounter++)
            {

            iRest = iRest - aiMonthLen[iCounter];
            iLast = aiMonthLen[iCounter];

            if (iRest <= 0)
               {
               iMonth = iCounter;

               if (iRest == 0)
                  iDay = aiMonthLen[iCounter];
               else
                  iDay = iRest + iLast;
               break;
               }
            }
        }

     _itoa(iYear, szYear, 10);

     _itoa(iMonth, szTmpMonth, 10);
     if (strlen(szTmpMonth) == 1)
        sprintf(szMonth, "0%s", szTmpMonth);
     else
        strcpy(szMonth, szTmpMonth);

     _itoa(iDay, szTmpDay, 10);
     if (strlen(szTmpDay) == 1)
        sprintf(szDay, "0%s", szTmpDay);
     else
        strcpy(szDay, szTmpDay);


     switch (iBase) 
            {
            case 2:
                 sprintf(szTmpDate, "%2.-2s%.2s%.2s", szYear, szMonth, szDay);
                 break;

            case 4:
                 sprintf(szTmpDate, "%.-4s%.2s%.2s", szYear, szMonth, szDay);
                 break;

            case 12:
                 sprintf(szTmpDate, "%2.-2s-%.2s-%.2s", szYear, szMonth, szDay);
                 break;

            case 14:
                 sprintf(szTmpDate, "%.-4s-%.2s-%.2s", szYear, szMonth, szDay);
                 break;
            }

     return (szTmpDate);
     }


/*-----------------------------------------------------------------------------
  DateToday: Returns a string with todays date.
-----------------------------------------------------------------------------*/
char *DateToday(int iFlag)
     {
     static char        szBuffer[16];
     time_t             ltime;
     struct tm          *curtime;

     time(&ltime);
     curtime = localtime(&ltime);

     switch (iFlag)
            {
            case 0:
                 sprintf(szBuffer, "%-2.2d%.2d%.2d", (1900 + curtime->tm_year) % 100, curtime->tm_mon+1, curtime->tm_mday);
                 break;

            case 1:
                 sprintf(szBuffer, "(%-2.2d%.2d%.2d/%.2d:%.2d)", (1900 + curtime->tm_year) % 100, curtime->tm_mon+1, curtime->tm_mday, curtime->tm_hour, curtime->tm_min);
                 break;

            case 2:
                 sprintf(szBuffer, "%4.4d%.2d%.2d", 1900 + curtime->tm_year, curtime->tm_mon+1, curtime->tm_mday);
                 break;

            case 3:
                 sprintf(szBuffer, "(%4.4d%.2d%.2d/%.2d:%.2d)", 1900 + curtime->tm_year, curtime->tm_mon+1, curtime->tm_mday, curtime->tm_hour, curtime->tm_min);
                 break;
                 }

     return(szBuffer);
     }


/*-----------------------------------------------------------------------------
  DayofWeek: Gets day of week from date string.
-----------------------------------------------------------------------------*/
int DayOfWeek(char *szDate)
     {

     return (0);
     }

/*************************************************************************************
            REDIM mlen(12)
            FOR counter = 1 TO 12
                mlen(counter) = 31
            NEXT counter
            mlen(2) = 28
            mlen(4) = 30
            mlen(6) = 30
            mlen(9) = 30
            mlen(11) = 30
            IF sDay$ = "" THEN
               sDay$ = RIGHT$(DATE$, 2) + LEFT$(DATE$, 2) + MID$(DATE$, 4, 2)
            END IF
            dyear = 1900 + VAL(LEFT$(sDay$, 2))
            iMonth = VAL(MID$(sDay$, 3, 2))
            iDay = VAL(RIGHT$(sDay$, 2))
            days = 0
            FOR counter = 1899 TO dyear - 1
                IF (temp MOD 4 = 0 AND temp MOD 100 <> 0) OR (temp MOD 400 = 0) THEN
                   days = days + (366 MOD 7)
                ELSE
                   days = days + (365 MOD 7)
                END IF
            NEXT
            FOR counter = 1 TO iMonth - 1
                days = days + mlen(counter)
            NEXT
            IF (dyear MOD 4 = 0 AND dyear MOD 100 <> 0) OR (dyear MOD 400 = 0) THEN
               IF iMonth > 2 THEN
                  days = days + 1
               END IF
            END IF
            days = days + iDay


days%7  = days MOD 7

            DayOfWeek = days MOD 7
        END FUNCTION

*************************************************************************************/
