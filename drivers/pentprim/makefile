# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: makefile 1.1 1997/12/10 16:47:15 jon Exp $
# $Locker: $
#
# Makefile for BRender Software primitives driver
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR
.IMPORT .IGNORE: BR_USE_MMX
.IF $(BR_BASE_TYPE) ==
.IMPORT: BR_BASE_TYPE
.ENDIF

# Set to true if MMX renderers should be built
#
BR_USE_MMX*:=1

TARGET_TYPE*:=LIB

LIB_INC:=drv

.IF $(TARGET_TYPE) == LIB

AUTOLOAD*:=0

BLD_BASE:=lib
LIB_PART=prim

CFLAGS=-DBrDrv1Begin=BrDrv1SoftPrimBegin

.ELIF $(TARGET_TYPE) == DRIVER

AUTOLOAD*:=1

BLD_BASE:=drv

TARGET=softprm$(LIB_SUFFIX_T).dll

LDLIBS+=\
	$(DDI_LIB_DIR)/fwimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(DDI_LIB_DIR)/hostimg$(LIB_SUFFIX_C)$(LIB_EXT)\
	$(LIB_DIR)/brmt$(LIB_TYPE)$(LIB_EXT)\

.ENDIF

CFLAGS += -DPARTS=0xffff -DAUTOLOAD=$(AUTOLOAD) -DUSE_MMX=$(BR_USE_MMX)
ASFLAGS += -DPARTS=0ffffh -DAUTOLOAD=$(AUTOLOAD) -DUSE_MMX=$(BR_USE_MMX)

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/driver$(OBJ_EXT)\
	$(BLD_DIR)/device$(OBJ_EXT)\
	$(BLD_DIR)/object$(OBJ_EXT)\
	$(BLD_DIR)/plib$(OBJ_EXT)\
	$(BLD_DIR)/pstate$(OBJ_EXT)\
	$(BLD_DIR)/sbuffer$(OBJ_EXT)\
	$(BLD_DIR)/match$(OBJ_EXT)\
	$(BLD_DIR)/work$(OBJ_EXT)\

OBJS_ASM=\
	$(BLD_DIR)/magicsym$(OBJ_EXT)\

XOBJS_C=\
	$(BLD_DIR)/frcp$(OBJ_EXT)\
	$(BLD_DIR)/rcp$(OBJ_EXT)\
	$(BLD_DIR)/dither$(OBJ_EXT)\
\
	$(BLD_DIR)/awtmz$(OBJ_EXT)\
	$(BLD_DIR)/l_piz$(OBJ_EXT)\
	$(BLD_DIR)/p_piz$(OBJ_EXT)\
	$(BLD_DIR)/perspz$(OBJ_EXT)\
\
	$(BLD_DIR)/awtm$(OBJ_EXT)\
	$(BLD_DIR)/l_pi$(OBJ_EXT)\
	$(BLD_DIR)/p_pi$(OBJ_EXT)\
	$(BLD_DIR)/persp$(OBJ_EXT)\

XOBJS_ASM=\
	$(BLD_DIR)/safediv$(OBJ_EXT)\
	$(BLD_DIR)/sar16$(OBJ_EXT)\
	$(BLD_DIR)/decal$(OBJ_EXT)\
\
	$(BLD_DIR)/t_piza$(OBJ_EXT)\
	$(BLD_DIR)/ti8_piz$(OBJ_EXT)\
	$(BLD_DIR)/ti8_pizp$(OBJ_EXT)\
	$(BLD_DIR)/t15_pizp$(OBJ_EXT)\
	$(BLD_DIR)/tt15_piz$(OBJ_EXT)\
	$(BLD_DIR)/tt16_piz$(OBJ_EXT)\
	$(BLD_DIR)/tt24_piz$(OBJ_EXT)\
\
	$(BLD_DIR)/t_pia$(OBJ_EXT)\
	$(BLD_DIR)/ti8_pi$(OBJ_EXT)\
	$(BLD_DIR)/ti8_pip$(OBJ_EXT)\
	$(BLD_DIR)/t15_pip$(OBJ_EXT)\
	$(BLD_DIR)/tt15_pi$(OBJ_EXT)\
	$(BLD_DIR)/tt16_pi$(OBJ_EXT)\
	$(BLD_DIR)/tt24_pi$(OBJ_EXT)\
