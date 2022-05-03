# Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
#
# $Id: highc_v1.mak 1.1 1997/12/10 16:43:19 jon Exp $
# $Locker: $
#
# General defines, rules and macros for BRender
#
# Metaware High-C Version 1.7
#

# Pick up configuration from environment, if not defined, pick defaults
#
.IMPORT .IGNORE: SERIAL LONG_ARGS 

# Type of executable to produce - debug, profile, or release
#
.IF $(BUILD)X == X
.IMPORT .IGNORE: BUILD
.END

.IF $(BUILD)X == X
BUILD=RELEASE
.END

# Target type
#
.IF $(TARGET_TYPE)X == X
.IMPORT .IGNORE: TARGET_TYPE 
.END

.IF $(TARGET_TYPE)X == X
TARGET_TYPE=LIB
.END

# Library base type
#
.IF $(BASE_TYPE)X == X
.IMPORT .IGNORE: BASE_TYPE 
.END

.IF $(BASE_TYPE)X == X
BASE_TYPE=FLOAT
.END

# Compiler part of library name - High C Version 1
#
LIB_SUFFIX_C=1

# Pick a library suffix based on library type
#
.IF $(BASE_TYPE) == FLOAT
LIB_SUFFIX_T=f
.ELIF $(BASE_TYPE) == FIXED
LIB_SUFFIX_T=x
.ELIF $(BASE_TYPE) == NONE
LIB_SUFFIX_T=
.ELSE
Error - Unknown base type
.END

## Build flags
##
OBJ_EXT:=.obj
LIB_EXT:=.lib
AS:=ml
CC:=hc386x
H2I:=h2inc
MKDEPFLAGS= -s -t -I$(BLD_DIR) $(INCLUDES)

BASE_CFLAGS =-Hon=Pointers_compatible -Hoff=Floating_point -c -D__HIGHC_V1__=1 -DBASED_$(BASE_TYPE)=1 $(INCLUDES)

BASE_ASFLAGS =-nologo -Cp -D__HIGHC_V1__=1 -D__HIGHC__=1 -DBASED_$(BASE_TYPE)=1 -I$(BLD_DIR) $(INCLUDES)

BASE_H2IFLAGS=/nologo /G3 /Zp4 /C /D__HIGHC_V1__  /D__HIGHC__ /D__H2INC__\
 /D_NO_PROTOTYPES -DBASED_$(BASE_TYPE)=1 $(INCLUDES)

.IF $(BUILD) == RELEASE
# Release Flags
#
BUILD_CFLAGS  =-O3 -Hpentium -DDEBUG=0 -DPARANOID=0 -DSTATIC=static
BUILD_LDFLAGS =
BUILD_ASFLAGS =
BUILD_H2IFLAGS=
LIB_SUFFIX_B:=r

.ELIF $(BUILD) == TEST
# Check Flags (Release library with validation checks for debugging user code)
#
BUILD_CFLAGS  =-DDEBUG=0 -DPARANOID=1 -DSTATIC=static
BUILD_LDFLAGS =
BUILD_ASFLAGS =-DDEBUG=0 -DCHECK=1
BUILD_H2IFLAGS=-DDEBUG=0 -DCHECK=1
LIB_SUFFIX_B:=t


.ELIF $(BUILD) == DEBUG
# Debug Flags
#
BUILD_CFLAGS  =-g -DDEBUG=1 -DPARANOID=1 -DSTATIC=
BUILD_LDFLAGS =
BUILD_ASFLAGS =-Zd
BUILD_H2IFLAGS=
LIB_SUFFIX_B:=d

.ELIF $(BUILD) == PROFILE
# Profile Flags
#
BUILD_CFLAGS  =-g -DDEBUG=0 -DPARANOID=0 -DSTATIC=
BUILD_LDFLAGS =
BUILD_ASFLAGS =-Zd
BUILD_H2IFLAGS=
LIB_SUFFIX_B:=p

.ELSE
Error - Unknown build type
.END

## The build directory
##
LIB_TYPE:=$(LIB_SUFFIX_C)$(LIB_SUFFIX_T)$(LIB_SUFFIX_B)$(LIB_SUFFIX)

.IF $(BLD_BASE)X == X
BLD_DIR:=bld_$(LIB_TYPE)
.ELSE
BLD_DIR:=$(BLD_BASE)_$(LIB_TYPE)
.END

## The library name <part><compiler><type><build><optional>.lib
##
LIB_NAME:=$(BASE_DIR)/lib/$(LIB_PART)$(LIB_TYPE).lib

# Keep proper dependencies (per build type)
#
.KEEP_STATE:=$(BLD_DIR)/state.out

## Generic rule for building objects and ASM header files
##
.IF $(LONG_ARGS)X != X

	# Use environment variables to cut down the cmmand line size
	#
$(BLD_DIR)/%.obj : %.c
	$(CC) @$(mktmp,hc386.rsp $(BASE_CFLAGS:s,\,\\,) $(BUILD_CFLAGS:s,\,\\,) $(EXTRA_CFLAGS:s,\,\\,)) -o $(@:s,/,\,) $<

$(BLD_DIR)/%.obj : %.asm
	@[
set ML=$(BASE_ASFLAGS) $(BUILD_ASFLAGS) $(EXTRA_ASFLAGS) -c -Fo$(@:s,/,\,) $<
$(AS)
	]

