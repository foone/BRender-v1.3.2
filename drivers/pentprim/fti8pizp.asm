;; Copyright (c) 1992,1993-1995 Argonalt Technologies Limited. All rights reserved.
;;
;; $Id: fti8pizp.asm 1.1 1997/12/10 16:47:00 jon Exp $
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

        externdef c TriangleRasterise_ZPTI_I8_D16_32:proc
		externdef c TriangleRasterise_ZPTI_I8_D16_64:proc
		externdef c TriangleRasterise_ZPTI_I8_D16_128:proc
		externdef c TriangleRasterise_ZPTI_I8_D16_256:proc
		externdef c TriangleRasterise_ZPTI_I8_D16_1024:proc

        externdef c TriangleRasterise_ZPTIF_I8_D16_32:proc
		externdef c TriangleRasterise_ZPTIF_I8_D16_64:proc
		externdef c TriangleRasterise_ZPTIF_I8_D16_128:proc
		externdef c TriangleRasterise_ZPTIF_I8_D16_256:proc
		externdef c TriangleRasterise_ZPTIF_I8_D16_1024:proc

        externdef c TriangleRasterise_ZPTIB_I8_D16_32:proc
		externdef c TriangleRasterise_ZPTIB_I8_D16_64:proc
		externdef c TriangleRasterise_ZPTIB_I8_D16_128:proc
		externdef c TriangleRasterise_ZPTIB_I8_D16_256:proc
		externdef c TriangleRasterise_ZPTIB_I8_D16_1024:proc

        externdef c TriangleRasterise_ZTIB_I8_D16_32:proc
		externdef c TriangleRasterise_ZTIB_I8_D16_64:proc
		externdef c TriangleRasterise_ZTIB_I8_D16_128:proc
		externdef c TriangleRasterise_ZTIB_I8_D16_256:proc
		externdef c TriangleRasterise_ZTIB_I8_D16_1024:proc

        externdef c TriangleRasterise_ZTB_I8_D16_32:proc
		externdef c TriangleRasterise_ZTB_I8_D16_64:proc
		externdef c TriangleRasterise_ZTB_I8_D16_128:proc
		externdef c TriangleRasterise_ZTB_I8_D16_256:proc
		externdef c TriangleRasterise_ZTB_I8_D16_1024:proc

        externdef c TriangleRasterise_ZPTIFB_I8_D16_32:proc
		externdef c TriangleRasterise_ZPTIFB_I8_D16_64:proc
		externdef c TriangleRasterise_ZPTIFB_I8_D16_128:proc
		externdef c TriangleRasterise_ZPTIFB_I8_D16_256:proc
		externdef c TriangleRasterise_ZPTIFB_I8_D16_1024:proc

		externdef c TriangleRasterise_ZTI_I8_D16_256:proc
		externdef c TriangleRasterise_ZT_I8_D16_256:proc
        externdef c TriangleRasterise_ZTIF_I8_D16_256:proc
        externdef c TriangleRasterise_ZTF_I8_D16_256:proc

        externdef c TriangleRasterise_ZTI_I8_D16_32:proc
        externdef c TriangleRasterise_ZT_I8_D16_32:proc

		externdef c TriangleRasterise_ZTI_I8_D16_64:proc
        externdef c TriangleRasterise_ZT_I8_D16_64:proc

		externdef c TriangleRasterise_ZTI_I8_D16_128:proc
		externdef c TriangleRasterise_ZT_I8_D16_128:proc

		externdef c TriangleRasterise_ZTI_I8_D16_1024:proc
		externdef c TriangleRasterise_ZT_I8_D16_1024:proc

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

work_pz_current		equ	workspace.s_z
work_pz_grad_x		equ	workspace.d_z_x
work_pz_d_nocarry	equ	workspace.d_z_y_0
work_pz_d_carry		equ	workspace.d_z_y_1

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

work_pz_current		equ	work.pz.current		
work_pz_grad_x		equ	work.pz.grad_x		
work_pz_d_nocarry	equ	work.pz.d_nocarry	
work_pz_d_carry		equ	work.pz.d_carry		

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


TrapeziumRender_ZPTI_I8_D16 macro size,dirn,pre,incu,decu,incv,decv,post1,post2,fogging,blend

TrapeziumRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn proc

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
		mov		ebx,workspace.depthAddress

		add		edi,eax				; calculate end colour buffer pointer
		add		eax,ebp				; calculate start colour buffer pointer

		mov		work.tsl._end,edi
		lea		ebx,[ebx+ebp*2]		; calculate start depth buffer pointer

		mov		work.tsl.start,eax
		mov		work.tsl.zstart,ebx

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
		jle_&dirn	ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_ui_vd

		jmp		ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_ui_vi

qi_ud:

		test	edi,edi
		jle_&dirn	ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_ud_vd

		jmp		ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_ud_vi

qd:

		test	esi,esi
		jle_&dirn	qd_ud

qd_ui:

		test	edi,edi
		jg_&dirn	ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_ui_vi

		cmp		edi,ebp
		jle_&dirn	ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_ui_vd

		jmp		ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_ub_vb

qd_ud:

		cmp		esi,ebp
		jg_&dirn	ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_ub_vb

		test	edi,edi
		jg_&dirn	ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_ud_vi

		cmp		edi,ebp
		jle_&dirn	ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_ud_vd

		jmp		ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_ub_vb
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
        jmp     ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_table[edx * 4 + 2 * 4]
endif

	; Scanline loops return here when finished
	;
ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_done::

no_pixels:

	; Updates for next scanline:
	;
		mov		eax,workspace.scanAddress
		mov		ebx,work.colour.stride_b

		mov		ecx,workspace.depthAddress
		mov		edx,work.depth.stride_b

		add		eax,ebx				; move down one line in colour buffer
		add		ecx,edx				; move down one line in depth buffer

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

		mov		ebp,work_main_i
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

		mov		eax,work_pz_current
		mov		ebx,work_pi_current

		add		eax,work_pz_d_nocarry	; step z according to carry from major edge
		add		ebx,work_pi_d_nocarry	; step i according to carry from major edge

		mov		work_pz_current,eax
		mov		work_pi_current,ebx

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

		mov		eax,work_pz_current
		mov		ebx,work_pi_current

		add		eax,work_pz_d_carry	; step z according to carry from major edge
		add		ebx,work_pi_d_carry	; step i according to carry from major edge

		mov		work_pz_current,eax
		mov		work_pi_current,ebx

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

TrapeziumRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn endp

ifidni <dirn>,<f>
ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_table dd  ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_ud_vi
                                                        dd  ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_ui_vi
                                                        dd  ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_ud_vd
                                                        dd  ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_ui_vd
else
ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_table dd  ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_ui_vd
                                                        dd  ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_ud_vd
                                                        dd  ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_ui_vi
                                                        dd  ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_ud_vi
endif

ScanlineRender_ZPTI_I8_D16 size,dirn,d,d,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,<fogging>,<blend>
ScanlineRender_ZPTI_I8_D16 size,dirn,d,i,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,<fogging>,<blend>
ScanlineRender_ZPTI_I8_D16 size,dirn,i,d,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,<fogging>,<blend>
ScanlineRender_ZPTI_I8_D16 size,dirn,i,i,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,<fogging>,<blend>
ScanlineRender_ZPTI_I8_D16 size,dirn,b,b,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,<fogging>,<blend>

		endm


TrapeziumRender_ZPT_I8_D16 macro size,dirn,pre,incu,decu,incv,decv,post1,post2,fogging,blend

TrapeziumRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn proc

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
		mov		ebx,workspace.depthAddress

		add		edi,eax				; calculate end colour buffer pointer
		add		eax,ebp				; calculate start colour buffer pointer

		mov		work.tsl._end,edi
		lea		ebx,[ebx+ebp*2]		; calculate start depth buffer pointer

		mov		work.tsl.start,eax
		mov		work.tsl.zstart,ebx

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
		jle_&dirn	ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_ui_vd

		jmp		ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_ui_vi

qi_ud:

		test	edi,edi
		jle_&dirn	ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_ud_vd

		jmp		ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_ud_vi

qd:

		test	esi,esi
		jle_&dirn	qd_ud

qd_ui:

		test	edi,edi
		jg_&dirn	ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_ui_vi

		cmp		edi,ebp
		jle_&dirn	ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_ui_vd

		jmp		ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_ub_vb

qd_ud:

		cmp		esi,ebp
		jg_&dirn	ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_ub_vb

		test	edi,edi
		jg_&dirn	ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_ud_vi

		cmp		edi,ebp
		jle_&dirn	ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_ud_vd

		jmp		ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_ub_vb
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
        jmp     ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_table[edx * 4 + 2 * 4]
endif

	; Scanline loops return here when finished
	;
ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_done::

