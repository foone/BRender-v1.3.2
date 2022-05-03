# Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
#
# $Id: makefile 1.1 1997/12/10 16:59:23 jon Exp $
# $Locker: $
#
# Makefile for BRender sample
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR

BR_BASE_TYPE=NONE

TARGET_TYPE:=LIB
LIB_PART:=trak
LIB_DIR*=$(BR_TARGET_DIR)\ddi_lib

OBJS_C=\
	$(BLD_DIR)/trackmem$(OBJ_EXT)\
	$(BLD_DIR)/calltrak$(OBJ_EXT)\
	$(BLD_DIR)/stdmem$(OBJ_EXT)\

OBJS_ASM=\
	$(BLD_DIR)/fnhooks$(OBJ_EXT)\

EXPORT_INCLUDES=\
	$(DDI_INC_DIR)/trackmem.h\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

