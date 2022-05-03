# Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
#
# $Id: watcom.mak 1.1 1997/12/10 16:43:50 jon Exp $
# $Locker: $
#
# General defines, rules and macros for BRender built with Watcom C32
#

# Compiler part of library name - Watcom
#
LIB_SUFFIX_C*:=w
LIB_SUFFIX_L*:=w

# Default Extender
#
EXTENDER*:=DOS4GW

# Default to register calling
#
.IMPORT .IGNORE: BR_LIB_SUFFIX
BR_LIB_SUFFIX*:=r

.INCLUDE .IGNORE: $(BR_MAKE_DIR)/macros.mak
.IMPORT .IGNORE: WATCOM

CC:=wcc386
CPPC:=wpp386
CPP:=wcl386 -p
LIB:=wlib
LIBFLAGS:=-q -b
LINK:=wlink

# Base flags
#
CFLAGS+= -s -wx -zc -zp4 -5$(BR_LIB_SUFFIX)

ASFLAGS+= -nologo -Cp -D__WATCOMC__=1

H2IFLAGS+= -D__WATCOMC__

# Build specific flags
#
.IF   $(BR_BUILD) == RELEASE
CFLAGS  +=-omaxnet -fp5
.ELIF   $(BR_BUILD) == EVAL
CFLAGS  +=-omaxnet -fp5
.ELIF $(BR_BUILD) == TEST
CFLAGS  +=-omaxnet -fp5
.ELIF $(BR_BUILD) == DEBUG
CFLAGS  +=-d2 -ep -ee -en
ASFLAGS +=-Zd
.ELIF $(BR_BUILD) == PROFILE
CFLAGS  +=-omaxnet -fp5 -d1
ASFLAGS +=-Zd
.END

## Generic rule for building objects and ASM header files
##
.IF $(BR_LONG_ARGS)X != X

$(BLD_DIR)/%.obj : %.c
	$(CC) @$(mktmp,wcl386.rsp $(CFLAGS:s,\,\\,)\n) -fo$(@:s,/,\,) $<

$(BLD_DIR)/%.obj : %.cpp
	$(CPPC) @$(mktmp,wcl386.rsp $(CFLAGS:s,\,\\,)\n) -fo$(@:s,/,\,) $<

$(BLD_DIR)/%.obj : %.asm
	$(AS) @$(mktmp $(ASFLAGS:s,\,\\,) -c -Fo$(@:s,\,\\,:s,/,\\,)) $<

%.obj : %.c
	$(CC) @$(mktmp,wcl386.rsp $(CFLAGS:s,\,\\,)\n) -fo$(@:s,/,\,) $<

%.obj : %.cpp
	$(CPPC) @$(mktmp,wcl386.rsp $(CFLAGS:s,\,\\,)\n) -fo$(@:s,/,\,) $<

%.i : %.c
	$(CC) @$(mktmp,wcl386.rsp $(CFLAGS:s,\,\\,)\n) -fo$(@:s,/,\,) -p $<

%.i : %.cpp
	$(CPPC) @$(mktmp,wcl386.rsp $(CFLAGS:s,\,\\,)\n) -fo$(@:s,/,\,) -p $<

%.ih : %.h
	$(CC) @$(mktmp,wcl386.rsp $(CFLAGS:s,\,\\,)\n) -fo$(@:s,/,\,) -p $<

%.obj : %.asm
	$(AS) @$(mktmp $(ASFLAGS:s,\,\\,) -c -Fo$(@:s,\,\\,:s,/,\\,)) $<

%.lst .PRECIOUS: %.asm
	$(AS) @$(mktmp $(ASFLAGS:s,\,\\,) -c -Fl$(@:s,\,\\,:s,/,\\,)) $<

.ELSE
	# Pass argumants normally
	#
$(BLD_DIR)/%.obj : %.c
	$(CC) $(CFLAGS) -fo$(@:s,/,\,) $<

$(BLD_DIR)/%.obj : %.cpp
	$(CPPC) $(CFLAGS) -fo$(@:s,/,\,) $<

$(BLD_DIR)/%.obj : %.asm
	$(AS) $(ASFLAGS) -c -Fo$(@:s,/,\,) $<

%.obj : %.c
	$(CC) $(CFLAGS) -fo$(@:s,/,\,) $<

%.obj : %.cpp
	$(CPPC) $(CFLAGS) -fo$(@:s,/,\,) $<

%.i : %.c
	$(CC) $(CFLAGS) -fo$(@:s,/,\,) -p $<

