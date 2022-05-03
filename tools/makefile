# Makefile for brender tools
#
# $Id: makefile 1.2 1997/12/11 19:22:09 jon Exp $
# $Locker: $
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR

default .PHONY: target
all .PHONY: includes depend target

includes depend target clean .PHONY:
	+$(BR_MAKE_DIR)\subdir modtest   $(MAKE) $@
	+$(BR_MAKE_DIR)\subdir timetest  $(MAKE) $@
	+$(BR_MAKE_DIR)\subdir mkranges  $(MAKE) $@
	+$(BR_MAKE_DIR)\subdir mkshades  $(MAKE) $@
	+$(BR_MAKE_DIR)\subdir dquery    $(MAKE) $@
	+$(BR_MAKE_DIR)\subdir texconv   $(MAKE) $@
	+$(BR_MAKE_DIR)\subdir geoconv   $(MAKE) $@
	+$(BR_MAKE_DIR)\subdir 3ds2br    $(MAKE) $@
	+$(BR_MAKE_DIR)\subdir mkblend   $(MAKE) $@
	+$(BR_MAKE_DIR)\subdir mkfog     $(MAKE) $@
	+$(BR_MAKE_DIR)\subdir paljoin   $(MAKE) $@
	+$(BR_MAKE_DIR)\subdir testview  $(MAKE) $@
	+$(BR_MAKE_DIR)\subdir trackmem  $(MAKE) $@
	+$(BR_MAKE_DIR)\subdir viewpal   $(MAKE) $@

