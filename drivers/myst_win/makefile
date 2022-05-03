# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: makefile 1.1 1997/12/10 16:46:07 jon Exp $
# $Locker: $
#
# Makefile for BRender Matrox Mystique driver
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR
.IMPORT .IGNORE: MSI95_DIR

MSI95_DIR*:=msi95

TARGET_TYPE*:=LIB

.IF $(TARGET_TYPE) == LIB

BLD_BASE:=lib
LIB_PART=myst

CFLAGS=-DBrDrv1Begin=BrDrv1MystBegin

.ELIF $(TARGET_TYPE) == DRIVER

BLD_BASE:=drv

TARGET=myst_win.dll

LDLIBS+=\
	$(DDI_LIB_DIR)/fwimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(DDI_LIB_DIR)/hostimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(DDI_LIB_DIR)/pmimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(MSI95_DIR)/msi95.lib\

.ENDIF

INCLUDES+=\
	$(MSI95_DIR)\

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/device$(OBJ_EXT)\
	$(BLD_DIR)/outfcty$(OBJ_EXT)\
	$(BLD_DIR)/devpixmp$(OBJ_EXT)\
	$(BLD_DIR)/devpmsys$(OBJ_EXT)\
	$(BLD_DIR)/scale$(OBJ_EXT)\
	$(BLD_DIR)/devclut$(OBJ_EXT)\
	$(BLD_DIR)/plib$(OBJ_EXT)\
	$(BLD_DIR)/pstate$(OBJ_EXT)\
	$(BLD_DIR)/sbuffer$(OBJ_EXT)\
	$(BLD_DIR)/texture$(OBJ_EXT)\
	$(BLD_DIR)/match$(OBJ_EXT)\
	$(BLD_DIR)/rendfunc$(OBJ_EXT)\
	$(BLD_DIR)/log$(OBJ_EXT)\
	$(BLD_DIR)/stubmain$(OBJ_EXT)\

OBJS_ASM=\
	$(BLD_DIR)/magicsym$(OBJ_EXT)\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

