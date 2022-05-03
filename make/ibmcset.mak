# Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
#
# $Id: ibmcset.mak 1.1 1997/12/10 16:43:22 jon Exp $
# $Locker: $
#
# General defines, rules and macros for BRender built with IBM CSet++
#

# Pick up configuration from environment, if not defined, pick defaults
#
.IMPORT .IGNORE: SERIAL LONG_ARGS

# Type of executable to produce - debug, profile, or release
#
.IF $(BUILD) == 
.IMPORT .IGNORE: BUILD 
.ENDIF

BUILD*:=RELEASE

# Target type
#
.IF $(TARGET_TYPE) ==
.IMPORT .IGNORE: TARGET_TYPE 
.END

TARGET_TYPE*:=LIB

# Environment
#
.IF $(EXVIRONMENT) ==
.IMPORT .IGNORE: ENVIRONMENT
.END

ENVIRONMENT*:=VIO

# Library base type
#
.IF $(BASE_TYPE) == 
.IMPORT .IGNORE: BASE_TYPE 
.ENDIF

BASE_TYPE*:=FIXED

# Compiler part of library name - IBM CSet++
#
LIB_SUFFIX_C=i

# Pick a library suffix based on library type
#
.IF   $(BASE_TYPE) == FLOAT
LIB_SUFFIX_T=f
.ELIF $(BASE_TYPE) == FIXED
LIB_SUFFIX_T=x
.ELIF $(BASE_TYPE) == NONE
LIB_SUFFIX_T=
.ELSE
Unknown_BASE_TYPE:
.ENDIF

## Build flags
##
OBJ_EXT:=.obj
LIB_EXT:=.lib
AS:=ml
CC:=icc
H2I:=h2inc
MKDEPFLAGS= -s -t -I$(BLD_DIR) $(INCLUDES)
4DOS=+c:\4dos\4dos.com /e:3048 /c

# Flags for each process are constructed from
# a common part, a per build part, and a per app. part
#
ASFLAGS=$(BASE_ASFLAGS) $(BUILD_ASFLAGS) $(EXTRA_ASFLAGS)
CFLAGS=$(BASE_CFLAGS) $(BUILD_CFLAGS) $(EXTRA_CFLAGS)
H2IFLAGS=$(BASE_H2IFLAGS) $(BUILD_H2IFLAGS) $(EXTRA_H2IFLAGS)

# Base flags 
#
BASE_CFLAGS  =-c -Sp4 -G5 -Gm+ -DBASED_$(BASE_TYPE)=1 $(INCLUDES)

BASE_ASFLAGS =-nologo -Cp -D__IBMC__=1\
 -DBASED_$(BASE_TYPE)=1 -I$(BLD_DIR) $(INCLUDES)

BASE_H2IFLAGS=/nologo /G3 /Zp4 /C /D__IBMC__ /D__H2INC__\
 /D_NO_PROTOTYPES -DBASED_$(BASE_TYPE)=1  $(INCLUDES)

.IF   $(BUILD) == RELEASE
# Release Flags
#
BUILD_CFLAGS  =-O+ -Gf+ -DDEBUG=0 -DPARANOID=0 -DSTATIC=static
BUILD_LDFLAGS =$(RELEASE_LDFLAGS)
BUILD_ASFLAGS =
BUILD_H2IFLAGS=
LIB_SUFFIX_B:=r

.ELIF $(BUILD) == TEST
# Check Flags (Release library with validation checks for debugging user code)
#
BUILD_CFLAGS  =-O+ -Gf+ -DDEBUG=0 -DPARANOID=1 -DSTATIC=static
BUILD_LDFLAGS =$(TEST_LDFLAGS)
BUILD_ASFLAGS =-DDEBUG=0 -DCHECK=1
BUILD_H2IFLAGS=-DDEBUG=0 -DCHECK=1
LIB_SUFFIX_B:=t


.ELIF $(BUILD) == DEBUG
# Debug Flags
#
BUILD_CFLAGS  =-Ti -DDEBUG=1 -DPARANOID=1 -DSTATIC=
BUILD_LDFLAGS =$(DEBUG_LDFLAGS)
BUILD_ASFLAGS =-Zd
BUILD_H2IFLAGS=
LIB_SUFFIX_B:=d

