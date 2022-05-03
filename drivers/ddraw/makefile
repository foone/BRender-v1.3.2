# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: makefile 1.1 1997/12/10 16:45:21 jon Exp $
# $Locker: $
#
# Makefile for BRender DirectDraw Driver
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR DDRAWSDK_DIR

DDRAWSDK_DIR*:=t:\directx2\sdk

.IF $(TARGET_TYPE) ==
TARGET_TYPE:=LIB
.ENDIF

BR_BASE_TYPE:=NONE

.IF $(TARGET_TYPE) == LIB

BLD_BASE:=lib
LIB_PART:=ddraw
CFLAGS+=-DBrDrv1Begin=BrDrv1DirectDrawBegin

.ELIF $(TARGET_TYPE) == DRIVER

BLD_BASE:=drv
TARGET:=ddraw.dll
CFLAGS+=
LDFLAGS+=/EXPORT:_BrDrv1Begin=BrDrv1Begin /MAP
LDFLAGS+=/DEFAULTLIB:kernel32 /DEFAULTLIB:user32 /DEFAULTLIB:gdi32
LDLIBS+=\
	$(DDI_LIB_DIR)/fwimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(DDI_LIB_DIR)/hostimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(DDI_LIB_DIR)/pmimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(DDRAWSDK_DIR)\lib\ddraw.lib

.ENDIF

INCLUDES:=\
 $(BR_SOURCE_DIR)\inc\
 $(BR_SOURCE_DIR)\fw\
 $(BR_SOURCE_DIR)\dev\
 $(BR_SOURCE_DIR)\dosio\
 $(BR_SOURCE_DIR)\host\
 $(BR_SOURCE_DIR)\pixelmap\
 $(DDRAWSDK_DIR)\inc

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/driver$(OBJ_EXT)\
	$(BLD_DIR)/object$(OBJ_EXT)\
	$(BLD_DIR)/device$(OBJ_EXT)\
	$(BLD_DIR)/outfcty$(OBJ_EXT)\
	$(BLD_DIR)/devpixmp$(OBJ_EXT)\
	$(BLD_DIR)/devclut$(OBJ_EXT)\
	$(BLD_DIR)/ocfree$(OBJ_EXT)\
	$(BLD_DIR)/devpmddo$(OBJ_EXT)\


OBJS_ASM=\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

