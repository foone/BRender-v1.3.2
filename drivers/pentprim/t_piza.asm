;; Copyright (c) 1991,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: t_piza.asm 1.1 1997/12/10 16:49:18 jon Exp $
;; $Locker: $
;;
;; Trapezoid rendering for arbitary width texture mapping
;;

	.386p
	.model	c,flat
	include drv.inc

WRAP	equ	1
WRAPROW	equ	1
TRANSP	equ	1

	.data
noffset	dd	?

	.code

	; Macro to increment those parts of the per scan line
	; increment that depend on whether there has been a carry
	; from the fractional part of the current screen x coordinate

scan_inc	macro	carry,LIGHT,SBPP
	local	lab2

	if	LIGHT
	mov	ebx,work.pi.current
	endif	; LIGHT

	if	LIGHT
	mov	ecx,work.pi.d_&carry&
	endif	; LIGHT

	mov	esi,work.pz.current
	mov	edi,work.pz.d_&carry&
	add	esi,edi
	mov	work.pz.current,esi

	add	ebx,ecx
	mov	work.pi.current,ebx

	mov	ebx,work.awsl.u_current
	mov	ecx,work.awsl.du_&carry&
	mov	ebp,work.awsl.source_current
	mov	edi,work.awsl.dsource_&carry&
	add	ebx,ecx			; work.awsl.u_current += work.awsl.dsource_&carry&
	sbb	ecx,ecx			; Save -carry
	add	ebp,edi			; work.awsl.source_current += work.awsl.dsource_&carry&
	mov	work.awsl.u_current,ebx
	
	if WRAP
	mov	ebx,dword ptr work.awsl.u_int_current
	add	ebx,work.awsl.du_int_&carry&
	sub	ebx,ecx			; work.awsl.u_int_current += carry;
	mov	dword ptr work.awsl.u_int_current,ebx
	endif	; WRAP

	ifidni	<SBPP>,<2>
	shl	ecx,1
	elseifidni	<SBPP>,<3>
	lea	ecx,[ecx+ecx*2]
	elseifidni	<SBPP>,<4>
	shl	ecx,2
	endif	; <SBPP>

	sub	ebp,ecx			; work.awsl.source_current += carry*SBPP

	mov	ebx,work.awsl.v_current
	mov	ecx,work.awsl.dv_&carry&
	add	ebx,ecx
	mov	work.awsl.v_current,ebx
	jnc	lab2

	mov	edi,work.texture.stride_b
	add	ebp,edi
lab2:

	mov	work.awsl.source_current,ebp
	endm



TriangleRenderZ2	macro	fn,LIGHT,SBPP,DBPP,BUMP

fn	proc uses eax ebx ecx edx esi edi ebp

outer:
	mov	eax,work.awsl.edge
	mov	ebx,[eax].scan_edge.count
	test	ebx,ebx
	jz	exit
	dec	ebx				; work.awsl.edge->count--
	mov	[eax].scan_edge.count,ebx

	if	BUMP
	mov	ecx,work.bump.base
	sub	ecx,work.texture.base
	mov	noffset,ecx
	endif

	mov	edi,work.awsl.start
	mov	ebx,work.awsl._end
	cmp	edi,ebx
	mov	eax,work.awsl.u_current
	mov	ebx,work.awsl.v_current

	mov	al,byte ptr work.awsl.u_int_current

	mov	ecx,work.pz.current
	mov	work.pz.currentpix,ecx

	if	LIGHT
	mov	ecx,work.pi.current
	mov	work.pi.currentpix,ecx
	endif	; LIGHT

	mov	ah,byte ptr work.awsl.u_int_current+1

	mov	esi,work.awsl.source_current
	mov	ebp,work.awsl.zstart

	jge	scan_back

	; Forward scan
