# Top level makefile for brender core libraries
#
# $Id: makefile 1.1 1997/12/10 17:01:57 jon Exp $
# $Locker: $
#
.IMPORT: BR_SOURCE_DIR BR_MAKEFILE BR_TARGET_DIR BR_MAKE_DIR

default .PHONY: target
all .PHONY: includes depend target

includes depend target clean .PHONY:
	+$(BR_MAKE_DIR)\subdir core      $(MAKE) $@
	+$(BR_MAKE_DIR)\subdir drivers   $(MAKE) $@
#	+$(BR_MAKE_DIR)\subdir tools     $(MAKE) $@
#	+$(BR_MAKE_DIR)\subdir samples   $(MAKE) $@

