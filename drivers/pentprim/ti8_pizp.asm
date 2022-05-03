;; Copyright (c) 1992,1993-1995 Argonalt Technologies Limited. All rights reserved.
;;
;; $Id: ti8_pizp.asm 1.1 1997/12/10 16:49:05 jon Exp $
;; $Locker: $
;;
;; Scanline loop for perpsective correct texture mapping
;;
		.386p
		.model	c,flat
		include	drv.inc
		.data

	; Macros allowing spare slots to be either filled with
	; NOPs or left empty
	;
uslot	macro
;		nop
		endm

vslot	macro
;		nop
		endm

		; Temporary storage
		; Fits into two cache lines
		; Still necessary for old code
		;
	if 1
z		dd	?
i		dd	?
du_numerator	dd	?
dv_numerator	dd	?
u_numerator	dd	?
v_numerator	dd	?
zdest		dd	?
dest		dd	?
dest2		dd	?
source		dd	?
denominator	dd	?
dz		dd	?
_di		dd	?
ddenominator	dd	?
	endif

		align 4

		.code

; Smoothly lit perspective texture mapping

TrapeziumPIZ2TIP        macro   name,pre,incu,decu,incv,decv,post1,post2,fog

		public	Trapezium&name&

ScanLine&name&_table	dd	ScanLine&name&_rl_ud_vd
				    	dd	ScanLine&name&_rl_ui_vd
		            	dd	ScanLine&name&_rl_ud_vi
				    	dd	ScanLine&name&_rl_ui_vi
		            	dd	ScanLine&name&_lr_ui_vi
				    	dd	ScanLine&name&_lr_ud_vi
            			dd	ScanLine&name&_lr_ui_vd
		            	dd	ScanLine&name&_lr_ud_vd

ScanLinePIZ2TIP ScanLine&name&_rl_ud_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,0,0,1,<fog>,ScanLine&name&_done
ScanLinePIZ2TIP ScanLine&name&_rl_ud_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,0,1,1,<fog>,ScanLine&name&_done
ScanLinePIZ2TIP ScanLine&name&_rl_ui_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,1,0,1,<fog>,ScanLine&name&_done
ScanLinePIZ2TIP ScanLine&name&_rl_ui_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,1,1,1,<fog>,ScanLine&name&_done
ScanLinePIZ2TIP ScanLine&name&_lr_ud_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,0,0,1,<fog>,ScanLine&name&_done
ScanLinePIZ2TIP ScanLine&name&_lr_ud_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,0,1,1,<fog>,ScanLine&name&_done
ScanLinePIZ2TIP ScanLine&name&_lr_ui_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,1,0,1,<fog>,ScanLine&name&_done
ScanLinePIZ2TIP ScanLine&name&_lr_ui_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,1,1,1,<fog>,ScanLine&name&_done

Trapezium&name&_done	proc
		popad
		ret
Trapezium&name&_done	endp

Trapezium&name&	proc

		pushad

		mov	ebx, work.top.count
		mov	eax, work.tsl.start

		cmp	ebx, 0					; Finish immediately if scanline count is zero
		je	Trapezium&name&_done

	; Start of outer loop
	;
outer_start:

	; Check for zero length spans
	;
		cmp	eax, work.tsl._end
		mov	ebx, work.pq.current

		mov	ecx, work.pq.grad_x
		je	ScanLine&name&_done

	; Fixups
	;
		mov	eax, work.tsl.source
		mov	edx, work.pq.d_nocarry

		mov	ebp, work.pu.current
		mov	edi, work.pu.grad_x

		mov	esi, work.tsl.du_numerator_nocarry
		pre

		cmp	ebp, ebx
		jl	uidone

uiloop:
		incu						; work.tsl.source = incu(work.tsl.source)
		add	edi, ecx				; work.pu.grad_x += work.pq.grad_x

		add	esi, edx				; work.tsl.du_numerator_nocarry += work.pq.d_nocarry
		sub	ebp, ebx				; work.pu.current -= work.pq.current

		cmp	ebp, ebx
		jge	uiloop

uidone:
		cmp	ebp, 0
		jge	uddone

udloop:
		decu						; work.tsl.source = decu(work.tsl.source)
		sub	edi, ecx				; work.pu.grad_x -= work.pq.grad_x

		sub	esi, edx				; work.tsl.du_numerator_nocarry -= work.pq.d_nocarry
		add	ebp, ebx				; work.pu.current += work.pq.current

		uslot
		jl	udloop

uddone:
		mov	work.pu.current, ebp
		mov	work.pu.grad_x, edi

		mov	work.tsl.du_numerator_nocarry, esi
		mov	ebp, work.pv.current

		mov	edi, work.pv.grad_x
		cmp	ebp, ebx

		mov	esi, work.tsl.dv_numerator_nocarry
		jl	vidone

viloop:
		incv						; work.tsl.source = incv(work.tsl.source)
		add	edi, ecx				; work.pv.grad_x += work.pq.grad_x

		add	esi, edx				; work.tsl.dv_numerator_nocarry += work.pq.d_nocarry
		sub	ebp, ebx				; work.pv.current -= work.pq.current

		cmp	ebp, ebx
		jge	viloop

vidone:
		cmp	ebp, 0
		jge	vddone

vdloop:
		decv						; work.tsl.source = decv(work.tsl.source)
		sub	edi, ecx				; work.pv.grad_x -= work.pq.grad_x

		sub	esi, edx				; work.tsl.dv_numerator_nocarry -= work.pq.d_nocarry
		add	ebp, ebx				; work.pv.current += work.pq.current

		uslot
		jl	vdloop