no_pixels:

	; Updates for next scanline:
	;
		mov		eax,workspace.scanAddress
		mov		ebx,work.colour.stride_b

		mov		ecx,workspace.depthAddress
		mov		edx,work.depth.stride_b

		add		eax,ebx				; move down one line in colour buffer
		add		ecx,edx				; move down one line in depth buffer

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

		mov		ebp,work_main_i
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

		mov		eax,work_pz_current
		mov		ebx,work_pz_d_nocarry

		add		eax,ebx				; step z according to carry from major edge
		mov		ebx,work.pv.current

		mov		work_pz_current,eax
		mov		eax,work.pu.current

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

		mov		eax,work_pz_current
		mov		ebx,work_pz_d_carry

		add		eax,ebx				; step z according to carry from major edge
		mov		ebx,work.pv.current

		mov		work_pz_current,eax
		mov		eax,work.pu.current

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

TrapeziumRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn endp

ifidni <dirn>,<f>
ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_table  dd  ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_ud_vi
                                                        dd  ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_ui_vi
                                                        dd  ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_ud_vd
                                                        dd  ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_ui_vd
else
ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_table  dd  ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_ui_vd
                                                        dd  ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_ud_vd
                                                        dd  ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_ui_vi
                                                        dd  ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_ud_vi
endif

ScanlineRender_ZPT_I8_D16 size,dirn,d,d,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,<fogging>,<blend>
ScanlineRender_ZPT_I8_D16 size,dirn,d,i,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,<fogging>,<blend>
ScanlineRender_ZPT_I8_D16 size,dirn,i,d,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,<fogging>,<blend>
ScanlineRender_ZPT_I8_D16 size,dirn,i,i,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,<fogging>,<blend>
ScanlineRender_ZPT_I8_D16 size,dirn,b,b,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,<fogging>,<blend>

		endm


ScanlineRender_ZPTI_I8_D16 macro size,dirn,udirn,vdirn,pre,incu,decu,incv,decv,post1,post2,fogging,blend

ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_u&udirn&_v&vdirn proc

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
		mov		ebp,work.tsl.zstart

		mov		ebx,work_pz_current
		mov		edx,work_pi_current

		ror		ebx,16					; Swap z words
		mov		work.tsl.i,edx

		mov		work.tsl.z,ebx
		mov		edx,work.pq.current

		mov		work.tsl.denominator,edx

next_pixel:

	; Texel fetch and store section
	;
	; eax = source offset
	; ebx = new z value
	; ecx = texel
	; edx = old z value, shade table
	; esi = texture base
	; edi = dest
	; ebp = zdest

	; Perform z buffer test, get texel
	;
		mov		dx,[ebp]
		xor		ecx,ecx

		mov		ebx,work.tsl.z
		mov		cl,[eax+esi]

		cmp		bx,dx
		ja		nodraw

	; Get intensity
	;
		mov		edx,work.shade_table
		mov		ch,byte ptr (work.tsl.i+2)

	; Test for transparency
	;
if TRANSPARENCY
		test	cl,cl
		jz		nodraw
endif

ifidni <fogging>,<F>
ifidni <blend>,<B>
    ; Look texel up in shade table, fog table, blend table, store texel
	;
        mov     cl,[ecx+edx]
        mov     edx,work.fog_table

        mov     ch,bh

    ;AGI stall here
        mov     cl,[ecx+edx]
        mov     edx,work.blend_table

        mov     ch,[edi]

    ;and here
        mov     cl,[edx+ecx]

        mov     [edi],cl
        nop
else
    ; Look texel up in shade table + fog table, store texel and z
	;
        mov     cl,[ecx+edx]
        mov     edx,work.fog_table

        mov     ch,bh
        mov     [ebp],bl

    ;AGI stall here...
        mov     cl,[ecx+edx]

        mov     [edi],cl
        mov     [ebp+1],bh
endif
else
ifidni <blend>,<B>
    ; Look texel up in shade table + blend table, store texel
	;
        mov     cl,[ecx+edx]
        mov     edx,work.blend_table

        mov     ch,[edi]

    ;AGI stall here...
        mov     cl,[ecx+edx]

        mov     [edi],cl
        nop
else
	; Look texel up in shade table, store texel and z
    ;
        mov     [ebp],bl
        mov     cl,[ecx+edx]

        mov     [ebp+1],bh
        mov     [edi],cl
endif
endif

nodraw:

	; Linear interpolation section
	;
	; eax = 
	; ebx = z
	; ecx = dz, i
	; edx = di
	; esi = 
	; edi = dest
	; ebp = zdest

	; Prepare source offset for modification
	;
		pre
		mov		ecx,work.tsl._end

	; Update destinations and check for end of scan
	;
		inc_&dirn	edi
		add_&dirn	ebp,2

		cmp		edi,ecx
        jg_&dirn    ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_done

	; Interpolate z and i
	;
		mov		ecx,work.tsl.dz
		mov		edx,work.tsl._di

		add_&dirn	ebx,ecx
		mov		ecx,work.tsl.i

		adc_&dirn	ebx,0		; carry into integer part of z
		add_&dirn	ecx,edx

		mov		work.tsl.dest,edi
		mov		work.tsl.zdest,ebp

		mov		work.tsl.z,ebx
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
		mov		ebp,work.tsl.zdest

		mov		edi,work.tsl.dest
		jmp		next_pixel

ScanlineRender_ZPTI&fogging&&blend&_I8_D16_&size&_&dirn&_u&udirn&_v&vdirn endp

		endm


ScanlineRender_ZPT_I8_D16 macro size,dirn,udirn,vdirn,pre,incu,decu,incv,decv,post1,post2,fogging,blend

ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_u&udirn&_v&vdirn proc

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
		mov		ebp,work.tsl.zstart

		mov		ebx,work_pz_current
		mov		edx,work.pq.current

		ror		ebx,16					; Swap z words
		mov		work.tsl.denominator,edx

		mov		work.tsl.z,ebx
		mov		work.tsl.dest,edi

next_pixel:

	; Texel fetch and store section
	;
	; eax = source offset
	; ebx = new z value
	; ecx = texel
	; edx = old z value, shade table
	; esi = texture base
	; edi = dest
	; ebp = zdest

	; Perform z buffer test and get texel
	;
		mov		dl,[ebp]
		mov		cl,[eax+esi]

		mov		dh,[ebp+1]
		mov		edi,work.tsl.dest

		cmp		bx,dx
		ja		nodraw

	; Test for transparency
	;
if TRANSPARENCY
		test	cl,cl
		jz		nodraw
endif

ifidni <fogging>,<F>
ifidni <blend>,<B>
    ; Look texel up in fog table + blend table, store texel
	;
        and     ecx,0ffh
        mov     edx,work.fog_table

        mov     ch,bh

    ;AGI stall
        mov     cl,[edx+ecx]
        mov     edx,work.blend_table

        mov     ch,[edi]

    ;and another
        mov     cl,[edx+ecx]

        mov     [edi],cl
        nop
else
    ; Look texel up in fog table, store texel and z
	;
        and     ecx,0ffh
        mov     edx,work.fog_table

        mov     ch,bh
        mov     [ebp],bl

        mov     [ebp+1],bh
    ;AGI stall
        mov     cl,[edx+ecx]

        mov     [edi],cl
        nop
endif
else
ifidni <blend>,<B>
    ; Look texel up in blend table, store texel
	;
        and     ecx,0ffh
        mov     edx,work.blend_table

        mov     ch,[edi]

    ;AGI stall
        mov     cl,[edx+ecx]

        mov     [edi],cl
else
	; Store texel and z
	;
        mov     [ebp],bx
        mov     [edi],cl
endif
endif

nodraw:

	; Linear interpolation section
	;
	; eax = 
	; ebx = z
	; ecx = dz
	; edx = 
	; esi = 
	; edi = dest
	; ebp = zdest

	; Prepare source offset for modification
	;
		pre
		mov		ecx,work.tsl._end

	; Update destinations and check for end of scan
	;
		inc_&dirn	edi
		add_&dirn	ebp,2

		cmp		edi,ecx
        jg_&dirn    ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_done

	; Interpolate z
	;
		mov		ecx,work.tsl.dz
		mov		work.tsl.dest,edi

		add_&dirn	ebx,ecx
		mov		work.tsl.zdest,ebp

		adc_&dirn	ebx,0		; carry into integer part of z

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

		mov		work.tsl.z,ebx
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
		mov	work.tsl.v_numerator,ecx

		post2
		mov		ebp,work.tsl.zdest

		mov		ebx,work.tsl.z
		jmp		next_pixel

ScanlineRender_ZPT&fogging&&blend&_I8_D16_&size&_&dirn&_u&udirn&_v&vdirn endp

		endm


		.code

		OPTION PROLOGUE:PROLOGUE 
		OPTION EPILOGUE:EPILOGUE


	if PARTS and PART_8Z



TriangleRender_ZPTI_I8_D16_32_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPTI_FLAT

                jc              TriangleRasterise_ZTI_I8_D16_32
                jmp             TriangleRasterise_ZPTI_I8_D16_32

TriangleRender_ZPTI_I8_D16_32_FLAT endp


TriangleRender_ZPTI_I8_D16_32 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPTI
                jc              TriangleRasterise_ZTI_I8_D16_32

