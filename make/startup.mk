# MSDOS DMAKE startup file.  Customize to suit your needs.
#
# Disable warnings for macros redefined here that were given
# on the command line.

__.SILENT := $(.SILENT)
.SILENT   := yes

# Applicable suffix definitions
A := .lib	# Libraries
E := .exe	# Executables
O := .obj	# Objects
S := .asm	# Assembler sources

# See if these are defined
TMPDIR := $(ROOTDIR)/tmp
.IMPORT .IGNORE : TMPDIR SHELL COMSPEC

# Recipe execution configurations
# First set SHELL, If it is not defined, use COMSPEC, otherwise
# it is assumed to be MKS Korn SHELL.
.IF $(SHELL) == $(NULL)
.IF $(COMSPEC) == $(NULL)
   SHELL := $(ROOTDIR)/bin/sh$E
.ELSE
   SHELL := $(COMSPEC)
.END
.END
GROUPSHELL := $(SHELL)

# Now set remaining arguments depending on which SHELL we
# are going to use.  COMSPEC (assumed to be command.com) or
# MKS Korn Shell.
.IF $(SHELL)==$(COMSPEC)
   SHELLFLAGS  := /c
   GROUPFLAGS  := $(SHELLFLAGS)
   SHELLMETAS  := *"?<>
   GROUPSUFFIX := .bat
   DIRSEPSTR   := \\
   DIVFILE      = $(TMPFILE:s,/,\)
.ELSE
   SHELLFLAGS  := -c
   GROUPFLAGS  := 
   SHELLMETAS  := *"?<>|()&][$$\#`'
   GROUPSUFFIX := .ksh
   .MKSARGS    := yes
   DIVFILE      = $(TMPFILE:s,/,${DIVSEP_shell_${USESHELL}})
   DIVSEP_shell_yes := \\\
   DIVSEP_shell_no  := \\
.END

# Standard C-language command names and flags
   CC      := wcl386		# C-compiler and flags
   CFLAGS  +=

   AS      := ml		# Assembler and flags
   ASFLAGS +=

   LD       = wlink		# Loader and flags
   LDFLAGS +=
   LDLIBS   =

# Definition of $(MAKE) macro for recursive makes.
   MAKE = $(MAKECMD) $(MFLAGS)

# Other Compilers, Tools and their flags
   AR     := lib			# archiver
   ARFLAGS+=

   RM	   := rm		# remove a file command
   RMFLAGS +=

# Implicit generation rules for making inferences.
# We don't provide .yr or .ye rules here.  They're obsolete.
# Rules for making *$O
   %$O : %.c ; $(CC) $(CFLAGS) -c $<
   %$O : %.cpp ; $(CC) $(CFLAGS) -c $<
   %$O : %$S ; $(AS) $(ASFLAGS) -c $<

#   %$O : %$S ; $(AS) $(ASFLAGS) $(<:s,/,\);
#   %$O : %$P ; $(PC) $(PFLAGS) -c $<
#   %$O : %.cl ; class -c $<
#   %$O : %.e %.r %.F %$F ; $(FC) $(RFLAGS) $(EFLAGS) $(FFLAGS) -c $<

# Executables
   %$E : %$O ; $(CC) $(LDFLAGS) -o$@ $< $(LDLIBS)

# DMAKE uses this recipe to remove intermediate targets
.REMOVE :; $(RM) -f $<

# Turn warnings back to previous setting.
.SILENT := $(__.SILENT)

# Local init file if any, gets parsed before user makefile
.INCLUDE .IGNORE: "_startup.mk"

