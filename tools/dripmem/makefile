# Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
#
# $Id: makefile 1.1 1998/11/17 15:32:44 jon Exp $
# $Locker: $
#
# Makefile for BRender sample
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR

BR_BASE_TYPE=NONE

TARGET_TYPE:=LIB
LIB_PART:=drip
LIB_DIR*=$(BR_TARGET_DIR)\ddi_lib

OBJS_C=\
	$(BLD_DIR)/dripmem$(OBJ_EXT)\
	$(BLD_DIR)/stdmem$(OBJ_EXT)\

EXPORT_INCLUDES=\
	$(DDI_INC_DIR)/dripmem.h\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