vddone:

		post1
		mov	work.tsl.dv_numerator_nocarry, esi

		post2
		mov	edx, work.tsl.direction

	; Select scanline loop and jump to it
	;
		mov	esi, work.pu.grad_x
		cmp	edi, 80000000h

		rcl	edx, 1
		cmp	esi, 80000000h

		rcl	edx, 1
		mov	work.pv.current, ebp

		mov	work.pv.grad_x, edi
		mov	work.tsl.source, eax

		jmp	ScanLine&name&_table[edx * 4]

	; Scanline loops return here when finished
	;
ScanLine&name&_done::

		mov	eax, work.top.d_f
		mov	ebp, work.top.d_i

		add	work.top.f, eax				; work.top.f += work.top.d_f
		mov	esi, work.colour.stride_b

		mov	ebx, work.tsl.du_numerator_nocarry
		mov	edi, work.main.d_i

		cmp	work.top.f, eax				; carry = (unsigned)work.top.f < (unsigned)work.top.d_f
		mov	ecx, work.tsl.dv_numerator_nocarry

		adc	ebp, esi				; ebp = work.top.d_i + work.colour.stride_b + carry
		mov	edx, work.pz.d_nocarry

		add	esi, edi				; esi = work.main.d_i + work.colour.stride_b
		add	edi, edi				; edi = 2 * work.main.d_i

		add	work.tsl._end, ebp			; work.tsl.end += work.top.d_i + work.colour.stride_b + carry
		mov	eax, work.main.d_f

		add	edi, work.depth.stride_b		; edi = 2 * work.main.d_i + work.depth.stride_b
		add	work.main.f, eax			; work.main.f += work.main.d_f

		cmp	work.main.f, eax			; carry = (unsigned)work.main.f < (unsigned)work.main.d_f
		mov	eax, work.pq.d_nocarry

		mov	ebp, work.pi.d_nocarry
		jc	carry

		add	work.pq.current, eax			; work.pq.current += work.pq.d_nocarry
		sub	work.pu.current, ebx			; work.pu.current += -work.tsl.du_numerator_nocarry

		sub	work.pv.current, ecx			; work.pv.current += -work.tsl.dv_numerator_nocarry
		mov	eax, work.tsl.start

		add	work.pz.current, edx			; work.pz.current += work.pz.d_nocarry
		add	eax, esi

		add	work.pi.current, ebp			; work.pi.current += work.pi.d_nocarry
		mov	work.tsl.start, eax			; work.tsl.start += work.main.d_i + work.colour.stride_b

		add	work.tsl.zstart, edi			; work.tsl.zstart += 2 * work.main.d_i + work.depth.stride_b
		dec	work.top.count				; Decrement Y count

		jnz	outer_start				; Loop if not zero

		popad

		ret

carry:
		add	eax, work.pq.grad_x
		add	ebx, work.pu.grad_x

		add	ecx, work.pv.grad_x
		add	edx, work.pz.grad_x

		add	ebp, work.pi.grad_x
		add	esi, 1

		add	edi, 2
		vslot

		add	work.pq.current, eax			; work.pq.current += work.pq.d_nocarry + work.pq.grad_x
		sub	work.pu.current, ebx			; work.pu.current += -work.tsl.du_numerator_nocarry + work.pu.grad_x

		sub	work.pv.current, ecx			; work.pv.current += -work.tsl.dv_numerator_nocarry + work.pv.grad_x
		mov	eax, work.tsl.start

		add	work.pz.current, edx			; work.pz.current += work.pz.d_nocarry + work.pz.grad_x
		add	eax, esi

		add	work.pi.current, ebp			; work.pi.current += work.pi.d_nocarry + work.pi.grad_x
		mov	work.tsl.start, eax			; work.tsl.start += work.main.d_i + work.colour.stride_b + 1

		add	work.tsl.zstart, edi			; work.tsl.zstart += 2 * work.main.d_i + work.depth.stride_b + 2
		dec	work.top.count				; Decrement Y count

		jnz	outer_start				; Loop if not zero

		popad

		ret

Trapezium&name&	endp

		endm


ScanLinePIZ2TIP macro   name,pre,incu,decu,incv,decv,post1,post2,LEFT_TO_RIGHT,U_INCREASING,V_INCREASING,TRANSPARENCY,FOGGING,done
name	proc

	; Make temporary copies of parameters that change
	;
		mov	work.tsl.denominator, ebx
		mov	edx, work.pu.current

if U_INCREASING
		sub	edx, ebx				; Move error into the range -1..0
		add	esi, ecx
endif

if V_INCREASING
		sub	ebp, ebx				; Move error into the range -1..0
		add	edi, ecx
endif

		mov	work.tsl.u_numerator, edx
		mov	work.tsl.du_numerator, esi

		mov	work.tsl.v_numerator, ebp
		mov	work.tsl.dv_numerator, edi

		mov	esi,work.texture.base
		mov	edi, work.tsl.start

		mov	ebp, work.tsl.zstart

		mov	ebx, work.pz.current
		mov	ecx, work.pz.grad_x

		ror	ebx, 16					; Swap z and dz words
		mov	edx, work.pi.current

		ror	ecx, 16					; Swap z and dz words
		mov	work.tsl.z, ebx

		mov	work.tsl.dz, ecx
		mov	work.tsl.i, edx

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
		mov	dx,[ebp]
		xor	ecx,ecx

		mov	ebx,work.tsl.z
		mov	cl,[eax+esi]

		cmp	bx,dx
		ja	nodraw

	; Get intensity
	;
		mov	edx,work.shade_table
		mov	ch,byte ptr (work.tsl.i+2)

	; Test for transparency
	;
