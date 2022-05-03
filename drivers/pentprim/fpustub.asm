;    Last change:  SL   25 Jul 96   12:06 pm
;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: fpustub.asm 1.1 1997/12/10 16:46:56 jon Exp $
;; $Locker: $
;;
;; Stubs to suppress FPU setup in DLL's
;;
	.386p
	.model flat
	.data

ifdef __DRIVER__

	public __8087
	public __init_387_emulator
	public _fltused_
	public __fltused

__8087			dd	0
__init_387_emulator	dd	0
_fltused_		dd	0
__fltused		dd	0

    .code
    public __PRO
    public __EPI
__PRO label near
__EPI label near
        ret
endif

	end

