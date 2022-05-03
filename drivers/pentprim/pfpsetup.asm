; Floating point Pentium optimised setup code for rasterisers
;
		.586p
		.model flat,c

		include drv.inc
		include 586_macs.inc

USE_WORKSPACE equ 1

; Single precision floating point format definitions
; Used for manipulating floats with integer instructions
;
MASK_SIGN		equ	080000000h
MASK_EXPONENT	equ	07f800000h
MASK_MANTISSA	equ 0007fffffh

EXPONENT_SHIFT	equ	23
EXPONENT_BIAS	equ	127

IMPLICIT_ONE	equ 1 shl EXPONENT_SHIFT
EXPONENT_OFFSET equ ((EXPONENT_BIAS + EXPONENT_SHIFT) shl EXPONENT_SHIFT) or MASK_MANTISSA

; FPU control work definitions
;
FPU_CW_PC_MASK equ 0300h
FPU_CW_PC_SHIFT equ 8
		
; Some equates that allow me to use the old style naming conventions (with .s replaced
; by _s) to refer to the workspace for the floating point setup
;
if USE_WORKSPACE

		include	fpwork.inc

work_main_i			equ	workspace.xm
work_main_d_i		equ	workspace.d_xm
work_main_y			equ	workspace.t_y

work_top_count		equ	workspace.topCount
work_top_i			equ	workspace.x1
work_top_d_i		equ	workspace.d_x1

work_bot_count		equ	workspace.bottomCount
work_bot_i			equ	workspace.x2
work_bot_d_i		equ	workspace.d_x2

work_pz_current		equ	workspace.s_z
work_pz_grad_x		equ	workspace.d_z_x
work_pz_d_nocarry	equ	workspace.d_z_y_0
work_pz_d_carry		equ	workspace.d_z_y_1

work_pu_current		equ	workspace.s_u
work_pu_grad_x		equ	workspace.d_u_x
work_pu_d_nocarry	equ	workspace.d_u_y_0
work_pu_d_carry		equ	workspace.d_u_y_1

work_pv_current		equ	workspace.s_v
work_pv_grad_x		equ	workspace.d_v_x
work_pv_d_nocarry	equ	workspace.d_v_y_0
work_pv_d_carry		equ	workspace.d_v_y_1

work_pi_current		equ	workspace.s_i
work_pi_grad_x		equ	workspace.d_i_x
work_pi_d_nocarry	equ	workspace.d_i_y_0
work_pi_d_carry		equ	workspace.d_i_y_1

workspace_v0            equ     workspace.v0
workspace_v1		equ	workspace.v1
workspace_v2		equ	workspace.v2

workspace_flip		equ	workspace.flip

workspace_iarea		equ	workspace.iarea

workspace_dx1_a		equ	workspace.dx1_a
workspace_dx2_a		equ	workspace.dx2_a
workspace_dy1_a		equ	workspace.dy1_a
workspace_dy2_a		equ	workspace.dy2_a

workspace_m_y		equ	m_y

workspace_t_dx		equ	workspace.t_dx
workspace_t_dy		equ	workspace.t_dy

workspace_xstep_0	equ	workspace.xstep_0
workspace_xstep_1	equ	workspace.xstep_1

else

work_main_i			equ	work.main.i			
work_main_d_i		equ	work.main.d_i		
work_main_y			equ	work.main.y			

work_top_count		equ	work.top.count		
work_top_i			equ	work.top.i			
work_top_d_i		equ	work.top.d_i		

work_bot_count		equ	work.bot.count		
work_bot_i			equ	work.bot.i			
work_bot_d_i		equ	work.bot.d_i		

work_pz_current		equ	work.pz.current		
work_pz_currentpix	equ	work.pz.currentpix		
work_pz_grad_x		equ	work.pz.grad_x		
work_pz_d_nocarry	equ	work.pz.d_nocarry	
work_pz_d_carry		equ	work.pz.d_carry		

work_pu_current		equ	work.pu.current		
work_pu_currentpix	equ	work.pu.currentpix		
work_pu_grad_x		equ	work.pu.grad_x		
work_pu_d_nocarry	equ	work.pu.d_nocarry	
work_pu_d_carry		equ	work.pu.d_carry		

work_pv_current		equ	work.pv.current		
work_pv_currentpix	equ	work.pv.currentpix		
work_pv_grad_x		equ	work.pv.grad_x		
work_pv_d_nocarry	equ	work.pv.d_nocarry	
work_pv_d_carry		equ	work.pv.d_carry		

work_pq_current		equ	work.pq.current		
work_pq_currentpix	equ	work.pq.currentpix		
work_pq_grad_x		equ	work.pq.grad_x		
work_pq_d_nocarry	equ	work.pq.d_nocarry	
work_pq_d_carry		equ	work.pq.d_carry		

work_pi_current		equ	work.pi.current		
work_pi_currentpix	equ	work.pi.currentpix		
work_pi_grad_x		equ	work.pi.grad_x		
work_pi_d_nocarry	equ	work.pi.d_nocarry	
work_pi_d_carry		equ	work.pi.d_carry		

workspace_v0		equ	v0
workspace_v1		equ	v1
workspace_v2		equ	v2

workspace_flip		equ	flip

workspace_iarea		equ	workspace.iarea

workspace_dx1_a		equ	dx1_a
workspace_dx2_a		equ	dx2_a
workspace_dy1_a		equ	dy1_a
workspace_dy2_a		equ	dy2_a

workspace_m_y		equ	m_y

workspace_t_dx		equ	t_dx
workspace_t_dy		equ	t_dy

workspace_xstep_0	equ	xstep_0
workspace_xstep_1	equ	xstep_1

endif


; Do all the per-triangle work 
;
; Total 173 cycles (less if fpu not set to extended precision)
;
;	eax: ptr to vertex0
;	ecx: ptr to vertex1
;	edx: ptr to vertex2
;
SETUP_FLOAT	macro empty:=<ret>

		local count_cont,exit,top_zero,bottom_zero,empty_triangle

		assume eax: ptr brp_vertex, ebx: ptr brp_vertex, ecx: ptr brp_vertex, edx: ptr brp_vertex

	; Calculate area of triangle and generate dx1/2area, dx1/2area, dx1/2area and dx1/2area
	;
	; Also sort the vertices in Y order whilst divide is happening
	;
											;	0		1		2		3		4		5		6		7
		fld		[edx].comp_f[C_SX*4]		;	x2
		fsub	[eax].comp_f[C_SX*4]		;	dx2
		fld		[ecx].comp_f[C_SX*4]		;	x1		dx2
		fsub	[eax].comp_f[C_SX*4]		;	dx1		dx2
		fld		[edx].comp_f[C_SY*4]		;	y2		dx1		dx2
		fsub	[eax].comp_f[C_SY*4]		;	dy2		dx1		dx2
		fld		[ecx].comp_f[C_SY*4]		;	y1		dy2		dx1		dx2
		fsub	[eax].comp_f[C_SY*4]		;	dy1		dy2		dx1		dx2

		fld		st(2)						;	dx1		dy1		dy2		dx1		dx2
		fmul	st,st(2)					;	dx1*dy2	dy1		dy2		dx1		dx2

		fld		st(4)						;	dx2		dx1*dy2	dy1		dy2		dx1		dx2
		fmul	st,st(2)					;	dx2*dy1	dx1*dy2	dy1		dy2		dx1		dx2

		mov		workspace_v0,eax
		mov		eax,[eax].comp_f[C_SY*4]

		mov		workspace_v1,ecx
		mov		ecx,[ecx].comp_f[C_SY*4]

		fsubp	st(1),st					;	2area	dy1		dy2		dx1		dx2

		xor		ebx,ebx
		cmp		ecx,eax

		mov		workspace_v2,edx
		mov		edx,[edx].comp_f[C_SY*4]

		fdivr	fp_one						;	1/2area	dy1		dy2		dx1		dx2

		rcl		ebx,1
		cmp		edx,eax

		rcl		ebx,1
		cmp		edx,ecx

		rcl		ebx,1			; ebx now has 3 bit number characterising the order of the vertices.

;U
;V
		mov		eax,sort_table_0[ebx*4]
		mov		edx,sort_table_2[ebx*4]

		mov		esi,flip_table[ebx*4]
		mov		ebx,sort_table_1[ebx*4]

	; Load eax,ebx,edx with pointers to the three vertices in vertical order
	;
		mov		eax,[workspace_v0+eax]
		mov		edx,[workspace_v0+edx]

		mov		ebx,[workspace_v0+ebx]
		mov		workspace_flip,esi

	; Work out Y extents of triangle
	;
	; Convert float to int using integer instructions, because FPU is in use doing division
	;
	; NB: this conversion only works if the number is greater than 0 and less than 1<<24
	;

		mov		ebp,[eax].comp_f[C_SY*4]
		mov		ecx,EXPONENT_OFFSET

		sub		ecx,ebp						; Offset exponent to get shift value
		and		ebp,MASK_MANTISSA			; Mask out mantissa

		shr		ecx,23						; Move shift value to low bits
		or		ebp,IMPLICIT_ONE			; Put the 1 back in top of mantissa

		shr		ebp,cl						; EBP = y_t

		mov		esi,[ebx].comp_f[C_SY*4]
		mov		ecx,EXPONENT_OFFSET

		sub		ecx,esi
		and		esi,MASK_MANTISSA

		shr		ecx,23
		or		esi,IMPLICIT_ONE

		shr		esi,cl						; ESI = y_m

		mov		edi,[edx].comp_f[C_SY*4]
		mov		ecx,EXPONENT_OFFSET

		sub		ecx,edi
		and		edi,MASK_MANTISSA

		shr		ecx,23
		or		edi,IMPLICIT_ONE

		shr		edi,cl						; EDI = y_b

	; Catch special cases of empty top or bottom trapezoids
	;
		cmp		ebp,esi
		je		top_zero

		cmp		esi,edi
		je		bottom_zero

	; Parameter gradient startup and Y deltas for edge gradients
	;
											;	0		1		2		3		4		5		6		7
		fmul	st(1),st					;	1/2area	dy1*a	dy2		dx1		dx2

		fld		[ebx].comp_f[C_SY*4]		;	sy2		1/2area	dy1*a	dy2		dx1		dx2
		fsub	[eax].comp_f[C_SY*4]		;	dsy1	1/2area	dy1*a	dy2		dx1		dx2
		 fxch	st(3)						;	dy2 	1/2area	dy1*a	dsy1	dx1		dx2

		fmul	st,st(1)					;	dy2*a	1/2area	dy1*a	dsy1	dx1		dx2

		fld		[edx].comp_f[C_SY*4]		;	sy3		dy2*a	1/2area	dy1*a	dsy1	dx1		dx2
		fsub	[ebx].comp_f[C_SY*4]		;	dsy2	dy2*a	1/2area	dy1*a	dsy1	dx1		dx2
		 fxch	st(5)						;	dx1		dy2*a	1/2area	dy1*a	dsy1	dsy2	dx2

count_cont:
		fmul	st,st(2)					;	dx1*a	dy2*a	1/2area	dy1*a	dsy1	dsy2	dx2

		fld		[edx].comp_f[C_SY*4]		;	sy3		dx1*a	dy2*a	1/2area	dy1*a	dsy1	dsy2	dx2
		fsub	[eax].comp_f[C_SY*4]		;	dsy3	dx1*a	dy2*a	1/2area	dy1*a	dsy1	dsy2	dx2
		 fxch	st(7)						;	dx2		dx1*a	dy2*a	1/2area	dy1*a	dsy1	dsy2	dsy3

		fmul	st,st(3)					;	dx2*a	dx1*a	dy2*a	1/2area	dy1*a	dsy1	dsy2	dsy3
		 fxch	st(3)						;	1/2area	dx1*a	dy2*a	dx2*a	dy1*a	dsy1	dsy2	dsy3

		fstp	workspace_iarea
		fstp	workspace_dx1_a
		fstp	workspace_dy2_a
		fstp	workspace_dx2_a
		fstp	workspace_dy1_a						;	dy1		dy2		dy3


	; Find edge gradients of triangle
	;
	; R = 1/(dy1.dy2.dy3)
	;
	; gradient_major = dy1.dx2.dy3.R
	; gradient_minor1 = dx1.dy2.dy3.R
	; gradient_minor2 = dy1.dy2.dx3.R
	;
											;	0		1		2		3		4		5		6		7
		fld		st(2)						;	dy3		dy1		dy2		dy3
		fmul	st,st(2)					;	dy2*dy3	dy1		dy2		dy3

		fld		[ebx].comp_f[C_SX*4]		;	x2		dy2*dy3	dy1		dy2		dy3
		fsub	[eax].comp_f[C_SX*4]		;	dx1		dy2*dy3	dy1		dy2		dy3

		fld		st(1)						;	dy2*dy3 dx1		dy2*dy3	dy1		dy2		dy3
		fmul	st,st(3)					;	dy123	dx1		dy2*dy3	dy1		dy2		dy3

		fld		[edx].comp_f[C_SX*4]		;	x3		dy123	dx1		dy2*dy3	dy1		dy2		dy3
		fsub	[ebx].comp_f[C_SX*4]		;	dx2		dy123	dx1		dy2*dy3	dy1		dy2		dy3
		 fxch	st(2)						;	dx1		dy123	dx2		dy2*dy3	dy1		dy2		dy3
		fld		fp_one						;	1.0		dx1		dy123	dx2		dy2*dy3	dy1		dy2		dy3
		fdivrp	st(2),st					;	dx1		R		dx2		dy2*dy3	dy1		dy2		dy3

	; Generate counts
	;
		inc		ebp
		mov		ecx,esi

		sub		ecx,ebp						; count_t = (y_m-y_t)-1
		mov		work_main_y,ebp				; save for X intercept calcs

		mov		work_top_count,ecx
		inc		esi

		sub		edi,esi						; count_b = (y_b-y_m)-1
		mov		workspace_m_y,esi			; save for X intercept calcs

		mov		work_bot_count,edi
		mov		esi,workspace_flip

	; Generate LR/RL flag into esi (used to index conversion numbers below)
	; Write out sorted vertices
	;
		mov		edi,workspace_iarea
		mov		top_vertex,eax

		xor		esi,edi						; Build LR flag in bit 31
		mov		mid_vertex,ebx

		shr		esi,31						; move down to bit 0
		mov		bot_vertex,edx

		mov		work.tsl.direction,esi

	;XXX Setup screen pointers and strides
	;

	;XXX Work out which scan conversion function to use
	;

	; Finish of gradient calculations, interleaved with working out t_dy, and m_dy, the fractions
	; that the top and middle vertices are from the integer scanline boundaries
	;
	; t_dy = (yt+1) - vt->y
	; m_dy = (ym+1) - vm->y
	;
											;	0		1		2		3		4		5		6		7
		fmulp	st(3),st					;	R		dx2		XYY		dy1		dy2		dy3

		fld		[edx].comp_f[C_SX*4]		;	x3		R		dx2		XYY		dy1		dy2		dy3
		 fxch	st(3)						;	XYY		R		dx2		x3		dy1		dy2		dy3

		fmul	st,st(1)					;	XYY*R	R		dx2		x3		dy1		dy2		dy3
		 fxch	st(3)						;	x3		R		dx2		XYY*R	dy1		dy2		dy3

		fsub	[eax].comp_f[C_SX*4]		;	dx3		R		dx2		XYY*R	dy1		dy2		dy3
		 fxch	st(1)						;	R		dx3		dx2		XYY*R	dy1		dy2		dy3

		fmulp	st(4),st					;	dx3		dx2		XYY*R	dy1*R	dy2		dy3
		 fxch	st(2)						;	XYY*R	dx2		dx3		dy1*R	dy2		dy3
											
		fild	workspace_m_y				;	m_y		XYY*R	dx2		dx3		dy1*R	dy2		dy3
		 fxch	st(2)						;	dx2		XYY*R	m_y		dx3		dy1*R	dy2		dy3
											
		fmulp	st(6),st					;	XYY*R	m_y		dx3		dy1*R	dy2		dx2*dy3
											
		fild	work_main_y					;	t_y		XYY*R	m_y		dx3		dy1*R	dy2		dx2*dy3
		 fxch	st(3)						;	dx3		XYY*R	m_y		t_y		dy1*R	dy2		dx2*dy3
											
		fmulp	st(5),st					;	XYY*R	m_y		t_y		dy1*R	dy2*dx3	dx2*dy3
		 fxch	st(1)						;	m_y		XYY*R	t_y		dy1*R	dy2*dx3	dx2*dy3

		fsub	[ebx].comp_f[C_SY*4]		;	m_dy	XYY*R	t_y		dy1*R	dy2*dx3	dx2*dy3
		 fxch	st(3)						;	dy1*R	XYY*R	t_y		m_dy	dy2*dx3	dx2*dy3
											
		fmul	st(4),st					;	dy1*R	XYY*R	t_y		m_dy	YYX*R	dx2*dy3
		 fxch	st(2)						;	t_y		XYY*R	dy1*R	m_dy	YYX*R	dx2*dy3
											
		fsub	[eax].comp_f[C_SY*4]		;	t_dy	XYY*R	dy1*R	m_dy	YYX*R	dx2*dy3
		 fxch	st(2)						;	dy1*R	XYY*R	t_dy	m_dy	YYX*R	dx2*dy3
											
		fmulp	st(5),st					;	XYY*R	t_dy	m_dy	YYX*R	YXY*R
		 fxch	st(2)						;	m_dy	t_dy	XYY*R	YYX*R	YXY*R
											;	m_dy	t_dy	g1		gm		g2
													
	; Work out initial X intercepts with top and middle scanlines
	;
	; x_major	= gm * t_dy + vt->x
	; x_minor1 = g1 * t_dy + vt->x
	; x_minor2 = g2 * m_dy + vm->x
	;
											;	0		1		2		3		4		5		6		7
		fld		st(1)						;	t_dy	m_dy	t_dy	g1		gm		g2
		fxch	st(1)						;	m_dy	t_dy	t_dy	g1		gm		g2
											
											
		fmul	st,st(5)					;	m_dy*g2	t_dy	t_dy	g1		gm		g2
		fxch	st(2)						;	t_dy	t_dy	m_dy*g2	g1		gm		g2
											
		fst		workspace_t_dy			
											
		fmul	st,st(3)					;	t_dy*g1	t_dy	m_dy*g2	g1		gm		g2
		 fxch	st(2)						;	m_dy*g2	t_dy	t_dy*g1	g1		gm		g2

		fadd	[ebx].comp_f[C_SX*4]		;	x_2		t_dy	t_dy*g1	g1		gm		g2
		 fxch	st(1)						;	t_dy	x_2		t_dy*g1	g1		gm		g2
											
		fmul	st,st(4)					;	t_dy*gm	x_2		t_dy*g1	g1		gm		g2
		 fxch	st(2)						;	t_dy*g1	x_2		t_dy*gm	g1		gm		g2
											
		fadd	[eax].comp_f[C_SX*4]		;	x_1		x_2		t_dy*gm	g1		gm		g2
		 fxch	st(3)						;	g1		x_2		t_dy*gm	x_1		gm		g2
											
		fadd	fp_conv_d16					;	g1+C	x_2		t_dy*gm	x_1		gm		g2
		 fxch	st(2)						;	t_dy*gm	x_2		g1+C	x_1		gm		g2
											
		fadd	[eax].comp_f[C_SX*4]		;	x_m		x_2		g1+C	x_1		gm		g2
		 fxch	st(4)						;	gm		x_2		g1+C	x_1		x_m		g2
											
		fadd	fp_conv_d16					;	gm+C	x_2		g1+C	x_1		x_m		g2
		 fxch	st(1)						;	x_2		gm+C	g1+C	x_1		x_m		g2
												
		fadd	fconv_d16_12[esi*8]			;	x_2+C	gm+C	g1+C	x_1		x_m		g2
		 fxch	st(5)						;	g2		gm+C	g1+C	x_1		x_m		x_2+C
												
		fadd	fp_conv_d16					;	g2+C	gm+C	g1+C	x_1		x_m		x_2+C
		 fxch	st(2)						;	g1+C	gm+C	g2+C	x_1		x_m		x_2+C
												