loopf:
	mov	ecx,work.awsl._end
	cmp	edi,ecx
	jge	next

	if	TRANSP

	ifidni	<SBPP>,<1>
	mov	dl,[esi]
	test	dl,dl
	jz	failf		; Skip if transparent
	elseifidni	<SBPP>,<2>
	mov	dl,[esi]
	mov	dh,1[esi]
	or	dl,dh
	jz	failf
	elseifidni	<SBPP>,<3>
	mov	dl,[esi]
	mov	dh,1[esi]
	or	dl,dh
	mov	dh,2[esi]
	or	dl,dh
	jz	failf
	endif	; <SBPP>

	endif	; TRANSP

	; 16 bit z test 

	mov	bl,byte ptr work.pz.currentpix+2
	mov	dl,[ebp]
	mov	bh,byte ptr work.pz.currentpix+3
	mov	dh,1[ebp]
	cmp	dl,bl
	sbb	dh,bh
	jb	failf			; Skip if pixel failed z test

	if	LIGHT

	ifidni	<DBPP>,<1>
	xor	ecx,ecx
	mov	[ebp],bl		; Store low byte of z

	mov	ch,byte ptr work.pi.currentpix+2	; Get lighting index
	mov	edx,work.shade_table
	mov	cl,[esi]		; Get texel
	mov	1[ebp],bh		; Store high byte of z
	add	edx,ecx
	mov	ecx,work.pz.currentpix
	add	ebp,2			; zptr += 2
	mov	bl,[edx]		; Light texel
	mov	edx,work.pz.grad_x	; z += dz
	mov	[edi],bl		; Store texel

	add	ecx,edx
	inc	edi			; ptr++

	elseifidni <DBPP>,<2>

	xor	ecx,ecx
	mov	[ebp],bx		; Store low byte of z

	mov	ch,byte ptr work.pi.currentpix+2	; Get lighting index
	mov	cl,[esi]		; Get texel
	mov	edx,work.shade_table
	add	ebp,2			; zptr += 2
	mov	bx,[edx+ecx*2]		; Light texel
	mov	ecx,work.pz.currentpix
	mov	edx,work.pz.grad_x	; z += dz
	mov	[edi],bx		; Store texel

	add	ecx,edx
	lea	edi,2[edi]		; ptr++

	elseifidni <DBPP>,<3>

	xor	ecx,ecx
	mov	[ebp],bx		; Store low byte of z

	mov	ch,byte ptr work.pi.currentpix+2	; Get lighting index
	mov	cl,[esi]		; Get texel
	mov	edx,work.shade_table
	add	ebp,2			; zptr += 2
	mov	bx,[edx+ecx*4]		; Light texel
	mov	[edi],bx		; Store texel
	mov	bl,2[edx+ecx*4]		; Light texel
	mov	2[edi],bl		; Store texel
	mov	ecx,work.pz.currentpix
	mov	edx,work.pz.grad_x	; z += dz

	add	ecx,edx
	lea	edi,3[edi]		; ptr++

	else	; <DBPP>
	SHOULDN'T GET HERE
	add	edi,DBPP
	endif	; <DBPP>

	mov	edx,work.pi.currentpix
	mov	work.pz.currentpix,ecx
	mov	ecx,work.pi.grad_x	; i += di
	add	edx,ecx
	mov	work.pi.currentpix,edx

	jmp	contf

failf:
	add	ebp,2			; zptr += 2

	ifidni	<DBPP>,<1>
	inc	edi			; ptr++
	else	; <DBPP>
	add	edi,DBPP
	endif	; <DBPP>

	mov	ecx,work.pz.currentpix
	mov	edx,work.pz.grad_x	; z += dz
	add	ecx,edx
	mov	edx,work.pi.currentpix
	mov	work.pz.currentpix,ecx
	mov	ecx,work.pi.grad_x	; i += di
	add	edx,ecx
	mov	work.pi.currentpix,edx

	elseif	BUMP	

	; Bump mapping

	mov	[ebp],bl		; Store low byte of z

	xor	ecx,ecx
	mov	cl,[esi]		; Get texel
	mov	edx,noffset
	mov	dl,[esi+edx]		; Get normal
	and	edx,0ffh
	add	edx,work.lighting_table	; Calculate light level from normal
	mov	ch,[edx]
	mov	edx,work.shade_table
	mov	cl,[edx+ecx]		; Light texel
	mov	1[ebp],bh		; Store high byte of z
	mov	[edi],cl

	mov	ecx,work.pz.currentpix
	add	ebp,2			; zptr += 2
	mov	edx,work.pz.grad_x	

	add	ecx,edx			; z += dz

	ifidni	<DBPP>,<1>
	inc	edi			; ptr++
	else	; <DBPP>
	add	edi,DBPP
	endif	; <DBPP>

	mov	work.pz.currentpix,ecx
	jmp	contf

