# Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
#
# $Id: makefile 1.2 1997/12/11 19:05:37 jon Exp $
# $Locker: $
#
# Makefile for geoconv
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR BR_EXTENDER

SAVE_3DS*=0

TARGET=geoconv.exe
TARGET_TYPE:=EXE

CFLAGS+=\
	-DSAVE_3DS=$(SAVE_3DS)\

OBJS_C=\
	$(BLD_DIR)/geoconv$(OBJ_EXT)\
	$(BLD_DIR)/irit$(OBJ_EXT)\
	$(BLD_DIR)/save_c$(OBJ_EXT)\
	$(BLD_DIR)/brbhook$(OBJ_EXT)\

.IF $(SAVE_3DS) != 0

OBJS_C+=\
	$(BLD_DIR)/save3ds$(OBJ_EXT)\

.ENDIF

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
	$(LIB_DIR)/prim$(LIB_TYPE)$(LIB_EXT)\
	$(LIB_DIR)/rndf$(LIB_TYPE)$(LIB_EXT)\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