if USE_WORKSPACE
		fstp	real8 ptr work_top_i		;	gm+C	g2+C	x_1		x_m		x_2+C
		fstp	real8 ptr work_main_i		;	g2+C	x_1		x_m		x_2+C
		fstp	real8 ptr work_bot_i		;	x_1		x_m		x_2+C

		fadd	fconv_d16_12[esi*8]			;	x_1+C	x_m		x_2+C
		 fxch	st(1)						;	x_m		x_1+C	x_2+C

		fadd	fconv_d16_m[esi*8]			;	x_m+C	x_1+C	x_2+C

	; Load deltas back in registers
	;
		mov		edx,work_main_i				; read fixed d_xm
		mov		esi,work_top_i				; read fixed d_x1

		mov		edi,work_bot_i				; read fixed d_x2
		mov		ebx,workspace_v0 			; Start preparing for parameter setup

		fstp	real8 ptr work_main_i		;	x_1+C	x_2+C
		fstp	real8 ptr work_top_i		;	x_2+C
		fstp	real8 ptr work_bot_i		;	

		mov		work_main_d_i,edx
		mov		work_top_d_i,esi

		mov		work_bot_d_i,edi
		mov		ecx,work_main_i

		sar		ecx,16
		mov		edx,work_main_d_i
else
		fstp	real8 ptr work_top_d_i		;	gm+C	g2+C	x_1		x_m		x_2+C
		fstp	real8 ptr work_main_d_i		;	g2+C	x_1		x_m		x_2+C
		fstp	real8 ptr work_bot_d_i		;	x_1		x_m		x_2+C

		fadd	fconv_d16_12[esi*8]			;	x_1+C	x_m		x_2+C
		 fxch	st(1)						;	x_m		x_1+C	x_2+C

		fadd	fconv_d16_m[esi*8]			;	x_m+C	x_1+C	x_2+C
		 fxch	st(2)						;	x_2+C	x_1+C	x_m+C

		fstp	real8 ptr work_bot_i		;	x_1+C	x_m+C
		fstp	real8 ptr work_top_i		;	x_m+C
		fstp	real8 ptr work_main_i		;	

		mov		ecx,work_main_i
		mov		edx,work_main_d_i

		sar		ecx,16
		mov		ebx,workspace_v0 			; Start preparing for parameter setup
endif
	
		sar		edx,16						; get integer part of x delta down major edge
		mov		workspace_t_dx,ecx

		fild	workspace_t_dx				;	t_x

	; Generate floating point versions of x delta and x delta+4
	;
		mov		workspace_xstep_0,edx
		inc		edx

		mov		workspace_xstep_1,edx
		mov		edx,[workspace_v2] 			; Start preparing for parameter setup

											;	0		1		2		3		4		5		6		7
		fsub	[eax].comp_f[C_SX*4]		;	t_dx
		fild	workspace_xstep_0			;	xstep_0	t_dx
		fild	workspace_xstep_1			;	xstep_1 xstep_0	t_dx
		 fxch	st(2)						;	tdx		xstep_0	xstep_1
		fstp	workspace_t_dx				;	xstep_0	xstep_1

		mov		ecx,[workspace_v1] 			; Start preparing for parameter setup

		fstp	workspace_xstep_0			;	step_1
		fstp	workspace_xstep_1			;

		jmp		exit

	; Special cases for top or bottom counts == 0
	;
top_zero:
		cmp		ebp,edi						; Check for completely empty triangle
		je		empty_triangle
											;	0		1		2		3		4		5		6		7
		fmul	st(1),st					;	1/2area	dy1*a	dy2		dx1		dx2

		fld		fp_one						;	1.0		1/2area	dy1*a	dy2		dx1		dx2
		 fxch	st(3)						;	dy2 	1/2area	dy1*a	1.0		dx1		dx2

		fmul	st,st(1)					;	dy2*a	1/2area	dy1*a	1.0		dx1		dx2

		fld		[edx].comp_f[C_SY*4]		;	sy3		dy2*a	1/2area	dy1*a	1.0		dx1		dx2
		fsub	[ebx].comp_f[C_SY*4]		;	dsy2	dy2*a	1/2area	dy1*a	1.0		dx1		dx2
		 fxch	st(5)						;	dx1		dy2*a	1/2area	dy1*a	1.0		dsy2	dx2

		jmp		count_cont

bottom_zero:
											;	0		1		2		3		4		5		6		7
		fmul	st(1),st					;	1/2area	dy1*a	dy2		dx1		dx2

		fld		[ebx].comp_f[C_SY*4]		;	sy2		1/2area	dy1*a	dy2		dx1		dx2
		fsub	[eax].comp_f[C_SY*4]		;	dsy1	1/2area	dy1*a	dy2		dx1		dx2
		 fxch	st(3)						;	dy2		1/2area	dy1*a	dsy1	dx1		dx2

		fmul	st,st(1)					;	dy2*a	1/2area	dy1*a	dsy1	dx1		dx2

		fld		fp_one						;	1.0		dy2*a	1/2area	dy1*a	dsy1	dx1		dx2
		 fxch	st(5)						;	dx1		dy2*a	1/2area	dy1*a	dsy1	1.0		dx2

		jmp		count_cont

	; Fill in block with NULL count and exit
	;
empty_triangle:
		mov		work_top_count,-1
		mov		work_bot_count,-1
		fstp	st(0)
		fstp	st(0)
		fstp	st(0)
		fstp	st(0)
		fstp	st(0)
		empty
exit:
endm

; Do all the per-triangle work for a single float parameter
;
; Total 40 cycles
;
;	eax: ptr to top vertex
;	ebx: ptr to vertex0
;	ecx: ptr to vertex1
;	edx: ptr to vertex2
;
SETUP_FLOAT_PARAM	macro	comp,param,conv,unsigned:=<0>

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
		fmul	workspace_dy1_a				;	dp2*a	dp1		dp2
		fld		st(1)						;	dp1		dp2*a	dp1		dp2
		fmul	workspace_dy2_a				;	dp1*b	dp2*a	dp1		dp2
		fld		workspace_t_dx				;	fdx		dp1*b	dp2*a	dp1		dp2
		 fxch	st(4)						;	dp2		dp1*b	dp2*a	dp1		fdx
		fmul	workspace_dx1_a				;	dp2*c	dp1*b	dp2*a	dp1		fdx
		fld		workspace_t_dy				;	fdy		dp2*c	dp1*b	dp2*a	dp1		fdx
		 fxch	st(4)						;	dp1		dp2*c	dp1*b	dp2*a	fdy		fdx
		fmul	workspace_dx2_a				;	dp1*d	dp2*c	dp1*b	dp2*a	fdy		fdx
		 fxch	st(3)						;	dp2*a	dp2*c	dp1*b	dp1*d	fdy		fdx
		fsubp	st(2),st					;	dp2*c	d1b-d2a	dp1*d	fdy		fdx
		fld		[eax].comp_f[comp*4]		;	param_t	dp2*c	d1b-d2a	dp1*d	fdy		fdx
		 fxch	st(3)						;	dp1*d	dp2*c	d1b-d2a	param_t	fdy		fdx
		fsubp	st(1),st					;	d2c-d1d	d1b-d2a	param_t	fdy		fdx
											;	pdy		pdx		param_t	fdy		fdx

	; Build the inputs to the rasteriser
	;
	; pdy_0 = pdy + xstep_0 * pdx
	; pdy_1 = pdy + xstep_1 * pdx
	; pstart = param_t + pdx * fdx + pdy * fdy
	;
	; (A couple of the fixed points conversions are interleaved into this block)
	; 12 cycles
	;
											;	0		1		2		3		4		5		6		7
		fld		st(1)						;	pdx		pdy		pdx		param_t	fdy		fdx
		fmul	workspace_xstep_0			;	pdx*xs0	pdy		pdx		param_t	fdy		fdx
		fld		st(2)						;	pdx		pdx*xs0	pdy		pdx		param_t	fdy		fdx
		fmul	workspace_xstep_1			;	pdx*xs1	pdx*xs0	pdy		pdx		param_t	fdy		fdx
		 fxch	st(1)						;	pdx*xs0	pdx*xs1	pdy		pdx		param_t	fdy		fdx
		fadd	st,st(2)					;	pdy_0	pdx*xs1	pdy		pdx		param_t	fdy		fdx
		 fxch	st(3)						;	pdx		pdx*xs1	pdy		pdy_0	param_t	fdy		fdx

		fmul	st(6),st					;	pdx		pdx*xs1	pdy		pdy_0	param_t	fdy		fdx*pdx
		 fxch	st(2)						;	pdy		pdx*xs1	pdx		pdy_0	param_t	fdy		fdx*pdx
		fadd	st(1),st					;	pdy		pdy_1	pdx		pdy_0	param_t	fdy		fdx*pdx

		fmulp	st(5),st					;	pdy_1	pdx		pdy_0	param_t	fdy*pdy	fdx*pdx
		 fxch	st(3)						;	param_t	pdx		pdy_0	pdy_1	fdy*pdy	fdx*pdx
		faddp	st(5),st					;	pdx		pdy_0	pdy_1	fdy*pdy	fpx+pt
		 fxch	st(1)						;	pdy_0	pdx		pdy_1	fdy*pdy	fpx+pt
		fadd	conv						;	C+pdy_0	pdx		pdy_1	fdy*pdy	fpx+pt
		 fxch	st(2)						;	pdy_1	pdx		C+pdy_0	fdy*pdy	fpx+pt
		fadd	conv						;	C+pdy_1	pdx		C+pdy_0	fdy*pdy	fpx+pt
		 fxch	st(3)						;	fdy*pdy	pdx		C+pdy_0	C+pdy_1	fpx+pt
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
		fadd	conv						;	C+pdx	C+pdy_0	C+pdy_1	pstart
		 fxch	st(3)						;	pstart	C+pdy_0	C+pdy_1	C+pdx

; 1 clock delay

		fadd	conv						;	C+pstrt	C+pdy_0	C+pdy_1	C+pdx
		 fxch	st(2)						;	C+pdy_1	C+pdy_0	C+pstrt	C+pdx

if USE_WORKSPACE
		fstp	real8 ptr work_&param&_current
		fstp	real8 ptr work_&param&_grad_x

		mov		esi,dword ptr work_&param&_current
		mov		edi,dword ptr work_&param&_grad_x

		fstp	real8 ptr work_&param&_current
		fstp	real8 ptr work_&param&_grad_x

		mov		dword ptr work_&param&_d_carry,esi
		mov		dword ptr work_&param&_d_nocarry,edi

else

		fstp	real8 ptr work_&param&_d_carry
		fstp	real8 ptr work_&param&_grad_x
		fstp	real8 ptr work_&param&_currentpix

		mov		esi,dword ptr work_&param&_grad_x
		mov		edi,dword ptr work_&param&_currentpix

		fstp	real8 ptr work_&param&_grad_x	;

		mov		dword ptr work_&param&_d_nocarry,esi
		mov		dword ptr work_&param&_current,edi

endif
if unsigned
	; Remap from -1 to 1 signed to 0 to 1 unsigned
	;
			mov		esi,dword ptr work_&param&_current
			xor		esi,080000000h
			mov		dword ptr work_&param&_current,esi
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
SETUP_FLOAT_CONST	macro	comp,param,conv,unsigned:=<0>

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
			
			mov		dword ptr work_&param&_grad_x,esi
			mov		dword ptr work_&param&_d_nocarry,esi

			fstp	real8 ptr work_&param&_current

			mov		dword ptr work_&param&_d_carry,esi

if unsigned
	; Remap from -1 to 1 signed to 0 to 1 unsigned
	;
			mov		esi,dword ptr s_p
			xor		esi,080000000h
			mov		dword ptr s_p,esi
endif

	endm


; Determine if this triangle will look sufficiently good with linear interpolation
;
; Total 40-44 cycles
;
;	eax: ptr to top vertex
;	ebx: ptr to vertex0
;	ecx: ptr to vertex1
;	edx: ptr to vertex2
;
SETUP_FLOAT_CHECK_PERSPECTIVE_CHEAT macro cheat

		assume esi: ptr brp_vertex, edi: ptr brp_vertex, ebp: ptr brp_vertex

	; Sort the vertices in SX order (relies on all positive values)
	;
	; 7 cycles
	;
		mov		esi,top_vertex
		mov		edi,mid_vertex

		mov		ebp,bot_vertex
		xor		ebx,ebx

		mov		eax,[esi].comp_f[C_SX*4]
		mov		ecx,[edi].comp_f[C_SX*4]

		mov		edx,[ebp].comp_f[C_SX*4]
		cmp		ecx,eax

		rcl		ebx,1
		cmp		edx,eax

		rcl		ebx,1
		cmp		edx,ecx

		rcl		ebx,1			; ebx now has 3 bit number characterising the order of the vertices.
		xor		ecx,ecx

	; Determine the range of SX and SY values covered by the three vertices
	;
	; 6 cycles
											;	0		1		2		3		4		5		6		7
		fld		[ebp].comp_f[C_SY*4]		;	yb

		mov		eax,sort_table_0[ebx*4]
		mov		edx,sort_table_2[ebx*4]

		fsub	[esi].comp_f[C_SY*4]		;	yrange

		mov		eax,[top_vertex+eax]
		mov		edx,[top_vertex+edx]

		fld		[edx].comp_f[C_SX*4]		;	xr		yrange
		fsub	[eax].comp_f[C_SX*4]		;	xrange	yrange

	; Sort the vertices in W order (relies on all positive values)
	;
	; 10 cycles
	;
		mov		eax,[esi].comp_f[C_W*4]
		mov		ebx,[edi].comp_f[C_W*4]

		mov		edx,[ebp].comp_f[C_W*4]
		cmp		ebx,eax

		fld		st(0)						;	xrange	xrange	yrange
		fsub	st(0),st(2)					;	xr-yr	xrange	yrange

		rcl		ecx,1
		cmp		edx,eax

		rcl		ecx,1
		cmp		edx,ebx

		fstp	xr_yr

		rcl		ecx,1			; ebx now has 3 bit number characterising the order of the vertices.
		mov		ebx,xr_yr

		mov		eax,sort_table_0[ecx*4]
		mov		edx,sort_table_2[ecx*4]

	; Select the larger of the SX and SY ranges by checking the sign of xrange-yrange
	;
	; 4-5 cycles + 0-3 cycles branch misprediction
	;
		test	ebx,080000000h
		je		xrange_larger

		fxch	st(1)						;	yrange	xrange
xrange_larger:
		fstp	st(1)						;	srange

		mov		eax,[top_vertex+eax]
		mov		edx,[top_vertex+edx]

	; Determine the minimum and range of W values covered by the three vertices
	;
	; 3 cycles
	;
		fld		[edx].comp_f[C_W*4]			;	wmax	srange
		fld		[eax].comp_f[C_W*4]			;	wmin	wmax	srange
		fsub	st(1),st(0)					;	wmin	wrange	srange
		 fxch	st(2)						;	srange	wrange	wmin

	; Multiply the ranges together and the minimum W by the cutoff factor (4.0)
	;
	; 3 cycles
	;
		fmul								;	wr*sr	wmin
		 fxch	st(1)						;	wmin	wr*sr
		fmul	fp_four						;	wmin*4	wr*sr
		 fxch	st(1)						;	wr*sr	wmin*4

		mov		eax,top_vertex	; restore pointers
		mov		ebx,workspace_v0

	; Store them out
	;
	; 5 cycles
	;
		fstp	wr_sr
		fstp	wmin_4

		mov		ecx,wr_sr
		mov		edx,wmin_4

	; Cheat if w_range * srange < w_min * cutoff
	;
	; 2 cycles
	;
		cmp		ecx,edx
		mov		ecx,workspace_v1

		mov		edx,workspace_v2
		jb		cheat

		endm


