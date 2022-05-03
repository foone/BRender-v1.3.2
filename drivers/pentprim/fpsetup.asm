; Floating point Pentium optimised setup code for rasterisers
;
.586p
.model flat,c

include	drv.inc
include fpwork.inc
include fpu.inc
.code

; Do all the per-triangle work for a single float parameter
;
;	eax: ptr to top vertex
;	ebx: ptr to vertex0
;	ecx: ptr to vertex1
;	edx: ptr to vertex2
;	ebp: ptr to param block
;
;
SETUP_FLOAT_PARAM	macro	comp,param,s_p,d_p_x,conv, unsigned:=<0>

		assume eax: ptr brp_vertex, ebx: ptr brp_vertex, ecx: ptr brp_vertex, edx: ptr brp_vertex

	; work out parameter deltas
	; dp1 = param_1 - param_0
	; dp2 = param_2 - param_0
	;
	; 4 cycles
	;
												;	0		1		2		3		4		5		6		7
			fld		[edx].comp_f[comp*4]		;	p2
			fsub	[ebx].comp_f[comp*4]		;	dp2
			fld		[ecx].comp_f[comp*4]		;	p1		dp2
			fsub	[ebx].comp_f[comp*4]		;	dp1		dp2

	; Multiply deltas by precomputed values to get x & y gradients
	; (Also interleaved load of parameter start and fractional x & y offsets of start position)
	;
	; pdx = dp1 * dy2_a - dp2 * dy1_a
	; pdy = dp2 * dx1_a - dp1 * dx2_a
	;
	; 11 cycles
	;
												;	0		1		2		3		4		5		6		7
			fld		st(1)						;	dp2		dp1		dp2
			fmul	[workspace.dy1_a]			;	dp2*a	dp1		dp2
			fld		st(1)						;	dp1		dp2*a	dp1		dp2
			fmul	[workspace.dy2_a]			;	dp1*b	dp2*a	dp1		dp2
			fld		[workspace.t_dx]			;	fdx		dp1*b	dp2*a	dp1		dp2
			 fxch	st(4)						;	dp2		dp1*b	dp2*a	dp1		fdx
			fmul	[workspace.dx1_a]			;	dp2*c	dp1*b	dp2*a	dp1		fdx
			fld		[workspace.t_dy]			; 	fdy		dp2*c	dp1*b	dp2*a	dp1		fdx
			 fxch	st(4)						; 	dp1		dp2*c	dp1*b	dp2*a	fdy		fdx
			fmul	[workspace.dx2_a]			; 	dp1*d	dp2*c	dp1*b	dp2*a	fdy		fdx
			 fxch	st(3)						; 	dp2*a	dp2*c	dp1*b	dp1*d	fdy		fdx
			fsubp	st(2),st					; 	dp2*c	d1b-d2a	dp1*d	fdy		fdx
			fld		[eax].comp_f[comp*4]		; 	param_t	dp2*c	d1b-d2a	dp1*d	fdy		fdx
			 fxch	st(3)						; 	dp1*d	dp2*c	d1b-d2a	param_t	fdy		fdx
			fsubp	st(1),st					; 	d2c-d1d	d1b-d2a	param_t	fdy		fdx
												; 	pdy		pdx		param_t	fdy		fdx

	; Build the inputs to the rasteriser
	;
	; pdy_0 = pdy + xstep_0 * pdx
	; pdy_1 = pdy + xstep_1 * pdx
	; pstart = param_t + pdx * fdx + pdy * fdy
	;
	; (A couple of the fixed points convertions are interleaved into this block)
	; 12 cycles
	;
												;	0		1		2		3		4		5		6		7
			fld		st(1)						; 	pdx		pdy		pdx		param_t	fdy		fdx
			fmul	[workspace.xstep_0]			; 	pdx*xs0	pdy		pdx		param_t	fdy		fdx
			fld		st(2)						; 	pdx		pdx*xs0	pdy		pdx		param_t	fdy		fdx
			fmul	[workspace.xstep_1]			; 	pdx*xs1	pdx*xs0	pdy		pdx		param_t	fdy		fdx
			 fxch	st(1)						; 	pdx*xs0	pdx*xs1	pdy		pdx		param_t	fdy		fdx
			fadd	st,st(2)					; 	pdy_0	pdx*xs1	pdy		pdx		param_t	fdy		fdx
			 fxch	st(3)						; 	pdx		pdx*xs1	pdy		pdy_0	param_t	fdy		fdx

			fmul	st(6),st					; 	pdx		pdx*xs1	pdy		pdy_0	param_t	fdy		fdx*pdx
			 fxch	st(2)						; 	pdy		pdx*xs1	pdx		pdy_0	param_t	fdy		fdx*pdx
			fadd	st(1),st					; 	pdy		pdy_1	pdx		pdy_0	param_t	fdy		fdx*pdx

			fmulp	st(5),st					; 	pdy_1	pdx		pdy_0	param_t	fdy*pdy	fdx*pdx
			 fxch	st(3)						; 	param_t	pdx		pdy_0	pdy_1	fdy*pdy	fdx*pdx
			faddp	st(5),st					; 	pdx		pdy_0	pdy_1	fdy*pdy	fpx+pt
			 fxch	st(1)						; 	pdy_0	pdx		pdy_1	fdy*pdy	fpx+pt
			fadd	conv						; 	C+pdy_0	pdx		pdy_1	fdy*pdy	fpx+pt
			 fxch	st(2)						; 	pdy_1	pdx		C+pdy_0	fdy*pdy	fpx+pt
			fadd	conv						; 	C+pdy_1	pdx		C+pdy_0	fdy*pdy	fpx+pt
			 fxch	st(3)						; 	fdy*pdy	pdx		C+pdy_0	C+pdy_1	fpx+pt
			faddp	st(4),st					;	pdx		C+pdy_0	C+pdy_1	pstart		


	; Convert to fixed point, pack and store in output block
	;
	; tsb->d_p_y0 = convert(pdy_0)
	; tsb->d_p_y1 = convert(pdy_1)
	; tsb->d_p_x = convert(pdx)
	; tsb->s_p = convert(pstart)
	;
	; 13 cycles
									;	0		1		2		3		4		5		6		7
			fadd	conv			; 	C+pdx	C+pdy_0	C+pdy_1	pstart
			 fxch	st(3)			; 	pstart	C+pdy_0	C+pdy_1	C+pdx

; 1 clock delay

			fadd	conv			; 	C+pstrt	C+pdy_0	C+pdy_1	C+pdx
			 fxch	st(2)			; 	C+pdy_1	C+pdy_0	C+pstrt	C+pdx

			fstp	real8 ptr s_p
			fstp	real8 ptr d_p_x
			mov		esi,dword ptr s_p
			mov		edi,dword ptr d_p_x
			fstp	real8 ptr s_p	;
			fstp	real8 ptr d_p_x	;
			mov		dword ptr s_p+4,esi
			mov		dword ptr d_p_x+4,edi
if unsigned
	; Remap from -1 to 1 signed to 0 to 1 unsigned
	;
			mov		esi,dword ptr s_p
			xor		esi,080000000h
			mov		dword ptr s_p,esi
endif

	endm


; Do all the per-triangle work for a single fixed parameter
;
;	eax: ptr to top vertex
;	ebx: ptr to vertex0
;	ecx: ptr to vertex1
;	edx: ptr to vertex2
;	ebp: ptr to param block
;
;
SETUP_FIXED_PARAM	macro	comp,param,s_p,d_p_x,conv, unsigned:=<0>

		assume eax: ptr brp_vertex, ebx: ptr brp_vertex, ecx: ptr brp_vertex, edx: ptr brp_vertex

	; work out parameter deltas
	; dp1 = param_1 - param_0
	; dp2 = param_2 - param_0
	;
	; 8? cycles
	;
												;	0		1		2		3		4		5		6		7
			fild	[edx].comp_x[comp*4]		;	p2
			fild	[ebx].comp_x[comp*4]		;   p0	  p2
			fild	[ecx].comp_x[comp*4]		;	p1		p0	  p2
			 fxch   st(1)						;	p0		p1	  p2
			fsub	st(2),st					;	p0		p1		dp2
			fsubp	st(1),st					;	dp1		dp2

	; Multiply deltas by precomputed values to get x & y gradients
	; (Also interleaved load of parameter start and fractional x & y offsets of start position)
	;
	; pdx = dp1 * dy2_a - dp2 * dy1_a
	; pdy = dp2 * dx1_a - dp1 * dx2_a
	;
	; 11 cycles
	;
												;	0		1		2		3		4		5		6		7
			fld		st(1)						;	dp2		dp1		dp2
			fmul	[workspace.dy1_a]			;	dp2*a	dp1		dp2
			fld		st(1)						;	dp1		dp2*a	dp1		dp2
			fmul	[workspace.dy2_a]			;	dp1*b	dp2*a	dp1		dp2
			fld		[workspace.t_dx]			;	fdx		dp1*b	dp2*a	dp1		dp2
			 fxch	st(4)						;	dp2		dp1*b	dp2*a	dp1		fdx
			fmul	[workspace.dx1_a]			;	dp2*c	dp1*b	dp2*a	dp1		fdx
			fld		[workspace.t_dy]			; 	fdy		dp2*c	dp1*b	dp2*a	dp1		fdx
			 fxch	st(4)						; 	dp1		dp2*c	dp1*b	dp2*a	fdy		fdx
			fmul	[workspace.dx2_a]			; 	dp1*d	dp2*c	dp1*b	dp2*a	fdy		fdx
			 fxch	st(3)						; 	dp2*a	dp2*c	dp1*b	dp1*d	fdy		fdx
			fsubp	st(2),st					; 	dp2*c	d1b-d2a	dp1*d	fdy		fdx
			fild	[eax].comp_x[comp*4]		; 	param_t	dp2*c	d1b-d2a	dp1*d	fdy		fdx
			 fxch	st(3)						; 	dp1*d	dp2*c	d1b-d2a	param_t	fdy		fdx
			fsubp	st(1),st					; 	d2c-d1d	d1b-d2a	param_t	fdy		fdx
												; 	pdy		pdx		param_t	fdy		fdx

	; Build the inputs to the rasteriser
	;
	; pdy_0 = pdy + xstep_0 * pdx
	; pdy_1 = pdy + xstep_1 * pdx
	; pstart = param_t + pdx * fdx + pdy * fdy
	;
	; (A couple of the fixed points convertions are interleaved into this block)
	; 12 cycles
	;
												;	0		1		2		3		4		5		6		7
			fld		st(1)						; 	pdx		pdy		pdx		param_t	fdy		fdx
			fmul	[workspace.xstep_0]			; 	pdx*xs0	pdy		pdx		param_t	fdy		fdx
			fld		st(2)						; 	pdx		pdx*xs0	pdy		pdx		param_t	fdy		fdx
			fmul	[workspace.xstep_1]			; 	pdx*xs1	pdx*xs0	pdy		pdx		param_t	fdy		fdx
			 fxch	st(1)						; 	pdx*xs0	pdx*xs1	pdy		pdx		param_t	fdy		fdx
			fadd	st,st(2)					; 	pdy_0	pdx*xs1	pdy		pdx		param_t	fdy		fdx
			 fxch	st(3)						; 	pdx		pdx*xs1	pdy		pdy_0	param_t	fdy		fdx

			fmul	st(6),st					; 	pdx		pdx*xs1	pdy		pdy_0	param_t	fdy		fdx*pdx
			 fxch	st(2)						; 	pdy		pdx*xs1	pdx		pdy_0	param_t	fdy		fdx*pdx
			fadd	st(1),st					; 	pdy		pdy_1	pdx		pdy_0	param_t	fdy		fdx*pdx

			fmulp	st(5),st					; 	pdy_1	pdx		pdy_0	param_t	fdy*pdy	fdx*pdx
			 fxch	st(3)						; 	param_t	pdx		pdy_0	pdy_1	fdy*pdy	fdx*pdx
			faddp	st(5),st					; 	pdx		pdy_0	pdy_1	fdy*pdy	fpx+pt
			 fxch	st(1)						; 	pdy_0	pdx		pdy_1	fdy*pdy	fpx+pt
			fadd	conv						; 	C+pdy_0	pdx		pdy_1	fdy*pdy	fpx+pt
			 fxch	st(2)						; 	pdy_1	pdx		C+pdy_0	fdy*pdy	fpx+pt
			fadd	conv						; 	C+pdy_1	pdx		C+pdy_0	fdy*pdy	fpx+pt
			 fxch	st(3)						; 	fdy*pdy	pdx		C+pdy_0	C+pdy_1	fpx+pt
			faddp	st(4),st					;	pdx		C+pdy_0	C+pdy_1	pstart


	; Convert to fixed point, pack and store in output block
	;
	; tsb->d_p_y0 = convert(pdy_0)
	; tsb->d_p_y1 = convert(pdy_1)
	; tsb->d_p_x = convert(pdx)
	; tsb->s_p = convert(pstart)
	;
	; 13 cycles
									;	0		1		2		3		4		5		6		7
			fadd	conv			; 	C+pdx	C+pdy_0	C+pdy_1	pstart
			 fxch	st(3)			; 	pstart	C+pdy_0	C+pdy_1	C+pdx

