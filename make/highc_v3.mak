# Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
#
# $Id: highc_v3.mak 1.1 1997/12/10 16:43:20 jon Exp $
# $Locker: $
#
# General defines, rules and macros for BRender
#
# Metaware High-C
#

# Pick up configuration from environment, if not defined, pick defaults
#
.IMPORT .IGNORE: LONG_ARGS 

# Type of executable to produce - debug, profile, or release
#
.IF $(BUILD) == 
.IMPORT .IGNORE: BUILD
.END

BUILD*:=RELEASE

# Target type
#
.IF $(TARGET_TYPE)X == X
.IMPORT .IGNORE: TARGET_TYPE 
.END

TARGET_TYPE*:=LIB

# Extender
#
.IF $(EXTENDER) ==
.IMPORT .IGNORE: EXTENDER 
.END

EXTENDER*:=PHARLAP

# Library base type
#
.IF $(BASE_TYPE) == 
.IMPORT .IGNORE: BASE_TYPE 
.ENDIF

BASE_TYPE*:=FIXED

# Compiler part of library name - High C
#
LIB_SUFFIX_C=h

# Pick a library suffix based on library type
#
.IF $(BASE_TYPE) == FLOAT
LIB_SUFFIX_T=f
.ELIF $(BASE_TYPE) == FIXED
LIB_SUFFIX_T=x
.ELIF $(BASE_TYPE) == NONE
LIB_SUFFIX_T=
.ELSE
Unknown_BASE_TYPE:
.END

## Build flags
##
OBJ_EXT:=.obj
LIB_EXT:=.lib
AS:=ml
CC:=hc386
H2I:=h2inc
MKDEPFLAGS= -s -t -I$(BLD_DIR) $(INCLUDES)
PERL=perl

# Flags for each process are constructed from
# a common prt, a per build part, and a per app. part
#
ASFLAGS=$(BASE_ASFLAGS) $(BUILD_ASFLAGS) $(EXTRA_ASFLAGS)
CFLAGS=$(BASE_CFLAGS) $(BUILD_CFLAGS) $(EXTRA_CFLAGS)
H2IFLAGS=$(BASE_H2IFLAGS) $(BUILD_H2IFLAGS) $(EXTRA_H2IFLAGS)

# Base flags 
#
BASE_CFLAGS = -Hloclib -Hnocopyr -Hmscerr -Hpragma=Align_members(4) -Hoff=Floating_point -c -DBASED_$(BASE_TYPE)=1 $(INCLUDES)

BASE_ASFLAGS =-nologo -Cp -D__HIGHC__=1 -DBASED_$(BASE_TYPE)=1 -I$(BLD_DIR) $(INCLUDES)

BASE_H2IFLAGS=/nologo /G3 /Zp4 /C /D__HIGHC__ /D__H2INC__\
 /D_NO_PROTOTYPES -DBASED_$(BASE_TYPE)=1 $(INCLUDES)

.IF $(BUILD) == RELEASE
# Release Flags
#
BUILD_CFLAGS  =-O7 -Hoff=behaved -DDEBUG=0 -DPARANOID=0 -DSTATIC=static
BUILD_LDFLAGS =$(RELEASE_LDFLAGS)
BUILD_ASFLAGS =
BUILD_H2IFLAGS=
LIB_SUFFIX_B:=r

.ELIF $(BUILD) == TEST
# Check Flags (Release library with validation checks for debugging user code)
#
BUILD_CFLAGS  =-O7 -Hoff=behaved -DDEBUG=0 -DPARANOID=1 -DSTATIC=static
BUILD_LDFLAGS =$(TEST_LDFLAGS)
BUILD_ASFLAGS =-DDEBUG=0 -DCHECK=1
BUILD_H2IFLAGS=-DDEBUG=0 -DCHECK=1
LIB_SUFFIX_B:=t


.ELIF $(BUILD) == DEBUG
# Debug Flags
#
BUILD_CFLAGS  =-g -DDEBUG=1 -DPARANOID=1 -DSTATIC=
BUILD_LDFLAGS =$(DEBUG_LDFLAGS)
BUILD_ASFLAGS =-Zd
BUILD_H2IFLAGS=
LIB_SUFFIX_B:=d