; Do the per-parameter calculations for perspective correct texture mapping
;
; Total 210-235 cycles
;
; Total setup is therefore:
;
;  Unlit  z-sorted   perspective: 423-453		cheat: 293-297		linear: 253 (172 more)
;  Smooth z-buffered perspective: 490-533		cheat: 373-377		linear: 333
;
; Proper perspective setup is 172 cycles more than for linear
; Cheat cases are 40 more than for linear
;
;	esi: ptr to top vertex
;	edi: ptr to middle vertex
;	ebp: ptr to bottom vertex
;
SETUP_FLOAT_UV_PERSPECTIVE macro

		assume esi: ptr brp_vertex, edi: ptr brp_vertex, ebp: ptr brp_vertex

	; N.B. in this code, u0, v0, w0 etc. refer to the top vertex, u1 etc. to the middle and
	; u2 etc. to the bottom vertex.
	;
	; We use the sorted vertices because we need some intermediate results for the top vertex.
	; The gradients must be recalculated, and the sign of the area calculation may have been
	; changed by the sorting process.
	;
	; N.B. It may be possible to save a few cycles by doing the gradient calculations using the
	; unsorted vertices and the calculations for the start values and some of the terms in the
	; maxuv calculations using the top vertex, but this is awkward to arrange.
	;
	; 22-23 cycles + 0-3 cycles branch misprediction
											;	0		1		2		3		4		5		6		7
		fld		workspace_iarea				;	1/2area

		cmp		workspace_flip,0
		je		no_flip

		fchs								;	-1/2area
no_flip:

		fld		[edi].comp_f[C_SX*4]		;	x1		1/2area
		fsub	[esi].comp_f[C_SX*4]		;	dx1		1/2area
		fld		[ebp].comp_f[C_SX*4]		;	x2		dx1		1/2area
		fsub	[esi].comp_f[C_SX*4]		;	dx2		dx1		1/2area
		 fxch	st(1)						;	dx1		dx2		1/2area
		fmul	st(0),st(2)					;	dx1*a	dx2		1/2area
		fld		[edi].comp_f[C_SY*4]		;	y1		dx1*a	dx2		1/2area
		 fxch	st(2)						;	dx2		dx1*a	y1		1/2area
		fmul	st(0),st(3)					;	dx2*a	dx1*a	y1		1/2area
		 fxch	st(2)						;	y1		dx1*a	dx2*a	1/2area
		fsub	[esi].comp_f[C_SY*4]		;	dy1		dx1*a	dx2*a	1/2area
		fld		[ebp].comp_f[C_SY*4]		;	y2		dy1		dx1*a	dx2*a	1/2area
		fsub	[esi].comp_f[C_SY*4]		;	dy2		dy1		dx1*a	dx2*a	1/2area
		 fxch	st(1)						;	dy1		dy2		dx1*a	dx2*a	1/2area
		fmul	st(0),st(4)					;	dy1*a	dy2		dx1*a	dx2*a	1/2area
		 fxch	st(2)						;	dx1*a	dy2		dy1*a	dx2*a	1/2area
		fstp	workspace_dx1_a				;	dy2		dy1*a	dx2*a	1/2area
		fmulp	st(3),st(0)					;	dy1*a	dx2*a	dy2*a
		fstp	workspace_dy1_a				;	dx2*a	dy2*a
		fstp	workspace_dx2_a				;	dy2*a
		fstp	workspace_dy2_a				;

	; Calculate integral base texture coords and write out as integers.
	;
if 1
	; Round by storing as integer and reading back
	;
	; 17 cycles and a latency of two before vb is available
		
		fld		[esi].comp_f[C_U*4]			;	u0
		fistp	work.awsl.u_current			;	
		fild	work.awsl.u_current			;	ub
		fld		[esi].comp_f[C_V*4]			;	v0		ub
		fistp	work.awsl.v_current			;	ub
		fild	work.awsl.v_current			;	vb		ub
		 fxch	st(1)						;	ub		vb

		fld		[edi].comp_f[C_W*4]			;	w1		ub		vb
else
	; Round by adding and subtracting a magic number
	;
	; 21 cycles including one whole cycles spare (used for loading w1) and three v slots

	; Determine current internal precision and select the correct magic number.
	;
		fnstcw	word ptr fpu_cw

		mov		eax,fpu_cw
; V
		and		eax,FPU_CW_PC_MASK
; V
		shr		eax,FPU_CW_PC_SHIFT
; V
											;	0		1		2		3		4		5		6		7
		fld		[esi].comp_f[C_U*4]			;	u0
		fld		fp_round[eax*4]				;	C		u0
		 fxch	st(1)						;	u0		C
		fadd	st(0),st(1)					;	u0+C	C
		fld		[esi].comp_f[C_V*4]			;	v0		u0+C	C
		fadd	st(0),st(2)					;	v0+C	u0+C	C
		 fxch	st(1)						;	u0+C	v0+C	C
		fsub	st(0),st(2)					;	ub		v0+C	C
		 fxch	st(1)						;	v0+C	ub		C
		fld		[edi].comp_f[C_W*4]			;	w1		v0+C	ub		C
		 fxch	st(1)						;	v0+C	w1		ub		C
		fsubrp	st(3),st(0)					;	w1		ub		vb

	; Write out the base values for later *** SHOULD GO INTO WORKSPACE! ***
	;
		fld		st(1)						;	ub		w0		ub		vb
		fadd	fp_conv_d					;	ub+C	w0		ub		vb
		fld		st(3)						;	vb		ub+C	w0		ub		vb
		fadd	fp_conv_d					;	vb+C	ub+C	w0		ub		vb
		 fxch	st(1)						;	ub+C	vb+C	w0		ub		vb
		fstp	work.awsl.u_current			;	vb+C	w0		ub		vb
		fstp	work.awsl.v_current			;	w0		ub		vb