if TRANSPARENCY
		test	cl,cl
		jz	nodraw
endif

if FOGGING
	; Look texel up in shade table, store texel and z
	;
		mov	cl,[ecx+edx]
                mov     edx,work.fog_table

                mov     ch,bh
                mov     [ebp],bl

        ;AGI stall here...
                mov     cl,[ecx+edx]

		mov	[edi],cl
		mov	[ebp+1],bh
else
	; Look texel up in shade table, store texel and z
	;
		mov	[ebp],bl
		mov	cl,[ecx+edx]

		mov	[ebp+1],bh
		mov	[edi],cl
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
		mov	ecx,work.tsl._end

	; Update destinations and check for end of scan
	;
if LEFT_TO_RIGHT
		inc	edi
		add	ebp,2

		cmp	edi,ecx
		jae	done
else
		dec	edi
		sub	ebp,2

		cmp	edi,ecx
		jbe	done
endif

	; Interpolate z and i
	;
		mov	ecx,work.tsl.dz
		mov	edx,work.tsl._di

if LEFT_TO_RIGHT
		add	ebx,ecx
else
		sub	ebx,ecx
endif

		mov	ecx,work.tsl.i

if LEFT_TO_RIGHT
		adc	ebx,0		; carry into integer part of z
		add	ecx,edx
else
		sbb	ebx,0		; carry into integer part of z
		sub	ecx,edx
endif

		mov	work.tsl.dest,edi
		mov	work.tsl.zdest,ebp

		mov	work.tsl.z,ebx
		mov	work.tsl.i,ecx

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
		mov	edx,work.tsl.denominator
		mov	ebp,work.tsl.ddenominator

		mov	ebx,work.tsl.u_numerator
		mov	edi,work.tsl.du_numerator

	; Interpolate u numerator and denominator
	;
if LEFT_TO_RIGHT
		add	edx,ebp
		sub	ebx,edi
else
		sub	edx,ebp
		add	ebx,edi
endif

		mov	ecx,work.tsl.v_numerator

	; Check for u error going outside range 0..1
	;
if U_INCREASING
		jl	doneu
	
	; Adjust u upward
	;
inculoop:
		incu
		add	edi,ebp

		sub	ebx,edx
		jge	inculoop
else
		jge	doneu

	; Adjust u downward
	;
deculoop:
		decu
		sub	edi,ebp

		add	ebx,edx
		jnc	deculoop
endif

		mov	work.tsl.du_numerator,edi
		vslot
doneu:
		mov	edi,work.tsl.dv_numerator
		mov	work.tsl.u_numerator,ebx
			
	; Interpolate v numerator
	;
if LEFT_TO_RIGHT
		sub	ecx,edi
else
		add	ecx,edi
endif

		mov	work.tsl.denominator,edx

	; Check for v error going outside range 0..1
	;
		uslot
if V_INCREASING
		jl	donev

	; Adjust v upward
	;
incvloop:
		incv
		add	edi,ebp

		sub	ecx,edx
		jge	incvloop
else
		jge	donev

	; Adjust v downward
	;
decvloop:	   
		decv		; Decrement v
		sub	edi,ebp

		add	ecx,edx
		jnc	decvloop
endif

		mov	work.tsl.dv_numerator,edi
		vslot
donev:

	; Fix wrapping of source offset after modification
	;
		post1
		mov	work.tsl.v_numerator,ecx

		post2
		mov	ebp,work.tsl.zdest

		mov	edi,work.tsl.dest
		jmp	next_pixel

name	endp
		endm


; Unlit version

TrapeziumPIZ2TP macro   name,pre,incu,decu,incv,decv,post1,post2,fog

		public	Trapezium&name&

ScanLine&name&_table	dd	ScanLine&name&_rl_ud_vd
				    	dd	ScanLine&name&_rl_ui_vd
		            	dd	ScanLine&name&_rl_ud_vi
				    	dd	ScanLine&name&_rl_ui_vi
		            	dd	ScanLine&name&_lr_ui_vi
				    	dd	ScanLine&name&_lr_ud_vi
            			dd	ScanLine&name&_lr_ui_vd
		            	dd	ScanLine&name&_lr_ud_vd

ScanLinePIZ2TP  ScanLine&name&_rl_ud_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,0,0,1,<fog>,ScanLine&name&_done
ScanLinePIZ2TP  ScanLine&name&_rl_ud_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,0,1,1,<fog>,ScanLine&name&_done
ScanLinePIZ2TP  ScanLine&name&_rl_ui_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,1,0,1,<fog>,ScanLine&name&_done
ScanLinePIZ2TP  ScanLine&name&_rl_ui_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,1,1,1,<fog>,ScanLine&name&_done
ScanLinePIZ2TP  ScanLine&name&_lr_ud_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,0,0,1,<fog>,ScanLine&name&_done
ScanLinePIZ2TP  ScanLine&name&_lr_ud_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,0,1,1,<fog>,ScanLine&name&_done
ScanLinePIZ2TP  ScanLine&name&_lr_ui_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,1,0,1,<fog>,ScanLine&name&_done
ScanLinePIZ2TP  ScanLine&name&_lr_ui_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,1,1,1,<fog>,ScanLine&name&_done

Trapezium&name&_done	proc
		popad
		ret
Trapezium&name&_done	endp

Trapezium&name&	proc

		pushad

		mov	ebx, work.top.count
		mov	eax, work.tsl.start

		cmp	ebx, 0					; Finish immediately if scanline count is zero
		je	Trapezium&name&_done

	; Start of outer loop
	;