TriangleRasterise_ZPTI_I8_D16_32 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		edx,work_pi_grad_x

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		work_main_i,eax

		mov		al,byte ptr work.awsl.u_current
		mov		work_main_d_i,ebx

		mov		ah,byte ptr work.awsl.v_current
		mov		work.tsl.dz,ecx

                shl             al,3
		mov		work.tsl._di,edx

                shr             eax,3
		mov		ebx,work.pq.grad_x

                and             eax,31*33
		mov		work.tsl.ddenominator,ebx

		mov		work.tsl.source,eax
		mov		eax,work.tsl.direction

	; Check scan direction and use appropriate rasteriser
	;
		test	eax,eax
		jnz		reversed

        call    TrapeziumRender_ZPTI_I8_D16_32_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTI_I8_D16_32_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTI_I8_D16_32_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTI_I8_D16_32_b
		
		ret

TriangleRender_ZPTI_I8_D16_32 endp


TriangleRender_ZPT_I8_D16_32 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPT
                jc TriangleRasterise_ZT_I8_D16_32

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		work_main_i,eax

		ror		ecx,16
		mov		al,byte ptr work.awsl.u_current

		mov		ah,byte ptr work.awsl.v_current
		mov		work_main_d_i,ebx

                shl             al,3
		mov		work.tsl.dz,ecx

                shr             eax,3
		mov		ebx,work.pq.grad_x

                and             eax,31*33
		mov		work.tsl.ddenominator,ebx

		mov		work.tsl.source,eax
		mov		eax,work.tsl.direction

	; Check scan direction and use appropriate rasteriser
	;
		test	eax,eax
		jnz		reversed

        call    TrapeziumRender_ZPT_I8_D16_32_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPT_I8_D16_32_f
		
		ret

reversed:

        call    TrapeziumRender_ZPT_I8_D16_32_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPT_I8_D16_32_b
		
		ret

TriangleRender_ZPT_I8_D16_32 endp

TriangleRender_ZPTI_I8_D16_64_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPTI_FLAT

		jc		TriangleRasterise_ZTI_I8_D16_64
		jmp		TriangleRasterise_ZPTI_I8_D16_64

TriangleRender_ZPTI_I8_D16_64_FLAT endp


TriangleRender_ZPTI_I8_D16_64 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPTI
		jc		TriangleRasterise_ZTI_I8_D16_64

TriangleRasterise_ZPTI_I8_D16_64 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		edx,work_pi_grad_x

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		work_main_i,eax

		mov		al,byte ptr work.awsl.u_current
		mov		work_main_d_i,ebx

		mov		ah,byte ptr work.awsl.v_current
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTI_I8_D16_64_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTI_I8_D16_64_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTI_I8_D16_64_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTI_I8_D16_64_b
		
		ret

TriangleRender_ZPTI_I8_D16_64 endp


TriangleRender_ZPT_I8_D16_64 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPT
		jc TriangleRasterise_ZT_I8_D16_64

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		work_main_i,eax

		ror		ecx,16
		mov		al,byte ptr work.awsl.u_current

		mov		ah,byte ptr work.awsl.v_current
		mov		work_main_d_i,ebx

		shl		al,2
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPT_I8_D16_64_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPT_I8_D16_64_f
		
		ret

reversed:

        call    TrapeziumRender_ZPT_I8_D16_64_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPT_I8_D16_64_b
		
		ret

TriangleRender_ZPT_I8_D16_64 endp


TriangleRender_ZPTI_I8_D16_128_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPTI_FLAT
		jc		TriangleRasterise_ZTI_I8_D16_128

		jmp		TriangleRasterise_ZPTI_I8_D16_128

TriangleRender_ZPTI_I8_D16_128_FLAT endp


TriangleRender_ZPTI_I8_D16_128 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPTI
		jc		TriangleRasterise_ZTI_I8_D16_128

TriangleRasterise_ZPTI_I8_D16_128 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		edx,work_pi_grad_x

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		work_main_i,eax

		mov		al,byte ptr work.awsl.u_current
		mov		work_main_d_i,ebx

		mov		ah,byte ptr work.awsl.v_current
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTI_I8_D16_128_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTI_I8_D16_128_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTI_I8_D16_128_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTI_I8_D16_128_b
		
		ret

TriangleRender_ZPTI_I8_D16_128 endp


TriangleRender_ZPT_I8_D16_128 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPT
		jc TriangleRasterise_ZT_I8_D16_128

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		work_main_i,eax

		ror		ecx,16
		mov		al,byte ptr work.awsl.u_current

		mov		ah,byte ptr work.awsl.v_current
		mov		work_main_d_i,ebx

		shl		al,1
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPT_I8_D16_128_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPT_I8_D16_128_f
		
		ret

reversed:

		call	TrapeziumRender_ZPT_I8_D16_128_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_ZPT_I8_D16_128_b
		
		ret

TriangleRender_ZPT_I8_D16_128 endp


TriangleRender_ZPTI_I8_D16_256_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.  Jump into the linear rasterizer in cheat cases.
	;
        call    TriangleSetup_ZPTI_FLAT
        jc      TriangleRasterise_ZTI_I8_D16_256

		jmp		TriangleRasterise_ZPTI_I8_D16_256

TriangleRender_ZPTI_I8_D16_256_FLAT endp


TriangleRender_ZPTI_I8_D16_256 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.  Jump into the linear rasterizer in cheat cases.
	;
        call    TriangleSetup_ZPTI
        jc      TriangleRasterise_ZTI_I8_D16_256

TriangleRasterise_ZPTI_I8_D16_256 label proc

	; Calculate address of first scanline in cone in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		edx,work_pi_grad_x

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		work_main_i,eax

		xor		eax,eax
		mov		work_main_d_i,ebx

		mov		al,byte ptr work.awsl.u_current
		mov		work.tsl.dz,ecx

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

		call	TrapeziumRender_ZPTI_I8_D16_256_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_ZPTI_I8_D16_256_f
		
		ret

reversed:

		call	TrapeziumRender_ZPTI_I8_D16_256_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_ZPTI_I8_D16_256_b
		
		ret

TriangleRender_ZPTI_I8_D16_256 endp


TriangleRender_ZPT_I8_D16_256 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.  Jump into the linear rasterizer in cheat cases.
	;
		call	TriangleSetup_ZPT
		jc		TriangleRasterise_ZT_I8_D16_256

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		work_main_i,eax

		ror		ecx,16
		mov		work_main_d_i,ebx

		xor		eax,eax
		mov		work.tsl.dz,ecx

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

		call	TrapeziumRender_ZPT_I8_D16_256_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_ZPT_I8_D16_256_f
		
		ret

reversed:

		call	TrapeziumRender_ZPT_I8_D16_256_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_ZPT_I8_D16_256_b
		
		ret

TriangleRender_ZPT_I8_D16_256 endp


TriangleRender_ZPTI_I8_D16_1024_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPTI_FLAT
		jc		TriangleRasterise_ZTI_I8_D16_1024

		jmp		TriangleRasterise_ZPTI_I8_D16_1024

TriangleRender_ZPTI_I8_D16_1024_FLAT endp


TriangleRender_ZPTI_I8_D16_1024 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPTI
		jc		TriangleRasterise_ZTI_I8_D16_1024

TriangleRasterise_ZPTI_I8_D16_1024 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		edx,work_pi_grad_x

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		work_main_i,eax

		mov		eax,work.awsl.v_current
		mov		work_main_d_i,ebx

		and		eax,1023
		mov		work.tsl.dz,ecx

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

		call	TrapeziumRender_ZPTI_I8_D16_1024_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_ZPTI_I8_D16_1024_f
		
		ret

reversed:

		call	TrapeziumRender_ZPTI_I8_D16_1024_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_ZPTI_I8_D16_1024_b
		
		ret

TriangleRender_ZPTI_I8_D16_1024 endp


TriangleRender_ZPT_I8_D16_1024 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPT
		jc TriangleRasterise_ZT_I8_D16_1024
	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		work_main_i,eax

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		eax,work.awsl.v_current

		mov		work_main_d_i,ebx
		and		eax,1023

		mov		work.tsl.dz,ecx
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

		call	TrapeziumRender_ZPT_I8_D16_1024_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_ZPT_I8_D16_1024_f
		
		ret

reversed:

		call	TrapeziumRender_ZPT_I8_D16_1024_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
		call	TrapeziumRender_ZPT_I8_D16_1024_b
		
		ret

TriangleRender_ZPT_I8_D16_1024 endp

	endif

	if PARTS and PART_8Z_FOG




TriangleRender_ZPTIF_I8_D16_32_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
        call    TriangleSetup_ZPTI_FLAT_NOCHEAT

                jmp             TriangleRasterise_ZPTIF_I8_D16_32

TriangleRender_ZPTIF_I8_D16_32_FLAT endp


TriangleRender_ZPTIF_I8_D16_32 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
        call    TriangleSetup_ZPTI_NOCHEAT

