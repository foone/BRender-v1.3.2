# Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
#
# $Id: djgpp.mak 1.1 1997/12/10 16:43:02 jon Exp $
# $Locker: $
#
# General defines, rules and macros for BRender built with DJGPP GNU C
#

# Pick up configuration from environment, if not defined, pick defaults
#
.IMPORT .IGNORE: LONG_ARGS

# Type of executable to produce - debug, profile, or release
#
.IF $(BUILD) == 
.IMPORT .IGNORE: BUILD 
.ENDIF

BUILD*:=RELEASE

# Extender
#
.IF $(EXTENDER) ==
.IMPORT .IGNORE: EXTENDER 
.END

EXTENDER*:=GO32

# Library base type
#
.IF $(BASE_TYPE) ==
.IMPORT .IGNORE: BASE_TYPE 
.ENDIF

BASE_TYPE*:=FIXED

# Compiler part of library name - Watcom
#
LIB_SUFFIX_C=g

# Pick a library suffix based on library type
#
.IF   $(BASE_TYPE) == FLOAT
LIB_SUFFIX_T=f
.ELIF $(BASE_TYPE) == FIXED
LIB_SUFFIX_T=x
.ELIF $(BASE_TYPE) == NONE
LIB_SUFFIX_T=
.ELSE
Unknown_BASE_TYPE
.ENDIF

## Build flags
##
OBJ_EXT:=.o
LIB_EXT:=.a
AS:=ml
CC:=gccrm
LD:=gcc
H2I:=h2inc
MKDEPFLAGS= -s -t -I$(BLD_DIR) $(INCLUDES)
PERL=perl
GNUAS=as
WDISASM=wdisasm

# Flags for each process are constructed from
# a common prt, a per build part, and a per app. part
#
ASFLAGS=$(BASE_ASFLAGS) $(BUILD_ASFLAGS) $(EXTRA_ASFLAGS)
CFLAGS=$(BASE_CFLAGS) $(BUILD_CFLAGS) $(EXTRA_CFLAGS)
H2IFLAGS=$(BASE_H2IFLAGS) $(BUILD_H2IFLAGS) $(EXTRA_H2IFLAGS)

# Base flags 
#
BASE_CFLAGS  =-c -m486 -DBASED_$(BASE_TYPE)=1 $(INCLUDES:s,\,/,)
BASE_ASFLAGS =-nologo -Cp -D__GNUC__=1 -DBASED_$(BASE_TYPE)=1 -I$(BLD_DIR) $(INCLUDES)
BASE_H2IFLAGS=/nologo /G3 /Zp4 /C /D__GNUC__ /D__H2INC__ /D_NO_PROTOTYPES -DBASED_$(BASE_TYPE)=1 $(INCLUDES)

.IF   $(BUILD) == RELEASE
# Release Flags
#
BUILD_CFLAGS  =-O2 -DDEBUG=0  -DPARANOID=0 -DSTATIC=static
BUILD_LDFLAGS =$(RELEASE_LDFLAGS)
BUILD_ASFLAGS =
BUILD_H2IFLAGS=
LIB_SUFFIX_B:=r

.ELIF $(BUILD) == TEST
# Test Flags
#
BUILD_CFLAGS  =-g -DDEBUG=0 -DPARANOID=1 -DSTATIC=
BUILD_LDFLAGS =$(TEST_LDFLAGS)
BUILD_ASFLAGS =
BUILD_H2IFLAGS=
LIB_SUFFIX_B:=d

.ELIF $(BUILD) == DEBUG
# Debug Flags
#
BUILD_CFLAGS  =-g -DDEBUG=1  -DPARANOID=1 -DSTATIC=
BUILD_LDFLAGS =$(DEBUG_LDFLAGS)
BUILD_ASFLAGS =
BUILD_H2IFLAGS=
LIB_SUFFIX_B:=d

.ELIF $(BUILD) == PROFILE
# Profile Flags
#
BUILD_CFLAGS  =-O2 -pg -DDEBUG=0  -DPARANOID=0 -DSTATIC=
BUILD_LDFLAGS =$(PROFILE_LDFLAGS)
BUILD_ASFLAGS =
BUILD_H2IFLAGS=
LIB_SUFFIX_B:=p

.ELSE
Error - Unknown build type
.ENDIF

## The build directory
##
LIB_TYPE:=$(LIB_SUFFIX_C)$(LIB_SUFFIX_T)$(LIB_SUFFIX_B)$(LIB_SUFFIX)

BLD_BASE*:=bld

BLD_DIR:=$(BLD_BASE)_$(LIB_TYPE)

## The library name <part><compiler><type><build><optional>.a
##
LIB_NAME:=$(BASE_DIR)/lib/$(LIB_PART)$(LIB_TYPE).a

# Keep proper dependencies (per build type)
#
.KEEP_STATE:=$(BLD_DIR)/state.out

## Generic rule for building objects and ASM header files
##
.IF $(LONG_ARGS)X != X

	# Use environment variables to cut down the cmmand line size
	#
$(BLD_DIR)/%.o : %.c
	$(CC) @$(mktmp $(CFLAGS)) -o $(@:s,\,/,) $<

