;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: gen_386.asm 1.2 1998/07/20 14:45:11 jon Exp $
;; $Locker: $
;;
;; Downcoded general geometry support
;;
	.586p
	.model flat,c

	include	drv.inc
	include 586_macs.inc

if ADD_RCS_ID
	.data
	db '$Id: gen_386.asm 1.2 1998/07/20 14:45:11 jon Exp $',0
	align	16
endif

	.code

	if BASED_FIXED	; Only provide these when driver is fixed point

	; Need to be converted to use separate face equations and
	; vertex indices
	;
	if 0

	include vertex.inc

VERTEX_TRANSFORM_PROJECT macro

		; W
		;
		mov	eax,scache.model_to_viewport.m[0+12]
		imul	[esi].fmt_vertex.p.v[0]
		mov	ebp,eax
		mov	ecx,edx

		mov	eax,scache.model_to_viewport.m[16+12]
		imul	[esi].fmt_vertex.p.v[4]
		add	ebp,eax
		adc	ecx,edx

		mov	eax,scache.model_to_viewport.m[32+12]
		imul	[esi].fmt_vertex.p.v[8]
		add	ebp,eax
		adc	ecx,edx

		shr	ebp,16
		shl	ecx,16
		or	ebp,ecx

		add	ebp,scache.model_to_viewport.m[48+12]

		; X
		;
		mov	ebx,scache.model_to_viewport.m[48+0]
		mov	ecx,scache.model_to_viewport_hi[0]

		mov	eax,scache.model_to_viewport.m[0+0]
		imul	[esi].fmt_vertex.p.v[0]
		add	ebx,eax
		adc	ecx,edx

		mov	eax,scache.model_to_viewport.m[16+0]
		imul	[esi].fmt_vertex.p.v[4]
		add	ebx,eax
		adc	ecx,edx

		mov	eax,scache.model_to_viewport.m[32+0]
		imul	[esi].fmt_vertex.p.v[8]
		add	eax,ebx
		adc	edx,ecx

		idiv	ebp
		add	eax,scache.offset_x
		mov	[edi].brp_vertex.comp_x[C_SX*4],eax

		; Y
		;
		mov	ebx,scache.model_to_viewport.m[48+4]
		mov	ecx,scache.model_to_viewport_hi[4]

		mov	eax,scache.model_to_viewport.m[0+4]
		imul	[esi].fmt_vertex.p.v[0]
		add	ebx,eax
		adc	ecx,edx

		mov	eax,scache.model_to_viewport.m[16+4]
		imul	[esi].fmt_vertex.p.v[4]
		add	ebx,eax
		adc	ecx,edx

		mov	eax,scache.model_to_viewport.m[32+4]
		imul	[esi].fmt_vertex.p.v[8]
		add	eax,ebx
		adc	edx,ecx

		idiv	ebp
		add	eax,scache.offset_y
		mov	[edi].brp_vertex.comp_x[C_SY*4],eax

		; Z
		;
		mov	ebx,scache.model_to_viewport.m[48+8]
		mov	ecx,scache.model_to_viewport_hi[8]

		mov	eax,scache.model_to_viewport.m[0+8]
		imul	[esi].fmt_vertex.p.v[0]
		add	ebx,eax
		adc	ecx,edx

		mov	eax,scache.model_to_viewport.m[16+8]
		imul	[esi].fmt_vertex.p.v[4]
		add	ebx,eax
		adc	ecx,edx

		mov	eax,scache.model_to_viewport.m[32+8]
		imul	[esi].fmt_vertex.p.v[8]
		add	eax,ebx
		adc	edx,ecx

		mov	[edi].brp_vertex.comp[C_W*4],ebp		; Set comp[C_W]

		idiv	ebp
		add	eax,scache.offset_z


		mov	[edi].brp_vertex.comp[C_SZ*4],eax
endm

BOUND_CHECK	macro	o
		local not_min,not_max
		cmp	eax,scache.min.v[o]
		jge	not_min
		mov	scache.min.v[o],eax
not_min:

		cmp	eax,scache.max.v[o]
		jle	not_max
		mov	scache.max.v[o],eax
not_max:
		endm

