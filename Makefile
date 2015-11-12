#############################################################################
# Makefile for building: RNAcomp
# Generated by qmake (3.0) (Qt 5.3.2)
# Project:  src/RNAcomp.pro
# Template: app
# Command: /home/sebastien/Qt5.3.2/5.3/gcc_64/bin/qmake -spec linux-g++ -o Makefile src/RNAcomp.pro
#############################################################################

MAKEFILE      = Makefile

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = 
CFLAGS        = -pipe -O2 -Wall -W -fPIE $(DEFINES)
CXXFLAGS      = -pipe -O2 -std=c++0x -Wall -W -fPIE $(DEFINES)
INCPATH       = -I../../src -I.
LINK          = g++
LFLAGS        = -Wl,-O1 -static-libstdc++
LIBS          = $(SUBLIBS)  
AR            = ar cqs
RANLIB        = 
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = cp -f
COPY_DIR      = cp -f -R
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./build/release/

####### Files

SOURCES       = src/main.cpp \
		src/RnaIndex.cpp \
		src/Query.cpp \
		src/RnaMatch.cpp \
		src/Util.cpp \
		src/RnaDataBase.cpp \
		src/QueryBuilder.cpp \
		src/OptimalQuerySequence.cpp \
		src/RnaAlignment.cpp 
OBJECTS       = ./build/release/main.o \
		./build/release/RnaIndex.o \
		./build/release/Query.o \
		./build/release/RnaMatch.o \
		./build/release/Util.o \
		./build/release/RnaDataBase.o \
		./build/release/QueryBuilder.o \
		./build/release/OptimalQuerySequence.o \
		./build/release/RnaAlignment.o
DIST          = src/main.cpp \
		src/RnaIndex.cpp \
		src/Query.cpp \
		src/RnaMatch.cpp \
		src/Util.cpp \
		src/RnaDataBase.cpp \
		src/QueryBuilder.cpp \
		src/OptimalQuerySequence.cpp \
		src/RnaAlignment.cpp
DESTDIR       = ./build/release/
TARGET        = ./build/release/piccolo


first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)



dist: 
	@test -d .tmp/piccolo1.0.0 || mkdir -p .tmp/piccolo1.0.0
	$(COPY_FILE) --parents $(DIST) .tmp/piccolo1.0.0/ && (cd `dirname .tmp/piccolo1.0.0` && $(TAR) piccolo1.0.0.tar piccolo1.0.0 && $(COMPRESS) piccolo1.0.0.tar) && $(MOVE) `dirname .tmp/piccolo1.0.0`/piccolo1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/piccolo1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


distclean: clean 
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


####### Sub-libraries

check: first

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: 

####### Compile

./build/release/main.o: src/main.cpp src/main.h \
		src/RnaIndex.h \
		src/BlockMeta.h \
		src/QueryResult.h \
		src/Util.h \
		src/RnaDataBase.h \
		src/RnaMatch.h \
		src/OptimalQuerySequence.h \
		src/QueryBuilder.h \
		src/Query.h \
		src/QueryMeta.h \
		src/RnaAlignment.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ./build/release/main.o src/main.cpp

./build/release/RnaIndex.o: src/RnaIndex.cpp src/RnaIndex.h \
		src/BlockMeta.h \
		src/QueryResult.h \
		src/Util.h \
		src/RnaDataBase.h \
		src/Query.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ./build/release/RnaIndex.o src/RnaIndex.cpp

./build/release/Query.o: src/Query.cpp src/Query.h \
		src/Util.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ./build/release/Query.o src/Query.cpp

./build/release/RnaMatch.o: src/RnaMatch.cpp src/RnaMatch.h \
		src/RnaIndex.h \
		src/BlockMeta.h \
		src/QueryResult.h \
		src/Util.h \
		src/RnaDataBase.h \
		src/OptimalQuerySequence.h \
		src/QueryBuilder.h \
		src/Query.h \
		src/QueryMeta.h \
		src/RnaAlignment.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ./build/release/RnaMatch.o src/RnaMatch.cpp

./build/release/Util.o: src/Util.cpp src/Util.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ./build/release/Util.o src/Util.cpp

./build/release/RnaDataBase.o: src/RnaDataBase.cpp src/RnaDataBase.h \
		src/Util.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ./build/release/RnaDataBase.o src/RnaDataBase.cpp

./build/release/QueryBuilder.o: src/QueryBuilder.cpp src/QueryBuilder.h \
		src/BlockMeta.h \
		src/Query.h \
		src/Util.h \
		src/QueryMeta.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ./build/release/QueryBuilder.o src/QueryBuilder.cpp

./build/release/OptimalQuerySequence.o: src/OptimalQuerySequence.cpp src/OptimalQuerySequence.h \
		src/QueryBuilder.h \
		src/BlockMeta.h \
		src/Query.h \
		src/Util.h \
		src/QueryMeta.h \
		src/OptimalQuerySequenceTemplate.h \
		src/VariadicList.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ./build/release/OptimalQuerySequence.o src/OptimalQuerySequence.cpp

./build/release/RnaAlignment.o: src/RnaAlignment.cpp src/RnaAlignment.h \
		src/Util.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ./build/release/RnaAlignment.o src/RnaAlignment.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:
