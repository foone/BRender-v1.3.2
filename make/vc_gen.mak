# Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
#
# $Id: vc_gen.mak 1.2 1997/12/11 14:52:20 jon Exp jon $
# $Locker: jon $
#
# Shared makefile for all VC++ derivate targets (VC, Bounds checker, Proton)
#

# Default Extender
#
EXTENDER*:=PHARLAP

# At present debug builds of drivers do not work, so don't let them happen!
#
.IF $(TARGET_TYPE) == DRIVER
BR_BUILD:=RELEASE
.ENDIF

.INCLUDE .IGNORE: $(BR_MAKE_DIR)/macros.mak

# Base flags
#
CFLAGS+= -c -nologo -Zp4 -G5 -Gz -QIfdiv- -D__VISUALC__=1

ASFLAGS+= -coff -nologo -Cp -D__VISUALC__=1

H2IFLAGS+= -D_WIN32 -D__VISUALC__

# Build dependant flags
#
.IF   $(BR_BUILD) == RELEASE
CFLAGS  +=-Ox -Ob2
.ELIF   $(BR_BUILD) == EVAL
CFLAGS  +=-Ox -Ob2
.ELIF $(BR_BUILD) == TEST
CFLAGS  +=-G5
.ELIF $(BR_BUILD) == DEBUG
CFLAGS  +=-Z7
ASFLAGS +=-Zi
.ELIF $(BR_BUILD) == PROFILE
CFLAGS  +=-Z7 -Ox -Ob2
ASFLAGS +=-Zd
.ENDIF

# Extra flags if linking against DLL version of C runtime
#
.IF $(BR_LIB_SUFFIX) == d
.IF $(BR_BUILD) == DEBUG
CFLAGS +=-MDd
.ELIF $(BR_BUILD) == PROFILE
CFLAGS +=-MDd
.ELSE
CFLAGS +=-MD
.ENDIF
.ENDIF

## Generic rule for building objects and ASM header files
##
.IF $(BR_LONG_ARGS)X != X

$(BLD_DIR)/%.obj : %.c
	$(CC) @$(mktmp,cl.rsp $(CFLAGS:s,\,\\,)\n) -Fo$(@:s,/,\,) $<

$(BLD_DIR)/%.obj : %.cpp
	$(CPPC) @$(mktmp,cl.rsp $(CFLAGS:s,\,\\,)\n) -Fo$(@:s,/,\,) $<

$(BLD_DIR)/%.obj : %.asm
	$(AS) @$(mktmp $(ASFLAGS:s,\,\\,) -c -Fo$(@:s,\,\\,:s,/,\\,)) $<

%.obj : %.c
	$(CC) @$(mktmp,cl.rsp $(CFLAGS:s,\,\\,)\n) -Fo$(@:s,/,\,) $<

%.obj : %.cpp
	$(CPPC) @$(mktmp,cl.rsp $(CFLAGS:s,\,\\,)\n) -Fo$(@:s,/,\,) $<

%.i : %.c
	$(CC) @$(mktmp,cl.rsp $(CFLAGS:s,\,\\,)\n) -Fo$(@:s,/,\,) -EP $<

%.i : %.cpp
	$(CC) @$(mktmp,cl.rsp $(CFLAGS:s,\,\\,)\n) -Fo$(@:s,/,\,) -EP $<

%.ih : %.h
	$(CC) @$(mktmp,cl.rsp $(CFLAGS:s,\,\\,)\n) -Fo$(@:s,/,\,) -EP $<

%.obj : %.asm
	$(AS) @$(mktmp $(ASFLAGS:s,\,\\,) -c -Fo$(@:s,\,\\,:s,/,\\,)) $<

%.lst .PRECIOUS: %.asm
	$(AS) @$(mktmp $(ASFLAGS:s,\,\\,) -c -Fl$(@:s,\,\\,:s,/,\\,)) $<

.ELSE
	# Pass arguments normally
	#
$(BLD_DIR)/%.obj : %.c
	$(CC) $(CFLAGS) -Fo$(@:s,/,\,) $<

$(BLD_DIR)/%.obj : %.cpp
	$(CC) $(CFLAGS) -Fo$(@:s,/,\,) $<

$(BLD_DIR)/%.obj : %.asm
	$(AS) $(ASFLAGS) -c -Fo$(@:s,/,\,) $<

%.obj : %.c
	$(CC) $(CFLAGS) -Fo$(@:s,/,\,) $<

%.obj : %.cpp
	$(CC) $(CFLAGS) -Fo$(@:s,/,\,) $<

%.i : %.c
	$(CC) $(CFLAGS) -Fo$(@:s,/,\,) -EP $<

%.i : %.cpp
	$(CC) $(CFLAGS) -Fo$(@:s,/,\,) -EP $<

%.ih : %.h
	$(CC) $(CFLAGS) -Fo$(@:s,/,\,) -EP $<

%.obj : %.asm
	$(AS) $(ASFLAGS) -c -Fo$(@:s,/,\,) $<

.ENDIF

## The final target
##

.IF $(TARGET_TYPE) == LIB

# Library
#
TARGET:=$(LIB_NAME)
"$(LIB_NAME)": $(OBJS)
	$(LIB) -nodefaultlib -out:$(@:s,/,\,) @$(mktmp,lib.rsp $(<:t"\n":s,/,\\,))

