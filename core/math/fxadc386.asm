;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: fxadc386.asm 1.1 1997/12/10 16:41:21 jon Exp $
;; $Locker: $
;;
fixed typedef dword

	.386p
	.model flat,c

if ADD_RCS_ID
	.data
		db '$Id: fxadc386.asm 1.1 1997/12/10 16:41:21 jon Exp $',0
	align	4
endif
	.code

; br_fixed_ls BR_ASM_CALL BrFixedAddCarry(br_fixed_ls a, br_fixed_ls b, char * flag);
;
; return a+b and return flag if carry from bit 15
;

BrFixedAddCarry proc uses ebx edx, param_a: fixed, param_b: fixed, param_flag: ptr byte
		mov	ebx,param_flag
		xor	eax,eax
		mov	byte ptr [ebx],0

		mov	ax,word ptr param_a
		mov	dx,word ptr param_a+2

		add	ax,word ptr param_b
		jnc	no_carry
		mov	byte ptr [ebx],-1
no_carry:
		adc	dx,word ptr param_b+2
		shl	edx,16
		or	eax,edx
		ret
BrFixedAddCarry endp


	end

