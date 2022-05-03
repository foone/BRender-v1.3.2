# Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
#
# $Id: macros.mak 1.1 1997/12/10 16:43:23 jon Exp $
# $Locker: $
#
# General macros and generic rules for all targets
#

default: target

all: includes depend target

# Pick up configuration from environment, if not defined, pick defaults
#
.IMPORT .IGNORE: BR_LONG_ARGS 

# Type of code to produce - debug, profile, or release
#
.IF $(BR_BUILD) == 
.IMPORT .IGNORE: BR_BUILD 
.ENDIF

BR_BUILD*:=RELEASE

# Target image type - EXE, LIB or DRIVER
#
TARGET_TYPE*:=LIB

# Extender
#
.IF $(BR_EXTENDER) ==
.IMPORT .IGNORE: BR_EXTENDER 
.END

# Library base type
#
.IF $(BR_BASE_TYPE) == 
.IMPORT .IGNORE: BR_BASE_TYPE 
.ENDIF

# Compiler specific type of library
#
.IF $(BR_LIB_SUFFIX) == 
.IMPORT .IGNORE: BR_LIB_SUFFIX
.ENDIF

BR_BASE_TYPE*:=FIXED

# Pick a library suffix based on library type
#
.IF   $(BR_BASE_TYPE) == FLOAT
LIB_SUFFIX_T*:=f
.ELIF $(BR_BASE_TYPE) == FIXED
LIB_SUFFIX_T*:=x
.ELIF $(BR_BASE_TYPE) == NONE
.ENDIF

# File extensions
#
EXE_EXT*:=.exe
OBJ_EXT*:=.obj
LIB_EXT*:=.lib

# Common commands
#
H2I*:=h2inc
PERL*:=perl
AS:=ml
COPY:=cp
COPY_FORCE:=cp -f
RM:=rm -f
RMDIR*:=+del /s/x/q
SET_WRITEABLE*:=chmod +w

# Real mode linker - used to generate real-mode interrupt handler images
#
REAL_LINK*:=link5

# Default include directories
#
INCLUDES+=$(INC_DIR) $(DDI_INC_DIR)

# Base flags
#
CFLAGS+= -DBASED_$(BR_BASE_TYPE)=1 -I$(INCLUDES:t" -I") -D__386__=1

LDFLAGS+=

ASFLAGS+= -DBASED_$(BR_BASE_TYPE)=1 -I$(BLD_DIR) -I$(INCLUDES:t" -I")

H2IFLAGS+=-nologo -G3 -Zp4 -w -C -WIN32 -D__H2INC__ -DBR_TOKEN_ENUM=0\
 -D_NO_PROTOTYPES -DBASED_$(BR_BASE_TYPE)=1 -I$(INCLUDES:t" -I")

.IF   $(BR_BUILD) == RELEASE
# Release Flags
#
CFLAGS  +=-DDEBUG=0 -DPARANOID=0 -DEVAL=0 -DSTATIC=static -DADD_RCS_ID=0
LDFLAGS +=$(RELEASE_LDFLAGS)
ASFLAGS +=-DDEBUG=0 -DPARANOID=0 -DEVAL=0 -DADD_RCS_ID=0
H2IFLAGS+=-DDEBUG=0 -DPARANOID=0 -DEVAL=0
LIB_SUFFIX_B*:=r

.ELIF   $(BR_BUILD) == EVAL
# Release Flags
#
CFLAGS  +=-DDEBUG=0 -DPARANOID=0 -DEVAL=1 -DSTATIC=static -DADD_RCS_ID=0
LDFLAGS +=$(EVAL_LDFLAGS)
ASFLAGS +=-DDEBUG=0 -DPARANOID=0 -DEVAL=1 -DADD_RCS_ID=0
H2IFLAGS+=-DDEBUG=0 -DPARANOID=0 -DEVAL=1
LIB_SUFFIX_B*:=e

.ELIF $(BR_BUILD) == TEST
# Check Flags (Release library with validation checks for debugging user code)
#
CFLAGS  +=-DDEBUG=0 -DPARANOID=1  -DEVAL=0 -DSTATIC=static -DADD_RCS_ID=0
LDFLAGS +=$(TEST_LDFLAGS)
ASFLAGS +=-DDEBUG=0 -DPARANOID=1 -DEVAL=0 -DADD_RCS_ID=0
H2IFLAGS+=-DDEBUG=0 -DPARANOID=1 -DEVAL=0
LIB_SUFFIX_B*:=t


.ELIF $(BR_BUILD) == DEBUG
# Debug Flags
#
CFLAGS  +=-DDEBUG=1 -DPARANOID=1 -DEVAL=0 -DSTATIC= -DADD_RCS_ID=1
LDFLAGS +=$(DEBUG_LDFLAGS)
ASFLAGS +=-DDEBUG=1 -DPARANOID=1 -DEVAL=0 -DADD_RCS_ID=1
H2IFLAGS+=-DDEBUG=1 -DPARANOID=1 -DEVAL=0
LIB_SUFFIX_B*:=d

