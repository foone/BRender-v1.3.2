;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: fixed386.asm 1.1 1997/12/10 16:41:21 jon Exp $
;; $Locker: $
;;
;; Various operations on fixed 16.16 numbers
;;
_F typedef dword
_FF typedef word

	.386p
	.model flat,c
if ADD_RCS_ID
	.data
	db '$Id: fixed386.asm 1.1 1997/12/10 16:41:21 jon Exp $',0
	align	4
endif
	.code

; _F FixedAbs(_F a);
;
; result = abs(a)
;
BrFixedAbs proc uses edx, pa: _F
		mov	eax,pa
		test	eax,eax
		jns	exit
		neg	eax
exit:		ret
BrFixedAbs endp

; _F FixedMul(_F a, _F b);
;
; result = a*b
;
BrFixedMul proc uses edx, pa: _F, pb: _F
		mov	eax,pa
		imul	pb
		shrd	eax,edx,16
		ret
BrFixedMul endp

; _F ASM_DECL FixedMac2(_F a, _F b, _F c, _F d);
;
; result = a*b + c*d
;
BrFixedMac2 proc uses edx ebx ecx, pa: _F, pb: _F, pc: _F, pd: _F
		mov	eax,pa
		imul	pb
		mov	ebx,eax
		mov	ecx,edx

		mov	eax,pc
		imul	pd
		add	eax,ebx
		adc	edx,ecx
		shrd	eax,edx,16
		ret
BrFixedMac2 endp


; _F ASM_DECL FixedMac3(_F a, _F b, _F c, _F d, _F e, _F f);
;
; result = a*b + c*d + e*f
;
BrFixedMac3 proc uses edx ebx ecx, pa: _F, pb: _F, pc: _F, pd: _F, pe: _F, pf: _F
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
		shrd	eax,edx,16
		ret
BrFixedMac3 endp

; _F ASM_DECL FixedMac4(_F a, _F b, _F c, _F d, _F e, _F f, _F g, _F h);
;
; result = a*b + c*d + e*f + g*h
;
BrFixedMac4 proc uses edx ebx ecx, pa: _F, pb: _F, pc: _F, pd: _F, pe: _F, pf: _F, pg: _F, ph: _F
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
		add	ebx,eax
		adc	ecx,edx

		mov	eax,pg
		imul	ph
		add	eax,ebx
		adc	edx,ecx
		shrd	eax,edx,16
		ret
BrFixedMac4 endp

; _F ASM_DECL FixedLength2(_F a, _F b);
;
; result = sqrt(a*a + b*b)
;
BrFixedLength2 proc uses ebx ecx edx esi, pa: _F, pb: _F
		mov	eax,pa
		imul	eax
		mov	ebx,eax
		mov	ecx,edx

		mov	eax,pb
		imul	eax
		add	ebx,eax
		adc	ecx,edx

		call	_Sqrt64
		ret
BrFixedLength2 endp


; _F ASM_DECL FixedLength3(_F a, _F b, _F c);
;
; result = sqrt(a*a + b*b + c*c)
;
BrFixedLength3 proc uses ebx ecx edx esi, pa: _F, pb: _F, pc: _F
		mov	eax,pa
		imul	eax
		mov	ebx,eax
		mov	ecx,edx

		mov	eax,pb
		imul	eax
		add	ebx,eax
		adc	ecx,edx

		mov	eax,pc
		imul	eax
		add	ebx,eax
		adc	ecx,edx

		call	_Sqrt64
		ret
BrFixedLength3 endp

; _F ASM_DECL FixedLength4(_F a, _F b, _F c, _F d);
;
; result = sqrt(a*a + b*b + c*c + d*d)
;
BrFixedLength4 proc uses ebx ecx edx esi, pa: _F, pb: _F, pc: _F, pd: _F
		mov	eax,pa
		imul	eax
		mov	ebx,eax
		mov	ecx,edx

		mov	eax,pb
		imul	eax
		add	ebx,eax
		adc	ecx,edx

		mov	eax,pc
		imul	eax
		add	ebx,eax
		adc	ecx,edx

		mov	eax,pd
		imul	eax
		add	ebx,eax
		adc	ecx,edx

		call	_Sqrt64
		ret
BrFixedLength4 endp

; _F ASM_DECL FixedRLength2(_F a, _F b);
;
; result = 1.0/sqrt(a*a + b*b) (low precision)
;
BrFixedRLength2 proc uses ebx ecx edx esi, pa: _F, pb: _F
		mov	eax,pa
		imul	eax
		mov	ebx,eax
		mov	ecx,edx

		mov	eax,pb
		imul	eax
		add	ebx,eax
		adc	ecx,edx

		call	_FastRSqrt64
		ret
BrFixedRLength2 endp


; _F ASM_DECL FixedRLength3(_F a, _F b, _F c);
;
; result = 1.0/sqrt(a*a + b*b + c*c) (low precision)
;
BrFixedRLength3 proc uses ebx ecx edx esi, pa: _F, pb: _F, pc: _F
		mov	eax,pa
		imul	eax
		mov	ebx,eax
		mov	ecx,edx

		mov	eax,pb
		imul	eax
		add	ebx,eax
		adc	ecx,edx

		mov	eax,pc
		imul	eax
		add	ebx,eax
		adc	ecx,edx

		call	_FastRSqrt64
		ret
BrFixedRLength3 endp

; _F ASM_DECL FixedRLength4(_F a, _F b, _F c, _f d);
;
; result = 1.0/sqrt(a*a + b*b + c*c +d*d) (low precision)
;
BrFixedRLength4 proc uses ebx ecx edx esi, pa: _F, pb: _F, pc: _F, pd: _F
		mov	eax,pa
		imul	eax
		mov	ebx,eax
		mov	ecx,edx

		mov	eax,pb
		imul	eax
		add	ebx,eax
		adc	ecx,edx

		mov	eax,pc
		imul	eax
		add	ebx,eax
		adc	ecx,edx

		mov	eax,pd
		imul	eax
		add	ebx,eax
		adc	ecx,edx

		call	_FastRSqrt64
		ret
BrFixedRLength4 endp

; _F FixedDiv(_F a, _F b);
;
; result = a/b
;
BrFixedDiv proc uses edx, pa: _F, pb: _F
		mov	edx,pa
		mov	eax,edx
		shl	eax,16
		sar	edx,16
		idiv	pb
		ret
