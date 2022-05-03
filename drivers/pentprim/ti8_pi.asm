;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: ti8_pi.asm 1.1 1997/12/10 16:49:00 jon Exp $
;; $Locker: $
;;
;; 8 bit Indexed mode
;;
;; Triangle scan convertion and rendering
;;
;; Perfect scan, integer vertices
;;
	.386p
	.model flat,c

	include	drv.inc
	include 586_macs.inc

USE_RCP		equ	1

if 1
SHOW_CODESIZE	macro _name,_total
		%out _name = _total
		endm
else
SHOW_CODESIZE		macro x,y
		endm
endif

X	equ 0
Y	equ 1
Z	equ 2

_X	equ 0
_Y	equ 4
_Z	equ 8

;; Setup some macros that are used to put colour and texture buffers in different
;; segments
;;
if 0
CSEG		equ	es:
DSEG		equ
TSEG		equ
SETUP_SEGS	macro
		push	es
		mov	es,word ptr work.colour.base+4
		endm
RESTORE_SEGS	macro
		pop	es
		endm
else
CSEG		equ
DSEG		equ
TSEG		equ
SETUP_SEGS	macro
		endm
RESTORE_SEGS	macro
		endm
endif


;; Various macros for generating the triangle renderers
;;

; Divide edx:eax by g_divisor with no overflow
;
; Assumes Z flag has sign of numerator
;
; Trashes ebx
;
;
GDIVIDE		macro	dirn
		local	negative,done,overflow

		mov	ebx,g_divisor
		js	negative
		cmp	ebx,edx
		jbe	overflow
	 	div	ebx
	ifidni <dirn>,<b>
		neg	eax
	endif
		jmp	done

negative:	neg	edx
		neg	eax
		sbb	edx,0
		cmp	ebx,edx
		jbe	overflow
	 	div	ebx
	ifidni <dirn>,<f>
		neg	eax
	endif
overflow:
done:
		endm

; Comparison that change sense with rendering direction
;
CMP_f		macro	a,b
		cmp	a,b
		endm

CMP_b		macro	a,b
		cmp	b,a
		endm

; Equates for the vertex coordinates
;
if SCREEN_FIXED
V0_X		equ	<vertex_0[_X]>
V0_Y		equ	<vertex_0[_Y]>
V1_X		equ	<vertex_1[_X]>
V1_Y		equ	<vertex_1[_Y]>
V2_X		equ	<vertex_2[_X]>
V2_Y		equ	<vertex_2[_Y]>
else
V0_X		equ	<[esi].brp_vertex.comp[C_SX*4]>
V0_Y		equ	<[esi].brp_vertex.comp[C_SY*4]>
V1_X		equ	<[edi].brp_vertex.comp[C_SX*4]>
V1_Y		equ	<[edi].brp_vertex.comp[C_SY*4]>
V2_X		equ	<[ebp].brp_vertex.comp[C_SX*4]>
V2_Y		equ	<[ebp].brp_vertex.comp[C_SY*4]>
endif

; Setup of an integer, perfect scan triangle  
;
; Leave g_divisor in eax, with sign of g_divisor in flags
;
SETUP_PI	macro

_setup_start = $

	; Get Y components of vertices
	;
		mov	eax,[esi].brp_vertex.comp[C_SY*4]
		mov	ebx,[edi].brp_vertex.comp[C_SY*4]
		mov	ecx,[ebp].brp_vertex.comp[C_SY*4]

	; Sort pointers and values in order of y value
	;
		cmp	eax,ebx
		jg	short sort_3
		cmp	ebx,ecx
		jle	short sort_done
	; abc
		cmp	eax,ecx
		jg	short sort_2
	; acb
		SWAP	edi,ebp
		jmp	short sort_done
sort_2:
	; cab
		SWAP	edi,ebp
		SWAP	esi,edi
		jmp	short sort_done
sort_3:
		cmp	ecx,ebx
		jg	short sort_4
	; cba
		SWAP	esi,ebp
		jmp	short sort_done

sort_4:		cmp	eax,ecx
		jg	short sort_5
	; bac
		SWAP	esi,edi
		jmp	short sort_done
sort_5:
	; bca
		SWAP	esi,edi
		SWAP	edi,ebp
sort_done:

if SCREEN_FIXED
	; Work out integer vertex values
	;
	; XXX Should really do this at projection time
	;
		mov	eax,[esi].brp_vertex.comp[C_SX*4]
		mov	ebx,[edi].brp_vertex.comp[C_SX*4]
		mov	ecx,[ebp].brp_vertex.comp[C_SX*4]
		sar	eax,16
		sar	ebx,16
		sar	ecx,16
		mov	vertex_0[_X],eax
		mov	vertex_1[_X],ebx
		mov	vertex_2[_X],ecx

		mov	eax,[esi].brp_vertex.comp[C_SY*4]
		mov	ebx,[edi].brp_vertex.comp[C_SY*4]
		mov	ecx,[ebp].brp_vertex.comp[C_SY*4]
		sar	eax,16
		sar	ebx,16
		sar	ecx,16
		mov	vertex_0[_Y],eax
		mov	vertex_1[_Y],ebx
		mov	vertex_2[_Y],ecx
endif

	; Work out deltas and gradients along edges
	;

		; long edge
		;
		mov	ebx,V2_Y	; work.main.y
		sub	ebx,V0_Y
		je	quit		; Ignore zero height polys
		mov	work.main.y,ebx

if USE_RCP
		mov	eax,V2_X	; work.main.x
		sub	eax,V0_X
		mov	work.main.x,eax
					; work.main.grad = work.main.x/work.main.y
		imul	_reciprocal[ebx*4]
else
		mov	edx,V2_X	; work.main.x
		sub	edx,V0_X
		mov	work.main.x,edx

		FIX_DIV
		idiv	ebx
endif
		mov	work.main.grad,eax
		mov	word ptr work.main.d_f+2,ax
		sar	eax,16
		add	eax,work.colour.stride_b
		mov	work.main.d_i,eax

		; top short edge
		;
if USE_RCP
		mov	eax,V1_X		; work.top.x
		sub	eax,V0_X
		mov	work.top.x,eax

		mov	ebx,V1_Y		; work.top.y
		sub	ebx,V0_Y
		mov	work.top.y,ebx
		mov	work.top.count,ebx
		je	short no_top

		imul	_reciprocal[ebx*4]
else
		mov	edx,V1_X		; work.top.x
		sub	edx,V0_X
		mov	work.top.x,edx

		mov	ebx,V1_Y		; work.top.y
		sub	ebx,V0_Y
		mov	work.top.y,ebx
		mov	work.top.count,ebx
		je	short no_top

		FIX_DIV				; work.top.grad = work.top.x/work.top.y
		idiv	ebx
