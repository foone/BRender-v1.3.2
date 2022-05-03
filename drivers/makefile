# Makefile for brender drivers directory
#
# $Id: makefile 1.1 1997/12/10 16:57:10 jon Exp $
# $Locker: $
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR

TARGET_TYPE*:=DRIVER

default .PHONY: target
all .PHONY: includes depend target

includes depend target clean .PHONY:
	+$(BR_MAKE_DIR)\subdir vesa      $(MAKE) "TARGET_TYPE=$(TARGET_TYPE)" $@
	+$(BR_MAKE_DIR)\subdir mcga      $(MAKE) "TARGET_TYPE=$(TARGET_TYPE)" $@
	+$(BR_MAKE_DIR)\subdir softrend  $(MAKE) "BR_BASE_TYPE=FLOAT" "TARGET_TYPE=$(TARGET_TYPE)" $@
	+$(BR_MAKE_DIR)\subdir softrend  $(MAKE) "BR_BASE_TYPE=FIXED" "TARGET_TYPE=$(TARGET_TYPE)" $@
	+$(BR_MAKE_DIR)\subdir pentprim  $(MAKE) "BR_BASE_TYPE=FLOAT" "TARGET_TYPE=$(TARGET_TYPE)" $@
	+$(BR_MAKE_DIR)\subdir pentprim  $(MAKE) "BR_BASE_TYPE=FIXED" "TARGET_TYPE=$(TARGET_TYPE)" $@

#	+$(BR_MAKE_DIR)\subdir ddraw     $(MAKE) "TARGET_TYPE=$(TARGET_TYPE)" $@