\
	$(BLD_DIR)/zb8$(OBJ_EXT)\
	$(BLD_DIR)/zb8sh$(OBJ_EXT)\
	$(BLD_DIR)/zb8p2lit$(OBJ_EXT)\
	$(BLD_DIR)/zb8p2fla$(OBJ_EXT)\
	$(BLD_DIR)/zb8p2ulb$(OBJ_EXT)\
	$(BLD_DIR)/zb8p2ltb$(OBJ_EXT)\
	$(BLD_DIR)/zb8p2flb$(OBJ_EXT)\
	$(BLD_DIR)/zb8p2unl$(OBJ_EXT)\
	$(BLD_DIR)/zb8awtm$(OBJ_EXT)\
	$(BLD_DIR)/zs8$(OBJ_EXT)\
	$(BLD_DIR)/zs8p2unl$(OBJ_EXT)\
	$(BLD_DIR)/zs8awtm$(OBJ_EXT)\
	$(BLD_DIR)/fpsetup$(OBJ_EXT)\
	$(BLD_DIR)/fti8_piz$(OBJ_EXT)\
\
	$(BLD_DIR)/zb16p2ul$(OBJ_EXT)\
\
	$(BLD_DIR)/pfpsetup$(OBJ_EXT)\
	$(BLD_DIR)/fti8pizp$(OBJ_EXT)\
	$(BLD_DIR)/fti8_pip$(OBJ_EXT)\

.IF $(AUTOLOAD) == 0

OBJS_C += $(XOBJS_C)
OBJS_ASM += $(XOBJS_ASM)

.ENDIF

.IF $(BR_USE_MMX) == 1
OBJS_C+=\

OBJS_ASM+=\
	$(BLD_DIR)/rastbuff$(OBJ_EXT)\
	$(BLD_DIR)/setupdat$(OBJ_EXT)\


.IF $(BR_BASE_TYPE) == FLOAT
OBJS_ASM+=\
	$(BLD_DIR)/gsetuptf$(OBJ_EXT)\

.ELSE
OBJS_ASM+=\
	$(BLD_DIR)/gsetuptx$(OBJ_EXT)\

.ENDIF

XOBJS_C+=\

XOBJS_ASM+=\
	$(BLD_DIR)/rastrise$(OBJ_EXT)\
	$(BLD_DIR)/xzuv$(OBJ_EXT)\
	$(BLD_DIR)/xzuvc$(OBJ_EXT)\
	$(BLD_DIR)/xzrgb$(OBJ_EXT)\
	$(BLD_DIR)/xzuvrgb$(OBJ_EXT)\
	$(BLD_DIR)/xzuv555$(OBJ_EXT)\
	$(BLD_DIR)/xzuvc555$(OBJ_EXT)\
	$(BLD_DIR)/xzrgb555$(OBJ_EXT)\
	$(BLD_DIR)/xzuvrgb5$(OBJ_EXT)\

.ENDIF


PRIM_FILES=\
	prim_l15.c\
	prim_l16.c\
	prim_l24.c\
	prim_l8.c\
	prim_p15.c\
	prim_p16.c\
	prim_p24.c\
	prim_p8.c\
	prim_t24.c\

.IF $(BR_BASE_TYPE) == FLOAT

PRIM_FILES+=\
	prim_t8f.c\
	prm_t15f.c\
	prm_t16f.c\
	mmx_t15f.c\
	mmx_t16f.c\

.ELSE

PRIM_FILES+=\
	prim_t8x.c\
	prm_t15x.c\
	prm_t16x.c\
	mmx_t15x.c\
	mmx_t16x.c\

.ENDIF

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

# Build primitive info tables from a script
#
.IF $(PRIM_FILES)
$(PRIM_FILES): infogen.pl

p%.c: p%.ifg
	$(PERL) infogen.pl < p$*.ifg > $@

p%f.c: p%.ifg
	$(PERL) infogen.pl < p$*.ifg > $@ image_suffix=f float_components

m%f.c: m%.ifg
	$(PERL) infogen.pl < m$*.ifg > $@ float_components

p%x.c: p%.ifg
	$(PERL) infogen.pl < p$*.ifg > $@ image_suffix=x

m%x.c: m%.ifg
	$(PERL) infogen.pl < m$*.ifg > $@

# Build rasteriser drivers

.IF $(TARGET_TYPE) == DRIVER