failf:
	add	ebp,2			; zptr += 2

	ifidni	<DBPP>,<1>
	inc	edi			; ptr++
	else	; <DBPP>
	add	edi,DBPP
	endif	; <DBPP>

	mov	ecx,work.pz.currentpix
	mov	edx,work.pz.grad_x	; z += dz
	add	ecx,edx
	mov	work.pz.currentpix,ecx

	else	; LIGHT

	; No lighting

	mov	[ebp],bl		; Store low byte of z

	ifidni	<SBPP>,<1>

	mov	cl,[esi]		; Get texel
	mov	1[ebp],bh		; Store high byte of z
	mov	[edi],cl

	elseifidni	<SBPP>,<2>

	mov	cl,[esi]
	mov	1[ebp],bh		; Store high byte of z
	mov	ch,1[esi]
	mov	[edi],cl
	mov	1[edi],ch

	elseifidni	<SBPP>,<3>

	mov	cl,[esi]		; Get texel
	mov	1[ebp],bh		; Store high byte of z
	mov	[edi],cl
	mov	ch,1[esi]
	mov	1[edi],ch		; Store green
	mov	cl,2[esi]
	mov	2[edi],cl		; Store red

	elseifidni	<SBPP>,<4>

	mov	ecx,[esi]
	mov	1[ebp],bh
	mov	[edi],ecx

	endif	; <SBPP>

	mov	ecx,work.pz.currentpix
	add	ebp,2			; zptr += 2
	mov	edx,work.pz.grad_x	

	add	ecx,edx			; z += dz

	ifidni	<DBPP>,<1>
	inc	edi			; ptr++
	else	; <DBPP>
	add	edi,DBPP
	endif	; <DBPP>

	mov	work.pz.currentpix,ecx
	jmp	contf

failf:
	add	ebp,2			; zptr += 2

	ifidni	<DBPP>,<1>
	inc	edi			; ptr++
	else	; <DBPP>
	add	edi,DBPP
	endif	; <DBPP>

	mov	ecx,work.pz.currentpix
	mov	edx,work.pz.grad_x	; z += dz
	add	ecx,edx
	mov	work.pz.currentpix,ecx

	endif	; LIGHT

contf:

	; Redo this with high and low halfwords of du swapped

	mov	ecx,work.awsl.du
	add	eax,ecx			; u += du
	mov	ecx,work.awsl.dsource

	sbb	edx,edx			; edx = -carry
	add	esi,ecx
	mov	ecx,work.awsl.du_int
	sub	eax,edx			; work.awsl.du += carry

	ifidni	<SBPP>,<2>
	shl	edx,1
	elseifidni	<SBPP>,<3>
	lea	edx,[edx+2*edx]
	elseifidni	<SBPP>,<4>
	shl	edx,2
	endif	; <SBPP>

	sub	esi,edx			; esi += carry
	add	eax,ecx			; u_int += du_int

	mov	ecx,work.awsl.dv
	mov	edx,work.texture.stride_b
	add	ebx,ecx
	jnc	nostridef

	add	esi,edx

nostridef:
	test	ah,ah
	jl	nowidthf

	mov	ecx,work.texture.width_p
	add	eax,ecx

	ifidni	<SBPP>,<1>
	add	esi,ecx
	elseifidni	<SBPP>,<2>
	lea	esi,[esi+2*ecx]
	elseifidni	<SBPP>,<3>
	lea	ecx,[ecx+2*ecx]
	add	esi,ecx
	elseifidni	<SBPP>,<4>
	lea	esi,[esi+4*ecx]
	endif	; <SBPP>

nowidthf:
	mov	ecx,work.texture.base
	cmp	esi,ecx			; Have we underrun?
	jge	nosizef

	mov	ecx,work.texture._size
	add	esi,ecx

nosizef:

	jmp	loopf

scan_back:
	; Backwards scan
loopb:
	mov	ecx,work.awsl._end
	cmp	edi,ecx
	jle	next

	mov	ecx,work.pz.currentpix
	mov	edx,work.pz.grad_x	; z += dz
	sub	ecx,edx

	if	LIGHT
	mov	edx,work.pi.currentpix
	endif

	mov	work.pz.currentpix,ecx

	if	LIGHT
	mov	ecx,work.pi.grad_x	; i += di
	sub	edx,ecx
	mov	work.pi.currentpix,edx
	endif

	; Redo this with high and low halfwords of du swapped

	mov	ecx,work.awsl.du
	add	eax,ecx			; u += du
	mov	ecx,work.awsl.dsource

	sbb	edx,edx			; edx = -carry
	add	esi,ecx
	mov	ecx,work.awsl.du_int
	sub	eax,edx
	add	eax,ecx			; u_int += du_int

	ifidni	<SBPP>,<1>
	sub	esi,edx
	elseifidni	<SBPP>,<2>
	shl	edx,1
	sub	esi,edx
	elseifidni	<SBPP>,<3>
	lea	edx,[edx+edx*2]
	sub	esi,edx
	elseifidni	<SBPP>,<4>
	shl	edx,2
	sub	esi,edx
	endif

	mov	ecx,work.awsl.dv
	add	ebx,ecx
	jnc	nostrideb

	mov	ecx,work.texture.stride_b
	add	esi,ecx