TriangleRasterise_ZPTIF_I8_D16_32 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		edx,work_pi_grad_x

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		work_main_i,eax

		mov		al,byte ptr work.awsl.u_current
		mov		work_main_d_i,ebx

		mov		ah,byte ptr work.awsl.v_current
		mov		work.tsl.dz,ecx

                shl             al,3
		mov		work.tsl._di,edx

                shr             eax,3
		mov		ebx,work.pq.grad_x

                and             eax,31*33
		mov		work.tsl.ddenominator,ebx

		mov		work.tsl.source,eax
		mov		eax,work.tsl.direction

	; Check scan direction and use appropriate rasteriser
	;
		test	eax,eax
		jnz		reversed

        call    TrapeziumRender_ZPTIF_I8_D16_32_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIF_I8_D16_32_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTIF_I8_D16_32_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIF_I8_D16_32_b
		
		ret

TriangleRender_ZPTIF_I8_D16_32 endp


TriangleRender_ZPTF_I8_D16_32 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPT_NOCHEAT

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		work_main_i,eax

		ror		ecx,16
		mov		al,byte ptr work.awsl.u_current

		mov		ah,byte ptr work.awsl.v_current
		mov		work_main_d_i,ebx

                shl             al,3
		mov		work.tsl.dz,ecx

                shr             eax,3
		mov		ebx,work.pq.grad_x

                and             eax,31*33
		mov		work.tsl.ddenominator,ebx

		mov		work.tsl.source,eax
		mov		eax,work.tsl.direction

	; Check scan direction and use appropriate rasteriser
	;
		test	eax,eax
		jnz		reversed

        call    TrapeziumRender_ZPTF_I8_D16_32_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTF_I8_D16_32_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTF_I8_D16_32_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTF_I8_D16_32_b
		
		ret

TriangleRender_ZPTF_I8_D16_32 endp

TriangleRender_ZPTIF_I8_D16_64_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
        call    TriangleSetup_ZPTI_FLAT_NOCHEAT

		jmp		TriangleRasterise_ZPTIF_I8_D16_64

TriangleRender_ZPTIF_I8_D16_64_FLAT endp


TriangleRender_ZPTIF_I8_D16_64 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
        call    TriangleSetup_ZPTI_NOCHEAT

TriangleRasterise_ZPTIF_I8_D16_64 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		edx,work_pi_grad_x

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		work_main_i,eax

		mov		al,byte ptr work.awsl.u_current
		mov		work_main_d_i,ebx

		mov		ah,byte ptr work.awsl.v_current
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTIF_I8_D16_64_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIF_I8_D16_64_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTIF_I8_D16_64_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIF_I8_D16_64_b
		
		ret

TriangleRender_ZPTIF_I8_D16_64 endp


TriangleRender_ZPTF_I8_D16_64 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPT_NOCHEAT

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		work_main_i,eax

		ror		ecx,16
		mov		al,byte ptr work.awsl.u_current

		mov		ah,byte ptr work.awsl.v_current
		mov		work_main_d_i,ebx

		shl		al,2
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTF_I8_D16_64_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTF_I8_D16_64_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTF_I8_D16_64_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTF_I8_D16_64_b
		
		ret

TriangleRender_ZPTF_I8_D16_64 endp


TriangleRender_ZPTIF_I8_D16_128_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPTI_FLAT_NOCHEAT

		jmp		TriangleRasterise_ZPTIF_I8_D16_128

TriangleRender_ZPTIF_I8_D16_128_FLAT endp


TriangleRender_ZPTIF_I8_D16_128 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPTI_NOCHEAT

TriangleRasterise_ZPTIF_I8_D16_128 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		edx,work_pi_grad_x

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		work_main_i,eax

		mov		al,byte ptr work.awsl.u_current
		mov		work_main_d_i,ebx

		mov		ah,byte ptr work.awsl.v_current
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTIF_I8_D16_128_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIF_I8_D16_128_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTIF_I8_D16_128_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIF_I8_D16_128_b
		
		ret

TriangleRender_ZPTIF_I8_D16_128 endp


TriangleRender_ZPTF_I8_D16_128 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPT_NOCHEAT

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		work_main_i,eax

		ror		ecx,16
		mov		al,byte ptr work.awsl.u_current

		mov		ah,byte ptr work.awsl.v_current
		mov		work_main_d_i,ebx

		shl		al,1
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTF_I8_D16_128_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTF_I8_D16_128_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTF_I8_D16_128_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTF_I8_D16_128_b
		
		ret

TriangleRender_ZPTF_I8_D16_128 endp


TriangleRender_ZPTIF_I8_D16_256_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.  Jump into the linear rasterizer in cheat cases.
	;
        call    TriangleSetup_ZPTI_FLAT
        jc      TriangleRasterise_ZTIF_I8_D16_256

		jmp		TriangleRasterise_ZPTIF_I8_D16_256

TriangleRender_ZPTIF_I8_D16_256_FLAT endp


TriangleRender_ZPTIF_I8_D16_256 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.  Jump into the linear rasterizer in cheat cases.
	;
        call    TriangleSetup_ZPTI
        jc      TriangleRasterise_ZTIF_I8_D16_256

TriangleRasterise_ZPTIF_I8_D16_256 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		edx,work_pi_grad_x

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		work_main_i,eax

		xor		eax,eax
		mov		work_main_d_i,ebx

		mov		al,byte ptr work.awsl.u_current
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTIF_I8_D16_256_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIF_I8_D16_256_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTIF_I8_D16_256_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIF_I8_D16_256_b
		
		ret

TriangleRender_ZPTIF_I8_D16_256 endp


TriangleRender_ZPTF_I8_D16_256 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.  Jump into the linear rasterizer in cheat cases.
	;
		call	TriangleSetup_ZPT
        jc      TriangleRasterise_ZTF_I8_D16_256

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		work_main_i,eax

		ror		ecx,16
		mov		work_main_d_i,ebx

		xor		eax,eax
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTF_I8_D16_256_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTF_I8_D16_256_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTF_I8_D16_256_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTF_I8_D16_256_b
		
		ret

TriangleRender_ZPTF_I8_D16_256 endp


TriangleRender_ZPTIF_I8_D16_1024_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPTI_FLAT_NOCHEAT

		jmp		TriangleRasterise_ZPTIF_I8_D16_1024

TriangleRender_ZPTIF_I8_D16_1024_FLAT endp


TriangleRender_ZPTIF_I8_D16_1024 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPTI_NOCHEAT

TriangleRasterise_ZPTIF_I8_D16_1024 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		edx,work_pi_grad_x

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		work_main_i,eax

		mov		eax,work.awsl.v_current
		mov		work_main_d_i,ebx

		and		eax,1023
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTIF_I8_D16_1024_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIF_I8_D16_1024_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTIF_I8_D16_1024_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIF_I8_D16_1024_b
		
		ret

TriangleRender_ZPTIF_I8_D16_1024 endp


TriangleRender_ZPTF_I8_D16_1024 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPT_NOCHEAT

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		work_main_i,eax

		ror		ecx,16
		mov		eax,work.awsl.v_current

		mov		work_main_d_i,ebx
		and		eax,1023

		mov		work.tsl.dz,ecx
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

        call    TrapeziumRender_ZPTF_I8_D16_1024_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTF_I8_D16_1024_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTF_I8_D16_1024_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTF_I8_D16_1024_b
		
		ret

TriangleRender_ZPTF_I8_D16_1024 endp

	endif

	if PARTS and PART_8Z_BLEND

TriangleRender_ZPTIB_I8_D16_32_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
        call    TriangleSetup_ZPTI_FLAT

		jc		TriangleRasterise_ZTIB_I8_D16_32

        jmp     TriangleRasterise_ZPTIB_I8_D16_32

TriangleRender_ZPTIB_I8_D16_32_FLAT endp


TriangleRender_ZPTIB_I8_D16_32 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
        call    TriangleSetup_ZPTI
		jc		TriangleRasterise_ZTIB_I8_D16_32

TriangleRasterise_ZPTIB_I8_D16_32 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		edx,work_pi_grad_x

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		work_main_i,eax

		mov		al,byte ptr work.awsl.u_current
		mov		work_main_d_i,ebx

		mov		ah,byte ptr work.awsl.v_current
		mov		work.tsl.dz,ecx

                shl             al,3
		mov		work.tsl._di,edx

                shr             eax,3
		mov		ebx,work.pq.grad_x

                and             eax,31*33
		mov		work.tsl.ddenominator,ebx

		mov		work.tsl.source,eax
		mov		eax,work.tsl.direction

	; Check scan direction and use appropriate rasteriser
	;
		test	eax,eax
		jnz		reversed

        call    TrapeziumRender_ZPTIB_I8_D16_32_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIB_I8_D16_32_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTIB_I8_D16_32_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIB_I8_D16_32_b
		
		ret

TriangleRender_ZPTIB_I8_D16_32 endp


