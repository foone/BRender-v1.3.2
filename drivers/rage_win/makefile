# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: makefile 1.1 1997/12/10 16:50:17 jon Exp $
# $Locker: $
#
# Makefile for BRender ATI Rage driver
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR
.IMPORT .IGNORE: ATI3DCIF_DIR DDRAWSDK_DIR

ATI3DCIF_DIR*:=ati3dcif
DDRAWSDK_DIR*:=t:\directx3\sdk

TARGET_TYPE*:=LIB

.IF $(TARGET_TYPE) == LIB

BLD_BASE:=lib
LIB_PART=rage

CFLAGS=-DBrDrv1Begin=BrDrv1ATIBegin

.ELIF $(TARGET_TYPE) == DRIVER

BLD_BASE:=drv

TARGET=rage_win.dll

LDFLAGS+=/DEFAULTLIB:kernel32 /DEFAULTLIB:user32 /DEFAULTLIB:gdi32
LDLIBS+=\
	$(DDI_LIB_DIR)/fwimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(DDI_LIB_DIR)/hostimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(DDI_LIB_DIR)/pmimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(ATI3DCIF_DIR)/ati3dcif.lib\
	$(DDRAWSDK_DIR)/lib/ddraw.lib\

.ENDIF

INCLUDES+=\
	$(ATI3DCIF_DIR)\
	$(DDRAWSDK_DIR)/inc\

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/driver$(OBJ_EXT)\
	$(BLD_DIR)/device$(OBJ_EXT)\
	$(BLD_DIR)/object$(OBJ_EXT)\
	$(BLD_DIR)/ocfree$(OBJ_EXT)\
	$(BLD_DIR)/outfcty$(OBJ_EXT)\
	$(BLD_DIR)/dpmfront$(OBJ_EXT)\
	$(BLD_DIR)/dpmback$(OBJ_EXT)\
	$(BLD_DIR)/devclut$(OBJ_EXT)\
	$(BLD_DIR)/plib$(OBJ_EXT)\
	$(BLD_DIR)/pstate$(OBJ_EXT)\
	$(BLD_DIR)/sbuffer$(OBJ_EXT)\
	$(BLD_DIR)/match$(OBJ_EXT)\
	$(BLD_DIR)/rendfunc$(OBJ_EXT)\
	$(BLD_DIR)/atilink$(OBJ_EXT)\
	$(BLD_DIR)/stubmain$(OBJ_EXT)\

OBJS_ASM=\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

