;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: vesabank.asm 1.1 1997/12/10 16:54:34 jon Exp $
;; $Locker: $
;;
	.386p
	.model flat,c

	include drv.inc

	.data
		db '$Id: vesabank.asm 1.1 1997/12/10 16:54:34 jon Exp $',0
		align 4
	.code

; Function to map proteced mode vesa access to int10
;
VESABankFunctionInt10 proc
		int	10h
		ret
VESABankFunctionInt10 endp
	
	end