; 1 clock delay

			fadd	conv			; 	C+pstrt	C+pdy_0	C+pdy_1	C+pdx
			 fxch	st(2)			; 	C+pdy_1	C+pdy_0	C+pstrt	C+pdx

			fstp	real8 ptr s_p
			fstp	real8 ptr d_p_x
			mov		esi,dword ptr s_p
			mov		edi,dword ptr d_p_x
			fstp	real8 ptr s_p	;
			fstp	real8 ptr d_p_x	;
			mov		dword ptr s_p+4,esi
			mov		dword ptr d_p_x+4,edi

if unsigned
	; Remap from -1 to 1 signed to 0 to 1 unsigned
	;
			mov		esi,dword ptr s_p
			xor		esi,080000000h
			mov		dword ptr s_p,esi
endif

	endm


; Do all the per-triangle work for a single non-variant float parameter
;
;	eax: ptr to top vertex
;	ebx: ptr to vertex0
;	ecx: ptr to vertex1
;	edx: ptr to vertex2
;	ebp: ptr to param block
;
;
SETUP_FLOAT_CONST	macro	comp,param,s_p,d_p_x,conv, unsigned:=<0>

		assume eax: ptr brp_vertex, ebx: ptr brp_vertex, ecx: ptr brp_vertex, edx: ptr brp_vertex

	; Build the inputs to the rasteriser
	; Convert to fixed point, pack and store in output block
	;
	; pdx = 0
	; pdy_0 = 0
	; pdy_1 = 0
	; pstart = param_0
	;
			fld		[ebx].comp_x[comp*4]		;	pstart
			fadd	conv						;	C+pstrt

			mov		esi,0
			
			mov		dword ptr d_p_x,esi
			mov		dword ptr d_p_x+4,esi

			fstp	real8 ptr s_p				;

			mov		dword ptr s_p+4,esi

if unsigned
	; Remap from -1 to 1 signed to 0 to 1 unsigned
	;
			mov		esi,dword ptr s_p
			xor		esi,080000000h
			mov		dword ptr s_p,esi
endif

	endm


; Do all the per-triangle work for a single non-variant fixed parameter
;
;	eax: ptr to top vertex
;	ebx: ptr to vertex0
;	ecx: ptr to vertex1
;	edx: ptr to vertex2
;	ebp: ptr to param block
;
;
SETUP_FIXED_CONST	macro	comp,param,s_p,d_p_x,conv, unsigned:=<0>

		assume eax: ptr brp_vertex, ebx: ptr brp_vertex, ecx: ptr brp_vertex, edx: ptr brp_vertex

			mov		esi,[ebx].comp_x[comp*4]
			mov		edi,0
			
if unsigned
	; Remap from -1 to 1 signed to 0 to 1 unsigned
	;
			xor		esi,080000000h
endif

			mov		dword ptr s_p,esi
			mov		dword ptr s_p+4,edi

			mov		dword ptr d_p_x,edi
			mov		dword ptr d_p_x+4,edi

	endm


SETUP_FLOAT	macro
	local count_cont,exit,top_zero,bottom_zero,empty_triangle

		assume eax: ptr brp_vertex, ebx: ptr brp_vertex, ecx: ptr brp_vertex, edx: ptr brp_vertex

		; Calculate area of triangle and generate dx1/2area, dx1/2area, dx1/2area and dx1/2area
		;
		; Also sort the vertices in Y order whilst divide is happening
		;
													;	0		1		2		3		4		5		6		7
			fld			[edx].comp_f[C_SX*4]		;	x2
			fsub		[eax].comp_f[C_SX*4]		;	dx2
			fld			[ecx].comp_f[C_SX*4]		;	x1		dx2
			fsub		[eax].comp_f[C_SX*4]		;	dx1		dx2
			fld			[edx].comp_f[C_SY*4]		;	y2		dx1		dx2
			fsub		[eax].comp_f[C_SY*4]		;	dy2		dx1		dx2
			fld			[ecx].comp_f[C_SY*4]		;	y1		dy2		dx1		dx2
			fsub		[eax].comp_f[C_SY*4]		;	dy1		dy2		dx1		dx2

			fld			st(2)						;	dx1		dy1		dy2		dx1		dx2
			fmul		st,st(2)					;	dx1*dy2	dy1		dy2		dx1		dx2

			fld			st(4)						;	dx2		dx1*dy2	dy1		dy2		dx1		dx2
			fmul		st,st(2)					;	dx2*dy1	dx1*dy2	dy1		dy2		dx1		dx2

			mov			eax,[eax].comp_f[C_SY*4]

			mov			ecx,[ecx].comp_f[C_SY*4]

			fsubp		st(1),st					;	2area	dy1		dy2		dx1		dx2

			xor			ebx,ebx
			cmp			ecx,eax

			rcl			ebx,1
			mov			edx,[edx].comp_f[C_SY*4]

			fdivr		fp_one						;	1/2area	dy1		dy2		dx1		dx2

			cmp			edx,eax

			rcl			ebx,1
			cmp			edx,ecx

			rcl			ebx,1			; ebx now has 3 bit number characterising the order of the vertices.

;U
;V
			mov			eax,sort_table_0[ebx*4]
			mov			edx,sort_table_2[ebx*4]

			mov			esi,flip_table[ebx*4]
			mov			ebx,sort_table_1[ebx*4]

	; Load eax,ebx,edx with pointers to the three vertices in vertical order
	;
			mov			eax,[workspace.v0+eax]
			mov			edx,[workspace.v0+edx]

			mov			ebx,[workspace.v0+ebx]
			mov			[workspace.flip],esi

	; Work out Y extents of triangle
	;
	; Convert float to int using integer instructions, because FPU is in use doing division
	;
	; NB: this convertion only works if the number is greater than 0 and less than 1<<24
	;
MASK_MANTISSA   equ 007fffffh
IMPLICIT_ONE	equ 1 shl 23
EXPONENT_OFFSET equ ((127+23) shl 23) or 07fffffh

			mov		 ebp,[eax].comp_f[C_SY*4]
			mov		 ecx,EXPONENT_OFFSET

			sub		 ecx,ebp				; Offset exponent to get shift value
			and		 ebp,MASK_MANTISSA	; Mask out mantissa

			shr		 ecx,23				; Move shift value to low bits
			or			ebp,IMPLICIT_ONE	; Put the 1 back in top of mantissa

			shr		 ebp,cl				; EBP = y_t

			mov		 esi,[ebx].comp_f[C_SY*4]
			mov		 ecx,EXPONENT_OFFSET

			sub		 ecx,esi
			and		 esi,MASK_MANTISSA

			shr		 ecx,23
			or			esi,IMPLICIT_ONE

			shr		 esi,cl				; ESI = y_m

			mov		 edi,[edx].comp_f[C_SY*4]
			mov		 ecx,EXPONENT_OFFSET

			sub		 ecx,edi
			and		 edi,MASK_MANTISSA

			shr		 ecx,23
			or			edi,IMPLICIT_ONE

			shr		 edi,cl				; EDI = y_b

		; Catch special cases of empty top or bottom trapezoids
		;
			cmp			ebp,esi
			je			top_zero

			cmp			esi,edi
			je			bottom_zero

	; Parameter gradient startup and Y deltas for edge gradients
	;
													;	0		1		2		3		4		5		6		7
			fmul		st(1),st					;	1/2area	dy1*a	dy2		dx1		dx2

			fld			[ebx].comp_f[C_SY*4]		;	sy2		1/2area	dy1*a	dy2		dx1		dx2
			fsub		[eax].comp_f[C_SY*4]		;   dsy1	1/2area	dy1*a	dy2		dx1		dx2
			 fxch	   st(3)						;   dy2  	1/2area	dy1*a	dsy1	dx1		dx2

			fmul		st,st(1)					;	dy2*a  	1/2area	dy1*a	dsy1	dx1		dx2

			fld			[edx].comp_f[C_SY*4]		;   sy3	 dy2*a  	1/2area	dy1*a	dsy1	dx1		dx2
			fsub		[ebx].comp_f[C_SY*4]		;   dsy2	dy2*a  	1/2area	dy1*a	dsy1	dx1		dx2
			 fxch	   st(5)						;   dx1	  dy2*a 	1/2area	dy1*a	dsy1	dsy2	dx2

