# Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
#
# $Id: bcheck.mak 1.1 1997/12/10 16:42:50 jon Exp $
# $Locker: $
#
# General defines, rules and macros for BRender built with 
# Bounds Checker Pro
#

# Compiler part of library name - bounds Checker
#
LIB_SUFFIX_C:=c
LIB_SUFFIX_L:=m

# Commands
#
CC:=bcompile
CPPC:=bcompile
CPP:=cl -E
LIB:=lib
LIBFLAGS:=-q -b
LINK:=link

CFLAGS+=-Zop $(BR_MAKE_DIR)\bcheck.ini

.INCLUDE .IGNORE: $(BR_MAKE_DIR)/vc_gen.mak

