;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: tt16_piz.asm 1.1 1997/12/10 16:49:12 jon Exp $
;; $Locker: $
;;
;; 16 bit RGB_565 5 bit mode
;;
;; Triangle scan convertion and rendering
;;
;; Perfect scan, integer vertices, Z buffered
;;
	.386p
	.model flat,c

	include	drv.inc
	include 586_macs.inc

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

;; Setup some macros that are used to put colour, texture and depth buffers in different
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
overflow:
		xor	eax,eax
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

; Setup of an integer, perfect scan triangle  
;
; Leave g_divisor in eax, with sign of g_divisor in flags
;
SETUP_PI	macro

_setup_start = $

	; Get pixel width of colour and z buffer from fw.output->width
	
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

if 1
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

	; Work out top.count and bot.count - the heights of each part
	; of the triangle
	;

		cmp	eax,ecx	; Check for zero height polys
		je	quit

	; Work out deltas and gradients along edges
	;
		; top short edge
		;
		mov	edx,vertex_1[_X]		; work.top.x
		sub	edx,vertex_0[_X]
		mov	work.top.x,edx

		mov	ebx,vertex_1[_Y]		; work.top.y
		sub	ebx,vertex_0[_Y]
		mov	work.top.y,ebx

		mov	work.top.count,ebx
		je	short no_top
		
		FIX_DIV				; work.top.grad = work.top.x/work.top.y
		idiv	ebx
		mov	work.top.grad,eax
		mov	word ptr work.top.d_f+2,ax
		sar	eax,16
		add	eax,work.colour.stride_p
		mov	work.top.d_i,eax