BrFixedDiv endp

; _F FixedDivR(_F a, _F b);
;
; result = a/b (rounded towards 0)
;
BrFixedDivR proc uses edx, pa: _F, pb: _F
		mov	eax,pa
		cdq
		xchg	edx,eax
		shrd	eax,edx,16
		sar	edx,16
		idiv	pb
		ret
BrFixedDivR endp

; _F FixedDivF(_F a, _F b);
;
; result = a/b * 2^31 - Both a and b are posotive
;
BrFixedDivF proc uses edx, pa: _F, pb: _F
		mov	edx,pa
		xor	eax,eax
		sar	edx,1
		rcr	eax,1
		div	pb
		ret
BrFixedDivF endp

; _F _FixedDivP(_F a, _F b);
;
;
_FixedDivP proc uses edx, pa: _F, pb: _F

		mov	edx,pa

		mov	eax,edx
		shl	eax,30
		sar	edx,2
		idiv	pb
		neg	eax
		add	eax,eax
		add	eax,080000000h

		ret
_FixedDivP endp

; _F FixedMulDiv(_F a, _F b, _F c);
;
; result = a*b/c
;
BrFixedMulDiv proc uses edx, pa:_F, pb:_F, pc:_F
		mov	eax,pa
		imul	pb
		idiv	pc
		ret
BrFixedMulDiv endp

; _F ASM_DECL FixedMac2Div(_F a, _F b, _F c, _F d, _F e);
;
; result = (a*b + c*d)/e
;
BrFixedMac2Div proc uses edx ebx ecx, pa: _F, pb: _F, pc: _F, pd: _F, pe: _F
		mov	eax,pa
		imul	pb
		mov	ebx,eax
		mov	ecx,edx

		mov	eax,pc
		imul	pd
		add	eax,ebx
		adc	edx,ecx

		idiv	pe
		ret
BrFixedMac2Div endp


; _F ASM_DECL FixedMac3Div(_F a, _F b, _F c, _F d, _F e, _F f, _F g);
;
; result = (a*b + c*d + e*f)/g
;
BrFixedMac3Div proc uses edx ebx ecx, pa: _F, pb: _F, pc: _F, pd: _F, pe: _F, pf: _F, pg: _F
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

		idiv	pg
		ret
BrFixedMac3Div endp

; _F ASM_DECL FixedMac4Div(_F a, _F b, _F c, _F d, _F e, _F f, _F g, _F h, _F i);
;
; result = (a*b + c*d + e*f + g*h)/i
;
BrFixedMac4Div proc uses edx ebx ecx, pa: _F, pb: _F, pc: _F, pd: _F, pe: _F, pf: _F, pg: _F, ph: _F, pi: _F
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
		add	ebx,eax
		adc	ecx,edx

		mov	eax,pg
		imul	ph
		add	eax,ebx
		adc	edx,ecx

		idiv	pi
		ret
BrFixedMac4Div endp

;; Fraction operations
;;

; _F ASM_DECL FixedFMac2(_FF a, _F b, _FF c, _F d);
;
; result = a*b + c*d
;
; a & c are signed fractions (words 8000->7fff)
;
BrFixedFMac2 proc uses edx ebx ecx, pa: _FF, pb: _F, pc: _FF, pd: _F
		movsx	eax,pa
		imul	pb
		mov	ebx,eax
		mov	ecx,edx

		movsx	eax,pc
		imul	pd
		add	eax,ebx
		adc	edx,ecx
		shrd	eax,edx,15
		ret
BrFixedFMac2 endp

; _F ASM_DECL FixedFMac3(_FF a, _F b, _FF c, _F d, _FF e, _F f);
;
; result = a*b + c*d + e*f
;
; a, c & e are signed fractions (words 8000->7fff)
;
BrFixedFMac3 proc uses edx ebx ecx, pa: _FF, pb: _F, pc: _FF, pd: _F, pe: _FF, pf: _F
		movsx	eax,pa
		imul	pb
		mov	ebx,eax
		mov	ecx,edx

		movsx	eax,pc
		imul	pd
		add	ebx,eax
		adc	ecx,edx

		movsx	eax,pe
		imul	pf
		add	eax,ebx
		adc	edx,ecx
		shrd	eax,edx,15
		ret
BrFixedFMac3 endp

; _F ASM_DECL FixedFMac4(_FF a, _F b, _FF c, _F d, _FF e, _F f, _F g, _F h);
;
; result = a*b + c*d + e*f + g*h
;
; a, c & e are signed fractions (words 8000->7fff)
;
BrFixedFMac4 proc uses edx ebx ecx, pa: _FF, pb: _F, pc: _FF, pd: _F, pe: _FF, pf: _F, pg :_FF, ph :_F
		movsx	eax,pa
		imul	pb
		mov	ebx,eax
		mov	ecx,edx

		movsx	eax,pc
		imul	pd
		add	ebx,eax
		adc	ecx,edx

		movsx	eax,pe
		imul	pf
		add	ebx,eax
		adc	ecx,edx

		movsx	eax,pg
		imul	ph
		add	eax,ebx
		adc	edx,ecx
		shrd	eax,edx,15
		ret
BrFixedFMac4 endp


; _F BrFixedRcp(_F a);
;
; result = 1.0/a
;
BrFixedRcp proc uses edx, pa: _F
		mov	edx,1
		xor	eax,eax
		idiv	pa
		ret
BrFixedRcp endp

; _F BrFixedSqr(_F a);
;
; result = a*a
;
BrFixedSqr proc uses edx, pa: _F
		mov	eax,pa
		imul	eax
		shrd	eax,edx,16
		ret
BrFixedSqr endp

; _F ASM_DECL FixedSqr2(_F a, _F b);
;
; result = a*a + b*b
;
BrFixedSqr2 proc uses edx ebx ecx, pa: _F, pb: _F
		mov	eax,pa
		imul	eax
		mov	ebx,eax
		mov	ecx,edx

		mov	eax,pb
		imul	eax
		add	eax,ebx
		adc	edx,ecx
		shrd	eax,edx,16
		ret
BrFixedSqr2 endp


; _F ASM_DECL FixedSqr3(_F a, _F b, _F c);
;
; result = a*a + b*b + c*c
;
BrFixedSqr3 proc uses edx ebx ecx, pa: _F, pb: _F, pc: _F
		mov	eax,pa
		imul	eax
		mov	ebx,eax
		mov	ecx,edx

		mov	eax,pb
		imul	eax
		add	ebx,eax
		adc	ecx,edx

		mov	eax,pc
		imul	eax
		add	eax,ebx
		adc	edx,ecx
		shrd	eax,edx,16
		ret