outer_start:

	; Check for zero length spans
	;
		cmp	eax, work.tsl._end
		mov	ebx, work.pq.current

		mov	ecx, work.pq.grad_x
		je	ScanLine&name&_done

	; Fixups
	;
		mov	eax, work.tsl.source
		mov	edx, work.pq.d_nocarry

		mov	ebp, work.pu.current
		mov	edi, work.pu.grad_x

		mov	esi, work.tsl.du_numerator_nocarry
		pre

		cmp	ebp, ebx
		jl	uidone

uiloop:
		incu						; work.tsl.source = incu(work.tsl.source)
		add	edi, ecx				; work.pu.grad_x += work.pq.grad_x

		add	esi, edx				; work.tsl.du_numerator_nocarry += work.pq.d_nocarry
		sub	ebp, ebx				; work.pu.current -= work.pq.current

		cmp	ebp, ebx
		jge	uiloop

uidone:
		cmp	ebp, 0
		jge	uddone

udloop:
		decu						; work.tsl.source = decu(work.tsl.source)
		sub	edi, ecx				; work.pu.grad_x -= work.pq.grad_x

		sub	esi, edx				; work.tsl.du_numerator_nocarry -= work.pq.d_nocarry
		add	ebp, ebx				; work.pu.current += work.pq.current

		uslot
		jl	udloop

uddone:
		mov	work.pu.current, ebp
		mov	work.pu.grad_x, edi

		mov	work.tsl.du_numerator_nocarry, esi
		mov	ebp, work.pv.current

		mov	edi, work.pv.grad_x
		cmp	ebp, ebx

		mov	esi, work.tsl.dv_numerator_nocarry
		jl	vidone

viloop:
		incv						; work.tsl.source = incv(work.tsl.source)
		add	edi, ecx				; work.pv.grad_x += work.pq.grad_x

		add	esi, edx				; work.tsl.dv_numerator_nocarry += work.pq.d_nocarry
		sub	ebp, ebx				; work.pv.current -= work.pq.current

		cmp	ebp, ebx
		jge	viloop

vidone:
		cmp	ebp, 0
		jge	vddone

vdloop:
		decv						; work.tsl.source = decv(work.tsl.source)
		sub	edi, ecx				; work.pv.grad_x -= work.pq.grad_x

		sub	esi, edx				; work.tsl.dv_numerator_nocarry -= work.pq.d_nocarry
		add	ebp, ebx				; work.pv.current += work.pq.current

		uslot
		jl	vdloop

vddone:

		post1
		mov	work.tsl.dv_numerator_nocarry, esi

		post2
		mov	edx, work.tsl.direction

	; Select scanline loop and jump to it
	;
		mov	esi, work.pu.grad_x
		cmp	edi, 80000000h

		rcl	edx, 1
		cmp	esi, 80000000h

		rcl	edx, 1
		mov	work.pv.current, ebp

		mov	work.pv.grad_x, edi
		mov	work.tsl.source, eax

		jmp	ScanLine&name&_table[edx * 4]

	; Scanline loops return here when finished
	;
ScanLine&name&_done::

		mov	eax, work.top.d_f
		mov	ebp, work.top.d_i

		add	work.top.f, eax				; work.top.f += work.top.d_f
		mov	esi, work.colour.stride_b

		mov	ebx, work.tsl.du_numerator_nocarry
		mov	edi, work.main.d_i

		cmp	work.top.f, eax				; carry = (unsigned)work.top.f < (unsigned)work.top.d_f
		mov	ecx, work.tsl.dv_numerator_nocarry

		adc	ebp, esi				; ebp = work.top.d_i + work.colour.stride_b + carry
		mov	edx, work.pz.d_nocarry

		add	esi, edi				; esi = work.main.d_i + work.colour.stride_b
		add	edi, edi				; edi = 2 * work.main.d_i

		add	work.tsl._end, ebp			; work.tsl.end += work.top.d_i + work.colour.stride_b + carry
		mov	eax, work.main.d_f

		add	edi, work.depth.stride_b		; edi = 2 * work.main.d_i + work.depth.stride_b
		add	work.main.f, eax			; work.main.f += work.main.d_f

		cmp	work.main.f, eax			; carry = (unsigned)work.main.f < (unsigned)work.main.d_f
		jc	carry

		mov	eax, work.pq.d_nocarry
		sub	work.pu.current, ebx			; work.pu.current += -work.tsl.du_numerator_nocarry

		add	work.pq.current, eax			; work.pq.current += work.pq.d_nocarry
		sub	work.pv.current, ecx			; work.pv.current += -work.tsl.dv_numerator_nocarry

		mov	eax, work.tsl.start
		add	work.pz.current, edx			; work.pz.current += work.pz.d_nocarry

		add	eax, esi
		add	work.tsl.zstart, edi			; work.tsl.zstart += 2 * work.main.d_i + work.depth.stride_b

		mov	work.tsl.start, eax			; work.tsl.start += work.main.d_i + work.colour.stride_b
		dec	work.top.count				; Decrement Y count

		jnz	outer_start				; Loop if not zero

		popad

		ret

