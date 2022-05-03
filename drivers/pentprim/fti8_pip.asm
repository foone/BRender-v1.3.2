;; Copyright (c) 1992,1993-1995 Argonalt Technologies Limited. All rights reserved.
;;
;; $Id: fti8_pip.asm 1.1 1997/12/10 16:47:01 jon Exp $
;; $Locker: $
;;
;; Scanline loop for perpsective correct texture mapping
;;
		.586p
		.model	c,flat

		include	drv.inc
		include	586_macs.inc
		include	fpsetup.inc
		include	fpwork.inc

		externdef c TriangleRasterise_PTI_I8_64:proc
		externdef c TriangleRasterise_PTI_I8_128:proc
		externdef c TriangleRasterise_PTI_I8_256:proc
		externdef c TriangleRasterise_PTI_I8_1024:proc

		externdef c TriangleRasterise_TI_I8_256:proc
		externdef c TriangleRasterise_T_I8_32:proc
		externdef c TriangleRasterise_T_I8_64:proc
		externdef c TriangleRasterise_T_I8_128:proc
		externdef c TriangleRasterise_T_I8_256:proc
		externdef c TriangleRasterise_T_I8_1024:proc


USE_WORKSPACE	equ	1

TRANSPARENCY	equ	1


		.data

; Fix to stop masm creating prologue\epilogue to handle stack frame. 

PROLOGUE MACRO procname, flags, argbytes, localbytes, reglist, userparms:VARARG
	push ebp
	
	mov ebp,esp
	push ebx

	push edi
	push esi
	exitm <0>
endm

EPILOGUE MACRO procname, flags, argbytes, localbytes, reglist, userparms:VARARG
	pop	esi													   
	pop	edi

	pop	ebx
	pop	ebp

	ret
endm


; Alternative versions of useful instructions for making forward and backward
; scanning versions of rasterisers
;
add_f	equ	add
add_b	equ	sub

adc_f	equ	adc
adc_b	equ	sbb

sub_f	equ	sub
sub_b	equ	add

sbb_f	equ	sbb
sbb_b	equ	adc

inc_f	equ	inc
inc_b	equ	dec

jg_f	equ	jg
jg_b	equ	jl

jge_f	equ	jge
jge_b	equ	jle

jl_f	equ	jl
jl_b	equ	jg

jle_f	equ	jle
jle_b	equ	jge

jl_i	equ	jl
jl_d	equ	jge

add_i	equ	add
add_d	equ	sub

sub_i	equ	sub
sub_d	equ	add

jge_i	equ	jge
jge_d	equ	jnc


; Some equates that allow me to use the old style naming conventions (with .s replaced
; by _s) to refer to the workspace for the floating point setup
;
if USE_WORKSPACE

work_main_i			equ	workspace.xm
work_main_d_i		equ	workspace.d_xm
work_main_y			equ	workspace.t_y

work_top_count		equ	workspace.topCount
work_top_i			equ	workspace.x1
work_top_d_i		equ	workspace.d_x1

work_bot_count		equ	workspace.bottomCount
work_bot_i			equ	workspace.x2
work_bot_d_i		equ	workspace.d_x2

work_pi_current		equ	workspace.s_i
work_pi_grad_x		equ	workspace.d_i_x
work_pi_d_nocarry	equ	workspace.d_i_y_0
work_pi_d_carry		equ	workspace.d_i_y_1

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

work_pi_current		equ	work.pi.current		
work_pi_grad_x		equ	work.pi.grad_x		
work_pi_d_nocarry	equ	work.pi.d_nocarry	
work_pi_d_carry		equ	work.pi.d_carry		

endif


	; Macros allowing spare slots to be either filled with
	; NOPs or left empty
	;
uslot	macro
;		nop
		endm

vslot	macro
;		nop
		endm


TrapeziumRender_PTI_I8 macro size,dirn,pre,incu,decu,incv,decv,post1,post2

TrapeziumRender_PTI_I8_&size&_&dirn proc

		mov		ebx,work_top_count	; check for empty trapezium

		test	ebx,ebx
		jl		done_trapezium

		mov		edi,work_top_i
		mov		ebp,work_main_i

		shr		edi,16				; get integer part of end of first scanline
		and		ebp,0ffffh			; get integer part of start of first scanline

scan_loop:

	; Calculate start and end addresses for next scanline
	;
		cmp		ebp,edi				; calculate pixel count
		jg_&dirn	no_pixels

		mov		eax,workspace.scanAddress

		add		edi,eax				; calculate end colour buffer pointer
		add		eax,ebp				; calculate start colour buffer pointer

		mov		work.tsl._end,edi
		mov		work.tsl.start,eax

	; Fix up the error values
	;
		mov		eax,work.tsl.source
		mov		ebx,work.pq.current

		mov		ecx,work.pq.grad_x
		mov		edx,work.pq.d_nocarry

		pre
		mov		ebp,work.pu.current

		mov		edi,work.pu.grad_x
		mov		esi,work.pu.d_nocarry

		cmp		ebp,ebx
		jl		uidone
uiloop:
		incu							; work.tsl.source = incu(work.tsl.source)
		sub		edi,ecx					; work.pu.grad_x -= work.pq.grad_x

		sub		esi,edx					; work.pu.d_nocarry -= work.pq.d_nocarry
		sub		ebp,ebx					; work.pu.current -= work.pq.current

		cmp		ebp,ebx
		jge		uiloop

		jmp		uddone
uidone:
		cmp		ebp, 0
		jge		uddone
udloop:
		decu							; work.tsl.source = decu(work.tsl.source)
		add		edi,ecx					; work.pu.grad_x += work.pq.grad_x

		add		esi,edx					; work.pu.d_nocarry += work.pq.d_nocarry
		add		ebp,ebx					; work.pu.current += work.pq.current

		uslot
		jl		udloop

uddone:
		mov		work.pu.current,ebp
		mov		work.pu.grad_x,edi

		mov		work.pu.d_nocarry,esi
		mov		ebp,work.pv.current

		mov		edi,work.pv.grad_x
		mov		esi,work.pv.d_nocarry

		cmp		ebp,ebx
		jl		vidone