%.obj : %.c
	@[
	$(CC) @$(mktmp,hc386.rsp $(BASE_CFLAGS:s,\,\\,) $(BUILD_CFLAGS:s,\,\\,) $(EXTRA_CFLAGS:s,\,\\,)) -o $(@:s,/,\,) $<
	]

%.pp : %.c
	@[
set CL=$(BASE_CFLAGS) $(BUILD_CFLAGS) $(EXTRA_CFLAGS) -Fo$(@:s,/,\,) -pc $< 
$(CC)
	]

%.cod : %.c
	@[
set CL=$(BASE_CFLAGS) $(BUILD_CFLAGS) $(EXTRA_CFLAGS) -FAs -Fa$(@:s,/,\,) $< 
$(CC)
	]

%.obj : %.asm
	@[
set ML=$(BASE_ASFLAGS) $(BUILD_ASFLAGS) $(EXTRA_ASFLAGS) -Fo$(@:s,/,\,) $<
$(AS)
	]

$(BLD_DIR)/%.inc : %.h
	@[
set H2INC=$(BASE_H2IFLAGS) $(BUILD_H2IFLAGS) $(EXTRA_H2IFLAGS) /Fa$@ $<
$(H2I) 
	]

$(BLD_DIR)\%.inc : %.h
	@[
set H2INC=$(BASE_H2IFLAGS) $(BUILD_H2IFLAGS) $(EXTRA_H2IFLAGS) /Fa$@ $<
$(H2I) 
	]
.ELSE
	# Pass argumants normally
	#
$(BLD_DIR)/%.obj : %.c
	$(CC) $(BASE_CFLAGS) $(BUILD_CFLAGS) $(EXTRA_CFLAGS) -Fo$(@:s,/,\,) $<

$(BLD_DIR)/%.obj : %.asm
	$(AS) $(BASE_ASFLAGS) $(BUILD_ASFLAGS) $(EXTRA_ASFLAGS) -c -Fo$(@:s,/,\,) $<

%.obj : %.c
	$(CC) $(BASE_CFLAGS) $(BUILD_CFLAGS) $(EXTRA_CFLAGS) -Fo$(@:s,/,\,) $<

%.obj : %.asm
	$(AS) $(BASE_ASFLAGS) $(BUILD_ASFLAGS) $(EXTRA_ASFLAGS) -c -Fo$(@:s,/,\,) $<

$(BLD_DIR)/%.inc : %.h
	$(H2I) $(BASE_H2IFLAGS) $(BUILD_H2IFLAGS) $(EXTRA_H2IFLAGS) /Fa$@ $<

$(BLD_DIR)\%.inc : %.h
	$(H2I) $(BASE_H2IFLAGS) $(BUILD_H2IFLAGS) $(EXTRA_H2IFLAGS) /Fa$@ $<

.END

## The final target
##
OBJS := $(OBJS_C) $(OBJS_ASM)
LDLIBS=$(BASE_LDLIBS) $(EXTRA_LDLIBS)

.IF $(TARGET_TYPE) == LIB

# Library
#
"$(LIB_NAME)": $(OBJS)
	-rm -f $@
	386lib $(@:s,/,\,) -twocase @$(mktmp,386lib.lbc -add $(&:t"\n-add ":s,/,\\,))

.ELIF $(TARGET_TYPE) == EXE_PHARLAP

# Executable - Pharlap TNT
#

BASE_LDFLAGS =-stack 65536
DEBUG_LDFLAGS:=
PROFILE_LDFLAGS:=
RELEASE_LDFLAGS:=
TEST_LDFLAGS:=

BASE_LDLIBS=

LIB_SUFFIX_EXTENDER:=pl

$(TARGET): $(EXTRA_LDLIBS)
$(TARGET): $(OBJS)
	386link @$(BASE_DIR)\hc386.dos -exe $(@:s,/,\,) @$(mktmp,386link.rsp $(<:t"\n":s,\,\\,)\n\
$(BASE_LDFLAGS)\n\
$(BUILD_LDFLAGS)\n\
$(EXTRA_LDFLAGS)\n\
-lib $(LDLIBS:s,\,\\,:s,/,\\,)\n\
)

.END

## Object directory
##
$(BLD_DIR)/exists:
	-mkdir $(BLD_DIR)
	echo $(BLD_DIR) >$(BLD_DIR)\exists
.IF $(LIB_INC)X != X
	$(MAKE) $(MAKEFILE) $(BLD_DIR)/$(LIB_INC).inc
.END



# Dependencies
#
SRCS:=$(OBJS_C:s/.obj/.c/) $(OBJS_ASM:s/.obj/.asm/)

	
depend: $(BLD_DIR)/exists
	sed "s/^$(BLD_DIR)\///" <$(mktmp,_mkdep.tmp $(SRCS:t"\n":s,\,/,)\n) >_mkdep.rsp
	$(MKDEP) $(MKDEPFLAGS) @_mkdep.rsp -o_depend.tmp
	sed "s/^/$(BLD_DIR)\//" <_depend.tmp >$(BLD_DIR)\\depend.out
	$(RM) _mkdep.rsp
	$(RM) _depend.tmp


.INCLUDE .IGNORE: $(BLD_DIR)/depend.out