carry:
		mov	eax, work.pq.d_nocarry
		add	ebx, work.pu.grad_x

		add	eax, work.pq.grad_x
		add	ecx, work.pv.grad_x

		add	edx, work.pz.grad_x
		add	esi, 1

		add	edi, 2
		add	work.pq.current, eax			; work.pq.current += work.pq.d_nocarry + work.pq.grad_x

		sub	work.pu.current, ebx			; work.pu.current += -work.tsl.du_numerator_nocarry + work.pu.grad_x
		sub	work.pv.current, ecx			; work.pv.current += -work.tsl.dv_numerator_nocarry + work.pv.grad_x

		mov	eax, work.tsl.start
		add	work.pz.current, edx			; work.pz.current += work.pz.d_nocarry + work.pz.grad_x

		add	eax, esi
		add	work.tsl.zstart, edi			; work.tsl.zstart += 2 * work.main.d_i + work.depth.stride_b + 2

		mov	work.tsl.start, eax			; work.tsl.start += work.main.d_i + work.colour.stride_b + 1
		dec	work.top.count				; Decrement Y count

		jnz	outer_start				; Loop if not zero

		popad

		ret

Trapezium&name&	endp

		endm


ScanLinePIZ2TP  macro   name,pre,incu,decu,incv,decv,post1,post2,LEFT_TO_RIGHT,U_INCREASING,V_INCREASING,TRANSPARENCY,FOGGING,done
name	proc

	; Make temporary copies of parameters that change
	;
		mov	work.tsl.denominator, ebx
		mov	edx, work.pu.current

if U_INCREASING
		sub	edx, ebx				; Move error into the range -1..0
		add	esi, ecx
endif

if V_INCREASING
		sub	ebp, ebx				; Move error into the range -1..0
		add	edi, ecx
endif

		mov	work.tsl.u_numerator, edx
		mov	work.tsl.du_numerator, esi

		mov	work.tsl.v_numerator, ebp
		mov	work.tsl.dv_numerator, edi

		mov	ebx, work.pz.current
		mov	ecx, work.pz.grad_x

		ror	ebx, 16					; Swap z and dz words
		mov	edi, work.tsl.start

		ror	ecx, 16					; Swap z and dz words
		mov	esi,work.texture.base

		mov	ebp, work.tsl.zstart
		mov	work.tsl.dest, edi

		mov	work.tsl.dz, ecx

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
		mov	dl,[ebp]
		mov	cl,[eax+esi]

		mov	dh,[ebp+1]
		mov	edi,work.tsl.dest

		cmp	bx,dx
		ja	nodraw

	; Test for transparency
	;
if TRANSPARENCY
		test	cl,cl
		jz	nodraw
endif

if FOGGING
	; Store texel and z
	;
                mov     edx,work.fog_table
                mov     ch,bh
        ;1 cycle prefix
                mov     [ebp],bx
                mov     cl,[edx+ecx]

		mov	[edi],cl
                test    cl,cl
        ;make sure execution continues in u-pipe
else
	; Store texel and z
	;
		mov	[ebp],bx
		mov	[edi],cl
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
		mov	ecx,work.tsl._end

	; Update destinations and check for end of scan
	;
if LEFT_TO_RIGHT
		inc	edi
		add	ebp,2

		cmp	edi,ecx
		jae	done
else
		dec	edi
		sub	ebp,2

		cmp	edi,ecx
		jbe	done
endif

	; Interpolate z
	;
		mov	ecx,work.tsl.dz
		mov	work.tsl.dest,edi

if LEFT_TO_RIGHT
		add	ebx,ecx
else
		sub	ebx,ecx
endif
		mov	work.tsl.zdest,ebp

if LEFT_TO_RIGHT
		adc	ebx,0		; carry into integer part of z
else
		sbb	ebx,0		; carry into integer part of z
endif

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
		mov	edx,work.tsl.denominator

		mov	work.tsl.z,ebx
		mov	ebp,work.tsl.ddenominator

		mov	ebx,work.tsl.u_numerator
		mov	edi,work.tsl.du_numerator

	; Interpolate u numerator and denominator
	;
if LEFT_TO_RIGHT
		add	edx,ebp
		sub	ebx,edi
else
		sub	edx,ebp
		add	ebx,edi
endif

		mov	ecx,work.tsl.v_numerator

	; Check for u error going outside range 0..1
	;
if U_INCREASING
		jl	doneu
	
	; Adjust u upward
	;
inculoop:
		incu
		add	edi,ebp

		sub	ebx,edx
		jge	inculoop
else
		jge	doneu

	; Adjust u downward
	;
deculoop:
		decu
		sub	edi,ebp

		add	ebx,edx
		jnc	deculoop
endif

		mov	work.tsl.du_numerator,edi
		vslot
doneu:
		mov	edi,work.tsl.dv_numerator
		mov	work.tsl.u_numerator,ebx
			
	; Interpolate v numerator
	;
if LEFT_TO_RIGHT
		sub	ecx,edi
else
		add	ecx,edi
endif

		mov	work.tsl.denominator,edx

	; Check for v error going outside range 0..1
	;
		uslot
if V_INCREASING
		jl	donev

	; Adjust v upward
	;
incvloop:
		incv
		add	edi,ebp

		sub	ecx,edx
		jge	incvloop
else
		jge	donev

	; Adjust v downward
	;
decvloop:	   
		decv		; Decrement v
		sub	edi,ebp

		add	ecx,edx
		jnc	decvloop
endif

		mov	work.tsl.dv_numerator,edi
		vslot
donev:

	; Fix wrapping of source offset after modification
	;
		post1
		mov	work.tsl.v_numerator,ecx

		post2
		mov	ebp,work.tsl.zdest

		mov	ebx,work.tsl.z
		jmp	next_pixel

name	endp
		endm


; Dithered, unlit

; a += b*frac/16