viloop:
		incv							; work.tsl.source = incv(work.tsl.source)
		sub		edi,ecx					; work.pv.grad_x -= work.pq.grad_x

		sub		esi,edx					; work.pv.d_nocarry -= work.pq.d_nocarry
		sub		ebp,ebx					; work.pv.current -= work.pq.current

		cmp		ebp,ebx
		jge		viloop

		jmp		vddone
vidone:
		test	ebp,ebp
		jge		vddone
vdloop:
		decv							; work.tsl.source = decv(work.tsl.source)
		add		edi,ecx					; work.pv.grad_x += work.pq.grad_x

		add		esi,edx					; work.pv.d_nocarry += work.pq.d_nocarry
		add		ebp,ebx					; work.pv.current += work.pq.current

		uslot
		jl		vdloop
vddone:

	; Select scanline loop and jump to it
	;
if 1
		post1
		mov		work.pv.current,ebp

		post2
		mov		work.pv.grad_x,edi

		mov		work.pv.d_nocarry,esi
		mov		work.tsl.source,eax

		mov		esi,work.pu.grad_x
		mov		ebp,work.pq.grad_x

		test	ebp,ebp
		jl_&dirn	qd

		test	esi,esi
		jle_&dirn	qi_ud

		test	edi,edi
		jle_&dirn	ScanlineRender_PTI_I8_&size&_&dirn&_ui_vd

		jmp		ScanlineRender_PTI_I8_&size&_&dirn&_ui_vi

qi_ud:

		test	edi,edi
		jle_&dirn	ScanlineRender_PTI_I8_&size&_&dirn&_ud_vd

		jmp		ScanlineRender_PTI_I8_&size&_&dirn&_ud_vi

qd:

		test	esi,esi
		jle_&dirn	qd_ud

qd_ui:

		test	edi,edi
		jg_&dirn	ScanlineRender_PTI_I8_&size&_&dirn&_ui_vi

		cmp		edi,ebp
		jle_&dirn	ScanlineRender_PTI_I8_&size&_&dirn&_ui_vd

		jmp		ScanlineRender_PTI_I8_&size&_&dirn&_ub_vb

qd_ud:

		cmp		esi,ebp
		jg_&dirn	ScanlineRender_PTI_I8_&size&_&dirn&_ub_vb

		test	edi,edi
		jg_&dirn	ScanlineRender_PTI_I8_&size&_&dirn&_ud_vi

		cmp		edi,ebp
		jle_&dirn	ScanlineRender_PTI_I8_&size&_&dirn&_ud_vd

		jmp		ScanlineRender_PTI_I8_&size&_&dirn&_ub_vb
else
		post1
		mov		ebx,work.pu.grad_x

		post2
		cmp		edi,80000000h

		sbb		edx,edx
		cmp		ebx,80000000h

		rcl		edx,1
		mov		work.pv.current,ebp

		mov		work.pv.grad_x,edi
		mov		work.pv.d_nocarry,esi

		mov		work.tsl.source,eax
		jmp		ScanlineRender_PTI_I8_&size&_&dirn&_table[edx * 4 + 2 * 4]
endif

	; Scanline loops return here when finished
	;
ScanlineRender_PTI_I8_&size&_&dirn&_done::

no_pixels:

	; Updates for next scanline:
	;
		mov		eax,workspace.scanAddress
		mov		ebx,work.colour.stride_b

		add		eax,ebx				; move down one line in colour buffer
		mov		ebp,work_main_i

		mov		workspace.scanAddress,eax
		mov		eax,work_main_d_i

		add		ebp,eax				; step major edge
		jc		carry

		mov		edi,work_top_i

		mov		work_main_i,ebp
		mov		eax,work_top_d_i

		add		edi,eax				; step minor edge
		mov		eax,work.pq.current

		mov		work_top_i,edi
		mov		ebx,work.pq.d_nocarry

		shr		edi,16				; get integer part of end of next scanline
		add		eax,ebx				; step q according to carry from major edge

		and		ebp,0ffffh			; get integer part of start of next scanline
		mov		work.pq.current,eax

		mov		ebx,work_pi_current
		mov		eax,work_pi_d_nocarry

		add		ebx,eax				; step i according to carry from major edge
		mov		eax,work.pu.current

		mov		work_pi_current,ebx
		mov		ebx,work.pv.current

		add		eax,work.pu.d_nocarry	; step u according to carry from major edge
		add		ebx,work.pv.d_nocarry	; step v according to carry from major edge

		mov		work.pu.current,eax
		mov		ecx,work_top_count

		mov		work.pv.current,ebx
		dec		ecx					; decrement line counter

		mov		work_top_count,ecx
		jge		scan_loop

		ret

carry:
		adc		ebp,0
		mov		edi,work_top_i

		mov		work_main_i,ebp
		mov		eax,work_top_d_i

		add		edi,eax				; step minor edge
		mov		eax,work.pq.current

		mov		work_top_i,edi
		mov		ebx,work.pq.d_carry

		shr		edi,16				; get integer part of end of next scanline
		add		eax,ebx				; step q according to carry from major edge

		and		ebp,0ffffh			; get integer part of start of next scanline
		mov		work.pq.current,eax

		mov		ebx,work_pi_current
		mov		eax,work_pi_d_carry

		add		ebx,eax				; step i according to carry from major edge
		mov		eax,work.pu.current

		mov		work_pi_current,ebx
		mov		ebx,work.pv.current

		add		eax,work.pu.d_nocarry	; step u according to carry from major edge
		add		ebx,work.pv.d_nocarry	; step v according to carry from major edge

		add		eax,work.pu.grad_x	; avoids the need to fixup nocarry and carry
		add		ebx,work.pv.grad_x	; versions

		mov		work.pu.current,eax
		mov		ecx,work_top_count

		mov		work.pv.current,ebx
		dec		ecx					; decrement line counter

		mov		work_top_count,ecx
		jge		scan_loop

done_trapezium:
		ret

TrapeziumRender_PTI_I8_&size&_&dirn endp

ifidni <dirn>,<f>
ScanlineRender_PTI_I8_&size&_&dirn&_table	dd	ScanlineRender_PTI_I8_&size&_&dirn&_ud_vi
											dd	ScanlineRender_PTI_I8_&size&_&dirn&_ui_vi
											dd	ScanlineRender_PTI_I8_&size&_&dirn&_ud_vd
											dd	ScanlineRender_PTI_I8_&size&_&dirn&_ui_vd