%.i : %.cpp
	$(CPPC) $(CFLAGS) -fo$(@:s,/,\,) -p $<

%.ih : %.h
	$(CC) $(CFLAGS) -fo$(@:s,/,\,) -p $<


%.obj : %.asm
	$(AS) $(ASFLAGS) -c -Fo$(@:s,/,\,) $<

.END

.IF $(TARGET_TYPE) == LIB

# Library
#
TARGET:=$(LIB_NAME)
"$(LIB_NAME)": $(OBJS)
	$(LIB) $(LIBFLAGS) $(@:s,/,\\) @$(mktmp,lib.rsp -+$(^:t" -+":s,\,\\,:s,/,\\)\n)

.ELIF $(TARGET_TYPE) == DRIVER

# Driver - Build a Win32 DLL
#
CFLAGS+= -D__DRIVER__
ASFLAGS+= -D__DRIVER__

LDFLAGS+:=option eliminate,quiet,objalign=4k,stub=$(BR_MAKE_DIR:s,\,\\,)\\brdstub.exe
LDFLAGS+:=library $(WATCOM)\\lib386\dos\clib3$(BR_LIB_SUFFIX)
DEBUG_LDFLAGS+:=
PROFILE_LDFLAGS+:=
RELEASE_LDFLAGS+:=
EVAL_LDFLAGS+:=
TEST_LDFLAGS+:=

.IF $(EXPORTS)
LDFLAGS+= @_exports.rsp
.ELSE
LDFLAGS+:=export _BrDrv1Begin
.END

.IF   $(BR_BUILD) == EVAL
DRV_EXT=bed
.ELSE
DRV_EXT=bdd
.END

$(TARGET): $(LDLIBS) $(EXPORTS) $(BR_MAKE_DIR)\expgen.pl
$(TARGET): $(OBJS) $(BR_MAKE_DIR)\expgen.pl
.IF $(EXPORTS)
.IF $(EXPORT_TYPE)
	$(PERL) $(BR_MAKE_DIR)/expgen.pl w $(LIB_SUFFIX_T) < $(EXPORTS) > _exports.rsp
.ELSE
	$(PERL) $(BR_MAKE_DIR)/expgen.pl w < $(EXPORTS) > _exports.rsp
.ENDIF
.END
	$(LINK) @$(mktmp,link.rsp name $@\n\
system nt_dll\n\
$(LDFLAGS)\n\
file $(<:t"\nfile ":s,\,/,)\n\
$(!null,$(LDLIBS) library) $(LDLIBS:t"\nlibrary ":s,\,/,)\n\
)
	$(COPY) $(TARGET) $(BR_TARGET_DIR)\dll\$*.$(DRV_EXT)
	$(RM) _exports.rsp

.ELSE

.IF $(BR_EXTENDER) == DOS4GW

# Executable - DOS4GW
#
LDFLAGS+:=option eliminate,dosseg,caseexact,quiet,stack=128k
DEBUG_LDFLAGS+:=debug all
PROFILE_LDFLAGS+:=debug all
RELEASE_LDFLAGS+:=
EVAL_LDFLAGS+:=
TEST_LDFLAGS+:=

LIB_SUFFIX_EXTENDER:=4g

$(TARGET): $(LDLIBS)
$(TARGET): $(OBJS)
	$(LINK) @$(mktmp,link.rsp name $@\n\
system dos4g\n\
$(LDFLAGS)\n\
file $(<:t"\nfile ":s,\,/,)\n\
library $(LDLIBS:t"\nlibrary ":s,\,/,)\n\
)

.ELIF $(BR_EXTENDER) == DOS4GWPRO

# Executable - DOS4GW Professional
#
LDFLAGS+:=option eliminate,dosseg,caseexact,quiet,stack=128k
DEBUG_LDFLAGS+:=debug all
PROFILE_LDFLAGS+:=debug all
RELEASE_LDFLAGS+:=
EVAL_LDFLAGS+:=
TEST_LDFLAGS+:=

LDLIBS+=

LIB_SUFFIX_EXTENDER:=4g

$(TARGET): $(LDLIBS)
$(TARGET): $(OBJS)
	$(LINK) @$(mktmp,link.rsp name _tmp.exe\n\
system dos4g\n\
$(LDFLAGS)\n\
file $(<:t"\nfile ":s,\,/,)\n\
library $(LDLIBS:t"\nlibrary ":s,\,/,)\n\
)
	4gwbind 4gwpro.exe _tmp.exe $(TARGET) -Q
	$(RM) _tmp.exe

