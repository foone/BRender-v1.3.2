# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: softd.mak 1.1 1997/12/10 16:48:22 jon Exp $
# $Locker: $
#
# Makefile for autoloaded primitives (Indexed Z)
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR


TARGET_TYPE*:=DRIVER

LIB_INC:=drv

.IF $(TARGET_TYPE) == LIB

BLD_BASE:=ld
LIB_PART:=ld

.ELIF $(TARGET_TYPE) == DRIVER

BLD_BASE:=sd
TARGET=softd.dll
EXPORTS:=softd.exf
CFLAGS+=
LDLIBS+=\
	$(DDI_LIB_DIR)/fwimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(LIB_DIR)/brmt$(LIB_TYPE)$(LIB_EXT)\

.ENDIF

CFLAGS+=-DPARTS=PART_DITHER -DBASED_$(BR_BASE_TYPE)=1
ASFLAGS+=-DPARTS=PART_DITHER -DBASED_$(BR_BASE_TYPE)=1

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/work$(OBJ_EXT)\
	$(BLD_DIR)/frcp$(OBJ_EXT)\
	$(BLD_DIR)/rcp$(OBJ_EXT)\
	$(BLD_DIR)/dither$(OBJ_EXT)\
\
	$(BLD_DIR)/perspz$(OBJ_EXT)\
	$(BLD_DIR)/persp$(OBJ_EXT)\

OBJS_ASM=\
	$(BLD_DIR)/safediv$(OBJ_EXT)\
	$(BLD_DIR)/sar16$(OBJ_EXT)\
	$(BLD_DIR)/magicsym$(OBJ_EXT)\
\
	$(BLD_DIR)/ti8_pizp$(OBJ_EXT)\
	$(BLD_DIR)/ti8_pip$(OBJ_EXT)\


.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