else
ScanlineRender_PTI_I8_&size&_&dirn&_table	dd	ScanlineRender_PTI_I8_&size&_&dirn&_ui_vd
											dd	ScanlineRender_PTI_I8_&size&_&dirn&_ud_vd
											dd	ScanlineRender_PTI_I8_&size&_&dirn&_ui_vi
											dd	ScanlineRender_PTI_I8_&size&_&dirn&_ud_vi
endif

ScanlineRender_PTI_I8 size,dirn,d,d,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>
ScanlineRender_PTI_I8 size,dirn,d,i,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>
ScanlineRender_PTI_I8 size,dirn,i,d,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>
ScanlineRender_PTI_I8 size,dirn,i,i,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>
ScanlineRender_PTI_I8 size,dirn,b,b,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>

		endm


TrapeziumRender_PT_I8 macro size,dirn,pre,incu,decu,incv,decv,post1,post2

TrapeziumRender_PT_I8_&size&_&dirn proc

		mov		ebx,work_top_count	; check for empty trapezium

		test	ebx,ebx
		jl		done_trapezium

		mov		edi,work_top_i
		mov		ebp,work_main_i

		shr		edi,16				; get integer part of end of first scanline
		and		ebp,0ffffh			; get integer part of start of first scanline

scan_loop:

	; Calculate start and end addresses for next scanline
	;
		cmp		ebp,edi				; calculate pixel count
		jg_&dirn	no_pixels

		mov		eax,workspace.scanAddress

		add		edi,eax				; calculate end colour buffer pointer
		add		eax,ebp				; calculate start colour buffer pointer

		mov		work.tsl._end,edi
		mov		work.tsl.start,eax

	; Fix up the error values
	;
		mov		eax,work.tsl.source
		mov		ebx,work.pq.current

		mov		ecx,work.pq.grad_x
		mov		edx,work.pq.d_nocarry

		pre
		mov		ebp,work.pu.current

		mov		edi,work.pu.grad_x
		mov		esi,work.pu.d_nocarry

		cmp		ebp,ebx
		jl		uidone
uiloop:
		incu							; work.tsl.source = incu(work.tsl.source)
		sub		edi,ecx					; work.pu.grad_x -= work.pq.grad_x

		sub		esi,edx					; work.pu.d_nocarry -= work.pq.d_nocarry
		sub		ebp,ebx					; work.pu.current -= work.pq.current

		cmp		ebp,ebx
		jge		uiloop

		jmp		uddone
uidone:
		cmp		ebp, 0
		jge		uddone
udloop:
		decu							; work.tsl.source = decu(work.tsl.source)
		add		edi,ecx					; work.pu.grad_x += work.pq.grad_x

		add		esi,edx					; work.pu.d_nocarry += work.pq.d_nocarry
		add		ebp,ebx					; work.pu.current += work.pq.current

		uslot
		jl		udloop

uddone:
		mov		work.pu.current,ebp
		mov		work.pu.grad_x,edi

		mov		work.pu.d_nocarry,esi
		mov		ebp,work.pv.current

		mov		edi,work.pv.grad_x
		mov		esi,work.pv.d_nocarry

		cmp		ebp,ebx
		jl		vidone
viloop:
		incv							; work.tsl.source = incv(work.tsl.source)
		sub		edi,ecx					; work.pv.grad_x -= work.pq.grad_x

		sub		esi,edx					; work.pv.d_nocarry -= work.pq.d_nocarry
		sub		ebp,ebx					; work.pv.current -= work.pq.current

		cmp		ebp,ebx
		jge		viloop

		jmp		vddone
vidone:
		test	ebp,ebp
		jge		vddone
vdloop:
		decv							; work.tsl.source = decv(work.tsl.source)
		add		edi,ecx					; work.pv.grad_x += work.pq.grad_x

		add		esi,edx					; work.pv.d_nocarry += work.pq.d_nocarry
		add		ebp,ebx					; work.pv.current += work.pq.current

		uslot
		jl		vdloop
vddone:

	; Select scanline loop and jump to it
	;
if 1
		post1
		mov		work.pv.current,ebp

		post2
		mov		work.pv.grad_x,edi

		mov		work.pv.d_nocarry,esi
		mov		work.tsl.source,eax

		mov		esi,work.pu.grad_x
		mov		ebp,work.pq.grad_x

		test	ebp,ebp
		jl_&dirn	qd

		test	esi,esi
		jle_&dirn	qi_ud

		test	edi,edi
		jle_&dirn	ScanlineRender_PT_I8_&size&_&dirn&_ui_vd

		jmp		ScanlineRender_PT_I8_&size&_&dirn&_ui_vi

qi_ud:

		test	edi,edi
		jle_&dirn	ScanlineRender_PT_I8_&size&_&dirn&_ud_vd

		jmp		ScanlineRender_PT_I8_&size&_&dirn&_ud_vi

qd:

		test	esi,esi
		jle_&dirn	qd_ud

qd_ui:

		test	edi,edi
		jg_&dirn	ScanlineRender_PT_I8_&size&_&dirn&_ui_vi

		cmp		edi,ebp
		jle_&dirn	ScanlineRender_PT_I8_&size&_&dirn&_ui_vd

		jmp		ScanlineRender_PT_I8_&size&_&dirn&_ub_vb

qd_ud:

		cmp		esi,ebp
		jg_&dirn	ScanlineRender_PT_I8_&size&_&dirn&_ub_vb

		test	edi,edi
		jg_&dirn	ScanlineRender_PT_I8_&size&_&dirn&_ud_vi

		cmp		edi,ebp
		jle_&dirn	ScanlineRender_PT_I8_&size&_&dirn&_ud_vd

		jmp		ScanlineRender_PT_I8_&size&_&dirn&_ub_vb
else
		post1
		mov		ebx,work.pu.grad_x

		post2
		cmp		edi,80000000h

		sbb		edx,edx
		cmp		ebx,80000000h

		rcl		edx,1
		mov		work.pv.current,ebp

		mov		work.pv.grad_x,edi
		mov		work.pv.d_nocarry,esi

		mov		work.tsl.source,eax
		jmp		ScanlineRender_PT_I8_&size&_&dirn&_table[edx * 4 + 2 * 4]
