# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: makefile 1.1 1997/12/10 17:16:33 jon Exp $
# $Locker: $
#
# Makefile for BRender maths support
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR

LIB_PART:=brmt
TARGET_TYPE:=LIB
CFLAGS+=-D__BR_V1DB__=0

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/angles$(OBJ_EXT)\
	$(BLD_DIR)/fixed$(OBJ_EXT)\
	$(BLD_DIR)/matrix23$(OBJ_EXT)\
	$(BLD_DIR)/matrix34$(OBJ_EXT)\
	$(BLD_DIR)/matrix4$(OBJ_EXT)\
	$(BLD_DIR)/plane$(OBJ_EXT)\
	$(BLD_DIR)/quat$(OBJ_EXT)\
	$(BLD_DIR)/scalar$(OBJ_EXT)\
	$(BLD_DIR)/transfrm$(OBJ_EXT)\
	$(BLD_DIR)/vector$(OBJ_EXT)\

OBJS_ASM=\
	$(BLD_DIR)/fixed386$(OBJ_EXT)\
	$(BLD_DIR)/fxadc386$(OBJ_EXT)\


EXPORT_INCLUDES=\
	$(DDI_INC_DIR)/math_ip.h\
	$(DDI_INC_DIR)/vecifns.h\
	$(INC_DIR)/brmath.h\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