endif

		mov	work.top.grad,eax
		mov	word ptr work.top.d_f+2,ax
		sar	eax,16
		add	eax,work.colour.stride_b
		mov	work.top.d_i,eax
no_top:
		; bottom short edge (don't need to save deltas)
		;
		mov	ebx,V2_Y		; work.bot.y
		sub	ebx,V1_Y
		mov	work.bot.count,ebx
		je	short no_bottom

if USE_RCP
		mov	eax,V2_X		; work.bot.x
		sub	eax,V1_X

		imul	_reciprocal[ebx*4]
else
		mov	edx,V2_X		; work.bot.x
		sub	edx,V1_X

		FIX_DIV				; work.bot.grad = work.bot.x/work.bot.y
		idiv	ebx
endif

		mov	work.bot.grad,eax
		mov	word ptr work.bot.d_f+2,ax
		sar	eax,16
		add	eax,work.colour.stride_b
		mov	work.bot.d_i,eax

no_bottom:

	; Work out start values for edges as pixel offsets
	;
if 1
		mov	ebx,work.colour.stride_b
		mov	ecx,dword ptr work.colour.base

		mov	eax,V0_Y
		imul	ebx
		add	eax,V0_X

		mov	work.main.f,080000000h
		mov	work.main.i,eax
		add	eax,ecx
		mov	work.top.f,080000000h
		mov	work.top.i,eax

		mov	eax,V1_Y
		imul	ebx
		add	eax,V1_X
else
		lea	ebx,work.row_table
		mov	ecx,dword ptr work.colour.base

		mov	eax,V0_Y
		mov	eax,[ebx+eax*4]
		add	eax,V0_X

		mov	work.main.f,080000000h
		mov	work.main.i,eax
		add	eax,ecx
		mov	work.top.f,080000000h
		mov	work.top.i,eax

		mov	eax,V1_Y
		mov	eax,[ebx+eax*4]
		add	eax,V1_X
endif

		add	eax,ecx
		mov	work.bot.f,080000000h
		mov	work.bot.i,eax

	; Work out divisor for use in parameter gradient calcs.
	;
		mov	eax,work.main.x
		imul	work.top.y
		mov	ebx,eax
		mov	eax,work.top.x
		imul	work.main.y
		sub	eax,ebx

		je	quit			; quit if g_divisor is 0

						; Leave g_divisor in eax
		endm

; Per parameter initialisation of perfect scan, integer vertex triangle
;
PARAM_PI_DIRN macro vparam,param,dirn

	; d_p_x = (d_p_1.work.main.y - d_p_2.work.top.y)/g_denom
	;
		mov	eax,[ebp].brp_vertex.vparam	; d_p_2
		sub	eax,[esi].brp_vertex.vparam
		push	eax
		imul	work.top.y
		mov	ebx,eax
		mov	ecx,edx

		mov	eax,[edi].brp_vertex.vparam	; d_p_1
		sub	eax,[esi].brp_vertex.vparam
		push	eax
		imul	work.main.y
		sub	eax,ebx
		sbb	edx,ecx

		GDIVIDE dirn

		mov	param.grad_x,eax

	; d_p_y = (d_p_2.work.top.x - d_p_1.work.main.x)/g_denom
	;
		pop	eax				; d_p_1
		imul	work.main.x
		mov	ebx,eax
		mov	ecx,edx
		pop	eax				; d_p_2
		imul	work.top.x
		sub	eax,ebx
		sbb	edx,ecx

		GDIVIDE dirn

		mov	param.grad_y,eax

	; Initialise parameter  from top vertex
	;
		mov	eax,[esi].brp_vertex.vparam

		mov	ebx,param.grad_x
		sar	ebx,1
		adc	eax,ebx		

		mov	param.current,eax

	; Work out parameter increments per scanline
	;
		movsx	eax,word ptr work.main.grad+2
		imul	param.grad_x
		add	eax,param.grad_y
		mov	param.d_nocarry,eax
		add	eax,param.grad_x
		mov	param.d_carry,eax
		endm

	.data
		db '$Id: ti8_pi.asm 1.1 1997/12/10 16:49:00 jon Exp $',0
		align 4

	; reciprocal table from fixed386.asm
	;
		extern _reciprocal:dword

p0_offset_x	dd	0	; Offset of start pixel centre from vertex 0
p0_offset_y	dd	0

temp_i		dd	0
temp_u		dd	0
temp_v		dd	0
temp_r		dd	0
temp_g		dd	0
temp_b		dd	0
temp_y		dd	0

	; Integer vertex poitisions
	;
vertex_0	dd	2 dup(?)
vertex_1	dd	2 dup(?)
vertex_2	dd	2 dup(?)

g_divisor	dd	0	; Bottom of param. gradient calc.

line_state_dst1	dd	0

	.code
	if PARTS and PART_8

; void TriangleRenderPI(struct br_vertex *pvertex_0,
;			struct br_vertex *pvertex_1,
;			struct br_vertex *pvertex_2);
;
; C stub directly to triangle renderer
;
TriangleRenderPI proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

	; Get pointers to vertex structures and colour
	;
	; eax = colour
	; esi = vertex 0
	; edi = vertex 1
	; ebp = vertex 2
	;
		push	ebp
		SETUP_SEGS

		mov	esi,pvertex_0
		mov	edi,pvertex_1
		mov	ebp,pvertex_2
		call	RawTriangle_PI

		RESTORE_SEGS
		pop	ebp
		ret
TriangleRenderPI endp


; Trapezoid loop for flat shaded z (16 bit) buffered faces
;
; Arguments control:
;	whether loop uses 'top' or 'bottom' variables
;	which direction scanline are rendered from the long edge
;

TRAPEZOID_PI	macro	half,dirn
   	local scan_loop,pixel_loop,pixel_behind,no_pixels
	local start_carried,done_trapezoid

		cmp	work.half.count,0
		je	done_trapezoid

		mov	ecx,work.half.i

	; Prepare for next iteration of loop
	;
scan_loop:
		push	esi
		push	edx

		add	esi,dword ptr work.colour.base
		mov	eax,work.pi.current

	; Render same colour pixels along scanline (forwards or backwards)
	;
	; eax:	colour	(16.16)
	; ecx:	address of end of scanline
	; esi:	frame buffer ptr
	;
		mov	ebx,eax
		shr	ebx,16
		CMP_&dirn esi,ecx
		jae	short no_pixels
pixel_loop:

	ifidni <dirn>,<b>
		dec	esi		; ptr++
	endif

		mov	CSEG [esi],bl	;	*ptr = colour
pixel_behind:

	ifidni <dirn>,<f>
		inc	esi		; ptr++
	endif
		CMP_&dirn esi,ecx
		jb	short pixel_loop

no_pixels:

	; Updates for next scanline:
	;
		pop	edx
		pop	esi
		mov	edi,work.half.d_f
		mov	eax,work.main.d_f

		add	work.half.f,edi	    	; x_end += work.main.grad_end (fraction)
		adc	ecx,work.half.d_i 	; endptr += integer(dx_end)+CARRY

		add	work.main.f,eax		; x+= work.main.grad (fraction)
		jc	start_carried

		add	esi,work.main.d_i		; fb_offset+=delta+CARRY
		mov	eax,work.pi.d_nocarry
		add	work.pi.current,eax

		dec	work.half.count
		jne	short scan_loop
		jmp	short done_trapezoid

start_carried:	adc	esi,work.main.d_i		; fb_offset+=delta+CARRY
		mov	eax,work.pi.d_carry
		add	work.pi.current,eax

		dec	work.half.count
		jne	scan_loop
done_trapezoid:
		endm
; Triangle_PI
;
; Render a triangle into frame buffer
;
;	Flat colour
;	Integer vertices
;	Perfect point sampling
;
; esi = vertex 0 pointer
; edi = vertex 1 pointer
; ebp = vertex 2 pointer
; 
RawTriangle_PI proc
	; Extract constant components
	;
		mov	eax,[esi].brp_vertex.comp[C_I*4]
		mov	work.pi.current,eax

		SETUP_PI
		jl	reversed
		mov	g_divisor,eax

	; Scan forwards

	; Load up registers for first time around inner loop
	;
		mov	esi,work.main.i
		mov	bl,byte ptr work.pi.current+2

		TRAPEZOID_PI top,f
		TRAPEZOID_PI bot,f
quit:		ret

	; Scan backwards
	;
reversed:
		neg	eax
		mov	g_divisor,eax

	; Load up registers for first time around inner loop
	;
		mov	esi,work.main.i
		mov	bl,byte ptr work.pi.current+2

		TRAPEZOID_PI top,b
		TRAPEZOID_PI bot,b
		ret


	SHOW_CODESIZE	<RawTriangle_PI>,<%($-_setup_start)>

RawTriangle_PI endp
	endif

	if PARTS and PART_8

; void TriangleRenderPII(struct br_vertex *pvertex_0,
;			struct br_vertex *pvertex_1,
;			struct br_vertex *pvertex_2);
;
; C stub directly to triangle renderer
;
TriangleRenderPII proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

	; Get pointers to vertex structures and colour
	;
	; eax = colour
	; esi = vertex 0
	; edi = vertex 1
	; ebp = vertex 2
	;
		push	ebp
		SETUP_SEGS

		mov	esi,pvertex_0
		mov	edi,pvertex_1
		mov	ebp,pvertex_2
		call	RawTriangle_PII

		RESTORE_SEGS
		pop	ebp
		ret
TriangleRenderPII endp

; Trapezoid loop for gouraud shaded z buffered faces
;
; Arguments control:
;	whether loop uses 'top' or 'bottom' variables
;	which direction scanline are rendered from the long edge
;
TRAPEZOID_PII	macro	half,dirn
   	local scan_loop,pixel_loop,pixel_behind,no_pixels
	local start_carried,done_trapezoid

	local preloop,mainloop,post,postloop,exit

		cmp	work.half.count,0
		je	done_trapezoid

		mov	ecx,work.half.i

	; Prepare for next iteration of loop
	;
scan_loop:

		pushad

		add	esi,dword ptr work.colour.base

		ifidni	<dirn>,<f>
		mov	line_state_dst1,ecx

		mov	edi,esi
		mov	ebp,ecx

		cmp	edi,ebp
		jge	exit

		and	ebp,not 3

		; Get work.pi.current into al:esi

		mov	eax,work.pi.current
		mov	esi,eax
		rol	esi,16
		ror	eax,16
		and	esi,0ffff0000h

		; Get work.pi.grad_x into dl:ecx

		mov	edx,work.pi.grad_x
		mov	ecx,edx
		rol	ecx,16
		ror	edx,16
		and	ecx,0ffff0000h

		cmp	edi,ebp
		jge	post

		; Do 1-3 bytes first

preloop:
		test	edi,3
		jz	mainloop

		mov	[edi],al
		inc	edi
		add	esi,ecx
		adc	al,dl
		jmp	preloop

mainloop:
		cmp	edi,ebp
		jge	post

		; Byte 0

		mov	bl,al
		add	esi,ecx
		adc	al,dl

		; Byte 1

		mov	bh,al
		ror	ebx,16
		add	esi,ecx
		adc	al,dl

		; Byte 2
		
		mov	bl,al
		add	esi,ecx
		adc	al,dl

		; Byte 3
		
		mov	bh,al
		ror	ebx,16
		add	esi,ecx
		adc	al,dl

		mov	[edi],ebx
		add	edi,4

		jmp	mainloop

post:
		mov	ebp,line_state_dst1

postloop:
		cmp	edi,ebp
		jge	exit

		mov	[edi],al
		inc	edi
		add	esi,ecx
		adc	al,dl
		jmp	postloop

exit:

		else

		mov	line_state_dst1,ecx

		mov	edi,esi
		mov	ebp,ecx

		cmp	edi,ebp
		jle	exit

		and	ebp,not 3
		add	ebp,4

		; Get work.pi.current into al:esi

		mov	eax,work.pi.current
		mov	esi,eax
		rol	esi,16
		ror	eax,16
		and	esi,0ffff0000h

		; Get work.pi.grad_x into dl:ecx

		mov	edx,work.pi.grad_x
		mov	ecx,edx
		rol	ecx,16
		ror	edx,16
		and	ecx,0ffff0000h

		cmp	edi,ebp
		jle	post

		; Do 1-3 bytes first

preloop:
		test	edi,3
		jz	mainloop

		dec	edi
		sub	esi,ecx
		sbb	al,dl
		mov	[edi],al
		jmp	preloop

mainloop:
		cmp	edi,ebp
		jle	post

		; Byte 3

		sub	esi,ecx
		sbb	al,dl
		mov	bh,al

		; Byte 2

		sub	esi,ecx
		sbb	al,dl
		mov	bl,al
		ror	ebx,16

		; Byte 1
		
		sub	esi,ecx
		sbb	al,dl
		mov	bh,al

		; Byte 0
		
		sub	esi,ecx
		sbb	al,dl
		mov	bl,al

		sub	edi,4
		mov	[edi],ebx

		jmp	mainloop

post:
		mov	ebp,line_state_dst1

postloop:
		cmp	edi,ebp
		jle	exit

		dec	edi
		sub	esi,ecx
		sbb	al,dl
		mov	[edi],al
		jmp	postloop

exit:
		endif

no_pixels:

	; Updates for next scanline:
	;
		popad

		mov	edi,work.half.d_f
		mov	eax,work.main.d_f

		add	work.half.f,edi	    	; x_end += work.main.grad_end (fraction)
		adc	ecx,work.half.d_i 	; endptr += integer(dx_end)+CARRY

		add	work.main.f,eax		; x+= work.main.grad (fraction)
		jc	start_carried

		add	esi,work.main.d_i		; fb_offset+=delta+CARRY
		mov	eax,work.pi.d_nocarry
		add	work.pi.current,eax

		dec	work.half.count
		jne	scan_loop
		jmp	done_trapezoid

start_carried:	adc	esi,work.main.d_i		; fb_offset+=delta+CARRY
		mov	eax,work.pi.d_carry
		add	work.pi.current,eax

		dec	work.half.count
		jne	scan_loop
done_trapezoid:
		endm


; RawTriangle_PII
;
; Render a triangle into frame buffer
;
;	Linear interpolated colour index
;	Integer vertices
;	Perfect point sampling
;
; esi = vertex 0 pointer
; edi = vertex 1 pointer
; ebp = vertex 2 pointer
; 
RawTriangle_PII proc
		SETUP_PI
		jl	reversed
		mov	g_divisor,eax

		PARAM_PI_DIRN comp[C_I*4],work.pi,f

	; Load up registers for first time around inner loop
	;
		mov	esi,work.main.i

		TRAPEZOID_PII top,f
		TRAPEZOID_PII bot,f
quit:		ret

	; Scan backwards
	;
reversed:
		neg	eax
		mov	g_divisor,eax

		PARAM_PI_DIRN comp[C_I*4],work.pi,b

	; Load up registers for first time around inner loop
	;
		mov	esi,work.main.i

		TRAPEZOID_PII top,b
		TRAPEZOID_PII bot,b
		ret

	SHOW_CODESIZE	<RawTriangle_PII>,<%($-_setup_start)>
RawTriangle_PII endp

	endif

	if PARTS and PART_8

TriangleRenderPIT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

	; Get pointers to vertex structures and colour
	;
	; eax = colour
	; esi = vertex 0
	; edi = vertex 1
	; ebp = vertex 2
	;
		push	ebp
		SETUP_SEGS

		mov	esi,pvertex_0
		mov	edi,pvertex_1
		mov	ebp,pvertex_2
		call	RawTriangle_PIT

		RESTORE_SEGS
		pop	ebp
		ret
TriangleRenderPIT endp

; Trapezoid loop for linear texture mapped faces
;
; Arguments control:
;	whether loop uses 'top' or 'bottom' variables
;	which direction scanline are rendered from the long edge
;
TRAPEZOID_PIT	macro	half,dirn
   	local scan_loop,pixel_loop,pixel_behind,no_pixels
	local start_carried,done_trapezoid

	local preloop,mainloop,post,postloop,exit

		cmp	work.half.count,0
		je	done_trapezoid

		mov	ecx,work.half.i

	; Prepare for next iteration of loop
	;
scan_loop:

		pushad
		add	esi,dword ptr work.colour.base
		ifidni	<dirn>,<f>

		mov	line_state_dst1,ecx

		mov	line_state_dst1,ecx

		mov	edi,esi
		mov	edx,ecx

		cmp	edi,edx
		jge	exit

		sub	edx,edi

		; Set up u, v and count

		xor	ebx,ebx
		mov	ecx,work.pu.current
		mov	bl,byte ptr work.pu.current+2
		rol	ecx,16
		mov	bh,byte ptr work.pv.current+2
		and	ecx,0ffff0000h
		or	edx,ecx

		; dx = count
		; edx upper half = {u}

		mov	eax,work.pv.current
		rol	eax,16

		; Set up du and dv

		xor	ecx,ecx
		mov	cl,byte ptr work.pu.grad_x+2
		mov	ch,byte ptr work.pv.grad_x+2
		mov	esi,work.pu.grad_x
		rol	esi,16
		and	esi,0ffff0000h
		mov	ebp,work.pv.grad_x
		rol	ebp,16
		and	ebp,0ffff0000h

mainloop:
		push	ebp
		mov	ebp,work.texture.base

		add	edx,esi			; Increment u fraction
		dec	edx
		mov	al,[ebx+ebp]		; Get texel
		adc	bl,cl			; Increment u
		pop	ebp

		test	al,al			; Check for transparency
		jz	pixel_behind

		mov	[edi],al
pixel_behind:
		inc	edi

		add	eax,ebp
		adc	bh,ch

		test	edx,0ffffh
		jnz	mainloop

exit:
		else

		mov	line_state_dst1,ecx

		mov	edi,esi
		mov	edx,ecx

		cmp	edi,edx
		jle	exit

		sub	edx,edi
		neg	edx

		; Set up u, v and count

		xor	ebx,ebx
		mov	ecx,work.pu.current
		mov	bl,byte ptr work.pu.current+2
		rol	ecx,16
		mov	bh,byte ptr work.pv.current+2
		and	ecx,0ffff0000h
		or	edx,ecx

		; dx = count
		; edx upper half = {u}

		mov	eax,work.pv.current
		rol	eax,16

		; Set up du and dv

		xor	ecx,ecx
		mov	cl,byte ptr work.pu.grad_x+2
		mov	ch,byte ptr work.pv.grad_x+2
		mov	esi,work.pu.grad_x
		rol	esi,16
		and	esi,0ffff0000h
		mov	ebp,work.pv.grad_x
		rol	ebp,16
		and	ebp,0ffff0000h

mainloop:

		sub	edx,esi				; Decrement u fraction
		sbb	bl,cl				; Decrement u

		sub	eax,ebp				; Decrement v fraction
		sbb	bh,ch				; Decrement v

		push	ecx
		mov	ecx,work.texture.base
		dec	edi
		dec	edx
		mov	al,[ebx+ecx]			; Get texel
		pop	ecx

		test	al,al				; Check for transparency
		jz	pixel_behind

		mov	[edi],al
pixel_behind:
		test	edx,0ffffh
		jnz	mainloop

exit:
		endif

		popad
no_pixels:
	; Updates for next scanline:
	;
		mov	edi,work.half.d_f
		mov	eax,work.main.d_f
		add	work.half.f,edi	    	; x_end += work.main.grad_end (fraction)
		adc	ecx,work.half.d_i 	; endptr += integer(dx_end)+CARRY

		add	work.main.f,eax		; x+= work.main.grad (fraction)
		jc	start_carried

		add	esi,work.main.d_i		; fb_offset+=delta+CARRY

	;; Per scanline parameter update - no carry
	;;
		mov	eax,work.pu.current
		mov	ebx,work.pv.current
		add	eax,work.pu.d_nocarry
		add	ebx,work.pv.d_nocarry
		mov	work.pu.current,eax
		mov	work.pv.current,ebx

		dec	work.half.count
		jne	scan_loop
		jmp	done_trapezoid

start_carried:	adc	esi,work.main.d_i		; fb_offset+=delta+CARRY

	;; Per scanline parameter update - carry
	;;
		mov	eax,work.pu.current
		mov	ebx,work.pv.current
		add	eax,work.pu.d_carry
		add	ebx,work.pv.d_carry
		mov	work.pu.current,eax
		mov	work.pv.current,ebx

		dec	work.half.count
		jne	scan_loop
done_trapezoid:
		endm


; RawTriangle_PIT
;
; Render a triangle into frame buffer
;
;	Linear interpolated colour texture
;	Linear interpolated shade index
;	Integer vertices
;	Perfect point sampling
;
; esi = vertex 0 pointer
; edi = vertex 1 pointer
; ebp = vertex 2 pointer
; 
RawTriangle_PIT proc

		SETUP_PI
		jl	reversed
		mov	g_divisor,eax

		PARAM_PI_DIRN comp[C_U*4],work.pu,f
		PARAM_PI_DIRN comp[C_V*4],work.pv,f

	; Load up registers for first time around inner loop
	;
		mov	esi,work.main.i

		TRAPEZOID_PIT top,f
		TRAPEZOID_PIT bot,f
quit:		ret

	; Scan backwards
	;
reversed:
		neg	eax
		mov	g_divisor,eax

		PARAM_PI_DIRN comp[C_U*4],work.pu,b
		PARAM_PI_DIRN comp[C_V*4],work.pv,b

	; Load up registers for first time around inner loop
	;
		mov	esi,work.main.i

		TRAPEZOID_PIT top,b
		TRAPEZOID_PIT bot,b
		ret

	SHOW_CODESIZE	<RawTriangle_PIT>,<%($-_setup_start)>
RawTriangle_PIT endp

	endif

	if PARTS and PART_8

TriangleRenderPITI proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

	; Get pointers to vertex structures and colour
	;
	; eax = colour
	; esi = vertex 0
	; edi = vertex 1
	; ebp = vertex 2
	;
		push	ebp
		SETUP_SEGS

		mov	esi,pvertex_0
		mov	edi,pvertex_1
		mov	ebp,pvertex_2
		call	RawTriangle_PITI

		RESTORE_SEGS
		pop	ebp
		ret
TriangleRenderPITI endp

; Trapezoid loop for linear texture mapped z buffered faces
;
; Arguments control:
;	whether loop uses 'top' or 'bottom' variables
;	which direction scanline are rendered from the long edge
;
TRAPEZOID_PITI	macro	half,dirn
   	local scan_loop,pixel_loop,pixel_behind,no_pixels
	local start_carried,done_trapezoid

		cmp	work.half.count,0
		je	done_trapezoid

		mov	ecx,work.half.i

	; Prepare for next iteration of loop
	;
scan_loop:	push	esi

		add	esi,dword ptr work.colour.base

		mov	eax,work.pu.current
		mov	ebx,work.pv.current
		mov	ebp,work.pi.current
		mov	temp_u,eax
		mov	temp_v,ebx
		mov	temp_i,ebp

	; Render same colour pixels along scanline (forwards or backwards)
	;
	; ecx:	address of end of scanline
	; esi:	frame buffer ptr
	;
		CMP_&dirn esi,ecx
		jae	short no_pixels

		mov	eax,work.pu.grad_x
		mov	edx,work.pv.grad_x
pixel_loop:
	ifidni <dirn>,<b>
		mov	ebp,temp_u
		mov	ebx,temp_v
		sub	ebp,eax
		sub	ebx,edx
		mov	temp_u,ebp
		mov	ebp,temp_i
		mov	temp_v,ebx

		sub	ebp,work.pi.grad_x
		dec	esi		; ptr++
		mov	temp_i,ebp

	;; Per pixel fetch colour
	;;
		xor	ebx,ebx
		mov	bl,byte ptr temp_u+2
		mov	bh,byte ptr temp_v+2
		mov	ebp,work.texture.base
		mov	bl,TSEG [ebp+ebx]
		test	bl,bl		; Check for transparency
		jz	pixel_behind

		mov	bh,byte ptr temp_i+2
		add	ebx,work.shade_table
		mov	bl,[ebx]
		mov	CSEG [esi],bl	;	*ptr = colour

pixel_behind:

	endif

	ifidni <dirn>,<f>

	;; Per pixel fetch colour
	;;
		mov	ebp,work.texture.base
		xor	ebx,ebx
		mov	bl,byte ptr temp_u+2
		mov	bh,byte ptr temp_v+2
		mov	bl,TSEG [ebp+ebx]
		mov	ebp,temp_u	; Load up ebp early
		test	bl,bl		; Check for transparency
		jz	pixel_behind

		mov	bh,byte ptr temp_i+2
		add	ebx,work.shade_table
		mov	bl,[ebx]
		mov	CSEG [esi],bl	;	*ptr = colour

pixel_behind:

	;; Per pixel update - forwards
	;;
		mov	ebx,temp_v
		add	ebp,eax
		add	ebx,edx
		mov	temp_u,ebp
		mov	ebp,temp_i
		mov	temp_v,ebx

		add	ebp,work.pi.grad_x
		inc	esi		; ptr++
		mov	temp_i,ebp

	endif

		CMP_&dirn esi,ecx
		jb	pixel_loop
no_pixels:
	; Updates for next scanline:
	;
		pop	esi
		mov	edi,work.half.d_f
		mov	eax,work.main.d_f
		add	work.half.f,edi	    	; x_end += work.main.grad_end (fraction)
		adc	ecx,work.half.d_i 	; endptr += integer(dx_end)+CARRY

		add	work.main.f,eax		; x+= work.main.grad (fraction)
		jc	start_carried

		add	esi,work.main.d_i		; fb_offset+=delta+CARRY

	;; Per scanline parameter update - no carry
	;;
		mov	ebp,work.pi.current
		mov	eax,work.pu.current
		mov	ebx,work.pv.current
		add	ebp,work.pi.d_nocarry
		add	eax,work.pu.d_nocarry
		add	ebx,work.pv.d_nocarry
		mov	work.pi.current,ebp
		mov	work.pu.current,eax
		mov	work.pv.current,ebx

		dec	work.half.count
		jne	scan_loop
		jmp	done_trapezoid

start_carried:	adc	esi,work.main.d_i		; fb_offset+=delta+CARRY

	;; Per scanline parameter update - carry
	;;
		mov	ebp,work.pi.current
		mov	eax,work.pu.current
		mov	ebx,work.pv.current
		add	ebp,work.pi.d_carry
		add	eax,work.pu.d_carry
		add	ebx,work.pv.d_carry
		mov	work.pi.current,ebp
		mov	work.pu.current,eax
		mov	work.pv.current,ebx

		dec	work.half.count
		jne	scan_loop
done_trapezoid:
		endm


; RawTriangle_PITI
;
; Render a triangle into frame buffer
;
;	Linear interpolated colour texture
;	Linear interpolated shade index
;	Integer vertices
;	Perfect point sampling
;
; esi = vertex 0 pointer
; edi = vertex 1 pointer
; ebp = vertex 2 pointer
; 
RawTriangle_PITI proc

		SETUP_PI
		jl	reversed
		mov	g_divisor,eax

		PARAM_PI_DIRN comp[C_I*4],work.pi,f
		PARAM_PI_DIRN comp[C_U*4],work.pu,f
		PARAM_PI_DIRN comp[C_V*4],work.pv,f

	; Load up registers for first time around inner loop
	;
		mov	esi,work.main.i

		TRAPEZOID_PITI top,f
		TRAPEZOID_PITI bot,f
quit:		ret

	; Scan backwards
	;
reversed:
		neg	eax
		mov	g_divisor,eax

		PARAM_PI_DIRN comp[C_I*4],work.pi,b
		PARAM_PI_DIRN comp[C_U*4],work.pu,b
		PARAM_PI_DIRN comp[C_V*4],work.pv,b

	; Load up registers for first time around inner loop
	;
		mov	esi,work.main.i

		TRAPEZOID_PITI top,b
		TRAPEZOID_PITI bot,b
		ret

	SHOW_CODESIZE	<RawTriangle_PITI>,<%($-_setup_start)>
RawTriangle_PITI endp

	endif

	if PARTS and PART_8

ditherspec	macro	i,a,b,c,d
dither&i&0	equ	a*4096
dither&i&1	equ	b*4096
dither&i&2	equ	c*4096
dither&i&3	equ	d*4096
		endm
@dith	macro	x,y
	exitm	%dither&x&&y&
	endm

	; Specify dither pattern

		ditherspec	0,13,1,4,0
		ditherspec	1,8,12,9,5
		ditherspec	2,10,6,7,11
		ditherspec	3,3,15,14,2

TriangleRenderPITD proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

	; Get pointers to vertex structures and colour
	;
	; eax = colour
	; esi = vertex 0
	; edi = vertex 1
	; ebp = vertex 2
	;
		push	ebp
		SETUP_SEGS

		mov	esi,pvertex_0
		mov	edi,pvertex_1
		mov	ebp,pvertex_2
		call	RawTriangle_PITD

		RESTORE_SEGS
		pop	ebp
		ret
TriangleRenderPITD endp

; Trapezoid loop for linear dithered texture mapped faces
;
; Arguments control:
;	whether loop uses 'top' or 'bottom' variables
;	which direction scanline are rendered from the long edge
;
TRAPEZOID_PITD	macro	half,dirn
		local scan_loop,pixel_loop,no_pixels
		local done_trapezoid

		cmp	work.half.count,0
		je	done_trapezoid

		mov	ecx,work.half.i

		ifidni	<half>,<top>
		mov	eax,vertex_0[_Y]
		else
		mov	eax,vertex_1[_Y]
		endif
		and	eax,3

		; Indexed jump into correct row modulo 4

		jmp	rowtable_PITD&half&&dirn&[eax*4]

		.data
rowtable_PITD&half&&dirn&	label	ptr dword
		dd	scan_loop&half&&dirn&0
		dd	scan_loop&half&&dirn&1
		dd	scan_loop&half&&dirn&2
		dd	scan_loop&half&&dirn&3

		.code

scan_loop:
		for	row,<0,1,2,3>
		local	pixel_loop,start_carried,no_pixels
		local	next_row

scan_loop&half&&dirn&&row&:

	; Prepare for next iteration of loop
	;
		push	esi
		push	edx

		add	esi,dword ptr work.colour.base

	; Render same colour pixels along scanline (forwards or backwards)
	;
	; ecx:	address of end of scanline
	; esi:	frame buffer ptr
	;
		CMP_&dirn esi,ecx
		jae	no_pixels

		; Indexed jump into correct column modulo 4

		mov	eax,esi
		and	eax,3
		jmp	columntable_PITD&row&&half&&dirn&[eax*4]
		.data
columntable_PITD&row&&half&&dirn&	label	ptr dword
		ifidni	<dirn>,<f>
		dd	pixel_start&half&&dirn&&row&0
		dd	pixel_start&half&&dirn&&row&1
		dd	pixel_start&half&&dirn&&row&2
		dd	pixel_start&half&&dirn&&row&3
		else
		dd	pixel_start&half&&dirn&&row&0
		dd	pixel_start&half&&dirn&&row&3
		dd	pixel_start&half&&dirn&&row&2
		dd	pixel_start&half&&dirn&&row&1
		endif
		.code

		for	col,<0,1,2,3>
pixel_start&half&&dirn&&row&&col&:
		mov	eax,work.pu.current
		mov	ebx,work.pv.current
		ifidni	<dirn>,<f>
		add	eax,@dith(row,col)
		add	ebx,@dith(row,col)
		else
		add	eax,@dith(row,%(4-col and 3))
		add	ebx,@dith(row,%(4-col and 3))
		endif
		mov	temp_u,eax
		mov	temp_v,ebx

		jmp	pixel_loop&half&&dirn&&row&&col&
		endm

pixel_loop:
		for	col,<0,1,2,3>
		local	pixel_behind

pixel_loop&half&&dirn&&row&&col&:

	ifidni <dirn>,<b>
	;; Per pixel update - backwards
	;;
		mov	eax,temp_u
		mov	ebx,temp_v
		sub	eax,work.pu.grad_x
		sub	ebx,work.pv.grad_x
		add	eax,@dith(row,%(3-col and 3))-@dith(row,%(4-col and 3))
		add	ebx,@dith(row,%(3-col and 3))-@dith(row,%(4-col and 3))
		mov	temp_u,eax
		mov	temp_v,ebx

		dec	esi		; ptr++
	endif

	;; Per pixel fetch colour
	;;
		xor	ebx,ebx
		mov	bl,byte ptr temp_u+2
		mov	bh,byte ptr temp_v+2
		add	ebx,work.texture.base
		mov	bl,TSEG [ebx]

		and	bl,bl		; Check for transparency
		je	pixel_behind

		mov	CSEG [esi],bl	;	*ptr = colour

pixel_behind:

	ifidni <dirn>,<f>
	;; Dithered increments for u and v
	;;
		mov	eax,temp_u
		mov	ebx,temp_v
		add	eax,work.pu.grad_x
		add	ebx,work.pv.grad_x
		add	eax,@dith(row,%(col+1 and 3))-@dith(row,col)
		add	ebx,@dith(row,%(col+1 and 3))-@dith(row,col)
		mov	temp_u,eax
		mov	temp_v,ebx

		inc	esi		; ptr++
	endif

		CMP_&dirn esi,ecx
		jae	no_pixels

		endm	; col,<0,1,2,3>
		jmp	pixel_loop

no_pixels:

	; Updates for next scanline:
	;
		pop	edx
		pop	esi
		mov	edi,work.half.d_f
		mov	eax,work.main.d_f

		add	work.half.f,edi	    	; x_end += work.main.grad_end (fraction)
		adc	ecx,work.half.d_i 	; endptr += integer(dx_end)+CARRY

		add	work.main.f,eax  		; x+= work.main.grad (fraction)
		jc	start_carried

		add	esi,work.main.d_i		; fb_offset+=delta+CARRY

	;; Per scanline parameter update - no carry
	;;
		mov	eax,work.pu.current
		mov	ebx,work.pv.current
		add	eax,work.pu.d_nocarry
		add	ebx,work.pv.d_nocarry
		mov	work.pu.current,eax
		mov	work.pv.current,ebx

		dec	work.half.count
		je	done_trapezoid
		jmp	next_row

start_carried:	adc	esi,work.main.d_i		; fb_offset+=delta+CARRY

	;; Per scanline parameter update - carry
	;;
		mov	eax,work.pu.current
		mov	ebx,work.pv.current
		add	eax,work.pu.d_carry
		add	ebx,work.pv.d_carry
		mov	work.pu.current,eax
		mov	work.pv.current,ebx

		dec	work.half.count
		je	done_trapezoid
next_row:
		endm	; row,<1,2,3,4>

		jmp	scan_loop

done_trapezoid:
		endm


; RawTriangle_PITD
;
; Render a triangle into frame buffer
;
;	Linear interpolated colour texture
;	Integer vertices
;	Perfect point sampling
;
; esi = vertex 0 pointer
; edi = vertex 1 pointer
; ebp = vertex 2 pointer
; 
RawTriangle_PITD proc

		SETUP_PI
		jl	reversed
		mov	g_divisor,eax

		PARAM_PI_DIRN comp[C_U*4],work.pu,f
		PARAM_PI_DIRN comp[C_V*4],work.pv,f

	; Load up registers for first time around inner loop
	;
		mov	esi,work.main.i

		TRAPEZOID_PITD top,f
		TRAPEZOID_PITD bot,f
quit:		ret

	; Scan backwards
	;
reversed:
		neg	eax
		mov	g_divisor,eax

		PARAM_PI_DIRN comp[C_U*4],work.pu,b
		PARAM_PI_DIRN comp[C_V*4],work.pv,b

		mov	eax,vertex_0[_Y]
		mov	temp_y,eax

	; Load up registers for first time around inner loop
	;
		mov	esi,work.main.i

		TRAPEZOID_PITD top,b
		TRAPEZOID_PITD bot,b
		ret

	SHOW_CODESIZE	<RawTriangle_PITD>,<%($-_setup_start)>
RawTriangle_PITD endp


	endif

	if PARTS and PART_8

	; Lighting dither may be out of phase with (u,v) dithering
	; ui_phase_x and ui_phase_y specify the phase difference
	; in the x and y directions

ui_phase_x	equ	2
ui_phase_y	equ	2

TriangleRenderPITID proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

	; Get pointers to vertex structures and colour
	;
	; eax = colour
	; esi = vertex 0
	; edi = vertex 1
	; ebp = vertex 2
	;
		push	ebp
		SETUP_SEGS

		mov	esi,pvertex_0
		mov	edi,pvertex_1
		mov	ebp,pvertex_2
		call	RawTriangle_PITID

		RESTORE_SEGS
		pop	ebp
		ret
TriangleRenderPITID endp

; Trapezoid loop for lit linear dithered-u,v texture mapped 
;
; Arguments control:
;	whether loop uses 'top' or 'bottom' variables
;	which direction scanline are rendered from the long edge
;
TRAPEZOID_PITID	macro	half,dirn
		local scan_loop,pixel_loop,no_pixels
		local done_trapezoid

		cmp	work.half.count,0
		je	done_trapezoid

		mov	ecx,work.half.i

		ifidni	<half>,<top>
		mov	eax,vertex_0[_Y]
		else
		mov	eax,vertex_1[_Y]
		endif
		and	eax,3

		; Indexed jump into correct row modulo 4

		jmp	rowtable_PITID&half&&dirn&[eax*4]

		.data
rowtable_PITID&half&&dirn&	label	ptr dword
		dd	scan_loop&half&&dirn&0
		dd	scan_loop&half&&dirn&1
		dd	scan_loop&half&&dirn&2
		dd	scan_loop&half&&dirn&3

		.code

scan_loop:
		for	row,<0,1,2,3>
		local	pixel_loop,start_carried,no_pixels
		local	next_row

scan_loop&half&&dirn&&row&:

	; Prepare for next iteration of loop
	;
		push	esi
		push	edx

		add	esi,dword ptr work.colour.base

	; Render same colour pixels along scanline (forwards or backwards)
	;
	; ecx:	address of end of scanline
	; esi:	frame buffer ptr
	;
		CMP_&dirn esi,ecx
		jae	no_pixels

		; Indexed jump into correct column modulo 4

		mov	eax,esi
		and	eax,3
		jmp	columntable_PITID&row&&half&&dirn&[eax*4]

		.data
columntable_PITID&row&&half&&dirn&	label	ptr dword
		ifidni	<dirn>,<f>
		dd	pixel_start&half&&dirn&&row&0
		dd	pixel_start&half&&dirn&&row&1
		dd	pixel_start&half&&dirn&&row&2
		dd	pixel_start&half&&dirn&&row&3
		else
		dd	pixel_start&half&&dirn&&row&0
		dd	pixel_start&half&&dirn&&row&3
		dd	pixel_start&half&&dirn&&row&2
		dd	pixel_start&half&&dirn&&row&1
		endif
		.code

		for	col,<0,1,2,3>
pixel_start&half&&dirn&&row&&col&:

		;; Dithered increments for u, v and index

		mov	eax,work.pu.current
		mov	ebx,work.pv.current
		mov	ebp,work.pi.current

		ifidni	<dirn>,<f>
		add	eax,@dith(row,col)
		add	ebx,@dith(row,col)
		add	ebp,@dith(%(ui_phase_y+row and 3),%(ui_phase_x+col and 3))
		else
		add	eax,@dith(row,%(4-col and 3))
		add	ebx,@dith(row,%(4-col and 3))
		add	ebp,@dith(%(ui_phase_y+row and 3),%(ui_phase_x+4-col and 3))
		endif
		mov	temp_u,eax
		mov	temp_v,ebx
		mov	temp_i,ebp

		jmp	pixel_loop&half&&dirn&&row&&col&
		endm

pixel_loop:
		for	col,<0,1,2,3>
		local	pixel_behind

pixel_loop&half&&dirn&&row&&col&:

	ifidni <dirn>,<b>
	;; Dithered increments for u, v and index
	;;
		mov	eax,temp_u
		mov	ebx,temp_v
		sub	eax,work.pu.grad_x
		sub	ebx,work.pv.grad_x
		add	eax,@dith(row,%(3-col and 3))-@dith(row,%(4-col and 3))
		add	ebx,@dith(row,%(3-col and 3))-@dith(row,%(4-col and 3))
		mov	temp_u,eax
		mov	temp_v,ebx

		mov	ebp,temp_i
		sub	ebp,work.pi.grad_x
		add	ebp,@dith(%(ui_phase_y+row and 3),%(ui_phase_x+3-col and 3))-@dith(%(ui_phase_y+row and 3),%(ui_phase_x+4-col and 3))
		mov	temp_i,ebp

		dec	esi		; ptr++
	endif

	;; Per pixel fetch colour
	;;
		xor	ebx,ebx
		mov	bl,byte ptr temp_u+2
		mov	bh,byte ptr temp_v+2
		mov	ebp,work.texture.base
		mov	bl,TSEG [ebp+ebx]

		and	bl,bl		; Check for transparency
		je	pixel_behind

		mov	bh,byte ptr temp_i+2
		add	ebx,work.shade_table
		mov	bl,[ebx]
		mov	CSEG [esi],bl	;	*ptr = colour

pixel_behind:

	ifidni <dirn>,<f>
	;; Per pixel update - forwards
	;;
		mov	eax,temp_u
		mov	ebx,temp_v
		add	eax,work.pu.grad_x
		add	ebx,work.pv.grad_x
		add	eax,@dith(row,%(col+1 and 3))-@dith(row,col)
		add	ebx,@dith(row,%(col+1 and 3))-@dith(row,col)
		mov	temp_u,eax
		mov	temp_v,ebx

		mov	ebp,temp_i
		add	ebp,work.pi.grad_x
		add	ebp,@dith(%(ui_phase_y+row and 3),%(ui_phase_x+col+1 and 3))-@dith(%(ui_phase_y+row and 3),%(ui_phase_x+col and 3))
		mov	temp_i,ebp

		inc	esi		; ptr++
	endif

		CMP_&dirn esi,ecx
		jae	no_pixels

		endm	; col,<0,1,2,3>
		jmp	pixel_loop

no_pixels:

	; Updates for next scanline:
	;
		pop	edx
		pop	esi
		mov	edi,work.half.d_f
		mov	eax,work.main.d_f

		add	work.half.f,edi	    	; x_end += work.main.grad_end (fraction)
		adc	ecx,work.half.d_i 	; endptr += integer(dx_end)+CARRY

		add	work.main.f,eax  		; x+= work.main.grad (fraction)
		jc	start_carried

		add	esi,work.main.d_i		; fb_offset+=delta+CARRY

	;; Per scanline parameter update - no carry
	;;
		mov	eax,work.pu.current
		mov	ebx,work.pv.current
		add	eax,work.pu.d_nocarry
		add	ebx,work.pv.d_nocarry
		mov	work.pu.current,eax
		mov	work.pv.current,ebx
		mov	ebp,work.pi.current
		add	ebp,work.pi.d_nocarry
		mov	work.pi.current,ebp

		dec	work.half.count
		je	done_trapezoid
		jmp	next_row

start_carried:	adc	esi,work.main.d_i		; fb_offset+=delta+CARRY

	;; Per scanline parameter update - carry
	;;
		mov	eax,work.pu.current
		mov	ebx,work.pv.current
		add	eax,work.pu.d_carry
		add	ebx,work.pv.d_carry
		mov	work.pu.current,eax
		mov	work.pv.current,ebx
		mov	ebp,work.pi.current
		add	ebp,work.pi.d_carry
		mov	work.pi.current,ebp

		dec	work.half.count
		je	done_trapezoid
next_row:
		endm	; row,<1,2,3,4>

		jmp	scan_loop

done_trapezoid:
		endm


; RawTriangle_PITID
;
; Render a triangle into frame buffer
;
;	Linear interpolated colour texture
;	Integer vertices
;	Perfect point sampling
;
; esi = vertex 0 pointer
; edi = vertex 1 pointer
; ebp = vertex 2 pointer
; 
RawTriangle_PITID proc

		SETUP_PI
		jl	reversed
		mov	g_divisor,eax

		PARAM_PI_DIRN comp[C_U*4],work.pu,f
		PARAM_PI_DIRN comp[C_V*4],work.pv,f
		PARAM_PI_DIRN comp[C_I*4],work.pi,f

	; Load up registers for first time around inner loop
	;
		mov	esi,work.main.i

		TRAPEZOID_PITID top,f
		TRAPEZOID_PITID bot,f
quit:		ret

	; Scan backwards
	;
reversed:
		neg	eax
		mov	g_divisor,eax

		PARAM_PI_DIRN comp[C_U*4],work.pu,b
		PARAM_PI_DIRN comp[C_V*4],work.pv,b
		PARAM_PI_DIRN comp[C_I*4],work.pi,b

		mov	eax,vertex_0[_Y]
		mov	temp_y,eax

	; Load up registers for first time around inner loop
	;
		mov	esi,work.main.i

		TRAPEZOID_PITID top,b
		TRAPEZOID_PITID bot,b
		ret

	SHOW_CODESIZE	<RawTriangle_PITID>,<%($-_setup_start)>
RawTriangle_PITID endp

	endif

	end
