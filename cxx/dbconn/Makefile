####### Compiler, tools and options

CC       = gcc
CXX      = g++
CFLAGS   = -pipe -Wall -W -march=i686 -mtune=i686 -g -DGLX_GLXEXT_LEGACY -fno-use-cxa-atexit -fexceptions -DLINUX
CXXFLAGS = -pipe -Wall -W -march=i686 -mtune=i686 -g -DGLX_GLXEXT_LEGACY -fno-use-cxa-atexit -fexceptions -DLINUX
INCPATH  = -I. -I$(ESUITE_HOME)/includes -I$(ESUITE_HOME)/includes/OCCI
LINK     = g++
LFLAGS   =
LIBS     = -L$(ESUITE_HOME)/lib -L$(HOME)/instantclient_10_2 \
		   -lEAlgorithm -locci -lclntsh
AR       = ar cqs
RANLIB   = 
TAR      = tar -cf
GZIP     = gzip -9f
COPY     = cp -f
COPY_FILE= $(COPY) -p
COPY_DIR = $(COPY) -pR
DEL_FILE = rm -f
SYMLINK  = ln -sf
DEL_DIR  = rmdir
MOVE     = mv -f
CHK_DIR_EXISTS= test -d
MKDIR    = mkdir -p

####### Output directory

OBJECTS_DIR = 

####### Files

HEADERS = 
SOURCES = 
OBJECTS = main.o \
          SysConfig.o \
          logger.o \
		  EDatabase.o \
          Auditor.o
DESTDIR  = 
TARGET   = dbconn

first: all
####### Implicit rules

.SUFFIXES: .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules

all: Makefile $(TARGET)

$(TARGET): $(OBJECTS) 
	$(LINK) $(LFLAGS) -o $@ $^ $(LIBS)

clean:
	-$(DEL_FILE) $(TARGET) $(OBJECTS) 
	-$(DEL_FILE) *~ core *.core


####### Install

install: all 

uninstall: 

