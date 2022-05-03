# Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
#
# $Id: makefile 1.2 1997/12/11 12:09:21 jon Exp $
# $Locker: $
#
# Makefile for texconv
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR BR_EXTENDER

TARGET=texconv.exe
TARGET_TYPE:=EXE

OBJS_C=\
	$(BLD_DIR)/texconv$(OBJ_EXT)\
	$(BLD_DIR)/cla$(OBJ_EXT)\
	$(BLD_DIR)/convert$(OBJ_EXT)\
	$(BLD_DIR)/lists$(OBJ_EXT)\
	$(BLD_DIR)/loadpix$(OBJ_EXT)\
	$(BLD_DIR)/log$(OBJ_EXT)\
	$(BLD_DIR)/pmaps$(OBJ_EXT)\
	$(BLD_DIR)/savepix$(OBJ_EXT)\
	$(BLD_DIR)/savetga$(OBJ_EXT)\
	$(BLD_DIR)/palquant$(OBJ_EXT)\
	$(BLD_DIR)/brbhook$(OBJ_EXT)\

OBJS_CPP=\
	$(BLD_DIR)/mip$(OBJ_EXT)\

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
	$(LIB_DIR)/dvmc$(LIB_TYPE_NB)$(LIB_EXT)\
	$(LIB_DIR)/dvvs$(LIB_TYPE_NB)$(LIB_EXT)\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

