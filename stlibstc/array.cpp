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


   THIS CODE CONTAIN BUGS, MUST BE THOUROGLY CHECKED
   =================================================


         A R R A Y . C
        ===============

        Library for simpler array handling

        Startat: 940604         Last: 940604        Timelog:  4h

----------------------------------------------------------------------------
940604, 4h  Start of this code, changed to unsigned int for Windows sake (MA).

****************************************************************************/

/*
 Known problems:
 --------------------------
 - Rensar inte nya items vilket den borde g”ra.

 */

#include <stdlib.h>
#include <string.h>
#include "stlibstc.h"


/*************************************************************************************
  ArrayNew: Creates a new array.
*************************************************************************************/
void *ArrayNew(unsigned int iItemLen, unsigned int iAntal)
     {
     return calloc(iAntal, iItemLen);
     }


/*************************************************************************************
  ArrayFree: Frees (deletes) a array.
*************************************************************************************/
void ArrayFree(char *array)
     {
     free(array);
     }


/*************************************************************************************
  ArrayResize: Resizes an array.
*************************************************************************************/
void *ArrayResize(char *array, unsigned int iItemLen, unsigned int iNewSize)
     {
     array = (char*) realloc (array, (iNewSize + 1) * iItemLen);
     return (array);
     }


/*************************************************************************************
  ArrayInsert: Inserts item to an existing array.
*************************************************************************************/
void *ArrayInsert(char *array, unsigned int iItemLen, unsigned int iMaxValue, unsigned int iPosition)
     {
     unsigned int   iCounter;
     unsigned int   iArraySize;

     iArraySize = iMaxValue + 1;

     array = (char*) realloc (array, (iArraySize + 1) * iItemLen);

     for (iCounter = iPosition; iCounter < (iMaxValue + 1); iCounter++)
         memcpy(&array[(iCounter + 1) * iItemLen], &array[iCounter * iItemLen], iItemLen);

     return (array);
     }


/*************************************************************************************
  ArrayAppend: Appends item to the end of an array.
*************************************************************************************/
void *ArrayAppend(char *array, unsigned int iItemLen, unsigned int iMaxValue)
     {
     unsigned int   iArraySize;

     iArraySize = iMaxValue + 1;
     array = (char*) realloc (array, (iArraySize + 1) * iItemLen);
     return (array);
     }


/*************************************************************************************
  ArrayDelete: Deletes item from an existing array.
*************************************************************************************/
void *ArrayDelete(char *array, unsigned int iItemLen, unsigned int iMaxValue, unsigned int iPosition)
     {
     unsigned int   iCounter;
     unsigned int   iArraySize;

     iArraySize = iMaxValue + 1;

     for (iCounter = iPosition; iCounter < iMaxValue; iCounter++)
         memcpy(&array[iCounter * iItemLen], &array[(iCounter + 1) * iItemLen], iItemLen);

     array = (char*) realloc (array, (iArraySize - 1) * iItemLen);

     return (array);
     }


/*************************************************************************************
  ArrayCopy: Copies item in array to another item.
*************************************************************************************/
void ArrayCopy(char *array, unsigned int iItemLen, unsigned int iFrom, unsigned int iTo)
     {
     memcpy(&array[iTo * iItemLen], &array[iFrom * iItemLen], iItemLen);
     }


/*************************************************************************************
  ArraySwap: Copies item in array to other item.
*************************************************************************************/
void ArraySwap(char *array, unsigned int iItemLen, unsigned int iItem1, unsigned int iItem2)
     {
     char   *tmpBuffer;

     tmpBuffer = (char*) malloc(iItemLen);
     memcpy(&tmpBuffer, &array[iItem2 * iItemLen], iItemLen);
     memcpy(&array[iItem2 * iItemLen], &array[iItem1 * iItemLen], iItemLen);
     memcpy(&array[iItem1 * iItemLen], &tmpBuffer, iItemLen);
     }