.ELIF $(BR_EXTENDER) == CAUSEWAY

# Executable - Causeway
#
LDFLAGS+:=option eliminate,dosseg,caseexact,quiet,stack=128k
DEBUG_LDFLAGS+:=debug all
PROFILE_LDFLAGS+:=debug all
RELEASE_LDFLAGS+:=
EVAL_LDFLAGS+:=
TEST_LDFLAGS+:=

# Compatible with DOS4G/W
#
LIB_SUFFIX_EXTENDER:=4g

$(TARGET): $(LDLIBS)
$(TARGET): $(OBJS)
	$(LINK) @$(mktmp,link.rsp name $@\n\
system causeway\n\
$(LDFLAGS)\n\
file $(<:t"\nfile ":s,\,/,)\n\
library $(LDLIBS:t"\nlibrary ":s,\,/,)\n\
)

.ELIF $(BR_EXTENDER) == X32

# Executable - Flashtek X-32
#
LDFLAGS+=option eliminate,dosseg,caseexact,quiet
DEBUG_LDFLAGS+=debug all
PROFILE_LDFLAGS+=debug all
RELEASE_LDFLAGS+=
EVAL_LDFLAGS+:=
TEST_LDFLAGS+=

LDLIBS+=

LIB_SUFFIX_EXTENDER:=x3

$(TARGET): $(LDLIBS)
$(TARGET): $(OBJS)
	$(LINK) @$(mktmp,link.rsp name $@\n\
$(LDFLAGS)\n\
system x32$(BR_LIB_SUFFIX)\n\
file $(<:t"\nfile ":s,\,/,)\n\
library $(LDLIBS:t"\nlibrary ":s,\,/,)\n\
)
	x32fix $(TARGET)

.ELIF $(BR_EXTENDER) == X32VM

# Executable - Flashtek X-32 with virtual memory
#
LDFLAGS+:=option eliminate,dosseg,caseexact,quiet
DEBUG_LDFLAGS+:=debug all
PROFILE_LDFLAGS+:=debug all
RELEASE_LDFLAGS+:=
EVAL_LDFLAGS+:=
TEST_LDFLAGS+:=

LDLIBS+=

LIB_SUFFIX_EXTENDER:=x3

$(TARGET): $(LDLIBS)
$(TARGET): $(OBJS)
	$(LINK) @$(mktmp,link.rsp name $@\n\
$(LDFLAGS)\n\
system x32$(BR_LIB_SUFFIX)v\n\
file $(<:t"\nfile ":s,\,/,)\n\
library $(LDLIBS:t"\nlibrary ":s,\,/,)\n\
)
	x32fix $(TARGET)

.ELIF $(BR_EXTENDER) == PHARLAP1

# Executable - Pharlap TNT using 386link
#

LDFLAGS+:=-stack 65536 -offset 16384 -libpath t:\watcom10\lib386,t:\watcom10\lib386\dos 
DEBUG_LDFLAGS+:=-fullsym
PROFILE_LDFLAGS+:=-fullsym

RELEASE_LDFLAGS+:=
EVAL_LDFLAGS+:=
TEST_LDFLAGS+:=

LBLIBS+=

LIB_SUFFIX_EXTENDER:=pl

$(TARGET): $(LDLIBS)
$(TARGET): $(OBJS)
	386link @wc386.dos -exe $(@:s,/,\,) @$(mktmp,386link.rsp $(<:t"\n":s,\,\\,)\n\
$(LDFLAGS)\n\
-lib $(LDLIBS:s,\,\\,:s,/,\\,)\n\
)
#	rebind $(TARGET)

.ELIF $(BR_EXTENDER) == PHARLAP

# Executable - Pharlap TNT using wlink
#

LDFLAGS+:=option eliminate,dosseg,caseexact,quiet
DEBUG_LDFLAGS+:=debug all
PROFILE_LDFLAGS+:=debug all

RELEASE_LDFLAGS+:=
EVAL_LDFLAGS+:=
TEST_LDFLAGS+:=

LBLIBS+=

LIB_SUFFIX_EXTENDER:=pl

$(TARGET): $(LDLIBS)
$(TARGET): $(OBJS)
	$(LINK) @$(mktmp,link.rsp name $@\n\
$(LDFLAGS)\n\
system tnt\n\
file $(<:t"\nfile ":s,\,/,)\n\
library $(LDLIBS:t"\nlibrary ":s,\,/,)\n\
)
	rebind $(TARGET)

