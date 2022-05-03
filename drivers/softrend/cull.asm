.686p
.model	flat,c

include	drv.inc
include pro_epi.inc
;include klamath.inc


.code


;static void GEOMETRY_CALL V1Face_CullOneSidedPerspective(struct br_geometry *self, struct br_renderer *renderer)
;{
;	int f,n;
;	struct v11face *fp = rend.faces;
;	struct temp_face *tfp = rend.temp_faces;
;	union {
;		float f;
;		int i;
;	}result;
;
;//	rend.nvisible_faces = 0;
;
; 	for(f=0,n=0; f < rend.nfaces; f++, fp++, tfp++) {
;
;		/*
;		 * if Plane_Eqn . Eye <= 0, face is away from eye
;		 */
;		result.f=BrVector3Dot((br_vector3 *)&fp->eqn,&scache.eye_m)-fp->eqn.v[3];
;		if(result.i&0x80000000){
;			tfp->flag = 0;
;			continue;
;		}
;
;		tfp->flag = TFF_VISIBLE;
;
;		rend.vertex_counts[fp->vertices[0]]++;
;		rend.vertex_counts[fp->vertices[1]]++;
;		rend.vertex_counts[fp->vertices[2]]++;
;
;//		rend.nvisible_faces++;
;	}
;}

if BASED_FLOAT

V1Face_CullOneSidedPerspective_A proc self:ptr geometry, renderer:ptr br_renderer

	; Read face count and finish if no faces
	;
		mov	ecx, rend.nfaces

		and	ecx, ecx
		jz	done

	; Read pointers to face equations, face vertex indices and temporary face
	; data
	;
	; Clear high words of accumulator registers
	;
	; Bias temporary face data pointer for later
	;
		mov	esi, rend.face_eqn
		mov	edi, rend.temp_faces

		assume esi:ptr br_vector4
		assume edi:ptr temp_face

		mov	ebp, rend.face_vertices
		xor	eax, eax

		assume ebp:ptr word

		xor	ebx, ebx
		sub	edi, sizeof temp_face

next:

	; Start plane equation test
								;	0		1		2
		fld [esi].v[0]			;	a
		fmul scache.eye_m.v[0]	;	a.x
		fld [esi].v[4]			;	b		a.x
		fmul scache.eye_m.v[4]	;	b.y		a.x
		fld [esi].v[8]			;	c		b.y		a.x
		fmul scache.eye_m.v[8]	;	c.z		b.y		a.x
		 fxch st(2)				;	a.x		b.y		c.z
		faddp st(1), st			;	a.x+b.y	c.z
		 fxch st(1)				;	c.z		a.x+b.y

		add	edi, sizeof temp_face
		;

		fsub [esi].v[12]		;	c.z-d	a.x+b.y

	; Store current loop counter, advance temporary face data
	; pointer and clear face visible flag (may be set later)
	;
		mov	count, ecx
		xor	edx, edx

		mov	[edi].flag, dl
		;

	; Finish plane equation test
	;
		faddp st(1), st			;	a.x+b.y+c.z-d

	; Read first and second vertex numbers
	;
		mov	al, byte ptr [ebp+0]
		mov	bl, byte ptr [ebp+4]

		mov	ah, byte ptr [ebp+1]
		mov	bh, byte ptr [ebp+5]

	; Check result of plane equation test
	;
		fstp result

		mov	dl, byte ptr result + 3
		mov	dh, TFF_VISIBLE

		and	dl, dl
		jl	behind

	; Store current face equation pointer and read in vertex use
	; counter pointer
	;
		mov	eqn, esi
		mov	esi, rend.vertex_counts

		assume esi:ptr byte

	; Write visible flag for face, read third vertex number and
	; increment use counters
	;
		mov	dl, byte ptr [ebp+2]
		mov	[edi].flag, dh

		mov	cl, [esi+eax]
		mov	dh, byte ptr [ebp+3]

		inc	cl
		mov	ch, [esi+ebx]

		mov	[esi+eax], cl
		mov	cl, [esi+edx]

		inc	ch
		inc	cl

		mov	[esi+ebx], ch
		mov	[esi+edx], cl

	; Restore saved pointer and counter
	;
		mov	esi, eqn
		mov	ecx, count

		assume esi:ptr br_vector4

behind:

	; Advance face equation and vertex number pointers and loop for next face
	;
		add esi, sizeof br_vector4
		add ebp, 3 * sizeof word

		dec	ecx
		jnz	next

done:

		ret

V1Face_CullOneSidedPerspective_A endp

V1Face_CullOneSidedPerspective_P6_A proc self:ptr geometry, renderer:ptr br_renderer

	; Read face count and finish if no faces
	;
		mov	ecx, rend.nfaces

		and	ecx, ecx
		jz	done

	; Read pointers to face equations, face vertex indices and temporary face
	; data
	;
	; Clear high words of accumulator registers
	;
		mov	esi, rend.face_eqn
		mov	edi, rend.temp_faces
		mov	ebp, rend.face_vertices
		mov	ebx, rend.vertex_counts

		assume esi:ptr br_vector4
		assume edi:ptr temp_face
		assume ebp:ptr word
		assume ebx:ptr byte

		xor	eax, eax

next:

	; Do plane equation test
								;	0		1		2
		fld [esi].v[0]			;	a
		fmul scache.eye_m.v[0]	;	a.x
		fld [esi].v[4]			;	b		a.x
		fmul scache.eye_m.v[4]	;	b.y		a.x
		fld [esi].v[8]			;	c		b.y		a.x
		fmul scache.eye_m.v[8]	;	c.z		b.y		a.x
		 fxch st(2)				;	a.x		b.y		c.z
		faddp st(1), st			;	a.x+b.y	c.z
		 fxch st(1)				;	c.z		a.x+b.y
		fsub [esi].v[12]		;	c.z-d	a.x+b.y
		faddp st(1), st			;	a.x+b.y+c.z-d

	; Check result of plane equation test and clear face visible flag
	; (may be set later)
	;
		fstp result

		mov	[edi].flag, 0

		mov	dl, byte ptr result + 3

		and	dl, dl
		jl	behind

	; Write visible flag for face and increment use counters
	;
		mov	[edi].flag, TFF_VISIBLE

		mov	ax, [ebp+0]
		inc	[ebx+eax]

		mov	ax, [ebp+2]
		inc	[ebx+eax]

		mov	ax, [ebp+4]
		inc	[ebx+eax]

behind:

	; Advance all pointers and loop for next face
	;
		add esi, sizeof br_vector4
		add	edi, sizeof temp_face
		add ebp, 3 * sizeof word

		dec	ecx
		jnz	next

done:

		ret

V1Face_CullOneSidedPerspective_P6_A endp

endif

.data

if BASED_FLOAT

eqn		dword	?
count	dword	?
result	dword	?

endif

end


