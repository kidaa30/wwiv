##############################################################################
#
# WWIV 5.0 MAKEFILE for GCC
# Copyright (c) 2000 WWIV Software Services
# All Rights Reserved.
#
# Written by Rushfan
#
# Modified for Linux by Ben "Knight Walker" Carner
#

##############################################################################
#
# This part of the makefile should not need to be changed unless files
# are added or removed.  All the customization settings are above.
#

ifneq (,$(findstring win32,$(MAKEFLAGS)))
OS=WIN32
PLATFORM_CFLAGS=
PLATFORM_DIR=platform/WIN32
PATH_SEPERATOR=;
EXE_EXT         = .exe
OBJ_EXT         = .o
else
OS=UNIX
# no need to define _UNIX since platform/testos.h does it now if "linux" is defined
PLATFORM_CFLAGS=
PLATFORM_DIR=platform/linux
PATH_SEPERATOR=:
EXE_EXT         = 
OBJ_EXT         = .o
endif

CC              = g++

OBJ		= ../obj
BIN		= ../bin

BIN_NAME        = $(BIN)/nodemgr$(EXE_EXT)
RM		= rm
MKDIR		= mkdir
CFLAGS		= -I../ -I../platform -I../$(PLATFORM_DIR) -Wall -g $(PLATFORM_CFLAGS)
LINKFLAGS	= -lm
PROJECT_DIR	= wwiv50
TAR		= tar czvf 

##############################################################################
#
# Implicit Rules
#
#
#
# This whole substitution bit is because gcc doesn't like files
# with a 'CPP' extension, but it does like 'cpp'
#
#
#.SUFFIXES:
#.SUFFIXES: .o .c .cpp .C .CPP
#

$(OBJ)/%.o : ${PLATFORM_DIR}/%.cpp
	@echo Compiling $<
	@$(CC) -c $< $(CFLAGS) -o $@

$(OBJ)/%.o : platform/%.cpp
	@echo Compiling $<
	@$(CC) -c $< $(CFLAGS) -o $@

$(OBJ)/%.o : %.cpp
	@echo Compiling $<
	@$(CC) -c $< $(CFLAGS) -o $@

#############################################################################
#
# Target "ALL" Rebuilds WWIV
#

COMMON_BBS_OBJS =	       	\
	$(OBJ)/main.o      	\

PLATFORM_UNIX_OBJS =			\

PLATFORM_WIN32_OBJS =		\

BBS_OBJS = $(COMMON_BBS_OBJS) $(PLATFORM_$(OS)_OBJS)
BBS_SOURCE =	$(patsubst %.o, %.cpp, $(BBS_OBJS))

###############################################################################
#
# Makefile Targets
#
#
#

all: $(BIN_NAME)

$(OBJ):
	-$(MKDIR) $(OBJ)

$(BIN):
	-$(MKDIR) $(BIN)

$(BIN_NAME): $(BIN) $(OBJ) $(BBS_OBJS)
	@echo Linking $@
	@$(CC) -o $(BIN_NAME) $(LINKFLAGS) $(BBS_OBJS)

clean:
	@echo Cleaning...
	-$(RM) -f $(BIN_NAME)
	-$(RM) -f $(BBS_OBJS)
	-$(RM) -f make.log
	-$(RM) -f *~

spotless:
	-$(RM) -rf $(BIN)/
	-$(RM) -rf $(OBJ)/
