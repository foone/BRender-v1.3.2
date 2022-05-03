;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: v1m_386.asm 1.3 1998/07/20 19:53:40 jon Exp $
;; $Locker: $
;;
;; Downcoded model support
;;
	.586p
	.model flat,c

	include	drv.inc
	include 586_macs.inc

if ADD_RCS_ID
	.data
	db '$Id: v1m_386.asm 1.3 1998/07/20 19:53:40 jon Exp $',0
	align	16
endif
	.data
loop_count	dd	0

	.code

	if BASED_FIXED	; Only provide these when driver is fixed point

	; Need to be converted to use separate face equations and
	; vertex indices
	;
	if 1

	include vertex.inc

; Process one on-screen group of culled faces
;
V1Face_CullOneSidedPerspective_A proc uses ebx esi edi,
	self : ptr word,  
	renderer : ptr word

		push	ebp

		mov	eax,rend.nfaces
		mov	loop_count,eax

		mov	esi,rend.face_eqn
		mov	edi,rend.face_vertices
		mov	ebp,rend.temp_faces
		
	; Loop for each face in group
	;
	; edx:eax	Multiply result
	; ecx:ebx	Multiply accumulator
	; esi 		Current br_face pointer
	; ebp 		Current temp_face pointer
	; edi		zb.vertex_counts
	;

	align	16

face_loop:

	; Dot the face normal with the eye position
	;		
		mov		eax,[esi].br_vector4.v[0]
		imul	scache.eye_m.v[0]
		mov		ebx,eax
		mov		ecx,edx

		mov		eax,[esi].br_vector4.v[4]
		imul	scache.eye_m.v[4]
		add		ebx,eax
		adc		ecx,edx

		mov		eax,[esi].br_vector4.v[8]
		imul	scache.eye_m.v[8]
		add		eax,ebx
		adc		edx,ecx

		shr		eax,16
		shl		edx,16
		or		eax,edx

	; If n.eye > face->d then face is visible
	;
		cmp		eax,[esi].br_vector4.v[12]
		jle		not_visible

	; Mark vertices as visible
	;
		mov		edx,rend.vertex_counts

	; Get vertex 0,1,2
	;
		mov		eax,dword ptr [edi+0]
		mov		ecx,dword ptr [edi+4]
		mov		ebx,dword ptr [edi+2]
		and		eax,0000ffffh
		and		ebx,0000ffffh
		and		ecx,0000ffffh

	; Mark face as visible
	;
		mov	[ebp].temp_face.flag, TFF_VISIBLE ; temp_face flag

	; Mark vertices as referenced
	;
		inc		byte ptr [edx+eax]
		inc		byte ptr [edx+ebx]
		inc		byte ptr [edx+ecx]

	; Next vertex
	;
		dec		loop_count
		lea		esi,[esi+sizeof br_vector4]
		lea		edi,[edi+3*sizeof word]
		lea		ebp,[ebp+sizeof temp_face]
		jne		face_loop

		pop		ebp
		ret

	; Next face
	;
not_visible:
		mov		byte ptr [ebp].temp_face.flag,0		; temp_face flag

		dec		loop_count
		lea		esi,[esi+sizeof br_vector4]
		lea		edi,[edi+3*sizeof word]
		lea		ebp,[ebp+sizeof temp_face]
		jne		face_loop

		pop		ebp
		ret
V1Face_CullOneSidedPerspective_A endp

; Process one on-screen group of culled faces
;
V1Face_OS_CullOneSidedPerspective_A proc uses ebx esi edi,
	self : 	ptr word,
	renderer : ptr word

		push	ebp

		mov		eax,rend.nfaces
		mov		loop_count,eax

		mov		esi,rend.face_eqn
		mov		edi,rend.face_vertices
		mov		ebp,rend.temp_faces

	; Loop for each face in group
	;
	; edx:eax	Multiply result
	; ecx:ebx	Multiply accumulator
	; esi 		Current br_face pointer
	; ebp 		Current temp_face pointer
	; edi		zb.vertex_counts
	;
	align	16
face_loop:

	; Dot the face normal with the eye position
	;		
		mov		eax,[esi].br_vector4.v[0]
		imul	scache.eye_m.v[0]
		mov		ebx,eax
		mov		ecx,edx

		mov		eax,[esi].br_vector4.v[4]
		imul	scache.eye_m.v[4]
		add		ebx,eax
		adc		ecx,edx

		mov		eax,[esi].br_vector4.v[8]
		imul	scache.eye_m.v[8]
		add		eax,ebx
		adc		edx,ecx

		shr		eax,16
		shl		edx,16
		or 		eax,edx

	; If n.eye > face->d then face is visible
	;
		cmp		eax,[esi].br_vector4.v[12]
		jle		not_visible

	; Mark vertices as visible
	;
		mov		edx,rend.vertex_counts

	; Get vertex 0,1,2
	;
		mov		eax,dword ptr [edi+0]
		mov		ecx,dword ptr [edi+4]
		mov		ebx,dword ptr [edi+2]
		and		eax,0000ffffh
		and		ebx,0000ffffh
		and		ecx,0000ffffh

	; Mark face as visible
	;
		mov		[ebp].temp_face.flag, TFF_VISIBLE ; temp_face flag

	; Mark vertices as referenced
	;
		mov		byte ptr [edx+eax],1
		mov		byte ptr [edx+ebx],1
		mov		byte ptr [edx+ecx],1

	; Next vertex
	;
		dec		loop_count
		lea		esi,[esi+sizeof br_vector4]
		lea		edi,[edi+3*sizeof word]
		lea		ebp,[ebp+sizeof temp_face]
		jne		face_loop

		pop		ebp
		ret

	; Next face
	;
not_visible:
		mov		byte ptr [ebp].temp_face.flag,0		; temp_face flag

		dec		loop_count
		lea		esi,[esi+sizeof br_vector4]
		lea		edi,[edi+3*sizeof word]
		lea		ebp,[ebp+sizeof temp_face]
		jne		face_loop

		pop		ebp
		ret
V1Face_OS_CullOneSidedPerspective_A endp

	endif

	endif	; BASED_FIXED

end