count_cont:
			fmul		st,st(2)					;	dx1*a   dy2*a  	1/2area	dy1*a	dsy1	dsy2	dx2

			fld			[edx].comp_f[C_SY*4]		;   sy3	 dx1*a   dy2*a  	1/2area	dy1*a	dsy1	dsy2	dx2
			fsub		[eax].comp_f[C_SY*4]		;   dsy3	dx1*a   dy2*a  	1/2area	dy1*a	dsy1	dsy2	dx2
			 fxch	   st(7)						;   dx2		dx1*a   dy2*a  	1/2area	dy1*a	dsy1	dsy2	dsy3

			fmul		st,st(3)					;   dx2*a	dx1*a   dy2*a  	1/2area	dy1*a	dsy1	dsy2	dsy3
			 fxch		st(3)						;   1/2area	dx1*a   dy2*a  	dx2*a	dy1*a	dsy1	dsy2	dsy3

			fstp		workspace.iarea
			fstp		[workspace.dx1_a]
			fstp		[workspace.dy2_a]
			fstp		[workspace.dx2_a]
			fstp		[workspace.dy1_a]			;  	dy1		dy2		dy3


	; Find edge gradients of triangle
	;
	; R = 1/(dy1.dy2.dy3)
	;
	; gradient_major = dy1.dx2.dy3.R
	; gradient_minor1 = dx1.dy2.dy3.R
	; gradient_minor2 = dy1.dy2.dx3.R
	;
													;	0		1		2		3		4		5		6		7
			fld			st(2)						;	dy3		dy1		dy2		dy3
			fmul		st,st(2)					;	dy2*dy3	dy1		dy2		dy3

			fld			[ebx].comp_f[C_SX*4]		;	x2		dy2*dy3	dy1		dy2		dy3
			fsub		[eax].comp_f[C_SX*4]		;	dx1		dy2*dy3	dy1		dy2		dy3

			fld			st(1)						;	dy2*dy3 dx1		dy2*dy3	dy1		dy2		dy3
			fmul		st,st(3)					;	dy123	dx1		dy2*dy3	dy1		dy2		dy3
if DEBUG
			fst			temp
			mov			ecx,temp
			and			ecx,07fffh
			jnz			notzero
			call		divzero
notzero:
endif

			fld			[edx].comp_f[C_SX*4]		;	x3		dy123	dx1		dy2*dy3	dy1		dy2		dy3
			fsub		[ebx].comp_f[C_SX*4]		;	dx2		dy123	dx1		dy2*dy3	dy1		dy2		dy3
			 fxch		 st(2)						;	dx1		dy123	dx2		dy2*dy3	dy1		dy2		dy3
			fld			fp_one						;	1.0		dx1		dy123	dx2		dy2*dy3	dy1		dy2		dy3
			fdivrp		st(2),st					;	dx1		R		dx2		dy2*dy3	dy1		dy2		dy3

	; Generate counts
	;
			inc			ebp
			mov			ecx,esi

			sub			ecx,ebp				;  count_t = (y_m-y_t)-1
			mov			[workspace.t_y],ebp			; save for X intercept calcs

			mov			[workspace.topCount],ecx
			inc			esi

			sub			edi,esi				;  count_b = (y_b-y_m)-1
			mov			m_y,esi				; save for X intercept calcs

			mov			[workspace].bottomCount,edi
			mov			esi,[workspace.flip]

	; Generate LR/RL flag into esi (used to index convertion numbers below)
	;
			mov			edi,workspace.iarea
;V
			xor			esi,edi			; Build LR flag in bit 31
;V
			shr			esi,31			; move down to bit 0
;V
			mov			[workspace.flip],esi


	;XXX Setup screen pointers and strides
	;

	;XXX Work out which scan convertion function to use
	;

		; Finish of gradient calculations, interleaved with working out t_dy, and m_dy, the fractions
		; that the top and middle vertices are from the integer scanline boundaries
		;
		; t_dy = (yt+1) - vt->y
		; m_dy = (ym+1) - vm->y
		;
													;	0		1		2		3		4		5		6		7
			fmulp		st(3),st					;	R		dx2		XYY		dy1		dy2		dy3

			fld			[edx].comp_f[C_SX*4]		;	x3		R		dx2		XYY		dy1		dy2		dy3
			 fxch		st(3)						;	XYY		R		dx2		x3		dy1		dy2		dy3

			fmul		st,st(1)					;	XYY*R	R		dx2		x3		dy1		dy2		dy3
			 fxch		st(3)						;	x3		R		dx2		XYY*R	dy1		dy2		dy3

			fsub		[eax].comp_f[C_SX*4]		;	dx3		R		dx2		XYY*R	dy1		dy2		dy3
			 fxch		st(1)						;	R		dx3		dx2		XYY*R	dy1		dy2		dy3

			fmulp		st(4),st					;	dx3		dx2		XYY*R	dy1*R	dy2		dy3
			 fxch		st(2)						;	XYY*R	dx2		dx3		dy1*R	dy2		dy3
													
			fild		m_y				            ;	m_y		XYY*R	dx2		dx3		dy1*R	dy2		dy3
			 fxch		st(2)						;	dx2		XYY*R	m_y		dx3		dy1*R	dy2		dy3
													
			fmulp		st(6),st		            ;	XYY*R	m_y		dx3		dy1*R	dy2		dx2*dy3
													
			fild		[workspace.t_y]							;	t_y		XYY*R	m_y		dx3		dy1*R	dy2		dx2*dy3
			 fxch		st(3)			            ;	dx3		XYY*R	m_y		t_y		dy1*R	dy2		dx2*dy3
													
			fmulp		st(5),st					;	XYY*R	m_y		t_y		dy1*R	dy2*dx3	dx2*dy3
			 fxch		st(1)			            ;	m_y		XYY*R	t_y		dy1*R	dy2*dx3	dx2*dy3

			fsub		[ebx].comp_f[C_SY*4]		;	m_dy	XYY*R	t_y		dy1*R	dy2*dx3	dx2*dy3
			 fxch		st(3)						;	dy1*R	XYY*R	t_y		m_dy	dy2*dx3	dx2*dy3
													
			fmul		st(4),st		            ;	dy1*R	XYY*R	t_y		m_dy	YYX*R	dx2*dy3
			 fxch		st(2)						;	t_y		XYY*R	dy1*R	m_dy	YYX*R	dx2*dy3
													
			fsub		[eax].comp_f[C_SY*4]		;	t_dy	XYY*R	dy1*R	m_dy	YYX*R	dx2*dy3
			 fxch		st(2)						;	dy1*R	XYY*R	t_dy	m_dy	YYX*R	dx2*dy3
													
			fmulp		st(5),st		            ;	XYY*R	t_dy	m_dy	YYX*R	YXY*R
			 fxch		st(2)						;	m_dy	t_dy	XYY*R	YYX*R	YXY*R
													;	m_dy	t_dy	g1		gm		g2
										            
	; Work out initial X intercepts with top and middle scanlines
	;
	; x_major  = gm * t_dy + vt->x
	; x_minor1 = g1 * t_dy + vt->x
	; x_minor2 = g2 * m_dy + vm->x
	;
													;	0		1		2		3		4		5		6		7
			fld			st(1)						;	t_dy	m_dy	t_dy	g1		gm		g2
			fxch		st(1)			            ;	m_dy	t_dy	t_dy	g1		gm		g2
													
													
			fmul		st,st(5)		            ;	m_dy*g2	t_dy	t_dy	g1		gm		g2
			fxch		st(2)						;	t_dy	t_dy	m_dy*g2	g1		gm		g2
													
			fst			[workspace.t_dy]            
													
			fmul		st,st(3)					;	t_dy*g1	t_dy	m_dy*g2	g1		gm		g2
			 fxch		st(2)			            ;	m_dy*g2	t_dy	t_dy*g1	g1		gm		g2

			fadd		[ebx].comp_f[C_SX*4]		;	x_2		t_dy	t_dy*g1	g1		gm		g2
			 fxch		st(1)						;	t_dy	x_2		t_dy*g1	g1		gm		g2
													
			fmul		st,st(4)		            ;	t_dy*gm	x_2		t_dy*g1	g1		gm		g2
			 fxch		st(2)						;	t_dy*g1	x_2		t_dy*gm	g1		gm		g2
													
			fadd		[eax].comp_f[C_SX*4]		;	x_1		x_2		t_dy*gm	g1		gm		g2
			 fxch		st(3)						;	g1		x_2		t_dy*gm	x_1		gm		g2
													
			fadd		fp_conv_d16		            ;	g1+C	x_2		t_dy*gm	x_1		gm		g2
			 fxch		st(2)						;	t_dy*gm	x_2		g1+C	x_1		gm		g2
													
			fadd		[eax].comp_f[C_SX*4]		;	x_m		x_2		g1+C	x_1		gm		g2
			 fxch		st(4)						;	gm		x_2		g1+C	x_1		x_m		g2
													
			fadd		fp_conv_d16		            ;	gm+C	x_2		g1+C	x_1		x_m		g2
			 fxch		st(1)						;	x_2		gm+C	g1+C	x_1		x_m		g2
													
			fadd	fconv_d16_12[esi*8]	            ;	x_2+C	gm+C	g1+C	x_1		x_m		g2
			 fxch		st(5)						;	g2		gm+C	g1+C	x_1		x_m		x_2+C
													
			fadd		fp_conv_d16		            ;	g2+C	gm+C	g1+C	x_1		x_m		x_2+C
			 fxch		st(2)						;	g1+C	gm+C	g2+C	x_1		x_m		x_2+C
													
			fstp real8 ptr [workspace].x1			;	gm+C	g2+C	x_1		x_m		x_2+C
			fstp real8 ptr [workspace].xm			;	g2+C	x_1		x_m		x_2+C
			fstp real8 ptr [workspace].x2			;	x_1		x_m		x_2+C

			fadd	fconv_d16_12[esi*8]				;	x_1+C	x_m		x_2+C
			fxch		st(1)						;	x_m		x_1+C	x_2+C

			fadd	fconv_d16_m[esi*8]				;	x_m+C	x_1+C	x_2+C


		; Load deltas back in registers
		;
			mov			edx,[workspace].xm	; read fixed d_xm
			mov			esi,[workspace].x1	; read fixed d_x1

			mov			edi,[workspace].x2	; read fixed d_x2
			mov		ebx,[workspace.v0]				; Start preparing for parmeter setup
		
			fstp real8 ptr [workspace].xm			;	x_1+C	x_2+C
			fstp real8 ptr [workspace].x1			;	x_2+C

			mov			ecx,[workspace].xm
			mov			[workspace].xm+4,edx
			
			sar			ecx,16
			mov			[workspace].x1+4,esi

			sar			edx,16			; get integer part of x delta down major edge

			mov			[workspace.t_dx],ecx

			fild		[workspace.t_dx]			;	t_x		x_2+C


		; Generate floating point versions of x delta and x delta+4
		;
			mov			[workspace.xstep_0],edx
			inc edx

			mov			[workspace.xstep_1],edx
			mov			edx,[workspace.v2]				; Start preparing for parmeter setup

													;	0		1		2		3		4		5		6		7
			fsub		[eax].comp_f[C_SX*4]		;	t_dx	x_2+C
			 fxch		st(1)						;	x_2+C	t_dx
			fstp real8 ptr [workspace].x2			;	t_dx

			fild		[workspace.xstep_0]			;	xstep_0	t_dx
			fild		[workspace.xstep_1]			;	xstep_1 xstep_0	t_dx
			 fxch		st(2)			;	tdx		xstep_0	xstep_1
			fstp		[workspace.t_dx]			;	xstep_0	xstep_1

			mov			[workspace].x2+4,edi
			mov		ecx,[workspace.v1]				; Start preparing for parmeter setup

			fstp		[workspace.xstep_0]			;	step_1
			fstp		[workspace.xstep_1]			;

			jmp			exit

	; Special cases for top or bottom counts == 0
	;
