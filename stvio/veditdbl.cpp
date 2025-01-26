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
DECLARE FUNCTION EditDbl% (value#, row%, col%, fmt$, retur%())
DECLARE SUB SetColor (hwnd AS ANY)
DECLARE SUB ValidPos (hwnd AS ANY, iRow%, iCol%, iLen%)
DECLARE FUNCTION ENNumEdit% (num$, row%, col%, frmat$, ret%())
REM     *********************************************************************
REM     * EDITDBL.MOD
REM     *********************************************************************
REM     * Function EditDbl%
REM     *
REM     *********************************************************************
REM     * 870513: Created
REM     *--------------------------------------------------------------------
REM     * 900629: Changed for New update of STLIB7
REM     *--------------------------------------------------------------------
REM     * 900630: Lots of bugs fixed/ ENFormat added
REM     *--------------------------------------------------------------------
REM     * 900708: Total rewrite. Mouse support added
REM     *--------------------------------------------------------------------
REM     * 921127: Mouse support removed
REM     *--------------------------------------------------------------------
REM     * 930419: Chnged for new color settings
REM     *--------------------------------------------------------------------
REM     * 930610: Fixed colours for new field look.
REM     *--------------------------------------------------------------------
REM     * 931107: Split into two files.
REM     *********************************************************************
	REM $INCLUDE: 'STLIBVB.BI'
	DEFINT A-Z

'       ---------------------------------------------------------------------
'        EditDbl: Give user fixed length input of doubles.         881016
'       ---------------------------------------------------------------------
	FUNCTION EditDbl (value#, row, col, fmt$, retur())
	    IF StrTrim$(st.iFloatDelim) = "" THEN
	       st.iFloatDelim = ","
	    END IF
	    old$ = st.iFloatDelim
	    st.iFloatDelim = "."
	    temp$ = StrFormat$(IDSFDBL, fmt$, STR$(value#))
	    st.iFloatDelim = old$
	    IF ASC(temp$) = 35 THEN
	       temp$ = ""
	    END IF
	    ret = ENNumEdit(temp$, row, col, fmt$, retur())
	    value# = VAL(temp$)
	    EditDbl = ret
	END FUNCTION

'       ---------------------------------------------------------------------
'        ENNumEdit:
'       ---------------------------------------------------------------------
	FUNCTION ENNumEdit (num$, row, col, frmat$, ret())
	   IF ASC(frmat$) = 45 THEN
	      IF INSTR(num$, "-") THEN
		 neg$ = "-"
	      ELSE
		 neg$ = " "
	      END IF
	      fmt$ = RIGHT$(frmat$, LEN(frmat$) - 1)
	      neg = TRUE
	   ELSE
	      neg$ = ""
	      fmt$ = frmat$
	      neg = FALSE
	   END IF
	   temp = INSTR(num$, "-")
	   IF temp THEN
	      MID$(num$, temp, 1) = " "
	   END IF
	   IF INSTR(fmt$, ".") THEN
	      temp = INSTR(fmt$, ".")
	      IF neg THEN
		 len1 = temp - 2
		 len2 = LEN(fmt$) - len1 - 2
	      ELSE
		 len1 = temp - 1
		 len2 = LEN(fmt$) - len1 - 1
	      END IF
	      temp = INSTR(num$, ".")
	      IF temp THEN
		 IF neg THEN
		    num1$ = MID$(num$, 2, len1)
		 ELSE
		    num1$ = LEFT$(num$, len1) + "."
		 END IF
		 num2$ = RIGHT$(num$, LEN(num$) - len1 - 1)
	      ELSE
		 num1$ = num1$ + "."
		 num2$ = ""
	      END IF
	      FOR counter = LEN(num2$) TO 1 STEP -1
		  IF MID$(num2$, counter, 1) = "0" THEN
		     MID$(num2$, counter, 1) = " "
		  ELSE
		     EXIT FOR
		  END IF
	      NEXT counter
	      decpos = 1
	      dcol1 = col
	      dcol2 = col + 2
	      dec = TRUE
	   ELSE
	      len1 = LEN(fmt$)
	      len2 = 0
	      num1$ = RIGHT$(num$, len1)
	      num2$ = ""
	      dec = FALSE
	   END IF
	   num$ = neg$ + num1$ + num2$

	   COLOR st.iGloefcolor, st.iGloebcolor

	   kpress = FALSE
	   quit = FALSE
	   DO
	      IF dec = TRUE THEN
		 IF decpos = 1 THEN
		    temp = dcol1
		 ELSE
		    temp = dcol2
		 END IF
		 IF neg = TRUE THEN
		    tcol = temp + LEN(num$) - LEN(num2$)
		 ELSE
		    tcol = temp + LEN(num$) - LEN(num2$) - 2
		 END IF
		 temp$ = num$
		 MID$(temp$, INSTR(temp$, "."), 1) = st.iFloatDelim
	      ELSE
		 IF neg = TRUE THEN
		    tcol = col + len1
		 ELSE
		    tcol = col + len1 - 1
		 END IF
		 temp$ = num$
	      END IF

	      LOCATE row, col
	      PRINT temp$;
	      LOCATE row, tcol, 1
	      CALL KbdInkey(IDKWAIT, keycode, shift)
	      LOCATE , , 0

	      SELECT CASE keycode
		     CASE ASC("-")
			  IF neg = TRUE THEN
			     IF neg$ = " " THEN
				neg$ = "-"
				MID$(num$, 1, 1) = "-"
			     ELSE
				neg$ = " "
				MID$(num$, 1, 1) = " "
			     END IF
			  END IF
		     CASE ASC(st.iFloatDelim)
			  IF dec = TRUE THEN
			     IF decpos = 1 THEN
				decpos = 2
			     END IF
			  END IF
		     CASE 48 TO 57
			  IF kpress = FALSE THEN
			     IF dec THEN
				IF neg THEN
				   num$ = SPACE$(len1 + 2) + "." + STRING$(len2, " ")
				   neg$ = " "
				ELSE
				   num$ = SPACE$(len1) + "." + STRING$(len2, " ")
				END IF
			     ELSE
				IF neg THEN
				   num$ = SPACE$(len1 + 1)
				ELSE
				   num$ = SPACE$(len1)
				END IF
			     END IF
			  END IF
			  IF dec THEN
			     IF decpos = 1 THEN
				IF neg THEN
				   temp$ = LTRIM$(MID$(num$, 2, len1 + 1))
				ELSE
				   temp$ = LTRIM$(LEFT$(num$, len1))
				END IF
				IF LEN(temp$) THEN
				   temp$ = temp$ + CHR$(keycode)
				ELSE
				   temp$ = CHR$(keycode)
				END IF
				IF neg THEN
				   IF LEN(temp$) <= len1 + 1 THEN
				      temp$ = SPACE$(len1 + 1 - LEN(temp$)) + temp$
				      MID$(num$, 2) = temp$
				   END IF
				ELSE
				   IF LEN(temp$) <= len1 THEN
				      temp$ = SPACE$(len1 - LEN(temp$)) + temp$
				      MID$(num$, 1) = temp$
				   END IF
				END IF
			     ELSE
				temp$ = RIGHT$(num$, len2)
				temp$ = RTRIM$(temp$)
				temp$ = temp$ + CHR$(keycode)
				IF neg THEN
				   MID$(num$, len1 + 4) = temp$
				ELSE
				   MID$(num$, len1 + 2) = temp$
				END IF
			     END IF
			  ELSE
			     IF neg THEN
				temp$ = LTRIM$(RIGHT$(num$, LEN(num$) - 1)) + CHR$(keycode)
			     ELSE
				temp$ = LTRIM$(num$) + CHR$(keycode)
			     END IF
			     IF neg THEN
				IF LEN(temp$) <= len1 THEN

				   MID$(num$, 2) = SPACE$(len1 - LEN(temp$)) + temp$
				END IF
			     ELSE
				IF LEN(temp$) <= len1 THEN
				   MID$(num$, 1) = SPACE$(len1 - LEN(temp$)) + temp$
				END IF
			     END IF
			  END IF
		     CASE KEYBKSP, KEYDEL
			  IF dec THEN
			     IF decpos = 1 THEN
				IF neg THEN
				   temp$ = MID$(num$, 2, len1 + 1)
				   temp$ = " " + LEFT$(temp$, LEN(temp$) - 1)
				   MID$(num$, 2) = temp$
				ELSE
				   temp$ = LEFT$(num$, len1)
				   temp$ = " " + LEFT$(temp$, LEN(temp$) - 1)
				   MID$(num$, 1) = temp$
				END IF
			     ELSE
				temp$ = RTRIM$(RIGHT$(num$, len2))
				IF LEN(temp$) > 0 THEN
				   temp$ = LEFT$(temp$, LEN(temp$) - 1) + " "
				   IF neg THEN
				      MID$(num$, len1 + 4) = temp$
				   ELSE
				      MID$(num$, len1 + 2) = temp$
				   END IF
				END IF
			     END IF
			  ELSE
			     IF neg THEN
				num$ = neg$ + " " + MID$(num$, 2, LEN(num$) - 2)
			     ELSE
				num$ = " " + LEFT$(num$, LEN(num$) - 1)
			     END IF
			  END IF
		     CASE ELSE
			  FOR counter = 1 TO UBOUND(ret)
			      IF ret(counter) = keycode THEN
				 quit = TRUE
				 ENNumEdit = keycode
				 EXIT FOR
			      END IF
			  NEXT counter
	      END SELECT
	      kpress = TRUE
	   LOOP WHILE quit = FALSE
	   IF dec = TRUE THEN
	      IF decpos = 1 THEN
		 temp = dcol1
	      ELSE
		 temp = dcol2
	      END IF
	      IF neg = TRUE THEN
		 tcol = temp + LEN(num$) - LEN(num2$)
	      ELSE
		 tcol = temp + LEN(num$) - LEN(num2$) - 2
	      END IF
	      temp$ = num$
	      MID$(temp$, INSTR(temp$, "."), 1) = st.iFloatDelim
	   ELSE
	      IF neg = TRUE THEN
		 tcol = col + len1
	      ELSE
		 tcol = col + len1 - 1
	      END IF
	      temp$ = num$
	   END IF

	   COLOR st.iGloffcolor, st.iGlofbcolor
	   LOCATE row, col
	   PRINT temp$;

	END FUNCTION

'       ---------------------------------------------------------------------
'        WinEditDbl:
'       ---------------------------------------------------------------------
	FUNCTION WinEditDbl (hwnd AS WINDOWTYPE, fValue#, iRow, iCol, sForm$, iRet())
	    iWinRow = iRow
	    iWinCol = iCol
	    iLen = LEN(sForm$)
	    CALL ValidPos(hwnd, iWinRow, iWinCol, iLen)
	    CALL SetColor(hwnd)
	    sForm$ = LEFT$(sForm$, iLen)
	    WinEditDbl = EditDbl(fValue#, iWinRow, iWinCol, sForm$, iRet())
	END FUNCTION

