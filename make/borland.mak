# Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
#
# $Id: borland.mak 1.1 1997/12/10 16:42:51 jon Exp $
# $Locker: $
#
# General defines, rules and macros for BRender built with Borland C++ 5.0 (32 bit)
#

# Compiler part of library name - Borland
#
LIB_SUFFIX_C*:=b
LIB_SUFFIX_L*:=b

# Default Extender
#
EXTENDER*:=PHARLAP

.INCLUDE .IGNORE: $(BR_MAKE_DIR)/macros.mak

CC:=bcc32
CPPC:=bcc32
CPP:=cpp32 -P-
LIB:=tlib
LIBFLAGS:=/P32
LINK:=tlink

# Base flags
#
CFLAGS+= -c -a4 -w-

ASFLAGS+= -nologo -Cp -D__BORLANDC__=1

H2IFLAGS+= -D__BORLANDC__

# Build specific flags
#
.IF   $(BR_BUILD) == RELEASE
CFLAGS  +=-Ox -5
.ELIF   $(BR_BUILD) == EVAL
CFLAGS  +=-Ox -5
.ELIF $(BR_BUILD) == TEST
CFLAGS  +=-Ox -5
.ELIF $(BR_BUILD) == DEBUG
CFLAGS  +=-v
ASFLAGS +=-Zd
.ELIF $(BR_BUILD) == PROFILE
CFLAGS  +=-Ox -5
ASFLAGS +=-Zd
.END

## Generic rule for building objects and ASM header files
##
.IF $(BR_LONG_ARGS)X != X

$(BLD_DIR)/%.obj : %.c
	$(CC) @$(mktmp,bcc32.rsp $(CFLAGS:s,\,\\,)\n) -o$(@:s,/,\,) $<

$(BLD_DIR)/%.obj : %.cpp
	$(CPPC) @$(mktmp,bcc32.rsp $(CFLAGS:s,\,\\,)\n) -o$(@:s,/,\,) $<

$(BLD_DIR)/%.obj : %.asm
	$(AS) @$(mktmp $(ASFLAGS:s,\,\\,) -c -Fo$(@:s,\,\\,:s,/,\\,)) $<

%.obj : %.c
	$(CC) @$(mktmp,bcc32.rsp $(CFLAGS:s,\,\\,)\n) -o$(@:s,/,\,) $<

%.obj : %.cpp
	$(CPPC) @$(mktmp,bcc32.rsp $(CFLAGS:s,\,\\,)\n) -o$(@:s,/,\,) $<

%.i : %.c
	$(CC) @$(mktmp,bcc32.rsp $(CFLAGS:s,\,\\,)\n) -o$(@:s,/,\,) -pc $<

%.ih : %.h
	$(CC) @$(mktmp,bcc32.rsp $(CFLAGS:s,\,\\,)\n) -o$(@:s,/,\,) -pc $<

%.obj : %.asm
	$(AS) @$(mktmp $(ASFLAGS:s,\,\\,) -c -Fo$(@:s,\,\\,:s,/,\\,)) $<

%.lst .PRECIOUS: %.asm
	$(AS) @$(mktmp $(ASFLAGS:s,\,\\,) -c -Fl$(@:s,\,\\,:s,/,\\,)) $<

.ELSE
	# Pass argumants normally
	#
$(BLD_DIR)/%.obj : %.c
	$(CC) $(CFLAGS) -o$(@:s,/,\,) $<

$(BLD_DIR)/%.obj : %.cpp
	$(CC) $(CFLAGS) -o$(@:s,/,\,) $<

$(BLD_DIR)/%.obj : %.asm
	$(AS) $(ASFLAGS) -c -Fo$(@:s,/,\,) $<

%.obj : %.c
	$(CC) $(CFLAGS) -o$(@:s,/,\,) $<

%.obj : %.cpp
	$(CC) $(CFLAGS) -o$(@:s,/,\,) $<

%.i : %.c
	$(CC) $(CFLAGS) -o$(@:s,/,\,) -pc $<

%.ih : %.h
	$(CC) $(CFLAGS) -o$(@:s,/,\,) -pc $<

%.obj : %.asm
	$(AS) $(ASFLAGS) -c -Fo$(@:s,/,\,) $<

%.lst .PRECIOUS: %.asm
	$(AS) $(ASFLAGS) -c -Fl$(@:s,/,\,) $<

