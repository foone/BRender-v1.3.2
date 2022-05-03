# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: makefile 1.1 1997/12/10 16:51:02 jon Exp $
# $Locker: $
#
# Makefile for BRender S3 Windows Driver
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR
.IMPORT .IGNORE: S3_DIR DDRAWSDK_DIR

S3_DIR*:=s3sdk
DDRAWSDK_DIR*:=t:\directx3\sdk

.IF $(TARGET_TYPE) ==
TARGET_TYPE:=DRIVER
.ENDIF

BR_BASE_TYPE:=NONE

.IF $(TARGET_TYPE) == LIB

BLD_BASE:=lib
LIB_PART:=brs3
CFLAGS+=-DBrDrv1Begin=BrDrv1S3Begin
CFLAGS+=-D__MSC__

.ELIF $(TARGET_TYPE) == DRIVER

BLD_BASE:=drv
TARGET:=s3_win.dll
LDEXPORT:=BrDrv1Begin
LDFLAGS+=/DEFAULTLIB:kernel32 /DEFAULTLIB:user32 /DEFAULTLIB:gdi32
LDLIBS+=\
	$(DDI_LIB_DIR)/fwimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(DDI_LIB_DIR)/hostimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(DDI_LIB_DIR)/pmimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(S3_DIR)/S3DTKW.LIB\

.ENDIF


INCLUDES+=\
	$(S3_DIR)\
	$(DDRAWSDK_DIR)/inc\

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/match$(OBJ_EXT)\
	$(BLD_DIR)/devclut$(OBJ_EXT)\
	$(BLD_DIR)/device$(OBJ_EXT)\
	$(BLD_DIR)/driver$(OBJ_EXT)\
 	$(BLD_DIR)/plib$(OBJ_EXT)\
	$(BLD_DIR)/outfcty$(OBJ_EXT)\
	$(BLD_DIR)/dpmfront$(OBJ_EXT)\
	$(BLD_DIR)/dpmback$(OBJ_EXT)\
	$(BLD_DIR)/sbuffer$(OBJ_EXT)\
	$(BLD_DIR)/rendfunc$(OBJ_EXT)\
	$(BLD_DIR)/pstate$(OBJ_EXT)\
	$(BLD_DIR)/s3link$(OBJ_EXT)\
	$(BLD_DIR)/log$(OBJ_EXT)\
	$(BLD_DIR)/ocfree$(OBJ_EXT)\
	$(BLD_DIR)/object$(OBJ_EXT)\
	$(BLD_DIR)/stubmain$(OBJ_EXT)\

OBJS_ASM=\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