dither		macro	a,b,frac

	push	b

	ifidni	<frac>,<0>
	elseifidni	<frac>,<1>	
	sar	b,4
	add	a,b
	elseifidni	<frac>,<2>	
	sar	b,3
	add	a,b
	elseifidni	<frac>,<3>	
	sar	b,3
	add	a,b
	sar	b,1
	add	a,b
	elseifidni	<frac>,<4>	
	sar	b,2
	add	a,b
	elseifidni	<frac>,<5>	
	sar	b,2
	add	a,b
	sar	b,2
	add	a,b
	elseifidni	<frac>,<6>	
	sar	b,2
	add	a,b
	sar	b,1
	add	a,b
	elseifidni	<frac>,<7>	
	sar	b,2
	add	a,b
	sar	b,1
	add	a,b
	sar	b,1
	add	a,b
	elseifidni	<frac>,<8>	
	sar	b,1
	add	a,b
	elseifidni	<frac>,<9>	
	sar	b,1
	add	a,b
	sar	b,3
	add	a,b
	elseifidni	<frac>,<10>	
	sar	b,1
	add	a,b
	sar	b,2
	add	a,b
	elseifidni	<frac>,<11>	
	sar	b,1
	add	a,b
	sar	b,2
	add	a,b
	sar	b,1
	add	a,b
	elseifidni	<frac>,<12>	
	sar	b,1
	add	a,b
	sar	b,1
	add	a,b
	elseifidni	<frac>,<13>	
	sar	b,1
	add	a,b
	sar	b,1
	add	a,b
	sar	b,2
	add	a,b
	elseifidni	<frac>,<14>	
	sar	b,1
	add	a,b
	sar	b,1
	add	a,b
	sar	b,1
	add	a,b
	elseifidni	<frac>,<15>	
	sar	b,1
	add	a,b
	sar	b,1
	add	a,b
	sar	b,1
	add	a,b
	sar	b,1
	add	a,b
	elseifidni	<frac>,<16>
	add	a,b
	elseifidni	<frac>,<-1>	
	sar	b,4
	sub	a,b
	elseifidni	<frac>,<-2>	
	sar	b,3
	sub	a,b
	elseifidni	<frac>,<-3>	
	sar	b,3
	sub	a,b
	sar	b,1
	sub	a,b
	elseifidni	<frac>,<-4>	
	sar	b,2
	sub	a,b
	elseifidni	<frac>,<-5>	
	sar	b,2
	sub	a,b
	sar	b,2
	sub	a,b
	elseifidni	<frac>,<-6>	
	sar	b,2
	sub	a,b
	sar	b,1
	sub	a,b
	elseifidni	<frac>,<-7>	
	sar	b,2
	sub	a,b
	sar	b,1
	sub	a,b
	sar	b,1
	sub	a,b
	elseifidni	<frac>,<-8>	
	sar	b,1
	sub	a,b
	elseifidni	<frac>,<-9>	
	sar	b,1
	sub	a,b
	sar	b,3
	sub	a,b
	elseifidni	<frac>,<-10>	
	sar	b,1
	sub	a,b
	sar	b,2
	sub	a,b
	elseifidni	<frac>,<-11>	
	sar	b,1
	sub	a,b
	sar	b,2
	sub	a,b
	sar	b,1
	sub	a,b
	elseifidni	<frac>,<-12>	
	sar	b,1
	sub	a,b
	sar	b,1
	sub	a,b
	elseifidni	<frac>,<-13>	
	sar	b,1
	sub	a,b
	sar	b,1
	sub	a,b
	sar	b,2
	sub	a,b
	elseifidni	<frac>,<-14>	
	sar	b,1
	sub	a,b
	sar	b,1
	sub	a,b
	sar	b,1
	sub	a,b
	elseifidni	<frac>,<-15>	
	sar	b,1
	sub	a,b
	sar	b,1
	sub	a,b
	sar	b,1
	sub	a,b
	sar	b,1
	sub	a,b
	endif

	pop	b

	endm

; Dithered perspective

ScanLinePIZ2TPDn	macro	name,pre,incu,decu,incv,decv,post1,post2,\
		d0,d1,d2,d3
name	proc

	pushad

		mov	 esi,work.pu.current
		mov	 ebx,work.pv.current
		mov	 eax,work.pu.grad_x
		mov	 edi,work.pv.grad_x
		mov	 edx,work.pq.current

	mov	work.tsl.v_numerator,ebx
		mov	 work.tsl.du_numerator,eax
	mov	work.tsl.dv_numerator,edi
	mov	work.tsl.denominator,edx

		mov	 ecx,work.pz.current
		mov	 edx,work.tsl.zstart

		mov	 work.tsl.z,ecx
		mov	 work.tsl.zdest,edx

		mov	 eax,work.tsl.source
		mov	 edi,work.tsl.start
		mov	 ebp,work.tsl._end

	mov	source,eax
		mov	 work.tsl.dest,edi

		cmp	 edi,ebp
	je	L50
		ja	  rtol	; Jump if right to left	
	
		; Left to right scan
	
	and	edi,3
	jmp	table_f_&name&[edi*4]

	.data
table_f_&name&	label	ptr dword
	dd	init_forward0
	dd	init_forward1
	dd	init_forward2
	dd	init_forward3
	.code

	for	count,<0,1,2,3>
init_forward&count&:
		mov	edx,work.tsl.denominator
	mov	ebx,work.tsl.v_numerator

	if	1		; Dithering?

; Initialise forward dither
; For dithering need esi += edx*fraction
;		and ebx += edx*fraction

	ifidni	<count>,<0>
	dither	esi,edx,d0
	dither	ebx,edx,d0
	elseifidni	<count>,<1>
	dither	esi,edx,d1
	dither	ebx,edx,d1
	elseifidni	<count>,<2>
	dither	esi,edx,d2
	dither	ebx,edx,d2
	elseifidni	<count>,<3>
	dither	esi,edx,d3
	dither	ebx,edx,d3
	endif

	endif

	jmp	forward&count&
	endm