BrFixedSqr3 endp

; _F ASM_DECL FixedSqr4(_F a, _F b, _F c, _F d);
;
; result = a*a + b*b + c*c + d*d
;
BrFixedSqr4 proc uses edx ebx ecx, pa: _F, pb: _F, pc: _F, pd: _F
		mov	eax,pa
		imul	eax
		mov	ebx,eax
		mov	ecx,edx

		mov	eax,pb
		imul	eax
		add	ebx,eax
		adc	ecx,edx

		mov	eax,pc
		imul	eax
		add	ebx,eax
		adc	ecx,edx

		mov	eax,pd
		imul	eax
		add	eax,ebx
		adc	edx,ecx
		shrd	eax,edx,16
		ret
BrFixedSqr4 endp


; INTERP_FUNCTION
;
; Given a 16 bit input, generate the output value of a function
; using a 256-word lookup table with interpolation between
; entries
;
; Table must have all deltas: -32768 <= delta  <= 32767
;
; ax = input
; ax = output
; eax, ebx, ecx & edx trashed
;

INTERP_FUNCTION	macro table
	; bx = split input into high and low (ebx:eax)
	;
		xor	ebx,ebx
		xchg	bl,ah

	; dx = delta to next entry in table
	;
		mov	cx,(table)[ebx*2]
		mov	dx,(table+2)[ebx*2]
		sub	edx,ecx

	; Scale delta by low byte of input
	;
		imul	dx
		mov	al,ah
		mov	ah,dl

	; Add base entry to interpolated delta
	;
		add	eax,ecx
	endm

; fixed_lf FixedSin(angle a)
;
;
BrFixedSin proc uses ebx ecx edx, input:word
		mov	ax,input
		INTERP_FUNCTION	sin_table
		cwde
		add	eax,eax
		ret
BrFixedSin endp


; fixed_lf FixedCos(angle a)
;
BrFixedCos proc uses ebx ecx edx, input:word
		mov	ax,input
		INTERP_FUNCTION	cos_table
		cwde
		add	eax,eax
		ret
BrFixedCos endp

; angle FixedASin(_F a)
;
BrFixedASin proc uses ebx ecx edx, input:_F
	; Offset so zero is in middle of table
	;
		mov	eax,input

		add	eax,10000h
		sar	eax,1

		INTERP_FUNCTION arcsin_table
		ret
BrFixedASin endp

; angle FixedACos(_F a)
;
BrFixedACos proc uses ebx ecx edx, input:_F
	; Offset so discontinuity is at ends of table
	;
		mov	eax,input
		add	eax,10000h
		sar	eax,1
		INTERP_FUNCTION arccos_table
		ret
BrFixedACos endp

; angle FixedATan2(_F a, _F b)
;
; Break in 8 octants, based on sign of X, sign of Y and relative magnitude
; of X to Y
;
;
;	0	x>0  y>0  |x| > |y| 	=       +atan(|y|/|x|)
;	1	x>0  y>0  |x| < |y| 	= PI/2  -atan(|x|/|y|)
;	2	x<0  y>0  |x| < |y| 	= PI/2  +atan(|x|/|y|)
;	3	x<0  y>0  |x| > |y| 	= PI    -atan(|y|/|x|)
;	4	x<0  y<0  |x| > |y| 	= PI    +atan(|y|/|x|)
;	5	x<0  y<0  |x| < |y| 	= PI*3/2-atan(|x|/|y|)
;	6	x>0  y<0  |x| < |y| 	= PI*3/2+atan(|x|/|y|)
;	7	x>0  y<0  |x| > |y| 	= PI*2  -atan(|y|/|x|)
;
BrFixedATan2 proc uses ebx ecx edx, x:_F, y:_F

		mov	ebx,y
		mov	ecx,x
		xor	eax,eax

	; Branch tree on the three conditions -> 8 octants
	;
		cmp	ecx,0		; sign of y
		jl	octant_4567
		je	y_zero

octant_0123:	cmp	ebx,0		; sign of x
		jl	octant_23
octant_01:	cmp	ebx,ecx		; |x| vs. |y|
		jl	octant_1
		je	boundary_01

octant_0:	mov	edx,ecx		; |y|/|x|
		div	ebx
		shr	eax,16
		INTERP_FUNCTION arctan_table
		ret

boundary_01:	mov	eax,02000H
		ret

octant_1:	mov	edx,ebx		; |x|/|y|
		div	ecx
		shr	eax,16
		INTERP_FUNCTION arctan_table
		neg	eax
		add	eax,04000H
		ret

octant_23:	neg	ebx
		cmp	ebx,ecx		; |x| vs. |y|
		jg	octant_3
		je	boundary_23

octant_2:	mov	edx,ebx		; |x|/|y|
		div	ecx
		shr	eax,16
		INTERP_FUNCTION arctan_table
		add	eax,04000H
		ret

boundary_23:	mov	eax,06000H
		ret

octant_3:	mov	edx,ecx		; |y|/|x|
		div	ebx
		shr	eax,16
		INTERP_FUNCTION arctan_table
		neg	eax
		add	eax,08000H
		ret

octant_4567:	neg	ecx
		cmp	ebx,0	; sign of x
		jg	octant_67

octant_45:	neg	ebx
		cmp	ebx,ecx		; |x| vs. |y|
		jl	octant_5
		je	boundary_45

octant_4:	mov	edx,ecx		; |y|/|x|
		div	ebx
		shr	eax,16
		INTERP_FUNCTION arctan_table
		add	eax,08000H
		ret

boundary_45:	mov	eax,0A000H
		ret

octant_5:	mov	edx,ebx		; |x|/|y|
		div	ecx
		shr	eax,16
		INTERP_FUNCTION arctan_table
		neg	eax
		add	eax,0C000H
		ret

octant_67:	cmp	ebx,ecx		; |x| vs. |y|
		jg	octant_7
		je	boundary_67

octant_6:	mov	edx,ebx		; |x|/|y|
		div	ecx
		shr	eax,16
		INTERP_FUNCTION arctan_table
		add	eax,0C000H
		ret

boundary_67:	mov	eax,0E000H
		ret

