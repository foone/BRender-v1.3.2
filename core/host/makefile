# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: makefile 1.1 1997/12/10 17:16:30 jon Exp $
# $Locker: $
#
# Makefile for BRender DOS Host support libary
#
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR
.IMPORT .IGNORE: BR_EXTENDER

BR_BASE_TYPE=NONE
TARGET_TYPE:=LIB
CFLAGS+=-D__BR_V1DB__=0
LIB_INC:=host
RES_IMAGE=hostimg

# Select flags and library name based on extender type
#
.IF $(BR_EXTENDER) == PHARLAP

CFLAGS+=-D__PHARLAP386__=1 -D__DOS__=1
ASFLAGS+=-D__PHARLAP386__=1  -D__DOS__=1
LIB_PART:=hstpl
BLD_BASE:=bpl

.ELIF $(BR_EXTENDER) == X32

CFLAGS+=-D__X32__=1 -D__DOS__=1
ASFLAGS+=-D__X32__=1  -D__DOS__=1
LIB_PART:=hstx3
BLD_BASE:=bx3

.ELIF $(BR_EXTENDER) == X32VM

CFLAGS+=-D__X32__=1  -D__DOS__=1
ASFLAGS+=-D__X32__=1 -D__DOS__=1
LIB_PART:=hstx3
BLD_BASE:=bx3

.ELIF $(BR_EXTENDER) == DOS4GW

CFLAGS+=-D__DOS4G__=1 -D__DPMI__=1 -D__DOS__=1
ASFLAGS+=-D__DOS4G__=1 -D__DPMI__=1 -D__DOS__=1
LIB_PART:=hst4g
BLD_BASE:=b4g

.ELIF $(BR_EXTENDER) == DOS4GWPRO

CFLAGS+=-D__DOS4G__=1 -D__DPMI__=1 -D__DOS__=1
ASFLAGS+=-D__DOS4G__=1 -D__DPMI__=1 -D__DOS__=1
LIB_PART:=hst4g
BLD_BASE:=b4g

.ELIF $(BR_EXTENDER) == POWERPACK

CFLAGS+=-D__POWERPACK__=1 -D__DPMI__=1 -D__DOS__=1 -WX
ASFLAGS+=-D__POWERPACK__=1 -D__DPMI__=1 -D__DOS__=1
LIB_PART:=hstpp
BLD_BASE:=bpp

.ELIF $(BR_EXTENDER) == GO32

CFLAGS+=-D__GO32__=1 -D__DPMI__=1 -D__DOS__=1
ASFLAGS+=-D__GO32__=1 -D__DPMI__=1 -D__DOS__=1
LIB_PART:=hstdj
BLD_BASE:=bdj

.ELIF $(BR_EXTENDER) == WIN_32

CFLAGS+=-D__WIN_32__=1 -U__DOS__
ASFLAGS+=-D__WIN_32__=1
LIB_PART:=hstwn
BLD_BASE:=bwn

.IF $(BR_MAKEFILE) == watcom.mak
CFLAGS +=-bt=nt
.END

.END

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/himage$(OBJ_EXT)\
	$(BLD_DIR)/hook$(OBJ_EXT)\
	$(BLD_DIR)/hostcfg$(OBJ_EXT)\
	$(BLD_DIR)/hstsetup$(OBJ_EXT)\
	$(BLD_DIR)/memmgmt$(OBJ_EXT)\
	$(BLD_DIR)/ports$(OBJ_EXT)\

OBJS_ASM=\
	$(BLD_DIR)/cpuid$(OBJ_EXT)\
	$(BLD_DIR)/farmem$(OBJ_EXT)\
	$(BLD_DIR)/prot$(OBJ_EXT)\
	$(BLD_DIR)/real$(OBJ_EXT)\
	$(BLD_DIR)/realmem$(OBJ_EXT)\
	$(BLD_DIR)/segregs$(OBJ_EXT)\

EXPORT_INCLUDES=\
	$(DDI_INC_DIR)/host.h\
	$(DDI_INC_DIR)/host_p.h\

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

