# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: makefile 1.1 1997/12/10 16:58:05 jon Exp $
# $Locker: $
#
# Makefile for Q3D test harness build with WATCOM C32
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR BR_EXTENDER

# Force floating point, stack based
#
TARGET_TYPE=EXE
BR_BASE_TYPE=FLOAT
BR_LIB_SUFFIX=s

TARGET=3ds2br.exe

OBJS_C=\
	$(BLD_DIR)/3ds2br.obj\

LDLIBS+=\
	$(LIB_DIR)/brdb$(LIB_TYPE)$(LIB_EXT)\
	$(LIB_DIR)/brmt$(LIB_TYPE)$(LIB_EXT)\
	$(LIB_DIR)/brfm$(LIB_TYPE)$(LIB_EXT)\
	$(LIB_DIR)/brfw$(LIB_TYPE_NB)$(LIB_EXT)\
	$(LIB_DIR)/brpm$(LIB_TYPE_NB)$(LIB_EXT)\
	$(LIB_DIR)/brst$(LIB_TYPE_NB)$(LIB_EXT)\
	$(LIB_DIR)/hst$(LIB_TYPE_EX)$(LIB_EXT)\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