octant_7:	mov	edx,ecx		; |y|/|x|
		div	ebx
		shr	eax,16
		INTERP_FUNCTION arctan_table
		neg	eax
		ret

	; Y component is zero
	;
y_zero:		cmp	ebx,0	; sign of x
		jl	x_lt_zero
		mov	eax,00h		; angle is 0
		ret

x_lt_zero:	mov	eax,08000h	; angle is PI 
		ret

BrFixedATan2 endp

; angle FixedATan2Fast(_F a, _F b)
;
; Less accurate atan2() - approximate atan(a) to (PI/4 * a) for 0-PI/4
;
; Break in 8 octants, based on sign of X, sign of Y and relative magnitude
; of X to Y
;
;	0	x>0  y>0  |x| > |y| 	=        PI/4*(|y|/|x|)
;	1	x>0  y>0  |x| < |y| 	= PI/2  -PI/4*(|x|/|y|)
;	2	x<0  y>0  |x| < |y| 	= PI/2  +PI/4*(|x|/|y|)
;	3	x<0  y>0  |x| > |y| 	= PI    -PI/4*(|y|/|x|)
;	4	x<0  y<0  |x| > |y| 	= PI    +PI/4*(|y|/|x|)
;	5	x<0  y<0  |x| < |y| 	= PI*3/2-PI/4*(|x|/|y|)
;	6	x>0  y<0  |x| < |y| 	= PI*3/2+PI/4*(|x|/|y|)
;	7	x>0  y<0  |x| > |y| 	= PI*2  -PI/4*(|y|/|x|)
;
BrFixedATan2Fast proc uses ebx ecx edx, x:_F, y:_F

		mov	ebx,y
		mov	ecx,x
		xor	eax,eax

	; Branch tree on the three conditions -> 8 octants
	;
		cmp	ecx,0		; sign of y
		jl	octant_4567
		je	y_zero
octant_0123:	cmp	ebx,0		; sign of x
		jl	octant_23
octant_01:	cmp	ebx,ecx		; |x| vs. |y|
		jl	octant_1
		je	boundary_01

octant_0:	mov	edx,ecx		; |y|/|x|
		div	ebx
		shr	eax,19
		ret

octant_1:	mov	edx,ebx		; |x|/|y|
		div	ecx
		shr	eax,19
		neg	eax
		add	eax,04000H
		ret

boundary_01:	mov	eax,02000H
		ret

octant_23:	neg	ebx
		cmp	ebx,ecx		; |x| vs. |y|
		jg	octant_3
		je	boundary_23

octant_2:	mov	edx,ebx		; |x|/|y|
		div	ecx
		shr	eax,19
		add	eax,04000H
		ret

boundary_23:	mov	eax,06000H
		ret

octant_3:	mov	edx,ecx		; |y|/|x|
		div	ebx
		shr	eax,19
		neg	eax
		add	eax,08000H
		ret


octant_4567:	neg	ecx
		cmp	ebx,0		; sign of x
		jg	octant_67
		je	boundary_45

octant_45:	neg	ebx
		cmp	ebx,ecx		; |x| vs. |y|
		jl	octant_5
octant_4:	mov	edx,ecx		; |y|/|x|
		div	ebx
		shr	eax,19
		add	eax,08000H
		ret

boundary_45:	mov	eax,0A000H
		ret

octant_5:	mov	edx,ebx		; |x|/|y|
		div	ecx
		shr	eax,19
		neg	eax
		add	eax,0C000H
		ret

octant_67:	cmp	ebx,ecx		; |x| vs. |y|
		jg	octant_7
		je	boundary_67

octant_6:	mov	edx,ebx		; |x|/|y|
		div	ecx
		shr	eax,19
		add	eax,0C000H
		ret

boundary_67:	mov	eax,0E000H
		ret

octant_7:	mov	edx,ecx		; |y|/|x|
		div	ebx
		shr	eax,19
		neg	eax
		ret

	; Y component is zero
	;
y_zero:		cmp	ebx,0	; sign of x
		jl	x_lt_zero
		mov	eax,00h		; angle = 0 
		ret

x_lt_zero:	mov	eax,08000h	; anglw = PI
		ret

BrFixedATan2Fast endp


; unsigned long Sqrt(unsigned long input);
;
; 32 bit integer square root
;
; XXX - Does not take any account of leading zeros on input
;
_Sqrt proc uses ebx ecx edx, input:dword
		mov	ebx,input

		xor	ecx,ecx 	; Error = 0
		xor	eax,eax 	; Estimate = 0
		xor	edx,edx 	; Estimate*2 = 0

		shld	ecx,ebx,2	; Shift first two bits into error
		shl	ebx,2

		cmp	edx,ecx		; if estimate * 2 < error
		jnc	next_0
		sbb	ecx,edx		;	update error
		inc	eax		;	set low bit of Estimate
		add	edx,2		;	set bit 1 of Estimate*2
next_0:
	rept 14				; 14 more bit pairs
	local next
		shld	ecx,ebx,2	; Shift two bits into error
		shl	ebx,2

		add	eax,eax		; estimate *= 2
		add	edx,edx	
		cmp	edx,ecx		; if estimate * 2 < error
		jnc	next
		sbb	ecx,edx		;	update error
		inc	eax		;	set low bit of Estimate
		add	edx,2		;	set bit 1 of Estimate*2
next:
	endm
		shld	ecx,ebx,2	; Shift last two bits into error

		add	eax,eax		; estimate *= 2
		add	edx,edx	
		cmp	edx,ecx		; if estimate * 2 < error
		adc	eax,0		;	set low bit of Estimate

		ret
_Sqrt endp

; Sqrt64 - eax = sqrt(ecx:ebx)
;
; 64 bit integer square root - takes register parameters
;
; XXX - Does not take any account of leading zeros on input
;
_Sqrt64 proc
		xor	esi,esi 	; Error = 0
		xor	eax,eax 	; Estimate = 0
		xor	edx,edx 	; Estimate*2 = 0

	rept 16				; 16 bit pairs from high dword
	local next
		shld	esi,ecx,2	; Shift two bits into error
		shl	ecx,2

		add	eax,eax		; estimate *= 2
		add	edx,edx	
		cmp	edx,esi		; if estimate * 2 < error
		jnc	next
		sbb	esi,edx		;	update error
		inc	eax		;	set low bit of Estimate
		add	edx,2		;	set bit 1 of Estimate*2
