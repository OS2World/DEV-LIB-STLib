IF EXIST DEBUG.ID DEL *.OBJ
NMAKE STVIO-R.MAK
COPY STLIBVIO.H D:\BIN\H
COPY STVIOR.LIB  D:\BIN\LIB 
DEL STVIOR.LIB
ECHO CURRENT > RELEASE.ID
DEL DEBUG.ID