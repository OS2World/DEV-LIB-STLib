# Created by IBM WorkFrame/2 MakeMake at 13:25:35 on 23 Nov 1995
#
# The actions included in this make file are:
#  Compile::C++ Compiler
#  Lib::Library Tool

.SUFFIXES: .cpp .obj

.all: \
    .\ststcr.lib

.cpp.obj:
    @echo " Compile::C++ Compiler "
   icc.exe /Gm /Gs /Fo"%|dpfF.obj" /C %s

.\STstCR.LIB: \
    .\ARRAY.obj \
    .\std.obj \
    .\serial.obj \
    .\BTISAM.obj \
    .\BTRIVE32.obj \
    .\DATETIME.obj \
    .\file.obj \
    .\memo.obj
    @echo " Lib::Library Tool "
    ilib.exe  /nol /noi .\STstCR.LIB  -+.\array.obj \
                                                        -+.\std.obj \
                                                        -+.\serial.obj \
                                                        -+.\BTISAM.obj \
                                                        -+.\BTRIVE32.obj \
                                                        -+.\DATETIME.obj \
                                                        -+.\file.obj \
                                                        -+.\memo.obj;

.\std.obj: \
    .\std.cpp \
    {.;$(INCLUDE);}stlibstc.h

.\serial.obj: \
    .\serial.cpp \
    {.;$(INCLUDE);}stlibstc.h

.\memo.obj: \
    .\memo.cpp \
    {.;$(INCLUDE);}stlibstc.h

.\file.obj: \
    .\file.cpp \
    {.;$(INCLUDE);}stlibstc.h

.\DATETIME.obj: \
    .\DATETIME.cpp \
    {.;$(INCLUDE);}stlibstc.h

.\BTRIVE32.obj: \
    .\BTRIVE32.cpp \
    {.;$(INCLUDE);}stlibstc.h

.\BTISAM.obj: \
    .\BTISAM.cpp \
    {.;$(INCLUDE);}stlibstc.h \
    {.;$(INCLUDE);}btisam.h

.\ARRAY.obj: \
    .\ARRAY.cpp \
    {.;$(INCLUDE);}stlibstc.h