TriangleRender_ZPTB_I8_D16_32 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPT

		jc TriangleRasterise_ZTB_I8_D16_32

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		work_main_i,eax

		ror		ecx,16
		mov		al,byte ptr work.awsl.u_current

		mov		ah,byte ptr work.awsl.v_current
		mov		work_main_d_i,ebx

                shl             al,3
		mov		work.tsl.dz,ecx

                shr             eax,3
		mov		ebx,work.pq.grad_x

                and             eax,31*33
		mov		work.tsl.ddenominator,ebx

		mov		work.tsl.source,eax
		mov		eax,work.tsl.direction

	; Check scan direction and use appropriate rasteriser
	;
		test	eax,eax
		jnz		reversed

        call    TrapeziumRender_ZPTB_I8_D16_32_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTB_I8_D16_32_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTB_I8_D16_32_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTB_I8_D16_32_b
		
		ret

TriangleRender_ZPTB_I8_D16_32 endp

TriangleRender_ZPTIB_I8_D16_64_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
        call    TriangleSetup_ZPTI_FLAT
		
		jc	TriangleRasterise_ZTIB_I8_D16_64

		jmp		TriangleRasterise_ZPTIB_I8_D16_64

TriangleRender_ZPTIB_I8_D16_64_FLAT endp


TriangleRender_ZPTIB_I8_D16_64 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
        call    TriangleSetup_ZPTI

		jc TriangleRasterise_ZTIB_I8_D16_64

TriangleRasterise_ZPTIB_I8_D16_64 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		edx,work_pi_grad_x

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		work_main_i,eax

		mov		al,byte ptr work.awsl.u_current
		mov		work_main_d_i,ebx

		mov		ah,byte ptr work.awsl.v_current
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTIB_I8_D16_64_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIB_I8_D16_64_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTIB_I8_D16_64_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIB_I8_D16_64_b
		
		ret

TriangleRender_ZPTIB_I8_D16_64 endp


TriangleRender_ZPTB_I8_D16_64 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPT
		jc TriangleRasterise_ZTB_I8_D16_64

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		work_main_i,eax

		ror		ecx,16
		mov		al,byte ptr work.awsl.u_current

		mov		ah,byte ptr work.awsl.v_current
		mov		work_main_d_i,ebx

		shl		al,2
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTB_I8_D16_64_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTB_I8_D16_64_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTB_I8_D16_64_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTB_I8_D16_64_b
		
		ret

TriangleRender_ZPTB_I8_D16_64 endp


TriangleRender_ZPTIB_I8_D16_128_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPTI_FLAT

		jc	TriangleRasterise_ZTIB_I8_D16_128
		jmp		TriangleRasterise_ZPTIB_I8_D16_128

TriangleRender_ZPTIB_I8_D16_128_FLAT endp


TriangleRender_ZPTIB_I8_D16_128 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZTI
		jc	TriangleRasterise_ZTIB_I8_D16_128

TriangleRasterise_ZPTIB_I8_D16_128 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		edx,work_pi_grad_x

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		work_main_i,eax

		mov		al,byte ptr work.awsl.u_current
		mov		work_main_d_i,ebx

		mov		ah,byte ptr work.awsl.v_current
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTIB_I8_D16_128_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIB_I8_D16_128_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTIB_I8_D16_128_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIB_I8_D16_128_b
		
		ret

TriangleRender_ZPTIB_I8_D16_128 endp


TriangleRender_ZPTB_I8_D16_128 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPT
		jc	TriangleRasterise_ZTB_I8_D16_128

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		work_main_i,eax

		ror		ecx,16
		mov		al,byte ptr work.awsl.u_current

		mov		ah,byte ptr work.awsl.v_current
		mov		work_main_d_i,ebx

		shl		al,1
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTB_I8_D16_128_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTB_I8_D16_128_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTB_I8_D16_128_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTB_I8_D16_128_b
		
		ret

TriangleRender_ZPTB_I8_D16_128 endp


TriangleRender_ZPTIB_I8_D16_256_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.  Jump into the linear rasterizer in cheat cases.
	;
        call    TriangleSetup_ZPTI_FLAT
		jc		TriangleRasterise_ZTIB_I8_D16_256

		jmp		TriangleRasterise_ZPTIB_I8_D16_256

TriangleRender_ZPTIB_I8_D16_256_FLAT endp


TriangleRender_ZPTIB_I8_D16_256 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.  Jump into the linear rasterizer in cheat cases.
	;
        call    TriangleSetup_ZPTI
		jc		TriangleRasterise_ZTIB_I8_D16_256

TriangleRasterise_ZPTIB_I8_D16_256 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		edx,work_pi_grad_x

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		work_main_i,eax

		xor		eax,eax
		mov		work_main_d_i,ebx

		mov		al,byte ptr work.awsl.u_current
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTIB_I8_D16_256_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIB_I8_D16_256_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTIB_I8_D16_256_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIB_I8_D16_256_b
		
		ret

TriangleRender_ZPTIB_I8_D16_256 endp


TriangleRender_ZPTB_I8_D16_256 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.  Jump into the linear rasterizer in cheat cases.
	;
        call    TriangleSetup_ZPT
		jc		TriangleRasterise_ZTB_I8_D16_256

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		work_main_i,eax

		ror		ecx,16
		mov		work_main_d_i,ebx

		xor		eax,eax
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTB_I8_D16_256_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTB_I8_D16_256_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTB_I8_D16_256_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTB_I8_D16_256_b
		
		ret

TriangleRender_ZPTB_I8_D16_256 endp


TriangleRender_ZPTIB_I8_D16_1024_FLAT proc uses eax ebx ecx edx esi edi,
        pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPTI_FLAT
		jc		TriangleRasterise_ZTIB_I8_D16_1024

		jmp		TriangleRasterise_ZPTIB_I8_D16_1024

TriangleRender_ZPTIB_I8_D16_1024_FLAT endp


TriangleRender_ZPTIB_I8_D16_1024 proc uses eax ebx ecx edx esi edi,
        pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPTI
		jc		TriangleRasterise_ZTIB_I8_D16_1024

TriangleRasterise_ZPTIB_I8_D16_1024 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		edx,work_pi_grad_x

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		work_main_i,eax

		mov		eax,work.awsl.v_current
		mov		work_main_d_i,ebx

		and		eax,1023
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTIB_I8_D16_1024_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIB_I8_D16_1024_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTIB_I8_D16_1024_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIB_I8_D16_1024_b
		
		ret

TriangleRender_ZPTIB_I8_D16_1024 endp


TriangleRender_ZPTB_I8_D16_1024 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPT
		jc		TriangleRasterise_ZTB_I8_D16_1024

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		work_main_i,eax

		ror		ecx,16
		mov		eax,work.awsl.v_current

		mov		work_main_d_i,ebx
		and		eax,1023

		mov		work.tsl.dz,ecx
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

        call    TrapeziumRender_ZPTB_I8_D16_1024_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTB_I8_D16_1024_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTB_I8_D16_1024_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTB_I8_D16_1024_b
		
		ret

TriangleRender_ZPTB_I8_D16_1024 endp


	endif

	if PARTS and PART_8Z_FOG_BLEND


TriangleRender_ZPTIFB_I8_D16_32_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
        call    TriangleSetup_ZPTI_FLAT_NOCHEAT

                jmp             TriangleRasterise_ZPTIFB_I8_D16_32

TriangleRender_ZPTIFB_I8_D16_32_FLAT endp


TriangleRender_ZPTIFB_I8_D16_32 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
        call    TriangleSetup_ZPTI_NOCHEAT

TriangleRasterise_ZPTIFB_I8_D16_32 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		edx,work_pi_grad_x

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		work_main_i,eax

		mov		al,byte ptr work.awsl.u_current
		mov		work_main_d_i,ebx

		mov		ah,byte ptr work.awsl.v_current
		mov		work.tsl.dz,ecx

                shl             al,3
		mov		work.tsl._di,edx

                shr             eax,3
		mov		ebx,work.pq.grad_x

                and             eax,31*33
		mov		work.tsl.ddenominator,ebx

		mov		work.tsl.source,eax
		mov		eax,work.tsl.direction

	; Check scan direction and use appropriate rasteriser
	;
		test	eax,eax
		jnz		reversed

        call    TrapeziumRender_ZPTIFB_I8_D16_32_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIFB_I8_D16_32_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTIFB_I8_D16_32_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIFB_I8_D16_32_b
		
		ret

TriangleRender_ZPTIFB_I8_D16_32 endp


TriangleRender_ZPTFB_I8_D16_32 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPT_NOCHEAT

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		work_main_i,eax

		ror		ecx,16
		mov		al,byte ptr work.awsl.u_current

		mov		ah,byte ptr work.awsl.v_current
		mov		work_main_d_i,ebx

                shl             al,3
		mov		work.tsl.dz,ecx

                shr             eax,3
		mov		ebx,work.pq.grad_x

                and             eax,31*33
		mov		work.tsl.ddenominator,ebx

		mov		work.tsl.source,eax
		mov		eax,work.tsl.direction

	; Check scan direction and use appropriate rasteriser
	;
		test	eax,eax
		jnz		reversed

        call    TrapeziumRender_ZPTFB_I8_D16_32_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTFB_I8_D16_32_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTFB_I8_D16_32_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTFB_I8_D16_32_b
		
		ret

