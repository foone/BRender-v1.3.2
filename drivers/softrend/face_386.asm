;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: face_386.asm 1.3 1998/07/20 14:45:11 jon Exp $
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
	db '$Id: face_386.asm 1.3 1998/07/20 14:45:11 jon Exp $',0
	align	16
endif

	.code

	option	prologue:none
	option	epilogue:none

; void BR_ASM_CALL OpTriangleMapQuad(struct brp_block *block, union brp_vertex *v0, union brp_vertex *v1, union brp_vertex *v2, ...)

OpTriangleMapQuad_A proc,
	block : ptr brp_block,  
	v0 : ptr brp_vertex,
	v1 : ptr brp_vertex,
	v2 : ptr brp_vertex

		push	ebp
		mov		ebp, esp

	; Fetch the index of the current face and address of the flags array
	;
		mov		eax, rend.face_flags
		mov		ecx, rend.current_index

		push	esi
		xor		edx, edx

	; Fetch the face flags for the current face, mask out the quad flags and shift to make a table
	; offset
	;
	; Meanwhile, find the address of the next brp_block in the chain, modify the block pointer on
	; the stack and lookup the next render routine, and get the current renderer pointer
	;
		mov		dl, [eax+ecx]
		mov		esi, block

		and		dl, BR_FACEF_QUAD_MASK
		push	edi

	if BR_FACEF_QUAD_SHIFT GT 2
		shr		dl, BR_FACEF_QUAD_SHIFT - 2
	elseif BR_FACEF_QUAD_SHIFT LT 2
		shl		dl, 2 - BR_FACEF_QUAD_SHIFT
	endif
		mov		eax, [esi].brp_block.chain

		mov		esi, rend.renderer
		mov		block, eax

		mov		eax, [eax].brp_block.render
		
	; Jump to the appropriate fixup routine
	;
		jmp		dword ptr MapQuad_table[edx]


	.data

MapQuad_table	label ptr dword

		dd		MapQuad_012
		dd		MapQuad_123
		dd		MapQuad_230
		dd		MapQuad_301
		dd		MapQuad_032
		dd		MapQuad_103
		dd		MapQuad_210
		dd		MapQuad_321


	.code

MAPQUAD macro	a, b, c

	; Copy the mapping coordinates for the appropriate corners of the texture into each vertex in
	; turn (this doesn't break anything because these vertices MUST all be quad mapped.
	;
		mov		edi, v0
		mov		ecx, [esi].br_renderer._state.cache.quad_transformed[a * 8].v[0 * 4]

		mov		edx, [esi].br_renderer._state.cache.quad_transformed[a * 8].v[1 * 4]
		mov		[edi].brp_vertex.comp[C_U * 4], ecx

		mov		[edi].brp_vertex.comp[C_V * 4], edx

		mov		edi, v1
		mov		ecx, [esi].br_renderer._state.cache.quad_transformed[b * 8].v[0 * 4]

		mov		edx, [esi].br_renderer._state.cache.quad_transformed[b * 8].v[1 * 4]
		mov		[edi].brp_vertex.comp[C_U * 4], ecx

		mov		[edi].brp_vertex.comp[C_V * 4], edx

		mov		edi, v2
		mov		ecx, [esi].br_renderer._state.cache.quad_transformed[c * 8].v[0 * 4]

		mov		edx, [esi].br_renderer._state.cache.quad_transformed[c * 8].v[1 * 4]
		mov		[edi].brp_vertex.comp[C_U * 4], ecx

		mov		[edi].brp_vertex.comp[C_V * 4], edx

	; Restore the stack and jump to the next operation in the chain
	;
		pop		edi

		pop		esi
		pop		ebp

		jmp		eax
		
		endm


MapQuad_012 label ptr code
		MAPQUAD	0, 1, 2

MapQuad_123 label ptr code
		MAPQUAD	1, 2, 3

MapQuad_230 label ptr code
		MAPQUAD	2, 3, 0

MapQuad_301 label ptr code
		MAPQUAD	3, 0, 1

MapQuad_032 label ptr code
		MAPQUAD	0, 3, 2

MapQuad_103 label ptr code
		MAPQUAD	1, 0, 3

MapQuad_210 label ptr code
		MAPQUAD	2, 1, 0

MapQuad_321 label ptr code
		MAPQUAD	3, 2, 1

OpTriangleMapQuad_A endp

end