no_top:
		; bottom short edge (don't need to save deltas)
		;
		mov	edx,vertex_2[_X]		; work.bot.x
		sub	edx,vertex_1[_X]

		mov	ebx,vertex_2[_Y]		; work.bot.y
		sub	ebx,vertex_1[_Y]

		mov	work.bot.count,ebx
		je	short no_bottom

		FIX_DIV				; work.bot.grad = work.bot.x/work.bot.y
		idiv	ebx
		mov	work.bot.grad,eax
		mov	word ptr work.bot.d_f+2,ax
		sar	eax,16
		add	eax,work.colour.stride_p
		mov	work.bot.d_i,eax

no_bottom:
		; long edge
		;
		mov	edx,vertex_2[_X]	; work.main.x
		sub	edx,vertex_0[_X]
		mov	work.main.x,edx

		mov	ebx,vertex_2[_Y]	; work.main.y
		sub	ebx,vertex_0[_Y]
		mov	work.main.y,ebx

		FIX_DIV				; work.main.grad = work.main.x/work.main.y
		idiv	ebx
		mov	work.main.grad,eax
		mov	word ptr work.main.d_f+2,ax
		sar	eax,16
		add	eax,work.colour.stride_p
		mov	work.main.d_i,eax

	; Work out start values for edges as pixel offsets
	;
		mov	ebx,work.colour.stride_p

		mov	eax,vertex_0[_Y]
		imul	ebx
		add	eax,vertex_0[_X]

		mov	work.main.f,080000000h
		mov	work.main.i,eax

		mov	work.top.f,080000000h
		mov	work.top.i,eax

		mov	eax,vertex_1[_Y]
		imul	ebx
		add	eax,vertex_1[_X]

		mov	work.bot.f,080000000h
		mov	work.bot.i,eax

if 0
		mov	eax,work.main.grad
		sar	eax,1
		mov	edx,eax
		shl	eax,16
		sar	edx,16
		add	work.main.f,eax
		adc	work.main.i,edx

		mov	eax,work.top.grad
		sar	eax,1
		mov	edx,eax
		sar	edx,16
		shl	eax,16
		add	work.top.f,eax
		adc	work.top.i,edx

		mov	eax,work.bot.grad
		sar	eax,1
		mov	edx,eax
		sar	edx,16
		shl	eax,16
		add	work.bot.f,eax
		adc	work.bot.i,edx
endif
		
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
PARAM_PI_DIRN macro vparam,param,dirn,unsigned:=<0>

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

;		mov	ebx,param.grad_y
;		sar	ebx,1
;		adc	eax,ebx

if unsigned
		xor	eax,080000000h
endif

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
		db '$Id: tt16_piz.asm 1.1 1997/12/10 16:49:12 jon Exp $',0
		align 4
		
;colour_row_width	dd 0

temp_b	dd 0
temp_g	dd 0
temp_r	dd 0
temp_colour	dd 0

p0_offset_x	dd	0	; Offset of start pixel centre from vertex 0
p0_offset_y	dd	0

temp_i		dd	0
temp_u		dd	0
temp_v		dd	0

	; Integer vertex poitisions
	;
vertex_0	dd	2 dup(?)
vertex_1	dd	2 dup(?)
vertex_2	dd	2 dup(?)

g_divisor	dd	0	; Bottom of param. gradient calc.

	.code

	if PARTS and PART_16Z

; void TriangleRenderPIZ2_RGB_565(struct br_vertex *pvertex_0,
;			struct br_vertex *pvertex_1,
;			struct br_vertex *pvertex_2);
;
; C stub directly to triangle renderer
;
TriangleRenderPIZ2_RGB_565 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

	; Get pointers to vertex structures and colour
	;
	; esi = vertex 0
	; edi = vertex 1
	; ebp = vertex 2
	;
		push	ebp
		SETUP_SEGS

		mov	esi,pvertex_0
		mov	edi,pvertex_1
		mov	ebp,pvertex_2
		call	RawTriangle_PIZ2_RGB_565

		RESTORE_SEGS
		pop	ebp
		ret
TriangleRenderPIZ2_RGB_565 endp

; Trapezoid loop for flat shaded z (16 bit) buffered faces
;
; Arguments control:
;	whether loop uses 'top' or 'bottom' variables
;	which direction scanline are rendered from the long edge
;
TRAPEZOID_PIZ2_RGB_565	macro	half,dirn
   	local scan_loop,pixel_loop,pixel_behind,no_pixels
	local start_carried,done_trapezoid

		cmp	work.half.count,0
		je	done_trapezoid

		mov	ecx,work.half.i

	; Prepare for next iteration of loop
	;
scan_loop:	push	esi
		push	edx
		push ecx
		
		mov edi,work.depth.base
		mov ebx,work.colour.base
		lea edi,[edi+esi*2]	;	z buffer
		
		lea esi,[esi*2]	; 	colour buffer
		lea ecx,[ecx*2]

	; Render same colour pixels along scanline (forwards or backwards)
	;
	; bl:	colour
	; ecx:	address of end of scanline
	; edx:	z 	(16.16)
	; ebp:	z delta (16.16)
	; esi:	frame buffer ptr
	; edi:	z buffer ptr
	;
		CMP_&dirn esi,ecx
		jae	short no_pixels

pixel_loop:
	ifidni <dirn>,<b>
		sub	edx,ebp		; z -= d_z_z
		sub	edi,2		; z_ptr--
		sub	esi,2		; ptr--
	endif
		mov	eax,edx
		shr	eax,16
		cmp	ax, DSEG [edi]	; if (z > *zptr)
		jae	short pixel_behind
		mov	DSEG [edi],ax	;	*zptr = z

		mov	eax,temp_colour		
		mov CSEG [esi+ebx],ax		;		xrgb 1 5 5 5
pixel_behind:

	ifidni <dirn>,<f>
		add	edx,ebp		; z += d_z_z
		add	edi,2		; z_ptr++
		add	esi,2		; ptr++
	endif
		CMP_&dirn esi,ecx
		jb	short pixel_loop
no_pixels:

	; Updates for next scanline:
	;
		pop	ecx
		pop	edx
		pop	esi
		mov	edi,work.half.d_f
		mov	eax,work.main.d_f
		add	work.half.f,edi	    	; x_end += work.main.grad_end (fraction)
		adc	ecx,work.half.d_i 	; endptr += integer(dx_end)+CARRY

		add	work.main.f,eax		; x+= work.main.grad (fraction)
		jc	start_carried

		add	esi,work.main.d_i		; fb_offset+=delta+CARRY
		add	edx,work.pz.d_nocarry

		dec	work.half.count
		jne	scan_loop
		jmp	short done_trapezoid

start_carried:	adc	esi,work.main.d_i		; fb_offset+=delta+CARRY
		add	edx,work.pz.d_carry

		dec	work.half.count
		jne	scan_loop
done_trapezoid:
	endm

; Triangle_PIZ2_RGB_565
;
; Render a triangle into frame buffer
;
;	Flat colour
;	Z buffer
;	Integer vertices
;	Perfect point sampling
;
; esi = vertex 0 pointer
; edi = vertex 1 pointer
; ebp = vertex 2 pointer
; 
RawTriangle_PIZ2_RGB_565 proc

if 1
	; Construct colour word
	;
		mov	eax,[esi].brp_vertex.comp[C_R*4]
		mov	ebx,[esi].brp_vertex.comp[C_G*4]
		mov	ecx,[esi].brp_vertex.comp[C_B*4]

		push	ebp
		mov	ebp,0f80000h

		and	eax,ebp
		and	ebx,0fc0000h
		and	ecx,ebp

		shr	eax,8
		shr	ebx,13
		shr	ecx,19

		or	eax,ebx
		or	eax,ecx
		mov 	temp_colour,eax
		pop	ebp
endif
		SETUP_PI
		jl	reversed
		mov	g_divisor,eax

	; Scan forwards
	;
		PARAM_PI_DIRN comp[C_SZ*4],work.pz,f,1

	; Load up registers for first time around inner loop
	;
		mov	esi,work.main.i
		mov	edx,work.pz.current
		mov	ebp,work.pz.grad_x

		
		TRAPEZOID_PIZ2_RGB_565 top,f
		TRAPEZOID_PIZ2_RGB_565 bot,f
quit:		ret

	; Scan backwards
	;
reversed:
		neg	eax
		mov	g_divisor,eax

		PARAM_PI_DIRN comp[C_SZ*4],work.pz,b,1

	; Load up registers for first time around inner loop
	;
		mov	esi,work.main.i
		mov	edx,work.pz.current
		mov	ebp,work.pz.grad_x

		TRAPEZOID_PIZ2_RGB_565 top,b
		TRAPEZOID_PIZ2_RGB_565 bot,b
		ret


	SHOW_CODESIZE	<RawTriangle_PIZ2_RGB_565>,<%($-_setup_start)>

RawTriangle_PIZ2_RGB_565 endp

	endif

	if PARTS and PART_16Z

; void TriangleRenderPIZ2I_RGB_565(struct br_vertex *pvertex_0,
;			struct br_vertex *pvertex_1,
;			struct br_vertex *pvertex_2);
;
; C stub directly to triangle renderer
;
TriangleRenderPIZ2I_RGB_565 proc uses eax ebx ecx edx esi edi,
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
		call	RawTriangle_PIZ2I_RGB_565

		RESTORE_SEGS
		pop	ebp
		ret
TriangleRenderPIZ2I_RGB_565 endp

; Trapezoid loop for gouraud shaded z buffered faces
;
; Arguments control:
;	whether loop uses 'top' or 'bottom' variables
;	which direction scanline are rendered from the long edge
;
TRAPEZOID_PIZ2I_RGB_565	macro	half,dirn
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
		push	ecx

		mov eax,work.pr.current
		mov temp_r,eax
		mov eax,work.pg.current
		mov temp_g,eax
		mov eax,work.pb.current
		mov temp_b,eax
		
		mov edi,work.depth.base
		mov ebx,work.colour.base
		lea edi,[edi+esi*2]	;	z buffer
		
		lea esi,[esi*2]	; 	colour buffer
		lea ecx,[ecx*2]

	; Render same colour pixels along scanline (forwards or backwards)
	;
	; bl:	colour
	; ecx:	address of end of scanline
	; edx:	z (16.16)
	; ebp:	z delta (16.16)
	; esi:	frame buffer ptr
	; edi:	z buffer ptr
	;
		CMP_&dirn esi,ecx
		jae	short no_pixels
pixel_loop:

	ifidni <dirn>,<b>
		sub	edx,ebp		; z -= d_z_z
		mov eax,work.pr.grad_x
		sub temp_r,eax
		mov eax,work.pg.grad_x
		sub temp_g,eax
		mov eax,work.pb.grad_x
		sub temp_b,eax
		
		sub	edi,2		; z_ptr--
		sub	esi,2		; ptr--
	endif
		mov	eax,edx
		shr	eax,16
		cmp	ax, DSEG [edi]	; if (z > *zptr)
		jae	short pixel_behind	

		mov	DSEG [edi],ax	;	*zptr = z
		
		push ecx

		xor ecx,ecx
		
		mov	cl,byte ptr temp_r+2
		shr	cl,3
		mov	eax,ecx
		shl	eax,6
		mov	cl,byte ptr temp_g+2
		shr	cl,2
		or	al,cl
		shl	eax,5
		mov	cl,byte ptr temp_b+2
		shr	cl,3
		or	al,cl
		
		mov CSEG [esi+ebx],ax		;		rgb 5 6 5
		
		pop ecx
pixel_behind:

	ifidni <dirn>,<f>
		add	edx,ebp		; z += d_z_z
		mov eax,work.pr.grad_x
		add temp_r,eax
		mov eax,work.pg.grad_x
		add temp_g,eax
		mov eax,work.pb.grad_x
		add temp_b,eax
		add	edi,2		; z_ptr++
		add	esi,2		; ptr++
	endif
		CMP_&dirn esi,ecx
		jb	short pixel_loop

no_pixels:

	; Updates for next scanline:
	;
		pop ecx
		pop	edx
		pop	esi
		mov	edi,work.half.d_f
		mov	eax,work.main.d_f

		add	work.half.f,edi	    	; x_end += work.main.grad_end (fraction)
		adc	ecx,work.half.d_i 	; endptr += integer(dx_end)+CARRY

		add	work.main.f,eax		; x+= work.main.grad (fraction)
		jc	start_carried

		add	esi,work.main.d_i		; fb_offset+=delta+CARRY
		add	edx,work.pz.d_nocarry	; z
		
		mov eax,work.pr.d_nocarry		; red
		add work.pr.current,eax		; red
		mov eax,work.pg.d_nocarry		; green
		add work.pg.current,eax		; green
		mov eax,work.pb.d_nocarry		; blue
		add work.pb.current,eax		; blue

		dec	work.half.count
		jne	scan_loop
		jmp	short done_trapezoid

start_carried:	adc	esi,work.main.d_i		; fb_offset+=delta+CARRY
		add	edx,work.pz.d_carry
			
		mov eax,work.pr.d_carry		; red
		add work.pr.current,eax		; red
		mov eax,work.pg.d_carry		; green
		add work.pg.current,eax		; green	
		mov eax,work.pb.d_carry	; blue
		add work.pb.current,eax	; blue

		dec	work.half.count
		jne	scan_loop
done_trapezoid:
		endm

; RawTriangle_PIZ2I_RGB_565
;
; Render a triangle into frame buffer
;
;	Linear interpolated colour index
;	Linear interpolated Z value
;	Integer vertices
;	Perfect point sampling
;
; esi = vertex 0 pointer
; edi = vertex 1 pointer
; ebp = vertex 2 pointer
; 
RawTriangle_PIZ2I_RGB_565 proc
		SETUP_PI
		jl	reversed
		mov	g_divisor,eax
		
		PARAM_PI_DIRN comp[C_SZ*4],work.pz,f,1
		PARAM_PI_DIRN comp[C_R*4],work.pr,f
		PARAM_PI_DIRN comp[C_G*4],work.pg,f
		PARAM_PI_DIRN comp[C_B*4],work.pb,f
		
	; Load up registers for first time around inner loop
	;
		mov	esi,work.main.i
		mov	edx,work.pz.current
		mov	ebp,work.pz.grad_x

		TRAPEZOID_PIZ2I_RGB_565 top,f
		TRAPEZOID_PIZ2I_RGB_565 bot,f
quit:		ret

	; Scan backwards
	;
reversed:
		neg	eax
		mov	g_divisor,eax
		
		PARAM_PI_DIRN comp[C_SZ*4],work.pz,b,1
		PARAM_PI_DIRN comp[C_R*4],work.pr,b
		PARAM_PI_DIRN comp[C_G*4],work.pg,b
		PARAM_PI_DIRN comp[C_B*4],work.pb,b

	; Load up registers for first time around inner loop
	;
		mov	esi,work.main.i
		mov	edx,work.pz.current
		mov	ebp,work.pz.grad_x

		TRAPEZOID_PIZ2I_RGB_565 top,b
		TRAPEZOID_PIZ2I_RGB_565 bot,b
		ret

	SHOW_CODESIZE	<RawTriangle_PIZ2I_RGB_565>,<%($-_setup_start)>
RawTriangle_PIZ2I_RGB_565 endp

	endif

	if PARTS and PART_16Z4

; void TriangleRenderPIZ4_RGB_565(struct br_vertex *pvertex_0,
;			struct br_vertex *pvertex_1,
;			struct br_vertex *pvertex_2);
;
; C stub directly to triangle renderer
;
TriangleRenderPIZ4_RGB_565 proc uses eax ebx ecx edx esi edi,
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
		call	RawTriangle_PIZ4_RGB_565

		RESTORE_SEGS
		pop	ebp
		ret
TriangleRenderPIZ4_RGB_565 endp

; Trapezoid loop for flat shaded z (32 bit) buffered faces
;
; Arguments control:
;	whether loop uses 'top' or 'bottom' variables
;	which direction scanline are rendered from the long edge
;
TRAPEZOID_PIZ4_RGB_565	macro	half,dirn
   	local scan_loop,pixel_loop,pixel_behind,no_pixels
	local start_carried,done_trapezoid

		cmp	work.half.count,0
		je	done_trapezoid

		mov	ecx,work.half.i

	; Prepare for next iteration of loop
	;
scan_loop:	push	esi
		push	edx
		push ecx
		
		mov edi,work.depth.base
		mov ebx,work.colour.base
		lea edi,[edi+esi*4]	;	z buffer
		
		lea esi,[esi*2]	; 	colour buffer
		lea ecx,[ecx*2]


	; Render same colour pixels along scanline (forwards or backwards)
	;
	; bl:	colour
	; ecx:	address of end of scanline
	; edx:	z 	(16.16)
	; ebp:	z delta (16.16)
	; esi:	frame buffer ptr
	; edi:	z buffer ptr
	;
		CMP_&dirn esi,ecx
		jae	short no_pixels
pixel_loop:
	ifidni <dirn>,<b>
		sub	edx,ebp		; z -= d_z_z
		sub	edi,4		; z_ptr--
		sub	esi,2		; ptr--
	endif
		cmp	edx, DSEG [edi]	; if (z > *zptr)
		jae	short pixel_behind
		mov	DSEG [edi],edx	;	*zptr = z
		
		xor ax,ax
		mov al,byte ptr temp_r+2
		shl ax,6
		or al,byte ptr temp_g+2
		shl ax,5
		or al,byte ptr temp_b+2
		
		mov CSEG [esi+ebx],ax		;		rgb 5 6 5
pixel_behind:

	ifidni <dirn>,<f>
		add	edx,ebp		; z += d_z_z
		add	edi,4		; z_ptr++
		add	esi,2		; ptr++
	endif
		CMP_&dirn esi,ecx
		jb	short pixel_loop
no_pixels:

	; Updates for next scanline:
	;
		pop ecx
		pop	edx
		pop	esi
		mov	edi,work.half.d_f
		mov	eax,work.main.d_f
		add	work.half.f,edi	    	; x_end += work.main.grad_end (fraction)
		adc	ecx,work.half.d_i 	; endptr += integer(dx_end)+CARRY

		add	work.main.f,eax		; x+= work.main.grad (fraction)
		jc	start_carried

		add	esi,work.main.d_i		; fb_offset+=delta+CARRY
		add	edx,work.pz.d_nocarry

		dec	work.half.count
		jne	scan_loop
		jmp	short done_trapezoid

start_carried:	adc	esi,work.main.d_i		; fb_offset+=delta+CARRY
		add	edx,work.pz.d_carry

		dec	work.half.count
		jne	scan_loop
done_trapezoid:
	endm

; Triangle_PIZ4_RGB_565
;
; Render a triangle into frame buffer
;
;	Flat colour
;	Z buffer
;	Integer vertices
;	Perfect point sampling
;
; esi = vertex 0 pointer
; edi = vertex 1 pointer
; ebp = vertex 2 pointer
; 
RawTriangle_PIZ4_RGB_565 proc
		SETUP_PI
		jl	reversed
		mov	g_divisor,eax

	; Scan forwards
	;
		PARAM_PI_DIRN comp[C_SZ*4],work.pz,f,1

	; Load up registers for first time around inner loop
	;
		mov	esi,work.main.i
		mov	edx,work.pz.current
		mov	ebp,work.pz.grad_x
		mov ax,word ptr work.pr.current+2
		shr ax,3
		mov word ptr work.pr.current+2,ax
		mov ax,word ptr work.pg.current+2
		shr ax,2
		mov word ptr work.pg.current+2,ax
		mov ax,word ptr work.pb.current+2
		shr ax,3
		mov word ptr work.pb.current+2,ax
		mov eax,work.pr.current
		mov temp_r,eax
		mov eax,work.pg.current
		mov temp_g,eax
		mov eax,work.pb.current
		mov temp_b,eax

		TRAPEZOID_PIZ4_RGB_565 top,f
		TRAPEZOID_PIZ4_RGB_565 bot,f
quit:		ret

	; Scan backwards
	;
reversed:
		neg	eax
		mov	g_divisor,eax

		PARAM_PI_DIRN comp[C_SZ*4],work.pz,b,1

	; Load up registers for first time around inner loop
	;
		mov	esi,work.main.i
		mov	edx,work.pz.current
		mov	ebp,work.pz.grad_x
		mov ax,word ptr work.pr.current+2
		shr ax,3
		mov word ptr work.pr.current+2,ax
		mov ax,word ptr work.pg.current+2
		shr ax,2
		mov word ptr work.pg.current+2,ax
		mov ax,word ptr work.pb.current+2
		shr ax,3
		mov word ptr work.pb.current+2,ax
		mov eax,work.pr.current
		mov temp_r,eax
		mov eax,work.pg.current
		mov temp_g,eax
		mov eax,work.pb.current
		mov temp_b,eax

		TRAPEZOID_PIZ4_RGB_565 top,b
		TRAPEZOID_PIZ4_RGB_565 bot,b
		ret


	SHOW_CODESIZE	<RawTriangle_PIZ4_RGB_565>,<%($-_setup_start)>

RawTriangle_PIZ4_RGB_565 endp

	endif

	if PARTS and PART_16Z4

; void TriangleRenderPIZ4I_RGB_565(struct br_vertex *pvertex_0,
;			struct br_vertex *pvertex_1,
;			struct br_vertex *pvertex_2);
;
; C stub directly to triangle renderer
;
TriangleRenderPIZ4I_RGB_565 proc uses eax ebx ecx edx esi edi,
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
		call	RawTriangle_PIZ4I_RGB_565

		RESTORE_SEGS
		pop	ebp
		ret
TriangleRenderPIZ4I_RGB_565 endp

; Trapezoid loop for gouraud shaded z buffered faces
;
; Arguments control:
;	whether loop uses 'top' or 'bottom' variables
;	which direction scanline are rendered from the long edge
;
TRAPEZOID_PIZ4I_RGB_565	macro	half,dirn
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
		push ecx

		mov eax,work.pr.current
		mov temp_r,eax
		mov eax,work.pg.current
		mov temp_g,eax
		mov eax,work.pb.current
		mov temp_b,eax
		
		mov edi,work.depth.base
		mov ebx,work.colour.base
		lea edi,[edi+esi*4]	;	z buffer
		
		lea esi,[esi*2]	; 	colour buffer
		lea ecx,[ecx*2]

	; Render same colour pixels along scanline (forwards or backwards)
	;
	; bl:	colour
	; ecx:	address of end of scanline
	; edx:	z (16.16)
	; ebp:	z delta (16.16)
	; esi:	frame buffer ptr
	; edi:	z buffer ptr
	;
		CMP_&dirn esi,ecx
		jae	short no_pixels
pixel_loop:

	ifidni <dirn>,<b>
		sub	edx,ebp		; z -= d_z_z
		mov eax,work.pr.grad_x
		sub temp_r,eax
		mov eax,work.pg.grad_x
		sub temp_g,eax
		mov eax,work.pb.grad_x
		sub temp_b,eax
		
		sub	edi,4		; z_ptr--
		sub	esi,2		; ptr--
	endif

		cmp	edx, DSEG [edi]	; if (z > *zptr)
		jae	short pixel_behind

		mov	DSEG [edi],edx	;	*zptr = z

		push ecx

		xor ax,ax
		xor cx,cx
		
		mov cl,byte ptr temp_r+2
		shr cl,3
		or al,cl
		shl ax,6
		mov cl,byte ptr temp_g+2
		shr cl,2
		or al,cl
		shl ax,5
		mov cl,byte ptr temp_b+2
		shr cl,3
		or al,cl
		
		mov CSEG [esi+ebx],ax		;		rgb 5 6 5
		
		pop ecx
pixel_behind:

	ifidni <dirn>,<f>
		add	edx,ebp		; z += d_z_z
		mov eax,work.pr.grad_x
		add temp_r,eax
		mov eax,work.pg.grad_x
		add temp_g,eax
		mov eax,work.pb.grad_x
		add temp_b,eax
		add	edi,4		; z_ptr++
		add	esi,2		; ptr++
	endif
		CMP_&dirn esi,ecx
		jb	short pixel_loop

no_pixels:

	; Updates for next scanline:
	;
		pop ecx
		pop	edx
		pop	esi
		mov	edi,work.half.d_f
		mov	eax,work.main.d_f

		add	work.half.f,edi	    	; x_end += work.main.grad_end (fraction)
		adc	ecx,work.half.d_i 	; endptr += integer(dx_end)+CARRY

		add	work.main.f,eax		; x+= work.main.grad (fraction)
		jc	start_carried

		add	esi,work.main.d_i		; fb_offset+=delta+CARRY
		add	edx,work.pz.d_nocarry	; z
		
		mov eax,work.pr.d_nocarry		; red
		add work.pr.current,eax		; red
		mov eax,work.pg.d_nocarry		; green
		add work.pg.current,eax		; green
		mov eax,work.pb.d_nocarry		; blue
		add work.pb.current,eax		; blue

		dec	work.half.count
		jne	scan_loop
		jmp	short done_trapezoid

start_carried:	adc	esi,work.main.d_i		; fb_offset+=delta+CARRY
		add	edx,work.pz.d_carry
			
		mov eax,work.pr.d_carry		; red
		add work.pr.current,eax		; red
		mov eax,work.pg.d_carry		; green
		add work.pg.current,eax		; green	
		mov eax,work.pb.d_carry	; blue
		add work.pb.current,eax	; blue

		dec	work.half.count
		jne	scan_loop
done_trapezoid:
		endm


; RawTriangle_PIZ4I_RGB_565
;
; Render a triangle into frame buffer
;
;	Linear interpolated colour index
;	Linear interpolated Z value
;	Integer vertices
;	Perfect point sampling
;
; esi = vertex 0 pointer
; edi = vertex 1 pointer
; ebp = vertex 2 pointer
; 
RawTriangle_PIZ4I_RGB_565 proc
		SETUP_PI
		jl	reversed
		mov	g_divisor,eax
		
		PARAM_PI_DIRN comp[C_SZ*4],work.pz,f,1
		PARAM_PI_DIRN comp[C_R*4],work.pr,f
		PARAM_PI_DIRN comp[C_G*4],work.pg,f
		PARAM_PI_DIRN comp[C_B*4],work.pb,f
		
		mov ax,word ptr work.pr.current+2
		shr ax,3
		mov word ptr work.pr.current+2,ax
		mov ax,word ptr work.pg.current+2
		shr ax,2
		mov word ptr work.pg.current+2,ax
		mov ax,word ptr work.pb.current+2
		shr ax,3
		mov word ptr work.pb.current+2,ax

	; Load up registers for first time around inner loop
	;
		mov	esi,work.main.i
		mov	edx,work.pz.current
		mov	ebp,work.pz.grad_x

		TRAPEZOID_PIZ4I_RGB_565 top,f
		TRAPEZOID_PIZ4I_RGB_565 bot,f
quit:		ret

	; Scan backwards
	;
reversed:
		neg	eax
		mov	g_divisor,eax

		PARAM_PI_DIRN comp[C_SZ*4],work.pz,b,1
		PARAM_PI_DIRN comp[C_R*4],work.pr,b
		PARAM_PI_DIRN comp[C_G*4],work.pg,b
		PARAM_PI_DIRN comp[C_B*4],work.pb,b
		
		mov ax,word ptr work.pr.current+2
		shr ax,3
		mov word ptr work.pr.current+2,ax
		mov ax,word ptr work.pg.current+2
		shr ax,2
		mov word ptr work.pg.current+2,ax
		mov ax,word ptr work.pb.current+2
		shr ax,3
		mov word ptr work.pb.current+2,ax

	; Load up registers for first time around inner loop
	;
		mov	esi,work.main.i
		mov	edx,work.pz.current
		mov	ebp,work.pz.grad_x

		TRAPEZOID_PIZ4I_RGB_565 top,b
		TRAPEZOID_PIZ4I_RGB_565 bot,b
		ret

	SHOW_CODESIZE	<RawTriangle_PIZ4I_RGB_565>,<%($-_setup_start)>
RawTriangle_PIZ4I_RGB_565 endp
	endif
		end