TriangleRender_ZPTFB_I8_D16_32 endp

TriangleRender_ZPTIFB_I8_D16_64_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
        call    TriangleSetup_ZPTI_FLAT_NOCHEAT

		jmp		TriangleRasterise_ZPTIFB_I8_D16_64

TriangleRender_ZPTIFB_I8_D16_64_FLAT endp


TriangleRender_ZPTIFB_I8_D16_64 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
        call    TriangleSetup_ZPTI_NOCHEAT

TriangleRasterise_ZPTIFB_I8_D16_64 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		edx,work_pi_grad_x

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		work_main_i,eax

		mov		al,byte ptr work.awsl.u_current
		mov		work_main_d_i,ebx

		mov		ah,byte ptr work.awsl.v_current
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTIFB_I8_D16_64_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIFB_I8_D16_64_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTIFB_I8_D16_64_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIFB_I8_D16_64_b
		
		ret

TriangleRender_ZPTIFB_I8_D16_64 endp


TriangleRender_ZPTFB_I8_D16_64 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPT_NOCHEAT

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		work_main_i,eax

		ror		ecx,16
		mov		al,byte ptr work.awsl.u_current

		mov		ah,byte ptr work.awsl.v_current
		mov		work_main_d_i,ebx

		shl		al,2
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTFB_I8_D16_64_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTFB_I8_D16_64_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTFB_I8_D16_64_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTFB_I8_D16_64_b
		
		ret

TriangleRender_ZPTFB_I8_D16_64 endp


TriangleRender_ZPTIFB_I8_D16_128_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPTI_FLAT_NOCHEAT

		jmp		TriangleRasterise_ZPTIFB_I8_D16_128

TriangleRender_ZPTIFB_I8_D16_128_FLAT endp


TriangleRender_ZPTIFB_I8_D16_128 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPTI_NOCHEAT

TriangleRasterise_ZPTIFB_I8_D16_128 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		edx,work_pi_grad_x

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		work_main_i,eax

		mov		al,byte ptr work.awsl.u_current
		mov		work_main_d_i,ebx

		mov		ah,byte ptr work.awsl.v_current
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTIFB_I8_D16_128_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIFB_I8_D16_128_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTIFB_I8_D16_128_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIFB_I8_D16_128_b
		
		ret

TriangleRender_ZPTIFB_I8_D16_128 endp


TriangleRender_ZPTFB_I8_D16_128 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPT_NOCHEAT

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		work_main_i,eax

		ror		ecx,16
		mov		al,byte ptr work.awsl.u_current

		mov		ah,byte ptr work.awsl.v_current
		mov		work_main_d_i,ebx

		shl		al,1
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTFB_I8_D16_128_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTFB_I8_D16_128_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTFB_I8_D16_128_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTFB_I8_D16_128_b
		
		ret

TriangleRender_ZPTFB_I8_D16_128 endp


TriangleRender_ZPTIFB_I8_D16_256_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.  Jump into the linear rasterizer in cheat cases.
	;
        call    TriangleSetup_ZPTI_FLAT_NOCHEAT

		jmp		TriangleRasterise_ZPTIFB_I8_D16_256

TriangleRender_ZPTIFB_I8_D16_256_FLAT endp


TriangleRender_ZPTIFB_I8_D16_256 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.  Jump into the linear rasterizer in cheat cases.
	;
        call    TriangleSetup_ZPTI_NOCHEAT

TriangleRasterise_ZPTIFB_I8_D16_256 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		edx,work_pi_grad_x

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		work_main_i,eax

		xor		eax,eax
		mov		work_main_d_i,ebx

		mov		al,byte ptr work.awsl.u_current
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTIFB_I8_D16_256_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIFB_I8_D16_256_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTIFB_I8_D16_256_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIFB_I8_D16_256_b
		
		ret

TriangleRender_ZPTIFB_I8_D16_256 endp


TriangleRender_ZPTFB_I8_D16_256 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.  Jump into the linear rasterizer in cheat cases.
	;
        call    TriangleSetup_ZPT_NOCHEAT

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		work_main_i,eax

		ror		ecx,16
		mov		work_main_d_i,ebx

		xor		eax,eax
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTFB_I8_D16_256_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTFB_I8_D16_256_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTFB_I8_D16_256_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTFB_I8_D16_256_b
		
		ret

TriangleRender_ZPTFB_I8_D16_256 endp


TriangleRender_ZPTIFB_I8_D16_1024_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPTI_FLAT_NOCHEAT

		jmp		TriangleRasterise_ZPTIFB_I8_D16_1024

TriangleRender_ZPTIFB_I8_D16_1024_FLAT endp


TriangleRender_ZPTIFB_I8_D16_1024 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPTI_NOCHEAT

TriangleRasterise_ZPTIFB_I8_D16_1024 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		edx,work_pi_grad_x

		ror		ecx,16
		cmp		edx,80000000h

		adc		edx,-1
		mov		work_main_i,eax

		mov		eax,work.awsl.v_current
		mov		work_main_d_i,ebx

		and		eax,1023
		mov		work.tsl.dz,ecx

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

        call    TrapeziumRender_ZPTIFB_I8_D16_1024_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIFB_I8_D16_1024_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTIFB_I8_D16_1024_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTIFB_I8_D16_1024_b
		
		ret

TriangleRender_ZPTIFB_I8_D16_1024 endp


TriangleRender_ZPTFB_I8_D16_1024 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

		mov		eax,pvertex_0

		mov		ecx,pvertex_1
		mov		edx,pvertex_2

	; Call the shared setup code.
	;
		call	TriangleSetup_ZPT_NOCHEAT

	; Calculate address of first scanline in colour and depth buffers
	;
		mov		esi,work_main_y
		mov		eax,work.colour.base

		dec		esi
		mov		ebx,work.colour.stride_b

		mov		ecx,work.depth.base
		mov		edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add		eax,ebx
		add		ecx,edx

		dec		eax
		sub		ecx,2

		mov		workspace.scanAddress,eax
		mov		workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	; Copy some values into perspective texture mappng workspace
	; Calculate offset of starting pixel in texture map
	;
		mov		eax,work_main_i
		mov		ebx,work_main_d_i

		ror		eax,16
		cmp		ebx,80000000h

		adc		ebx,-1
		mov		ecx,work_pz_grad_x

		ror		ebx,16
		cmp		ecx,80000000h

		adc		ecx,-1
		mov		work_main_i,eax

		ror		ecx,16
		mov		eax,work.awsl.v_current

		mov		work_main_d_i,ebx
		and		eax,1023

		mov		work.tsl.dz,ecx
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

        call    TrapeziumRender_ZPTFB_I8_D16_1024_f
		
		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTFB_I8_D16_1024_f
		
		ret

reversed:

        call    TrapeziumRender_ZPTFB_I8_D16_1024_b

		mov		eax,work_bot_i
		mov		ebx,work_bot_d_i

		mov		ecx,work_bot_count
		mov		work_top_i,eax

		mov		work_top_d_i,ebx
		mov		work_top_count,ecx
		
        call    TrapeziumRender_ZPTFB_I8_D16_1024_b
		
		ret

TriangleRender_ZPTFB_I8_D16_1024 endp

	endif


		OPTION PROLOGUE:NONE
		OPTION EPILOGUE:NONE

	if PARTS and PART_8Z


TrapeziumRender_ZPTI_I8_D16 32,f,\
        <shl eax,3>,\
        <add al,1000b>,<sub al,1000b>,<inc ah>,<dec ah>,\
        <shr eax,3>,<and eax,03ffh>
TrapeziumRender_ZPTI_I8_D16 32,b,\
        <shl eax,3>,\
        <add al,1000b>,<sub al,1000b>,<inc ah>,<dec ah>,\
        <shr eax,3>,<and eax,03ffh>

TrapeziumRender_ZPT_I8_D16 32,f,\
        <shl eax,3>,\
        <add al,1000b>,<sub al,1000b>,<inc ah>,<dec ah>,\
        <shr eax,3>,<and eax,03ffh>
TrapeziumRender_ZPT_I8_D16 32,b,\
        <shl eax,3>,\
        <add al,1000b>,<sub al,1000b>,<inc ah>,<dec ah>,\
        <shr eax,3>,<and eax,03ffh>

TrapeziumRender_ZPTI_I8_D16 64,f,\
	<shl eax,2>,\
	<add al,100b>,<sub al,100b>,<inc ah>,<dec ah>,\
	<shr eax,2>,<and eax,0fffh>
TrapeziumRender_ZPTI_I8_D16 64,b,\
	<shl eax,2>,\
	<add al,100b>,<sub al,100b>,<inc ah>,<dec ah>,\
	<shr eax,2>,<and eax,0fffh>

