# Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
#
# $Id: visualc.mak 1.1 1997/12/10 16:43:49 jon Exp $
# $Locker: $
#
# General defines, rules and macros for BRender built with 
# Microsoft Visual C++ 2.0 upwards (currently 4.2).
#

# Compiler part of library name - Microsoft
#
LIB_SUFFIX_C:=m
LIB_SUFFIX_L:=m

# Commands
#
CC:=cl
CPPC:=cl
CPP:=cl -E
LIB:=lib
LIBFLAGS:=-q -b
LINK:=link

CFLAGS+=-W3

.INCLUDE .IGNORE: $(BR_MAKE_DIR)/vc_gen.mak

