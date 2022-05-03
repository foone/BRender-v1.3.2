# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: softi.mak 1.1 1997/12/10 16:48:34 jon Exp $
# $Locker: $
#
# Makefile for autoloaded primitives (Indexed no Z)
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR

TARGET_TYPE*:=DRIVER

LIB_INC:=drv

.IF $(TARGET_TYPE) == LIB

BLD_BASE:=li
LIB_PART:=li

.ELIF $(TARGET_TYPE) == DRIVER

BLD_BASE:=si
TARGET=softi$(LIB_SUFFIX_T).dll
EXPORTS:=softi.exf
EXPORT_TYPE:=1
CFLAGS+=
LDLIBS+=\
	$(DDI_LIB_DIR)/fwimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(LIB_DIR)/brmt$(LIB_TYPE)$(LIB_EXT)\

.ENDIF

CFLAGS+=-DPARTS=PART_8 -DBASED_$(BR_BASE_TYPE)=1
ASFLAGS+=-DPARTS=PART_8 -DBASED_$(BR_BASE_TYPE)=1

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/work$(OBJ_EXT)\

OBJS_ASM=\
	$(BLD_DIR)/magicsym$(OBJ_EXT)\
\
	$(BLD_DIR)/ti8_pip$(OBJ_EXT)\
\
	$(BLD_DIR)/zs8$(OBJ_EXT)\
	$(BLD_DIR)/zs8p2unl$(OBJ_EXT)\
	$(BLD_DIR)/zs8awtm$(OBJ_EXT)\
        $(BLD_DIR)/decal$(OBJ_EXT)\
	$(BLD_DIR)/fpsetup$(OBJ_EXT)\
\
	$(BLD_DIR)/pfpsetup$(OBJ_EXT)\
	$(BLD_DIR)/fti8_pip$(OBJ_EXT)\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

