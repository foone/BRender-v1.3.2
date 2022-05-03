# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: softhz.mak 1.1 1997/12/10 16:48:29 jon Exp $
# $Locker: $
#
# Makefile for autoloaded primitives (Indexed no Z)
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR

TARGET_TYPE*:=DRIVER

LIB_INC:=drv

.IF $(TARGET_TYPE) == LIB

BLD_BASE:=lhz
LIB_PART:=lhz

.ELIF $(TARGET_TYPE) == DRIVER

BLD_BASE:=shz
TARGET=softhz$(LIB_SUFFIX_T).dll
EXPORTS:=softhz.exf
EXPORT_TYPE:=1
CFLAGS+=
LDLIBS+=\
	$(DDI_LIB_DIR)/fwimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(LIB_DIR)/brmt$(LIB_TYPE)$(LIB_EXT)\

.ENDIF

CFLAGS+=-DPARTS=PART_15Z_OR_16Z -DBASED_$(BR_BASE_TYPE)=1
ASFLAGS+=-DPARTS=PART_15Z_OR_16Z -DBASED_$(BR_BASE_TYPE)=1

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/work$(OBJ_EXT)\
	$(BLD_DIR)/frcp$(OBJ_EXT)\
	$(BLD_DIR)/rcp$(OBJ_EXT)\
	$(BLD_DIR)/dither$(OBJ_EXT)\
\
	$(BLD_DIR)/awtmz$(OBJ_EXT)\
	$(BLD_DIR)/perspz$(OBJ_EXT)\

OBJS_ASM=\
	$(BLD_DIR)/safediv$(OBJ_EXT)\
	$(BLD_DIR)/sar16$(OBJ_EXT)\
	$(BLD_DIR)/magicsym$(OBJ_EXT)\
\
	$(BLD_DIR)/t_piza$(OBJ_EXT)\
	$(BLD_DIR)/t15_pizp$(OBJ_EXT)\
	$(BLD_DIR)/tt15_piz$(OBJ_EXT)\
	$(BLD_DIR)/tt16_piz$(OBJ_EXT)\
\
        $(BLD_DIR)/fpsetup$(OBJ_EXT)\
        $(BLD_DIR)/zb16p2ul$(OBJ_EXT)\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

