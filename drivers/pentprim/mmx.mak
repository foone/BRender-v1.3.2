# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: mmx.mak 1.1 1997/12/10 16:47:21 jon Exp $
# $Locker: $
#
# Makefile for autoloaded primitives (Indexed Z)
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR


TARGET_TYPE*:=DRIVER

LIB_INC:=drv

.IF $(TARGET_TYPE) == LIB

BLD_BASE:=lmx
LIB_PART:=lmx

.ELIF $(TARGET_TYPE) == DRIVER

BLD_BASE:=mmx
TARGET=mmx.dll
EXPORTS:=mmx.exf
CFLAGS+=
LDLIBS+=\
	$(DDI_LIB_DIR)/fwimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(LIB_DIR)/brmt$(LIB_TYPE)$(LIB_EXT)\

.ENDIF

CFLAGS+=-DPARTS=PART_15Z_OR_16Z
ASFLAGS+=-DPARTS=PART_15Z_OR_16Z

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/work$(OBJ_EXT)\

OBJS_ASM=\
	$(BLD_DIR)/magicsym$(OBJ_EXT)\
\
	$(BLD_DIR)/rastrise$(OBJ_EXT)\
	$(BLD_DIR)/xzuv$(OBJ_EXT)\
	$(BLD_DIR)/xzuvc$(OBJ_EXT)\
	$(BLD_DIR)/xzrgb$(OBJ_EXT)\
	$(BLD_DIR)/xzuvrgb$(OBJ_EXT)\
	$(BLD_DIR)/xzuv555$(OBJ_EXT)\
	$(BLD_DIR)/xzuvc555$(OBJ_EXT)\
	$(BLD_DIR)/xzrgb555$(OBJ_EXT)\
	$(BLD_DIR)/xzuvrgb5$(OBJ_EXT)\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

