# Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
#
# $Id: makefile 1.2 1998/11/12 13:15:14 johng Exp $
# $Locker: $
#
# Makefile for BRender Framework
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR

BR_BASE_TYPE=NONE

LIB_PART:=brfw
TARGET_TYPE:=LIB
CFLAGS+=-D__BR_V1DB__=0
RES_IMAGE=fwimg

## Base library object files
##
OBJS_C=\
	$(BLD_DIR)/brbegin$(OBJ_EXT)\
	$(BLD_DIR)/brbhook$(OBJ_EXT)\
	$(BLD_DIR)/brlists$(OBJ_EXT)\
	$(BLD_DIR)/brqsort$(OBJ_EXT)\
	$(BLD_DIR)/bswap$(OBJ_EXT)\
	$(BLD_DIR)/datafile$(OBJ_EXT)\
	$(BLD_DIR)/devlist$(OBJ_EXT)\
	$(BLD_DIR)/devsetup$(OBJ_EXT)\
	$(BLD_DIR)/diag$(OBJ_EXT)\
	$(BLD_DIR)/error$(OBJ_EXT)\
	$(BLD_DIR)/file$(OBJ_EXT)\
	$(BLD_DIR)/fwsetup$(OBJ_EXT)\
	$(BLD_DIR)/genfile$(OBJ_EXT)\
	$(BLD_DIR)/image$(OBJ_EXT)\
	$(BLD_DIR)/lexer$(OBJ_EXT)\
	$(BLD_DIR)/loader$(OBJ_EXT)\
	$(BLD_DIR)/mem$(OBJ_EXT)\
	$(BLD_DIR)/object$(OBJ_EXT)\
	$(BLD_DIR)/objectc$(OBJ_EXT)\
	$(BLD_DIR)/pattern$(OBJ_EXT)\
	$(BLD_DIR)/pool$(OBJ_EXT)\
	$(BLD_DIR)/printf$(OBJ_EXT)\
	$(BLD_DIR)/register$(OBJ_EXT)\
	$(BLD_DIR)/resource$(OBJ_EXT)\
	$(BLD_DIR)/resreg$(OBJ_EXT)\
	$(BLD_DIR)/scratch$(OBJ_EXT)\
	$(BLD_DIR)/token$(OBJ_EXT)\
	$(BLD_DIR)/tokenval$(OBJ_EXT)\
	$(BLD_DIR)/assocarr$(OBJ_EXT)\
    $(BLD_DIR)/sys_conf$(OBJ_EXT)\
    $(BLD_DIR)/devquery$(OBJ_EXT)\

OBJS_ASM=\
	$(BLD_DIR)/blockops$(OBJ_EXT)\
	$(BLD_DIR)/magicsym$(OBJ_EXT)\

EXPORT_INCLUDES=\
	$(DDI_INC_DIR)/blockops.h\
	$(DDI_INC_DIR)/brassert.h\
	$(DDI_INC_DIR)/brddi.h\
	$(DDI_INC_DIR)/brddi_p.h\
	$(DDI_INC_DIR)/brexcept.h\
	$(DDI_INC_DIR)/brhton.h\
	$(DDI_INC_DIR)/brimage.h\
	$(INC_DIR)/brlists.h\
	$(DDI_INC_DIR)/datafile.h\
	$(DDI_INC_DIR)/dataf_p.h\
	$(DDI_INC_DIR)/dev_objs.cgh\
	$(DDI_INC_DIR)/dev_objs.hpp\
	$(DDI_INC_DIR)/formats.h\
	$(DDI_INC_DIR)/lexer.h\
	$(DDI_INC_DIR)/pretok.tok\
	$(DDI_INC_DIR)/register.h\
	$(DDI_INC_DIR)/shortcut.h\
	$(DDI_INC_DIR)/timing.h\
	$(DDI_INC_DIR)/tokenmap.h\
	$(INC_DIR)/pretok.h\
	$(INC_DIR)/brstdlib.h\


default: target

depend: pretok.c pretok.h

.INCLUDE: $(BR_MAKE_DIR)/$(BR_MAKEFILE)

"$(DDI_INC_DIR)/dev_objs.cgh": dev_objs.cgh
	$(COPY_FORCE) $< $@

"$(INC_DIR)/pretok.h": pretok.h
	$(COPY_FORCE) $< $@

# Build pre-defined token list
#
pretok.c toktype.c pretok.h .UPDATEALL: pretok.tok
	-$(SET_WRITEABLE) pretok.h
	-$(SET_WRITEABLE) pretok.c
	-$(SET_WRITEABLE) toktype.c
	perl $(BR_MAKE_DIR)/tokgen.pl $<

"$(DDI_INC_DIR)/pretok.tok": pretok.tok
	$(COPY) $< $@

# Convert device classes into C headers
#
dev_objs.cgh: dev_objs.hpp
	-$(SET_WRITEABLE) $@
	+$(CPP) -D__CLASSGEN__ $< | $(PERL) $(BR_MAKE_DIR)/classgen.pl $(@:b) > $@