L33:
	for	count,<0,1,2,3>
	local	transp2,L34,L34a,L36,L37
	local	skipdecvf,L40

		mov	edx,work.tsl.denominator
	mov	ebx,work.tsl.v_numerator

forward&count&:
		mov	 ecx,work.pq.grad_x
		mov	 ebp,work.tsl.du_numerator
	mov	edi,work.tsl.dv_numerator
		mov	 eax,source

	pre
		cmp	 esi,edx
		jl	L36

		sub	 esi,edx		; u_numerator -= denominator
@@:
		incu
		add	 ebp,ecx		; du_numerator += d_denominator
		sub	 esi,edx		; u_numerator -= denominator
		jge	 @B

	add	esi,edx		; u_numerator += denominator
	
		mov	 work.tsl.du_numerator,ebp
	jmp	L37
	
L36:		test	esi,esi
	jge	L37

		mov	 ebp,work.tsl.du_numerator
@@:
		decu
		sub	 ebp,ecx		; du_numerator -= d_denominator
		add	 esi,edx		; u_numerator -= denominator
		jnc	 @B
	
		mov	 work.tsl.du_numerator,ebp
L37:
	test	ebx,ebx		; test v_numerator
	jge	skipdecvf

@@:	   
		decv
		sub	 edi,ecx		; dv_numerator -= d_denominator
		add	 ebx,edx		; v_numerator -= denominator
		jnc	 @B

	mov	work.tsl.dv_numerator,edi
	jmp	L40
skipdecvf:
		cmp	 ebx,edx		; Compare v_numerator and denominator
		jl	  L40		; Jump if proper fraction

		sub	 ebx,edx
	
@@:
	incv
		add	 edi,ecx
		sub	 ebx,edx
		jge	 @B
	
	add	ebx,edx
	mov	work.tsl.dv_numerator,edi
L40:
	post1
	post2

;		U,V and W increments

		sub	 esi,ebp		; u_numerator -= du_numerator
	sub	 ebx,edi		; v_numerator -= dv_numerator
		add	 edx,ecx		; demominator += d_denominator

	if	1		; Dithering?

; Dither forwards
; For dithering need esi (u_numerator) += edx*fraction
;		and ebx (v_numerator) += edx*fraction

	ifidni	<count>,<0>
	dither	esi,edx,%d1-d0
	dither	ebx,edx,%d1-d0
	elseifidni	<count>,<1>
	dither	esi,edx,%d2-d1
	dither	ebx,edx,%d2-d1
	elseifidni	<count>,<2>
	dither	esi,edx,%d3-d2
	dither	ebx,edx,%d3-d2
	elseifidni	<count>,<3>
	dither	esi,edx,%d0-d3
	dither	ebx,edx,%d0-d3
	endif

	endif

	mov	work.tsl.denominator,edx
	mov	work.tsl.v_numerator,ebx

; End of calcs 

	mov	edi,work.tsl.dest
	mov	ebp,work.tsl._end
		cmp	 edi,ebp
	jae	L50

	mov	 ebp,work.tsl.zdest
		mov	 source,eax	; Save source
	mov	ebx,work.tsl.z		; Get new z value
	mov	dx,[ebp]
	ror	ebx,16
		cmp	 bx,dx
		ja	  @F		; Jump if new pixel behind old
	
		mov	 edx,work.texture.base
	add	eax,edx

	;; Fetch texel and test for transparency

	mov	cl,[eax]
if 1
	test	cl,cl		; Transparent?
	jz	@F
endif

	;; Store texel and z value

		mov	 [edi],cl	; Store texel
		mov	 [ebp],bx	; Store new z
@@:
		add	 ebp,2		; zdest++
	ror	ebx,16
		mov	 ecx,work.pz.grad_x
		add	 ebx,ecx		; z += dz
L34a:
	mov	work.tsl.zdest,ebp
	mov	work.tsl.z,ebx

	inc	edi
	cmp	edi,work.tsl._end
	jae	L50
	mov	work.tsl.dest,edi

	endm

	jmp	L33

	; Right to left scan

rtol:
	and	edi,3
	jmp	table_b_&name&[edi*4]

	.data
table_b_&name&	label	ptr dword
	dd	init_backward0
	dd	init_backward1
	dd	init_backward2
	dd	init_backward3
	.code

	for	count,<0,1,2,3>
init_backward&count&:
		mov	edx,work.tsl.denominator
	mov	ebx,work.tsl.v_numerator

	if	1		; Dithering?

; Initialise backward dither
; For dithering need esi += edx*fraction
;		and ebx += edx*fraction

	ifidni	<count>,<0>
	dither	esi,edx,d0
	dither	ebx,edx,d0
	elseifidni	<count>,<1>
	dither	esi,edx,d1
	dither	ebx,edx,d1
	elseifidni	<count>,<2>
	dither	esi,edx,d2
	dither	ebx,edx,d2
	elseifidni	<count>,<3>
	dither	esi,edx,d3
	dither	ebx,edx,d3
	endif

	endif

	jmp	backward&count&
	endm
R33:
	for	count,<3,2,1,0>
	local	R33,transp2r,R34,R34a
	local	R36,R37
	local	skipdecvb,R40

	mov	edx,work.tsl.denominator
	mov	ebx,work.tsl.v_numerator