includes depend target clean .PHONY::
	@+$(MAKE) "BR_BASE_TYPE=$(BR_BASE_TYPE)" "TARGET_TYPE=$(TARGET_TYPE)" "BR_USE_MMX=$(BR_USE_MMX)" "AUTOLOAD=$(AUTOLOAD)" -f softilp.mak $@
	@+$(MAKE) "BR_BASE_TYPE=$(BR_BASE_TYPE)" "TARGET_TYPE=$(TARGET_TYPE)" "BR_USE_MMX=$(BR_USE_MMX)" "AUTOLOAD=$(AUTOLOAD)" -f softizlp.mak $@
	@+$(MAKE) "BR_BASE_TYPE=$(BR_BASE_TYPE)" "TARGET_TYPE=$(TARGET_TYPE)" "BR_USE_MMX=$(BR_USE_MMX)" "AUTOLOAD=$(AUTOLOAD)" -f softi.mak $@
	@+$(MAKE) "BR_BASE_TYPE=$(BR_BASE_TYPE)" "TARGET_TYPE=$(TARGET_TYPE)" "BR_USE_MMX=$(BR_USE_MMX)" "AUTOLOAD=$(AUTOLOAD)" -f softiz.mak $@
	@+$(MAKE) "BR_BASE_TYPE=$(BR_BASE_TYPE)" "TARGET_TYPE=$(TARGET_TYPE)" "BR_USE_MMX=$(BR_USE_MMX)" "AUTOLOAD=$(AUTOLOAD)" -f softizf.mak $@
	@+$(MAKE) "BR_BASE_TYPE=$(BR_BASE_TYPE)" "TARGET_TYPE=$(TARGET_TYPE)" "BR_USE_MMX=$(BR_USE_MMX)" "AUTOLOAD=$(AUTOLOAD)" -f softizb.mak $@
	@+$(MAKE) "BR_BASE_TYPE=$(BR_BASE_TYPE)" "TARGET_TYPE=$(TARGET_TYPE)" "BR_USE_MMX=$(BR_USE_MMX)" "AUTOLOAD=$(AUTOLOAD)" -f softizfb.mak $@
	@+$(MAKE) "BR_BASE_TYPE=$(BR_BASE_TYPE)" "TARGET_TYPE=$(TARGET_TYPE)" "BR_USE_MMX=$(BR_USE_MMX)" "AUTOLOAD=$(AUTOLOAD)" -f softhlp.mak $@
	@+$(MAKE) "BR_BASE_TYPE=$(BR_BASE_TYPE)" "TARGET_TYPE=$(TARGET_TYPE)" "BR_USE_MMX=$(BR_USE_MMX)" "AUTOLOAD=$(AUTOLOAD)" -f softhzlp.mak $@
	@+$(MAKE) "BR_BASE_TYPE=$(BR_BASE_TYPE)" "TARGET_TYPE=$(TARGET_TYPE)" "BR_USE_MMX=$(BR_USE_MMX)" "AUTOLOAD=$(AUTOLOAD)" -f softh.mak $@
	@+$(MAKE) "BR_BASE_TYPE=$(BR_BASE_TYPE)" "TARGET_TYPE=$(TARGET_TYPE)" "BR_USE_MMX=$(BR_USE_MMX)" "AUTOLOAD=$(AUTOLOAD)" -f softhz.mak $@
	@+$(MAKE) "BR_BASE_TYPE=$(BR_BASE_TYPE)" "TARGET_TYPE=$(TARGET_TYPE)" "BR_USE_MMX=$(BR_USE_MMX)" "AUTOLOAD=$(AUTOLOAD)" -f softt.mak $@
	@+$(MAKE) "BR_BASE_TYPE=$(BR_BASE_TYPE)" "TARGET_TYPE=$(TARGET_TYPE)" "BR_USE_MMX=$(BR_USE_MMX)" "AUTOLOAD=$(AUTOLOAD)" -f softtz.mak $@
	@+$(MAKE) "BR_BASE_TYPE=$(BR_BASE_TYPE)" "TARGET_TYPE=$(TARGET_TYPE)" "BR_USE_MMX=$(BR_USE_MMX)" "AUTOLOAD=$(AUTOLOAD)" -f softb.mak $@
	@+$(MAKE) "BR_BASE_TYPE=$(BR_BASE_TYPE)" "TARGET_TYPE=$(TARGET_TYPE)" "BR_USE_MMX=$(BR_USE_MMX)" "AUTOLOAD=$(AUTOLOAD)" -f softd.mak $@
	@+$(MAKE) "BR_BASE_TYPE=$(BR_BASE_TYPE)" "TARGET_TYPE=$(TARGET_TYPE)" "BR_USE_MMX=$(BR_USE_MMX)" "AUTOLOAD=$(AUTOLOAD)" -f mmx.mak $@

.ENDIF

.END

