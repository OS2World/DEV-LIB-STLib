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
DECLARE FUNCTION EditTime% (edtime$, row%, col%, retur%())
DECLARE SUB SetColor (hwnd AS ANY)
DECLARE SUB ValidPos (hwnd AS ANY, iRow%, iCol%, iLen%)
DECLARE FUNCTION ESChkTimeRet% (ret%, row%, col%, fieldid%, retur%(), quit%)
REM     *********************************************************************
REM     * EDITTIME.MOD
REM     *********************************************************************
REM     * FUNCTION EditTime%
REM     *
REM     *********************************************************************
REM     * 890328: Created
REM     *--------------------------------------------------------------------
REM     * 900629: Changed for New update of STLIB7
REM     *--------------------------------------------------------------------
REM     * 900630: Exit with <-/-> removed. Bugs in EditTime fixed
REM     *--------------------------------------------------------------------
REM     * 900706: EditString using string, full mouse support
REM     *--------------------------------------------------------------------
REM     * 900907: EditTime changed to HH/MM (seconds skipped)
REM     *--------------------------------------------------------------------
REM     * 921127: Mouse support removed
REM     *--------------------------------------------------------------------
REM     * 930419: Change for new color settings
REM     *--------------------------------------------------------------------
REM     * 930426: Fixed ; for print on line 24,25
REM     *--------------------------------------------------------------------
REM     * 930603: Fixed bug that did NOT allow 1900/2000 (00 year)
REM     *--------------------------------------------------------------------
REM     * 930610: Fixed colours for new field look and added IDHPSW.
REM     *--------------------------------------------------------------------
REM     * 931107: Split into three files
REM     *********************************************************************
	REM $INCLUDE: 'STLIBVB.BI'
	DEFINT A-Z

'       ---------------------------------------------------------------------
'        EditTime: Shows editfield for time input.                    880624
'       ---------------------------------------------------------------------
	FUNCTION EditTime (edtime$, row, col, retur())
	    hour$ = LEFT$(TIME$, 2)
	    minute$ = MID$(TIME$, 4, 2)
	    IF LEN(edtime$) = 4 THEN
	       temp$ = MID$(edtime$, 1, 2)
	       IF VAL(temp$) < 24 THEN
		  hour$ = temp$
	       END IF
	       temp$ = MID$(edtime$, 3, 2)
	       IF VAL(temp$) < 61 THEN
		  minute$ = temp$
	       END IF
	    END IF
	    IF StrTrim$(st.iTimeDelim) = "" THEN
	       st.iTimeDelim = ":"
	    END IF

	    COLOR st.iGloefcolor, st.iGloebcolor

	    LOCATE row, col
	    PRINT hour$; st.iTimeDelim; minute$;
	    REDIM ret(UBOUND(retur) + 3)
	    FOR counter = 1 TO UBOUND(retur)
		ret(counter) = retur(counter)
	    NEXT counter
	    ret(UBOUND(retur) + 1) = KEYXLAST
	    ret(UBOUND(retur) + 2) = KEYENTER
	    ret(UBOUND(retur) + 3) = KEYTAB
	    quit = FALSE
	    fieldid = 1
	    DO
	       SELECT CASE fieldid
		      CASE 1
			   quit1 = FALSE
			   DO
			      old$ = hour$
			      ret = EditString(hour$, row, col, 2, ret(), IDHTIME)
			      IF VAL(hour$) < 24 THEN
				 quit1 = TRUE
			      ELSE
				 hour$ = old$
				 BEEP
			      END IF
			   LOOP UNTIL quit1 = TRUE
		      CASE 2
			   quit1 = FALSE
			   DO
			      old$ = minute$
			      ret = EditString(minute$, row, col + 3, 2, ret(), IDHTIME)
			      IF VAL(minute$) < 61 THEN
				 quit1 = TRUE
			      ELSE
				 minute$ = old$
				 BEEP
			      END IF
			   LOOP UNTIL quit1 = TRUE
		      CASE ELSE
	       END SELECT

	       COLOR st.iGloefcolor, st.iGloebcolor
	       LOCATE row, col
	       PRINT hour$; st.iTimeDelim; minute$;

	       EditTime = ESChkTimeRet(ret, row, col, fieldid, retur(), quit)
	       IF fieldid > 2 THEN
		  fieldid = 2
	       ELSEIF fieldid < 1 THEN
		  fieldid = 1
	       END IF
	    LOOP UNTIL quit = TRUE
	    edtime$ = hour$ + minute$

	    COLOR st.iGloffcolor, st.iGlofbcolor
	    LOCATE row, col
	    PRINT hour$; st.iTimeDelim; minute$;
	END FUNCTION

'       ---------------------------------------------------------------------
'        ESChkTimeRet:
'       ---------------------------------------------------------------------
	FUNCTION ESChkTimeRet (ret, row, col, fieldid, retur(), quit)
	    SELECT CASE ret
		   CASE KEYENTER
			fieldid = fieldid + 1
			IF fieldid > 2 THEN
			   FOR counter = 1 TO UBOUND(retur)
			       IF retur(counter) = KEYENTER THEN
				  quit = TRUE
				  ESChkTimeRet = KEYENTER
			       END IF
			   NEXT counter
			END IF
		   CASE KEYTAB
			fieldid = fieldid + 1
			IF fieldid > 2 THEN
			   FOR counter = 1 TO UBOUND(retur)
			       IF retur(counter) = KEYTAB THEN
				  quit = TRUE
				  ESChkTimeRet = KEYTAB
			       END IF
			   NEXT counter
			END IF
		   CASE ELSE
			FOR counter = 1 TO UBOUND(retur)
			    IF retur(counter) = ret THEN
			       quit = TRUE
			       ESChkTimeRet = ret
			    END IF
			NEXT counter
	       END SELECT
	END FUNCTION

'       ---------------------------------------------------------------------
'        WinEditTime:
'       ---------------------------------------------------------------------
	FUNCTION WinEditTime (hwnd AS WINDOWTYPE, sTime$, iRow, iCol, iRet())
	    iWinRow = iRow
	    iWinCol = iCol
	    iLen = 8
	    CALL ValidPos(hwnd, iWinRow, iWinCol, iLen)
	    IF iLen = 8 THEN
	       CALL SetColor(hwnd)
	       WinEditTime = EditTime(sTime$, iWinRow, iWinCol, iRet())
	    END IF
	END FUNCTION

