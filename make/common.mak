# Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
#
# $Id: common.mak 1.1 1997/12/10 16:42:58 jon Exp $
# $Locker: $
#
# Common rules for all compilers
#

## Object directory
##
$(BLD_DIR)/exists:
	-+mkdir $(BLD_DIR)
	+echo $(BLD_DIR) >$(BLD_DIR)\exists

target: includes $(TARGET)

# Dependencies
#
SRCS:=$(OBJS_C:s/.obj/.c/:f) $(OBJS_CPP:s/.obj/.cpp/:f) $(OBJS_ASM:s/.obj/.asm/:f)

depend: includes $(BLD_DIR)/exists
.IF $(LIB_INC)
depend: $(BLD_DIR)/$(LIB_INC).inc
.END
.IF $(RES_IMAGE)
depend: $(RES_IMAGE).h
.END

depend::
	$(PERL) $(BR_MAKE_DIR)\mkdep.pl @$(mktmp -o $(BLD_DIR)/ -s \
-i $(BLD_DIR) -i $(INCLUDES:t" -i ":s/\/\\/) -x $(OBJ_EXT) $(SRCS)) > $(BLD_DIR)\depend.out
.IF $(LIB_INC)
	$(PERL) $(BR_MAKE_DIR)\mkdep.pl @$(mktmp -o $(BLD_DIR)/ -s \
-i $(BLD_DIR) -i $(INCLUDES:t" -i ":s/\/\\/) -x inc $(LIB_INC).h) >> $(BLD_DIR)\depend.out
.END

# Rule to generate all the include files
#
includes: $(BLD_DIR)/exists $(EXPORT_INCLUDES)

# Generate makefiles for sample code from standard templates
#
MAKEFILES=\
 makefile.wat\
 makefile.dst\
 makefile.bcc\
 makefile.msc\

$(MAKEFILES): $(BR_MAKE_DIR)/makegen.pl $(MAKEFILE:s/-f//)
	$(PERL) $(BR_MAKE_DIR)/makegen.pl $(BR_MAKE_DIR)/$@ >$@  < $(mktmp,t.txt \
?target ;$(EXE_EXT);;\n\
?object_c ;$(BLD_DIR:s,\,/,)/;;\n\
?object_c ;$(OBJ_EXT);;\n\
?object_cpp ;$(BLD_DIR:s,\,/,)/;;\n\
?object_cpp ;$(OBJ_EXT);;\n\
?object_asm ;$(BLD_DIR:s,\,/,)/;;\n\
?object_asm ;$(OBJ_EXT);;\n\
?library ;$(LIB_DIR:s,\,/,)/;;\n\
?library ;$(LIB_TYPE)$(LIB_EXT);$$(LIB_TYPE);\n\
?library ;$(LIB_TYPE_NB)$(LIB_EXT);$$(LIB_TYPE_NB);\n\
?library ;$(LIB_TYPE_EX)$(LIB_EXT);$$(LIB_TYPE_EX);\n\
\n\
target $(TARGET)\n\
$(!null,$(OBJS_C) object_c ) $(OBJS_C:t"\nobject_c ":s,\,/,)\n\
$(!null,$(OBJS_CPP) object_cpp ) $(OBJS_CPP:t"\nobject_cpp ":s,\,/,)\n\
$(!null,$(OBJS_ASM) object_asm ) $(OBJS_ASM:t"\nobject_asm ":s,\,/,)\n\
$(!null,$(LDLIBS) library ) $(LDLIBS:t"\nlibrary ":s,\,/,)\n\
)

makefiles: $(MAKEFILES)

# Tidying up
#
clean:
	-$(RM) *.i
	-$(RM) *.o
	-$(RM) *.obj
	-$(RM) *.err
	-$(RM) *.map
	-$(RM) *.BAK
	-$(RMDIR) $(BLD_DIR)

# Keep proper dependencies (per build type)
#
.KEEP_STATE:=$(BLD_DIR)/state.out

