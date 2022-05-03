;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: sar16.asm 1.1 1997/12/10 16:48:10 jon Exp $
;; $Locker: $
;;
;; Out of line call for gauranteed SAR
;;
	.386p
	.model flat
ifndef __WATCOMC__
	.code
__sar16	proc
	mov	eax,4[esp]
	sar	eax,16
	ret
__sar16	endp
endif

	end