next:
	endm

	rept 16				; 16 bit pairs from low dword
	local next
		shld	esi,ebx,2	; Shift two bits into error
		shl	ebx,2

		add	eax,eax		; estimate *= 2
		add	edx,edx	
		cmp	edx,esi		; if estimate * 2 < error
		jnc	next
		sbb	esi,edx		;	update error
		inc	eax		;	set low bit of Estimate
		add	edx,2		;	set bit 1 of Estimate*2
next:
	endm
		ret
_Sqrt64 endp

; unsigned long FastSqrt(unsigned long input);
;
; 32 bit low precision integer square root
;
_FastSqrt proc uses ebx ecx, input:dword

		mov	ebx,input
		xor	eax,eax

	; Find even exponent and shift to bits 0-7
	;				  
		bsr	ecx,ebx
		jz	is_zero
		or	cl,1
		inc	cl
		ror	ebx,cl
		shr	ebx,24

	; Lookup normalised sqrt from table
	;
		mov	bx,(fsqrt_table-128)[ebx*2]

	; Shift result back by half exponent
	;
		shr	cl,1
		shld	ax,bx,cl
is_zero:
		ret
_FastSqrt endp

; FastISqrt - eax = 1/sqrt(input) * 65536
;
; 32 bit low precision integer reciprocal square root
;
_FastRSqrt proc uses ebx ecx, input:dword

		mov	ebx,input
		xor	eax,eax

	; Find even exponent and shift to bits 0-7
	;
		bsr	ecx,ebx
		jz	is_zero
		or	cl,1
		ror	ebx,cl
		ror	ebx,1
		shr	ebx,24

	; Lookup normalised 1/sqrt from table
	;
		mov	ax,(frsqrt_table-128)[ebx*2]

	; Shift result down by half exponent
	;
		shr	cl,1
		shr	eax,cl
is_zero:
		ret
_FastRSqrt endp

; FastRSqrt64 - eax = 1/sqrt(ecx:ebx)
;
; 64 bit low precision integer reciprocal square root
;
; Destroys ecx,ebx,edx
;
_FastRSqrt64 proc

		and	ecx,ecx
		jne	has_high_word

		xor	eax,eax

	; Find even exponent and shift to bits 0-7
	;
		bsr	ecx,ebx
		jz	is_zero
		or	cl,1
		ror	ebx,cl
		ror	ebx,1
		shr	ebx,24

	; Lookup normalised 1/sqrt from table
	;
		mov	ax,(frsqrt_table-128)[ebx*2]

	; Shift result down by half exponent
	;
		shr	cl,1
		shr	eax,cl
is_zero:
		ret

has_high_word:

	; Find even exponent and shift to bits 0-7
	;
		mov	edx,ecx
		bsr	eax,ecx

		mov	cl,31
		sub	cl,al
		and	cl,not 1

		shld	edx,ebx,cl
		shr	edx,24

	; Lookup normalised 1/sqrt from table
	;
		mov	ax,(frsqrt_table-128)[edx*2]

	; Shift result down by half exponent
	;
		neg	ecx
		add	ecx,30
		shr	cl,1
		shr	eax,cl
		ret
_FastRSqrt64 endp

	if 0
TestFastRSqrt64 proc uses edx ebx ecx, p: _F

		mov	ebx,p
		mov	ecx,ebx

		shr	ecx,16
		shl	ebx,16

		call	_FastRSqrt64

		ret
TestFastRSqrt64 endp
	endif

	.data

; Full wave+1/4 table
;
public sin_table
public cos_table
public arcsin_table
public arccos_table

sin_table label word
	dw 00000H,00324H,00647H,0096AH,00C8BH,00FABH,012C7H,015E1H
	dw 018F8H,01C0BH,01F19H,02223H,02527H,02826H,02B1EH,02E10H
	dw 030FBH,033DEH,036B9H,0398CH,03C56H,03F16H,041CDH,0447AH
	dw 0471CH,049B3H,04C3FH,04EBFH,05133H,0539AH,055F4H,05842H
	dw 05A81H,05CB3H,05ED6H,060EBH,062F1H,064E7H,066CEH,068A5H
	dw 06A6CH,06C23H,06DC9H,06F5EH,070E1H,07254H,073B5H,07503H
	dw 07640H,0776BH,07883H,07989H,07A7CH,07B5CH,07C29H,07CE2H
	dw 07D89H,07E1CH,07E9CH,07F08H,07F61H,07FA6H,07FD7H,07FF5H