backward&count&:
		mov	 ecx,work.pq.grad_x
		mov	 ebp,work.tsl.du_numerator
	mov	edi,work.tsl.dv_numerator
		mov	 eax,source	

	pre
		cmp	 esi,edx
		jl	  R36

		sub	 esi,edx		; u_numerator -= denominator
@@:
		incu
		add	 ebp,ecx
		sub	 esi,edx
		jge	 @B

	add	esi,edx

		mov	 work.tsl.du_numerator,ebp
	jmp	R37
	
R36:		test	esi,esi
	jge	R37

		mov	 ebp,work.tsl.du_numerator
@@:
		decu
		sub	 ebp,ecx		; du_numerator -= d_denominator
		add	 esi,edx		; u_numerator -= denominator
		jnc	 @B
	
		mov	 work.tsl.du_numerator,ebp
R37:
	test	ebx,ebx		; test v_numerator
	jge	skipdecvb

@@:	   
		decv
		sub	 edi,ecx
		add	 ebx,edx
		jnc	 @B

	mov	work.tsl.dv_numerator,edi
	jmp	R40
skipdecvb:
		cmp	 ebx,edx
		jl	  R40		; Jump if v not too small

		sub	 ebx,edx
	
@@:
	incv
		add	 edi,ecx
		sub	 ebx,edx
		jge	 @B
	
	add	ebx,edx
	mov	work.tsl.dv_numerator,edi
	
R40:
	post1
	post2

;		U,V and W increments

		add	 esi,ebp		; u_numerator += du_numerator
	add	 ebx,edi		; v_numerator += dv_numerator
		sub	 edx,ecx		; demominator -= d_denominator

	if	1		; Dithering?

; Dither backwards
; For dithering need esi (u_numerator) += edx*fraction
;		and ebx (v_numerator) += edx*fraction

	ifidni	<count>,<0>
	dither	esi,edx,%d3-d0
	dither	ebx,edx,%d3-d0
	elseifidni	<count>,<1>
	dither	esi,edx,%d0-d1
	dither	ebx,edx,%d0-d1
	elseifidni	<count>,<2>
	dither	esi,edx,%d1-d2
	dither	ebx,edx,%d1-d2
	elseifidni	<count>,<3>
	dither	esi,edx,%d2-d3
	dither	ebx,edx,%d2-d3
	endif

	endif
	mov	work.tsl.denominator,edx
	mov	work.tsl.v_numerator,ebx

; End of calcs

	mov	edi,work.tsl.dest
	mov	ebp,work.tsl._end
		cmp	 edi,ebp
	jbe	L50

	mov	 ebp,work.tsl.zdest
		mov	 source,eax	; Save source
	mov	ebx,work.tsl.z		; Get new z value
	mov	dx,[ebp]
	ror	ebx,16
		cmp	 bx,dx
		ja	  @F		; Jump if new pixel behind old
	
		mov	 edx,work.texture.base
	add	eax,edx

	;; Fetch texel and text for transparency

	mov	cl,[eax]
if 1
	test	cl,cl		; Transparent?
	jz	@F
endif

		mov	 [ebp],bx	; Store new z
		mov	 [edi],cl	; Store texel

@@:
		sub	 ebp,2		; zdest++
	ror	ebx,16
		mov	 ecx,work.pz.grad_x
		sub	 ebx,ecx
R34a:
	mov	work.tsl.zdest,ebp
	mov	work.tsl.z,ebx

	dec	edi
	cmp	edi,work.tsl._end
		mov	 work.tsl.dest,edi
	jbe	L50
	
	endm

	jmp	R33
	
L50:
	popad
	ret
	
name	endp

	endm

	; Dithered texture mapping 

ScanLinePIZ2TPD	macro	name,pre,incu,decu,incv,decv,post1,post2

ScanLinePIZ2TPDn	&name&_0,\
	pre,incu,decu,incv,decv,post1,post2,\
	13,1,4,16

ScanLinePIZ2TPDn	&name&_1,\
	pre,incu,decu,incv,decv,post1,post2,\
	8,12,9,5

ScanLinePIZ2TPDn	&name&_2,\
	pre,incu,decu,incv,decv,post1,post2,\
	10,6,7,11

ScanLinePIZ2TPDn	&name&_3,\
	pre,incu,decu,incv,decv,post1,post2,\
	3,15,14,2

	.data
&name&_table	label	ptr dword
	dd	&name&_0
	dd	&name&_1
	dd	&name&_2
	dd	&name&_3
	.code

name	proc

	mov	eax,work.tsl.y
	and	eax,3
	jmp	&name&_table[4*eax]

name	endp

	endm

	if PARTS and PART_DITHER

ScanLinePIZ2TPD	ScanLinePIZ2TPD64,\
	<<shl eax,2>>,\
	<<add al,100b>>,<<sub al,100b>>,<<inc ah>>,<<dec ah>>,\
	<<shr eax,2>>,<<and eax,0fffh>>

ScanLinePIZ2TPD	ScanLinePIZ2TPD128,\
	<<shl eax,1>>,\
	<<add al,10b>>,<<sub al,10b>>,<<inc ah>>,<<dec ah>>,\
	<<shr eax,1>>,<<and eax,3fffh>>

ScanLinePIZ2TPD	ScanLinePIZ2TPD256,\
	<<>>,\
	<<inc al>>,<<dec al>>,<<inc ah>>,<<dec ah>>,\
	<<>>,<<>>

ScanLinePIZ2TPD	ScanLinePIZ2TPD1024,\
	<<shl eax,6>>,\
	<<add ax,40h>>,<<sub ax,40h>>,<<add eax,10000h>>,<<sub eax,10000h>>,\
	<<shr eax,6>>,<<and eax,0fffffh>>

	endif

	end
