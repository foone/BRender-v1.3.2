# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: makefile 1.1 1997/12/10 17:16:35 jon Exp $
# $Locker: $
#
# Makefile for BRender Pixelmap support
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR

BR_BASE_TYPE=NONE

LIB_PART:=brpm
TARGET_TYPE:=LIB
CFLAGS+=-D__BR_V1DB__=0
RES_IMAGE=pmimg

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/fontptrs$(OBJ_EXT)\
	$(BLD_DIR)/genclip$(OBJ_EXT)\
	$(BLD_DIR)/gencopy$(OBJ_EXT)\
	$(BLD_DIR)/pixelmap$(OBJ_EXT)\
	$(BLD_DIR)/pmdsptch$(OBJ_EXT)\
	$(BLD_DIR)/pmfile$(OBJ_EXT)\
	$(BLD_DIR)/pmgen$(OBJ_EXT)\
	$(BLD_DIR)/pmmem$(OBJ_EXT)\
	$(BLD_DIR)/pmnull$(OBJ_EXT)\
	$(BLD_DIR)/pmsetup$(OBJ_EXT)\
	$(BLD_DIR)/quantize$(OBJ_EXT)\
	$(BLD_DIR)/scale$(OBJ_EXT)\
        $(BLD_DIR)/mipsupt$(OBJ_EXT)\

OBJS_ASM=\
	$(BLD_DIR)/fnt3x5$(OBJ_EXT)\
	$(BLD_DIR)/fntp4x6$(OBJ_EXT)\
	$(BLD_DIR)/fntp7x9$(OBJ_EXT)\
	$(BLD_DIR)/memloops$(OBJ_EXT)\

EXPORT_INCLUDES=\
	$(DDI_INC_DIR)/pm.h\
	$(DDI_INC_DIR)/pm_ip.h\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)
