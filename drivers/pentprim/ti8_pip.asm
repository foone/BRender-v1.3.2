;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: ti8_pip.asm 1.1 1997/12/10 16:49:02 jon Exp $
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
z		dd	?
i		dd	?
du_numerator	dd	?
dv_numerator	dd	?
u_numerator	dd	?
v_numerator	dd	?
;zdest		dd	?
dest		dd	?
dest2		dd	?
source		dd	?
denominator	dd	?
dz		dd	?
_di		dd	?
ddenominator	dd	?

		align 4

		.code

; Smoothly lit perspective texture mapping 

TrapeziumPITIP	macro	name,pre,incu,decu,incv,decv,post1,post2

		public	Trapezium&name&

ScanLine&name&_table	dd	ScanLine&name&_rl_ud_vd
				    	dd	ScanLine&name&_rl_ui_vd
		            	dd	ScanLine&name&_rl_ud_vi
				    	dd	ScanLine&name&_rl_ui_vi
		            	dd	ScanLine&name&_lr_ui_vi
				    	dd	ScanLine&name&_lr_ud_vi
            			dd	ScanLine&name&_lr_ui_vd
		            	dd	ScanLine&name&_lr_ud_vd

ScanLinePITIP	ScanLine&name&_rl_ud_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,0,0,1,ScanLine&name&_done
ScanLinePITIP	ScanLine&name&_rl_ud_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,0,1,1,ScanLine&name&_done
ScanLinePITIP	ScanLine&name&_rl_ui_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,1,0,1,ScanLine&name&_done
ScanLinePITIP	ScanLine&name&_rl_ui_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,1,1,1,ScanLine&name&_done
ScanLinePITIP	ScanLine&name&_lr_ud_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,0,0,1,ScanLine&name&_done
ScanLinePITIP	ScanLine&name&_lr_ud_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,0,1,1,ScanLine&name&_done
ScanLinePITIP	ScanLine&name&_lr_ui_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,1,0,1,ScanLine&name&_done
ScanLinePITIP	ScanLine&name&_lr_ui_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,1,1,1,ScanLine&name&_done

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
		add	esi, edi				; esi = work.main.d_i + work.colour.stride_b

		add	edi, edi				; edi = 2 * work.main.d_i
		add	work.tsl._end, ebp			; work.tsl.end += work.top.d_i + work.colour.stride_b + carry

		mov	eax, work.main.d_f
		add	edi, work.depth.stride_b		; edi = 2 * work.main.d_i + work.depth.stride_b

		add	work.main.f, eax			; work.main.f += work.main.d_f
		mov	ebp, work.pi.d_nocarry

		cmp	work.main.f, eax			; carry = (unsigned)work.main.f < (unsigned)work.main.d_f
		jc	carry

		mov	eax, work.pq.d_nocarry
		sub	work.pu.current, ebx			; work.pu.current += -work.tsl.du_numerator_nocarry

		add	work.pq.current, eax			; work.pq.current += work.pq.d_nocarry
		sub	work.pv.current, ecx			; work.pv.current += -work.tsl.dv_numerator_nocarry

		mov	eax, work.tsl.start
		add	work.pi.current, ebp			; work.pi.current += work.pi.d_nocarry

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

		add	ebp, work.pi.grad_x
		add	esi, 1

		add	edi, 2
		add	work.pq.current, eax			; work.pq.current += work.pq.d_nocarry + work.pq.grad_x

		sub	work.pu.current, ebx			; work.pu.current += -work.tsl.du_numerator_nocarry + work.pu.grad_x
		sub	work.pv.current, ecx			; work.pv.current += -work.tsl.dv_numerator_nocarry + work.pv.grad_x

		mov	eax, work.tsl.start
		add	work.pi.current, ebp			; work.pi.current += work.pi.d_nocarry + work.pi.grad_x

		add	eax, esi
		add	work.tsl.zstart, edi			; work.tsl.zstart += 2 * work.main.d_i + work.depth.stride_b + 2

		mov	work.tsl.start, eax			; work.tsl.start += work.main.d_i + work.colour.stride_b + 1
		dec	work.top.count				; Decrement Y count

		jnz	outer_start				; Loop if not zero

		popad

		ret

Trapezium&name&	endp

		endm


