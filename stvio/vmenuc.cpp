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
REM     *********************************************************************
REM     * WMENUC.MOD
REM     *********************************************************************
REM     * Part of WIN.MOD library after split
REM     *
REM     *********************************************************************
REM     * 931107: Split...
REM     *--------------------------------------------------------------------
REM     *********************************************************************
	REM $INCLUDE: 'STLIBVB.BI'
	DEFINT A-Z

'       ---------------------------------------------------------------------
'        WinMenuChoice:
'       ---------------------------------------------------------------------
	FUNCTION WinMenuChoice (hwnd AS WINDOWTYPE, iFlag, iRow, iCol, rubrik$, menu$(), li AS LISTBOXTYPE)
	    SELECT CASE iFlag
		   CASE 1
			li.llen = 0
			FOR counter = 1 TO UBOUND(menu$)
			    IF LEN(menu$(counter)) > li.llen THEN
			       li.llen = LEN(menu$(counter))
			    END IF
			NEXT counter
			IF li.llen < 22 THEN
			   li.llen = 22
			END IF
			
			li.row1 = iRow
			li.col1 = iCol

			IF (li.row1 = 0) OR (li.col1 = 0) THEN
			   trow1 = 13 - ((UBOUND(menu$) + 5) \ 2)
			   tcol1 = 40 - ((li.llen + 4) \ 2)
			   trow2 = UBOUND(menu$) + 4
			   tcol2 = li.llen + 5
			ELSE
			   trow1 = li.row1
			   tcol1 = li.col1
			   trow2 = UBOUND(menu$) + 4
			   tcol2 = li.llen + 5
			
			END IF
			
			li.row1 = trow1 + 1
			li.col1 = tcol1 + 1
			
			sVio$ = WinOpenWindow$(hwnd, trow1, tcol1, trow2, tcol2, rubrik$, -1, -1, -1)

			CALL VioPrint(trow1 + trow2 - 2, tcol1 + 2, STRING$(tcol2 - 3, 196))

			temp$ = " V„lj med / och Enter "
			CALL VioCPrint(trow1 + trow2 - 1, StrCenter(tcol1 + 3, tcol2, temp$), VioColor(hwnd.wfcolor, hwnd.wbcolor), SPACE$(LEN(temp$)))
			CALL VioPrint(trow1 + trow2 - 1, StrCenter(tcol1 + 3, tcol2, temp$), temp$)
			li.currow = li.row1 + 1
			li.mark = 0
			rubrik$ = sVio$

		   CASE 2
			IF li.mark <> li.currow OR rubrik$ = "REDRAW" THEN
			   FOR counter = 1 TO UBOUND(menu$)
			       vioclr = VioColor(hwnd.ffcolor, hwnd.fbcolor)
			       CALL VioCPrint(li.row1 + counter, li.col1 + 1, vioclr, SPACE$(li.llen + 2))
			       CALL VioCPrint(li.row1 + counter, li.col1 + 2, vioclr, menu$(counter))
			   NEXT counter
			   vioclr = VioColor(hwnd.efcolor, hwnd.ebcolor)
			   CALL VioCPrint(li.currow, li.col1 + 1, vioclr, SPACE$(li.llen + 2))
			   CALL VioCPrint(li.currow, li.col1 + 2, vioclr, menu$(li.currow - li.row1))
			   temp = INSTR(menu$(li.currow - li.row1), MID$(short$, li.currow - li.row1, 1))
			END IF
			li.mark = li.currow
			CALL KbdInkey(IDKNOWAIT, keycode, nul)
			
			SELECT CASE keycode
			       CASE KEYHOME
				    li.currow = li.row1 + 1
			       CASE KEYEND
				    li.currow = li.row1 + UBOUND(menu$)
			       CASE KEYUARROW
				    DO
				       li.currow = li.currow - 1
				    LOOP WHILE StrTrim$(menu$(li.currow - li.row1)) = "" AND (li.currow - li.row1) > 0
			       CASE KEYDARROW
				    li.currow = li.currow + 1
				    DO WHILE (li.currow - li.row1) < UBOUND(menu$)
				       IF StrTrim$(menu$(li.currow - li.row1)) = "" THEN
					  li.currow = li.currow + 1
				       ELSE
					  EXIT DO
				       END IF
				    LOOP
			       CASE ELSE
			END SELECT
			
			IF li.currow < li.row1 + 1 THEN
			   li.currow = li.row1 + 1
			ELSEIF li.currow > li.row1 + UBOUND(menu$) THEN
			   li.currow = li.row1 + UBOUND(menu$)
			END IF
			
			WinMenuChoice = keycode
			li.curpos = li.currow - li.row1

		   CASE 3
			CALL WinCloseWindow(hwnd, rubrik$)
			WinMenuChoice = TRUE
		   CASE ELSE
		END SELECT
	END FUNCTION