endif

	; Scanline loops return here when finished
	;
ScanlineRender_PT_I8_&size&_&dirn&_done::

no_pixels:

	; Updates for next scanline:
	;
		mov		eax,workspace.scanAddress
		mov		ebx,work.colour.stride_b

		add		eax,ebx				; move down one line in colour buffer
		mov		ebp,work_main_i

		mov		workspace.scanAddress,eax
		mov		eax,work_main_d_i

		add		ebp,eax				; step major edge
		jc		carry

		mov		edi,work_top_i

		mov		work_main_i,ebp
		mov		eax,work_top_d_i

		add		edi,eax				; step minor edge
		mov		eax,work.pq.current

		mov		work_top_i,edi
		mov		ebx,work.pq.d_nocarry

		shr		edi,16				; get integer part of end of next scanline
		add		eax,ebx				; step q according to carry from major edge

		and		ebp,0ffffh			; get integer part of start of next scanline
		mov		work.pq.current,eax

		mov		eax,work.pu.current
		mov		ebx,work.pv.current

		add		eax,work.pu.d_nocarry	; step u according to carry from major edge
		add		ebx,work.pv.d_nocarry	; step v according to carry from major edge

		mov		work.pu.current,eax
		mov		ecx,work_top_count

		mov		work.pv.current,ebx
		dec		ecx					; decrement line counter

		mov		work_top_count,ecx
		jge		scan_loop

		ret

carry:
		adc		ebp,0
		mov		edi,work_top_i

		mov		work_main_i,ebp
		mov		eax,work_top_d_i

		add		edi,eax				; step minor edge
		mov		eax,work.pq.current

		mov		work_top_i,edi
		mov		ebx,work.pq.d_carry

		shr		edi,16				; get integer part of end of next scanline
		add		eax,ebx				; step q according to carry from major edge

		and		ebp,0ffffh			; get integer part of start of next scanline
		mov		work.pq.current,eax

		mov		eax,work.pu.current
		mov		ebx,work.pv.current

		add		eax,work.pu.d_nocarry	; step u according to carry from major edge
		add		ebx,work.pv.d_nocarry	; step v according to carry from major edge

		add		eax,work.pu.grad_x	; avoids the need to fixup nocarry and carry
		add		ebx,work.pv.grad_x	; versions

		mov		work.pu.current,eax
		mov		ecx,work_top_count

		mov		work.pv.current,ebx
		dec		ecx					; decrement line counter

		mov		work_top_count,ecx
		jge		scan_loop

done_trapezium:
		ret

TrapeziumRender_PT_I8_&size&_&dirn endp

ifidni <dirn>,<f>
ScanlineRender_PT_I8_&size&_&dirn&_table	dd	ScanlineRender_PT_I8_&size&_&dirn&_ud_vi
											dd	ScanlineRender_PT_I8_&size&_&dirn&_ui_vi
											dd	ScanlineRender_PT_I8_&size&_&dirn&_ud_vd
											dd	ScanlineRender_PT_I8_&size&_&dirn&_ui_vd
else
ScanlineRender_PT_I8_&size&_&dirn&_table	dd	ScanlineRender_PT_I8_&size&_&dirn&_ui_vd
											dd	ScanlineRender_PT_I8_&size&_&dirn&_ud_vd
											dd	ScanlineRender_PT_I8_&size&_&dirn&_ui_vi
											dd	ScanlineRender_PT_I8_&size&_&dirn&_ud_vi
endif

ScanlineRender_PT_I8 size,dirn,d,d,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>
ScanlineRender_PT_I8 size,dirn,d,i,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>
ScanlineRender_PT_I8 size,dirn,i,d,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>
ScanlineRender_PT_I8 size,dirn,i,i,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>
ScanlineRender_PT_I8 size,dirn,b,b,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>

		endm




ScanlineRender_PTI_I8 macro size,dirn,udirn,vdirn,pre,incu,decu,incv,decv,post1,post2

ScanlineRender_PTI_I8_&size&_&dirn&_u&udirn&_v&vdirn proc

	; Make temporary copies of parameters that change
	;
		mov		edx,work.pu.current
		mov		esi,work.pu.grad_x

ifidni <udirn>,<i>
		mov		ebx,work.pq.current
		mov		ecx,work.pq.grad_x

		sub		edx,ebx				; Move error into the range -1..0
		sub		esi,ecx
endif

		mov		ebp,work.pv.current
		mov		edi,work.pv.grad_x

ifidni <vdirn>,<i>
ifdifi <udirn>,<i>
		mov		ebx,work.pq.current
		mov		ecx,work.pq.grad_x
endif

		sub		ebp,ebx				; Move error into the range -1..0
		sub		edi,ecx
endif

		mov		work.tsl.u_numerator, edx
		mov		work.tsl.du_numerator, esi

		mov		work.tsl.v_numerator, ebp
		mov		work.tsl.dv_numerator, edi

		mov		esi,work.texture.base
		mov		eax,work.tsl.source

		mov		edi,work.tsl.start
		mov		edx,work_pi_current

		mov		work.tsl.dest,edi
		mov		work.tsl.i,edx

		mov		edx,work.pq.current
		xor		ebx,ebx

		mov		work.tsl.denominator,edx
		mov		bh,byte ptr (work.tsl.i+2)

next_pixel:

	; Texel fetch and store section
	;
	; eax = source offset
	; ebx = texel
	; ecx = texel (lit)
	; edx = shade table
	; esi = texture base
	; edi = dest
	; ebp = 

	; Get texel and intensity
	;
		mov		edx,work.shade_table
		mov		bl,[eax+esi]

		mov		edi,work.tsl.dest
		mov		ebp,work.tsl._end

		inc_&dirn	edi
		mov		cl,[ebx+edx]

	; Test for transparency
	;
if TRANSPARENCY
		test	bl,bl
		jz		nodraw
endif

	; Look texel up in shade table, store texel
	;
ifidni <dirn>,<f>
		mov		[edi-1],cl
else
		mov		[edi+1],cl
endif
		vslot