ScanLinePITIP	macro	name,pre,incu,decu,incv,decv,post1,post2,LEFT_TO_RIGHT,U_INCREASING,V_INCREASING,TRANSPARENCY,done
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
		mov	edx, work.pi.current

		mov	edi,work.tsl.start
		mov	work.tsl.i, edx

		xor	ebx,ebx
		mov	work.tsl.dest,edi

		mov	bh,byte ptr (work.tsl.i+2)

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
		mov	edx,work.shade_table
		mov	bl,[eax+esi]

		mov	edi,work.tsl.dest
		mov	ebp,work.tsl._end

if LEFT_TO_RIGHT
		inc	edi
else
		dec	edi
endif
		mov	cl,[ebx+edx]

	; Test for transparency
	;
if TRANSPARENCY
		test	bl,bl
		jz	nodraw
endif

	; Look texel up in shade table, store texel
	;
if LEFT_TO_RIGHT
		mov	[edi-1],cl
else
		mov	[edi+1],cl
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
if LEFT_TO_RIGHT
		cmp	edi,ebp
		jae	done		; Exit as early as possible *** PREFIX (masked by cmp if paired)
else
		cmp	edi,ebp
		jbe	done		; Exit as early as possible *** PREFIX (masked by cmp if paired)
endif

	; Interpolate i
	;
		mov	ecx,work.tsl.i
		mov	edx,work.tsl._di

	; Prepare source offset for modification
	;
		pre

if LEFT_TO_RIGHT
		add	ecx,edx
else
		sub	ecx,edx
endif

		mov	work.tsl.dest,edi
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
			
		mov	work.tsl.denominator,edx
		xor	ebx,ebx

	; Interpolate v numerator
	;
if LEFT_TO_RIGHT
		sub	ecx,edi
else
		add	ecx,edi
endif

	; Check for v error going outside range 0..1
	;
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
		xor	ebx,ebx

		mov	bh,byte ptr (work.tsl.i+2)
		jmp	next_pixel

name	endp
		endm


FastScanLinePITIPFamily	macro	name,pre,incu,decu,incv,decv,post1,post2

		public	&name&_table

&name&_table	dd	&name&_rl_ui_vi
            	dd	&name&_rl_ui_vd
            	dd	&name&_rl_ud_vi
            	dd	&name&_rl_ud_vd
            	dd	&name&_lr_ud_vd
            	dd	&name&_lr_ud_vi
            	dd	&name&_lr_ui_vd
            	dd	&name&_lr_ui_vi

FastScanLinePITIP	&name&_rl_ud_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,0,0,1
FastScanLinePITIP	&name&_rl_ud_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,0,1,1
FastScanLinePITIP	&name&_rl_ui_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,1,0,1
FastScanLinePITIP	&name&_rl_ui_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,1,1,1
FastScanLinePITIP	&name&_lr_ud_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,0,0,1
FastScanLinePITIP	&name&_lr_ud_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,0,1,1
FastScanLinePITIP	&name&_lr_ui_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,1,0,1
FastScanLinePITIP	&name&_lr_ui_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,1,1,1

		endm

FastScanLinePITIP	macro	name,pre,incu,decu,incv,decv,post1,post2,LEFT_TO_RIGHT,U_INCREASING,V_INCREASING,TRANSPARENCY
name	proc

		pushad

	; Make temporary copies of parameters that change
	;
		mov	eax,work.pu.current
		mov	ebx,work.pu.grad_x

		mov	ecx,work.pq.current
		mov	edx,work.pq.grad_x

if U_INCREASING
		sub	eax,ecx	; u -= q	; Move error into range -1..0
		add	ebx,edx	; du += dq
endif

		mov	u_numerator,eax
		mov	du_numerator,ebx

		mov	eax,work.pv.current
		mov	ebx,work.pv.grad_x

if V_INCREASING
		sub	eax,ecx	; v -= q	; Move error into range -1..0
		add	ebx,edx	; dv += dq
endif

		mov	v_numerator,eax
		mov	dv_numerator,ebx

		mov	denominator,ecx
		mov	ddenominator,edx

		mov	eax,work.pi.current
		mov	ebx,work.pi.grad_x

		mov	i,eax
		mov	_di,ebx

	; Copy some parameters into registers
	;
		mov	eax,work.tsl.source
		mov	edi,work.tsl.start

	; Check for empty scans
	;