.ELIF $(BR_EXTENDER) == PHARLAP3

# Executable - Pharlap v3
#
LDFLAGS+:=-stack 65536 -nobanner -twocase -pack -callbufs 16 -nomap \
-backcompat -libpath t:\watcom10\lib386,t:\watcom10\lib386\dos -lib clib3s,math387s,emu387
DEBUG_LDFLAGS+:=-fullsym
PROFILE_LDFLAGS+:=-fullsym
RELEASE_LDFLAGS+:=
EVAL_LDFLAGS+:=
TEST_LDFLAGS+:=

LBLIBS+=

LIB_SUFFIX_EXTENDER:=p3

$(TARGET): $(LDLIBS)
$(TARGET): $(OBJS)
	386link -exe $*.exp @$(mktmp,386link.rsp $(<:t"\n":s,\,\\,)\n\
$(LDFLAGS)\n\
-lib $(LDLIBS:s,\,\\,:s,/,\\,)\n\
)

.ELIF $(BR_EXTENDER) == OS2

# Executable - OS/2 VIO
#
LDFLAGS+:=option eliminate,dosseg,caseexact,quiet,stack=128k
DEBUG_LDFLAGS+:=debug all
PROFILE_LDFLAGS+:=debug all
RELEASE_LDFLAGS+:=
EVAL_LDFLAGS+:=
TEST_LDFLAGS+:=

LDLIBS+=

$(TARGET): $(LDLIBS)
$(TARGET): $(OBJS)
	$(LINK) @$(mktmp,link.rsp name $@\n\
system os2v2\n\
$(LDFLAGS)\n\
file $(<:t"\nfile ":s,\,/,)\n\
library $(LDLIBS:t"\nlibrary ":s,\,/,)\n\
)

.ELIF $(BR_EXTENDER) == NT

# Executable - NT Console App.
#
LDFLAGS+:=option eliminate,dosseg,caseexact,quiet,stack=128k
DEBUG_LDFLAGS+:=debug all
PROFILE_LDFLAGS+:=debug all
RELEASE_LDFLAGS+:=
EVAL_LDFLAGS+:=
TEST_LDFLAGS+:=

$(TARGET): $(LDLIBS)
$(TARGET): $(OBJS)
	$(LINK) @$(mktmp,link.rsp name $@\n\
system nt\n\
$(LDFLAGS)\n\
file $(<:t"\nfile ":s,\,/,)\n\
library $(LDLIBS:t"\nlibrary ":s,\,/,)\n\
)
.ELSE
Unknown_EXTENDER:
.END

.END

## Build resident images
##
%.h : %.fns $(BR_MAKE_DIR)\resgen.pl
	-$(SET_WRITEABLE) $@
	$(PERL) $(BR_MAKE_DIR)/resgen.pl $@ _img.rsp $(LIB_SUFFIX_C) <$*.fns
	-$(RM) $(DDI_LIB_DIR)\$*$(LIB_SUFFIX_C).lib
	$(LIB) -q -n $(DDI_LIB_DIR)\$*$(LIB_SUFFIX_C).lib @_img.rsp
	$(RM) _img.rsp

## Definitions common to all compilers
##
.INCLUDE .IGNORE: $(BR_MAKE_DIR)/common.mak

## Build resident images
##
.IF $(RES_IMAGE)
"$(DDI_LIB_DIR)\$(RES_IMAGE)$(LIB_SUFFIX_C).lib" $(RES_IMAGE).h .UPDATEALL: $(RES_IMAGE).fns $(BR_MAKE_DIR)\resgen.pl
	-$(SET_WRITEABLE) $(RES_IMAGE).h
	$(PERL) $(BR_MAKE_DIR)/resgen.pl $(RES_IMAGE).h _img.rsp w <$(RES_IMAGE).fns
	-$(RM) $(DDI_LIB_DIR)\$(RES_IMAGE)$(LIB_SUFFIX_C).lib
	$(LIB) -q -n $(DDI_LIB_DIR)\$(RES_IMAGE)$(LIB_SUFFIX_C).lib @_img.rsp
	$(RM) _img.rsp

target: $(DDI_LIB_DIR)\$(RES_IMAGE)$(LIB_SUFFIX_C).lib
.END

## Dependencies
##
.INCLUDE .IGNORE: $(BLD_DIR)/depend.out

