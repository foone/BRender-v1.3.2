# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: softh.mak 1.1 1997/12/10 16:48:24 jon Exp $
# $Locker: $
#
# Makefile for autoloaded primitives (Indexed no Z)
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR

TARGET_TYPE*:=DRIVER

LIB_INC:=drv

.IF $(TARGET_TYPE) == LIB

BLD_BASE:=lh
LIB_PART:=lh

.ELIF $(TARGET_TYPE) == DRIVER

BLD_BASE:=sh
TARGET=softh$(LIB_SUFFIX_T).dll
EXPORTS:=softh.exf
EXPORT_TYPE:=1
CFLAGS+=
LDLIBS+=\
	$(DDI_LIB_DIR)/fwimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(LIB_DIR)/brmt$(LIB_TYPE)$(LIB_EXT)\

.ENDIF

CFLAGS+=-DPARTS=PART_15_OR_16
ASFLAGS+=-DPARTS=PART_15_OR_16

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/work$(OBJ_EXT)\
	$(BLD_DIR)/frcp$(OBJ_EXT)\
	$(BLD_DIR)/rcp$(OBJ_EXT)\
	$(BLD_DIR)/dither$(OBJ_EXT)\
\
	$(BLD_DIR)/awtm$(OBJ_EXT)\
	$(BLD_DIR)/persp$(OBJ_EXT)\

OBJS_ASM=\
	$(BLD_DIR)/safediv$(OBJ_EXT)\
	$(BLD_DIR)/sar16$(OBJ_EXT)\
	$(BLD_DIR)/magicsym$(OBJ_EXT)\
\
	$(BLD_DIR)/t_pia$(OBJ_EXT)\
	$(BLD_DIR)/t15_pip$(OBJ_EXT)\
	$(BLD_DIR)/tt15_pi$(OBJ_EXT)\
	$(BLD_DIR)/tt16_pi$(OBJ_EXT)\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