nodraw:

	; Linear interpolation section
	;
	; eax = 
	; ebx = 
	; ecx = i
	; edx = di
	; esi = 
	; edi = dest
	; ebp = 

	; Update destination and check for end of scan
	;
		cmp		edi,ebp
		jg_&dirn	ScanlineRender_PTI_I8_&size&_&dirn&_done

	; Interpolate i
	;
		mov		ecx,work.tsl.i
		mov		edx,work.tsl._di

	; Prepare source offset for modification
	;
		pre

		add_&dirn	ecx,edx

		mov		work.tsl.dest,edi
		mov		work.tsl.i,ecx

	; Perspective interpolation section
	;
	; eax = source offset
	; ebx = u
	; ecx = v
	; edx = q
	; esi = 
	; edi = du,dv
	; ebp = dq
	;
		mov		edx,work.tsl.denominator
		mov		ebp,work.tsl.ddenominator

		mov		ebx,work.tsl.u_numerator
		mov		edi,work.tsl.du_numerator

	; Interpolate u numerator and denominator
	;
		add_&dirn	edx,ebp
		add_&dirn	ebx,edi

		mov		ecx,work.tsl.v_numerator

ifidni <udirn>,<b>

	; Check for u error going outside range 0..1
	;
		jge		nodecu
	
	; Adjust u downward
	;
deculoop:
		decu
		add		edi,ebp

		add		ebx,edx
		jl		deculoop

		mov		work.tsl.du_numerator,edi
		jmp		doneu

nodecu:

		cmp		ebx,edx
		jl		doneu

	; Adjust u upward
	;
inculoop:
		incu
		sub		edi,ebp

		sub		ebx,edx

		cmp		ebx,edx
		jge		inculoop

		mov		work.tsl.du_numerator,edi
		vslot

else

	; Check for u error going outside range 0..1
	;
		jl_&udirn	doneu
	
	; Adjust u
	;
stepuloop:
ifidni <udirn>,<i>
		incu
else
		decu
endif
		sub_&udirn	edi,ebp

		sub_&udirn	ebx,edx
		jge_&udirn	stepuloop

		mov		work.tsl.du_numerator,edi
		vslot

endif

doneu:
		mov		edi,work.tsl.dv_numerator
		mov		work.tsl.u_numerator,ebx
			
	; Interpolate v numerator
	;
		add_&dirn	ecx,edi
		mov		work.tsl.denominator,edx

ifidni <vdirn>,<b>

	; Check for v error going outside range 0..1
	;
		uslot
		jge		nodecv
	
	; Adjust v downward
	;
decvloop:
		decv
		add		edi,ebp

		add		ecx,edx
		jl		decvloop

		mov		work.tsl.dv_numerator,edi
		jmp		donev

nodecv:

		cmp		ecx,edx
		jl		donev

	; Adjust v upward
	;
incvloop:
		incv
		sub		edi,ebp

		sub		ecx,edx

		cmp		ecx,edx
		jge		incvloop

		mov		work.tsl.dv_numerator,edi
		vslot

else

	; Check for v error going outside range 0..1
	;
		uslot
		jl_&vdirn	donev

	; Adjust v
	;
stepvloop:
ifidni <vdirn>,<i>
		incv
else
		decv
endif
		sub_&vdirn	edi,ebp

		sub_&vdirn	ecx,edx
		jge_&vdirn	stepvloop

		mov		work.tsl.dv_numerator,edi
		vslot

endif

donev:

	; Fix wrapping of source offset after modification
	;
		post1
		mov		work.tsl.v_numerator,ecx

		post2
		xor		ebx,ebx

		mov		bh,byte ptr (work.tsl.i+2)
		jmp		next_pixel

ScanlineRender_PTI_I8_&size&_&dirn&_u&udirn&_v&vdirn endp

		endm


ScanlineRender_PT_I8 macro size,dirn,udirn,vdirn,pre,incu,decu,incv,decv,post1,post2

ScanlineRender_PT_I8_&size&_&dirn&_u&udirn&_v&vdirn proc

	; Make temporary copies of parameters that change
	;
		mov		edx,work.pu.current
		mov		esi,work.pu.grad_x

ifidni <udirn>,<i>
		mov		ebx,work.pq.current
		mov		ecx,work.pq.grad_x

		sub		edx,ebx				; Move error into the range -1..0
		sub		esi,ecx
endif

		mov		ebp,work.pv.current
		mov		edi,work.pv.grad_x

ifidni <vdirn>,<i>
ifdifi <udirn>,<i>
		mov		ebx,work.pq.current
		mov		ecx,work.pq.grad_x
endif

		sub		ebp,ebx				; Move error into the range -1..0
		sub		edi,ecx
endif

		mov		work.tsl.u_numerator, edx
		mov		work.tsl.du_numerator, esi

		mov		work.tsl.v_numerator, ebp
		mov		work.tsl.dv_numerator, edi

		mov		esi,work.texture.base
		mov		eax,work.tsl.source

		mov		edi,work.tsl.start
		mov		edx,work.pq.current

		mov		work.tsl.dest,edi
		mov		work.tsl.denominator,edx

next_pixel:

	; Texel fetch and store section
	;
	; eax = source offset
	; ebx = 
	; ecx = texel
	; edx = 
	; esi = texture base
	; edi = dest
	; ebp = 

	; Get texel
	;
		mov		cl,[eax+esi]
		mov		edi,work.tsl.dest

	; Test for transparency
	;
if TRANSPARENCY
		test	cl,cl
		jz		nodraw
endif

	; Store texel
	;
		mov		[edi],cl
		vslot
nodraw:

	; Linear interpolation section
	;
	; eax = 
	; ebx = 
	; ecx = 
	; edx = 
	; esi = 
	; edi = dest
	; ebp = 

	; Update destinations and check for end of scan
	;
		mov		ebp,work.tsl._end
		inc_&dirn	edi

		cmp		edi,ebp
		jg_&dirn	ScanlineRender_PT_I8_&size&_&dirn&_done

	; Perspective interpolation section
	;
	; eax = source offset
	; ebx = u
	; ecx = v
	; edx = q
	; esi = 
	; edi = du,dv
	; ebp = dq
	;

	; Prepare source offset for modification
	;
		pre
		mov		work.tsl.dest,edi

		mov		edx,work.tsl.denominator
		mov		ebp,work.tsl.ddenominator

		mov		ebx,work.tsl.u_numerator
		mov		edi,work.tsl.du_numerator

	; Interpolate u numerator and denominator
	;
		add_&dirn	edx,ebp
		add_&dirn	ebx,edi

		mov		ecx,work.tsl.v_numerator