cos_table label word
	dw 07FFFH,07FF5H,07FD7H,07FA6H,07F61H,07F08H,07E9CH,07E1CH
	dw 07D89H,07CE2H,07C29H,07B5CH,07A7CH,07989H,07883H,0776BH
	dw 07640H,07503H,073B5H,07254H,070E1H,06F5EH,06DC9H,06C23H
	dw 06A6CH,068A5H,066CEH,064E7H,062F1H,060EBH,05ED6H,05CB3H
	dw 05A81H,05842H,055F4H,0539AH,05133H,04EBFH,04C3FH,049B3H
	dw 0471CH,0447AH,041CDH,03F16H,03C56H,0398CH,036B9H,033DEH
	dw 030FBH,02E10H,02B1EH,02826H,02527H,02223H,01F19H,01C0BH
	dw 018F8H,015E1H,012C7H,00FABH,00C8BH,0096AH,00647H,00324H
	dw 00000H,0FCDCH,0F9B9H,0F696H,0F375H,0F055H,0ED39H,0EA1FH
	dw 0E708H,0E3F5H,0E0E7H,0DDDDH,0DAD9H,0D7DAH,0D4E2H,0D1F0H
	dw 0CF05H,0CC22H,0C947H,0C674H,0C3AAH,0C0EAH,0BE33H,0BB86H
	dw 0B8E4H,0B64DH,0B3C1H,0B141H,0AECDH,0AC66H,0AA0CH,0A7BEH
	dw 0A57FH,0A34DH,0A12AH,09F15H,09D0FH,09B19H,09932H,0975BH
	dw 09594H,093DDH,09237H,090A2H,08F1FH,08DACH,08C4BH,08AFDH
	dw 089C0H,08895H,0877DH,08677H,08584H,084A4H,083D7H,0831EH
	dw 08277H,081E4H,08164H,080F8H,0809FH,0805AH,08029H,0800BH
	dw 08001H,0800BH,08029H,0805AH,0809FH,080F8H,08164H,081E4H
	dw 08277H,0831EH,083D7H,084A4H,08584H,08677H,0877DH,08895H
	dw 089C0H,08AFDH,08C4BH,08DACH,08F1FH,090A2H,09237H,093DDH
	dw 09594H,0975BH,09932H,09B19H,09D0FH,09F15H,0A12AH,0A34DH
	dw 0A57FH,0A7BEH,0AA0CH,0AC66H,0AECDH,0B141H,0B3C1H,0B64DH
	dw 0B8E4H,0BB86H,0BE33H,0C0EAH,0C3AAH,0C674H,0C947H,0CC22H
	dw 0CF05H,0D1F0H,0D4E2H,0D7DAH,0DAD9H,0DDDDH,0E0E7H,0E3F5H
	dw 0E708H,0EA1FH,0ED39H,0F055H,0F375H,0F696H,0F9B9H,0FCDCH
	dw 00000H,00324H,00647H,0096AH,00C8BH,00FABH,012C7H,015E1H
	dw 018F8H,01C0BH,01F19H,02223H,02527H,02826H,02B1EH,02E10H
	dw 030FBH,033DEH,036B9H,0398CH,03C56H,03F16H,041CDH,0447AH
	dw 0471CH,049B3H,04C3FH,04EBFH,05133H,0539AH,055F4H,05842H
	dw 05A81H,05CB3H,05ED6H,060EBH,062F1H,064E7H,066CEH,068A5H
	dw 06A6CH,06C23H,06DC9H,06F5EH,070E1H,07254H,073B5H,07503H
	dw 07640H,0776BH,07883H,07989H,07A7CH,07B5CH,07C29H,07CE2H
	dw 07D89H,07E1CH,07E9CH,07F08H,07F61H,07FA6H,07FD7H,07FF5H
	dw 07FFFH,07FFFH

arcsin_table label word
	dw 0C001H,0C519H,0C737H,0C8D7H,0CA37H,0CB6DH,0CC87H,0CD8AH
	dw 0CE7CH,0CF5FH,0D037H,0D104H,0D1C9H,0D286H,0D33CH,0D3EDH
	dw 0D498H,0D53EH,0D5DFH,0D67CH,0D716H,0D7ACH,0D83FH,0D8CFH
	dw 0D95CH,0D9E7H,0DA6FH,0DAF4H,0DB78H,0DBF9H,0DC79H,0DCF7H
	dw 0DD73H,0DDEDH,0DE66H,0DEDDH,0DF53H,0DFC8H,0E03BH,0E0ADH
	dw 0E11EH,0E18DH,0E1FCH,0E26AH,0E2D6H,0E342H,0E3ACH,0E416H
	dw 0E47FH,0E4E7H,0E54EH,0E5B4H,0E61AH,0E67FH,0E6E3H,0E746H
	dw 0E7A9H,0E80CH,0E86DH,0E8CEH,0E92FH,0E98FH,0E9EEH,0EA4DH
	dw 0EAABH,0EB09H,0EB66H,0EBC3H,0EC20H,0EC7CH,0ECD7H,0ED33H
	dw 0ED8DH,0EDE8H,0EE42H,0EE9CH,0EEF5H,0EF4EH,0EFA7H,0EFFFH
	dw 0F057H,0F0AFH,0F106H,0F15DH,0F1B4H,0F20BH,0F261H,0F2B8H
	dw 0F30DH,0F363H,0F3B9H,0F40EH,0F463H,0F4B8H,0F50CH,0F561H
	dw 0F5B5H,0F609H,0F65DH,0F6B1H,0F704H,0F758H,0F7ABH,0F7FEH
	dw 0F851H,0F8A4H,0F8F7H,0F949H,0F99CH,0F9EEH,0FA41H,0FA93H
	dw 0FAE5H,0FB37H,0FB89H,0FBDBH,0FC2DH,0FC7FH,0FCD1H,0FD23H
	dw 0FD74H,0FDC6H,0FE17H,0FE69H,0FEBAH,0FF0CH,0FF5EH,0FFAFH
	dw 00000H,00051H,000A2H,000F4H,00146H,00197H,001E9H,0023AH
	dw 0028CH,002DDH,0032FH,00381H,003D3H,00425H,00477H,004C9H
	dw 0051BH,0056DH,005BFH,00612H,00664H,006B7H,00709H,0075CH
	dw 007AFH,00802H,00855H,008A8H,008FCH,0094FH,009A3H,009F7H
	dw 00A4BH,00A9FH,00AF4H,00B48H,00B9DH,00BF2H,00C47H,00C9DH
	dw 00CF3H,00D48H,00D9FH,00DF5H,00E4CH,00EA3H,00EFAH,00F51H
	dw 00FA9H,01001H,01059H,010B2H,0110BH,01164H,011BEH,01218H
	dw 01273H,012CDH,01329H,01384H,013E0H,0143DH,0149AH,014F7H
	dw 01555H,015B3H,01612H,01671H,016D1H,01732H,01793H,017F4H
	dw 01857H,018BAH,0191DH,01981H,019E6H,01A4CH,01AB2H,01B19H
	dw 01B81H,01BEAH,01C54H,01CBEH,01D2AH,01D96H,01E04H,01E73H
	dw 01EE2H,01F53H,01FC5H,02038H,020ADH,02123H,0219AH,02213H
	dw 0228DH,02309H,02387H,02407H,02488H,0250CH,02591H,02619H
	dw 026A4H,02731H,027C1H,02854H,028EAH,02984H,02A21H,02AC2H
	dw 02B68H,02C13H,02CC4H,02D7AH,02E37H,02EFCH,02FC9H,030A1H
	dw 03184H,03276H,03379H,03493H,035C9H,03729H,038C9H,03AE7H
	dw 04000H