$(BLD_DIR)/%.o : %.asm
	@[
	set ML=$(ASFLAGS) -c -Fo$(TMP)tmp.obj $<
	$(AS)
	$(WDISASM) -au -l=tmp.lst $(TMP)tmp.obj
	$(PERL) $(BASE_DIR)/bin/wat2gas.pl $(TMP)tmp.lst  >$(TMP)tmp.s
	$(GNUAS) $(TMP)tmp.s -o $(@:s,/,\,)
	$(RM) $(TMP)tmp.obj
	$(RM) $(TMP)tmp.lst
	$(RM) $(TMP)tmp.s
	]

%.o : %.c
	$(CC) @$(mktmp $(CFLAGS)) -o $(@:s,\,/,) $<

%.s : %.c
	$(CC) @$(mktmp $(CFLAGS)) -S -o $(@:s,\,/,) $<

%.o : %.asm
	@[
	set ML=$(ASFLAGS) -c -Fo$(TMP)tmp.obj $<
	$(AS)
	$(WDISASM) -au -l=tmp.lst $(TMP)tmp.obj
	$(PERL) $(BASE_DIR)/bin/wat2gas.pl $(TMP)tmp.lst  >$(TMP)tmp.s
	$(GNUAS) $(TMP)tmp.s -o $(@:s,/,\,)
	$(RM) $(TMP)tmp.obj
	$(RM) $(TMP)tmp.lst
	$(RM) $(TMP)tmp.s
	]

$(BLD_DIR)/%.inc : %.h
	@[
	set H2INC=$(H2IFLAGS) /Fa$@ $<
	$(H2I) 
	]

$(BLD_DIR)\%.inc : %.h
	@[
	set H2INC=$(H2IFLAGS) /Fa$@ $<
	$(H2I) 
	]
.ELSE
	# Pass argumants normally
	#
$(BLD_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -o$(@:s,\,/,) $<

$(BLD_DIR)/%.o : %.asm
	$(AS) $(ASFLAGS) -c -coff -Fo$@ $<

%.o : %.c
	$(CC) $(CFLAGS) -o$(@:s,\,/,) $<

%.o : %.asm
	$(AS) $(ASFLAGS) -c -Fo$(TMP)tmp.obj $<
	$(WDISASM) -au -l=tmp.lst $(TMP)tmp.obj
	$(PERL) $(BASE_DIR)/bin/wat2gas.pl $(TMP)tmp.lst  >$(TMP)tmp.s
	$(GNUAS) $(TMP)tmp.s -o $(@:s,/,\,)
	$(RM) $(TMP)tmp.obj
	$(RM) $(TMP)tmp.lst
	$(RM) $(TMP)tmp.s

$(BLD_DIR)/%.inc : %.h
	$(H2I) $(H2IFLAGS) /Fa$@ $<

$(BLD_DIR)\%.inc : %.h
	$(H2I) $(H2IFLAGS) /Fa$@ $<

.ENDIF

## Build ASM function prototype files
##
%.h .NOSTATE: %.fns
	$(PERL) $(BASE_DIR)\bin\genasmpr.pl $< >$@

## The final target
##
OBJS := $(OBJS_C) $(OBJS_ASM)
LDLIBS=$(BASE_LDLIBS) $(EXTRA_LDLIBS)

.IF $(TARGET_TYPE) == LIB

# Library
#
"$(LIB_NAME)": $(OBJS)
	-rm -f $@
	t:\djgpp112\bin\ar -rvs $(@:s,\,/,) @$(mktmp $(OBJS)\n)

.ELSE
.IF $(EXTENDER) == GO32

# Executable - GO32 extender
#
BASE_LDFLAGS =
DEBUG_LDFLAGS:=-g
PROFILE_LDFLAGS:=-g
RELEASE_LDFLAGS:=
TEST_LDFLAGS:=

BASE_LDLIBS=-lpc -lm

LIB_SUFFIX_EXTENDER:=dj

$(TARGET): $(EXTRA_LDLIBS)
$(TARGET): $(OBJS)
	$(LD) $(BASE_LDFLAGS) $(BUILD_LDFLAGS) \
@$(mktmp $(OBJS:t"\n")\n$(LDLIBS:s,\,/,:t"\n")\n) $(EXTRA_LDFLAGS) -o $*
	coff2exe $*
.ELSE
Unknown_EXTENDER:
.ENDIF
.ENDIF

## Object directory
##
$(BLD_DIR)/exists:
	-mkdir $(BLD_DIR)
	echo $(BLD_DIR) >$(BLD_DIR)\exists
.IF $(LIB_INC)
	%$(MAKE) $(MAKEFILE) $(BLD_DIR)/$(LIB_INC).inc
.ENDIF

# Dependencies
#
SRCS:=$(OBJS_C:s/.o/.c/) $(OBJS_ASM:s/.o/.asm/)

depend: $(BLD_DIR)/exists
	
depend:
	sed "s/^$(BLD_DIR)\///" <$(mktmp,_mkdep.tmp $(SRCS:t"\n":s,\,/,)\n) >_mkdep.rsp
	$(MKDEP) $(MKDEPFLAGS) @_mkdep.rsp -o_depend.tmp
	sed "s/^/$(BLD_DIR)\//;s/\.obj/\.o/" <_depend.tmp >$(BLD_DIR)\\depend.out
	$(RM) _mkdep.rsp
	$(RM) _depend.tmp

.INCLUDE .IGNORE: $(BLD_DIR)/depend.out

