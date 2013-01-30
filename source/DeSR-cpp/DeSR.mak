#####################################
# UNIX Makefile for a UIMACPP annotator
#####################################
CC=		gcc
CCLD=		$(CC)
CXX=		g++
CXXLD=		$(CXX)
CXXLINK=	$(CXXLD) $(LDFLAGS) -o $@

COMPILE=	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEFINITIONS) $(INCLUDES)
CXXCOMPILE=	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEFINITIONS) $(INCLUDES)

LTCOMPILE=	$(CC) $(CPPFLAGS) $(CFLAGS)  -fPIC -DPIC $(DEFINITIONS) $(INCLUDES)
LTCXXCOMPILE=	$(CXX) $(CPPFLAGS) $(CXXFLAGS)  -fPIC -DPIC $(DEFINITIONS) $(INCLUDES)
LTCXXSHARED=	$(CXXLD) -shared   -fPIC -DPIC
LTLINK=		$(LIBTOOL) --mode=link $(CCLD) $(CFLAGS) $(LDFLAGS) -o $@

# name of the annotator to be created
TARGET_FILE=DeSR

# list of user's object files to be linked when building the annotator
OBJS=DeSR.o 

#Use this var to pass additional user-defined parameters to the compiler
USER_CFLAGS= -I$(DESR_HOME)/src -l_desr -Bdynamic

#Use this var to pass additional user-defined parameters to the linker
USER_LINKFLAGS= -l_desr -Bdynamic

# Set DEBUG=1 for a debug build (if not 1 a ship build will result)
DEBUG=1

# Set DLL_BUILD=1 to build an annotator (shared library)
#    if not 1 an executable binary will be built
DLL_BUILD=1

# Generic compiler instructions taken from UimaCpp source examples.
ifeq ($(UIMACPP_HOME),)
  $(error UIMACPP_HOME not set)
endif

ifeq ($(TARGET_FILE),)
  $(error TARGET_FILE not set)
endif

# the suffix for shared libraries
DLL_SUFFIX=so
SHARED_FLAG=-shared
UNAME = $(shell uname -s)
ifeq ($(UNAME), Darwin)
  DLL_SUFFIX=dylib
  SHARED_FLAG=-dynamiclib
  EXTRA_LINK_LIBS=-lapr-1.0
else
  EXTRA_LINK_LIBS=-lapr-1
endif

# Uncomment the next line to build 32-bit annotators on a 64-bit OS
#FORCE_32=-m32

ifeq ($(DEBUG),1)
# compile flags for debug mode
BUILD_CFLAGS=-DDEBUG -g -fno-inline -fno-default-inline -fPIC $(FORCE_32)
BUILD_LFLAGS= $(FORCE_32)
else
# compile flags for ship mode:
# all optimization on, Multithreaded, dynamic link runtime
BUILD_CFLAGS=-DNDEBUG -DTRACEOFF -O3 -fPIC $(FORCE_32)
BUILD_LFLAGS= -Wl,--strip-debug $(FORCE_32)
endif

# include directory for compile
INCLUDES=-I$(UIMACPP_HOME)/include -I$(UIMACPP_HOME)/include/apr-1 -I $(DESR_HOME) -I $(DESR_HOME)/ixe -I $(DESR_HOME)/classifier
#INCLUDES=-I$(UIMACPP_HOME)/include -I$(UIMACPP_HOME)/include/apr-1 

# compiler flags:
CFLAGS=-Wall -x c++ $(BUILD_CFLAGS) $(INCLUDES) $(USER_CFLAGS) \
	 -Wno-deprecated

ifeq ($(DLL_BUILD),1)
DLL_LINKFLAGS=$(SHARED_FLAG)
TARGET_FILE_NAME=$(TARGET_FILE).$(DLL_SUFFIX)
BIN_LINKFLAGS=
else
BIN_LINKFLAGS= -lxerces-c -licuuc -licuio -licui18n -licudata -ldl
TARGET_FILE_NAME=$(TARGET_FILE)
endif

LINKFLAGS= $(DLL_LINKFLAGS) $(BIN_LINKFLAGS) \
	 $(USER_LINKFLAGS) $(BUILD_LFLAGS) \
	 -L$(UIMACPP_HOME)/lib -L$(DESR_HOME)/src -luima $(EXTRA_LINK_LIBS)

# compiler to use
CC=g++
# rules to generate object files from c/c++ source files
.SUFFIXES : .o .c .cpp

.cpp.o :
	$(CC) $(CFLAGS) -c $<

.cpp.o :
	$(CC) $(CFLAGS) -c $<
	
.c.o :
	$(CC) $(CFLAGS) -c $<

	
all:  $(TARGET_FILE_NAME) 

$(TARGET_FILE_NAME):  $(OBJS)
	$(CC)  $(OBJS) \
	$(LINKFLAGS) \
	-o $(TARGET_FILE_NAME)


clean:
	-rm -f $(OBJS)
	-rm -f $(TARGET_FILE_NAME)
	-rm -f $(USER_XTARGET)