ifidni <udirn>,<b>

	; Check for u error going outside range 0..1
	;
		jge		nodecu
	
	; Adjust u downward
	;
deculoop:
		decu
		add		edi,ebp

		add		ebx,edx
		jl		deculoop

		mov		work.tsl.du_numerator,edi
		jmp		doneu

nodecu:

		cmp		ebx,edx
		jl		doneu

	; Adjust u upward
	;
inculoop:
		incu
		sub		edi,ebp

		sub		ebx,edx

		cmp		ebx,edx
		jge		inculoop

		mov		work.tsl.du_numerator,edi
		vslot

else

	; Check for u error going outside range 0..1
	;
		jl_&udirn	doneu
	
	; Adjust u
	;
stepuloop:
ifidni <udirn>,<i>
		incu
else
		decu
endif
		sub_&udirn	edi,ebp

		sub_&udirn	ebx,edx
		jge_&udirn	stepuloop

		mov		work.tsl.du_numerator,edi
		vslot

endif

doneu:
		mov		edi,work.tsl.dv_numerator
		mov		work.tsl.u_numerator,ebx
			
	; Interpolate v numerator
	;
		add_&dirn	ecx,edi
		mov		work.tsl.denominator,edx

ifidni <vdirn>,<b>

	; Check for v error going outside range 0..1
	;
		uslot
		jge		nodecv
	
	; Adjust v downward
	;
decvloop:
		decv
		add		edi,ebp

		add		ecx,edx
		jl		decvloop

		mov		work.tsl.dv_numerator,edi
		jmp		donev

nodecv:

		cmp		ecx,edx
		jl		donev

	; Adjust v upward
	;
incvloop:
		incv
		sub		edi,ebp

		sub		ecx,edx

		cmp		ecx,edx
		jge		incvloop

		mov		work.tsl.dv_numerator,edi
		vslot

else

	; Check for v error going outside range 0..1
	;
		uslot
		jl_&vdirn	donev

	; Adjust v
	;
stepvloop:
ifidni <vdirn>,<i>
		incv
else
		decv
endif
		sub_&vdirn	edi,ebp

		sub_&vdirn	ecx,edx
		jge_&vdirn	stepvloop

		mov		work.tsl.dv_numerator,edi
		vslot

endif

donev:

	; Fix wrapping of source offset after modification
	;
		post1
		mov		work.tsl.v_numerator,ecx

		post2
		jmp		next_pixel

ScanlineRender_PT_I8_&size&_&dirn&_u&udirn&_v&vdirn endp

		endm


		.code

OPTION PROLOGUE:PROLOGUE 
OPTION EPILOGUE:EPILOGUE


	if PARTS and PART_8

TriangleRender_PTI_I8_64_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_PTI_FLAT_NOCHEAT

		jmp		TriangleRasterise_PTI_I8_64

TriangleRender_PTI_I8_64_FLAT endp


TriangleRender_PTI_I8_64 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_PTI_NOCHEAT

TriangleRasterise_PTI_I8_64 label proc

	; Calculate address of first scanline in colour buffer
	;
		mov		esi,work_main_y
		mov		ecx,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		imul	ebx,esi

		add		ecx,ebx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i

		dec		ecx
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		workspace.scanAddress,ecx

		ror		ebx,16
		mov		edx,work_pi_grad_x

		mov		work_main_i,eax
		mov		al,byte ptr work.awsl.u_current

		mov		work_main_d_i,ebx
		mov		ah,byte ptr work.awsl.v_current

		shl		al,2
		mov		work.tsl._di,edx

		shr		eax,2
		mov		ebx,work.pq.grad_x

		and		eax,63*65
		mov		work.tsl.ddenominator,ebx

		mov		work.tsl.source,eax
		mov		eax,work.tsl.direction

	; Check scan direction and use appropriate rasteriser
	;
		test	eax,eax
		jnz		reversed

		call	TrapeziumRender_PTI_I8_64_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_PTI_I8_64_f
		
		ret

reversed:

		call	TrapeziumRender_PTI_I8_64_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_PTI_I8_64_b
		
		ret

TriangleRender_PTI_I8_64 endp


TriangleRender_PT_I8_64 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_PT
		jc		TriangleRasterise_T_I8_64

	; Calculate address of first scanline in colour buffer
	;
		mov		esi,work_main_y
		mov		ecx,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		imul	ebx,esi

		add		ecx,ebx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i

		dec		ecx
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		workspace.scanAddress,ecx

		ror		ebx,16
		mov		cl,byte ptr work.awsl.u_current

		mov		ch,byte ptr work.awsl.v_current
		mov		work_main_i,eax

		shl		cl,2
		mov		work_main_d_i,ebx

		shr		ecx,2
		mov		ebx,work.pq.grad_x

		and		ecx,63*65
		mov		work.tsl.ddenominator,ebx

		mov		work.tsl.source,ecx
		mov		eax,work.tsl.direction

	; Check scan direction and use appropriate rasteriser
	;
		test	eax,eax
		jnz		reversed

		call	TrapeziumRender_PT_I8_64_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_PT_I8_64_f
		
		ret

reversed:

		call	TrapeziumRender_PT_I8_64_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_PT_I8_64_b
		
		ret

TriangleRender_PT_I8_64 endp


TriangleRender_PT_I8_32 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_PT
		jc		TriangleRasterise_T_I8_32

	; Calculate address of first scanline in colour buffer
	;
		mov		esi,work_main_y
		mov		ecx,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		imul	ebx,esi

		add		ecx,ebx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i

		dec		ecx
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		workspace.scanAddress,ecx

		ror		ebx,16
		mov		cl,byte ptr work.awsl.u_current

		mov		ch,byte ptr work.awsl.v_current
		mov		work_main_i,eax

		shl		cl,2
		mov		work_main_d_i,ebx

		shr		ecx,2
		mov		ebx,work.pq.grad_x

		and		ecx,63*65
		mov		work.tsl.ddenominator,ebx

		mov		work.tsl.source,ecx
		mov		eax,work.tsl.direction

	; Check scan direction and use appropriate rasteriser
	;
		test	eax,eax
		jnz		reversed

		call	TrapeziumRender_PT_I8_32_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_PT_I8_32_f
		
		ret

