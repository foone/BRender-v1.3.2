# Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
#
# $Id: makefile 1.1 1997/12/10 16:58:32 jon Exp $
# $Locker: $
#
# Makefile for Q3D test harness
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR BR_EXTENDER

TARGET=modtest.exe
TARGET_TYPE:=EXE

OBJS_C=\
	$(BLD_DIR)/modtest$(OBJ_EXT)\

OBJS_ASM=\

LDLIBS+=\
	$(LIB_DIR)/brdb$(LIB_TYPE)$(LIB_EXT)\
	$(LIB_DIR)/brmt$(LIB_TYPE)$(LIB_EXT)\
	$(LIB_DIR)/brfm$(LIB_TYPE)$(LIB_EXT)\
	$(LIB_DIR)/brfw$(LIB_TYPE_NB)$(LIB_EXT)\
	$(LIB_DIR)/brpm$(LIB_TYPE_NB)$(LIB_EXT)\
	$(LIB_DIR)/brst$(LIB_TYPE_NB)$(LIB_EXT)\
	$(LIB_DIR)/dosio$(LIB_TYPE_NB)$(LIB_EXT)\
	$(LIB_DIR)/hst$(LIB_TYPE_EX)$(LIB_EXT)\

#	$(LIB_DIR)/rndx$(LIB_TYPE)$(LIB_EXT)\
#	$(LIB_DIR)/dvmc$(LIB_TYPE)$(LIB_EXT)\
#	$(LIB_DIR)/pmmx$(LIB_TYPE)$(LIB_EXT)\
#	$(LIB_DIR)/prim$(LIB_TYPE)$(LIB_EXT)\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

