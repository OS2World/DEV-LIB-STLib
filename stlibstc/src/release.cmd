IF EXIST DEBUG.ID DEL *.OBJ
IF EXIST DDEBUG.ID DEL *.OBJ
IF EXIST DREL.ID DEL *.OBJ
NMAKE STSTC-R.MAK
@COPY STLIBSTC.H D:\BIN\H
@COPY BTISAM.H D:\BIN\H
@COPY STSTCR.LIB  D:\BIN\LIB 
@DEL STSTCR.LIB
ECHO CURRENT > RELEASE.ID
@DEL DEBUG.ID
@DEL DREL.ID
@DEL DDEBUG.ID