arccos_table label word
	dw 07FFFH,07AE7H,078C9H,07729H,075C9H,07493H,07379H,07276H
	dw 07184H,070A1H,06FC9H,06EFCH,06E37H,06D7AH,06CC4H,06C13H
	dw 06B68H,06AC2H,06A21H,06984H,068EAH,06854H,067C1H,06731H
	dw 066A4H,06619H,06591H,0650CH,06488H,06407H,06387H,06309H
	dw 0628DH,06213H,0619AH,06123H,060ADH,06038H,05FC5H,05F53H
	dw 05EE2H,05E73H,05E04H,05D96H,05D2AH,05CBEH,05C54H,05BEAH
	dw 05B81H,05B19H,05AB2H,05A4CH,059E6H,05981H,0591DH,058BAH
	dw 05857H,057F4H,05793H,05732H,056D1H,05671H,05612H,055B3H
	dw 05555H,054F7H,0549AH,0543DH,053E0H,05384H,05329H,052CDH
	dw 05273H,05218H,051BEH,05164H,0510BH,050B2H,05059H,05001H
	dw 04FA9H,04F51H,04EFAH,04EA3H,04E4CH,04DF5H,04D9FH,04D48H
	dw 04CF3H,04C9DH,04C47H,04BF2H,04B9DH,04B48H,04AF4H,04A9FH
	dw 04A4BH,049F7H,049A3H,0494FH,048FCH,048A8H,04855H,04802H
	dw 047AFH,0475CH,04709H,046B7H,04664H,04612H,045BFH,0456DH
	dw 0451BH,044C9H,04477H,04425H,043D3H,04381H,0432FH,042DDH
	dw 0428CH,0423AH,041E9H,04197H,04146H,040F4H,040A2H,04051H
	dw 03FFFH,03FAEH,03F5DH,03F0BH,03EB9H,03E68H,03E16H,03DC5H
	dw 03D73H,03D22H,03CD0H,03C7EH,03C2CH,03BDAH,03B88H,03B36H
	dw 03AE4H,03A92H,03A40H,039EDH,0399BH,03948H,038F6H,038A3H
	dw 03850H,037FDH,037AAH,03757H,03703H,036B0H,0365CH,03608H
	dw 035B4H,03560H,0350BH,034B7H,03462H,0340DH,033B8H,03362H
	dw 0330CH,032B7H,03260H,0320AH,031B3H,0315CH,03105H,030AEH
	dw 03056H,02FFEH,02FA6H,02F4DH,02EF4H,02E9BH,02E41H,02DE7H
	dw 02D8CH,02D32H,02CD6H,02C7BH,02C1FH,02BC2H,02B65H,02B08H
	dw 02AAAH,02A4CH,029EDH,0298EH,0292EH,028CDH,0286CH,0280BH
	dw 027A8H,02745H,026E2H,0267EH,02619H,025B3H,0254DH,024E6H
	dw 0247EH,02415H,023ABH,02341H,022D5H,02269H,021FBH,0218CH
	dw 0211DH,020ACH,0203AH,01FC7H,01F52H,01EDCH,01E65H,01DECH
	dw 01D72H,01CF6H,01C78H,01BF8H,01B77H,01AF3H,01A6EH,019E6H
	dw 0195BH,018CEH,0183EH,017ABH,01715H,0167BH,015DEH,0153DH
	dw 01497H,013ECH,0133BH,01285H,011C8H,01103H,01036H,00F5EH
	dw 00E7BH,00D89H,00C86H,00B6CH,00A36H,008D6H,00736H,00518H
	dw 00000H

arctan_table label word
	dw 00000H,00028H,00051H,0007AH,000A2H,000CBH,000F4H,0011DH
	dw 00145H,0016EH,00197H,001BFH,001E8H,00211H,00239H,00262H
	dw 0028BH,002B3H,002DCH,00304H,0032DH,00355H,0037EH,003A6H
	dw 003CEH,003F7H,0041FH,00448H,00470H,00498H,004C0H,004E8H
	dw 00511H,00539H,00561H,00589H,005B1H,005D9H,00601H,00628H
	dw 00650H,00678H,006A0H,006C7H,006EFH,00716H,0073EH,00765H
	dw 0078DH,007B4H,007DBH,00803H,0082AH,00851H,00878H,0089FH
	dw 008C6H,008EDH,00913H,0093AH,00961H,00987H,009AEH,009D4H
	dw 009FBH,00A21H,00A47H,00A6DH,00A94H,00ABAH,00AE0H,00B05H
	dw 00B2BH,00B51H,00B77H,00B9CH,00BC2H,00BE7H,00C0CH,00C32H
	dw 00C57H,00C7CH,00CA1H,00CC6H,00CEBH,00D0FH,00D34H,00D58H
	dw 00D7DH,00DA1H,00DC6H,00DEAH,00E0EH,00E32H,00E56H,00E7AH
	dw 00E9EH,00EC1H,00EE5H,00F08H,00F2CH,00F4FH,00F72H,00F95H
	dw 00FB8H,00FDBH,00FFEH,01021H,01044H,01066H,01089H,010ABH
	dw 010CDH,010EFH,01111H,01133H,01155H,01177H,01199H,011BAH
	dw 011DCH,011FDH,0121EH,0123FH,01260H,01281H,012A2H,012C3H
	dw 012E4H,01304H,01325H,01345H,01365H,01385H,013A5H,013C5H
	dw 013E5H,01405H,01424H,01444H,01463H,01483H,014A2H,014C1H
	dw 014E0H,014FFH,0151EH,0153CH,0155BH,01579H,01598H,015B6H
	dw 015D4H,015F2H,01610H,0162EH,0164CH,0166AH,01687H,016A5H
	dw 016C2H,016DFH,016FCH,01719H,01736H,01753H,01770H,0178CH
	dw 017A9H,017C5H,017E2H,017FEH,0181AH,01836H,01852H,0186EH
	dw 0188AH,018A5H,018C1H,018DCH,018F7H,01913H,0192EH,01949H
	dw 01964H,0197FH,01999H,019B4H,019CEH,019E9H,01A03H,01A1DH
	dw 01A37H,01A51H,01A6BH,01A85H,01A9FH,01AB9H,01AD2H,01AECH
	dw 01B05H,01B1EH,01B37H,01B50H,01B69H,01B82H,01B9BH,01BB4H
	dw 01BCCH,01BE5H,01BFDH,01C16H,01C2EH,01C46H,01C5EH,01C76H
	dw 01C8EH,01CA5H,01CBDH,01CD5H,01CECH,01D04H,01D1BH,01D32H
	dw 01D49H,01D60H,01D77H,01D8EH,01DA5H,01DBBH,01DD2H,01DE9H
	dw 01DFFH,01E15H,01E2CH,01E42H,01E58H,01E6EH,01E84H,01E99H
	dw 01EAFH,01EC5H,01EDAH,01EF0H,01F05H,01F1BH,01F30H,01F45H
	dw 01F5AH,01F6FH,01F84H,01F99H,01FADH,01FC2H,01FD7H,01FEBH
	dw 02000H

