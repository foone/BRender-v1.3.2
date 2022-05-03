# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: makefile 1.1 1997/12/10 16:54:02 jon Exp $
# $Locker: $
#
# Makefile for BRender VESA driver
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR

.IF $(TARGET_TYPE) ==
TARGET_TYPE:=LIB
.ENDIF

BR_BASE_TYPE:=NONE

LIB_INC:=drv

.IF $(TARGET_TYPE) == LIB

BLD_BASE:=lib
LIB_PART:=dvvs
CFLAGS+=-DBrDrv1Begin=BrDrv1VESABegin

.ELIF $(TARGET_TYPE) == DRIVER

BLD_BASE:=drv
TARGET:=vesa.dll
CFLAGS+=
LDEXPORT=BrDrv1Begin
LDLIBS+=\
	$(DDI_LIB_DIR)/fwimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(DDI_LIB_DIR)/hostimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(DDI_LIB_DIR)/pmimg$(LIB_SUFFIX_C)$(LIB_EXT)\

.ENDIF

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/bios$(OBJ_EXT)\
	$(BLD_DIR)/devclut$(OBJ_EXT)\
	$(BLD_DIR)/device$(OBJ_EXT)\
	$(BLD_DIR)/devpixmp$(OBJ_EXT)\
	$(BLD_DIR)/devpml$(OBJ_EXT)\
	$(BLD_DIR)/devpmw$(OBJ_EXT)\
	$(BLD_DIR)/devpmwb$(OBJ_EXT)\
	$(BLD_DIR)/driver$(OBJ_EXT)\
	$(BLD_DIR)/object$(OBJ_EXT)\
	$(BLD_DIR)/ocfree$(OBJ_EXT)\
	$(BLD_DIR)/outfcty$(OBJ_EXT)\
	$(BLD_DIR)/vesaapi$(OBJ_EXT)\

OBJS_ASM=\
	$(BLD_DIR)/ports$(OBJ_EXT)\
	$(BLD_DIR)/vesabank$(OBJ_EXT)\
	$(BLD_DIR)/vesaloop$(OBJ_EXT)\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

