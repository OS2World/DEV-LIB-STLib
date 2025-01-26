# Created by IBM WorkFrame/2 MakeMake at 13:25:35 on 23 Nov 1995
#
# The actions included in this make file are:
#  Compile::C++ Compiler
#  Lib::Library Tool
!INCLUDE E:\BIN\H\HEADER.MAK

DIR=d:\source\c\stlibvio
BUILD=STVIOR.LIB
USE_CPP=$(CPP_FLAGS)
USE_LIB=$(LIB_FLAGS)

.SUFFIXES: .CPP .obj

.all: \
    $(DIR)\$(BUILD)

.CPP.obj:
    @echo "C/C++ compiler" 
    $(CC) $(USE_CPP)

$(DIR)\$(BUILD): \
    $(DIR)\vio.obj \
    $(DIR)\vlistbox.obj \
    $(DIR)\veditbtn.obj \
    $(DIR)\vedtbool.obj \
    $(DIR)\veditint.obj \
    $(DIR)\veditstr.obj \
    $(DIR)\vperbox.obj \
    $(DIR)\vmsgbox.obj \
    $(DIR)\vprint.obj \
    $(DIR)\vdesktop.obj \
    $(DIR)\vwindow.obj
    @echo " Lib::Library Tool "
    $(ILIB) $(USE_LIB) $(DIR)\$(BUILD) \
                     -+$(DIR)\vio.obj \
                     -+$(DIR)\vlistbox.obj \
                     -+$(DIR)\veditbtn.obj \
                     -+$(DIR)\vedtbool.obj \
                     -+$(DIR)\veditint.obj \
                     -+$(DIR)\veditstr.obj \
                     -+$(DIR)\vperbox.obj \
                     -+$(DIR)\vmsgbox.obj \
                     -+$(DIR)\vprint.obj \
                     -+$(DIR)\vdesktop.obj \
                     -+$(DIR)\vwindow.obj;

$(DIR)\vio.obj: \
    $(DIR)\vio.cpp \
    {$(DIR);$(INCLUDE);}$(H_VIO)

$(DIR)\vlistbox.obj: \
    $(DIR)\vlistbox.cpp \
    {$(DIR);$(INCLUDE);}$(H_VIO)

$(DIR)\veditbtn.obj: \
    $(DIR)\veditbtn.cpp \
    {$(DIR);$(INCLUDE);}$(H_VIO)

$(DIR)\vedtbool.obj: \
    $(DIR)\vedtbool.cpp \
    {$(DIR);$(INCLUDE);}$(H_VIO)

$(DIR)\veditint.obj: \
    $(DIR)\veditint.cpp \
    {$(DIR);$(INCLUDE);}$(H_VIO)

$(DIR)\veditstr.obj: \
    $(DIR)\veditstr.cpp \
    {$(DIR);$(INCLUDE);}$(H_VIO)

$(DIR)\vperbox.obj: \
    $(DIR)\vperbox.cpp \
    {$(DIR);$(INCLUDE);}$(H_VIO)

$(DIR)\vmsgbox.obj: \
    $(DIR)\vmsgbox.cpp \
    {$(DIR);$(INCLUDE);}$(H_VIO)

$(DIR)\vprint.obj: \
    $(DIR)\vprint.cpp \
    {$(DIR);$(INCLUDE);}$(H_VIO)

$(DIR)\vdesktop.obj: \
    $(DIR)\vdesktop.cpp \
    {$(DIR);$(INCLUDE);}$(H_VIO)

$(DIR)\vwindow.obj: \
    $(DIR)\vwindow.cpp \
    {$(DIR);$(INCLUDE);}$(H_VIO)
