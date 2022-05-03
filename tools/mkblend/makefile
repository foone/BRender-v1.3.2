# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: makefile 1.1 1997/12/10 16:58:18 jon Exp $
# $Locker: $
#
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR BR_EXTENDER

TARGET_TYPE=EXE
TARGET=mkblend.exe

OBJS_C=\
	$(BLD_DIR)/mkblend.obj\

LDLIBS+=\
	$(LIB_DIR)/brdb$(LIB_TYPE)$(LIB_EXT)\
	$(LIB_DIR)/brmt$(LIB_TYPE)$(LIB_EXT)\
	$(LIB_DIR)/brfm$(LIB_TYPE)$(LIB_EXT)\
	$(LIB_DIR)/brfw$(LIB_TYPE_NB)$(LIB_EXT)\
	$(LIB_DIR)/brpm$(LIB_TYPE_NB)$(LIB_EXT)\
	$(LIB_DIR)/brst$(LIB_TYPE_NB)$(LIB_EXT)\
	$(LIB_DIR)/dosio$(LIB_TYPE_NB)$(LIB_EXT)\
	$(LIB_DIR)/hst$(LIB_TYPE_EX)$(LIB_EXT)\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)