if 0
		mov	esi,work.tsl._end

		cmp	edi,esi
		je	done
endif
	
next_pixel:

	; Texel fetch and store section
	;
	; eax = source offset
	; ebx = 
	; ecx = texel
	; edx = shade table
	; esi = texture base
	; edi = dest
	; ebp = 

		xor	ecx,ecx

	; Get texel and intensity
	;
		mov	esi,work.texture.base
		mov	ch,byte ptr i+2

		mov	edx,work.shade_table
		mov	cl,[eax+esi]	; *** AGI, CACHE DELAYS

	; Test for transparency
	;
if TRANSPARENCY
		test	cl,cl
		jz	nodraw
endif

	; Look texel up in shade table, store texel
	;
		mov	cl,[ecx+edx]

		mov	[edi],cl
nodraw:

	; Linear interpolation section
	;
	; eax = source offset
	; ebx = 
	; ecx = i
	; edx = di
	; esi = 
	; edi = dest
	; ebp = 

	; Update destinations and check for end of scan
	;
if LEFT_TO_RIGHT
		inc	edi

		cmp	edi,work.tsl._end
		jae	done		; Exit as early as possible *** PREFIX (masked by cmp if paired)
else
		dec	edi

		cmp	edi,work.tsl._end
		jbe	done		; Exit as early as possible *** PREFIX (masked by cmp if paired)
endif

	; Interpolate i
	;
		mov	edx,_di

		mov	ecx,i

if LEFT_TO_RIGHT
		add	ecx,edx
else
		sub	ecx,edx
endif

		mov	dest,edi

		mov	i,ecx

	; Perspective interpolation section
	;
	; eax = source offset
	; ebx = u
	; ecx = v
	; edx = q
	; esi = du
	; edi = dv
	; ebp = dq
	;
		mov	edx,denominator
		mov	ebp,ddenominator

		mov	ebx,u_numerator
		mov	esi,du_numerator

	; Prepare source offset for modification
	;
		pre	; *** this will ruin pairing - try to move?

	; Interpolate u numerator and denominator
	;
if LEFT_TO_RIGHT
		add	edx,ebp
		sub	ebx,esi
else
		sub	edx,ebp
		add	ebx,esi
endif

		mov	ecx,v_numerator

	; Check for u error going outside range 0..1
	;
if U_INCREASING
		jl	doneu
	
	; Adjust u upward
	;
inculoop:
		incu
		add	esi,ebp

		sub	ebx,edx
		jge	inculoop
else
		jge	doneu

	; Adjust u downward
	;
deculoop:
		decu
		sub	esi,ebp

		add	ebx,edx
		jnc	deculoop
endif

		mov	du_numerator,esi
doneu:
		mov	edi,dv_numerator
		mov	u_numerator,ebx
			
	; Interpolate v numerator
	;
if LEFT_TO_RIGHT
		sub	ecx,edi
else
		add	ecx,edi
endif

		mov	denominator,edx

	; Check for v error going outside range 0..1
	;
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

		mov	dv_numerator,edi
donev:

	; Fix wrapping of source offset after modification
	;
		post1
		mov	v_numerator,ecx
		post2

		mov	edi,dest

		jmp	next_pixel

done:
		popad
		ret
	
name	endp
		endm


TrapeziumPITP	macro	name,pre,incu,decu,incv,decv,post1,post2

		public	Trapezium&name&

ScanLine&name&_table	dd	ScanLine&name&_rl_ud_vd
				    	dd	ScanLine&name&_rl_ui_vd
		            	dd	ScanLine&name&_rl_ud_vi
				    	dd	ScanLine&name&_rl_ui_vi
		            	dd	ScanLine&name&_lr_ui_vi
				    	dd	ScanLine&name&_lr_ud_vi
            			dd	ScanLine&name&_lr_ui_vd
		            	dd	ScanLine&name&_lr_ud_vd

