IF EXIST RELEASE.ID DEL *.OBJ
IF EXIST DDEBUG.ID DEL *.OBJ
IF EXIST DREL.ID DEL *.OBJ
NMAKE STSTC-D.MAK
COPY STLIBSTC.H D:\BIN\H
COPY BTISAM.H D:\BIN\H
COPY STSTCD.LIB  D:\BIN\LIB 
DEL STSTCD.LIB
ECHO CURRENT > DEBUG.ID
DEL RELEASE.ID
DEL DREL.ID
DEL DDEBUG.ID
