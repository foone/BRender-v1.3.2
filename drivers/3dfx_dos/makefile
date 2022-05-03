# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: makefile 1.4 1998/02/04 17:12:26 jon Exp $
# $Locker: $
#
# Makefile for BRender 3Dfx Driver
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR

.IF "$(3DFX_DIR)" == ""

.IMPORT .IGNORE: 3DFX_DIR

3DFX_DIR*:=c:\brender\drivers\3dfx_dos\glide\dos

.ENDIF

.IF $(TARGET_TYPE) ==
TARGET_TYPE:=DRIVER
.ENDIF

BR_BASE_TYPE:=NONE


.IF $(TARGET_TYPE) == LIB

BLD_BASE:=lib
LIB_PART:=3dfx
CFLAGS+=-DBrDrv1Begin=BrDrv13DFXBegin
CFLAGS+=-D__MSC__

.ELIF $(TARGET_TYPE) == DRIVER

LDEXPORT:=BrDrv1Begin
BLD_BASE:=drv
TARGET:=3dfx_dos.dll
CFLAGS+=-D__MSC__

LDLIBS+=\
	$(DDI_LIB_DIR)/fwimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(DDI_LIB_DIR)/hostimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(DDI_LIB_DIR)/pmimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(3DFX_DIR)/glide$(LIB_EXT)\
	$(3DFX_DIR)/sst1init$(LIB_EXT)

.ENDIF

INCLUDES:=\
	$(BR_SOURCE_DIR)\inc\
	$(BR_SOURCE_DIR)\fw\
	$(BR_SOURCE_DIR)\dev\
	$(BR_SOURCE_DIR)\dosio\
	$(BR_SOURCE_DIR)\host\
	$(BR_SOURCE_DIR)\pixelmap\
	$(3DFX_DIR)\

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/match$(OBJ_EXT)\
	$(BLD_DIR)/devclut$(OBJ_EXT)\
	$(BLD_DIR)/device$(OBJ_EXT)\
	$(BLD_DIR)/plib$(OBJ_EXT)\
	$(BLD_DIR)/outfcty$(OBJ_EXT)\
	$(BLD_DIR)/devpixmp$(OBJ_EXT)\
	$(BLD_DIR)/devpmsys$(OBJ_EXT)\
	$(BLD_DIR)/sbuffer$(OBJ_EXT)\
	$(BLD_DIR)/rendfunc$(OBJ_EXT)\
	$(BLD_DIR)/hstdlib$(OBJ_EXT)\
	$(BLD_DIR)/pstate$(OBJ_EXT)\
	$(BLD_DIR)/scale$(OBJ_EXT)\
	$(BLD_DIR)/texture$(OBJ_EXT)\

OBJS_ASM=\
	$(BLD_DIR)/memloops$(OBJ_EXT)\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