nostrideb:

	test	ah,ah
	jl	nowidthb

	mov	ecx,work.texture.width_p
	add	eax,ecx

	ifidni	<SBPP>,<1>
	add	esi,ecx
	elseifidni	<SBPP>,<2>
	lea	esi,[esi+2*ecx]
	elseifidni	<SBPP>,<3>
	lea	ecx,[ecx+2*ecx]
	add	esi,ecx
	elseifidni	<SBPP>,<4>
	lea	esi,[esi+4*ecx]
	endif	; <SBPP>

nowidthb:
	mov	ecx,work.texture.base
	cmp	esi,ecx
	jge	nosizeb

	mov	ecx,work.texture._size
	add	esi,ecx

nosizeb:
	sub	ebp,2			; zptr += 2

	ifidni	<DBPP>,<1>
	dec	edi			; ptr += 1
	else	; <DBPP>
	sub	edi,DBPP
	endif	; <DBPP>

	if TRANSP

	ifidni <SBPP>,<1>
	mov	bl,[esi]
	test	bl,bl
	jz	loopb		; Skip if transparent
	elseifidni	<SBPP>,<2>
	mov	bl,[esi]
	mov	bh,1[esi]
	or	bl,bh
	jz	failb
	elseifidni	<SBPP>,<3>
	mov	bl,[esi]
	mov	bh,1[esi]
	or	bl,bh
	mov	bh,2[esi]
	or	bl,bh
	jz	failb
	endif	; <SBPP>

	endif	; <TRANSP>

	mov	dl,byte ptr work.pz.currentpix+2
	mov	bl,[ebp]
	mov	dh,byte ptr work.pz.currentpix+3
	mov	bh,1[ebp]
	sub	bl,dl
	sbb	bh,dh
	jb	loopb			; Skip if pixel failed z test

	mov	[ebp],dx		; Store z

	if	LIGHT
	xor	ecx,ecx
	mov	ch,byte ptr work.pi.currentpix+2
	mov	edx,work.shade_table
	endif	; <LIGHT>

	ifidni	<SBPP>,<1>

	mov	cl,[esi]		; Get texel

	ifidni	<DBPP>,<1>
	if	LIGHT
	mov	cl,[ecx+edx]		; Light texel
	endif

	if	BUMP
	and	ecx,0ffh
	mov	edx,noffset
	mov	dl,[esi+edx]
	and	edx,0ffh
	add	edx,work.lighting_table	; Calculate light level from normal
	mov	ch,[edx]
	mov	edx,work.shade_table
	mov	cl,[edx+ecx]		; Light texel
	endif

	mov	[edi],cl

	elseifidni	<DBPP>,<2>

        if      LIGHT
	mov	cx,[edx+ecx*2]
	mov	[edi],cx
	endif
	
	elseifidni	<DBPP>,<3>

	if	LIGHT
	mov	bx,[edx+ecx*4]
	mov	[edi],bx
	mov	bl,2[edx+ecx*4]
	mov	2[edi],bl
	endif

	endif	; DBPP

	elseifidni	<SBPP>,<2>

	mov	cl,[esi]
	mov	[edi],cl		; Store texel
	mov	cl,1[esi]
	mov	1[edi],cl		; Store texel

	elseifidni	<SBPP>,<3>

	mov	ecx,[esi]
	mov	[edi],cl		; Store texel
	mov	1[edi],ch		; Store texel
	ror	ecx,16
	mov	2[edi],cl		; Store texel

	elseifidni	<SBPP>,<4>

	mov	ecx,[esi]
	mov	[edi],ecx		; Store texel

	endif	; <SBPP>

failb:
	jmp	loopb