endif
		
	; Calculate u' and v' (deltas from base coords) and q' (= q * product(w)) for vertex 0
	;
	; 5 cycles
											;	0		1		2		3		4		5		6		7
		fmul	[ebp].comp_f[C_W*4]			;	q'0		ub		vb
		fld		[esi].comp_f[C_U*4]			;	u0		q'0		ub		vb
		fsub	st(0),st(2)					;	u'0		q'0		ub		vb
		fld		[esi].comp_f[C_V*4]			;	v0		u'0		q'0		ub		vb
		fsub	st(0),st(4)					;	v'0		u'0		q'0		ub		vb

	; Calculate |u'0| + |v'0| + |q'0|
	;
	; 10 cycles

		fld		st(2)						;	q'0		v'0		u'0		q'0		ub		vb
		fst		q0							;	q'0		v'0		u'0		q'0		ub		vb
		fabs								;	|q'0|	v'0		u'0		q'0		ub		vb
		fld		st(2)						;	u'0		|q'0|	v'0		u'0		q'0		ub		vb
		fabs								;	|u'0|	|q'0|	v'0		u'0		q'0		ub		vb
		fadd								;	maxuv	v'0		u'0		q'0		ub		vb
		fld		st(1)						;	v'0		maxuv	v'0		u'0		q'0		ub		vb
		fabs								;	|v'0|	maxuv	v'0		u'0		q'0		ub		vb
		fadd								;	maxuv	v'0		u'0		q'0		ub		vb

	; Multiply u'0 and v'0 by q'0
	;
	; 5 cycles

		 fxch	st(2)						;	u'0		v'0		maxuv	q'0		ub		vb
		fmul	st(0),st(3)					;	u'q'0	v'0		maxuv	q'0		ub		vb
		fld		[ebp].comp_f[C_W*4]			;	w2		u'q'0	v'0		maxuv	q'0		ub		vb
		 fxch	st(4)						;	q'0		u'q'0	v'0		maxuv	w2		ub		vb
		fmulp	st(2),st(0)					;	u'q'0	v'q'0	maxuv	w2		ub		vb

		 fxch	st(2)						;	maxuv	v'q'0	u'q'0	w2		ub		vb
		fstp	maxuv						;	v'q'0	u'q'0	w2		ub		vb

	; Calculate u' and v' (deltas from base coords) and q' (= q * product(w)) for vertex 1
	;
	; 5 cycles
											;	0		1		2		3		4		5		6		7
		fld		[edi].comp_f[C_U*4]			;	u1		v'q'0	u'q'0	w2		ub		vb
		 fxch	st(3)						;	w2		v'q'0	u'q'0	u1		ub		vb
		fmul	[esi].comp_f[C_W*4]			;	q'1		v'q'0	u'q'0	u1		ub		vb
		 fxch	st(3)						;	u1		v'q'0	u'q'0	q'1		ub		vb
		fsub	st(0),st(4)					;	u'1		v'q'0	u'q'0	q'1		ub		vb
		fld		[edi].comp_f[C_V*4]			;	v1		u'1		v'q'0	u'q'0	q'1		ub		vb
		fsub	st(0),st(6)					;	v'1		u'1		v'q'0	u'q'0	q'1		ub		vb
		 
	; Multiply u'1 and v'1 by q'1
	;
	; 5 cycles

		 fxch	st(1)						;	u'1		v'1		v'q'0	u'q'0	q'1		ub		vb
		fmul	st(0),st(4)					;	u'q'1	v'1		v'q'0	u'q'0	q'1		ub		vb
		 fxch	st(4)						;	q'1		v'1		v'q'0	u'q'0	u'q'1	ub		vb
		fld		[esi].comp_f[C_W*4]			;	w0		q'1		v'1		v'q'0	u'q'0	u'q'1	ub		vb
		 fxch	st(1)						;	q'1		w0		v'1		v'q'0	u'q'0	u'q'1	ub		vb
		fmul	st(2),st(0)					;	q'1		w0		v'q'1	v'q'0	u'q'0	u'q'1	ub		vb
		fstp	q1							;	w0		v'q'1	v'q'0	u'q'0	u'q'1	ub		vb

	; Calculate u' and v' (deltas from base coords) and q' (= q * product(w)) for vertex 2
	;
	; 4 cycles
											;	0		1		2		3		4		5		6		7
		fmul	[edi].comp_f[C_W*4]			;	q'2		v'q'1	v'q'0	u'q'0	u'q'1	ub		vb
		 fxch	st(5)						;	ub		v'q'1	v'q'0	u'q'0	u'q'1	q'2		vb
		fsubr	[ebp].comp_f[C_U*4]			;	u'2		v'q'1	v'q'0	u'q'0	u'q'1	q'2		vb
		 fxch	st(6)						;	vb		v'q'1	v'q'0	u'q'0	u'q'1	q'2		u'2
		fsubr	[ebp].comp_f[C_V*4]			;	v'2		v'q'1	v'q'0	u'q'0	u'q'1	q'2		u'2

		fld		maxuv						;	maxuv	v'2		v'q'1	v'q'0	u'q'0	u'q'1	q'2		u'2
		 fxch	st(6)						;	q'2		v'2		v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'2

	; Multiply u'2 and v'2 by q'2
	;
	; 4 cycles

		fmul	st(7),st(0)					;	q'2		v'2		v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fst		q2							;	q'2		v'2		v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fmulp	st(1),st(0)					;	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2

	; Calculate |u'0| + |v'0| + |q'0| + sum(|u'q'| + |v'q'|) (vertex 0 is the top vertex)
	;
	; 18 cycles
											;	0		1		2		3		4		5		6		7
		fld		st(3)						;	u'q'0	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fabs								;	|u'q'0|	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		faddp	st(6),st(0)					;	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fld		st(2)						;	v'q'0	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fabs								;	|v'q'0|	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		faddp	st(6),st(0)					;	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fld		st(4)						;	u'q'1	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fabs								;	|u'q'1|	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		faddp	st(6),st(0)					;	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fld		st(1)						;	v'q'1	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fabs								;	|v'q'1|	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		faddp	st(6),st(0)					;	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fld		st(6)						;	u'q'2	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fabs								;	|u'q'2|	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		faddp	st(6),st(0)					;	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fld		st(0)						;	v'q'2	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fabs								;	|v'q'2|	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		faddp	st(6),st(0)					;	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2

	; Calculate deltas for u'q' and v'q'
	;
	; 6 cycles
											;	0		1		2		3		4		5		6		7
		 fxch	st(2)						;	v'q'0	v'q'1	v'q'2	u'q'0	u'q'1	maxuv	u'q'2
		fsub	st(1),st(0)					;	v'q'0	dv1		v'q'2	u'q'0	u'q'1	maxuv	u'q'2
		fsub	st(2),st(0)					;	v'q'0	dv1		dv2		u'q'0	u'q'1	maxuv	u'q'2
		 fxch	st(3)						;	u'q'0	dv1		dv2		v'q'0	u'q'1	maxuv	u'q'2
		fsub	st(4),st(0)					;	u'q'0	dv1		dv2		v'q'0	du1		maxuv	u'q'2
		fsub	st(6),st(0)					;	u'q'0	dv1		dv2		v'q'0	du1		maxuv	du2

		 fxch	st(5)						;	maxuv	dv1		dv2		v'q'0	du1		u'q'0	du2
		fstp	maxuv						;	dv1		dv2		v'q'0	du1		u'q'0	du2

	; Calculate normalisation factor to ensure that numerator and denominator
	; values do not overflow at any point in the rasterisation.
	;
	; Dan's code now calculates 2^28/maxuv and multiplies everything by this.
	; However, it should be sufficient to use 2^(28-ceil(log2(maxuv))).
	;
	; In fact, I think the largest value that will ever appear in the
	; calculations is bounded by:
	;
	; max(|q'0|,|q'1|,|q'2|) + max(|u'0|,|u'1|,|u'2|,|v'0|,|v'1|,|v'2|) * max(dq/dx,dq/dy_carry))
	; 
	; 4-6 cycles + 0-3 cycles misprediction penalty
	;
		mov		eax,maxuv
;V
	; Round up to a power of two
	;
		test	eax,MASK_MANTISSA
		je		exact

		and		eax,not MASK_MANTISSA
;V
		add		eax,1 shl EXPONENT_SHIFT
;V
exact:

	; Multiply reciprocal by 2^28
	;
		neg		eax
;V
		add		eax,(EXPONENT_BIAS * 2 + 28) shl EXPONENT_SHIFT
;V
	; Do parameter calculations for u and v
	;
	; pdx = dp1 * dy2_a - dp2 * dy1_a
	; pdy = dp2 * dx1_a - dp1 * dx2_a
	;
	; pdy_0 = pdy + xstep_0 * pdx
	; pstart = param_t + pdx * fdx + pdy * fdy
	;
	; Final results are multiplied by the normalisation factor.  d_carry is
	; not calculated as the outer loop adds grad_x separately.
	;
	; 64 cycles
											;	0		1		2		3		4		5		6		7
		fld		st(3)						;	du1		dv1		dv2		v'q'0	du1		u'q'0	du2
		fmul	workspace_dy2_a				;	du1*b	dv1		dv2		v'q'0	du1		u'q'0	du2
		fld		st(6)						;	du2		du1*b	dv1		dv2		v'q'0	du1		u'q'0	du2
		fmul	workspace_dy1_a				;	du2*a	du1*b	dv1		dv2		v'q'0	du1		u'q'0	du2
		 fxch	st(5)						;	du1		du1*b	dv1		dv2		v'q'0	du2*a	u'q'0	du2

		mov		maxuv,eax

		fmul	workspace_dx2_a				;	du1*d	du1*b	dv1		dv2		v'q'0	du2*a	u'q'0	du2
		 fxch	st(1)						;	du1*b	du1*d	dv1		dv2		v'q'0	du2*a	u'q'0	du2
		fsubrp	st(5),st(0)					;	du1*d	dv1		dv2		v'q'0	udx		u'q'0	du2
		 fxch	st(6)						;	du2		dv1		dv2		v'q'0	udx		u'q'0	du1*d
		fmul	workspace_dx1_a				;	du2*c	dv1		dv2		v'q'0	udx		u'q'0	du1*d
		fld		workspace_t_dx				;	fdx		du2*c	dv1		dv2		v'q'0	udx		u'q'0	du1*d
		fmul	st(0),st(5)					;	fdx*udx	du2*c	dv1		dv2		v'q'0	udx		u'q'0	du1*d
		 fxch	st(7)						;	du1*d	du2*c	dv1		dv2		v'q'0	udx		u'q'0	fdx*udx
		fsubp	st(1),st(0)					;	udy		dv1		dv2		v'q'0	udx		u'q'0	fdx*udx
		fld		workspace_t_dy				;	fdy		udy		dv1		dv2		v'q'0	udx		u'q'0	fdx*udx
		 fxch	st(7)						;	fdx*udx	udy		dv1		dv2		v'q'0	udx		u'q'0	fdy
		faddp	st(6),st(0)					;	udy		dv1		dv2		v'q'0	udx		fux+ut	fdy
		fmul	st(6),st(0)					;	udy		dv1		dv2		v'q'0	udx		fux+ut	fdy*udy
		fld		st(4)						;	udx		udy		dv1		dv2		v'q'0	udx		fux+ut	fdy*udy
		 fxch	st(7)						;	fdy*udy	udy		dv1		dv2		v'q'0	udx		fux+ut	udx
		faddp	st(6),st(0)					;	udy		dv1		dv2		v'q'0	udx		ustart	udx
		 fxch	st(6)						;	udx		dv1		dv2		v'q'0	udx		ustart	udy
		fmul	workspace_xstep_0			;	udx*xs0	dv1		dv2		v'q'0	udx		ustart	udy
		fld		st(1)						;	dv1		udx*xs0	dv1		dv2		v'q'0	udx		ustart	udy
		 fxch	st(6)						;	ustart	udx*xs0	dv1		dv2		v'q'0	udx		dv1		udy
		fmul	maxuv						;	ustart'	udx*xs0	dv1		dv2		v'q'0	udx		dv1		udy
		 fxch	st(1)						;	udx*xs0	ustart'	dv1		dv2		v'q'0	udx		dv1		udy
		faddp	st(7),st(0)					;	ustart'	dv1		dv2		v'q'0	udx		dv1		udy_0
		 fxch	st(4)						;	udx		dv1		dv2		v'q'0	ustart'	dv1		udy_0
		fmul	maxuv						;	udx'	dv1		dv2		v'q'0	ustart'	dv1		udy_0
		 fxch	st(4)						;	ustart'	dv1		dv2		v'q'0	udx'	dv1		udy_0
		fadd	fp_conv_d					;	ustrt+C	dv1		dv2		v'q'0	udx'	dv1		udy_0
		 fxch	st(6)						;	udy_0	dv1		dv2		v'q'0	udx'	dv1		ustrt+C
		fmul	maxuv						;	udy_0'	dv1		dv2		v'q'0	udx'	dv1		ustrt+C
		 fxch	st(4)						;	udx'	dv1		dv2		v'q'0	udy_0'	dv1		ustrt+C
		fadd	fp_conv_d					;	udx+C	dv1		dv2		v'q'0	udy_0'	dv1		ustrt+C
		 fxch	st(6)						;	ustrt+C	dv1		dv2		v'q'0	udy_0'	dv1		udx+C
		fstp	real8 ptr work.pu.currentpix ;	dv1		dv2		v'q'0	udy_0'	dv1		udx+C
		fld		st(1)						;	dv2		dv1		dv2		v'q'0	udy_0'	dv1		udx+C
		 fxch	st(4)						;	udy_0'	dv1		dv2		v'q'0	dv2		dv1		udx+C
		fadd	fp_conv_d					;	udy_0+C	dv1		dv2		v'q'0	dv2		dv1		udx+C
		 fxch	st(6)						;	udx+C	dv1		dv2		v'q'0	dv2		dv1		udy_0+C
		fstp	real8 ptr work.pu.grad_x	;	dv1		dv2		v'q'0	dv2		dv1		udy_0+C
		fmul	workspace_dy2_a				;	dv1*b	dv2		v'q'0	dv2		dv1		udy_0+C
		 fxch	st(5)						;	udy_0+C	dv2		v'q'0	dv2		dv1		dv1*b
		fstp	real8 ptr work.pu.d_carry	;	dv2		v'q'0	dv2		dv1		dv1*b

		fmul	workspace_dy1_a				;	dv2*a	v'q'0	dv2		dv1		dv1*b
		 fxch	st(3)						;	dv1		v'q'0	dv2		dv2*a	dv1*b

		mov		eax,work.pu.currentpix
		mov		ebx,work.pu.d_carry

		fmul	workspace_dx2_a				;	dv1*d	v'q'0	dv2		dv2*a	dv1*b
		 fxch	st(2)						;	dv2		v'q'0	dv1*d	dv2*a	dv1*b

		mov		work.pu.current,eax
		mov		work.pu.d_nocarry,ebx

		fmul	workspace_dx1_a				;	dv2*c	v'q'0	dv1*d	dv2*a	dv1*b
		 fxch	st(4)						;	dv1*b	v'q'0	dv1*d	dv2*a	dv2*c
		fsubrp	st(3),st(0)					;	v'q'0	dv1*d	vdx		dv2*c
		fld		workspace_t_dx				;	fdx		v'q'0	dv1*d	vdx		dv2*c
		 fxch	st(2)						;	dv1*d	v'q'0	fdx		vdx		dv2*c
		fsubp	st(4),st(0)					;	v'q'0	fdx		vdx		vdy
		 fxch	st(1)						;	fdx		v'q'0	vdx		vdy
		fmul	st(0),st(2)					;	fdx*vdx	v'q'0	vdx		vdy
		fld		workspace_t_dy				;	fdy		fdx*vdx	v'q'0	vdx		vdy
		fmul	st(0),st(4)					;	fdy*vdy	fdx*vdx	v'q'0	vdx		vdy
		 fxch	st(1)						;	fdx*vdx	fdy*vdy	v'q'0	vdx		vdy
		faddp	st(2),st(0)					;	fdy*vdy	fvx+ft	vdx		vdy
		fld		st(2)						;	vdx		fdy*vdy	fvx+ft	vdx		vdy
		 fxch	st(1)						;	fdy*vdy	vdx		fvx+ft	vdx		vdy
		faddp	st(2),st(0)					;	vdx		vstart	vdx		vdy
		fmul	workspace_xstep_0			;	vdx*xs0	vstart	vdx		vdy
		 fxch	st(2)						;	vdx		vstart	vdx*xs0	vdy
;1 cycle
		fmul	maxuv						;	vdx'	vstart	vdx*xs0	vdy
		 fxch	st(2)						;	vdx*xs0	vstart	vdx'	vdy
		faddp	st(3),st(0)					;	vstart	vdx'	vdy_0
		fmul	maxuv						;	vstart'	vdx'	vdy_0
		 fxch	st(1)						;	vdx'	vstart'	vdy_0
		fadd	fp_conv_d					;	vdx+C	vstart'	vdy_0
		 fxch	st(2)						;	vdy_0	vstart'	vdx+C
		fmul	maxuv						;	vdy_0'	vstart'	vdx+C
		 fxch	st(1)						;	vstart'	vdy_0'	vdx+C
		fadd	fp_conv_d					;	vstrt+C	vdy_0'	vdx+C
		 fxch	st(1)						;	vdy_0'	vstrt+C	vdx+C
		fadd	fp_conv_d					;	vdy_0+C	vstrt+C	vdx+C
		 fxch	st(2)						;	vdx+C	vstrt+C	vdy_0+C
		fstp	real8 ptr work.pv.grad_x	;	vstrt+C	vdy_0+C
		fstp	real8 ptr work.pv.currentpix ;	vdy_0+C
		fstp	real8 ptr work.pv.d_carry	;	

		mov		eax,work.pv.currentpix
		mov		ebx,work.pv.d_carry

		mov		work.pv.current,eax
		mov		work.pv.d_nocarry,ebx

	; Do parameter calculations for q'
	;
	; pdx = dp1 * dy2_a - dp2 * dy1_a
	; pdy = dp2 * dx1_a - dp1 * dx2_a
	;
	; pdy_0 = pdy + xstep_0 * pdx
	; pdy_1 = pdy + xstep_1 * pdx
	; pstart = param_t + pdx * fdx + pdy * fdy
	;
	; Final results are multiplied by the normalisation factor.
	;
	; 44 cycles
											;	0		1		2		3		4		5		6		7
		fld		q2							;	q2
		fsub	q0							;	dq2
		fld		q1							;	q1		dq2
		fsub	q0							;	dq1		dq2

		fld		st(1)						;	dq2		dq1		dq2
		fmul	workspace_dy1_a				;	dq2*a	dq1		dq2
		fld		st(1)						;	dq1		dq2*a	dq1		dq2
		fmul	workspace_dy2_a				;	dq1*b	dq2*a	dq1		dq2
		fld		workspace_t_dx				;	fdx		dq1*b	dq2*a	dq1		dq2
		 fxch	st(4)						;	dq2		dq1*b	dq2*a	dq1		fdx
		fmul	workspace_dx1_a				;	dq2*c	dq1*b	dq2*a	dq1		fdx
		fld		workspace_t_dy				;	fdy		dq2*c	dq1*b	dq2*a	dq1		fdx
		 fxch	st(4)						;	dq1		dq2*c	dq1*b	dq2*a	fdy		fdx
		fmul	workspace_dx2_a				;	dq1*d	dq2*c	dq1*b	dq2*a	fdy		fdx
		 fxch	st(3)						;	dq2*a	dq2*c	dq1*b	dq1*d	fdy		fdx
		fsubp	st(2),st					;	dq2*c	d1b-d2a	dq1*d	fdy		fdx
		fld		q0							;	q'0		dq2*c	d1b-d2a	dq1*d	fdy		fdx
		 fxch	st(3)						;	dq1*d	dq2*c	d1b-d2a	q'0		fdy		fdx
		fsubp	st(1),st					;	d2c-d1d	d1b-d2a	q'0		fdy		fdx
											;	qdy		qdx		q'0		fdy		fdx

		fld		st(1)						;	qdx		qdy		qdx		q'0		fdy		fdx
		fmul	workspace_xstep_0			;	qdx*xs0	qdy		qdx		q'0		fdy		fdx
		fld		st(2)						;	qdx		qdx*xs0	qdy		qdx		q'0		fdy		fdx
		fmul	workspace_xstep_1			;	qdx*xs1	qdx*xs0	qdy		qdx		q'0		fdy		fdx
		 fxch	st(1)						;	qdx*xs0	qdx*xs1	qdy		qdx		q'0		fdy		fdx
		fadd	st,st(2)					;	qdy_0	qdx*xs1	qdy		qdx		q'0		fdy		fdx
		 fxch	st(3)						;	qdx		qdx*xs1	qdy		qdy_0	q'0		fdy		fdx

		fmul	st(6),st					;	qdx		qdx*xs1	qdy		qdy_0	q'0		fdy		fdx*qdx
		 fxch	st(2)						;	qdy		qdx*xs1	qdx		qdy_0	q'0		fdy		fdx*qdx
		fadd	st(1),st					;	qdy		qdy_1	qdx		qdy_0	q'0		fdy		fdx*qdx

		fmulp	st(5),st					;	qdy_1	qdx		qdy_0	q'0		fdy*qdy	fdx*qdx
		 fxch	st(3)						;	q'0		qdx		qdy_0	qdy_1	fdy*qdy	fdx*qdx
		faddp	st(5),st					;	qdx		qdy_0	qdy_1	fdy*qdy	fpx+qt
		 fxch	st(1)						;	qdy_0	qdx		qdy_1	fdy*qdy	fpx+qt

		fmul	maxuv						;	qdy_0'	qdx		qdy_1	fdy*qdy	fpx+qt
		 fxch	st(3)						;	fdy*qdy	qdx		qdy_1	qdy_0'	fpx+qt
		fxch	st(2)						;	qdy_1	qdx		fdy*qdy	qdy_0'	fpx+qt
		fmul	maxuv						;	qdy_1'	qdx		fdy*qdy	qdy_0'	fpx+qt
		 fxch	st(4)						;	fpx+qt	qdx		fdy*qdy	qdy_0'	qdy_1'	

		faddp	st(2),st					;	qdx		qstart	qdy_0'	qdy_1'

		fmul	maxuv						;	qdx'	qstart	qdy_0'	qdy_1'
		 fxch	st(2)						;	qdy_0'	qstart	qdx'	qdy_1'
		fadd	fp_conv_d					;	C+qdy_0	qstart	qdx'	qdy_1'
		 fxch	st(1)						;	qstart	C+qdy_0	qdx'	qdy_1'
		fmul	maxuv						;	qstart'	C+qdy_0	qdx'	qdy_1'
		 fxch	st(3)						;	qdy_1'	C+qdy_0	qdx'	qstart'
		fadd	fp_conv_d					;	C+qdy_1	C+qdy_0	qdx'	qstart'
		 fxch	st(2)						;	qdx'	C+qdy_0	C+qdy_1	qstart'
		fadd	fp_conv_d					;	C+qdx	C+qdy_0	C+qdy_1	qstart'
		 fxch	st(3)						;	qstart'	C+qdy_0	C+qdy_1	C+qdx
		fadd	fp_conv_d					;	C+qstrt	C+qdy_0	C+qdy_1	C+qdx
		 fxch	st(2)						;	C+qdy_1	C+qdy_0	C+qstrt	C+qdx

		fstp	real8 ptr work.pq.d_carry	;	C+qdy_0	C+qstrt	C+qdx
		fstp	real8 ptr work.pq.grad_x	;	C+qstrt	C+qdx
		fstp	real8 ptr work.pq.currentpix ;	C+qdx

		mov		eax,work.pq.grad_x
		mov		ebx,work.pq.currentpix

		fstp	real8 ptr work.pq.grad_x	;

		mov		work.pq.d_nocarry,eax
		mov		work.pq.current,ebx

		endm




; Do all the per-triangle work 
;
; Total ? cycles (less if fpu not set to extended precision)
;
;	eax: ptr to vertex0
;	ecx: ptr to vertex1
;	edx: ptr to vertex2
;
SETUP_FIXED	macro empty:=<ret>

		local count_cont,exit,top_zero,bottom_zero,empty_triangle

converted_vertex struct
_x		dword	?
		dword	?,?
_y		dword	?
converted_vertex ends

		assume eax: ptr brp_vertex, ebx: ptr brp_vertex, ecx: ptr brp_vertex, edx: ptr brp_vertex
			
	; Convert X,Y to float
	;
		fild	[eax].comp_x[C_SX*4]
		fild	[eax].comp_x[C_SY*4]

		fild	[ecx].comp_x[C_SX*4]
		fild	[ecx].comp_x[C_SY*4]

		fild	[edx].comp_x[C_SX*4]
		fild	[edx].comp_x[C_SY*4]

		 fxch	st(1)

		fstp	[workspace.v2_x]
		fstp	[workspace.v2_y]

		fstp	[workspace.v1_y]
		fstp	[workspace.v1_x]

		fstp	[workspace.v0_y]
		fstp	[workspace.v0_x]

	; Calculate area of triangle and generate dx1/2area, dx1/2area, dx1/2area and dx1/2area
	;
	; Also sort the vertices in Y order whilst divide is happening
	;
											;	0		1		2		3		4		5		6		7
		fld		[workspace.v2_x]			;	x2
		fsub	[workspace.v0_x]			;	dx2
		fld		[workspace.v1_x]			;	x1		dx2
		fsub	[workspace.v0_x]			;	dx1		dx2
		fld		[workspace.v2_y]			;	y2		dx1		dx2
		fsub	[workspace.v0_y]			;	dy2		dx1		dx2
		fld		[workspace.v1_y]			;	y1		dy2		dx1		dx2
		fsub	[workspace.v0_y]			;	dy1		dy2		dx1		dx2

		fld		st(2)						;	dx1		dy1		dy2		dx1		dx2
		fmul	st,st(2)					;	dx1*dy2	dy1		dy2		dx1		dx2

		fld		st(4)						;	dx2		dx1*dy2	dy1		dy2		dx1		dx2
		fmul	st,st(2)					;	dx2*dy1	dx1*dy2	dy1		dy2		dx1		dx2

		mov		workspace_v0,eax
		mov		eax,[eax].comp_x[C_SY*4]

		mov		workspace_v1,ecx
		mov		ecx,[ecx].comp_x[C_SY*4]

		fsubp	st(1),st					;	2area	dy1		dy2		dx1		dx2

		xor		ebx,ebx
		cmp		ecx,eax

		mov		workspace_v2,edx
		mov		edx,[edx].comp_x[C_SY*4]

		fdivr	fp_one_fixed				;	1/2area	dy1		dy2		dx1		dx2

		rcl		ebx,1
		cmp		edx,eax

		rcl		ebx,1
		cmp		edx,ecx

		rcl		ebx,1			; ebx now has 3 bit number characterising the order of the vertices.

		mov		sort_value,ebx
;V

		mov		eax,sort_table_0[ebx*4]
		mov		edx,sort_table_2[ebx*4]

		mov		esi,flip_table[ebx*4]
		mov		ebx,sort_table_1[ebx*4]

	; Load eax,ebx,edx with pointers to the three vertices in vertical order
	;
		mov		eax,[workspace_v0+eax]
		mov		edx,[workspace_v0+edx]

		mov		ebx,[workspace_v0+ebx]
		mov		workspace_flip,esi

	; Work out Y extents of triangle
	;
		mov		ebp,[eax].comp_x[C_SY*4]
		sar		ebp,16				; EBP = y_t

		mov		esi,[ebx].comp_x[C_SY*4]
		sar		esi,16				; ESI = y_m

		mov		edi,[edx].comp_x[C_SY*4]
		sar		edi,16				; EDI = y_b

	; Reload eax,ebx,edx with pointers to converted copies of vertices x,y
	;
		mov		mid_vertex,ebx
		mov		ebx,sort_value

		mov		top_vertex,eax				; remeber for parameter setup
		mov		bot_vertex,edx

		mov		eax,sort_table_0[ebx*4]
		mov		edx,sort_table_2[ebx*4]

		mov		ebx,sort_table_1[ebx*4]
;V

		lea		eax,[workspace.v0_x+eax]
		lea		edx,[workspace.v0_x+edx]

		lea		ebx,[workspace.v0_x+ebx]
;V

		assume eax: ptr converted_vertex, ebx:ptr converted_vertex,	edx: ptr converted_vertex

	; Catch special cases of empty top or bottom trapezoids
	;
		cmp		ebp,esi
		je		top_zero

		cmp		esi,edi
		je		bottom_zero
			
	; Parameter gradient startup and Y deltas for edge gradients
	;
											;	0		1		2		3		4		5		6		7
		fmul	st(1),st					;	1/2area	dy1*a	dy2		dx1		dx2

		fld		[ebx]._y					;	sy2		1/2area	dy1*a	dy2		dx1		dx2
		fsub	[eax]._y					;	dsy1	1/2area	dy1*a	dy2		dx1		dx2
		 fxch	st(3)						;	dy2 	1/2area	dy1*a	dsy1	dx1		dx2

		fmul	st,st(1)					;	dy2*a	1/2area	dy1*a	dsy1	dx1		dx2

		fld		[edx]._y					;	sy3		dy2*a	1/2area	dy1*a	dsy1	dx1		dx2
		fsub	[ebx]._y					;	dsy2	dy2*a	1/2area	dy1*a	dsy1	dx1		dx2
		 fxch	st(5)						;	dx1		dy2*a	1/2area	dy1*a	dsy1	dsy2	dx2
count_cont:
		fmul	st,st(2)					;	dx1*a	dy2*a	1/2area	dy1*a	dsy1	dsy2	dx2

		fld		[edx]._y					;	sy3		dx1*a	dy2*a	1/2area	dy1*a	dsy1	dsy2	dx2
		fsub	[eax]._y					;	dsy3	dx1*a	dy2*a	1/2area	dy1*a	dsy1	dsy2	dx2
		 fxch	st(7)						;	dx2		dx1*a	dy2*a	1/2area	dy1*a	dsy1	dsy2	dsy3

		fmul	st,st(3)					;	dx2*a	dx1*a	dy2*a	1/2area	dy1*a	dsy1	dsy2	dsy3
		 fxch	st(3)						;	1/2area	dx1*a	dy2*a	dx2*a	dy1*a	dsy1	dsy2	dsy3

		fstp	workspace_iarea
		fstp	workspace_dx1_a
		fstp	workspace_dy2_a
		fstp	workspace_dx2_a
		fstp	workspace_dy1_a				;	dy1		dy2		dy3

	; Find edge gradients of triangle
	;
	; R = 1/(dy1.dy2.dy3)
	;
	; gradient_major = dy1.dx2.dy3.R
	; gradient_minor1 = dx1.dy2.dy3.R
	; gradient_minor2 = dy1.dy2.dx3.R
	;
											;	0		1		2		3		4		5		6		7
		fld		st(2)						;	dy3		dy1		dy2		dy3
		fmul	st,st(2)					;	dy2*dy3	dy1		dy2		dy3

		fld		[ebx]._x					;	x2		dy2*dy3	dy1		dy2		dy3
		fsub	[eax]._x					;	dx1		dy2*dy3	dy1		dy2		dy3

		fld		st(1)						;	dy2*dy3 dx1		dy2*dy3	dy1		dy2		dy3
		fmul	st,st(3)					;	dy123	dx1		dy2*dy3	dy1		dy2		dy3

		fld		[edx]._x					;	x3		dy123	dx1		dy2*dy3	dy1		dy2		dy3
		fsub	[ebx]._x					;	dx2		dy123	dx1		dy2*dy3	dy1		dy2		dy3
		 fxch	st(2)						;	dx1		dy123	dx2		dy2*dy3	dy1		dy2		dy3
		fld		fp_one						;	1.0		dx1		dy123	dx2		dy2*dy3	dy1		dy2		dy3
		fdivrp	st(2),st					;	dx1		R		dx2		dy2*dy3	dy1		dy2		dy3

	; Generate counts
	;
		inc		ebp
		mov		ecx,esi

		sub		ecx,ebp						;	count_t = (y_m-y_t)-1

		sal		ebp,16

		mov		work_main_y,ebp				; save for X intercept calcs

		mov		work_top_count,ecx
		inc		esi

		sub		edi,esi						;	count_b = (y_b-y_m)-1

		sal		esi,16

		mov		m_y,esi						; save for X intercept calcs

		mov		work_bot_count,edi
		mov		esi,workspace_flip

	; Generate LR/RL flag into esi (used to index conversion numbers below)
	;
		mov		edi,workspace_iarea
;V
		xor		esi,edi						; Build LR flag in bit 31
;V
		shr		esi,31						; move down to bit 0
;V
		mov		work.tsl.direction,esi

	;XXX Setup screen pointers and strides
	;

	;XXX Work out which scan conversion function to use
	;

	; Finish of gradient calculations, interleaved with working out t_dy, and m_dy, the fractions
	; that the top and middle vertices are from the integer scanline boundaries
	;
	; t_dy = (yt+1) - vt->y
	; m_dy = (ym+1) - vm->y
	;
											;	0		1		2		3		4		5		6		7
		fmulp	st(3),st					;	R		dx2		XYY		dy1		dy2		dy3

		fld		[edx]._x					;	x3		R		dx2		XYY		dy1		dy2		dy3
		 fxch	st(3)						;	XYY		R		dx2		x3		dy1		dy2		dy3

		fmul	st,st(1)					;	XYY*R	R		dx2		x3		dy1		dy2		dy3
		 fxch	st(3)						;	x3		R		dx2		XYY*R	dy1		dy2		dy3

		fsub	[eax]._x					;	dx3		R		dx2		XYY*R	dy1		dy2		dy3
		 fxch	st(1)						;	R		dx3		dx2		XYY*R	dy1		dy2		dy3

		fmulp	st(4),st					;	dx3		dx2		XYY*R	dy1*R	dy2		dy3
		 fxch	st(2)						;	XYY*R	dx2		dx3		dy1*R	dy2		dy3

		fild	workspace_m_y				;	m_y		XYY*R	dx2		dx3		dy1*R	dy2		dy3
		 fxch	st(2)						;	dx2		XYY*R	m_y		dx3		dy1*R	dy2		dy3

		fmulp	st(6),st					;	XYY*R	m_y		dx3		dy1*R	dy2		dx2*dy3

		fild	work_main_y					;	t_y		XYY*R	m_y		dx3		dy1*R	dy2		dx2*dy3
		 fxch	st(3)						;	dx3		XYY*R	m_y		t_y		dy1*R	dy2		dx2*dy3

		fmulp	st(5),st					;	XYY*R	m_y		t_y		dy1*R	dy2*dx3	dx2*dy3
		 fxch	st(1)						;	m_y		XYY*R	t_y		dy1*R	dy2*dx3	dx2*dy3

		fsub	[ebx]._y					;	m_dy	XYY*R	t_y		dy1*R	dy2*dx3	dx2*dy3
		 fxch	st(3)						;	dy1*R	XYY*R	t_y		m_dy	dy2*dx3	dx2*dy3

		fmul	st(4),st					;	dy1*R	XYY*R	t_y		m_dy	YYX*R	dx2*dy3
		 fxch	st(2)						;	t_y		XYY*R	dy1*R	m_dy	YYX*R	dx2*dy3

		fsub	[eax]._y					;	t_dy	XYY*R	dy1*R	m_dy	YYX*R	dx2*dy3
		 fxch	st(2)						;	dy1*R	XYY*R	t_dy	m_dy	YYX*R	dx2*dy3

		fmulp	st(5),st					;	XYY*R	t_dy	m_dy	YYX*R	YXY*R
		 fxch	st(2)						;	m_dy	t_dy	XYY*R	YYX*R	YXY*R
											;	m_dy	t_dy	g1		gm		g2

	; Work out initial X intercepts with top and middle scanlines
	;
	; x_major	= gm * t_dy + vt->x
	; x_minor1 = g1 * t_dy + vt->x
	; x_minor2 = g2 * m_dy + vm->x
	;
											;	0		1		2		3		4		5		6		7
		fld		st(1)						;	t_dy	m_dy	t_dy	g1		gm		g2
		fxch	st(1)						;	m_dy	t_dy	t_dy	g1		gm		g2
											
											
		fmul	st,st(5)					;	m_dy*g2	t_dy	t_dy	g1		gm		g2
		fxch	st(2)						;	t_dy	t_dy	m_dy*g2	g1		gm		g2
											
		fst		workspace_t_dy

		fmul	st,st(3)					;	t_dy*g1	t_dy	m_dy*g2	g1		gm		g2
		 fxch	st(2)						;	m_dy*g2	t_dy	t_dy*g1	g1		gm		g2
											
		fadd	[ebx]._x					;	x_2		t_dy	t_dy*g1	g1		gm		g2
		 fxch	st(1)						;	t_dy	x_2		t_dy*g1	g1		gm		g2
											
		fmul	st,st(4)					;	t_dy*gm	x_2		t_dy*g1	g1		gm		g2
		 fxch	st(2)						;	t_dy*g1	x_2		t_dy*gm	g1		gm		g2

		fadd	[eax]._x					;	x_1		x_2		t_dy*gm	g1		gm		g2
		 fxch	st(3)						;	g1		x_2		t_dy*gm	x_1		gm		g2
											
		fadd	fp_conv_d16					;	g1+C	x_2		t_dy*gm	x_1		gm		g2
		 fxch	st(2)						;	t_dy*gm	x_2		g1+C	x_1		gm		g2
											
		fadd	[eax]._x					;	x_m		x_2		g1+C	x_1		gm		g2
		 fxch	st(4)						;	gm		x_2		g1+C	x_1		x_m		g2

		fadd	fp_conv_d16					;	gm+C	x_2		g1+C	x_1		x_m		g2
		 fxch	st(1)						;	x_2		gm+C	g1+C	x_1		x_m		g2
												
		fadd	xconv_d16_12[esi*8]			;	x_2+C	gm+C	g1+C	x_1		x_m		g2
		 fxch	st(5)						;	g2		gm+C	g1+C	x_1		x_m		x_2+C
												
		fadd	fp_conv_d16					;	g2+C	gm+C	g1+C	x_1		x_m		x_2+C
		 fxch	st(2)						;	g1+C	gm+C	g2+C	x_1		x_m		x_2+C

if USE_WORKSPACE
		fstp	real8 ptr work_top_i 		;	gm+C	g2+C	x_1		x_m		x_2+C
		fstp	real8 ptr work_main_i		;	g2+C	x_1		x_m		x_2+C
		fstp	real8 ptr work_bot_i 		;	x_1		x_m		x_2+C
												
		fadd	xconv_d16_12[esi*8]			;	x_1+C	x_m		x_2+C
		fxch	st(1)						;	x_m		x_1+C	x_2+C
												
		fadd	xconv_d16_m[esi*8]			;	x_m+C	x_1+C	x_2+C

 	; Load deltas back in registers
	;
		mov		edx,work_main_i				; read fixed d_xm
		mov		esi,work_top_i				; read fixed d_x1

		mov		edi,work_bot_i				; read fixed d_x2
		mov		ebx,workspace_v0			; Start preparing for parmeter setup
	
		fstp	real8 ptr work_main_i		;	x_1+C	x_2+C
		fstp	real8 ptr work_top_i		;	x_2+C

		mov		ecx,work_main_i
		mov		work_main_d_i,edx
		
		mov		work_top_d_i,esi
		and		ecx,0ffff0000h

		sar		edx,16						; get integer part of x delta down major edge
		mov		workspace_t_dx,ecx

		mov		ecx,workspace_v1			; Start preparing for parameter setup

		fild	workspace_t_dx				;	t_x		x_2+C

	; Generate floating point versions of x delta and x delta+4
	;
		mov		workspace_xstep_0,edx
		inc		edx
		mov		workspace_xstep_1,edx
		mov		edx,workspace_v2			; Start preparing for parameter setup

		fsub	[eax]._x					;	t_dx	x_2+C
		 fxch	st(1)						;	x_2+C	t_dx
		fstp	real8 ptr work_bot_i		;	t_dx

	; Load eax with ptr to top vertex
	;
		mov		eax,top_vertex
		mov		work_bot_d_i,edi

		fstp	[workspace_t_dx]			;

		fild	workspace_xstep_0			; xstep_0
		fild	workspace_xstep_1			; xstep_1 xstep_0
else
		fstp	real8 ptr work_top_d_i		;	gm+C	g2+C	x_1		x_m		x_2+C
		fstp	real8 ptr work_main_d_i		;	g2+C	x_1		x_m		x_2+C
		fstp	real8 ptr work_bot_d_i		;	x_1		x_m		x_2+C

		fadd	fconv_d16_12[esi*8]			;	x_1+C	x_m		x_2+C
		 fxch	st(1)						;	x_m		x_1+C	x_2+C

		fadd	fconv_d16_m[esi*8]			;	x_m+C	x_1+C	x_2+C
		 fxch	st(2)						;	x_2+C	x_1+C	x_m+C

		fstp	real8 ptr work_bot_i		;	x_1+C	x_m+C
		fstp	real8 ptr work_top_i		;	x_m+C
		fstp	real8 ptr work_main_i		;	

		fild	workspace_main_i			;	t_x

		mov		edx,work_main_d_i
		mov		ebx,workspace_v0			; Start preparing for parmeter setup

		sar		edx,16						; get integer part of x delta down major edge
		mov		ecx,workspace_v1			; Start preparing for parameter setup

		fsub	[eax]._x					;	t_dx

	; Generate floating point versions of x delta and x delta+4
	;
		mov		workspace_xstep_0,edx
		inc		edx
		mov		workspace_xstep_1,edx
		mov		edx,workspace_v2			; Start preparing for parameter setup

		fstp	[workspace_t_dx]			;

		fild	workspace_xstep_0			; xstep_0
		fild	workspace_xstep_1			; xstep_1 xstep_0

	; Load eax with ptr to top vertex
	;
		mov		eax,top_vertex
endif

	; Scale the t_d? fractions down by 65536.0 by modifying the exponent directly
	;
		mov		esi,workspace_t_dx

		test	esi,esi
		je		tdx_zero

		sub		esi,08000000h
tdx_zero:
		mov		edi,workspace_t_dy

		test	edi,edi
		je		tdy_zero

		sub		edi,08000000h
tdy_zero:
		mov		workspace_t_dx,esi

		mov		workspace_t_dy,edi

		fstp	workspace_xstep_1			; xstep_0
		fstp	workspace_xstep_0			;

		jmp		exit

		assume eax: ptr converted_vertex, ebx:ptr converted_vertex,	edx: ptr converted_vertex

	; Special cases for top or bottom counts == 0
	;
top_zero:
		cmp		ebp,edi			; Check for completely empty triangle
		je		empty_triangle
											;	0		1		2		3		4		5		6		7
		fmul	st(1),st					;	1/2area	dy1*a	dy2		dx1		dx2

		fld		fp_one						;	1.0		1/2area	dy1*a	dy2		dx1		dx2
		 fxch	st(3)						;	dy2 	1/2area	dy1*a	1.0		dx1		dx2
												
		fmul	st,st(1)					;	dy2*a	1/2area	dy1*a	1.0		dx1		dx2
												
		fld		[edx]._y					;	sy3		dy2*a	1/2area	dy1*a	1.0		dx1		dx2
		fsub	[ebx]._y					;	dsy2	dy2*a	1/2area	dy1*a	1.0		dx1		dx2
		 fxch	st(5)						;	dx1		dy2*a	1/2area	dy1*a	1.0		dsy2	dx2

		jmp		count_cont

bottom_zero:
											;	0		1		2		3		4		5		6		7
		fmul	st(1),st					;	1/2area	dy1*a	dy2		dx1		dx2
											
		fld		[ebx]._y					;	sy2		1/2area	dy1*a	dy2		dx1		dx2
		fsub	[eax]._y					;	dsy1	1/2area	dy1*a	dy2		dx1		dx2
		 fxch	st(3)						;	dy2 	1/2area	dy1*a	dsy1	dx1		dx2
												
		fmul	st,st(1)					;	dy2*a	1/2area	dy1*a	dsy1	dx1		dx2

		fld		fp_one						;	1.0		dy2*a	1/2area	dy1*a	dsy1	dx1		dx2
		 fxch	st(5)						;	dx1		dy2*a	1/2area	dy1*a	dsy1	1.0		dx2

		jmp		count_cont

	; Fill in block with NULL count and exit
	;
empty_triangle:
		mov		work_top_count,-1
		mov		work_bot_count,-1
		fstp	st(0)
		fstp	st(0)
		fstp	st(0)
		fstp	st(0)
		fstp	st(0)
		empty

exit:
		mov		esi,work_main_y
		shr		esi,16
		mov		work_main_y,esi

		endm


; Do all the per-triangle work for a single fixed parameter
;
; Total ? cycles
;
;	eax: ptr to top vertex
;	ebx: ptr to vertex0
;	ecx: ptr to vertex1
;	edx: ptr to vertex2
;
;
SETUP_FIXED_PARAM	macro	comp,param,conv,unsigned:=<0>

		assume eax: ptr brp_vertex, ebx: ptr brp_vertex, ecx: ptr brp_vertex, edx: ptr brp_vertex

	; work out parameter deltas
	; dp1 = param_1 - param_0
	; dp2 = param_2 - param_0
	;
	; 8? cycles
	;
											;	0		1		2		3		4		5		6		7
		fild	[edx].comp_x[comp*4]		;	p2
		fild	[ebx].comp_x[comp*4]		;	p0	p2
		fild	[ecx].comp_x[comp*4]		;	p1		p0	p2
		 fxch	st(1)						;	p0		p1	p2
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
		fmul	workspace_dy1_a				;	dp2*a	dp1		dp2
		fld		st(1)						;	dp1		dp2*a	dp1		dp2
		fmul	workspace_dy2_a				;	dp1*b	dp2*a	dp1		dp2
		fld		workspace_t_dx				;	fdx		dp1*b	dp2*a	dp1		dp2
		 fxch	st(4)						;	dp2		dp1*b	dp2*a	dp1		fdx
		fmul	workspace_dx1_a				;	dp2*c	dp1*b	dp2*a	dp1		fdx
		fld		workspace_t_dy				;	fdy		dp2*c	dp1*b	dp2*a	dp1		fdx
		 fxch	st(4)						;	dp1		dp2*c	dp1*b	dp2*a	fdy		fdx
		fmul	workspace_dx2_a				;	dp1*d	dp2*c	dp1*b	dp2*a	fdy		fdx
		 fxch	st(3)						;	dp2*a	dp2*c	dp1*b	dp1*d	fdy		fdx
		fsubp	st(2),st					;	dp2*c	d1b-d2a	dp1*d	fdy		fdx
		fild	[eax].comp_x[comp*4]		;	param_t	dp2*c	d1b-d2a	dp1*d	fdy		fdx
		 fxch	st(3)						;	dp1*d	dp2*c	d1b-d2a	param_t	fdy		fdx
		fsubp	st(1),st					;	d2c-d1d	d1b-d2a	param_t	fdy		fdx
											;	pdy		pdx		param_t	fdy		fdx

	; Build the inputs to the rasteriser
	;
	; pdy_0 = pdy + xstep_0 * pdx
	; pdy_1 = pdy + xstep_1 * pdx
	; pstart = param_t + pdx * fdx + pdy * fdy
	;
	; (A couple of the fixed points conversions are interleaved into this block)
	; 12 cycles
	;
											;	0		1		2		3		4		5		6		7
		fld		st(1)						;	pdx		pdy		pdx		param_t	fdy		fdx
		fmul	workspace_xstep_0			;	pdx*xs0	pdy		pdx		param_t	fdy		fdx
		fld		st(2)						;	pdx		pdx*xs0	pdy		pdx		param_t	fdy		fdx
		fmul	workspace_xstep_1			;	pdx*xs1	pdx*xs0	pdy		pdx		param_t	fdy		fdx
		 fxch	st(1)						;	pdx*xs0	pdx*xs1	pdy		pdx		param_t	fdy		fdx
		fadd	st,st(2)					;	pdy_0	pdx*xs1	pdy		pdx		param_t	fdy		fdx
		 fxch	st(3)						;	pdx		pdx*xs1	pdy		pdy_0	param_t	fdy		fdx

		fmul	st(6),st					;	pdx		pdx*xs1	pdy		pdy_0	param_t	fdy		fdx*pdx
		 fxch	st(2)						;	pdy		pdx*xs1	pdx		pdy_0	param_t	fdy		fdx*pdx
		fadd	st(1),st					;	pdy		pdy_1	pdx		pdy_0	param_t	fdy		fdx*pdx

		fmulp	st(5),st					;	pdy_1	pdx		pdy_0	param_t	fdy*pdy	fdx*pdx
		 fxch	st(3)						;	param_t	pdx		pdy_0	pdy_1	fdy*pdy	fdx*pdx
		faddp	st(5),st					;	pdx		pdy_0	pdy_1	fdy*pdy	fpx+pt
		 fxch	st(1)						;	pdy_0	pdx		pdy_1	fdy*pdy	fpx+pt
		fadd	conv						;	C+pdy_0	pdx		pdy_1	fdy*pdy	fpx+pt
		 fxch	st(2)						;	pdy_1	pdx		C+pdy_0	fdy*pdy	fpx+pt
		fadd	conv						;	C+pdy_1	pdx		C+pdy_0	fdy*pdy	fpx+pt
		 fxch	st(3)						;	fdy*pdy	pdx		C+pdy_0	C+pdy_1	fpx+pt
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
		fadd	conv						;	C+pdx	C+pdy_0	C+pdy_1	pstart
		 fxch	st(3)						;	pstart	C+pdy_0	C+pdy_1	C+pdx

; 1 clock delay

		fadd	conv						;	C+pstrt	C+pdy_0	C+pdy_1	C+pdx
		 fxch	st(2)						;	C+pdy_1	C+pdy_0	C+pstrt	C+pdx

if USE_WORKSPACE
		fstp	real8 ptr work_&param&_current
		fstp	real8 ptr work_&param&_grad_x

		mov		esi,dword ptr work_&param&_current
		mov		edi,dword ptr work_&param&_grad_x

		fstp	real8 ptr work_&param&_current
		fstp	real8 ptr work_&param&_grad_x

		mov		dword ptr work_&param&_d_carry,esi
		mov		dword ptr work_&param&_d_nocarry,edi

else

		fstp	real8 ptr work_&param&_d_carry
		fstp	real8 ptr work_&param&_grad_x
		fstp	real8 ptr work_&param&_currentpix

		mov		esi,dword ptr work_&param&_grad_x
		mov		edi,dword ptr work_&param&_currentpix

		fstp	real8 ptr work_&param&_grad_x	;

		mov		dword ptr work_&param&_d_nocarry,esi
		mov		dword ptr work_&param&_current,edi

endif
if unsigned
	; Remap from -1 to 1 signed to 0 to 1 unsigned
	;
			mov		esi,dword ptr work_&param&_current
			xor		esi,080000000h
			mov		dword ptr work_&param&_current,esi
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

			mov		dword ptr work_&param&_current,esi
			mov		dword ptr work_&param&_d_carry,edi

			mov		dword ptr work_&param&_grad_x,edi
			mov		dword ptr work_&param&_d_nocarry,edi

	endm


; Determine if this triangle will look sufficiently good with linear interpolation
;
; Total 30-53 cycles
;
;	eax: ptr to top vertex
;	ebx: ptr to vertex0
;	ecx: ptr to vertex1
;	edx: ptr to vertex2
;
SETUP_FIXED_CHECK_PERSPECTIVE_CHEAT macro cheat

		assume esi: ptr brp_vertex, edi: ptr brp_vertex, ebp: ptr brp_vertex

	; There are stalls and empty slots aplenty in here - difficult to remove

	; Fetch sorted vertex pointers
	;
	; 2 cycles

		mov		esi,top_vertex
		mov		edi,mid_vertex

		mov		ebp,bot_vertex
; v slot

	; Determine the range of SX and SY values covered by the three vertices
	; 
	; 7-9 cycles + 0-9 cycles branche misprediction

		mov		eax,[ebp].comp_x[C_SY*4]
		mov		ebx,[esi].comp_x[C_SX*4]

		sub		eax,[esi].comp_x[C_SY*4]
		mov		ecx,[edi].comp_x[C_SX*4]

		cmp		ebx,ecx
		jg		x0_larger

		mov		ebx,ecx
		mov		ecx,[esi].comp_x[C_SX*4]
x0_larger:

		mov		edx,[ebp].comp_x[C_SX*4]

		cmp		ebx,edx
		jg		x2_smaller

		mov		ebx,edx
x2_smaller:

		cmp		ecx,edx
		jl		x2_larger
		
		mov		ecx,edx
x2_larger:
		sub		ebx,ecx

	; Select the larger of the SX and SY ranges
	;
	; 1-2 cycles + 0-3 cycles branch misprediction

		cmp		eax,ebx
		ja		yrange_larger

		mov		eax,ebx
yrange_larger:

	; Calculate the W range and minimum value
	;
	; 5-7 cycles + 0-6 cycles branch misprediction

		mov		ebx,[esi].comp_x[C_W*4]
		mov		ecx,[edi].comp_x[C_W*4]

		cmp		ebx,ecx
		jg		w0_larger

		mov		ebx,ecx
		mov		ecx,[esi].comp_x[C_W*4]
w0_larger:

		mov		edx,[ebp].comp_x[C_W*4]

		cmp		ebx,edx
		jg		w2_smaller

		mov		ebx,edx
w2_smaller:

		cmp		ecx,edx
		jl		w2_larger
		
		mov		ecx,edx
w2_larger:
		sub		ebx,ecx

	; Multiply the ranges together and divide by the cutoff factor (4.0)
	;
	; 13 cycles

		imul	ebx

		shr		eax,18

		shl		edx,14
		mov		ebx,workspace_v0

		or		eax,edx
		mov		edx,workspace_v2

	; Cheat if w_range * srange / cutoff < w_min
	;
	; 2 cycles

		cmp		eax,ecx
		mov		eax,top_vertex	; restore pointers

		mov		ecx,workspace_v1
		jb		cheat

		endm


; Do the per-parameter calculations for perspective correct texture mapping
;
; Total ? cycles:
;
;	esi: ptr to top vertex
;	edi: ptr to middle vertex
;	ebp: ptr to bottom vertex
;
SETUP_FIXED_UV_PERSPECTIVE macro

		assume esi: ptr brp_vertex, edi: ptr brp_vertex, ebp: ptr brp_vertex

	; N.B. in this code, u0, v0, w0 etc. refer to the top vertex, u1 etc. to the middle and
	; u2 etc. to the bottom vertex.
	;
	; We use the sorted vertices because we need some intermediate results for the top vertex.
	; The gradients must be recalculated, and the sign of the area calculation may have been
	; changed by the sorting process.
	;
	; I could use the preconverted coordinates, I think it would save 1 cycle
	; at most.
	;
	; N.B. It may be possible to save a few cycles by doing the gradient calculations using the
	; unsorted vertices and the calculations for the start values and some of the terms in the
	; maxuv calculations using the top vertex, but this is awkward to arrange.
	;
	; 22-23 cycles + 0-3 cycles branch misprediction
											;	0		1		2		3		4		5		6		7
		fld		workspace_iarea				;	1/2area

		cmp		workspace_flip,0
		je		no_flip

		fchs								;	-1/2area
no_flip:

		fild	[ebp].comp_x[C_SX*4]		;	x2		1/2area
		fild	[esi].comp_x[C_SX*4]		;	x0		x2		1/2area
		fild	[edi].comp_x[C_SX*4]		;	x1		x0		x2		1/2area
		 fxch	st(1)						;	x0		x1		x2		1/2area
		fsub	st(2),st					;	x0		x1		dx2		1/2area
		fsubp	st(1),st					;	dx1		dx2		1/2area
		fmul	st(0),st(2)					;	dx1*a	dx2		1/2area
		fild 	[edi].comp_x[C_SY*4]		;	y1		dx1*a	dx2		1/2area
		 fxch	st(2)						;	dx2		dx1*a	y1		1/2area
		fmul	st(0),st(3)					;	dx2*a	dx1*a	y1		1/2area
		 fxch	st(2)						;	y1		dx1*a	dx2*a	1/2area
		fild	[esi].comp_x[C_SY*4]		;	y0		y1		dx1*a	dx2*a	1/2area
		fild	[ebp].comp_x[C_SY*4]		;	y2		y0		y1		dx1*a	dx2*a	1/2area
		 fxch	st(1)						;	y0		y2		y1		dx1*a	dx2*a	1/2area
		fsub	st(2),st					;	y0		y2		dy1		dx1*a	dx2*a	1/2area
		fsubp	st(1),st					;	dy2		dy1		dx1*a	dx2*a	1/2area
		 fxch	st(1)						;	dy1		dy2		dx1*a	dx2*a	1/2area
		fmul	st(0),st(4)					;	dy1*a	dy2		dx1*a	dx2*a	1/2area
		 fxch	st(2)						;	dx1*a	dy2		dy1*a	dx2*a	1/2area
		fstp	workspace_dx1_a				;	dy2		dy1*a	dx2*a	1/2area
		fmulp	st(3),st(0)					;	dy1*a	dx2*a	dy2*a
		fstp	workspace_dy1_a				;	dx2*a	dy2*a
		fstp	workspace_dx2_a				;	dy2*a
		fstp	workspace_dy2_a				;

	; Calculate integral base texture coords, first as fixed point values
	; then as integers
	;
	; 9 cycles

		mov		eax,[esi].comp_f[C_U*4]
		mov		ebx,[esi].comp_f[C_V*4]

		mov		ecx,eax
		mov		edx,ebx

		and		eax,0ffff0000h
		and		ebx,0ffff0000h

		shr		ecx,16
		mov		work.awsl.u_current,eax

		shr		edx,16
		mov		work.awsl.v_current,ebx

		fild	work.awsl.v_current			;	vb
		fild	work.awsl.u_current			;	ub		vb

		mov		work.awsl.u_current,ecx
		mov		work.awsl.v_current,edx

		fild	[edi].comp_f[C_W*4]			;	w1		ub		vb
		
	; Calculate u' and v' (deltas from base coords) and q' (= q * product(w)) for vertex 0
	;
	; 5 cycles
											;	0		1		2		3		4		5		6		7
		fimul	[ebp].comp_x[C_W*4]			;	q'0		ub		vb
		fild	[esi].comp_x[C_U*4]			;	u0		q'0		ub		vb
		fsub	st(0),st(2)					;	u'0		q'0		ub		vb
		fild	[esi].comp_x[C_V*4]			;	v0		u'0		q'0		ub		vb
		fsub	st(0),st(4)					;	v'0		u'0		q'0		ub		vb

	; Calculate |u'0| + |v'0| + |q'0|
	;
	; Because we do not do fixed->float conversions, u'0 is actually
	; u'0 * 2^16, v'0 is actually v'0 * 2^16 and q'0 is actually
	; q'0 * 2^32.  
	;
	; When we calculate u'q'n and v'q'n they will be * 2^48, so we
	; rescale them all to be * 2^48.
	;
	; This could be optimised.
	;
	; 19 cycles

		fld		st(2)						;	q'0		v'0		u'0		q'0		ub		vb
		fst		q0							;	q'0		v'0		u'0		q'0		ub		vb
		fabs								;	|q'0|	v'0		u'0		q'0		ub		vb
		fld		st(2)						;	u'0		|q'0|	v'0		u'0		q'0		ub		vb
		fmul	fp_one_fixed
		fabs								;	|u'0|	|q'0|	v'0		u'0		q'0		ub		vb
		fadd								;	maxuv	v'0		u'0		q'0		ub		vb
		fld		st(1)						;	v'0		maxuv	v'0		u'0		q'0		ub		vb
		fmul	fp_one_fixed
		fabs								;	|v'0|	maxuv	v'0		u'0		q'0		ub		vb
		fadd								;	maxuv	v'0		u'0		q'0		ub		vb
		fmul	fp_one_fixed

	; Multiply u'0 and v'0 by q'0
	;
	; 5 cycles

		 fxch	st(2)						;	u'0		v'0		maxuv	q'0		ub		vb
		fmul	st(0),st(3)					;	u'q'0	v'0		maxuv	q'0		ub		vb
		fild	[ebp].comp_x[C_W*4]			;	w2		u'q'0	v'0		maxuv	q'0		ub		vb
		 fxch	st(4)						;	q'0		u'q'0	v'0		maxuv	w2		ub		vb
		fmulp	st(2),st(0)					;	u'q'0	v'q'0	maxuv	w2		ub		vb

		 fxch	st(2)						;	maxuv	v'q'0	u'q'0	w2		ub		vb
		fstp	maxuv						;	v'q'0	u'q'0	w2		ub		vb

	; Calculate u' and v' (deltas from base coords) and q' (= q * product(w)) for vertex 1
	;
	; 5 cycles
											;	0		1		2		3		4		5		6		7
		fild	[edi].comp_x[C_U*4]			;	u1		v'q'0	u'q'0	w2		ub		vb
		 fxch	st(3)						;	w2		v'q'0	u'q'0	u1		ub		vb
		fimul	[esi].comp_x[C_W*4]			;	q'1		v'q'0	u'q'0	u1		ub		vb
		 fxch	st(3)						;	u1		v'q'0	u'q'0	q'1		ub		vb
		fsub	st(0),st(4)					;	u'1		v'q'0	u'q'0	q'1		ub		vb
		fild	[edi].comp_x[C_V*4]			;	v1		u'1		v'q'0	u'q'0	q'1		ub		vb
		fsub	st(0),st(6)					;	v'1		u'1		v'q'0	u'q'0	q'1		ub		vb
		 
	; Multiply u'1 and v'1 by q'1
	;
	; 5 cycles

		 fxch	st(1)						;	u'1		v'1		v'q'0	u'q'0	q'1		ub		vb
		fmul	st(0),st(4)					;	u'q'1	v'1		v'q'0	u'q'0	q'1		ub		vb
		 fxch	st(4)						;	q'1		v'1		v'q'0	u'q'0	u'q'1	ub		vb
		fild	[esi].comp_x[C_W*4]			;	w0		q'1		v'1		v'q'0	u'q'0	u'q'1	ub		vb
		 fxch	st(1)						;	q'1		w0		v'1		v'q'0	u'q'0	u'q'1	ub		vb
		fmul	st(2),st(0)					;	q'1		w0		v'q'1	v'q'0	u'q'0	u'q'1	ub		vb
		fstp	q1							;	w0		v'q'1	v'q'0	u'q'0	u'q'1	ub		vb

	; Calculate u' and v' (deltas from base coords) and q' (= q * product(w)) for vertex 2
	;
	; 4 cycles
											;	0		1		2		3		4		5		6		7
		fimul[edi].comp_x[C_W*4]			;	q'2		v'q'1	v'q'0	u'q'0	u'q'1	ub		vb
		 fxch	st(5)						;	ub		v'q'1	v'q'0	u'q'0	u'q'1	q'2		vb
		fisubr[ebp].comp_x[C_U*4]			;	u'2		v'q'1	v'q'0	u'q'0	u'q'1	q'2		vb
		 fxch	st(6)						;	vb		v'q'1	v'q'0	u'q'0	u'q'1	q'2		u'2
		fisubr[ebp].comp_x[C_V*4]			;	v'2		v'q'1	v'q'0	u'q'0	u'q'1	q'2		u'2

		fld		maxuv						;	maxuv	v'2		v'q'1	v'q'0	u'q'0	u'q'1	q'2		u'2
		 fxch	st(6)						;	q'2		v'2		v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'2

	; Multiply u'2 and v'2 by q'2
	;
	; 4 cycles

		fmul	st(7),st(0)					;	q'2		v'2		v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fst		q2							;	q'2		v'2		v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fmulp	st(1),st(0)					;	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2

	; Calculate |u'0| + |v'0| + |q'0| + sum(|u'q'| + |v'q'|) (vertex 0 is the top vertex)
	;
	; 18 cycles
											;	0		1		2		3		4		5		6		7
		fld		st(3)						;	u'q'0	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fabs								;	|u'q'0|	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		faddp	st(6),st(0)					;	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fld		st(2)						;	v'q'0	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fabs								;	|v'q'0|	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		faddp	st(6),st(0)					;	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fld		st(4)						;	u'q'1	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fabs								;	|u'q'1|	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		faddp	st(6),st(0)					;	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fld		st(1)						;	v'q'1	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fabs								;	|v'q'1|	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		faddp	st(6),st(0)					;	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fld		st(6)						;	u'q'2	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fabs								;	|u'q'2|	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		faddp	st(6),st(0)					;	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fld		st(0)						;	v'q'2	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		fabs								;	|v'q'2|	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2
		faddp	st(6),st(0)					;	v'q'2	v'q'1	v'q'0	u'q'0	u'q'1	maxuv	u'q'2

	; Calculate deltas for u'q' and v'q'
	;
	; 6 cycles
											;	0		1		2		3		4		5		6		7
		 fxch	st(2)						;	v'q'0	v'q'1	v'q'2	u'q'0	u'q'1	maxuv	u'q'2
		fsub	st(1),st(0)					;	v'q'0	dv1		v'q'2	u'q'0	u'q'1	maxuv	u'q'2
		fsub	st(2),st(0)					;	v'q'0	dv1		dv2		u'q'0	u'q'1	maxuv	u'q'2
		 fxch	st(3)						;	u'q'0	dv1		dv2		v'q'0	u'q'1	maxuv	u'q'2
		fsub	st(4),st(0)					;	u'q'0	dv1		dv2		v'q'0	du1		maxuv	u'q'2
		fsub	st(6),st(0)					;	u'q'0	dv1		dv2		v'q'0	du1		maxuv	du2

		 fxch	st(5)						;	maxuv	dv1		dv2		v'q'0	du1		u'q'0	du2
		fstp	maxuv						;	dv1		dv2		v'q'0	du1		u'q'0	du2

	; Calculate normalisation factor to ensure that numerator and denominator
	; values do not overflow at any point in the rasterisation.
	;
	; Dan's code now calculates 2^28/maxuv and multiplies everything by this.
	; However, it should be sufficient to use 2^(28-ceil(log2(maxuv))).
	;
	; In fact, I think the largest value that will ever appear in the
	; calculations is bounded by:
	;
	; max(|q'0|,|q'1|,|q'2|) + max(|u'0|,|u'1|,|u'2|,|v'0|,|v'1|,|v'2|) * max(dq/dx,dq/dy_carry))
	; 
	; 4-6 cycles + 0-3 cycles misprediction penalty
	;
		mov		eax,maxuv
;V
	; Round up to a power of two
	;
		test	eax,MASK_MANTISSA
		je		exact

		and		eax,not MASK_MANTISSA
;V
		add		eax,1 shl EXPONENT_SHIFT
;V
exact:

	; Multiply reciprocal by 2^28
	;
		neg		eax
;V
		add		eax,(EXPONENT_BIAS * 2 + 28) shl EXPONENT_SHIFT
;V

	; Do parameter calculations for u and v
	;
	; pdx = dp1 * dy2_a - dp2 * dy1_a
	; pdy = dp2 * dx1_a - dp1 * dx2_a
	;
	; pdy_0 = pdy + xstep_0 * pdx
	; pstart = param_t + pdx * fdx + pdy * fdy
	;
	; Final results are multiplied by the normalisation factor.  d_carry is
	; not calculated as the outer loop adds grad_x separately.
	;
	; 64 cycles
											;	0		1		2		3		4		5		6		7
		fld		st(3)						;	du1		dv1		dv2		v'q'0	du1		u'q'0	du2
		fmul	workspace_dy2_a				;	du1*b	dv1		dv2		v'q'0	du1		u'q'0	du2
		fld		st(6)						;	du2		du1*b	dv1		dv2		v'q'0	du1		u'q'0	du2
		fmul	workspace_dy1_a				;	du2*a	du1*b	dv1		dv2		v'q'0	du1		u'q'0	du2
		 fxch	st(5)						;	du1		du1*b	dv1		dv2		v'q'0	du2*a	u'q'0	du2

		mov		maxuv,eax

		fmul	workspace_dx2_a				;	du1*d	du1*b	dv1		dv2		v'q'0	du2*a	u'q'0	du2
		 fxch	st(1)						;	du1*b	du1*d	dv1		dv2		v'q'0	du2*a	u'q'0	du2
		fsubrp	st(5),st(0)					;	du1*d	dv1		dv2		v'q'0	udx		u'q'0	du2
		 fxch	st(6)						;	du2		dv1		dv2		v'q'0	udx		u'q'0	du1*d
		fmul	workspace_dx1_a				;	du2*c	dv1		dv2		v'q'0	udx		u'q'0	du1*d
		fld		workspace_t_dx				;	fdx		du2*c	dv1		dv2		v'q'0	udx		u'q'0	du1*d
		fmul	st(0),st(5)					;	fdx*udx	du2*c	dv1		dv2		v'q'0	udx		u'q'0	du1*d
		 fxch	st(7)						;	du1*d	du2*c	dv1		dv2		v'q'0	udx		u'q'0	fdx*udx
		fsubp	st(1),st(0)					;	udy		dv1		dv2		v'q'0	udx		u'q'0	fdx*udx
		fld		workspace_t_dy				;	fdy		udy		dv1		dv2		v'q'0	udx		u'q'0	fdx*udx
		 fxch	st(7)						;	fdx*udx	udy		dv1		dv2		v'q'0	udx		u'q'0	fdy
		faddp	st(6),st(0)					;	udy		dv1		dv2		v'q'0	udx		fux+ut	fdy
		fmul	st(6),st(0)					;	udy		dv1		dv2		v'q'0	udx		fux+ut	fdy*udy
		fld		st(4)						;	udx		udy		dv1		dv2		v'q'0	udx		fux+ut	fdy*udy
		 fxch	st(7)						;	fdy*udy	udy		dv1		dv2		v'q'0	udx		fux+ut	udx
		faddp	st(6),st(0)					;	udy		dv1		dv2		v'q'0	udx		ustart	udx
		 fxch	st(6)						;	udx		dv1		dv2		v'q'0	udx		ustart	udy
		fmul	workspace_xstep_0			;	udx*xs0	dv1		dv2		v'q'0	udx		ustart	udy
		fld		st(1)						;	dv1		udx*xs0	dv1		dv2		v'q'0	udx		ustart	udy
		 fxch	st(6)						;	ustart	udx*xs0	dv1		dv2		v'q'0	udx		dv1		udy
		fmul	maxuv						;	ustart'	udx*xs0	dv1		dv2		v'q'0	udx		dv1		udy
		 fxch	st(1)						;	udx*xs0	ustart'	dv1		dv2		v'q'0	udx		dv1		udy
		faddp	st(7),st(0)					;	ustart'	dv1		dv2		v'q'0	udx		dv1		udy_0
		 fxch	st(4)						;	udx		dv1		dv2		v'q'0	ustart'	dv1		udy_0
		fmul	maxuv						;	udx'	dv1		dv2		v'q'0	ustart'	dv1		udy_0
		 fxch	st(4)						;	ustart'	dv1		dv2		v'q'0	udx'	dv1		udy_0
		fadd	fp_conv_d					;	ustrt+C	dv1		dv2		v'q'0	udx'	dv1		udy_0
		 fxch	st(6)						;	udy_0	dv1		dv2		v'q'0	udx'	dv1		ustrt+C
		fmul	maxuv						;	udy_0'	dv1		dv2		v'q'0	udx'	dv1		ustrt+C
		 fxch	st(4)						;	udx'	dv1		dv2		v'q'0	udy_0'	dv1		ustrt+C
		fadd	fp_conv_d					;	udx+C	dv1		dv2		v'q'0	udy_0'	dv1		ustrt+C
		 fxch	st(6)						;	ustrt+C	dv1		dv2		v'q'0	udy_0'	dv1		udx+C
		fstp	real8 ptr work.pu.currentpix ;	dv1		dv2		v'q'0	udy_0'	dv1		udx+C
		fld		st(1)						;	dv2		dv1		dv2		v'q'0	udy_0'	dv1		udx+C
		 fxch	st(4)						;	udy_0'	dv1		dv2		v'q'0	dv2		dv1		udx+C
		fadd	fp_conv_d					;	udy_0+C	dv1		dv2		v'q'0	dv2		dv1		udx+C
		 fxch	st(6)						;	udx+C	dv1		dv2		v'q'0	dv2		dv1		udy_0+C
		fstp	real8 ptr work.pu.grad_x	;	dv1		dv2		v'q'0	dv2		dv1		udy_0+C
		fmul	workspace_dy2_a				;	dv1*b	dv2		v'q'0	dv2		dv1		udy_0+C
		 fxch	st(5)						;	udy_0+C	dv2		v'q'0	dv2		dv1		dv1*b
		fstp	real8 ptr work.pu.d_carry	;	dv2		v'q'0	dv2		dv1		dv1*b

		fmul	workspace_dy1_a				;	dv2*a	v'q'0	dv2		dv1		dv1*b
		 fxch	st(3)						;	dv1		v'q'0	dv2		dv2*a	dv1*b

		mov		eax,work.pu.currentpix
		mov		ebx,work.pu.d_carry

		fmul	workspace_dx2_a				;	dv1*d	v'q'0	dv2		dv2*a	dv1*b
		 fxch	st(2)						;	dv2		v'q'0	dv1*d	dv2*a	dv1*b

		mov		work.pu.current,eax
		mov		work.pu.d_nocarry,ebx

		fmul	workspace_dx1_a				;	dv2*c	v'q'0	dv1*d	dv2*a	dv1*b
		 fxch	st(4)						;	dv1*b	v'q'0	dv1*d	dv2*a	dv2*c
		fsubrp	st(3),st(0)					;	v'q'0	dv1*d	vdx		dv2*c
		fld		workspace_t_dx				;	fdx		v'q'0	dv1*d	vdx		dv2*c
		 fxch	st(2)						;	dv1*d	v'q'0	fdx		vdx		dv2*c
		fsubp	st(4),st(0)					;	v'q'0	fdx		vdx		vdy
		 fxch	st(1)						;	fdx		v'q'0	vdx		vdy
		fmul	st(0),st(2)					;	fdx*vdx	v'q'0	vdx		vdy
		fld		workspace_t_dy				;	fdy		fdx*vdx	v'q'0	vdx		vdy
		fmul	st(0),st(4)					;	fdy*vdy	fdx*vdx	v'q'0	vdx		vdy
		 fxch	st(1)						;	fdx*vdx	fdy*vdy	v'q'0	vdx		vdy
		faddp	st(2),st(0)					;	fdy*vdy	fvx+ft	vdx		vdy
		fld		st(2)						;	vdx		fdy*vdy	fvx+ft	vdx		vdy
		 fxch	st(1)						;	fdy*vdy	vdx		fvx+ft	vdx		vdy
		faddp	st(2),st(0)					;	vdx		vstart	vdx		vdy
		fmul	workspace_xstep_0			;	vdx*xs0	vstart	vdx		vdy
		 fxch	st(2)						;	vdx		vstart	vdx*xs0	vdy
;1 cycle
		fmul	maxuv						;	vdx'	vstart	vdx*xs0	vdy
		 fxch	st(2)						;	vdx*xs0	vstart	vdx'	vdy
		faddp	st(3),st(0)					;	vstart	vdx'	vdy_0
		fmul	maxuv						;	vstart'	vdx'	vdy_0
		 fxch	st(1)						;	vdx'	vstart'	vdy_0
		fadd	fp_conv_d					;	vdx+C	vstart'	vdy_0
		 fxch	st(2)						;	vdy_0	vstart'	vdx+C
		fmul	maxuv						;	vdy_0'	vstart'	vdx+C
		 fxch	st(1)						;	vstart'	vdy_0'	vdx+C
		fadd	fp_conv_d					;	vstrt+C	vdy_0'	vdx+C
		 fxch	st(1)						;	vdy_0'	vstrt+C	vdx+C
		fadd	fp_conv_d					;	vdy_0+C	vstrt+C	vdx+C
		 fxch	st(2)						;	vdx+C	vstrt+C	vdy_0+C
		fstp	real8 ptr work.pv.grad_x	;	vstrt+C	vdy_0+C
		fstp	real8 ptr work.pv.currentpix ;	vdy_0+C
		fstp	real8 ptr work.pv.d_carry	;	

		mov		eax,work.pv.currentpix
		mov		ebx,work.pv.d_carry

		mov		work.pv.current,eax
		mov		work.pv.d_nocarry,ebx

	; Do parameter calculations for q'
	;
	; pdx = dp1 * dy2_a - dp2 * dy1_a
	; pdy = dp2 * dx1_a - dp1 * dx2_a
	;
	; pdy_0 = pdy + xstep_0 * pdx
	; pdy_1 = pdy + xstep_1 * pdx
	; pstart = param_t + pdx * fdx + pdy * fdy
	;
	; Because q'n is actually q'n * 2^32, and u'q'n and v'q'n are 
	; * 2^48, we rescale to be *2'48.
	;
	; This could be optimised
	;
	; Final results are multiplied by the normalisation factor.
	;
	; 53 cycles
											;	0		1		2		3		4		5		6		7
		fld		q2							;	q2
		fsub	q0							;	dq2
		fmul	fp_one_fixed
		fld		q1							;	q1		dq2
		fsub	q0							;	dq1		dq2
		fmul	fp_one_fixed

		fld		st(1)						;	dq2		dq1		dq2
		fmul	workspace_dy1_a				;	dq2*a	dq1		dq2
		fld		st(1)						;	dq1		dq2*a	dq1		dq2
		fmul	workspace_dy2_a				;	dq1*b	dq2*a	dq1		dq2
		fld		workspace_t_dx				;	fdx		dq1*b	dq2*a	dq1		dq2
		 fxch	st(4)						;	dq2		dq1*b	dq2*a	dq1		fdx
		fmul	workspace_dx1_a				;	dq2*c	dq1*b	dq2*a	dq1		fdx
		fld		workspace_t_dy				;	fdy		dq2*c	dq1*b	dq2*a	dq1		fdx
		 fxch	st(4)						;	dq1		dq2*c	dq1*b	dq2*a	fdy		fdx
		fmul	workspace_dx2_a				;	dq1*d	dq2*c	dq1*b	dq2*a	fdy		fdx
		 fxch	st(3)						;	dq2*a	dq2*c	dq1*b	dq1*d	fdy		fdx
		fsubp	st(2),st					;	dq2*c	d1b-d2a	dq1*d	fdy		fdx
		fld		q0							;	q'0		dq2*c	d1b-d2a	dq1*d	fdy		fdx
		fmul	fp_one_fixed
		 fxch	st(3)						;	dq1*d	dq2*c	d1b-d2a	q'0		fdy		fdx
		fsubp	st(1),st					;	d2c-d1d	d1b-d2a	q'0		fdy		fdx
											;	qdy		qdx		q'0		fdy		fdx

		fld		st(1)						;	qdx		qdy		qdx		q'0		fdy		fdx
		fmul	workspace_xstep_0			;	qdx*xs0	qdy		qdx		q'0		fdy		fdx
		fld		st(2)						;	qdx		qdx*xs0	qdy		qdx		q'0		fdy		fdx
		fmul	workspace_xstep_1			;	qdx*xs1	qdx*xs0	qdy		qdx		q'0		fdy		fdx
		 fxch	st(1)						;	qdx*xs0	qdx*xs1	qdy		qdx		q'0		fdy		fdx
		fadd	st,st(2)					;	qdy_0	qdx*xs1	qdy		qdx		q'0		fdy		fdx
		 fxch	st(3)						;	qdx		qdx*xs1	qdy		qdy_0	q'0		fdy		fdx

		fmul	st(6),st					;	qdx		qdx*xs1	qdy		qdy_0	q'0		fdy		fdx*qdx
		 fxch	st(2)						;	qdy		qdx*xs1	qdx		qdy_0	q'0		fdy		fdx*qdx
		fadd	st(1),st					;	qdy		qdy_1	qdx		qdy_0	q'0		fdy		fdx*qdx

		fmulp	st(5),st					;	qdy_1	qdx		qdy_0	q'0		fdy*qdy	fdx*qdx
		 fxch	st(3)						;	q'0		qdx		qdy_0	qdy_1	fdy*qdy	fdx*qdx
		faddp	st(5),st					;	qdx		qdy_0	qdy_1	fdy*qdy	fpx+qt
		 fxch	st(1)						;	qdy_0	qdx		qdy_1	fdy*qdy	fpx+qt

		fmul	maxuv						;	qdy_0'	qdx		qdy_1	fdy*qdy	fpx+qt
		 fxch	st(3)						;	fdy*qdy	qdx		qdy_1	qdy_0'	fpx+qt
		fxch	st(2)						;	qdy_1	qdx		fdy*qdy	qdy_0'	fpx+qt
		fmul	maxuv						;	qdy_1'	qdx		fdy*qdy	qdy_0'	fpx+qt
		 fxch	st(4)						;	fpx+qt	qdx		fdy*qdy	qdy_0'	qdy_1'	

		faddp	st(2),st					;	qdx		qstart	qdy_0'	qdy_1'

		fmul	maxuv						;	qdx'	qstart	qdy_0'	qdy_1'
		 fxch	st(2)						;	qdy_0'	qstart	qdx'	qdy_1'
		fadd	fp_conv_d					;	C+qdy_0	qstart	qdx'	qdy_1'
		 fxch	st(1)						;	qstart	C+qdy_0	qdx'	qdy_1'
		fmul	maxuv						;	qstart'	C+qdy_0	qdx'	qdy_1'
		 fxch	st(3)						;	qdy_1'	C+qdy_0	qdx'	qstart'
		fadd	fp_conv_d					;	C+qdy_1	C+qdy_0	qdx'	qstart'
		 fxch	st(2)						;	qdx'	C+qdy_0	C+qdy_1	qstart'
		fadd	fp_conv_d					;	C+qdx	C+qdy_0	C+qdy_1	qstart'
		 fxch	st(3)						;	qstart'	C+qdy_0	C+qdy_1	C+qdx
		fadd	fp_conv_d					;	C+qstrt	C+qdy_0	C+qdy_1	C+qdx
		 fxch	st(2)						;	C+qdy_1	C+qdy_0	C+qstrt	C+qdx

		fstp	real8 ptr work.pq.d_carry	;	C+qdy_0	C+qstrt	C+qdx
		fstp	real8 ptr work.pq.grad_x	;	C+qstrt	C+qdx
		fstp	real8 ptr work.pq.currentpix ;	C+qdx

		mov		eax,work.pq.grad_x
		mov		ebx,work.pq.currentpix

		fstp	real8 ptr work.pq.grad_x	;

		mov		work.pq.d_nocarry,eax
		mov		work.pq.current,ebx

		endm


		.code

OPTION PROLOGUE:NONE
OPTION EPILOGUE:NONE

	ifdef BASED_FLOAT
	if BASED_FLOAT

		ECHO FLOAT

	if PARTS and (PART_8Z or PART_15Z or PART_16Z or PART_24Z or PART_8Z4 or PART_15Z4 or PART_16Z4 or PART_24Z4 or PART_8Z_FOG or PART_8Z_BLEND or PART_8Z_FOG_BLEND)

		ECHO Z-BUFFERED

TriangleSetup_ZPTI proc
		SETUP_FLOAT <ret>
		SETUP_FLOAT_PARAM C_SZ,pz,fp_conv_d16,1
		SETUP_FLOAT_PARAM C_I,pi,fp_conv_d16
		SETUP_FLOAT_CHECK_PERSPECTIVE_CHEAT cheat
		SETUP_FLOAT_UV_PERSPECTIVE
		clc
		ret
cheat:
		mov	esi,work.tsl.direction
		mov	workspace.flip,esi

		SETUP_FLOAT_PARAM C_U,pu,fp_conv_d16
		SETUP_FLOAT_PARAM C_V,pv,fp_conv_d16
		stc
		ret
TriangleSetup_ZPTI endp

TriangleSetup_ZPTI_FLAT proc
		SETUP_FLOAT <ret>
		SETUP_FLOAT_PARAM C_SZ,pz,fp_conv_d16,1
		SETUP_FLOAT_CONST C_I,pi,fp_conv_d16
		SETUP_FLOAT_CHECK_PERSPECTIVE_CHEAT cheat
		SETUP_FLOAT_UV_PERSPECTIVE
		clc
		ret
cheat:
		mov	esi,work.tsl.direction
		mov	workspace.flip,esi

		SETUP_FLOAT_PARAM C_U,pu,fp_conv_d16
		SETUP_FLOAT_PARAM C_V,pv,fp_conv_d16
		stc
		ret
TriangleSetup_ZPTI_FLAT endp

TriangleSetup_ZPT proc
		SETUP_FLOAT <ret>
		SETUP_FLOAT_PARAM C_SZ,pz,fp_conv_d16,1
		SETUP_FLOAT_CHECK_PERSPECTIVE_CHEAT cheat
		SETUP_FLOAT_UV_PERSPECTIVE
		clc
		ret
cheat:
		mov	esi,work.tsl.direction
		mov	workspace.flip,esi

		SETUP_FLOAT_PARAM C_U,pu,fp_conv_d16
		SETUP_FLOAT_PARAM C_V,pv,fp_conv_d16
		stc
		ret
TriangleSetup_ZPT endp

; Non cheat versions as a temporary measure because we don't have arbitrary
; width linear texture mapping using the new setup yet.

TriangleSetup_ZPTI_NOCHEAT proc
		SETUP_FLOAT <ret>
		SETUP_FLOAT_PARAM C_SZ,pz,fp_conv_d16,1
		SETUP_FLOAT_PARAM C_I,pi,fp_conv_d16

		mov		esi,top_vertex
		mov		edi,mid_vertex
		mov		ebp,bot_vertex

		SETUP_FLOAT_UV_PERSPECTIVE
		ret
TriangleSetup_ZPTI_NOCHEAT endp

TriangleSetup_ZPTI_FLAT_NOCHEAT proc
		SETUP_FLOAT <ret>
		SETUP_FLOAT_PARAM C_SZ,pz,fp_conv_d16,1
		SETUP_FLOAT_CONST C_I,pi,fp_conv_d16

		mov		esi,top_vertex
		mov		edi,mid_vertex
		mov		ebp,bot_vertex

		SETUP_FLOAT_UV_PERSPECTIVE
		ret
TriangleSetup_ZPTI_FLAT_NOCHEAT endp

TriangleSetup_ZPT_NOCHEAT proc
		SETUP_FLOAT <ret>
		SETUP_FLOAT_PARAM C_SZ,pz,fp_conv_d16,1

		mov		esi,top_vertex
		mov		edi,mid_vertex
		mov		ebp,bot_vertex

		SETUP_FLOAT_UV_PERSPECTIVE
		ret
TriangleSetup_ZPT_NOCHEAT endp

	endif

	if PARTS and (PART_8 or PART_15 or PART_16 or PART_24)

		ECHO NON Z-BUFFERED

TriangleSetup_PTI proc
		SETUP_FLOAT <ret>
		SETUP_FLOAT_PARAM C_I,pi,fp_conv_d16
		SETUP_FLOAT_CHECK_PERSPECTIVE_CHEAT cheat
		SETUP_FLOAT_UV_PERSPECTIVE
		clc
		ret
cheat:
		mov	esi,work.tsl.direction
		mov	workspace.flip,esi

		SETUP_FLOAT_PARAM C_U,pu,fp_conv_d16
		SETUP_FLOAT_PARAM C_V,pv,fp_conv_d16
		stc
		ret
TriangleSetup_PTI endp

TriangleSetup_PTI_FLAT proc
		SETUP_FLOAT <ret>
		SETUP_FLOAT_CONST C_I,pi,fp_conv_d16
		SETUP_FLOAT_CHECK_PERSPECTIVE_CHEAT cheat
		SETUP_FLOAT_UV_PERSPECTIVE
		clc
		ret
cheat:
		mov	esi,work.tsl.direction
		mov	workspace.flip,esi

		SETUP_FLOAT_PARAM C_U,pu,fp_conv_d16
		SETUP_FLOAT_PARAM C_V,pv,fp_conv_d16
		stc
		ret
TriangleSetup_PTI_FLAT endp

TriangleSetup_PT proc
		SETUP_FLOAT <ret>
		SETUP_FLOAT_CHECK_PERSPECTIVE_CHEAT cheat
		SETUP_FLOAT_UV_PERSPECTIVE
		clc
		ret
cheat:
		mov	esi,work.tsl.direction
		mov	workspace.flip,esi

		SETUP_FLOAT_PARAM C_U,pu,fp_conv_d16
		SETUP_FLOAT_PARAM C_V,pv,fp_conv_d16
		stc
		ret
TriangleSetup_PT endp

; Non cheat versions as a temporary measure because we don't have arbitrary
; width linear texture mapping using the new setup yet.

TriangleSetup_PTI_NOCHEAT proc
		SETUP_FLOAT <ret>
		SETUP_FLOAT_PARAM C_I,pi,fp_conv_d16

		mov		esi,top_vertex
		mov		edi,mid_vertex
		mov		ebp,bot_vertex

		SETUP_FLOAT_UV_PERSPECTIVE
		ret
TriangleSetup_PTI_NOCHEAT endp

TriangleSetup_PTI_FLAT_NOCHEAT proc
		SETUP_FLOAT <ret>
		SETUP_FLOAT_CONST C_I,pi,fp_conv_d16

		mov		esi,top_vertex
		mov		edi,mid_vertex
		mov		ebp,bot_vertex

		SETUP_FLOAT_UV_PERSPECTIVE
		ret
TriangleSetup_PTI_FLAT_NOCHEAT endp

TriangleSetup_PT_NOCHEAT proc
		SETUP_FLOAT <ret>

		mov		esi,top_vertex
		mov		edi,mid_vertex
		mov		ebp,bot_vertex

		SETUP_FLOAT_UV_PERSPECTIVE
		ret
TriangleSetup_PT_NOCHEAT endp

	endif

	endif
	endif

	ifdef BASED_FIXED
	if BASED_FIXED

		ECHO FIXED

	if PARTS and (PART_8Z or PART_15Z or PART_16Z or PART_24Z or PART_8Z4 or PART_15Z4 or PART_16Z4 or PART_24Z4 or PART_8Z_FOG or PART_8Z_BLEND or PART_8Z_FOG_BLEND)

		ECHO Z-BUFFERED

TriangleSetup_ZPTI proc
		SETUP_FIXED <ret>
		SETUP_FIXED_PARAM C_SZ,pz,fp_conv_d,1
		SETUP_FIXED_PARAM C_I,pi,fp_conv_d
		SETUP_FIXED_CHECK_PERSPECTIVE_CHEAT cheat
		SETUP_FIXED_UV_PERSPECTIVE
		clc
		ret
cheat:
		mov	esi,work.tsl.direction
		mov	workspace.flip,esi

		SETUP_FIXED_PARAM C_U,pu,fp_conv_d
		SETUP_FIXED_PARAM C_V,pv,fp_conv_d
		stc
		ret
TriangleSetup_ZPTI endp

TriangleSetup_ZPTI_FLAT proc
		SETUP_FIXED <ret>
		SETUP_FIXED_PARAM C_SZ,pz,fp_conv_d,1
		SETUP_FIXED_CONST C_I,pi,fp_conv_d
		SETUP_FIXED_CHECK_PERSPECTIVE_CHEAT cheat
		SETUP_FIXED_UV_PERSPECTIVE
		clc
		ret
cheat:
		mov	esi,work.tsl.direction
		mov	workspace.flip,esi

		SETUP_FIXED_PARAM C_U,pu,fp_conv_d
		SETUP_FIXED_PARAM C_V,pv,fp_conv_d
		stc
		ret
TriangleSetup_ZPTI_FLAT endp

TriangleSetup_ZPT proc
		SETUP_FIXED <ret>
		SETUP_FIXED_PARAM C_SZ,pz,fp_conv_d,1
		SETUP_FIXED_CHECK_PERSPECTIVE_CHEAT cheat
		SETUP_FIXED_UV_PERSPECTIVE
		clc
		ret
cheat:
		mov	esi,work.tsl.direction
		mov	workspace.flip,esi

		SETUP_FIXED_PARAM C_U,pu,fp_conv_d
		SETUP_FIXED_PARAM C_V,pv,fp_conv_d
		stc
		ret
TriangleSetup_ZPT endp

; Non cheat versions as a temporary measure because we don't have arbitrary
; width linear texture mapping using the new setup yet.

TriangleSetup_ZPTI_NOCHEAT proc
		SETUP_FIXED <ret>
		SETUP_FIXED_PARAM C_SZ,pz,fp_conv_d,1
		SETUP_FIXED_PARAM C_I,pi,fp_conv_d

		mov		esi,top_vertex
		mov		edi,mid_vertex
		mov		ebp,bot_vertex

		SETUP_FIXED_UV_PERSPECTIVE
		ret
TriangleSetup_ZPTI_NOCHEAT endp

TriangleSetup_ZPTI_FLAT_NOCHEAT proc
		SETUP_FIXED <ret>
		SETUP_FIXED_PARAM C_SZ,pz,fp_conv_d,1
		SETUP_FIXED_CONST C_I,pi,fp_conv_d

		mov		esi,top_vertex
		mov		edi,mid_vertex
		mov		ebp,bot_vertex

		SETUP_FIXED_UV_PERSPECTIVE
		ret
TriangleSetup_ZPTI_FLAT_NOCHEAT endp

TriangleSetup_ZPT_NOCHEAT proc
		SETUP_FIXED <ret>
		SETUP_FIXED_PARAM C_SZ,pz,fp_conv_d,1

		mov		esi,top_vertex
		mov		edi,mid_vertex
		mov		ebp,bot_vertex

		SETUP_FIXED_UV_PERSPECTIVE
		ret
TriangleSetup_ZPT_NOCHEAT endp

endif

if PARTS and (PART_8 or PART_15 or PART_16 or PART_24)

ECHO NON Z-BUFFERED

TriangleSetup_PTI proc
		SETUP_FIXED <ret>
		SETUP_FIXED_PARAM C_I,pi,fp_conv_d
		SETUP_FIXED_CHECK_PERSPECTIVE_CHEAT cheat
		SETUP_FIXED_UV_PERSPECTIVE
		clc
		ret
cheat:
		mov	esi,work.tsl.direction
		mov	workspace.flip,esi

		SETUP_FIXED_PARAM C_U,pu,fp_conv_d
		SETUP_FIXED_PARAM C_V,pv,fp_conv_d
		stc
		ret
TriangleSetup_PTI endp

TriangleSetup_PTI_FLAT proc
		SETUP_FIXED <ret>
		SETUP_FIXED_CONST C_I,pi,fp_conv_d
		SETUP_FIXED_CHECK_PERSPECTIVE_CHEAT cheat
		SETUP_FIXED_UV_PERSPECTIVE
		clc
		ret
cheat:
		mov	esi,work.tsl.direction
		mov	workspace.flip,esi

		SETUP_FIXED_PARAM C_U,pu,fp_conv_d
		SETUP_FIXED_PARAM C_V,pv,fp_conv_d
		stc
		ret
TriangleSetup_PTI_FLAT endp

TriangleSetup_PT proc
		SETUP_FIXED <ret>
		SETUP_FIXED_CHECK_PERSPECTIVE_CHEAT cheat
		SETUP_FIXED_UV_PERSPECTIVE
		clc
		ret
cheat:
		mov	esi,work.tsl.direction
		mov	workspace.flip,esi

		SETUP_FIXED_PARAM C_U,pu,fp_conv_d
		SETUP_FIXED_PARAM C_V,pv,fp_conv_d
		stc
		ret
TriangleSetup_PT endp

; Non cheat versions as a temporary measure because we don't have arbitrary
; width linear texture mapping using the new setup yet.

TriangleSetup_PTI_NOCHEAT proc
		SETUP_FIXED <ret>
		SETUP_FIXED_PARAM C_I,pi,fp_conv_d

		mov		esi,top_vertex
		mov		edi,mid_vertex
		mov		ebp,bot_vertex

		SETUP_FIXED_UV_PERSPECTIVE
		ret
TriangleSetup_PTI_NOCHEAT endp

TriangleSetup_PTI_FLAT_NOCHEAT proc
		SETUP_FIXED <ret>
		SETUP_FIXED_CONST C_I,pi,fp_conv_d

		mov		esi,top_vertex
		mov		edi,mid_vertex
		mov		ebp,bot_vertex

		SETUP_FIXED_UV_PERSPECTIVE
		ret
TriangleSetup_PTI_FLAT_NOCHEAT endp

TriangleSetup_PT_NOCHEAT proc
		SETUP_FIXED <ret>

		mov		esi,top_vertex
		mov		edi,mid_vertex
		mov		ebp,bot_vertex

		SETUP_FIXED_UV_PERSPECTIVE
		ret
TriangleSetup_PT_NOCHEAT endp

	endif

	endif
	endif


		.data

	; Three vertex pointers
	;
if USE_WORKSPACE eq 0
v0		dword	0
v1		dword	0
v2		dword	0
endif

	; Three sorted vertex pointers
	;
top_vertex	dword	0
mid_vertex	dword	0
bot_vertex	dword	0

xr_yr	dword	0
wr_sr	dword	0
wmin_4	dword	0

fp_four	dword	4.0

if USE_WORKSPACE eq 0
flip	dword	0
endif

	; area * 2 and deltas over area * 2
	;
	; *** would using qwords be better? ***
	;
if USE_WORKSPACE eq 0
iarea	dword	0

dx1_a	dword	0
dx2_a	dword	0
dy1_a	dword	0
dy2_a	dword	0

m_y		dword	0

t_dx	dword	0
t_dy	dword	0

xstep_0	dword	0
xstep_1	dword	0
endif

u_base	dword	0
v_base	dword	0

q0		dword	0
q1		dword	0
q2		dword	0

maxuv	dword	0

; Magic numbers to round to integer by adding and subtracting
;
fpu_cw		dword	0

fp_round	dword	(127+23) shl 23		; single precision
			dword	0					; reserved
			dword	(127+52) shl 23		; double precision
			dword	(127+63) shl 23		; extended precision

; Conversion values for major and minor edges, indexed by LR/RL flag*8
;
	; Fixed
	;
		align 8
xconv_d16_12	qword	04338000000000000h	; Conv + 0.0
xconv_d16_m		qword	04338000000010000h	; Conv + 1.0
				qword	04338000000000000h	; Conv + 0.0

	; Float
	;
fconv_d16_12	qword	04238000000000000h	; Conv + 0.0
fconv_d16_m		qword	04238000000010000h	; Conv + 1.0
				qword	04238000000000000h	; Conv + 0.0


fp_one			dword	1.0
fp_one_fixed	dword	65536.0
fp_conv_d		dword	(127+52-0) shl 23 + (1 shl 22)
fp_conv_d8		dword	(127+52-8) shl 23 + (1 shl 22)
fp_conv_d16		dword	(127+52-16) shl 23 + (1 shl 22)
fp_conv_d24		dword	(127+52-24) shl 23 + (1 shl 22)

fp_single_cw	word	107fh
fp_double_cw	word	127fh
fp_extended_cw	word	137fh,0


	; Converts results of comparisons to sort order for vertices
	;
sort_table_1	dword	1*4
				dword	2*4
				dword	0*4
sort_table_0	dword	0*4
				dword	0*4
				dword	0*4
sort_table_2	dword	2*4
				dword	1*4
				dword	0*4
				dword	1*4
				dword	2*4
				dword	0*4
				dword	0*4
				dword	0*4

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
										 
