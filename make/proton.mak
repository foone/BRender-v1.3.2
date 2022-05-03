# Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
#
# $Id: proton.mak 1.1 1997/12/10 16:43:43 jon Exp $
# $Locker: $
#
# General defines, rules and macros for BRender built with 
# Intel Reference Compiler (Proton)
#

# Compiler part of library name - Proton
#
LIB_SUFFIX_C:=p
LIB_SUFFIX_L:=m

# Commands
#
CC:=icl /nologo
CPPC:=icl /nologo
CPP:=icl /nologo -E
LIB:=lib
LIBFLAGS:=-q -b
LINK:=link

CFLAGS+= -w

.INCLUDE .IGNORE: $(BR_MAKE_DIR)/vc_gen.mak