ScanLinePITP	ScanLine&name&_rl_ud_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,0,0,1,ScanLine&name&_done
ScanLinePITP	ScanLine&name&_rl_ud_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,0,1,1,ScanLine&name&_done
ScanLinePITP	ScanLine&name&_rl_ui_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,1,0,1,ScanLine&name&_done
ScanLinePITP	ScanLine&name&_rl_ui_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,1,1,1,ScanLine&name&_done
ScanLinePITP	ScanLine&name&_lr_ud_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,0,0,1,ScanLine&name&_done
ScanLinePITP	ScanLine&name&_lr_ud_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,0,1,1,ScanLine&name&_done
ScanLinePITP	ScanLine&name&_lr_ui_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,1,0,1,ScanLine&name&_done
ScanLinePITP	ScanLine&name&_lr_ui_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,1,1,1,ScanLine&name&_done

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
		add	esi, edi				; esi = work.main.d_i + work.colour.stride_b

		add	edi, edi				; edi = 2 * work.main.d_i
		add	work.tsl._end, ebp			; work.tsl.end += work.top.d_i + work.colour.stride_b + carry

		mov	eax, work.main.d_f
		add	edi, work.depth.stride_b		; edi = 2 * work.main.d_i + work.depth.stride_b

		add	work.main.f, eax			; work.main.f += work.main.d_f
		vslot	; ****

		cmp	work.main.f, eax			; carry = (unsigned)work.main.f < (unsigned)work.main.d_f
		jc	carry

		mov	eax, work.pq.d_nocarry
		sub	work.pu.current, ebx			; work.pu.current += -work.tsl.du_numerator_nocarry

		add	work.pq.current, eax			; work.pq.current += work.pq.d_nocarry
		sub	work.pv.current, ecx			; work.pv.current += -work.tsl.dv_numerator_nocarry

		mov	eax, work.tsl.start
		vslot	; ****

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

		add	esi, 1
		add	edi, 2

		add	work.pq.current, eax			; work.pq.current += work.pq.d_nocarry + work.pq.grad_x
		sub	work.pu.current, ebx			; work.pu.current += -work.tsl.du_numerator_nocarry + work.pu.grad_x

		sub	work.pv.current, ecx			; work.pv.current += -work.tsl.dv_numerator_nocarry + work.pv.grad_x
		mov	eax, work.tsl.start

		add	eax, esi
		add	work.tsl.zstart, edi			; work.tsl.zstart += 2 * work.main.d_i + work.depth.stride_b + 2

		mov	work.tsl.start, eax			; work.tsl.start += work.main.d_i + work.colour.stride_b + 1
		dec	work.top.count				; Decrement Y count

		jnz	outer_start				; Loop if not zero

		popad

		ret

Trapezium&name&	endp

		endm


ScanLinePITP	macro	name,pre,incu,decu,incv,decv,post1,post2,LEFT_TO_RIGHT,U_INCREASING,V_INCREASING,TRANSPARENCY,done
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

		mov	work.tsl.dv_numerator, edi
		mov	esi,work.texture.base

		mov	work.tsl.v_numerator, ebp
		mov	edi,work.tsl.start

		mov	work.tsl.dest,edi
		vslot

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
		mov	cl,[eax+esi]
		mov	edi,work.tsl.dest

	; Test for transparency
	;
if TRANSPARENCY
		test	cl,cl
		jz	nodraw
endif

	; Store texel
	;
		mov	[edi],cl
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

	; Update destination and check for end of scan
	;
		mov	ebp,work.tsl._end
if LEFT_TO_RIGHT
		inc	edi

		cmp	edi,ebp
		jae	done		; Exit as early as possible *** PREFIX (masked by cmp if paired)
else
		dec	edi

		cmp	edi,ebp
		jbe	done		; Exit as early as possible *** PREFIX (masked by cmp if paired)
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

	; Prepare source offset for modification
	;
		pre
		mov	work.tsl.dest,edi

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
		jmp	next_pixel

name	endp
		endm


FastScanLinePITPFamily	macro	name,pre,incu,decu,incv,decv,post1,post2

		public	&name&_table

&name&_table	dd	&name&_rl_ui_vi
            	dd	&name&_rl_ui_vd
            	dd	&name&_rl_ud_vi
            	dd	&name&_rl_ud_vd
            	dd	&name&_lr_ud_vd
            	dd	&name&_lr_ud_vi
            	dd	&name&_lr_ui_vd
            	dd	&name&_lr_ui_vi

