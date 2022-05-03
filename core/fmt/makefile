# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: makefile 1.1 1997/12/10 17:16:27 jon Exp $
# $Locker: $
#
# Makefile for BRender foreign file format support
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR

LIB_PART:=brfm
TARGET_TYPE:=LIB

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/load3ds$(OBJ_EXT)\
	$(BLD_DIR)/loadasc$(OBJ_EXT)\
	$(BLD_DIR)/loadbmp$(OBJ_EXT)\
	$(BLD_DIR)/loadgif$(OBJ_EXT)\
	$(BLD_DIR)/loadiff$(OBJ_EXT)\
	$(BLD_DIR)/loadnff$(OBJ_EXT)\
	$(BLD_DIR)/loadscr$(OBJ_EXT)\
	$(BLD_DIR)/loadtga$(OBJ_EXT)\
	$(BLD_DIR)/savescr$(OBJ_EXT)\
    $(BLD_DIR)/loadvue$(OBJ_EXT)\

OBJS_ASM=\


EXPORT_INCLUDES=\
	$(INC_DIR)/fmt.h\
	$(INC_DIR)/fmt_p.h\


.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