VERTEX_TRANSFORM_PROJECT_CHECK	macro

		; W
		;
		mov	eax,scache.model_to_viewport.m[0+12]
		imul	[esi].fmt_vertex.p.v[0]
		mov	ebp,eax
		mov	ecx,edx

		mov	eax,scache.model_to_viewport.m[16+12]
		imul	[esi].fmt_vertex.p.v[4]
		add	ebp,eax
		adc	ecx,edx

		mov	eax,scache.model_to_viewport.m[32+12]
		imul	[esi].fmt_vertex.p.v[8]
		add	ebp,eax
		adc	ecx,edx

		shr	ebp,16
		shl	ecx,16
		or	ebp,ecx

		add	ebp,scache.model_to_viewport.m[48+12]
		

		; X
		;
		mov	ebx,scache.model_to_viewport.m[48+0]
		mov	ecx,scache.model_to_viewport_hi[0]

		mov	eax,scache.model_to_viewport.m[0+0]
		imul	[esi].fmt_vertex.p.v[0]
		add	ebx,eax
		adc	ecx,edx

		mov	eax,scache.model_to_viewport.m[16+0]
		imul	[esi].fmt_vertex.p.v[4]
		add	ebx,eax
		adc	ecx,edx

		mov	eax,scache.model_to_viewport.m[32+0]
		imul	[esi].fmt_vertex.p.v[8]
		add	eax,ebx
		adc	edx,ecx

		idiv	ebp
		add	eax,scache.offset_x
		mov	[edi].brp_vertex.comp[C_SX*4],eax

		BOUND_CHECK 0

		; Y
		;
		mov	ebx,scache.model_to_viewport.m[48+4]
		mov	ecx,scache.model_to_viewport_hi[4]

		mov	eax,scache.model_to_viewport.m[0+4]
		imul	[esi].fmt_vertex.p.v[0]
		add	ebx,eax
		adc	ecx,edx

		mov	eax,scache.model_to_viewport.m[16+4]
		imul	[esi].fmt_vertex.p.v[4]
		add	ebx,eax
		adc	ecx,edx

		mov	eax,scache.model_to_viewport.m[32+4]
		imul	[esi].fmt_vertex.p.v[8]
		add	eax,ebx
		adc	edx,ecx

		idiv	ebp
		add	eax,scache.offset_y
		mov	[edi].brp_vertex.comp[C_SY*4],eax

		BOUND_CHECK 4

		; Z
		;
		mov	ebx,scache.model_to_viewport.m[48+8]
		mov	ecx,scache.model_to_viewport_hi[8]

		mov	eax,scache.model_to_viewport.m[0+8]
		imul	[esi].fmt_vertex.p.v[0]
		add	ebx,eax
		adc	ecx,edx

		mov	eax,scache.model_to_viewport.m[16+8]
		imul	[esi].fmt_vertex.p.v[4]
		add	ebx,eax
		adc	ecx,edx

		mov	eax,scache.model_to_viewport.m[32+8]
		imul	[esi].fmt_vertex.p.v[8]
		add	eax,ebx
		adc	edx,ecx

		mov	[edi].brp_vertex.comp[C_W*4],ebp		; Set comp[C_W]

		idiv	ebp
		add	eax,scache.offset_z

		mov	[edi].brp_vertex.comp[C_SZ*4],eax

		endm

; Call lighting function - taking colour from surface
;
VERTEX_LIGHT_SURF	macro
		local	light_loop

	; renderer	pointer
	; point 	pointer
	; map		pointer
	; normal 	pointer
	; colour
	; components
	;
		lea	eax,[edi].brp_vertex.comp
		mov		eax,scache.colour

		push	eax
		push	ebx

		lea	eax,[esi].fmt_vertex.n
		lea	ebx,[esi].fmt_vertex.map

		push	eax
		push	ebx

		push	esi

		mov	ebx,rend.renderer
		push	ebx
		
		lea	ebx,[ebx].br_renderer._state.cache.vertex_fns

	; Assumes at least one function in table
	;
		mov	eax,[ebx]
		add	ebx,4

light_loop:	call	eax
		mov	eax,[ebx]
		add	ebx,4
		test	eax,eax
		jne	light_loop

		add	esp,24

endm

; Call lighting function - taking colour from geometry
;
VERTEX_LIGHT_GEOM	macro
		local	light_loop

	; renderer	pointer
	; point 	pointer
	; map		pointer
	; normal 	pointer
	; colour
	; components
	;
		lea	eax,[edi].brp_vertex.comp
		mov		ebx,scache.colour

		push	eax
		push	ebx

		lea	eax,[esi].fmt_vertex.n
		lea	ebx,[esi].fmt_vertex.map

		push	eax
		push	ebx


		push	esi
		mov	ebx,rend.renderer
		push	ebx
		
		lea	ebx,[ebx].br_renderer._state.cache.vertex_fns

	; Assumes at least one function in table
	;
		mov	eax,[ebx]
		add	ebx,4

light_loop:	call	eax
		mov	eax,[ebx]
		add	ebx,4
		test	eax,eax
		jne	light_loop

		add	esp,24

endm

