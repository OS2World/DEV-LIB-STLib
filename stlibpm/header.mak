CC=icc.exe
ILIB=ilib.exe
RC=rc.exe
IMPLIB=implib.exe
MC=mc.exe

CPP_FLAGS=/DOS2 /Gm+ /Gs+ /Fo"%|dpfF.obj" /C %s
D_CPP_FLAGS=/DOS2 /Tx /Gh /Ti+ /Gm+ /Tm /Fb /Fo"%|dpfF.obj" /C %s
LINK_FLAGS=/B" /nodeb /packd /optfunc /exepack:2 /pmtype:pm"
D_LINK_FLAGS=/B" /de /br /pmtype:pm"

CPP_FLAGS_DLL=/DOS2 /Gm /Gs /Ge- /Fo"%|dpfF.obj" /C %s
D_CPP_FLAGS_DLL=/DOS2 /Ti+ /Gm+ /Ge- /Gd- /Fo"%|dpfF.obj" /C %s
LINK_FLAGS_DLL=/B" /nodeb /packd /optfunc /exepack:2"
D_LINK_FLAGS_DLL=/B" /de /br /pmtype:pm"

RC_FLAGS=-r -x2 %s %|dpfF.RES
D_RC_FLAGS=-r %s %|dpfF.RES

LIB_FLAGS=/NoFreeFormat /nol /noi 

H_PM=stlibpmc.h
H_VIO=stlibvio.h
H_STD=stlibstc.h
H_MYSQL=mysql.h
H_BTR=btisam.h
H_DBF=dbf.h

STLIB_PM=stpmcr.lib ststcr.lib mysqldll.lib btrcalls.lib
STLIB_VIO=stvior.lib ststcr.lib mysqldll.lib btrcalls.lib
D_STLIB_PM=stpmcd.lib ststcd.lib mysqldll.lib btrcalls.lib
D_STLIB_VIO=stviod.lib ststcd.lib mysqldll.lib btrcalls.lib

STLIB_PM_DLL=stpmcrd.lib ststcrd.lib mysqldll.lib btrcalls.lib
D_STLIB_PM_DLL=stpmcdd.lib ststcdd.lib mysqldll.lib btrcalls.lib
