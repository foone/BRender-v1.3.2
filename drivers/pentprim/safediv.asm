;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: safediv.asm 1.1 1997/12/10 16:48:09 jon Exp $
;; $Locker: $
;;
;;

	.386p
	.model flat,c
	.data
	db '$Id: safediv.asm 1.1 1997/12/10 16:48:09 jon Exp $',0
	align	4
	.code

; result = edx:eax/divisor (0 on overflow)
;
safediv		macro	divisor
		local	pos1,pos2,zero,nzero;
		
; Get sign of result into ebx

		mov	ecx,divisor
		mov	ebx,ecx
		xor	ebx,edx

		; Make divisor and dividend positive

		test	edx,edx
		jns	pos1
		neg	edx
		neg	eax
		sbb	edx,0
pos1:
		; edx:eax now positive

		test	ecx,ecx
		jz	zero
		jns	pos2
		neg	ecx
pos2:
		; Compare for overflow

		cmp	edx,ecx
		jae	zero
		; edx<ecx - div *must* be safe now or I'll eat my hat                                                                NOT!

		div	ecx

		; Give result the correct sign

		sar	ebx,31
		xor	eax,ebx
		sub	eax,ebx

		jmp	nzero
zero:
		xor	eax,eax
nzero:
		endm

; result = (a*b + c*d)/e, 0 if overflow
;
SafeFixedMac2Div proc uses edx ebx ecx, pa: dword, pb: dword, pc: dword, pd: dword, pe: dword
		mov	eax,pa
		imul	pb
		mov	ebx,eax
		mov	ecx,edx

		mov	eax,pc
		imul	pd
		add	eax,ebx
		adc	edx,ecx

		safediv	pe
		ret
zero:
		xor	eax,eax
		ret
SafeFixedMac2Div endp

; result = (a*b + c*d + e*f)/g, 0 if overflow
;
SafeFixedMac3Div proc uses edx ebx ecx, pa: dword, pb: dword, pc: dword, pd: dword, pe: dword, pf: dword, pg: dword
		mov	eax,pa
		imul	pb
		mov	ebx,eax
		mov	ecx,edx

		mov	eax,pc
		imul	pd
		add	ebx,eax
		adc	ecx,edx

		mov	eax,pe
		imul	pf
		add	eax,ebx
		adc	edx,ecx

		safediv	pg
		ret
zero:
		xor	eax,eax
		ret
SafeFixedMac3Div endp

		end