; Premultiply viewport into model->screen matrix 
;
; scache.model_to_viewport[0..3][0] = scache.model_to_screen[0.[0..3][0] * scales[X]
; scache.model_to_viewport[0..3][1] = scache.model_to_screen[0..3][1] *  scales[Y]
; scache.model_to_viewport[0..3][2] = scache.model_to_screen[0..3][2] * -1
;
; Keep a 64 bit result for row 3 - the translations and perspective
; that will not be multiplied (incoming points are x,y,z,1)
;
; Return !0 if there was an overflow
;
ModelToViewportUpdate_A proc uses ebx esi edi

	; Loop for rows 0-2
	;
				mov	ecx,3
				xor	esi,esi

row_loop:
		; X
				mov	eax, scache.model_to_screen.m[esi+0]
				imul	scache.scale_x

				mov	ebx,edx
				and	ebx,0ffff8000h
				je	no_x_overflow
				cmp	ebx,0ffff8000h
				jne	overflowed

no_x_overflow:	shr	eax,16
				shl	edx,16
				or	eax,edx

				mov	scache.model_to_viewport.m[esi+0],eax

		; Y
				mov	eax,scache.model_to_screen.m[esi+4]
				imul	scache.scale_y

				mov	ebx,edx
				and	ebx,0ffff8000h
				je	no_y_overflow
				cmp	ebx,0ffff8000h
				jne	overflowed

no_y_overflow:	shr	eax,16
				shl	edx,16
				or	eax,edx

				mov	scache.model_to_viewport.m[esi+4],eax


	; Y
				mov	eax,scache.model_to_screen.m[esi+8]
				imul	scache.scale_z

				mov	ebx,edx
				and	ebx,0ffff8000h
				je	no_z_overflow
				cmp	ebx,0ffff8000h
				jne	overflowed

no_z_overflow:	shr	eax,16
				shl	edx,16
				or	eax,edx

				mov	scache.model_to_viewport.m[esi+8],eax


		; W
				mov	eax,scache.model_to_screen.m[esi+12]
				mov	scache.model_to_viewport.m[esi+12],eax

				add	esi,16
				dec	ecx
				jne	row_loop

	; row 3
	;
		; X
				mov	eax, scache.model_to_screen.m[48+0]
				imul	scache.scale_x
				mov	scache.model_to_viewport.m[48+0],eax
				mov	scache.model_to_viewport_hi[0],edx

		; Y
				mov	eax,scache.model_to_screen.m[48+4]
				imul	scache.scale_y
				mov	scache.model_to_viewport.m[48+4],eax
				mov	scache.model_to_viewport_hi[4],edx

		; Z
				mov	eax,scache.model_to_screen.m[48+8]
				imul	scache.scale_z
				mov	scache.model_to_viewport.m[48+8],eax
				mov	scache.model_to_viewport_hi[8],edx

		; W
				mov	eax,scache.model_to_screen.m[48+12]
				mov	scache.model_to_viewport.m[48+12],eax

				xor	eax,eax
				ret
overflowed:
				mov	eax,-1
				ret

ModelToViewportUpdate_A endp


; Transform and projects a groups of vertices
;
Vertex_OS_TransformProject_A proc uses ebx esi edi,
	self : ptr word,  
	renderer : ptr word

		push	ebp

		mov	eax,rend.nvertices
		mov	scache.loop_count,eax
		mov	esi,rend.vertices
		mov	edi,rend.temp_vertices
		mov	ebp,rend.vertex_counts

	align	16
vertex_loop:
		cmp	byte ptr [ebp],0
		je	next_vertex

		push	ebp
		VERTEX_TRANSFORM_PROJECT
		pop	ebp
	; Next vertex
	;
next_vertex:
		inc	ebp
		dec	scache.loop_count
		lea	esi,[esi+FMT_VERTEX_SIZE]
		lea	edi,[edi+BRP_VERTEX_SIZE]
		jne	vertex_loop

		pop	ebp
		ret
Vertex_OS_TransformProject_A endp

; Transform, projects and bounds checks a groups of vertices
;
Vertex_OS_TransformProjectBounds_A proc uses ebx esi edi,
	self : ptr word,  
	renderer : ptr word

		push	ebp

		mov	eax,rend.nvertices
		mov	scache.loop_count,eax
		mov	esi,rend.vertices
		mov	edi,rend.temp_vertices
		mov	ebp,rend.vertex_counts

	align	16
vertex_loop:
		cmp	byte ptr [ebp],0
		je	next_vertex

		push	ebp
		VERTEX_TRANSFORM_PROJECT_CHECK
		pop	ebp
	; Next vertex
	;
next_vertex:
		inc	ebp
		dec	scache.loop_count
		lea	esi,[esi+FMT_VERTEX_SIZE]
		lea	edi,[edi+BRP_VERTEX_SIZE]
		jne	vertex_loop

		pop	ebp
		ret
Vertex_OS_TransformProjectBounds_A endp

; Transform, project and light a group of vertices
;
;
Vertex_OS_TransformProjectSurf_A proc uses ebx esi edi,
	self : ptr word,  
	renderer : ptr word

		push	ebp

		mov	eax,rend.nvertices
		mov	scache.loop_count,eax
		mov	esi,rend.vertices
		mov	edi,rend.temp_vertices
		mov	ebp,rend.vertex_counts

	align	16
vertex_loop:
		
		cmp	byte ptr [ebp],0
		je	next_vertex

		push	ebp
		VERTEX_TRANSFORM_PROJECT
		VERTEX_LIGHT_SURF
		pop	ebp

	; Next vertex
	;
next_vertex:
		inc	ebp
		dec	scache.loop_count
		lea	esi,[esi+FMT_VERTEX_SIZE]
		lea	edi,[edi+BRP_VERTEX_SIZE]
		jne	vertex_loop

		pop	ebp
		ret
Vertex_OS_TransformProjectSurf_A endp

; Transform, project and light a group of vertices
;
; br_uint_32 BR_ASM_CALL ZbOSTVGroupLitBC_A(br_vertex *vp, struct temp_vertex_fixed *tvp,int count, br_uint_8 *countp);
;

Vertex_OS_TransformProjectBoundsSurf_A proc uses ebx esi edi,
	self : ptr word,  
	renderer : ptr word

		push	ebp

		mov	eax,rend.nvertices
		mov	scache.loop_count,eax
		mov	esi,rend.vertices
		mov	edi,rend.temp_vertices
		mov	ebp,rend.vertex_counts
	align	16
vertex_loop:
		cmp	byte ptr [ebp],0
		je	next_vertex

		push	ebp
		VERTEX_TRANSFORM_PROJECT_CHECK
		VERTEX_LIGHT_SURF
		pop	ebp

	; Next vertex
	;
next_vertex:
		inc	ebp
		dec	scache.loop_count
		lea	esi,[esi+FMT_VERTEX_SIZE]
		lea	edi,[edi+BRP_VERTEX_SIZE]
		jne	vertex_loop

		pop	ebp
		ret
Vertex_OS_TransformProjectBoundsSurf_A endp

; Transform, project and light a group of vertices
;
;
Vertex_OS_TransformProjectGeom_A proc uses ebx esi edi,
	self : ptr word,  
	renderer : ptr word

		push	ebp

		mov	eax,rend.nvertices
		mov	scache.loop_count,eax
		mov	esi,rend.vertices
		mov	edi,rend.temp_vertices
		mov	ebp,rend.vertex_counts
	align	16
vertex_loop:
		
		cmp	byte ptr [ebp],0
		je	next_vertex

		push	ebp
		VERTEX_TRANSFORM_PROJECT
		VERTEX_LIGHT_GEOM
		pop	ebp

	; Next vertex
	;
next_vertex:
		inc	ebp
		dec	scache.loop_count
		lea	esi,[esi+FMT_VERTEX_SIZE]
		lea	edi,[edi+BRP_VERTEX_SIZE]
		jne	vertex_loop

		pop	ebp
		ret
Vertex_OS_TransformProjectGeom_A endp

; Transform, project and light a group of vertices
;
; br_uint_32 BR_ASM_CALL ZbOSTVGroupLitBC_A(br_vertex *vp, struct temp_vertex_fixed *tvp,int count, br_uint_8 *countp);
;

Vertex_OS_TransformProjectBoundsGeom_A proc uses ebx esi edi,
	self : ptr word,  
	renderer : ptr word

		push	ebp

		mov	eax,rend.nvertices
		mov	scache.loop_count,eax
		mov	esi,rend.vertices
		mov	edi,rend.temp_vertices
		mov	ebp,rend.vertex_counts
	align	16
vertex_loop:
		cmp	byte ptr [ebp],0
		je	next_vertex

		push	ebp
		VERTEX_TRANSFORM_PROJECT_CHECK
		VERTEX_LIGHT_GEOM
		pop		ebp

	; Next vertex
	;
next_vertex:
		inc	ebp
		dec	scache.loop_count
		lea	esi,[esi+FMT_VERTEX_SIZE]
		lea	edi,[edi+BRP_VERTEX_SIZE]
		jne	vertex_loop

		pop	ebp
		ret

Vertex_OS_TransformProjectBoundsGeom_A endp

	endif

	endif	; BASED_FIXED
end