next:
	; Per scan line updates

	assume	eax:ptr scan_edge
	mov	eax,work.awsl.edge

	mov	ebx,[eax].f
	mov	ecx,[eax].d_f
	add	ebx,ecx			; edge->f += edge->d_f
	mov	[eax].f,ebx
	mov	ecx,[eax].d_i
	mov	ebx,work.awsl._end

	ifidni 	<DBPP>,<1>
	adc	ebx,ecx			; work.awsl._end += edge->d_i+carry
	elseifidni	<DBPP>,<2>
	adc	ecx,0
	shl	ecx,1
	add	ebx,ecx
	elseifidni	<DBPP>,<3>
	adc	ecx,0
	lea	ecx,[ecx+2*ecx]
	add	ebx,ecx			; work.awsl._end += 3*(edge->d_i+carry)
	elseifidni	<DBPP>,<4>
	adc	ecx,0
	shl	ecx,2
	add	ebx,ecx
	endif	; <DBPP>

	mov	work.awsl._end,ebx

	mov	ebx,work.main.f
	mov	ecx,work.main.d_f
	add	ebx,ecx
	mov	work.main.f,ebx
	mov	ebx,work.awsl.start
	mov	ecx,work.main.d_i
	mov	esi,work.awsl.zstart
	jnc	noscarry

	; if (carry) ...

	adc	ecx,0
	lea	esi,[esi+2*ecx]

	ifidni	<DBPP>,<1>
	add	ebx,ecx
	elseifidni	<DBPP>,<2>
	lea	ebx,[ebx+2*ecx]
	elseifidni	<DBPP>,<3>
	lea	ecx,[ecx+2*ecx]
	add	ebx,ecx
	elseifidni	<DBPP>,<4>
	lea	ebx,[ebx+4*ecx]
	endif

	mov	work.awsl.start,ebx
	mov	work.awsl.zstart,esi

	scan_inc	carry,LIGHT,SBPP

	jmp	cont

	; ... else ...
noscarry:

	lea	esi,[esi+2*ecx]

	ifidni	<DBPP>,<1>
	add	ebx,ecx
	elseifidni	<DBPP>,<2>
	lea	ebx,[ebx+2*ecx]
	elseifidni	<DBPP>,<3>
	lea	ecx,[ecx+2*ecx]
	add	ebx,ecx
	elseifidni	<DBPP>,<4>
	lea	ebx,[ebx+4*ecx]
	endif

	mov	work.awsl.start,ebx
	mov	work.awsl.zstart,esi

	scan_inc	nocarry,LIGHT,SBPP

	; ... endif

cont:
	mov	ebx,dword ptr work.awsl.u_int_current
	test	bh,bh			; Test sign of low halfword
	jl	lab1

	mov	ecx,work.texture.width_p
	add	ebx,ecx
	mov	dword ptr work.awsl.u_int_current,ebx
	mov	ebx,work.awsl.source_current

	ifidni	<SBPP>,<1>
	add	ebx,ecx
	elseifidni	<SBPP>,<2>
	lea	ebx,[ebx+2*ecx]
	elseifidni	<SBPP>,<3>
	lea	ecx,[ecx+2*ecx]
	add	ebx,ecx
	elseifidni	<SBPP>,<4>
	lea	ebx,[ebx+4*ecx]
	endif

	mov	work.awsl.source_current,ebx

lab1:
	mov	ebx,work.awsl.source_current
	mov	ecx,work.texture.base
	cmp	ebx,ecx
	jge	outer

	mov	ecx,work.texture._size
	add	ebx,ecx
	mov	work.awsl.source_current,ebx

	jmp	outer

exit:
	ret

fn	endp

	endm

	if PARTS and PART_8Z
TriangleRenderZ2	TrapezoidRenderPIZ2TA,0,1,1,0
	endif

	if PARTS and PART_BUMP
TriangleRenderZ2	TrapezoidRenderPIZ2TAN,0,1,1,1
	endif

	if PARTS and PART_8Z
TriangleRenderZ2	TrapezoidRenderPIZ2TIA,1,1,1,0
	endif

	if PARTS and PART_15Z
TriangleRenderZ2	TrapezoidRenderPIZ2TA15,0,2,2,0
	endif

	if PARTS and PART_24Z
TriangleRenderZ2	TrapezoidRenderPIZ2TA24,0,3,3,0
	endif

	if PARTS and PART_15Z
TriangleRenderZ2	TrapezoidRenderPIZ2TIA_RGB_555,1,1,2,0
	endif

	if PARTS and PART_24Z
TriangleRenderZ2	TrapezoidRenderPIZ2TIA_RGB_888,1,1,3,0
	endif
	end