FastScanLinePITP	&name&_rl_ud_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,0,0,1
FastScanLinePITP	&name&_rl_ud_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,0,1,1
FastScanLinePITP	&name&_rl_ui_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,1,0,1
FastScanLinePITP	&name&_rl_ui_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,0,1,1,1
FastScanLinePITP	&name&_lr_ud_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,0,0,1
FastScanLinePITP	&name&_lr_ud_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,0,1,1
FastScanLinePITP	&name&_lr_ui_vd,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,1,0,1
FastScanLinePITP	&name&_lr_ui_vi,<pre>,<incu>,<decu>,<incv>,<decv>,<post1>,<post2>,1,1,1,1

		endm

FastScanLinePITP	macro	name,pre,incu,decu,incv,decv,post1,post2,LEFT_TO_RIGHT,U_INCREASING,V_INCREASING,TRANSPARENCY
name	proc

		pushad

	; Make temporary copies of parameters that change
	;
		mov	eax,work.pu.current
		mov	ebx,work.pu.grad_x

		mov	ecx,work.pq.current
		mov	edx,work.pq.grad_x

if U_INCREASING
		sub	eax,ecx	; u -= q	; Move error into range -1..0
		add	ebx,edx	; du += dq
endif

		mov	u_numerator,eax
		mov	du_numerator,ebx

		mov	eax,work.pv.current
		mov	ebx,work.pv.grad_x

if V_INCREASING
		sub	eax,ecx	; v -= q	; Move error into range -1..0
		add	ebx,edx	; dv += dq
endif

		mov	v_numerator,eax
		mov	dv_numerator,ebx

		mov	denominator,ecx
		mov	ddenominator,edx

	; Copy some parameters into registers
	;
		mov	eax,work.tsl.source
		mov	edi,work.tsl.start

	; Check for empty scans
	;
if 0
		mov	esi,work.tsl._end

		cmp	edi,esi
		je	done
endif
	
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
		mov	esi,work.texture.base

		mov	cl,[eax+esi]	; *** AGI, CACHE DELAYS

	; Test for transparency
	;
if TRANSPARENCY
		test	cl,cl
		jz	nodraw
endif

	; Store texel
	;

		mov	[edi],cl
nodraw:

	; Linear interpolation section
	;
	; eax = source offset
	; ebx = 
	; ecx = 
	; edx = 
	; esi = 
	; edi = dest
	; ebp = 

	; Update destinations and check for end of scan
	;
if LEFT_TO_RIGHT
		inc	edi

		cmp	edi,work.tsl._end
		jae	done		; Exit as early as possible *** PREFIX (masked by cmp if paired)
else
		dec	edi

		cmp	edi,work.tsl._end
		jbe	done		; Exit as early as possible *** PREFIX (masked by cmp if paired)
endif

		mov	dest,edi

	; Perspective interpolation section
	;
	; eax = source offset
	; ebx = u
	; ecx = v
	; edx = q
	; esi = du
	; edi = dv
	; ebp = dq
	;
		mov	edx,denominator
		mov	ebp,ddenominator

		mov	ebx,u_numerator
		mov	esi,du_numerator

	; Prepare source offset for modification
	;
		pre	; *** this will ruin pairing - try to move?

	; Interpolate u numerator and denominator
	;
if LEFT_TO_RIGHT
		add	edx,ebp
		sub	ebx,esi
else
		sub	edx,ebp
		add	ebx,esi
endif

		mov	ecx,v_numerator

	; Check for u error going outside range 0..1
	;
if U_INCREASING
		jl	doneu
	
	; Adjust u upward
	;
inculoop:
		incu
		add	esi,ebp

		sub	ebx,edx
		jge	inculoop
else
		jge	doneu

	; Adjust u downward
	;
deculoop:
		decu
		sub	esi,ebp

		add	ebx,edx
		jnc	deculoop
endif

		mov	du_numerator,esi
doneu:
		mov	edi,dv_numerator
		mov	u_numerator,ebx
			
	; Interpolate v numerator
	;
if LEFT_TO_RIGHT
		sub	ecx,edi
else
		add	ecx,edi
endif

		mov	denominator,edx

	; Check for v error going outside range 0..1
	;
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

		mov	dv_numerator,edi