top_zero:
			cmp			ebp,edi			; Check for completely empty triangle
			je			empty_triangle
													;	0		1		2		3		4		5		6		7
			fmul		st(1),st					;	1/2area	dy1*a	dy2		dx1		dx2

			fld			fp_one						;	1.0		1/2area	dy1*a	dy2		dx1		dx2
			 fxch	   st(3)						;   dy2  	1/2area	dy1*a	1.0		dx1		dx2

			fmul		st,st(1)					;	dy2*a  	1/2area	dy1*a	1.0		dx1		dx2

			fld			[edx].comp_f[C_SY*4]		;   sy3	 dy2*a  	1/2area	dy1*a	1.0		dx1		dx2
			fsub		[ebx].comp_f[C_SY*4]		;   dsy2	dy2*a  	1/2area	dy1*a	1.0		dx1		dx2
			 fxch	   st(5)						;   dx1	  dy2*a 	1/2area	dy1*a	1.0		dsy2	dx2

			jmp		count_cont

bottom_zero:
													;	0		1		2		3		4		5		6		7
			fmul		st(1),st					;	1/2area	dy1*a	dy2		dx1		dx2

			fld			[ebx].comp_f[C_SY*4]		;	sy2		1/2area	dy1*a	dy2		dx1		dx2
			fsub		[eax].comp_f[C_SY*4]		;   dsy1	1/2area	dy1*a	dy2		dx1		dx2
			 fxch	   st(3)						;   dy2  	1/2area	dy1*a	dsy1	dx1		dx2

			fmul		st,st(1)					;	dy2*a  	1/2area	dy1*a	dsy1	dx1		dx2

			fld			fp_one						;   1.0	 dy2*a  	1/2area	dy1*a	dsy1	dx1		dx2
			 fxch	   st(5)						;   dx1	  dy2*a 	1/2area	dy1*a	dsy1	1.0		dx2

			jmp		count_cont

	; Fill in block with NULL count and exit
	;
empty_triangle:
			mov workspace.topCount,-1
			mov workspace.bottomCount,-1
			fstp		st(0)
			fstp		st(0)
			fstp		st(0)
			fstp		st(0)
			fstp		st(0)
			ret
exit:
endm

SETUP_FIXED	macro
	local count_cont,exit,top_zero,bottom_zero,empty_triangle

converted_vertex struct
_x				dword	?
				dword	?,?
_y				dword	?
converted_vertex ends

		assume eax: ptr brp_vertex, ebx: ptr brp_vertex, ecx: ptr brp_vertex, edx: ptr brp_vertex
			
		; Convert X,Y to float
		;
			fild		[eax].comp_x[C_SX*4]
			fild		[eax].comp_x[C_SY*4]

			fild		[ecx].comp_x[C_SX*4]
			fild		[ecx].comp_x[C_SY*4]

			fild		[edx].comp_x[C_SX*4]
			fild		[edx].comp_x[C_SY*4]

			 fxch		st(1)

			fstp		[workspace.v2_x]
			fstp		[workspace.v2_y]

			fstp		[workspace.v1_y]
			fstp		[workspace.v1_x]

			fstp		[workspace.v0_y]
			fstp		[workspace.v0_x]


		; Calculate area of triangle and generate dx1/2area, dx1/2area, dx1/2area and dx1/2area
		;
		; Also sort the vertices in Y order whilst divide is happening
		;
													;	0		1		2		3		4		5		6		7
			fld			[workspace.v2_x]			;	x2
			fsub		[workspace.v0_x]			;	dx2
			fld			[workspace.v1_x]			;	x1		dx2
			fsub		[workspace.v0_x]			;	dx1		dx2
			fld			[workspace.v2_y]			;	y2		dx1		dx2
			fsub		[workspace.v0_y]			;	dy2		dx1		dx2
			fld			[workspace.v1_y]			;	y1		dy2		dx1		dx2
			fsub		[workspace.v0_y]			;	dy1		dy2		dx1		dx2

			fld			st(2)						;	dx1		dy1		dy2		dx1		dx2
			fmul		st,st(2)					;	dx1*dy2	dy1		dy2		dx1		dx2

			fld			st(4)						;	dx2		dx1*dy2	dy1		dy2		dx1		dx2
			fmul		st,st(2)					;	dx2*dy1	dx1*dy2	dy1		dy2		dx1		dx2

			mov			eax,[eax].comp_x[C_SY*4]

			mov			ecx,[ecx].comp_x[C_SY*4]

			fsubp		st(1),st					;	2area	dy1		dy2		dx1		dx2

			xor			ebx,ebx
			cmp			ecx,eax

			rcl			ebx,1
			mov			edx,[edx].comp_x[C_SY*4]

			fdivr		fp_one_fixed				;	1/2area	dy1		dy2		dx1		dx2

			cmp			edx,eax

			rcl			ebx,1
			cmp			edx,ecx

			rcl			ebx,1			; ebx now has 3 bit number characterising the order of the vertices.

			mov			sort_value,ebx
;V

			mov			eax,sort_table_0[ebx*4]
			mov			edx,sort_table_2[ebx*4]

			mov			esi,flip_table[ebx*4]
			mov			ebx,sort_table_1[ebx*4]

	; Load eax,ebx,edx with pointers to the three vertices in vertical order
	;
			mov			eax,[workspace.v0+eax]
			mov			edx,[workspace.v0+edx]

			mov			ebx,[workspace.v0+ebx]
			mov			[workspace.flip],esi



	; Work out Y extents of triangle
	;

			mov		 ebp,[eax].comp_x[C_SY*4]
			sar		 ebp,16				; EBP = y_t

			mov		 esi,[ebx].comp_x[C_SY*4]
			sar		 esi,16				; ESI = y_m

			mov		 edi,[edx].comp_x[C_SY*4]
			sar		 edi,16				; EDI = y_b


	; Reload eax,ebx,edx with pointers to converted copies of vertices x,y
	;
			mov			ebx,sort_value
			mov			[workspace.top_vertex],eax		; remeber for parameter setup

			mov			eax,sort_table_0[ebx*4]
			mov			edx,sort_table_2[ebx*4]

			mov			ebx,sort_table_1[ebx*4]
;V

			lea			eax,[workspace.v0_x+eax]
			lea			edx,[workspace.v0_x+edx]

			lea			ebx,[workspace.v0_x+ebx]
;V

		assume eax: ptr converted_vertex, ebx:ptr converted_vertex,  edx: ptr converted_vertex

		; Catch special cases of empty top or bottom trapezoids
		;
			cmp			ebp,esi
			je			top_zero

			cmp			esi,edi
			je			bottom_zero
			

	; Parameter gradient startup and Y deltas for edge gradients
	;
													;	0		1		2		3		4		5		6		7
			fmul		st(1),st					;	1/2area	dy1*a	dy2		dx1		dx2

			fld			[ebx]._y					;	sy2		1/2area	dy1*a	dy2		dx1		dx2
			fsub		[eax]._y					;   dsy1	1/2area	dy1*a	dy2		dx1		dx2
			 fxch	   st(3)						;   dy2  	1/2area	dy1*a	dsy1	dx1		dx2

			fmul		st,st(1)					;	dy2*a  	1/2area	dy1*a	dsy1	dx1		dx2

			fld			[edx]._y					;   sy3	 dy2*a  	1/2area	dy1*a	dsy1	dx1		dx2
			fsub		[ebx]._y					;   dsy2	dy2*a  	1/2area	dy1*a	dsy1	dx1		dx2
			 fxch	   st(5)						;   dx1	  dy2*a 	1/2area	dy1*a	dsy1	dsy2	dx2
