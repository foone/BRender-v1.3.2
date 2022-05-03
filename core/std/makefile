# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: makefile 1.1 1997/12/10 17:16:36 jon Exp $
# $Locker: $
#
# Makefile for BRender Standard CLIB interface
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR

BR_BASE_TYPE=NONE

LIB_PART:=brst
TARGET_TYPE:=LIB
CFLAGS+=-D__BR_V1DB__=0

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/stddiag$(OBJ_EXT)\
	$(BLD_DIR)/stdmem$(OBJ_EXT)\
	$(BLD_DIR)/stdfile$(OBJ_EXT)\
	$(BLD_DIR)/stdlib$(OBJ_EXT)\
	$(BLD_DIR)/math$(OBJ_EXT)\
	$(BLD_DIR)/logwrite$(OBJ_EXT)\
	$(BLD_DIR)/brexcept$(OBJ_EXT)\

OBJS_ASM=\


.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

