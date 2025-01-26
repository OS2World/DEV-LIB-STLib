# Created by IBM WorkFrame/2 MakeMake at 13:25:35 on 23 Nov 1995
#
# The actions included in this make file are:
#  Compile::C++ Compiler
#  Lib::Library Tool
!INCLUDE C:\BIN\H\HEADER.MAK

DIR=d:\source\c\stlibstc
BUILD=STSTCD.LIB
USE_CPP=$(D_CPP_FLAGS)
USE_LIB=$(LIB_FLAGS)

.SUFFIXES: .cpp .obj

.all: \
    $(DIR)\$(BUILD)

.cpp.obj:
    @echo " Compile::C++ Compiler "
    $(CC) $(USE_CPP)

$(DIR)\$(BUILD): \
    $(DIR)\dbf.obj \
    $(DIR)\ARRAY.obj \
    $(DIR)\std.obj \
    $(DIR)\serial.obj \
    $(DIR)\BTISAM.obj \
    $(DIR)\BTRIVE32.obj \
    $(DIR)\DATETIME.obj \
    $(DIR)\file.obj \
    $(DIR)\memo.obj
    @echo " Lib::Library Tool "
    $(ILIB) $(USE_LIB) $(DIR)\$(BUILD) \
                       -+$(DIR)\array.obj \
                       -+$(DIR)\dbf.obj \
                       -+$(DIR)\std.obj \
                       -+$(DIR)\serial.obj \
                       -+$(DIR)\BTISAM.obj \
                       -+$(DIR)\BTRIVE32.obj \
                       -+$(DIR)\DATETIME.obj \
                       -+$(DIR)\file.obj \
                       -+$(DIR)\memo.obj;

$(DIR)\std.obj: \
    $(DIR)\std.cpp \
    {$(DIR);$(INCLUDE);}$(H_STD)

$(DIR)\serial.obj: \
    $(DIR)\serial.cpp \
    {$(DIR);$(INCLUDE);}$(H_STD)

$(DIR)\memo.obj: \
    $(DIR)\memo.cpp \
    {$(DIR);$(INCLUDE);}$(H_STD)

$(DIR)\file.obj: \
    $(DIR)\file.cpp \
    {$(DIR);$(INCLUDE);}$(H_STD)

$(DIR)\DATETIME.obj: \
    $(DIR)\DATETIME.cpp \
    {$(DIR);$(INCLUDE);}$(H_STD)

$(DIR)\BTRIVE32.obj: \
    $(DIR)\BTRIVE32.cpp \
    {$(DIR);$(INCLUDE);}$(H_STD)

$(DIR)\BTISAM.obj: \
    $(DIR)\BTISAM.cpp \
    {$(DIR);$(INCLUDE);}$(H_STD) \
    {$(DIR);$(INCLUDE);}$(H_BTR)

$(DIR)\ARRAY.obj: \
    $(DIR)\ARRAY.cpp \
    {$(DIR);$(INCLUDE);}$(H_STD)

$(DIR)\dbf.obj: \
    $(DIR)\dbf.cpp \
    {$(DIR);$(INCLUDE);}$(H_STD) \
    {$(DIR);$(INCLUDE);}$(H_DBF)