count_cont:
			fmul		st,st(2)					;	dx1*a   dy2*a  	1/2area	dy1*a	dsy1	dsy2	dx2

			fld			[edx]._y					;   sy3	 dx1*a   dy2*a  	1/2area	dy1*a	dsy1	dsy2	dx2
			fsub		[eax]._y					;   dsy3	dx1*a   dy2*a  	1/2area	dy1*a	dsy1	dsy2	dx2
			 fxch	   st(7)						;   dx2		dx1*a   dy2*a  	1/2area	dy1*a	dsy1	dsy2	dsy3

			fmul		st,st(3)					;   dx2*a	dx1*a   dy2*a  	1/2area	dy1*a	dsy1	dsy2	dsy3
			 fxch		st(3)						;   1/2area	dx1*a   dy2*a  	dx2*a	dy1*a	dsy1	dsy2	dsy3

			fstp		workspace.iarea
			fstp		[workspace.dx1_a]
			fstp		[workspace.dy2_a]
			fstp		[workspace.dx2_a]
			fstp		[workspace.dy1_a]			;  	dy1		dy2		dy3


	; Find edge gradients of triangle
	;
	; R = 1/(dy1.dy2.dy3)
	;
	; gradient_major = dy1.dx2.dy3.R
	; gradient_minor1 = dx1.dy2.dy3.R
	; gradient_minor2 = dy1.dy2.dx3.R
	;
										;	0		1		2		3		4		5		6		7
			fld			st(2)			;	dy3		dy1		dy2		dy3
			fmul		st,st(2)		;	dy2*dy3	dy1		dy2		dy3

			fld			[ebx]._x		;	x2		dy2*dy3	dy1		dy2		dy3
			fsub		[eax]._x		;	dx1		dy2*dy3	dy1		dy2		dy3

			fld			st(1)			;	dy2*dy3 dx1		dy2*dy3	dy1		dy2		dy3
			fmul		st,st(3)		;	dy123	dx1		dy2*dy3	dy1		dy2		dy3

			fld			[edx]._x		;	x3		dy123	dx1		dy2*dy3	dy1		dy2		dy3
			fsub		[ebx]._x		;	dx2		dy123	dx1		dy2*dy3	dy1		dy2		dy3
			 fxch		 st(2)			;	dx1		dy123	dx2		dy2*dy3	dy1		dy2		dy3
			fld			fp_one			;	1.0		dx1		dy123	dx2		dy2*dy3	dy1		dy2		dy3
			fdivrp		st(2),st		;	dx1		R		dx2		dy2*dy3	dy1		dy2		dy3

	; Generate counts
	;
			inc			ebp
			mov			ecx,esi

			sub			ecx,ebp				;  count_t = (y_m-y_t)-1

			sal			ebp,16

			mov			[workspace.t_y],ebp			; save for X intercept calcs

			mov			[workspace.topCount],ecx
			inc			esi

			sub			edi,esi				;  count_b = (y_b-y_m)-1

			sal			esi,16

			mov			m_y,esi				; save for X intercept calcs

			mov			[workspace].bottomCount,edi
			mov			esi,[workspace.flip]

	; Generate LR/RL flag into esi (used to index convertion numbers below)
	;
			mov			edi,workspace.iarea
;V
			xor			esi,edi			; Build LR flag in bit 31
;V
			shr			esi,31			; move down to bit 0
;V
			mov			[workspace.flip],esi


	;XXX Setup screen pointers and strides
	;

	;XXX Work out which scan convertion function to use
	;

		; Finish of gradient calculations, interleaved with working out t_dy, and m_dy, the fractions
		; that the top and middle vertices are from the integer scanline boundaries
		;
		; t_dy = (yt+1) - vt->y
		; m_dy = (ym+1) - vm->y
		;
										;	0		1		2		3		4		5		6		7
			fmulp		st(3),st		;	R		dx2		XYY		dy1		dy2		dy3

			fld			[edx]._x		;	x3		R		dx2		XYY		dy1		dy2		dy3
			 fxch		st(3)			;	XYY		R		dx2		x3		dy1		dy2		dy3

			fmul		st,st(1)		;	XYY*R	R		dx2		x3		dy1		dy2		dy3
			 fxch		st(3)			;	x3		R		dx2		XYY*R	dy1		dy2		dy3

			fsub		[eax]._x		;	dx3		R		dx2		XYY*R	dy1		dy2		dy3
			 fxch		st(1)			;	R		dx3		dx2		XYY*R	dy1		dy2		dy3

			fmulp		st(4),st		;	dx3		dx2		XYY*R	dy1*R	dy2		dy3
			 fxch		st(2)			;	XYY*R	dx2		dx3		dy1*R	dy2		dy3

			fild		m_y				;	m_y		XYY*R	dx2		dx3		dy1*R	dy2		dy3
			 fxch		st(2)			;	dx2		XYY*R	m_y		dx3		dy1*R	dy2		dy3

			fmulp		st(6),st		;	XYY*R	m_y		dx3		dy1*R	dy2		dx2*dy3

			fild		[workspace.t_y]	;	t_y		XYY*R	m_y		dx3		dy1*R	dy2		dx2*dy3
			 fxch		st(3)			;	dx3		XYY*R	m_y		t_y		dy1*R	dy2		dx2*dy3

			fmulp		st(5),st		;	XYY*R	m_y		t_y		dy1*R	dy2*dx3	dx2*dy3
			 fxch		st(1)			;	m_y		XYY*R	t_y		dy1*R	dy2*dx3	dx2*dy3

			fsub		[ebx]._y		;	m_dy	XYY*R	t_y		dy1*R	dy2*dx3	dx2*dy3
			 fxch		st(3)			;	dy1*R	XYY*R	t_y		m_dy	dy2*dx3	dx2*dy3

			fmul		st(4),st		;	dy1*R	XYY*R	t_y		m_dy	YYX*R	dx2*dy3
			 fxch		st(2)			;	t_y		XYY*R	dy1*R	m_dy	YYX*R	dx2*dy3

			fsub		[eax]._y		;	t_dy	XYY*R	dy1*R	m_dy	YYX*R	dx2*dy3
			 fxch		st(2)			;	dy1*R	XYY*R	t_dy	m_dy	YYX*R	dx2*dy3

			fmulp		st(5),st		;	XYY*R	t_dy	m_dy	YYX*R	YXY*R
			 fxch		st(2)			;	m_dy	t_dy	XYY*R	YYX*R	YXY*R
										;	m_dy	t_dy	g1		gm		g2

	; Work out initial X intercepts with top and middle scanlines
	;
	; x_major  = gm * t_dy + vt->x
	; x_minor1 = g1 * t_dy + vt->x
	; x_minor2 = g2 * m_dy + vm->x
	;
													;	0		1		2		3		4		5		6		7
			fld			st(1)						;	t_dy	m_dy	t_dy	g1		gm		g2
			fxch		st(1)						;	m_dy	t_dy	t_dy	g1		gm		g2
										            
										            
			fmul		st,st(5)					;	m_dy*g2	t_dy	t_dy	g1		gm		g2
			fxch		st(2)						;	t_dy	t_dy	m_dy*g2	g1		gm		g2
										            
			fst			[workspace.t_dy]

			fmul		st,st(3)					;	t_dy*g1	t_dy	m_dy*g2	g1		gm		g2
			 fxch		st(2)						;	m_dy*g2	t_dy	t_dy*g1	g1		gm		g2
													
			fadd		[ebx]._x		            ;	x_2		t_dy	t_dy*g1	g1		gm		g2
			 fxch		st(1)			            ;	t_dy	x_2		t_dy*g1	g1		gm		g2
													
			fmul		st,st(4)					;	t_dy*gm	x_2		t_dy*g1	g1		gm		g2
			 fxch		st(2)			            ;	t_dy*g1	x_2		t_dy*gm	g1		gm		g2

			fadd		[eax]._x					;	x_1		x_2		t_dy*gm	g1		gm		g2
			 fxch		st(3)						;	g1		x_2		t_dy*gm	x_1		gm		g2
													
			fadd		fp_conv_d16		            ;	g1+C	x_2		t_dy*gm	x_1		gm		g2
			 fxch		st(2)			            ;	t_dy*gm	x_2		g1+C	x_1		gm		g2
													
			fadd		[eax]._x					;	x_m		x_2		g1+C	x_1		gm		g2
			 fxch		st(4)			            ;	gm		x_2		g1+C	x_1		x_m		g2

			fadd		fp_conv_d16					;	gm+C	x_2		g1+C	x_1		x_m		g2
			 fxch		st(1)						;	x_2		gm+C	g1+C	x_1		x_m		g2
													
			fadd	xconv_d16_12[esi*8]	            ;	x_2+C	gm+C	g1+C	x_1		x_m		g2
			 fxch		st(5)			            ;	g2		gm+C	g1+C	x_1		x_m		x_2+C
													
			fadd		fp_conv_d16					;	g2+C	gm+C	g1+C	x_1		x_m		x_2+C
			 fxch		st(2)			            ;	g1+C	gm+C	g2+C	x_1		x_m		x_2+C

			fstp real8 ptr [workspace].x1			;	gm+C	g2+C	x_1		x_m		x_2+C
			fstp real8 ptr [workspace].xm			;	g2+C	x_1		x_m		x_2+C
			fstp real8 ptr [workspace].x2			;	x_1		x_m		x_2+C
											        
			fadd	xconv_d16_12[esi*8]		        ;	x_1+C	x_m		x_2+C
			fxch		st(1)						;	x_m		x_1+C	x_2+C
													
			fadd	xconv_d16_m[esi*8]		        ;	x_m+C	x_1+C	x_2+C


		; Load deltas back in registers
		;
			mov			edx,[workspace].xm	; read fixed d_xm
			mov			esi,[workspace].x1	; read fixed d_x1

			mov			edi,[workspace].x2	; read fixed d_x2
			mov		ebx,[workspace.v0]				; Start preparing for parmeter setup
		
			fstp real8 ptr [workspace].xm			;	x_1+C	x_2+C
			fstp real8 ptr [workspace].x1			;	x_2+C

			mov			ecx,[workspace].xm
			mov			[workspace].xm+4,edx
			
			mov			[workspace].x1+4,esi
			and			ecx,0ffff0000h

			sar			edx,16			; get integer part of x delta down major edge
			mov			[workspace.t_dx],ecx

			mov		ecx,[workspace.v1]				; Start preparing for parmeter setup

			fild		[workspace.t_dx]			;	t_x		x_2+C

		; Generate floating point versions of x delta and x delta+4
		;
			mov			[workspace.xstep_0],edx
			inc			edx
			mov			[workspace.xstep_1],edx
			mov			edx,[workspace.v2]				; Start preparing for parmeter setup

			fsub		[eax]._x					;	t_dx	x_2+C
			 fxch		st(1)						;	x_2+C	t_dx
			fstp real8 ptr [workspace].x2			;	t_dx

		; Load eax with ptr to top vertex
		;
			mov			eax,[workspace.top_vertex]
			mov			[workspace].x2+4,edi

			fstp		[workspace.t_dx]			;

			fild		[workspace.xstep_0]			; xstep_0
			fild		[workspace.xstep_1]			; xstep_1 xstep_0

		; Scale the t_d? fractions down by 65536.0 by modifying the exponent directly
		;

			mov			esi,workspace.t_dx

			test		esi,esi
			je			tdx_zero

			sub			esi,08000000h
tdx_zero:	mov			edi,workspace.t_dy

			test		edi,edi
			je			tdy_zero

			sub			edi,08000000h
