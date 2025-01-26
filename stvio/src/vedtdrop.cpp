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
DECLARE SUB SetColor (hwnd AS ANY)
DECLARE SUB ValidPos (hwnd AS ANY, iRow%, iCol%, iLen%)
REM     *********************************************************************
REM     * EDITDROP.MOD
REM     *********************************************************************
REM     * FUNCTION EditDrop%
REM     *
REM     *********************************************************************
REM     * 910712: Created
REM     *--------------------------------------------------------------------
REM     * 921127: Code changed for non-auto dropdown (ENTER drops).
REM     *--------------------------------------------------------------------
REM     * 921202: Change to autodrop if item=0.
REM     *--------------------------------------------------------------------
REM     * 930419: Changed for new color settings.
REM     *--------------------------------------------------------------------
REM     * 930610: Fixed colours for new field look.
REM     *********************************************************************
	REM $INCLUDE: 'STLIBVB.BI'

DEFINT A-Z
'       ---------------------------------------------------------------------
'        EditDrop: Dropdown list.                                     930419
'       ---------------------------------------------------------------------
	FUNCTION EditDrop (aDrop$(), curItem, row, col, lcol, ret(), hook)

	    vio = VioColor(st.iGloefcolor, st.iGloebcolor)
	    CALL VioCPrint(row, col, vio, SPACE$(lcol) + "³" + CHR$(31))
	    CALL VioCPrint(row, col, vio, LEFT$(aDrop$(curItem), lcol))

	    IF curItem <> 0 THEN
	       quit1 = FALSE
	       DO
		  LOCATE row, col, 1
		  CALL KbdInkey(IDKWAIT, iKey, void)
		  LOCATE row, col, 0
		  SELECT CASE iKey
			 CASE KEYENTER
			      quit1 = TRUE
			 CASE ELSE
			      FOR counter = 1 TO UBOUND(ret)
				  IF iKey = ret(counter) THEN
				     EditDrop = iKey
				     vio = VioColor(st.iGloefcolor, st.iGloebcolor)
				     CALL VioCPrint(row, col, vio, SPACE$(lcol + 2))
				     CALL VioCPrint(row, col, vio, LEFT$(aDrop$(curItem), lcol))
				     EXIT FUNCTION
				  END IF
			      NEXT
		  END SELECT
	       LOOP WHILE quit1 = FALSE
	    ELSE
	       CALL VioCPrint(row, col, vio, LEFT$(aDrop$(1), lcol))
	    END IF

	    IF UBOUND(aDrop$) < 4 THEN
	       items = UBOUND(aDrop$)
	    ELSE
	       items = 4
	    END IF

	    ' Kolla av storlek och var p† sk„rmen (MA:930426)
	    '-------------------------------------------------
	    IF row + items + 1 > 25 THEN
	       items = 24 - row
	    END IF

	    r1 = row + 1
	    c1 = col
	    r2 = r1 + items
	    c2 = c1 + lcol + 1

	    vio$ = VioSpace$(r1, c1, r2, c2)
	    CALL VioGetWindow(r1, c1, r2, c2, vio$)
	    CALL VioFrame(r1, c1, r2, c2, VioColor(st.iGloebcolor, st.iGloefcolor), 32)
	    FOR counter = r1 TO r2 - 1
		CALL VioPrint(counter, c1, "³")
		CALL VioPrint(counter, c2, "³")
	    NEXT
	    CALL VioPrint(r2, c1, "À" + STRING$(lcol, "Ä") + "Ù")

	    DIM lb AS LISTBOXTYPE
	    lb.row1 = r1
	    lb.col1 = c1 + 1
	    lb.row2 = r2 - 1
	    lb.col2 = c2 - 1
	    lb.startat = curItem
	    lb.frame = FALSE

	    nul = ListBox(lb, 0, aDrop$())

	    quit = FALSE
	    DO
	       retval = ListBox(lb, 1, aDrop$())
	       SELECT CASE retval
		      CASE KEYHOME, KEYEND, KEYUARROW, KEYDARROW
			   CALL VioPrint(row, col, SPACE$(lcol) + "³" + CHR$(31))
			   CALL VioPrint(row, col, LEFT$(aDrop$(lb.curpos), lcol))
		      CASE ELSE
			   FOR counter = 1 TO UBOUND(ret)
			       IF retval = ret(counter) THEN
				  EditDrop = retval
				  curItem = lb.curpos
				  aDrop$(0) = aDrop$(curItem)
				  quit = TRUE
				  EXIT FOR
			       END IF
			   NEXT
	       END SELECT
	    LOOP WHILE quit = FALSE
	    nul = ListBox(lb, 2, aDrop$())
	    CALL VioPutWindow(r1, c1, r2, c2, vio$)

	    vio = VioColor(st.iGloffcolor, st.iGlofbcolor)
	    CALL VioCPrint(row, col, vio, SPACE$(lcol + 2))
	    CALL VioCPrint(row, col, vio, LEFT$(aDrop$(curItem), lcol))

	END FUNCTION

'       ---------------------------------------------------------------------
'        WinEditDrop:
'       ---------------------------------------------------------------------
	FUNCTION WinEditDrop (hwnd AS WINDOWTYPE, asDrop$(), iCurItem, iRow, iCol, iLenCol, iRet(), iHook)
	    iWinRow = iRow
	    iWinCol = iCol
	    iWinLen = iLenCol
	    CALL ValidPos(hwnd, iWinRow, iWinCol, iWinLen)
	    CALL SetColor(hwnd)
	    WinEditDrop = EditDrop(asDrop$(), iCurItem, iWinRow, iWinCol, iWinLen, iRet(), iHook)
	END FUNCTION