reversed:

		call	TrapeziumRender_PT_I8_32_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_PT_I8_32_b
		
		ret

TriangleRender_PT_I8_32 endp

TriangleRender_PTI_I8_128_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_PTI_FLAT_NOCHEAT

		jmp		TriangleRasterise_PTI_I8_128

TriangleRender_PTI_I8_128_FLAT endp


TriangleRender_PTI_I8_128 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_PTI_NOCHEAT

TriangleRasterise_PTI_I8_128 label proc

	; Calculate address of first scanline in colour buffer
	;
		mov		esi,work_main_y
		mov		ecx,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		imul	ebx,esi

		add		ecx,ebx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i

		dec		ecx
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		workspace.scanAddress,ecx

		ror		ebx,16
		mov		edx,work_pi_grad_x

		mov		work_main_i,eax
		mov		al,byte ptr work.awsl.u_current

		mov		work_main_d_i,ebx
		mov		ah,byte ptr work.awsl.v_current

		shl		al,1
		mov		work.tsl._di,edx

		shr		eax,1
		mov		ebx,work.pq.grad_x

		and		eax,127*129
		mov		work.tsl.ddenominator,ebx

		mov		work.tsl.source,eax
		mov		eax,work.tsl.direction

	; Check scan direction and use appropriate rasteriser
	;
		test	eax,eax
		jnz		reversed

		call	TrapeziumRender_PTI_I8_128_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_PTI_I8_128_f
		
		ret

reversed:

		call	TrapeziumRender_PTI_I8_128_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_PTI_I8_128_b
		
		ret

TriangleRender_PTI_I8_128 endp


TriangleRender_PT_I8_128 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_PT
		jc		TriangleRasterise_T_I8_128

	; Calculate address of first scanline in colour buffer
	;
		mov		esi,work_main_y
		mov		ecx,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		imul	ebx,esi

		add		ecx,ebx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i

		dec		ecx
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		workspace.scanAddress,ecx

		ror		ebx,16
		mov		cl,byte ptr work.awsl.u_current

		mov		ch,byte ptr work.awsl.v_current
		mov		work_main_i,eax

		shl		cl,1
		mov		work_main_d_i,ebx

		shr		ecx,1
		mov		ebx,work.pq.grad_x

		and		ecx,127*129
		mov		work.tsl.ddenominator,ebx

		mov		work.tsl.source,ecx
		mov		eax,work.tsl.direction

	; Check scan direction and use appropriate rasteriser
	;
		test	eax,eax
		jnz		reversed

		call	TrapeziumRender_PT_I8_128_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_PT_I8_128_f
		
		ret

reversed:

		call	TrapeziumRender_PT_I8_128_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_PT_I8_128_b
		
		ret

TriangleRender_PT_I8_128 endp


TriangleRender_PTI_I8_256_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.  Jump into the linear rasterizer in cheat cases.
	;
		call	TriangleSetup_PTI_FLAT
		jc		TriangleRasterise_TI_I8_256

		jmp		TriangleRasterise_PTI_I8_256

TriangleRender_PTI_I8_256_FLAT endp


TriangleRender_PTI_I8_256 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.  Jump into the linear rasterizer in cheat cases.
	;
		call	TriangleSetup_PTI
		jc		TriangleRasterise_TI_I8_256

TriangleRasterise_PTI_I8_256 label proc

	; Calculate address of first scanline in colour buffer
	;
		mov		esi,work_main_y
		mov		ecx,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		imul	ebx,esi

		add		ecx,ebx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i

		dec		ecx
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		workspace.scanAddress,ecx

		ror		ebx,16
		mov		edx,work_pi_grad_x

		mov		work_main_i,eax

		xor		eax,eax
		mov		work_main_d_i,ebx

		mov		al,byte ptr work.awsl.u_current

		mov		ah,byte ptr work.awsl.v_current
		mov		work.tsl._di,edx

		mov		work.tsl.source,eax
		mov		ebx,work.pq.grad_x

		mov		work.tsl.ddenominator,ebx
		mov		eax,work.tsl.direction

	; Check scan direction and use appropriate rasteriser
	;
		test	eax,eax
		jnz		reversed

		call	TrapeziumRender_PTI_I8_256_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_PTI_I8_256_f
		
		ret

reversed:

		call	TrapeziumRender_PTI_I8_256_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_PTI_I8_256_b
		
		ret

TriangleRender_PTI_I8_256 endp


TriangleRender_PT_I8_256 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.  Jump into the linear rasterizer in cheat cases.
	;
		call	TriangleSetup_PT
		jc		TriangleRasterise_T_I8_256

	; Calculate address of first scanline in colour buffer
	;
		mov		esi,work_main_y
		mov		ecx,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		imul	ebx,esi

		add		ecx,ebx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i

		dec		ecx
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		workspace.scanAddress,ecx

		ror		ebx,16
		mov		work_main_i,eax

		xor		eax,eax
		mov		work_main_d_i,ebx

		mov		al,byte ptr work.awsl.u_current
		mov		ebx,work.pq.grad_x

		mov		ah,byte ptr work.awsl.v_current
		mov		work.tsl.ddenominator,ebx

		mov		work.tsl.source,eax
		mov		eax,work.tsl.direction

	; Check scan direction and use appropriate rasteriser
	;
		test	eax,eax
		jnz		reversed

		call	TrapeziumRender_PT_I8_256_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_PT_I8_256_f
		
		ret

reversed:

		call	TrapeziumRender_PT_I8_256_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_PT_I8_256_b
		
		ret

TriangleRender_PT_I8_256 endp


TriangleRender_PTI_I8_1024_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_PTI_FLAT_NOCHEAT

		jmp		TriangleRasterise_PTI_I8_1024

TriangleRender_PTI_I8_1024_FLAT endp


TriangleRender_PTI_I8_1024 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_PTI_NOCHEAT