.END


.IF $(TARGET_TYPE) == LIB

# Library
#
TARGET:=$(LIB_NAME)
"$(LIB_NAME)": $(OBJS)
	+$(LIB) $(@:s,/,\,) $(LIBFLAGS) @$(mktmp,tlib.rsp -+$(&:t"-+&\n":s,/,\\,))

.ELIF $(TARGET_TYPE) == DRIVER

# Driver - Build a Win32 DLL
#
LDFLAGS+:=option eliminate,quiet,objalign=4k,stub=$(BR_MAKE_DIR:s,\,\\,)\\brdstub.exe
DEBUG_LDFLAGS+:=
PROFILE_LDFLAGS+:=
RELEASE_LDFLAGS+:=
TEST_LDFLAGS+:=

$(TARGET): $(LDLIBS)
$(TARGET): $(OBJS)
	$(LINK) @$(mktmp,link.rsp name $@\n\
system nt_dll\n\
$(LDFLAGS)\n\
file $(<:t"\nfile ":s,\,/,)\n\
library clib3$(BR_LIB_SUFFIX)\n\
$(!null,$(LDLIBS) library) $(LDLIBS:t"\nlibrary ":s,\,/,)\n\
)
	$(COPY) $(TARGET) $(BR_TARGET_DIR)\dll

.ELSE

.IF $(BR_EXTENDER) == PHARLAP

# Executable - Pharlap TNT using 386link
#

LDFLAGS+:=-stack 65536
DEBUG_LDFLAGS+:=
PROFILE_LDFLAGS+:=

RELEASE_LDFLAGS+:=
TEST_LDFLAGS+:=

LBLIBS+=

LIB_SUFFIX_EXTENDER:=pl

$(TARGET): $(LDLIBS)
$(TARGET): $(OBJS)
	386link @bcc32.dos -exe $(@:s,/,\,) @$(mktmp,386link.rsp $(<:t"\n":s,\,\\,)\n\
$(LDFLAGS)\n\
-lib $(LDLIBS:s,\,\\,:s,/,\\,)\n\
)
#	rebind $(TARGET)

.ELIF $(EXTENDER) == POWERPACK

# Executable - Powerpack
#
LDFLAGS+=-Tpe -ax 
DEBUG_LDFLAGS+=-v
PROFILE_LDFLAGS+=-v
RELEASE_LDFLAGS+=
TEST_LDFLAGS+=

LDLIBS:=dpmi32.lib cw32.lib

LIB_SUFFIX_EXTENDER:=pp

$(TARGET): $(LDLIBS)
$(TARGET): $(OBJS)
	tlink32 $(LDFLAGS) \
@$(mktmp,tlink32.rsp \
c0x32.obj+\n\
$(<:t"+\n":s,\,\\,:s,/,\\,)\n\
$(@:s,/,\,)\n\
$(*:s,/,\,)\n\
$(LDLIBS:t"+\n":s,\,\\,:s,/,\\,)\
)

.ELIF $(BR_EXTENDER) == NT

# Executable - NT Console App.
#
LDFLAGS+:=option eliminate,dosseg,caseexact,quiet,stack=128k
DEBUG_LDFLAGS+:=debug all
PROFILE_LDFLAGS+:=debug all
RELEASE_LDFLAGS+:=
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

#%.h : %.fns $(BR_MAKE_DIR)\resgen.pl
#	-$(SET_WRITEABLE) $@
#	$(PERL) $(BR_MAKE_DIR)/resgen.pl $@ _img.rsp $(LIB_SUFFIX_C) <$*.fns
#	-$(RM) $(DDI_LIB_DIR)\$*$(LIB_SUFFIX_C).lib
#	$(LIB) -q -n $(DDI_LIB_DIR)\$*$(LIB_SUFFIX_C).lib @_img.rsp
#	$(RM) _img.rsp

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
	t:\wtc_106\binnt\wlib -q -n $(DDI_LIB_DIR)\$(RES_IMAGE)$(LIB_SUFFIX_C).lib @_img.rsp
	$(RM) _img.rsp

target: $(DDI_LIB_DIR)\$(RES_IMAGE)$(LIB_SUFFIX_C).lib
.END

## Dependencies
##
.INCLUDE .IGNORE: $(BLD_DIR)/depend.out