.ELIF $(BUILD) == PROFILE
# Profile Flags
#
BUILD_CFLAGS  =-O+ -Ti -DDEBUG=0 -DPARANOID=0 -DSTATIC=
BUILD_LDFLAGS =$(PROFILE_LDFLAGS)
BUILD_ASFLAGS =-Zd
BUILD_H2IFLAGS=
LIB_SUFFIX_B:=p

.ELSE
Unknown_BASE_TYPE:
.ENDIF

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
	@[
	set _ARGS=$(CFLAGS) -fo$(@:s,/,\,) $<
	$(CC) @_ARGS
	]

$(BLD_DIR)/%.obj : %.asm
	@[
	set ML=$(ASFLAGS) -c -Fo$(@:s,/,\,) $<
	$(AS)
	]

%.obj : %.c
	@[
	set _ARGS=$(CFLAGS) -fo$(@:s,/,\,) $< 

	$(CC) @_ARGS
	]

%.pp : %.c
	@[
	set _ARGS=$(CFLAGS) -fo$(@:s,/,\,) -p $< 
	$(CC) @_ARGS
	]

%.pp : %.h
	@[
	set _ARGS=$(CFLAGS) -D_NO_PROTOTYPES -fo$(@:s,/,\,) -p $< 
	$(CC) @_ARGS
	]

%.obj : %.asm
	@[
	set ML=$(ASFLAGS) -c -Fo$(@:s,/,\,) $<
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
	$(CC) $(CFLAGS) -fo$(@:s,/,\,) $<

$(BLD_DIR)/%.obj : %.asm
	$(AS) $(ASFLAGS) -c -Fo$(@:s,/,\,) $<

%.obj : %.c
	$(CC) $(CFLAGS) -fo$(@:s,/,\,) $<

%.pp : %.c
	$(CC) $(CFLAGS) -fo$(@:s,/,\,) -p $<

%.pp : %.h
	$(CC) $(CFLAGS) -fo$(@:s,/,\,) -p $<


%.obj : %.asm
	$(AS) $(ASFLAGS) -c -Fo$(@:s,/,\,) $<

$(BLD_DIR)/%.inc : %.h
	cp $(mktmp set H2INC=$(H2IFLAGS:s,\,\\,) /Fa$(@:s,\,\\,) $<\n\
	$(H2I)) _h2inc.bat
	$(4DOS) _h2inc.bat

$(BLD_DIR)\%.inc : %.h
	cp $(mktmp set H2INC=$(H2IFLAGS:s,\,\\,) /Fa$(@:s,\,\\,) $<\n\
	$(H2I)) _h2inc.bat
	$(4DOS) _h2inc.bat

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
"$(LIB_NAME)": $(OBJS) $(OBJS_EXTRA)
	-rm -f $@
	lib /nologo /noi $(@:s,/,\,) @$(mktmp +$(&:t"&\n+":s,\,\\,:s,/,\\,));

.ELSE

.IF $(ENVIRONMENT) == VIO

# Executable - VIO
#
BASE_LDFLAGS:=
DEBUG_LDFLAGS:=
PROFILE_LDFLAGS:=
RELEASE_LDFLAGS:=
TEST_LDFLAGS:=

BASE_LDLIBS=

LIB_SUFFIX_EXTENDER:=vi

$(TARGET): $(EXTRA_LDLIBS)
$(TARGET): $(OBJS)
	wlink @$(mktmp,link.rsp name $@\n\
system dos4g\n\
$(BASE_LDFLAGS)\n\
$(BUILD_LDFLAGS)\n\
$(EXTRA_LDFLAGS)\n\
file $(<:t"\nfile ":s,\,/,)\n\
library $(LDLIBS:t"\nlibrary ":s,\,/,)\n\
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
	%$(MAKE) $(MAKEFILE) $(BLD_DIR)/$(LIB_INC).inc
.ENDIF



# Dependencies
#
SRCS:=$(OBJS_C:s/.obj/.c/) $(OBJS_ASM:s/.obj/.asm/)

depend: $(BLD_DIR)/exists
	
depend:
	sed "s/^$(BLD_DIR)\///" <$(mktmp,_mkdep.tmp $(SRCS:t"\n":s,\,/,)\n) >_mkdep.rsp
	$(MKDEP) $(MKDEPFLAGS) @_mkdep.rsp -o_depend.tmp
	sed "s/^/$(BLD_DIR)\//" <_depend.tmp >$(BLD_DIR)\\depend.out
	$(RM) _mkdep.rsp
	$(RM) _depend.tmp

.INCLUDE .IGNORE: $(BLD_DIR)/depend.out

