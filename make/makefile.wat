{{}} Copyright (c) 1996 Argonaut Software Ltd. All rights reserved.
{{}}
{{}} A template for demo makefiles
{{}}
{{}} $Id: makefile.wat 1.1 1997/12/10 16:43:27 jon Exp $
{{}} $Locker: $
{{}}
# Makefile for Watcom C using WMAKE
#
TARGET={{target}}.exe

BASE_DIR=..\..\..

# Float, Release, Register
#
LIB_TYPE=wfrr
LIB_TYPE_NB=wrr
LIB_TYPE_EX=4gwrr
BASED=FLOAT

# Fixed, Release, Register
#
#LIB_TYPE=wxrr
#LIB_TYPE_NB=wrr
#LIB_TYPE_EX=4gwrr
#BASED=FIXED

OBJS=&
	{{object_c}}.obj&
	{{object_cpp}}.obj&
	{{object_asm}}.obj&

LIBS=&
	$(BASE_DIR)/lib/{{library}}.lib&

# Default rules
#
{?object_c?}CC=wcc386
{?object_c?}CFLAGS=-omaxnet -zp4 -5r -fp5 -DBASED_$(BASED)=1 -I$(BASE_DIR)\inc
{?object_c?}
{?object_c?}.c.obj:
{?object_c?}	$(CC) $(CFLAGS) $<
{?object_c?}
{?object_cpp?}CPPC=wpp386
{?object_cpp?}CPPFLAGS=-omaxnet -zp4 -5r -fp5 -DBASED_$(BASED)=1 -I$(BASE_DIR)\inc
{?object_cpp?}
{?object_cpp?}.cpp.obj:
{?object_cpp?}	$(CPPC) $(CPPFLAGS) $<
{?object_cpp?}
{?object_asm?}AS=ml
{?object_asm?}ASFLAGS=-c -DBASED_$(BASED)=1 -I$(BASE_DIR)\inc
{?object_asm?}
{?object_asm?}.asm.obj:
{?object_asm?}	$(AS) $(ASFLAGS) $<
{?object_asm?}

$(TARGET): $(OBJS) wlink.rsp
	wlink @wlink.rsp

# Link response file
#
wlink.rsp: makefile.wat
	echo name $(TARGET) >wlink.rsp
	echo system dos4g >>wlink.rsp
	echo option caseexact,quiet,stack=64k >>wlink.rsp
	for %i in ($(OBJS)) do echo file %i >>wlink.rsp
	for %i in ($(LIBS)) do echo library %i >>wlink.rsp