tdy_zero:	mov			workspace.t_dx,esi

			mov			workspace.t_dy,edi

			fstp		workspace.xstep_1			; xstep_0
			fstp		workspace.xstep_0			;

			jmp			exit

		assume eax: ptr converted_vertex, ebx:ptr converted_vertex,  edx: ptr converted_vertex

	; Special cases for top or bottom counts == 0
	;
top_zero:
			cmp			ebp,edi			; Check for completely empty triangle
			je			empty_triangle
													;	0		1		2		3		4		5		6		7
			fmul		st(1),st					;	1/2area	dy1*a	dy2		dx1		dx2

			fld			fp_one						;	1.0		1/2area	dy1*a	dy2		dx1		dx2
			 fxch	   st(3)		    			;   dy2  	1/2area	dy1*a	1.0		dx1		dx2
													
			fmul		st,st(1)		            ;	dy2*a  	1/2area	dy1*a	1.0		dx1		dx2
										            
			fld			[edx]._y					;   sy3	 dy2*a  	1/2area	dy1*a	1.0		dx1		dx2
			fsub		[ebx]._y					;   dsy2	dy2*a  	1/2area	dy1*a	1.0		dx1		dx2
			 fxch	   st(5)		                ;   dx1	  dy2*a 	1/2area	dy1*a	1.0		dsy2	dx2

			jmp		count_cont

bottom_zero:
													;	0		1		2		3		4		5		6		7
			fmul		st(1),st					;	1/2area	dy1*a	dy2		dx1		dx2
													
			fld			[ebx]._y		            ;	sy2		1/2area	dy1*a	dy2		dx1		dx2
			fsub		[eax]._y		            ;   dsy1	1/2area	dy1*a	dy2		dx1		dx2
			 fxch	   st(3)		    			;   dy2  	1/2area	dy1*a	dsy1	dx1		dx2
													
			fmul		st,st(1)		            ;	dy2*a  	1/2area	dy1*a	dsy1	dx1		dx2

			fld			fp_one						;   1.0	 dy2*a  	1/2area	dy1*a	dsy1	dx1		dx2
			 fxch	   st(5)						;   dx1	  dy2*a 	1/2area	dy1*a	dsy1	1.0		dx2

			jmp		count_cont

	; Fill in block with NULL count and exit
	;
empty_triangle:
			mov workspace.topCount,-1
			mov workspace.bottomCount,-1
			fstp		st(0)
			fstp		st(0)
			fstp		st(0)
			fstp		st(0)
			fstp		st(0)
			ret

exit:
			mov			esi,workspace.t_y
			shr			esi,16
			mov			workspace.t_y,esi
endm



; macros to extract the colour index for the flat shaders. 

SETUP_FLOAT_COLOUR_SHADETABLE macro
	fld dword ptr [eax+4*C_I]
	fadd fp_conv_d16
	mov esi,work.shade_table
	xor ebx,ebx
	mov bl,byte ptr work.index_base
	fstp qword ptr temporary_intensity
	mov bh, byte ptr temporary_intensity+2
	mov bl,[esi+ebx]
	mov byte ptr workspace.colour,bl
endm

SETUP_FLOAT_COLOUR macro
	fld dword ptr [eax+4*C_I]
	fadd fp_conv_d16
	fstp qword ptr temporary_intensity
	mov bl, byte ptr temporary_intensity+2
	mov byte ptr workspace.colour,bl
endm

SETUP_FIXED_COLOUR_SHADETABLE macro
    xor ebx,ebx
	mov esi,work.shade_table
	mov bh, byte ptr [eax+4*C_I+2]
	mov bl,byte ptr work.index_base
	mov bl,[esi+ebx]
	mov byte ptr workspace.colour,bl
endm

SETUP_FIXED_COLOUR macro
    mov bl, byte ptr [eax+4*C_I+2]
	mov byte ptr workspace.colour,bl
endm

; macros to setup lighting for special cased flat lighting.

SETUP_FLOAT_FLAT_LIT macro
	fld dword ptr [eax+4*C_I]
	fadd fp_conv_d16
	xor ebx,ebx
	mov edi,work.shade_table
	fstp qword ptr temporary_intensity
	mov bh, byte ptr temporary_intensity+2
	add edi,ebx
	mov workspace.shadeTable,edi
endm

SETUP_FIXED_FLAT_LIT macro
    xor ebx,ebx
	mov edi,work.shade_table
	mov bh, byte ptr [eax+4*C_I+2]
	add edi,ebx
	mov workspace.shadeTable,edi
endm



;Arbitrary width specific setup, could do with a little optimisation.

SETUP_FPU macro
	fnstcw	old_fpu_cw
	fldcw	fpu_cw
endm



SETUP_FLAGS macro ; approx 21 cycles fixed, 45 cycles float
	mov edx,workspace.v2
	mov eax,workspace.v0
	mov ecx,workspace.v1
	mov esi,2
if BASED_FIXED
	mov ebx,dword ptr[edx+4*C_U]
	mov ebp,dword ptr[eax+4*C_U]
	mov edi,dword ptr[ecx+4*C_U]
else
	fld dword ptr[edx+4*C_U]
	fadd fp_conv_d16
	fld dword ptr[eax+4*C_U]
	fadd fp_conv_d16
	fld dword ptr[ecx+4*C_U]
	fadd fp_conv_d16
	fld dword ptr[edx+4*C_V]
	fadd fp_conv_d16
	fld dword ptr[eax+4*C_V]
	fadd fp_conv_d16
	fld dword ptr[ecx+4*C_V]
	fadd fp_conv_d16
	 fxch st(2)
	fstp qword ptr workspace.scratch0
	fstp qword ptr workspace.scratch2
	fstp qword ptr workspace.scratch4
	fstp qword ptr workspace.scratch6
	fstp qword ptr workspace.scratch8
	fstp qword ptr workspace.scratch10
	mov ebx,workspace.scratch6
	mov ebp,workspace.scratch8
	mov edi,workspace.scratch10
endif
	and ebx,0ffff0000h
	and ebp,0ffff0000h
	and edi,0ffff0000h
	cmp ebx,ebp
	jne wrapped
	cmp ebx,edi
	jne wrapped
if BASED_FIXED
	mov ebx,dword ptr[edx+4*C_V]
	mov ebp,dword ptr[eax+4*C_V]
	mov edi,dword ptr[ecx+4*C_V]
else
	mov ebx,workspace.scratch0
	mov ebp,workspace.scratch2
	mov edi,workspace.scratch4
endif
	and ebx,0ffff0000h
	and ebp,0ffff0000h
	and edi,0ffff0000h
	cmp ebx,ebp
	jne wrapped
	cmp ebx,edi
	jne wrapped

	mov esi,0
wrapped:
	mov eax,workspace.flip
	or eax,esi
	mov workspaceA.flags,eax
endm



REMOVE_INTEGER_PARTS_OF_PARAM macro param
local paramNegative
	mov ebp,esi
	mov eax,param

	test eax,80000000h
	jnz paramNegative

	mov ebp,edi

	and eax,ebp
paramNegative:
	and ebp,esi

	or eax,ebp

	mov param,eax
endm



REMOVE_INTEGER_PARTS_OF_PARAMETERS macro
; assumes 8.24 format
	mov edi,0ffffffh
	mov esi,0ff000000h

	and workspace.s_u,0ffffffh
	and workspace.s_v,0ffffffh

	REMOVE_INTEGER_PARTS_OF_PARAM workspace.d_u_x
	REMOVE_INTEGER_PARTS_OF_PARAM workspace.d_u_y_0
	REMOVE_INTEGER_PARTS_OF_PARAM workspace.d_u_y_1

	REMOVE_INTEGER_PARTS_OF_PARAM workspace.d_v_x
	REMOVE_INTEGER_PARTS_OF_PARAM workspace.d_v_y_0
	REMOVE_INTEGER_PARTS_OF_PARAM workspace.d_v_y_1
endm

 

MULTIPLY_UP_PARAM_VALUES macro param,dimension,magic ;24 cycles
;										st(0)		st(1)		st(2)		st(3)		st(4)		st(5)		st(6)		st(7)
	fild work.texture.dimension		;	d
	fild workspace.s_&param			;	sp			d
	fild workspace.d_&param&_x		;	dpdx		sp			d
	fild workspace.d_&param&_y_0	;	dpdy0		dpdx		sp			d
	fxch st(2)						;	sp			dpdx		dpdy0		d
	fmul st,st(3)					;	spd			dpdx		dpdy0		d
	fild workspace.d_&param&_y_1	;	dpdy1		spd			dpdx		dpdy0		d
	fxch st(2)						;	dpdx		spd			dpdy1		dpdy0		d
	fmul st,st(4)					;	dpdxd		spd			dpdy1		dpdy0		d
	 fxch st(1)						;	spd			dpdxd		dpdy1		dpdy0		d
	fadd magic						;	spdx		dpdxd		dpdy1		dpdy0		d
	 fxch st(3)						;	dpdy0		dpdxd		dpdy1		spdx		d
	fmul st,st(4)					;	dpdy0d		dpdxd		dpdy1		spdx		d
	 fxch st(1)						;	dpdxd		dpdy0d		dpdy1		spdx		d
	fadd magic						;	dpdxdx		dpdy0d		dpdy1		spdx		d
	 fxch st(2)						;	dpdy1		dpdy0d		dpdxdx		spdx		d
	fmul st,st(4)					;	dpdy1d		dpdy0d		dpdxdx		spdx		d
	 fxch st(4)						;	d			dpdy0d		dpdxdx		spdx		dpdy1d
	fstp st(0)						;	dpdy0d		dpdxdx		spdx		dpdy1d
	fadd magic						;	dpdy0dx		dpdxdx		spdx		dpdy1d
	 fxch st(3)						;	dpdy1d		dpdxdx		spdx		dpdy0dx
	fadd magic						;	dpdy1dx		dpdxdx		spdx		dpdy0dx
	 fxch st(2)						;	spdx		dpdxdx		dpdy1dx		dpdy0dx
	fstp qword ptr workspaceA.s&param		;	dpdxdx		dpdy1dx		dpdy0dx
	fstp qword ptr workspaceA.d&param&x	;	dpdy1dx		dpdy0dx
	fstp qword ptr workspaceA.d&param&y1	;	dpdy0dx
	fstp qword ptr workspaceA.d&param&y0	;	
endm