TrapeziumRender_ZPT_I8_D16 64,f,\
	<shl eax,2>,\
	<add al,100b>,<sub al,100b>,<inc ah>,<dec ah>,\
	<shr eax,2>,<and eax,0fffh>
TrapeziumRender_ZPT_I8_D16 64,b,\
	<shl eax,2>,\
	<add al,100b>,<sub al,100b>,<inc ah>,<dec ah>,\
	<shr eax,2>,<and eax,0fffh>

TrapeziumRender_ZPTI_I8_D16 128,f,\
	<shl eax,1>,\
	<add al,10b>,<sub al,10b>,<inc ah>,<dec ah>,\
	<shr eax,1>,<and eax,3fffh>
TrapeziumRender_ZPTI_I8_D16 128,b,\
	<shl eax,1>,\
	<add al,10b>,<sub al,10b>,<inc ah>,<dec ah>,\
	<shr eax,1>,<and eax,3fffh>

TrapeziumRender_ZPT_I8_D16 128,f,\
	<shl eax,1>,\
	<add al,10b>,<sub al,10b>,<inc ah>,<dec ah>,\
	<shr eax,1>,<and eax,3fffh>
TrapeziumRender_ZPT_I8_D16 128,b,\
	<shl eax,1>,\
	<add al,10b>,<sub al,10b>,<inc ah>,<dec ah>,\
	<shr eax,1>,<and eax,3fffh>

TrapeziumRender_ZPTI_I8_D16 256,f,\
	<>,\
	<inc al>,<dec al>,<inc ah>,<dec ah>,\
	<>,<>
TrapeziumRender_ZPTI_I8_D16 256,b,\
	<>,\
	<inc al>,<dec al>,<inc ah>,<dec ah>,\
	<>,<>

TrapeziumRender_ZPT_I8_D16 256,f,\
	<>,\
	<inc al>,<dec al>,<inc ah>,<dec ah>,\
	<>,<>
TrapeziumRender_ZPT_I8_D16 256,b,\
	<>,\
	<inc al>,<dec al>,<inc ah>,<dec ah>,\
	<>,<>

TrapeziumRender_ZPTI_I8_D16 1024,f,\
	<shl eax,6>,\
	<add ax,40h>,<sub ax,40h>,<add eax,10000h>,<sub eax,10000h>,\
	<shr eax,6>,<and eax,0fffffh>
TrapeziumRender_ZPTI_I8_D16 1024,b,\
	<shl eax,6>,\
	<add ax,40h>,<sub ax,40h>,<add eax,10000h>,<sub eax,10000h>,\
	<shr eax,6>,<and eax,0fffffh>

TrapeziumRender_ZPT_I8_D16 1024,f,\
	<shl eax,6>,\
	<add ax,40h>,<sub ax,40h>,<add eax,10000h>,<sub eax,10000h>,\
	<shr eax,6>,<and eax,0fffffh>
TrapeziumRender_ZPT_I8_D16 1024,b,\
	<shl eax,6>,\
	<add ax,40h>,<sub ax,40h>,<add eax,10000h>,<sub eax,10000h>,\
	<shr eax,6>,<and eax,0fffffh>

	endif

	if PARTS and PART_8Z_FOG


TrapeziumRender_ZPTI_I8_D16 32,f,\
        <shl eax,3>,\
        <add al,1000b>,<sub al,1000b>,<inc ah>,<dec ah>,\
    <shr eax,3>,<and eax,03ffh>,<F>
TrapeziumRender_ZPTI_I8_D16 32,b,\
        <shl eax,3>,\
        <add al,1000b>,<sub al,1000b>,<inc ah>,<dec ah>,\
    <shr eax,3>,<and eax,03ffh>,<F>

TrapeziumRender_ZPT_I8_D16 32,f,\
        <shl eax,3>,\
        <add al,1000b>,<sub al,1000b>,<inc ah>,<dec ah>,\
    <shr eax,3>,<and eax,03ffh>,<F>
TrapeziumRender_ZPT_I8_D16 32,b,\
        <shl eax,3>,\
        <add al,1000b>,<sub al,1000b>,<inc ah>,<dec ah>,\
    <shr eax,3>,<and eax,03ffh>,<F>

TrapeziumRender_ZPTI_I8_D16 64,f,\
	<shl eax,2>,\
	<add al,100b>,<sub al,100b>,<inc ah>,<dec ah>,\
    <shr eax,2>,<and eax,0fffh>,<F>
TrapeziumRender_ZPTI_I8_D16 64,b,\
	<shl eax,2>,\
	<add al,100b>,<sub al,100b>,<inc ah>,<dec ah>,\
    <shr eax,2>,<and eax,0fffh>,<F>

TrapeziumRender_ZPT_I8_D16 64,f,\
	<shl eax,2>,\
	<add al,100b>,<sub al,100b>,<inc ah>,<dec ah>,\
    <shr eax,2>,<and eax,0fffh>,<F>
TrapeziumRender_ZPT_I8_D16 64,b,\
	<shl eax,2>,\
	<add al,100b>,<sub al,100b>,<inc ah>,<dec ah>,\
    <shr eax,2>,<and eax,0fffh>,<F>

TrapeziumRender_ZPTI_I8_D16 128,f,\
	<shl eax,1>,\
	<add al,10b>,<sub al,10b>,<inc ah>,<dec ah>,\
    <shr eax,1>,<and eax,3fffh>,<F>
TrapeziumRender_ZPTI_I8_D16 128,b,\
	<shl eax,1>,\
	<add al,10b>,<sub al,10b>,<inc ah>,<dec ah>,\
    <shr eax,1>,<and eax,3fffh>,<F>

TrapeziumRender_ZPT_I8_D16 128,f,\
	<shl eax,1>,\
	<add al,10b>,<sub al,10b>,<inc ah>,<dec ah>,\
    <shr eax,1>,<and eax,3fffh>,<F>
TrapeziumRender_ZPT_I8_D16 128,b,\
	<shl eax,1>,\
	<add al,10b>,<sub al,10b>,<inc ah>,<dec ah>,\
    <shr eax,1>,<and eax,3fffh>,<F>

TrapeziumRender_ZPTI_I8_D16 256,f,\
	<>,\
	<inc al>,<dec al>,<inc ah>,<dec ah>,\
    <>,<>,<F>
TrapeziumRender_ZPTI_I8_D16 256,b,\
	<>,\
	<inc al>,<dec al>,<inc ah>,<dec ah>,\
    <>,<>,<F>

TrapeziumRender_ZPT_I8_D16 256,f,\
	<>,\
	<inc al>,<dec al>,<inc ah>,<dec ah>,\
    <>,<>,<F>
TrapeziumRender_ZPT_I8_D16 256,b,\
	<>,\
	<inc al>,<dec al>,<inc ah>,<dec ah>,\
    <>,<>,<F>

TrapeziumRender_ZPTI_I8_D16 1024,f,\
	<shl eax,6>,\
	<add ax,40h>,<sub ax,40h>,<add eax,10000h>,<sub eax,10000h>,\
    <shr eax,6>,<and eax,0fffffh>,<F>
TrapeziumRender_ZPTI_I8_D16 1024,b,\
	<shl eax,6>,\
	<add ax,40h>,<sub ax,40h>,<add eax,10000h>,<sub eax,10000h>,\
    <shr eax,6>,<and eax,0fffffh>,<F>

TrapeziumRender_ZPT_I8_D16 1024,f,\
	<shl eax,6>,\
	<add ax,40h>,<sub ax,40h>,<add eax,10000h>,<sub eax,10000h>,\
    <shr eax,6>,<and eax,0fffffh>,<F>
TrapeziumRender_ZPT_I8_D16 1024,b,\
	<shl eax,6>,\
	<add ax,40h>,<sub ax,40h>,<add eax,10000h>,<sub eax,10000h>,\
    <shr eax,6>,<and eax,0fffffh>,<F>

	endif

	if PARTS and PART_8Z_BLEND

TrapeziumRender_ZPTI_I8_D16 32,f,\
        <shl eax,3>,\
        <add al,1000b>,<sub al,1000b>,<inc ah>,<dec ah>,\
    <shr eax,3>,<and eax,03ffh>,<>,<B>
TrapeziumRender_ZPTI_I8_D16 32,b,\
        <shl eax,3>,\
        <add al,1000b>,<sub al,1000b>,<inc ah>,<dec ah>,\
    <shr eax,3>,<and eax,03ffh>,<>,<B>

TrapeziumRender_ZPT_I8_D16 32,f,\
        <shl eax,3>,\
        <add al,1000b>,<sub al,1000b>,<inc ah>,<dec ah>,\
    <shr eax,3>,<and eax,03ffh>,<>,<B>
TrapeziumRender_ZPT_I8_D16 32,b,\
        <shl eax,3>,\
        <add al,1000b>,<sub al,1000b>,<inc ah>,<dec ah>,\
    <shr eax,3>,<and eax,03ffh>,<>,<B>

