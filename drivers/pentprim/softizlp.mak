# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: softizlp.mak 1.1 1997/12/10 16:48:50 jon Exp $
# $Locker: $
#
# Makefile for autoloaded primitives (Indexed Z)
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR


TARGET_TYPE*:=DRIVER

LIB_INC:=drv

.IF $(TARGET_TYPE) == LIB

BLD_BASE:=liz
LIB_PART:=liz

.ELIF $(TARGET_TYPE) == DRIVER

BLD_BASE:=siz
TARGET=softiz.dll
EXPORTS:=softizlp.exf
CFLAGS+=
LDLIBS+=\
	$(DDI_LIB_DIR)/fwimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(LIB_DIR)/brmt$(LIB_TYPE)$(LIB_EXT)\

.ENDIF

CFLAGS+=-DPARTS=PART_8Z -DBASED_$(BR_BASE_TYPE)=1
ASFLAGS+=-DPARTS=PART_8Z -DBASED_$(BR_BASE_TYPE)=1

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/work$(OBJ_EXT)\
\
	$(BLD_DIR)/l_piz$(OBJ_EXT)\
	$(BLD_DIR)/p_piz$(OBJ_EXT)\

OBJS_ASM=\
	$(BLD_DIR)/safediv$(OBJ_EXT)\
	$(BLD_DIR)/sar16$(OBJ_EXT)\
	$(BLD_DIR)/magicsym$(OBJ_EXT)\
\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