.ELIF $(TARGET_TYPE) == DRIVER

# Driver - Build a Win32 DLL
#
CFLAGS+= -D__DRIVER__
ASFLAGS+= -D__DRIVER__

# Make sure we use the static C runtime for the driver
#
.IF $(BR_BUILD) == DEBUG
CFLAGS +=-MLd -LD
.ELIF $(BR_BUILD) == PROFILE
CFLAGS +=-MLd -LD
.ELSE
CFLAGS +=-ML -LD
.ENDIF

LDFLAGS+:=/ALIGN:4096 /STUB:$(BR_MAKE_DIR:s,\,\\,)\\brdstub.exe
DEBUG_LDFLAGS+:=
PROFILE_LDFLAGS+:=
RELEASE_LDFLAGS+:=
EVAL_LDFLAGS+:=
TEST_LDFLAGS+:=

.IF $(EXPORTS) ==
LDFLAGS+=/EXPORT:BrDrv1Begin
.ELSE
LDFLAGS+=/DEF:_exports.def
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
	$(PERL) $(BR_MAKE_DIR)/expgen.pl m $(LIB_SUFFIX_T) < $(EXPORTS) > _exports.def
.ELSE
	$(PERL) $(BR_MAKE_DIR)/expgen.pl m < $(EXPORTS) > _exports.def
.ENDIF
.END
	$(LINK) @$(mktmp,link.rsp /OUT:$@\n\
/DLL\n\
/NOENTRY\n\
$(<:t"\n":s,\,/,)\n\
$(LDFLAGS)\n\
$(!null,$(LDLIBS) /DEFAULTLIB:)$(LDLIBS:t"\n/DEFAULTLIB:":s,\,/,)\n\
)
	$(COPY) $(TARGET) $(BR_TARGET_DIR)\dll\$*.$(DRV_EXT)
	$(RM) $(TARGET:s/.dll/.exp)
	$(RM) $(TARGET:s/.dll/.lib)
.ELSE

.IF $(BR_EXTENDER) == PHARLAP

# Executable - Pharlap TNT using 386link
#

LDFLAGS+:=-stack 65536
DEBUG_LDFLAGS+:=-cvsym
PROFILE_LDFLAGS+:=-cvsym

RELEASE_LDFLAGS+:=
EVAL_LDFLAGS+:=
TEST_LDFLAGS+:=

LBLIBS+=

LIB_SUFFIX_EXTENDER:=pl

$(TARGET): $(LDLIBS)
$(TARGET): $(OBJS)
	386link @msvc32.dos -exe $(@:s,/,\,) @$(mktmp,386link.rsp $(<:t"\n":s,\,\\,)\n\
$(LDFLAGS)\n\
-lib $(LDLIBS:s,\,\\,:s,/,\\,)\n\
)
#	rebind $(TARGET)

.ELIF $(BR_EXTENDER) == NT .OR. $(BR_EXTENDER) == WIN_32

# Executable - NT Console App.
#
LIB_SUFFIX_EXTENDER:=wn

LDFLAGS+:=/DEFAULTLIB:advapi32.lib
DEBUG_LDFLAGS+:=/DEBUG
PROFILE_LDFLAGS+:=/DEBUG
RELEASE_LDFLAGS+:=
TEST_LDFLAGS+:=
LDLIBS+=

"$(TARGET)": $(LDLIBS)
"$(TARGET)": $(OBJS)

	$(LINK) @$(mktmp,link.rsp /OUT:$@\n\
$(<:t"\n":s,\,/,)\n\
$(LDFLAGS)\n\
$(!null,$(LDLIBS) /DEFAULTLIB:)$(LDLIBS:t"\n/DEFAULTLIB:":s,\,/,)\n\
)

.ELSE
Unknown_EXTENDER:
.ENDIF

.ENDIF

## Definitions common to all compilers
##
.INCLUDE .IGNORE: $(BR_MAKE_DIR)/common.mak

## Build resident images
##
.IF $(RES_IMAGE)
"$(DDI_LIB_DIR)\$(RES_IMAGE)$(LIB_SUFFIX_C).lib" $(RES_IMAGE).h .UPDATEALL: $(RES_IMAGE).fns $(BR_MAKE_DIR)\resgen.pl
	-$(SET_WRITEABLE) $(RES_IMAGE).h
	$(PERL) $(BR_MAKE_DIR)/resgen.pl $(RES_IMAGE).h _img.def m <$(RES_IMAGE).fns
	-$(RM) $(DDI_LIB_DIR)\$(RES_IMAGE)$(LIB_SUFFIX_C).lib
	$(LIB) /def:_img.def /nologo /machine:IX86 /out:$(DDI_LIB_DIR)\$(RES_IMAGE)$(LIB_SUFFIX_C).lib
	$(RM) $(DDI_LIB_DIR)\$(RES_IMAGE)$(LIB_SUFFIX_C).exp
	$(RM) _img.def

target: $(DDI_LIB_DIR)\$(RES_IMAGE)$(LIB_SUFFIX_C).lib
.END

## Dependencies
##
.INCLUDE .IGNORE: $(BLD_DIR)/depend.out