donev:

	; Fix wrapping of source offset after modification
	;
		post1
		mov	v_numerator,ecx
		post2

		mov	edi,dest

		jmp	next_pixel

done:
		popad
		ret
	
name	endp
		endm


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

ScanLinePITPDn	macro	name,pre,incu,decu,incv,decv,post1,post2,\
			d0,d1,d2,d3
name		proc

		pushad

                mov     esi,work.pu.current
                mov     ebx,work.pv.current
                mov     eax,work.pu.grad_x
                mov     edi,work.pv.grad_x
                mov     edx,work.pq.current

		mov	v_numerator,ebx
                mov     du_numerator,eax
		mov	dv_numerator,edi
		mov	denominator,edx

                mov     eax,work.tsl.source
                mov     edi,work.tsl.start
                mov     ebp,work.tsl._end

		mov	source,eax
                mov     dest,edi

                cmp     edi,ebp
		je	L50
                ja      rtol		; Jump if right to left	
		
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
	        mov	edx,denominator
		mov	ebx,v_numerator

		if	1		; Dithering?

; Initialise forward dither
; For dithering need esi += edx*fraction
;                and ebx += edx*fraction

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

	        mov	edx,denominator
		mov	ebx,v_numerator

forward&count&:
                mov     ecx,work.pq.grad_x
                mov     ebp,du_numerator
		mov	edi,dv_numerator
                mov     eax,source

		pre
                cmp     esi,edx
                jl	L36

                sub     esi,edx		; u_numerator -= denominator
@@:
                incu
                add     ebp,ecx		; du_numerator += d_denominator
                sub     esi,edx		; u_numerator -= denominator
                jge     @B

		add	esi,edx		; u_numerator += denominator
		
                mov     du_numerator,ebp
		jmp	L37
		
L36:            test    esi,esi
		jge	L37

                mov     ebp,du_numerator
@@:
                decu
                sub     ebp,ecx		; du_numerator -= d_denominator
                add     esi,edx		; u_numerator -= denominator
                jnc     @B
		
                mov     du_numerator,ebp
L37:
		test	ebx,ebx		; test v_numerator
		jge	skipdecvf

@@:       
                decv
                sub     edi,ecx		; dv_numerator -= d_denominator
                add     ebx,edx		; v_numerator -= denominator
                jnc     @B

		mov	dv_numerator,edi
		jmp	L40
skipdecvf:
                cmp     ebx,edx		; Compare v_numerator and denominator
                jl      L40		; Jump if proper fraction

                sub     ebx,edx
		
@@:
		incv
                add     edi,ecx
                sub     ebx,edx
                jge     @B
		
		add	ebx,edx
		mov	dv_numerator,edi
L40:
		post1
		post2

;		U,V and W increments

                sub     esi,ebp		; u_numerator -= du_numerator
		sub     ebx,edi		; v_numerator -= dv_numerator
                add     edx,ecx		; demominator += d_denominator

		if	1		; Dithering?

; Dither forwards
; For dithering need esi (u_numerator) += edx*fraction
;                and ebx (v_numerator) += edx*fraction

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

		mov	denominator,edx
		mov	v_numerator,ebx

; End of calcs 

		mov	edi,dest
		mov	ebp,work.tsl._end
                cmp     edi,ebp
		jae	L50

                mov     source,eax	; Save source
		
                mov     edx,work.texture.base
		add	eax,edx

		;; Fetch texel and test for transparency

		mov	cl,[eax]
if 1
		test	cl,cl		; Transparent?
		jz	@F
endif

		;; Store texel and z value

                mov     [edi],cl	; Store texel
@@:
L34a:
		inc	edi
		cmp	edi,work.tsl._end
		jae	L50
		mov	dest,edi

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
	        mov	edx,denominator
		mov	ebx,v_numerator

		if	1		; Dithering?

; Initialise backward dither
; For dithering need esi += edx*fraction
;                and ebx += edx*fraction

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

		mov	edx,denominator
		mov	ebx,v_numerator

backward&count&:
                mov     ecx,work.pq.grad_x
                mov     ebp,du_numerator
		mov	edi,dv_numerator
                mov     eax,source	

		pre
                cmp     esi,edx
                jl      R36

                sub     esi,edx		; u_numerator -= denominator