MULTIPLY_UP_V_BY_STRIDE macro magic ; 24 cycles
;										st(0)		st(1)		st(2)		st(3)		st(4)		st(5)		st(6)		st(7)
	fild work.texture.stride_b		;	d
	fild workspaceA.sv					;	sp			d
	fild workspaceA.dvx					;	dpdx		sp			d
	fild workspaceA.dvy0					;	dpdy0		dpdx		sp			d
	fxch st(2)						;	sp			dpdx		dpdy0		d
	fmul st,st(3)					;	spd			dpdx		dpdy0		d
	fild workspaceA.dvy1					;	dpdy1		spd			dpdx		dpdy0		d
	fxch st(2)						;	dpdx		spd			dpdy1		dpdy0		d
	fmul st,st(4)					;	dpdxd		spd			dpdy1		dpdy0		d
	 fxch st(1)						;	spd			dpdxd		dpdy1		dpdy0		d
	fadd magic						;	spdx		dpdxd		dpdy1		dpdy0		d
	 fxch st(3)						;	dpdy0		dpdxd		dpdy1		spdx		d
	fmul st,st(4)					;	dpdy0d		dpdxd		dpdy1		spdx		d
	 fxch st(1)						;	dpdxd		dpdy0d		dpdy1		spdx		d
	fadd magic						;	dpdxdx		dpdy0d		dpdy1		spdx		d
	 fxch st(2)						;	dpdy1		dpdy0d		dpdxdx		spdx		d
	fmul st,st(4)					;	dpdy1d		dpdy0d		dpdxdx		spdx		d
	 fxch st(4)						;	d			dpdy0d		dpdxdx		spdx		dpdy1d
	fstp st(0)						;	dpdy0d		dpdxdx		spdx		dpdy1d
	fadd magic						;	dpdy0dx		dpdxdx		spdx		dpdy1d
	 fxch st(3)						;	dpdy1d		dpdxdx		spdx		dpdy0dx
	fadd magic						;	dpdy1dx		dpdxdx		spdx		dpdy0dx
	 fxch st(2)						;	spdx		dpdxdx		dpdy1dx		dpdy0dx
	fstp qword ptr workspaceA.sv			;	dpdxdx		dpdy1dx		dpdy0dx
	fstp qword ptr workspaceA.dvx			;	dpdy1dx		dpdy0dx
	fstp qword ptr workspaceA.dvy1		;	dpdy0dx
	fstp qword ptr workspaceA.dvy0		;
endm



SPLIT_INTO_INTEGER_AND_FRACTIONAL_PARTS macro ; 24 cycles
	mov ebx,workspaceA.sv

	shl ebx,16
	mov edx,workspaceA.dvx

	shl edx,16
	mov workspaceA.svf,ebx

	mov ebx,workspaceA.dvy0
	mov workspaceA.dvxf,edx

	shl ebx,16
	mov edx,workspaceA.dvy1

	shl edx,16
	mov workspaceA.dvy0f,ebx

	mov workspaceA.dvy1f,edx

;integer parts

	mov ebx,workspaceA.sv

	sar ebx,16
	mov edx,workspaceA.dvx

	sar edx,16
	mov workspaceA.sv,ebx

	mov ebx,workspaceA.dvy0
	mov workspaceA.dvx,edx

	sar ebx,16
	mov edx,workspaceA.dvy1

	sar edx,16
	mov workspaceA.dvy0,ebx

	mov workspaceA.dvy1,edx

endm



CREATE_CARRY_VERSIONS macro
	mov eax,workspaceA.dvy0
	mov ebx,workspaceA.dvy1

	add eax,work.texture.stride_b
	mov ecx,workspaceA.dvx

	add ebx,work.texture.stride_b
	add ecx,work.texture.stride_b

	mov workspaceA.dvy0c,eax
	mov workspaceA.dvy1c,ebx

	mov workspaceA.dvxc,ecx
endm



WRAP_SETUP macro
	mov ecx,work.texture.width_p
	mov eax,work.texture._size
	shl ecx,16
	add eax,work.texture.base
	mov workspaceA.uUpperBound,ecx
	mov workspaceA.vUpperBound,eax
endm



RESTORE_FPU macro
	fldcw	old_fpu_cw
endm



ARBITRARY_SETUP macro
	SETUP_FPU
	SETUP_FLAGS
	REMOVE_INTEGER_PARTS_OF_PARAMETERS
	MULTIPLY_UP_PARAM_VALUES u,width_p,fp_conv_d8r
	MULTIPLY_UP_PARAM_VALUES v,height,fp_conv_d8r
	SPLIT_INTO_INTEGER_AND_FRACTIONAL_PARTS
	MULTIPLY_UP_V_BY_STRIDE fp_conv_d
	CREATE_CARRY_VERSIONS
	WRAP_SETUP
	RESTORE_FPU
endm


OPTION PROLOGUE:NONE
OPTION EPILOGUE:NONE

ifdef BASED_FLOAT
if BASED_FLOAT

;**********************
; FLOATING POINT SETUP
;**********************

ECHO FLOAT

if PARTS and (PART_8Z or PART_15Z or PART_16Z or PART_24Z or PART_8Z4 or PART_15Z4 or PART_16Z4 or PART_24Z4 or PART_8Z_FOG or PART_8Z_BLEND or PART_8Z_FOG_BLEND)

ECHO Z-BUFFERED

TriangleSetup_ZTIS_ARBITRARY proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d16,1
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d24
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d24
			SETUP_FLOAT_PARAM C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d16
			SETUP_FLOAT_PARAM C_S,_s,workspace.s_s,workspace.d_s_x,fp_conv_d16
			ARBITRARY_SETUP
			ret
TriangleSetup_ZTIS_ARBITRARY endp

TriangleSetup_ZTI_ARBITRARY proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d16,1
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d24
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d24
			SETUP_FLOAT_PARAM C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d16
			ARBITRARY_SETUP
			ret
TriangleSetup_ZTI_ARBITRARY endp

TriangleSetup_ZTIS_ARBITRARY_FLAT proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d16,1
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d24
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d24
			SETUP_FLOAT_CONST C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d16
			SETUP_FLOAT_PARAM C_S,_s,workspace.s_s,workspace.d_s_x,fp_conv_d16
			ARBITRARY_SETUP
			ret
TriangleSetup_ZTIS_ARBITRARY_FLAT endp

TriangleSetup_ZTI_ARBITRARY_FLAT proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d16,1
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d24
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d24
			SETUP_FLOAT_CONST C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d16
			ARBITRARY_SETUP
			ret
TriangleSetup_ZTI_ARBITRARY_FLAT endp

TriangleSetup_ZTIS proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d16,1
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d16
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d16
			SETUP_FLOAT_PARAM C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d16
			SETUP_FLOAT_PARAM C_S,_s,workspace.s_s,workspace.d_s_x,fp_conv_d16
			ret
TriangleSetup_ZTIS endp

TriangleSetup_ZTI proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d16,1
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d16
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d16
			SETUP_FLOAT_PARAM C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d16
			ret
TriangleSetup_ZTI endp

TriangleSetup_ZTIS_FLAT proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d16,1
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d16
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d16
			SETUP_FLOAT_CONST C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d16
			SETUP_FLOAT_CONST C_S,_s,workspace.s_s,workspace.d_s_x,fp_conv_d16
			ret
TriangleSetup_ZTIS_FLAT endp

TriangleSetup_ZTI_FLAT proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d16,1
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d16
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d16
			SETUP_FLOAT_CONST C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d16
			ret
TriangleSetup_ZTI_FLAT endp

TriangleSetup_ZT_FLAT_LIT proc
			SETUP_FLOAT_FLAT_LIT
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d16,1
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d16
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d16
			ret
TriangleSetup_ZT_FLAT_LIT endp

TriangleSetup_ZT_ARBITRARY proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d16,1
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d24
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d24
			ARBITRARY_SETUP
			ret
TriangleSetup_ZT_ARBITRARY endp

TriangleSetup_ZT proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d16,1
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d16
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d16
			ret
TriangleSetup_ZT endp

TriangleSetup_ZI proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d16,1
			SETUP_FLOAT_PARAM C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d16
			ret
TriangleSetup_ZI endp

TriangleSetup_ZI_FLAT proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d16,1
			SETUP_FLOAT_CONST C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d16
			ret
TriangleSetup_ZI_FLAT endp

TriangleSetup_Z_ShadeTable proc
			SETUP_FLOAT_COLOUR_SHADETABLE
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d16,1
			ret
TriangleSetup_Z_ShadeTable endp

TriangleSetup_Z proc
			SETUP_FLOAT_COLOUR
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d16,1
			ret
TriangleSetup_Z endp

endif

if PARTS and (PART_8 or PART_15 or PART_16 or PART_24)

ECHO NON Z-BUFFERED

TriangleSetup_TIS_ARBITRARY proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d24
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d24
			SETUP_FLOAT_PARAM C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d16
			SETUP_FLOAT_PARAM C_S,_s,workspace.s_s,workspace.d_s_x,fp_conv_d16
			ARBITRARY_SETUP
			ret
TriangleSetup_TIS_ARBITRARY endp

TriangleSetup_TI_ARBITRARY proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d24
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d24
			SETUP_FLOAT_PARAM C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d16
			ARBITRARY_SETUP
			ret
TriangleSetup_TI_ARBITRARY endp

TriangleSetup_TIS_ARBITRARY_FLAT proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d24
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d24
			SETUP_FLOAT_CONST C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d16
			SETUP_FLOAT_CONST C_S,_s,workspace.s_s,workspace.d_s_x,fp_conv_d16
			ARBITRARY_SETUP
			ret
TriangleSetup_TIS_ARBITRARY_FLAT endp

TriangleSetup_TI_ARBITRARY_FLAT proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d24
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d24
			SETUP_FLOAT_CONST C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d16
			ARBITRARY_SETUP
			ret
TriangleSetup_TI_ARBITRARY_FLAT endp

TriangleSetup_TIS proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d16
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d16
			SETUP_FLOAT_PARAM C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d16
			SETUP_FLOAT_PARAM C_S,_s,workspace.s_s,workspace.d_s_x,fp_conv_d16
			ret
TriangleSetup_TIS endp

TriangleSetup_TI proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d16
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d16
			SETUP_FLOAT_PARAM C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d16
			ret
TriangleSetup_TI endp

TriangleSetup_TIS_FLAT proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d16
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d16
			SETUP_FLOAT_CONST C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d16
			SETUP_FLOAT_CONST C_S,_s,workspace.s_s,workspace.d_s_x,fp_conv_d16
			ret
TriangleSetup_TIS_FLAT endp