; sqrt(x)*0x1000 from 64 to 255
;
fsqrt_table label word
	dw 08000H,080FFH,081FCH,082F7H,083F0H,084E7H,085DDH,086D1H
	dw 087C3H,088B4H,089A3H,08A90H,08B7CH,08C66H,08D4EH,08E36H
	dw 08F1BH,09000H,090E2H,091C4H,092A4H,09383H,09460H,0953CH
	dw 09617H,096F1H,097CAH,098A1H,09977H,09A4CH,09B20H,09BF2H
	dw 09CC4H,09D94H,09E64H,09F32H,0A000H,0A0CCH,0A197H,0A261H
	dw 0A32BH,0A3F3H,0A4BAH,0A581H,0A646H,0A70BH,0A7CFH,0A892H
	dw 0A953H,0AA15H,0AAD5H,0AB94H,0AC53H,0AD11H,0ADCDH,0AE8AH
	dw 0AF45H,0B000H,0B0B9H,0B172H,0B22BH,0B2E2H,0B399H,0B44FH
	dw 0B504H,0B5B9H,0B66DH,0B720H,0B7D3H,0B885H,0B936H,0B9E7H
	dw 0BA97H,0BB46H,0BBF5H,0BCA3H,0BD50H,0BDFDH,0BEA9H,0BF55H
	dw 0C000H,0C0AAH,0C154H,0C1FDH,0C2A5H,0C34EH,0C3F5H,0C49CH
	dw 0C542H,0C5E8H,0C68EH,0C732H,0C7D7H,0C87AH,0C91DH,0C9C0H
	dw 0CA62H,0CB04H,0CBA5H,0CC46H,0CCE6H,0CD86H,0CE25H,0CEC3H
	dw 0CF62H,0D000H,0D09DH,0D13AH,0D1D6H,0D272H,0D30DH,0D3A8H
	dw 0D443H,0D4DDH,0D577H,0D610H,0D6A9H,0D742H,0D7DAH,0D871H
	dw 0D908H,0D99FH,0DA35H,0DACBH,0DB61H,0DBF6H,0DC8BH,0DD1FH
	dw 0DDB3H,0DE47H,0DEDAH,0DF6DH,0E000H,0E092H,0E123H,0E1B5H
	dw 0E246H,0E2D6H,0E367H,0E3F7H,0E486H,0E515H,0E5A4H,0E633H
	dw 0E6C1H,0E74FH,0E7DCH,0E869H,0E8F6H,0E983H,0EA0FH,0EA9BH
	dw 0EB26H,0EBB1H,0EC3CH,0ECC7H,0ED51H,0EDDBH,0EE65H,0EEEEH
	dw 0EF77H,0F000H,0F088H,0F110H,0F198H,0F21FH,0F2A6H,0F32DH
	dw 0F3B4H,0F43AH,0F4C0H,0F546H,0F5CBH,0F651H,0F6D6H,0F75AH
	dw 0F7DEH,0F863H,0F8E6H,0F96AH,0F9EDH,0FA70H,0FAF3H,0FB75H
	dw 0FBF7H,0FC79H,0FCFBH,0FD7CH,0FDFDH,0FE7EH,0FEFFH,0FF7FH

; 0x10000/sqrt(x) from 64 to 255
;
frsqrt_table label word
	dw 0FFFFH,0FE05H,0FC17H,0FA33H,0F85BH,0F68CH,0F4C8H,0F30DH
	dw 0F15BH,0EFB3H,0EE13H,0EC7BH,0EAEBH,0E964H,0E7E3H,0E66BH
	dw 0E4F9H,0E38EH,0E229H,0E0CCH,0DF74H,0DE23H,0DCD7H,0DB91H
	dw 0DA51H,0D916H,0D7E0H,0D6B0H,0D584H,0D45EH,0D33CH,0D21EH
	dw 0D105H,0CFF1H,0CEE1H,0CDD4H,0CCCCH,0CBC8H,0CAC8H,0C9CBH
	dw 0C8D2H,0C7DDH,0C6EBH,0C5FCH,0C511H,0C429H,0C344H,0C263H
	dw 0C184H,0C0A8H,0BFD0H,0BEFAH,0BE26H,0BD56H,0BC88H,0BBBDH
	dw 0BAF4H,0BA2EH,0B96AH,0B8A9H,0B7EAH,0B72DH,0B673H,0B5BBH
	dw 0B504H,0B450H,0B39FH,0B2EFH,0B241H,0B195H,0B0EBH,0B043H
	dw 0AF9DH,0AEF8H,0AE56H,0ADB5H,0AD16H,0AC79H,0ABDDH,0AB43H
	dw 0AAAAH,0AA13H,0A97EH,0A8EAH,0A858H,0A7C7H,0A737H,0A6A9H
	dw 0A61DH,0A592H,0A508H,0A47FH,0A3F8H,0A372H,0A2EEH,0A26AH
	dw 0A1E8H,0A167H,0A0E7H,0A069H,09FECH,09F6FH,09EF4H,09E7AH
	dw 09E01H,09D89H,09D13H,09C9DH,09C28H,09BB4H,09B42H,09AD0H
	dw 09A5FH,099EFH,09981H,09913H,098A6H,0983AH,097CEH,09764H
	dw 096FBH,09692H,0962AH,095C3H,0955DH,094F8H,09493H,09430H
	dw 093CDH,0936BH,09309H,092A9H,09249H,091E9H,0918BH,0912DH
	dw 090D0H,09074H,09018H,08FBDH,08F63H,08F09H,08EB0H,08E58H
	dw 08E00H,08DA9H,08D53H,08CFDH,08CA8H,08C53H,08BFFH,08BACH
	dw 08B59H,08B06H,08AB5H,08A64H,08A13H,089C3H,08973H,08924H
	dw 088D6H,08888H,0883BH,087EEH,087A1H,08755H,0870AH,086BFH
	dw 08675H,0862BH,085E1H,08598H,08550H,08508H,084C0H,08479H
	dw 08432H,083ECH,083A6H,08361H,0831CH,082D7H,08293H,0824FH
	dw 0820CH,081C9H,08186H,08144H,08103H,080C1H,08080H,08040H

	end