.ELIF $(BR_BUILD) == PROFILE
# Profile Flags
#
CFLAGS  +=-DDEBUG=0 -DPARANOID=0 -DEVAL=0 -DSTATIC= -DADD_RCS_ID=1
LDFLAGS +=$(PROFILE_LDFLAGS)
ASFLAGS +=-DDEBUG=0 -DPARANOID=0 -DEVAL=0 -DADD_RCS_ID=1
H2IFLAGS+=-DDEBUG=0 -DPARANOID=0 -DEVAL=0
LIB_SUFFIX_B*:=p

.ENDIF

## The build directory
##
LIB_TYPE:=$(LIB_SUFFIX_C)$(LIB_SUFFIX_T)$(LIB_SUFFIX_B)$(BR_LIB_SUFFIX)

LIB_TYPE_NB:=$(LIB_SUFFIX_C)$(LIB_SUFFIX_B)$(BR_LIB_SUFFIX)

LIB_TYPE_EX=$(LIB_SUFFIX_EXTENDER)$(LIB_SUFFIX_C)$(LIB_SUFFIX_B)$(BR_LIB_SUFFIX)

BLD_BASE*:=bld

BLD_DIR*:=$(BLD_BASE)_$(LIB_TYPE)

# Target directories
#
LIB_DIR*=$(BR_TARGET_DIR)\lib
INC_DIR*=$(BR_TARGET_DIR)\inc

DDI_LIB_DIR*=$(BR_TARGET_DIR)\ddi_lib
DDI_INC_DIR*=$(BR_TARGET_DIR)\ddi_inc

## The library name <part><compiler><type><build><optional>.lib
##
LIB_NAME:=$(LIB_DIR)/$(LIB_PART)$(LIB_TYPE)$(LIB_EXT)

# Accumulate all objects
#
OBJS:= $(OBJS_C) $(OBJS_CPP) $(OBJS_ASM) $(OBJS_EXTRA)

# Include file copying rules
#
"$(INC_DIR)/%.h": %.h
	$(COPY_FORCE) $< $@

"$(INC_DIR)/%.hpp": %.hpp
	$(COPY_FORCE) $< $@

"$(DDI_INC_DIR)/%.h": %.h
	$(COPY_FORCE) $< $@

"$(DDI_INC_DIR)/%.hpp": %.hpp
	$(COPY_FORCE) $< $@

# Special rules to build header files containing
# real mode code
#
%.hex: %.asm $(BR_MAKE_DIR)\dumpc.pl $(BR_MAKE_DIR)\maptodef.pl
	$(AS) -c -nologo $*.asm
	$(REAL_LINK) /map $*.obj,_hex.exe,_hex.map;
	$(BR_MAKE_DIR)\exeldr _hex.exe -o _hex.out
	$(PERL) $(BR_MAKE_DIR)/dumpc.pl _hex.out >$@
	$(PERL) $(BR_MAKE_DIR)/maptodef.pl _hex.map >>$@
	$(RM) _hex.*

# Build ASM includes from C headers
#
.IF $(BR_LONG_ARGS)X != X

$(BLD_DIR)\%.inc: %.h $(BR_MAKE_DIR)\h2incfix.pl
	$(BR_MAKE_DIR)\execenv H2INC $(mktmp $(H2IFLAGS:s,\,\\,:s,/,\\,) -Fa_tmp.inc $*.h) h2inc
	$(PERL) $(BR_MAKE_DIR)/h2incfix.pl <_tmp.inc >$(@:s,\,\\,:s,/,\\,)
	$(RM) _tmp.inc

$(BLD_DIR)/%.inc: %.h $(BR_MAKE_DIR)\h2incfix.pl
	$(BR_MAKE_DIR)\execenv H2INC $(mktmp $(H2IFLAGS:s,\,\\,:s,/,\\,) -Fa_tmp.inc $*.h) h2inc
	$(PERL) $(BR_MAKE_DIR)/h2incfix.pl <_tmp.inc >$(@:s,\,\\,:s,/,\\,)
	$(RM) _tmp.inc

.ELSE

$(BLD_DIR)/%.inc: %.h $(BR_MAKE_DIR)\h2incfix.pl
	$(H2I) $(H2IFLAGS) /Fa_tmp.inc  $*.h
	$(PERL) $(BR_MAKE_DIR)/h2incfix.pl <_tmp.inc >$(@:s,\,\\,:s,/,\\,)
	$(RM) _tmp.inc

$(BLD_DIR)\%.inc: %.h $(BR_MAKE_DIR)\h2incfix.pl
	$(H2I) $(H2IFLAGS) /Fa_tmp.inc  $*.h
	$(PERL) $(BR_MAKE_DIR)/h2incfix.pl <_tmp.inc >$(@:s,\,\\,:s,/,\\,)
	$(RM) _tmp.inc
.ENDIF

## Build ASM function prototype files
##
%.h .NOSTATE: %.afn $(BR_MAKE_DIR)\genasmpr.pl
	$(PERL) $(BR_MAKE_DIR)\genasmpr.pl $*.afn >$@