TrapeziumRender_ZPTI_I8_D16 64,f,\
	<shl eax,2>,\
	<add al,100b>,<sub al,100b>,<inc ah>,<dec ah>,\
    <shr eax,2>,<and eax,0fffh>,<>,<B>
TrapeziumRender_ZPTI_I8_D16 64,b,\
	<shl eax,2>,\
	<add al,100b>,<sub al,100b>,<inc ah>,<dec ah>,\
    <shr eax,2>,<and eax,0fffh>,<>,<B>

TrapeziumRender_ZPT_I8_D16 64,f,\
	<shl eax,2>,\
	<add al,100b>,<sub al,100b>,<inc ah>,<dec ah>,\
    <shr eax,2>,<and eax,0fffh>,<>,<B>
TrapeziumRender_ZPT_I8_D16 64,b,\
	<shl eax,2>,\
	<add al,100b>,<sub al,100b>,<inc ah>,<dec ah>,\
    <shr eax,2>,<and eax,0fffh>,<>,<B>

TrapeziumRender_ZPTI_I8_D16 128,f,\
	<shl eax,1>,\
	<add al,10b>,<sub al,10b>,<inc ah>,<dec ah>,\
    <shr eax,1>,<and eax,3fffh>,<>,<B>
TrapeziumRender_ZPTI_I8_D16 128,b,\
	<shl eax,1>,\
	<add al,10b>,<sub al,10b>,<inc ah>,<dec ah>,\
    <shr eax,1>,<and eax,3fffh>,<>,<B>

TrapeziumRender_ZPT_I8_D16 128,f,\
	<shl eax,1>,\
	<add al,10b>,<sub al,10b>,<inc ah>,<dec ah>,\
    <shr eax,1>,<and eax,3fffh>,<>,<B>
TrapeziumRender_ZPT_I8_D16 128,b,\
	<shl eax,1>,\
	<add al,10b>,<sub al,10b>,<inc ah>,<dec ah>,\
    <shr eax,1>,<and eax,3fffh>,<>,<B>

TrapeziumRender_ZPTI_I8_D16 256,f,\
	<>,\
	<inc al>,<dec al>,<inc ah>,<dec ah>,\
    <>,<>,<>,<B>
TrapeziumRender_ZPTI_I8_D16 256,b,\
	<>,\
	<inc al>,<dec al>,<inc ah>,<dec ah>,\
    <>,<>,<>,<B>

TrapeziumRender_ZPT_I8_D16 256,f,\
	<>,\
	<inc al>,<dec al>,<inc ah>,<dec ah>,\
    <>,<>,<>,<B>
TrapeziumRender_ZPT_I8_D16 256,b,\
	<>,\
	<inc al>,<dec al>,<inc ah>,<dec ah>,\
    <>,<>,<>,<B>

TrapeziumRender_ZPTI_I8_D16 1024,f,\
	<shl eax,6>,\
	<add ax,40h>,<sub ax,40h>,<add eax,10000h>,<sub eax,10000h>,\
    <shr eax,6>,<and eax,0fffffh>,<>,<B>
TrapeziumRender_ZPTI_I8_D16 1024,b,\
	<shl eax,6>,\
	<add ax,40h>,<sub ax,40h>,<add eax,10000h>,<sub eax,10000h>,\
    <shr eax,6>,<and eax,0fffffh>,<>,<B>

TrapeziumRender_ZPT_I8_D16 1024,f,\
	<shl eax,6>,\
	<add ax,40h>,<sub ax,40h>,<add eax,10000h>,<sub eax,10000h>,\
    <shr eax,6>,<and eax,0fffffh>,<>,<B>
TrapeziumRender_ZPT_I8_D16 1024,b,\
	<shl eax,6>,\
	<add ax,40h>,<sub ax,40h>,<add eax,10000h>,<sub eax,10000h>,\
    <shr eax,6>,<and eax,0fffffh>,<>,<B>

	endif

	if PARTS and PART_8Z_FOG_BLEND

TrapeziumRender_ZPTI_I8_D16 32,f,\
        <shl eax,3>,\
        <add al,1000b>,<sub al,1000b>,<inc ah>,<dec ah>,\
    <shr eax,3>,<and eax,03ffh>,<F>,<B>
TrapeziumRender_ZPTI_I8_D16 32,b,\
        <shl eax,3>,\
        <add al,1000b>,<sub al,1000b>,<inc ah>,<dec ah>,\
    <shr eax,3>,<and eax,03ffh>,<F>,<B>

TrapeziumRender_ZPT_I8_D16 32,f,\
        <shl eax,3>,\
        <add al,1000b>,<sub al,1000b>,<inc ah>,<dec ah>,\
    <shr eax,3>,<and eax,03ffh>,<F>,<B>
TrapeziumRender_ZPT_I8_D16 32,b,\
        <shl eax,3>,\
        <add al,1000b>,<sub al,1000b>,<inc ah>,<dec ah>,\
    <shr eax,3>,<and eax,03ffh>,<F>,<B>

TrapeziumRender_ZPTI_I8_D16 64,f,\
	<shl eax,2>,\
	<add al,100b>,<sub al,100b>,<inc ah>,<dec ah>,\
    <shr eax,2>,<and eax,0fffh>,<F>,<B>
TrapeziumRender_ZPTI_I8_D16 64,b,\
	<shl eax,2>,\
	<add al,100b>,<sub al,100b>,<inc ah>,<dec ah>,\
    <shr eax,2>,<and eax,0fffh>,<F>,<B>

TrapeziumRender_ZPT_I8_D16 64,f,\
	<shl eax,2>,\
	<add al,100b>,<sub al,100b>,<inc ah>,<dec ah>,\
    <shr eax,2>,<and eax,0fffh>,<F>,<B>
TrapeziumRender_ZPT_I8_D16 64,b,\
	<shl eax,2>,\
	<add al,100b>,<sub al,100b>,<inc ah>,<dec ah>,\
    <shr eax,2>,<and eax,0fffh>,<F>,<B>

TrapeziumRender_ZPTI_I8_D16 128,f,\
	<shl eax,1>,\
	<add al,10b>,<sub al,10b>,<inc ah>,<dec ah>,\
    <shr eax,1>,<and eax,3fffh>,<F>,<B>
TrapeziumRender_ZPTI_I8_D16 128,b,\
	<shl eax,1>,\
	<add al,10b>,<sub al,10b>,<inc ah>,<dec ah>,\
    <shr eax,1>,<and eax,3fffh>,<F>,<B>

TrapeziumRender_ZPT_I8_D16 128,f,\
	<shl eax,1>,\
	<add al,10b>,<sub al,10b>,<inc ah>,<dec ah>,\
    <shr eax,1>,<and eax,3fffh>,<F>,<B>
TrapeziumRender_ZPT_I8_D16 128,b,\
	<shl eax,1>,\
	<add al,10b>,<sub al,10b>,<inc ah>,<dec ah>,\
    <shr eax,1>,<and eax,3fffh>,<F>,<B>

TrapeziumRender_ZPTI_I8_D16 256,f,\
	<>,\
	<inc al>,<dec al>,<inc ah>,<dec ah>,\
    <>,<>,<F>,<B>
TrapeziumRender_ZPTI_I8_D16 256,b,\
	<>,\
	<inc al>,<dec al>,<inc ah>,<dec ah>,\
    <>,<>,<F>,<B>

TrapeziumRender_ZPT_I8_D16 256,f,\
	<>,\
	<inc al>,<dec al>,<inc ah>,<dec ah>,\
    <>,<>,<F>,<B>
TrapeziumRender_ZPT_I8_D16 256,b,\
	<>,\
	<inc al>,<dec al>,<inc ah>,<dec ah>,\
    <>,<>,<F>,<B>

TrapeziumRender_ZPTI_I8_D16 1024,f,\
	<shl eax,6>,\
	<add ax,40h>,<sub ax,40h>,<add eax,10000h>,<sub eax,10000h>,\
    <shr eax,6>,<and eax,0fffffh>,<F>,<B>
TrapeziumRender_ZPTI_I8_D16 1024,b,\
	<shl eax,6>,\
	<add ax,40h>,<sub ax,40h>,<add eax,10000h>,<sub eax,10000h>,\
    <shr eax,6>,<and eax,0fffffh>,<F>,<B>

TrapeziumRender_ZPT_I8_D16 1024,f,\
	<shl eax,6>,\
	<add ax,40h>,<sub ax,40h>,<add eax,10000h>,<sub eax,10000h>,\
    <shr eax,6>,<and eax,0fffffh>,<F>,<B>
TrapeziumRender_ZPT_I8_D16 1024,b,\
	<shl eax,6>,\
	<add ax,40h>,<sub ax,40h>,<add eax,10000h>,<sub eax,10000h>,\
    <shr eax,6>,<and eax,0fffffh>,<F>,<B>

	endif

    end





