# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: makefile 1.1 1997/12/10 16:52:17 jon Exp $
# $Locker: $
#
# Makefile for BRender software renderer
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR

AFLAGS+=-DBASED_$(BR_BASE_TYPE)=1

.IF $(TARGET_TYPE) ==
TARGET_TYPE:=LIB
.ENDIF

LIB_INC:=drv

.IF $(TARGET_TYPE) == LIB

BLD_BASE:=lib
LIB_PART=rnd$(LIB_SUFFIX_T)
CFLAGS=-DBrDrv1Begin=BrDrv1SoftRendBegin

.ELIF $(TARGET_TYPE) == DRIVER

BLD_BASE:=drv
TARGET=softrnd$(LIB_SUFFIX_T).dll
LDFLAGS+=
LDEXPORT=BrDrv1Begin
LDLIBS+=\
	$(DDI_LIB_DIR)/fwimg$(LIB_SUFFIX_L)$(LIB_EXT)\
	$(DDI_LIB_DIR)/hostimg$(LIB_SUFFIX_L)$(LIB_EXT)\
	$(DDI_LIB_DIR)/pmimg$(LIB_SUFFIX_L)$(LIB_EXT)\
	$(LIB_DIR)/brmt$(LIB_TYPE)$(LIB_EXT)\

.ENDIF

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/alpha$(OBJ_EXT)\
	$(BLD_DIR)/clip$(OBJ_EXT)\
	$(BLD_DIR)/convert$(OBJ_EXT)\
	$(BLD_DIR)/device$(OBJ_EXT)\
	$(BLD_DIR)/driver$(OBJ_EXT)\
	$(BLD_DIR)/faceops$(OBJ_EXT)\
	$(BLD_DIR)/genrend$(OBJ_EXT)\
	$(BLD_DIR)/gprim$(OBJ_EXT)\
	$(BLD_DIR)/gv1buckt$(OBJ_EXT)\
	$(BLD_DIR)/gv1model$(OBJ_EXT)\
	$(BLD_DIR)/heap$(OBJ_EXT)\
	$(BLD_DIR)/light24$(OBJ_EXT)\
	$(BLD_DIR)/light8$(OBJ_EXT)\
	$(BLD_DIR)/mapping$(OBJ_EXT)\
	$(BLD_DIR)/object$(OBJ_EXT)\
	$(BLD_DIR)/onscreen$(OBJ_EXT)\
	$(BLD_DIR)/renderer$(OBJ_EXT)\
	$(BLD_DIR)/rendfcty$(OBJ_EXT)\
	$(BLD_DIR)/rndstate$(OBJ_EXT)\
	$(BLD_DIR)/setup$(OBJ_EXT)\
	$(BLD_DIR)/sstate$(OBJ_EXT)\
	$(BLD_DIR)/state$(OBJ_EXT)\
	$(BLD_DIR)/v1model$(OBJ_EXT)\
	$(BLD_DIR)/lighting$(OBJ_EXT)\
	$(BLD_DIR)/depth$(OBJ_EXT)\

OBJS_ASM=\
	$(BLD_DIR)/cull$(OBJ_EXT)\
	$(BLD_DIR)/gen_386$(OBJ_EXT)\
	$(BLD_DIR)/geometry$(OBJ_EXT)\
	$(BLD_DIR)/magicsym$(OBJ_EXT)\
	$(BLD_DIR)/v1m_386$(OBJ_EXT)\
	$(BLD_DIR)/face_386$(OBJ_EXT)\
	$(BLD_DIR)/subdiv$(OBJ_EXT)\

EXPORT_INCLUDES=\
	$(DDI_INC_DIR)/priminfo.h\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)