TriangleSetup_TI_FLAT proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d16
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d16
			SETUP_FLOAT_CONST C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d16
			ret
TriangleSetup_TI_FLAT endp

TriangleSetup_T_ARBITRARY proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d24
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d24
			ARBITRARY_SETUP
			ret
TriangleSetup_T_ARBITRARY endp

TriangleSetup_T proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d16
			SETUP_FLOAT_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d16
			ret
TriangleSetup_T endp

TriangleSetup_I proc
			SETUP_FLOAT
			SETUP_FLOAT_PARAM C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d16
			ret
TriangleSetup_I endp

TriangleSetup_I_FLAT proc
			SETUP_FLOAT
			SETUP_FLOAT_CONST C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d16
			ret
TriangleSetup_I_FLAT endp

TriangleSetup_ShadeTable proc
			SETUP_FLOAT_COLOUR_SHADETABLE
			SETUP_FLOAT
			ret
TriangleSetup_ShadeTable endp

TriangleSetup proc
			SETUP_FLOAT_COLOUR
			SETUP_FLOAT
			ret
TriangleSetup endp

endif

endif
endif

ifdef BASED_FIXED
if BASED_FIXED

;*******************
; FIXED POINT SETUP
;*******************

if PARTS and (PART_8Z or PART_15Z or PART_16Z or PART_24Z or PART_8Z4 or PART_15Z4 or PART_16Z4 or PART_24Z4 or PART_8Z_FOG or PART_8Z_BLEND or PART_8Z_FOG_BLEND)

ECHO Z-BUFFERED

ECHO FIXED

TriangleSetup_ZTI_ARBITRARY proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d,1
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d8
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d8
			SETUP_FIXED_PARAM C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d
			ARBITRARY_SETUP
			ret
TriangleSetup_ZTI_ARBITRARY endp

TriangleSetup_ZTIS_ARBITRARY proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d,1
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d8
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d8
			SETUP_FIXED_PARAM C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d
                        SETUP_FIXED_PARAM C_S,_s,workspace.s_s,workspace.d_s_x,fp_conv_d
			ARBITRARY_SETUP
			ret
TriangleSetup_ZTIS_ARBITRARY endp

TriangleSetup_ZTI_ARBITRARY_FLAT proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d,1
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d8
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d8
			SETUP_FIXED_CONST C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d
			ARBITRARY_SETUP
			ret
TriangleSetup_ZTI_ARBITRARY_FLAT endp

TriangleSetup_ZTIS_ARBITRARY_FLAT proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d,1
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d8
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d8
			SETUP_FIXED_CONST C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d
            SETUP_FIXED_CONST C_S,_s,workspace.s_s,workspace.d_s_x,fp_conv_d
			ARBITRARY_SETUP
			ret
TriangleSetup_ZTIS_ARBITRARY_FLAT endp


TriangleSetup_ZTI proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d,1
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d
			SETUP_FIXED_PARAM C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d
			ret
TriangleSetup_ZTI endp

TriangleSetup_ZTIS proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d,1
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d
			SETUP_FIXED_PARAM C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d
                        SETUP_FIXED_PARAM C_S,_s,workspace.s_s,workspace.d_s_x,fp_conv_d
			ret
TriangleSetup_ZTIS endp

TriangleSetup_ZTI_FLAT proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d,1
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d
			SETUP_FIXED_CONST C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d
			ret
TriangleSetup_ZTI_FLAT endp

TriangleSetup_ZT_FLAT_LIT proc
			SETUP_FIXED_FLAT_LIT
			SETUP_FIXED
			SETUP_FIXED_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d,1
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d
			ret
TriangleSetup_ZT_FLAT_LIT endp

TriangleSetup_ZTIS_FLAT proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d,1
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d
			SETUP_FIXED_CONST C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d
            SETUP_FIXED_CONST C_S,_s,workspace.s_s,workspace.d_s_x,fp_conv_d
			ret
TriangleSetup_ZTIS_FLAT endp

TriangleSetup_ZT_ARBITRARY proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d,1
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d8
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d8
			ARBITRARY_SETUP
			ret
TriangleSetup_ZT_ARBITRARY endp

TriangleSetup_ZT proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d,1
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d
			ret
TriangleSetup_ZT endp

TriangleSetup_ZI proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d,1
			SETUP_FIXED_PARAM C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d
			ret
TriangleSetup_ZI endp

TriangleSetup_ZI_FLAT proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d,1
			SETUP_FIXED_CONST C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d
			ret
TriangleSetup_ZI_FLAT endp

TriangleSetup_Z_ShadeTable proc
            SETUP_FIXED_COLOUR_SHADETABLE
			SETUP_FIXED
			SETUP_FIXED_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d,1
			ret
TriangleSetup_Z_ShadeTable endp

TriangleSetup_Z proc
            SETUP_FIXED_COLOUR
			SETUP_FIXED
			SETUP_FIXED_PARAM C_SZ,_z,workspace.s_z,workspace.d_z_x,fp_conv_d,1
			ret
TriangleSetup_Z endp

endif

if PARTS and (PART_8 or PART_15 or PART_16 or PART_24)

ECHO NON Z-BUFFERED

TriangleSetup_TI_ARBITRARY proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d8
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d8
			SETUP_FIXED_PARAM C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d
			ARBITRARY_SETUP
			ret
TriangleSetup_TI_ARBITRARY endp

TriangleSetup_TIS_ARBITRARY proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d8
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d8
			SETUP_FIXED_PARAM C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d
            SETUP_FIXED_PARAM C_S,_s,workspace.s_s,workspace.d_s_x,fp_conv_d
			ARBITRARY_SETUP
			ret
TriangleSetup_TIS_ARBITRARY endp

TriangleSetup_TI_ARBITRARY_FLAT proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d8
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d8
			SETUP_FIXED_CONST C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d
			ARBITRARY_SETUP
			ret
TriangleSetup_TI_ARBITRARY_FLAT endp

TriangleSetup_TIS_ARBITRARY_FLAT proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d8
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d8
			SETUP_FIXED_CONST C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d
            SETUP_FIXED_CONST C_S,_s,workspace.s_s,workspace.d_s_x,fp_conv_d
			ARBITRARY_SETUP
			ret
TriangleSetup_TIS_ARBITRARY_FLAT endp

TriangleSetup_TI proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d
			SETUP_FIXED_PARAM C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d
			ret
TriangleSetup_TI endp

TriangleSetup_TIS proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d
			SETUP_FIXED_PARAM C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d
                        SETUP_FIXED_PARAM C_S,_s,workspace.s_s,workspace.d_s_x,fp_conv_d
			ret
TriangleSetup_TIS endp

TriangleSetup_TI_FLAT proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d
			SETUP_FIXED_CONST C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d
			ret
TriangleSetup_TI_FLAT endp

TriangleSetup_TIS_FLAT proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d
			SETUP_FIXED_CONST C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d
            SETUP_FIXED_CONST C_S,_s,workspace.s_s,workspace.d_s_x,fp_conv_d
			ret
TriangleSetup_TIS_FLAT endp

TriangleSetup_T_ARBITRARY proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d8
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d8
			ARBITRARY_SETUP
			ret
TriangleSetup_T_ARBITRARY endp

TriangleSetup_T proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_U,_u,workspace.s_u,workspace.d_u_x,fp_conv_d
			SETUP_FIXED_PARAM C_V,_v,workspace.s_v,workspace.d_v_x,fp_conv_d
			ret
TriangleSetup_T endp

TriangleSetup_I proc
			SETUP_FIXED
			SETUP_FIXED_PARAM C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d
			ret
TriangleSetup_I endp

TriangleSetup_I_FLAT proc
			SETUP_FIXED
			SETUP_FIXED_CONST C_I,_i,workspace.s_i,workspace.d_i_x,fp_conv_d
			ret
TriangleSetup_I_FLAT endp

TriangleSetup_ShadeTable proc
            SETUP_FIXED_COLOUR_SHADETABLE
            SETUP_FIXED
			ret
TriangleSetup_ShadeTable endp

TriangleSetup proc
            SETUP_FIXED_COLOUR
            SETUP_FIXED
			ret
TriangleSetup endp

endif

endif
endif

if DEBUG
divzero		proc
			ret
divzero		endp

	.data
temp		dword		0
endif
.data

align 8
workspace _workspace <>
align 8
workspaceA ArbitraryWidthWorkspace {}

; Conversion values for major and minor edges, indexed by LR/RL flag*8
;
	; Fixed
	;


align 8
xconv_d16_12	qword	04338000000000000h	; Conv + 0.0
xconv_d16_m		qword	04338000000010000h 	; Conv + 1.0
				qword	04338000000000000h	; Conv + 0.0

	; Float
	;

fconv_d16_12	qword	04238000000000000h	; Conv + 0.0
fconv_d16_m		qword	04238000000010000h 	; Conv + 1.0
				qword	04238000000000000h	; Conv + 0.0

fp_one			dword	1.0
fp_one_fixed	dword	65536.0
fp_conv_d		dword	(127+52-0) shl 23 + (1 shl 22)
fp_conv_d8		dword	(127+52-8) shl 23 + (1 shl 22)
fp_conv_d8r		dword	(127+52+8) shl 23 + (1 shl 22)
fp_conv_d16		dword	(127+52-16) shl 23 + (1 shl 22)
fp_conv_d24		dword	(127+52-24) shl 23 + (1 shl 22)
fp_conv_d32		dword	(127+52-32) shl 23 + (1 shl 22)

fp_single_cw	word	107fh
fp_double_cw	word	127fh
fp_extended_cw	word	137fh,0
fpu_cw	dw		FPU_CW_IC_AFFINE+FPU_CW_RC_CHOP+FPU_CW_PC_DOUBLE+FPU_CW_MASK_ALL
old_fpu_cw dw ?

align 8 
temporary_intensity qword ?


	; Converts results of comparisons to sort order for vertices
	;
sort_table_1	dword   1*4
				dword   2*4
				dword   0*4
sort_table_0	dword   0*4
				dword   0*4
				dword   0*4
sort_table_2	dword   2*4
				dword   1*4
				dword   0*4
				dword   1*4
				dword   2*4
				dword   0*4
				dword   0*4
				dword   0*4

	; Parity table of 3 bit number
	;
flip_table		dword	000000000h		;000
				dword	080000000h		;001
				dword	080000000h		;010
				dword	000000000h		;011
				dword	080000000h		;100
				dword	000000000h		;101
				dword	000000000h		;110
				dword	080000000h		;111

end
										 
