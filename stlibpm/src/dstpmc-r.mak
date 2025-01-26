# Created by IBM WorkFrame/2 MakeMake at 13:25:35 on 23 Nov 1995
#
# The actions included in this make file are:
#  Compile::C++ Compiler
#  Lib::Library Tool

.SUFFIXES: .cpp .obj

.all: \
    .\stpmcrd.lib

.cpp.obj:
    @echo " Compile::C++ Compiler "
   icc.exe /Gm /G4 /Gs /Ge- /Fo"%|dpfF.obj" /C %s

.\STPMCRD.LIB: \
    .\help.obj \
    .\control.obj \
    .\cnr.obj \
    .\HOVERBOX.obj \
    .\STATBAR.obj \
    .\bitmap.obj \
    .\WINUTIL.obj \
    .\btnbar.obj \
    .\CLIP.obj \
    .\FONT.obj \
    .\MENU.obj \
    .\mle.obj \
    .\MSGBOX.obj \
    .\pmprint.obj \
    .\POINT.obj \
    .\PROFILE.obj \
    .\SPLITBAR.obj \
    .\CTRLBAR.obj
    @echo " Lib::Library Tool "
    ilib.exe  /nol /noi .\STPMCRD.LIB -+.\help.obj \
                                                        -+.\cnr.obj \
                                                        -+.\control.obj \
                                                        -+.\HOVERBOX.obj \
                                                        -+.\STATBAR.obj \
                                                        -+.\bitmap.obj \
                                                        -+.\WINUTIL.obj \
                                                        -+.\btnbar.obj \
                                                        -+.\CLIP.obj \
                                                        -+.\FONT.obj \
                                                        -+.\menu.obj \
                                                        -+.\mle.obj \
                                                        -+.\MSGBOX.obj \
                                                        -+.\pmprint.obj \
                                                        -+.\POINT.obj \
                                                        -+.\PROFILE.obj \
                                                        -+.\SPLITBAR.obj \
                                                        -+.\CTRLBAR.obj;

.\help.obj: \
    .\help.cpp \
    {.;$(INCLUDE);}stlibpmc.h

.\cnr.obj: \
    .\cnr.cpp \
    {.;$(INCLUDE);}stlibpmc.h

.\control.obj: \
    .\control.cpp \
    {.;$(INCLUDE);}stlibpmc.h

.\CTRLBAR.obj: \
    .\CTRLBAR.cpp \
    {.;$(INCLUDE);}stlibpmc.h

.\HOVERBOX.obj: \
    .\HOVERBOX.cpp \
    {.;$(INCLUDE);}stlibpmc.h

.\STATBAR.obj: \
    .\STATBAR.cpp \
    {.;$(INCLUDE);}stlibpmc.h

.\bitmap.obj: \
    .\bitmap.cpp \
    {.;$(INCLUDE);}stlibpmc.h

.\SPLITBAR.obj: \
    .\SPLITBAR.cpp \
    {.;$(INCLUDE);}stlibpmc.h

.\PROFILE.obj: \
    .\PROFILE.cpp \
    {.;$(INCLUDE);}stlibpmc.h

.\POINT.obj: \
    .\POINT.cpp \
    {.;$(INCLUDE);}stlibpmc.h

.\pmprint.obj: \
    .\pmprint.cpp \
    {.;$(INCLUDE);}stlibpmc.h

.\MSGBOX.obj: \
    .\MSGBOX.cpp \
    {.;$(INCLUDE);}stlibpmc.h

.\mle.obj: \
    .\mle.cpp \
    {.;$(INCLUDE);}stlibpmc.h

.\MENU.obj: \
    .\MENU.cpp \
    {.;$(INCLUDE);}stlibpmc.h

.\FONT.obj: \
    .\FONT.cpp \
    {.;$(INCLUDE);}stlibpmc.h

.\CLIP.obj: \
    .\CLIP.cpp \
    {.;$(INCLUDE);}stlibpmc.h

.\btnbar.obj: \
    .\btnbar.cpp \
    {.;$(INCLUDE);}stlibpmc.h

.\WINUTIL.obj: \
    .\WINUTIL.cpp \
    {.;$(INCLUDE);}stlibpmc.h