.ELIF $(BUILD) == PROFILE
# Profile Flags
#
BUILD_CFLAGS  =-g -O7 -DDEBUG=0 -DPARANOID=0 -DSTATIC=
BUILD_LDFLAGS =$(PROFILE_LDFLAGS)
BUILD_ASFLAGS =-Zd
BUILD_H2IFLAGS=
LIB_SUFFIX_B:=p

.ELSE
Error - Unknown build type
.END

## The build directory
##
LIB_TYPE:=$(LIB_SUFFIX_C)$(LIB_SUFFIX_T)$(LIB_SUFFIX_B)$(LIB_SUFFIX)

BLD_BASE*:=bld

BLD_DIR:=$(BLD_BASE)_$(LIB_TYPE)

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
	$(CC) @$(mktmp,hc386.rsp $(CFLAGS:s,\,\\,)) -o $(@:s,/,\,) $<

$(BLD_DIR)/%.obj : %.asm
	@[
set ML=$(ASFLAGS) -c -Fo$(@:s,/,\,) $<
$(AS)
	]

%.obj : %.c
	@[
	$(CC) @$(mktmp,hc386.rsp $(CFLAGS:s,\,\\,)) -o $(@:s,/,\,) $<
	]

%.pp : %.c
	@[
set CL=$(CFLAGS) -Fo$(@:s,/,\,) -pc $< 
$(CC)
	]

%.cod : %.c
	@[
set CL=$(CFLAGS) -FAs -Fa$(@:s,/,\,) $< 
$(CC)
	]

%.obj : %.asm
	@[
set ML=$(ASFLAGS) -Fo$(@:s,/,\,) $<
$(AS)
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
$(BLD_DIR)/%.obj : %.c
	$(CC) $(CFLAGS) -Fo$(@:s,/,\,) $<

$(BLD_DIR)/%.obj : %.asm
	$(AS) $(ASFLAGS) -c -Fo$(@:s,/,\,) $<

%.obj : %.c
	$(CC) $(CFLAGS) -Fo$(@:s,/,\,) $<

%.obj : %.asm
	$(AS) $(ASFLAGS) -c -Fo$(@:s,/,\,) $<

$(BLD_DIR)/%.inc : %.h
	$(H2I) $(H2IFLAGS) /Fa$@ $<

$(BLD_DIR)\%.inc : %.h
	$(H2I) $(H2IFLAGS) /Fa$@ $<

.END

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
	386lib $(@:s,/,\,) -twocase @$(mktmp,386lib.lbc -add $(&:t"\n-add ":s,/,\\,))

.ELSE
.IF $(EXTENDER) == PHARLAP

# Executable - Pharlap TNT
#

BASE_LDFLAGS =-stack 65536 -nobanner
DEBUG_LDFLAGS:=-fullsym
PROFILE_LDFLAGS:=-fullsym
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
.ELIF $(EXTENDER) == PHARLAP3

# Executable - Pharlap v3
#
BASE_LDFLAGS:=-stack 65536
DEBUG_LDFLAGS:=-fullsym
PROFILE_LDFLAGS:=-fullsym
RELEASE_LDFLAGS:=
TEST_LDFLAGS:=

BASE_LDLIBS=

LIB_SUFFIX_EXTENDER:=pl

$(TARGET): $(EXTRA_LDLIBS)
$(TARGET): $(OBJS)
	386link -exe $*.exp @$(BASE_DIR)\hc386.pl3 @$(mktmp,386link.rsp $(<:t"\n":s,\,\\,)\n\
$(BASE_LDFLAGS)\n\
$(BUILD_LDFLAGS)\n\
$(EXTRA_LDFLAGS)\n\
-lib $(LDLIBS:s,\,\\,:s,/,\\,)\n\
)
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
	$(MAKE) $(MAKEFILE) $(BLD_DIR)/$(LIB_INC).inc
.ENDIF



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