@@:
                incu
                add     ebp,ecx
                sub     esi,edx
                jge     @B

		add	esi,edx

                mov     du_numerator,ebp
		jmp	R37
		
R36:            test    esi,esi
		jge	R37

                mov     ebp,du_numerator
@@:
                decu
                sub     ebp,ecx		; du_numerator -= d_denominator
                add     esi,edx		; u_numerator -= denominator
                jnc     @B
		
                mov     du_numerator,ebp
R37:
		test    ebx,ebx		; test v_numerator
		jge	skipdecvb

@@:       
                decv
                sub     edi,ecx
                add     ebx,edx
                jnc     @B

		mov	dv_numerator,edi
		jmp	R40
skipdecvb:
                cmp     ebx,edx
                jl      R40		; Jump if v not too small

                sub     ebx,edx
		
@@:
		incv
                add     edi,ecx
                sub     ebx,edx
                jge     @B
		
		add	ebx,edx
		mov	dv_numerator,edi
		
R40:
		post1
		post2

;		U,V and W increments

                add     esi,ebp		; u_numerator += du_numerator
		add     ebx,edi		; v_numerator += dv_numerator
                sub     edx,ecx		; demominator -= d_denominator

		if	1		; Dithering?

; Dither backwards
; For dithering need esi (u_numerator) += edx*fraction
;                and ebx (v_numerator) += edx*fraction

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
		mov	denominator,edx
		mov	v_numerator,ebx

; End of calcs

		mov	edi,dest
		mov	ebp,work.tsl._end
                cmp     edi,ebp
		jbe	L50

                mov     source,eax	; Save source
		mov	dx,[ebp]
		ror	ebx,16
		
                mov     edx,work.texture.base
		add	eax,edx

		;; Fetch texel and text for transparency

		mov	cl,[eax]
if 1
		test	cl,cl		; Transparent?
		jz	@F
endif

                mov     [edi],cl	; Store texel

@@:
R34a:
		dec	edi
		cmp	edi,work.tsl._end
                mov     dest,edi
		jbe	L50
		
		endm

		jmp	R33
		
L50:
		popad
		ret
		
name		endp

		endm

    ; Dithered texture mapping 

ScanLinePITPD	macro	name,pre,incu,decu,incv,decv,post1,post2

ScanLinePITPDn	&name&_0,\
		pre,incu,decu,incv,decv,post1,post2,\
		13,1,4,16

ScanLinePITPDn	&name&_1,\
		pre,incu,decu,incv,decv,post1,post2,\
		8,12,9,5

ScanLinePITPDn	&name&_2,\
		pre,incu,decu,incv,decv,post1,post2,\
		10,6,7,11

ScanLinePITPDn	&name&_3,\
		pre,incu,decu,incv,decv,post1,post2,\
		3,15,14,2

		.data
&name&_table		label	ptr dword
		dd	&name&_0
		dd	&name&_1
		dd	&name&_2
		dd	&name&_3
		.code

name		proc

		mov	eax,work.tsl.y
		and	eax,3
		jmp	&name&_table[4*eax]

name		endp

		endm

	if PARTS and PART_DITHER

ScanLinePITPD	ScanLinePITPD64,\
		<<shl eax,2>>,\
		<<add al,100b>>,<<sub al,100b>>,<<inc ah>>,<<dec ah>>,\
		<<shr eax,2>>,<<and eax,0fffh>>

ScanLinePITPD	ScanLinePITPD128,\
		<<shl eax,1>>,\
		<<add al,10b>>,<<sub al,10b>>,<<inc ah>>,<<dec ah>>,\
		<<shr eax,1>>,<<and eax,3fffh>>

ScanLinePITPD	ScanLinePITPD256,\
		<<>>,\
		<<inc al>>,<<dec al>>,<<inc ah>>,<<dec ah>>,\
		<<>>,<<>>

ScanLinePITPD	ScanLinePITPD1024,\
		<<shl eax,6>>,\
		<<add ax,40h>>,<<sub ax,40h>>,<<add eax,10000h>>,<<sub eax,10000h>>,\
		<<shr eax,6>>,<<and eax,0fffffh>>

	endif

		end

