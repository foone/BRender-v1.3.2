# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: makefile 1.1 1997/12/10 17:16:26 jon Exp $
# $Locker: $
#
# Makefile for BRender DOS IO support libary
#
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR
.IMPORT .IGNORE: BR_EXTENDER

BR_BASE_TYPE=NONE
TARGET_TYPE:=LIB
CFLAGS+=-D__BR_V1DB__=0

LIB_PART:=dosio
TARGET_TYPE:=LIB

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/divtrap$(OBJ_EXT)\
	$(BLD_DIR)/dossetup$(OBJ_EXT)\
	$(BLD_DIR)/eventq$(OBJ_EXT)\
	$(BLD_DIR)/keyboard$(OBJ_EXT)\
	$(BLD_DIR)/readmse$(OBJ_EXT)\

OBJS_ASM=\
	$(BLD_DIR)/clock$(OBJ_EXT)\
	$(BLD_DIR)/divtraph$(OBJ_EXT)\

EXPORT_INCLUDES=\
	$(INC_DIR)/dosio.h\
	$(INC_DIR)/eventq.h\
	$(INC_DIR)/keyboard.h\
	$(INC_DIR)/dos_x.h\
	$(INC_DIR)/dos_p.h\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