TriangleRasterise_PTI_I8_1024 label proc

	; Calculate address of first scanline in colour buffer
	;
		mov		esi,work_main_y
		mov		ecx,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		imul	ebx,esi

		add		ecx,ebx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i

		dec		ecx
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		workspace.scanAddress,ecx

		ror		ebx,16
		mov		edx,work_pi_grad_x

		mov		work_main_i,eax
		mov		eax,work.awsl.v_current

		and		eax,1023
		mov		work_main_d_i,ebx

		mov		ebx,work.awsl.u_current
		mov		work.tsl._di,edx

		shl		eax,10
		and		ebx,1023

		or		eax,ebx
		mov		ebx,work.pq.grad_x

		mov		work.tsl.source,eax
		mov		work.tsl.ddenominator,ebx

		mov		eax,work.tsl.direction

	; Check scan direction and use appropriate rasteriser
	;
		test	eax,eax
		jnz		reversed

		call	TrapeziumRender_PTI_I8_1024_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_PTI_I8_1024_f
		
		ret

reversed:

		call	TrapeziumRender_PTI_I8_1024_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_PTI_I8_1024_b
		
		ret

TriangleRender_PTI_I8_1024 endp


TriangleRender_PT_I8_1024 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_PT
		jc		TriangleRasterise_T_I8_1024

	; Calculate address of first scanline in colour buffer
	;
		mov		esi,work_main_y
		mov		ecx,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		imul	ebx,esi

		add		ecx,ebx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i

		dec		ecx
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		workspace.scanAddress,ecx

		ror		ebx,16
		mov		work_main_i,eax

		mov		work_main_d_i,ebx
		mov		eax,work.awsl.v_current

		and		eax,1023
		mov		ebx,work.awsl.u_current

		shl		eax,10
		and		ebx,1023

		or		eax,ebx
		mov		ebx,work.pq.grad_x

		mov		work.tsl.source,eax
		mov		work.tsl.ddenominator,ebx

		mov		eax,work.tsl.direction

	; Check scan direction and use appropriate rasteriser
	;
		test	eax,eax
		jnz		reversed

		call	TrapeziumRender_PT_I8_1024_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_PT_I8_1024_f
		
		ret

reversed:

		call	TrapeziumRender_PT_I8_1024_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_PT_I8_1024_b
		
		ret

TriangleRender_PT_I8_1024 endp


OPTION PROLOGUE:NONE
OPTION EPILOGUE:NONE


TrapeziumRender_PT_I8 32,f,\
	<shl eax,3>,\
	<add al,1000b>,<sub al,1000b>,<inc ah>,<dec ah>,\
	<shr eax,3>,<and eax,03ffh>
TrapeziumRender_PT_I8 32,b,\
	<shl eax,3>,\
	<add al,1000b>,<sub al,1000b>,<inc ah>,<dec ah>,\
	<shr eax,3>,<and eax,03ffh>

TrapeziumRender_PTI_I8 64,f,\
	<shl eax,2>,\
	<add al,100b>,<sub al,100b>,<inc ah>,<dec ah>,\
	<shr eax,2>,<and eax,0fffh>
TrapeziumRender_PTI_I8 64,b,\
	<shl eax,2>,\
	<add al,100b>,<sub al,100b>,<inc ah>,<dec ah>,\
	<shr eax,2>,<and eax,0fffh>

TrapeziumRender_PT_I8 64,f,\
	<shl eax,2>,\
	<add al,100b>,<sub al,100b>,<inc ah>,<dec ah>,\
	<shr eax,2>,<and eax,0fffh>
TrapeziumRender_PT_I8 64,b,\
	<shl eax,2>,\
	<add al,100b>,<sub al,100b>,<inc ah>,<dec ah>,\
	<shr eax,2>,<and eax,0fffh>

TrapeziumRender_PTI_I8 128,f,\
	<shl eax,1>,\
	<add al,10b>,<sub al,10b>,<inc ah>,<dec ah>,\
	<shr eax,1>,<and eax,3fffh>
TrapeziumRender_PTI_I8 128,b,\
	<shl eax,1>,\
	<add al,10b>,<sub al,10b>,<inc ah>,<dec ah>,\
	<shr eax,1>,<and eax,3fffh>

TrapeziumRender_PT_I8 128,f,\
	<shl eax,1>,\
	<add al,10b>,<sub al,10b>,<inc ah>,<dec ah>,\
	<shr eax,1>,<and eax,3fffh>
TrapeziumRender_PT_I8 128,b,\
	<shl eax,1>,\
	<add al,10b>,<sub al,10b>,<inc ah>,<dec ah>,\
	<shr eax,1>,<and eax,3fffh>

TrapeziumRender_PTI_I8 256,f,\
	<>,\
	<inc al>,<dec al>,<inc ah>,<dec ah>,\
	<>,<>
TrapeziumRender_PTI_I8 256,b,\
	<>,\
	<inc al>,<dec al>,<inc ah>,<dec ah>,\
	<>,<>

TrapeziumRender_PT_I8 256,f,\
	<>,\
	<inc al>,<dec al>,<inc ah>,<dec ah>,\
	<>,<>
TrapeziumRender_PT_I8 256,b,\
	<>,\
	<inc al>,<dec al>,<inc ah>,<dec ah>,\
	<>,<>

TrapeziumRender_PTI_I8 1024,f,\
	<shl eax,6>,\
	<add ax,40h>,<sub ax,40h>,<add eax,10000h>,<sub eax,10000h>,\
	<shr eax,6>,<and eax,0fffffh>
TrapeziumRender_PTI_I8 1024,b,\
	<shl eax,6>,\
	<add ax,40h>,<sub ax,40h>,<add eax,10000h>,<sub eax,10000h>,\
	<shr eax,6>,<and eax,0fffffh>

TrapeziumRender_PT_I8 1024,f,\
	<shl eax,6>,\
	<add ax,40h>,<sub ax,40h>,<add eax,10000h>,<sub eax,10000h>,\
	<shr eax,6>,<and eax,0fffffh>
TrapeziumRender_PT_I8 1024,b,\
	<shl eax,6>,\
	<add ax,40h>,<sub ax,40h>,<add eax,10000h>,<sub eax,10000h>,\
	<shr eax,6>,<and eax,0fffffh>

	endif

		end
