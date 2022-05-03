.686p
.model	flat,c

include	drv.inc
include pro_epi.inc
;include klamath.inc


.code

M00 = 0
M01 = 4
M02 = 8
M03 = 12
M10 = 16
M11 = 20
M12 = 24
M13 = 28
M20 = 32
M21 = 36
M22 = 40
M23 = 44
M30 = 48
M31 = 52
M32 = 56
M33 = 60

VX = 0
VY = 4
VZ = 8
VW = 12

;Macro to transform a single vertex.
;Assumes dest,src,mat have been setup. 

TRANSFORM_VERTEX macro dest,src,mat ;Should come to 44 cycles on a pentium
	assume dest:ptr dword,src:ptr dword,mat:ptr dword

									;	0			1			2			3			4			5			6			7
	fld	[mat+M00]					;	M00
	fmul [src+VX]					;	XM00
	fld [mat+M10]					;	M10			XM00
	fmul [src+VY]					;	YM10		XM00
	 fxch st(1)						;   XM00		YM10
	fadd [mat+M30]					;	XM00+M30	YM10
	fld [mat+M20]					;	M20			XM00+M30	YM10
	fmul [src+VZ]					;	ZM20		XM00+M30	YM10
	 fxch st(2)						;	YM10		XM00+M30	ZM20
	faddp st(1),st					;	XYM013		ZM20
	fld [mat+M01]					;	M01			XYM013		ZM20
	fmul [src+VX]					;	XM01		XYM013		ZM20
	 fxch st(2)						;	ZM20		XYM013		XM01
	faddp st(1),st					;	XYZM0123	XM01
	fld [mat+M11]					;	M11			XYZM0123	XM01
	fmul [src+VY]					;	YM11		XYZM0123	XM01
	 fxch st(2)						;	XM01		XYZM0123	YM11
	fadd [mat+M31]					;	XM01+M31	XYZM0123	YM11
	fld [mat+M21]					;	M21			XM01+M31	XYZM0123	YM11
	fmul [src+VZ]					;	ZM21		XM01+M31	XYZM0123	YM11
	 fxch st(3)						;	YM11		XM01+M31	XYZM0123	ZM21
	faddp st(1),st					;	YXM013		XYZM0123	ZM21
	 fxch st(1)						;	XYZM0123	YXM013		ZM21
	fstp [dest+4*C_X]				;	YXM013		ZM21
	faddp st(1),st					;	1YXZM0123
	fld [mat+M02]					;	M02			1YXZM0123
	fmul [src+VX]					;	XM02		1YXZM0123
	fld [mat+M12]					;	M12			XM02		1YXZM0123
	fmul [src+VY]					;	YM12		XM02		1YXZM0123
	 fxch st(1)						;	XM02		YM12		1YXZM0123
	fadd [mat+M32]					;	XM02+M32	YM12		1YXZM0123
	fld [mat+M22]					;	M22			XM02+M32	YM12		1YXZM0123
	fmul [src+VZ]					;	ZM22		XM02+M32	YM12		1YXZM0123
	 fxch st(2)						;	YM12		XM02+M32	ZM22		1YXZM0123
	faddp st(1),st					;	XYM013		ZM22		1YXZM0123
	fld [mat+M03]					;	M03			XYM013		ZM22		1YXZM0123
	fmul [src+VX]					;	XM03		XYM013		ZM22		1YXZM0123
	 fxch st(2)						;	ZM22		XYM013		XM03		1YXZM0123
	faddp st(1),st					;	2XYZM0123	XM03		1YXZM0123
	fld [mat+M13]					;	M13			2XYZM0123	XM03		1YXZM0123
	fmul [src+VY]					;	YM13		2XYZM0123	XM03		1YXZM0123
	 fxch st(2)						;	XM03		2XYZM0123	YM13		1YXZM0123
	fadd [mat+M33]					;	XM03+M33	2XYZM0123	YM13		1YXZM0123
	fld [mat+M23]					;	M23			XM03+M33	2XYZM0123	YM13		1YXZM0123
	fmul [src+VZ]					;	ZM23		XM03+M33	2XYZM0123	YM13		1YXZM0123
	 fxch st(3)						;	YM13		XM03+M33	2XYZM0123	ZM23		1YXZM0123
	faddp st(1),st					;	XYM013		2XYZM0123	ZM23		1YXZM0123
	 fxch st(1)						;	2XYZM0123	XYM013		ZM23		1YXZM0123
	fstp [dest+4*C_Z]				;	XYM013		ZM23		1YXZM0123
	faddp st(1),st					;	3XYZM0123	1YXZM0123
	 fxch st(1)						;	1YXZM0123	3XYZM0123
	fstp [dest+4*C_Y]				;	3XYZM0123
	fstp [dest+4*C_W]				;
endm

PROJECT_VERTEX macro dest,src,renderer
	assume dest:ptr dword,src:ptr dword,renderer:ptr br_renderer
									;	0			1			2			3			4			5			6			7
	fld fp_one						;	1
	fdiv [src+4*C_W]				;	1/W
; write out q
	fst	[dest+4*C_Q]
;grow_old wait 32|38 cycles
; offsets and scales=20 cycles
	fld st							;	1/W			1/W
	fmul [src+4*C_X]				;	X/W			1/W
	fld st(1)						;	1/W			X/W			1/W
	fmul [src+4*C_Y]				;	Y/W			X/W			1/W
	 fxch st(1)						;	X/W			Y/W			1/W
	;stall
	fmul [renderer]._state.cache.comp_scales[4*C_SX]
									;	SX/W		Y/W			1/W
	 fxch st(1)						;	Y/W			SX/W		1/W
	;stall
	fmul [renderer]._state.cache.comp_scales[4*C_SY]
									;	SY/W		SX/W		1/W
	 fxch st(1)						;	SX/W		SY/W		1/W
	fadd [renderer]._state.cache.comp_offsets[4*C_SX]
									;	SX/W+O		SY/W		1/W
	 fxch st(2)						;	1/W			SY/W		SX/W+O
	fmul [src+4*C_Z]				;	Z/W			SY/W		SX/W+O
	 fxch st(1)						;	SY/W		Z/W			SX/W+O
	fadd [renderer]._state.cache.comp_offsets[4*C_SY]
									;	SY/W+O		Z/W			SX/W+O
	 fxch st(1)						;	Z/W			SY/W+O		SX/W+O
	fmul [renderer]._state.cache.comp_scales[4*C_SZ]
									;	SZ/W		SY/W+O		SX/W+O
	 fxch st(1)						;	SY/W+O		SZ/W		SX/W+O
	;stall
	fstp [dest+4*C_SY]				;	SZ/W		SX/W+O
	fadd [renderer]._state.cache.comp_offsets[4*C_SZ]
									;	SZ/W+O		SX/W+O
	 fxch st(1)						;	SX/W+O		SZ/W+O
	fstp [dest+4*C_SX]				;	SZ/W+O
	fstp [dest+4*C_SZ]				;
endm

OUTCODE_VERTEX macro outcode,screen,scratch,scratch2
assume screen:ptr dword
									;	0			1			2			3			4			5			6			7
	xor scratch,scratch
	lea scratch2,outcodeTableRTH
	fld [screen+4*C_W]				;	w

	fld [screen+4*C_X]				;	x			w
	fucomi st,st(1)					;	x			w
	rcl scratch,1

	fld [screen+4*C_Y]				;	y			x			w
	fucomi st,st(2)					;	y			x			w
	rcl scratch,1

	fld [screen+4*C_Z]				;	z			y			x			w
	fucomi st,st(3)					;	z			y			x			w
	 fxch st(3)						;	w			y			x			z
	rcl scratch,1

	fchs							;	-w			y			x			z
     fxch st(2)						;	x			y			-w			z

	mov scratch,[scratch2+4*scratch]
	lea scratch2,outcodeTableLBY
	xor outcode,scratch
	xor scratch,scratch

	fucomip st,st(2)				;	y			-w			z
	rcl scratch,1

	fucomip st,st(1)				;	-w			z
	 fxch st(1)						;	-w			z
	rcl scratch,1

	fucomip st,st(1)				;	-w
	rcl scratch,1
	
	fstp st(0)
	mov scratch,[scratch2+4*scratch]
	xor outcode,scratch

endm

ABS_MASK = 07fffffffh
SIGN_MASK = 080000000h

EXTRA_DODGY_OUTCODE_ORDINATE macro outcode,screen,scratch0,scratch1,scratch2,scratch3,ORDINATE
	mov scratch0,[screen+4*C_&ORDINATE]
	mov scratch1,absW

	and scratch0,ABS_MASK
	mov scratch2,[screen+4*C_&ORDINATE]

	and scratch2,SIGN_MASK
	mov scratch3,signW

	shr scratch2,28
	cmp scratch0,scratch1

	rcl scratch3,1
	cmp scratch1,scratch0

	rcl scratch3,1
	;stall

	or scratch3,scratch2
	lea scratch2,table&ORDINATE

	;stall
	;stall

	mov scratch3,[scratch2+4*scratch3]
	;stall

	xor outcode,scratch3
	;stall
endm

EXTRA_DODGY_OUTCODE_POINT macro outcode,screen,scratch0,scratch1,scratch2,scratch3
; outcode is register for outcoding into 
; screen is a pointer to a brp vertex structure
	mov scratch2,[screen+4*C_W]
	mov outcode,[screen] ; vertex flags
		
	mov scratch3,[screen+4*C_W]
	and scratch2,ABS_MASK

	and scratch3,SIGN_MASK
	mov absW,scratch2

	rol scratch3,1
	mov scratch0,[screen+4*C_X]

	mov signW,scratch3
	mov scratch1,absW

	and scratch0,ABS_MASK
	mov scratch2,[screen+4*C_X]

	and scratch2,SIGN_MASK
	mov scratch3,signW

	shr scratch2,28
	cmp scratch0,scratch1

	rcl scratch3,1
	cmp scratch1,scratch0

	rcl scratch3,1
	mov scratch0,[screen+4*C_Y]

	or scratch3,scratch2
	lea scratch2,tableX

	mov scratch1,absW
	and scratch0,ABS_MASK

	mov scratch3,[scratch2+4*scratch3]
	mov scratch2,[screen+4*C_Y]

	xor outcode,scratch3
	;stall

;	EXTRA_DODGY_OUTCODE_ORDINATE outcode,screen,scratch0,scratch1,scratch2,scratch3,Y

	and scratch2,SIGN_MASK
	mov scratch3,signW

	shr scratch2,28
	cmp scratch0,scratch1

	rcl scratch3,1
	cmp scratch1,scratch0

	rcl scratch3,1
	mov scratch0,[screen+4*C_Z]

	or scratch3,scratch2
	lea scratch2,tableY

	mov scratch1,absW
	and scratch0,ABS_MASK

	mov scratch3,[scratch2+4*scratch3]
	mov scratch2,[screen+4*C_Z]

	xor outcode,scratch3
	;stall

;	EXTRA_DODGY_OUTCODE_ORDINATE outcode,screen,scratch0,scratch1,scratch2,scratch3,Z

	and scratch2,SIGN_MASK
	mov scratch3,signW

	shr scratch2,28
	cmp scratch0,scratch1

	rcl scratch3,1
	cmp scratch1,scratch0

	rcl scratch3,1
	;stall

	or scratch3,scratch2
	lea scratch2,tableZ

	;stall
	;stall

	mov scratch3,[scratch2+4*scratch3]
	;stall

	xor outcode,scratch3
	;stall

	mov [screen],outcode
endm


EXTRA_DODGY_PROJECT_VERTEX macro dest,src,renderer,outcode,scratch0,scratch1,scratch2,scratch3
; starts projection

	assume dest:ptr dword,src:ptr dword,renderer:ptr br_renderer
									;	0			1			2			3			4			5			6			7
	fld fp_one						;	1
	fdiv [src+4*C_W]				;	1/W

	EXTRA_DODGY_OUTCODE_POINT outcode,src,scratch0,scratch1,scratch2,scratch3

	test outcode,OUTCODES_ALL
	jne skipProjection
; write out q
	fst	[dest+4*C_Q]
; offsets and scales=20 cycles
	fld st							;	1/W			1/W
	fmul [src+4*C_X]				;	X/W			1/W
	fld st(1)						;	1/W			X/W			1/W
	fmul [src+4*C_Y]				;	Y/W			X/W			1/W
	 fxch st(1)						;	X/W			Y/W			1/W
	;stall
	fmul [renderer]._state.cache.comp_scales[4*C_SX]
									;	SX/W		Y/W			1/W
	 fxch st(1)						;	Y/W			SX/W		1/W
	;stall
	fmul [renderer]._state.cache.comp_scales[4*C_SY]
									;	SY/W		SX/W		1/W
	 fxch st(1)						;	SX/W		SY/W		1/W
	fadd [renderer]._state.cache.comp_offsets[4*C_SX]
									;	SX/W+O		SY/W		1/W
	 fxch st(2)						;	1/W			SY/W		SX/W+O
	fmul [src+4*C_Z]				;	Z/W			SY/W		SX/W+O
	 fxch st(1)						;	SY/W		Z/W			SX/W+O
	fadd [renderer]._state.cache.comp_offsets[4*C_SY]
									;	SY/W+O		Z/W			SX/W+O
	 fxch st(1)						;	Z/W			SY/W+O		SX/W+O
	fmul [renderer]._state.cache.comp_scales[4*C_SZ]
									;	SZ/W		SY/W+O		SX/W+O
	 fxch st(1)						;	SY/W+O		SZ/W		SX/W+O
	;stall
	fstp [dest+4*C_SY]				;	SZ/W		SX/W+O
	fadd [renderer]._state.cache.comp_offsets[4*C_SZ]
									;	SZ/W+O		SX/W+O
	 fxch st(1)						;	SX/W+O		SZ/W+O
	fstp [dest+4*C_SX]				;	SZ/W+O
	fstp [dest+4*C_SZ]				;
endm




UPDATE_BOUNDS macro
endm

;static void GEOMETRY_CALL Vertex_OS_TransformProject(struct br_geometry *self, struct br_renderer *renderer)
;{
;	int v;
;	brp_vertex *tvp = rend.temp_vertices;
;
;	for(v=0; v < rend.nvertices; v++, tvp++) {
;
;		if(rend.vertex_counts[v] == 0)
;			continue;
;
;		TRANSFORM_VERTEX((br_vector4 *)(tvp->comp+C_X),&rend.vertex_p[v], &scache.model_to_screen);
;		PROJECT_VERTEX(tvp,tvp->comp[C_X],tvp->comp[C_Y],tvp->comp[C_Z],tvp->comp[C_W]);
;	}
;}

if BASED_FLOAT

Vertex_OS_TransformProject_A proc self:ptr br_geometry, renderer:ptr br_renderer
	mov ecx,rend.nvertices
	mov edi,rend.temp_vertices

	mov esi,rend.vertex_p
	mov eax,rend.vertex_counts

	mov edx,renderer
	lea ebp,scache.model_to_screen

forVertices:
	dec ecx
	js done
	
	mov bl,[eax]

	test bl,bl
	jz next

	TRANSFORM_VERTEX edi,esi,ebp
	PROJECT_VERTEX edi,edi,edx

next:
	add esi,sizeof br_vector3
	inc eax

	add edi,sizeof brp_vertex
	jmp forVertices
done:
	ret
Vertex_OS_TransformProject_A endp


;static void GEOMETRY_CALL Vertex_TransformProjectOutcode(struct br_geometry *self, struct br_renderer *renderer)
;{
;	int v;
;	int temp0,temp1;
;	brp_vertex *tvp = rend.temp_vertices;
;
;	for(v=0; v < rend.nvertices; v++, tvp++) {
;
;		if(rend.vertex_counts[v] == 0)
;			continue;
;
;		TRANSFORM_VERTEX((br_vector4 *)(tvp->comp+C_X),&rend.vertex_p[v], &scache.model_to_screen);
;		OUTCODE_POINT(tvp->flags, (br_vector4 *)(tvp->comp+C_X));
;		if(!(tvp->flags & OUTCODES_ALL)) {
;			PROJECT_VERTEX(tvp,tvp->comp[C_X],tvp->comp[C_Y],tvp->comp[C_Z],tvp->comp[C_W]);
;		}
;	}
;}


if 0 ; This code works, didn't come to a conclusion as to whether it was worthwhile.
Vertex_TransformProjectOutcode_CMOV proc self:ptr br_geometry, renderer:ptr br_renderer
	mov ecx,rend.nvertices
	mov edi,rend.temp_vertices

	mov eax,rend.vertex_counts
	mov edx,renderer

	mov esi,rend.vertex_p
	lea ebp,scache.model_to_screen

forVertices:
	dec ecx
	js done
	

	mov bl,[eax]
	test bl,bl

	mov ebx,[edi]
	jz next

	TRANSFORM_VERTEX edi,esi,ebp

	push esi
	push ebp

	OUTCODE_VERTEX ebx,edi,esi,ebp

	pop ebp 
	pop esi

	mov [edi],ebx

	test ebx,OUTCODES_ALL
	jne next

	PROJECT_VERTEX edi,edi,edx

next:
	add esi,sizeof br_vector3
	inc eax

	add edi,sizeof brp_vertex
	jmp forVertices
done:
	ret
Vertex_TransformProjectOutcode_CMOV endp

endif






Vertex_TransformProjectOutcode_A proc self:ptr br_geometry, renderer:ptr br_renderer
	mov ecx,rend.nvertices
	mov edi,rend.temp_vertices

	mov eax,rend.vertex_counts
	mov edx,renderer

	mov esi,rend.vertex_p
	lea ebp,scache.model_to_screen

forVertices:
	dec ecx
	js done
	

	mov bl,[eax]
	test bl,bl

	mov ebx,[edi]
	jz next

	TRANSFORM_VERTEX edi,esi,ebp

	push esi
	push eax
	
	push ebx
	push ecx

	push ebp
	
	EXTRA_DODGY_PROJECT_VERTEX edi,edi,edx,ebp,eax,ebx,ecx,esi
endProjection:
	pop ebp

	pop ecx
	pop ebx

	pop eax
	pop esi

next:
	add esi,sizeof br_vector3
	inc eax

	add edi,sizeof brp_vertex
	jmp forVertices
done:
	ret
skipProjection:
	fstp st
	jmp endProjection
Vertex_TransformProjectOutcode_A endp



;static void GEOMETRY_CALL Vertex_OS_TransformProjectSurf(struct br_geometry *self, struct br_renderer *renderer)
;{
;	int v,i;
;	brp_vertex *tvp = rend.temp_vertices;
;	br_vector3 *vp_p = rend.vertex_p;
;	br_vector2 *vp_map = rend.vertex_map;
;	br_vector3 *vp_n = rend.vertex_n;
;
;	for(v=0; v < rend.nvertices; v++, vp_p++, vp_map++, vp_n++, tvp++) {
;
;		if(rend.vertex_counts[v] == 0)
;			continue;
;
;		TRANSFORM_VERTEX((br_vector4 *)(tvp->comp+C_X),vp_p, &scache.model_to_screen);
;		PROJECT_VERTEX(tvp,tvp->comp[C_X],tvp->comp[C_Y],tvp->comp[C_Z],tvp->comp[C_W]);
;
;		for(i=0; i < renderer->state.cache.nvertex_fns; i++)
;			renderer->state.cache.vertex_fns[i](renderer, vp_p, vp_map, vp_n, scache.colour, tvp->comp);
;	}
;}

if 0 
zVertex_OS_TransformProjectSurf proc self:ptr br_geometry, renderer:ptr br_renderer
	mov ecx,rend.nvertices
	mov edi,rend.temp_vertices

	mov esi,rend.vertex_p
	mov eax,rend.vertex_counts

	mov edx,renderer
	lea ebp,scache.model_to_screen

forVertices:
	dec ecx
	js done
	
	mov bl,[eax]

	test bl,bl
	jz next

	TRANSFORM_VERTEX edi,esi,ebp
	PROJECT_VERTEX edi,edi,edx

	push ecx
	mov ecx,[edx]._state.cache.nvertex_fns

forRenderFns:
	dec ecx
	js renderFnsDone

	add esp,24

	jmp forRenderFns
renderFnsDone:
	pop ecx 
next:
	add esi,sizeof br_vector3
	inc eax

	add edi,sizeof brp_vertex
	jmp forVertices
done:
	ret
zVertex_OS_TransformProjectSurf endp
endif 
endif

.data

if BASED_FLOAT
fp_one dword 1.0

outcodeTableRTH dword OUTCODE_RIGHT or OUTCODE_N_RIGHT or OUTCODE_HITHER or OUTCODE_N_HITHER or OUTCODE_TOP or OUTCODE_N_TOP
	dword OUTCODE_RIGHT or OUTCODE_N_RIGHT or OUTCODE_TOP or OUTCODE_N_TOP
	dword OUTCODE_RIGHT or OUTCODE_N_RIGHT or OUTCODE_HITHER or OUTCODE_N_HITHER
	dword OUTCODE_RIGHT or OUTCODE_N_RIGHT
	dword OUTCODE_HITHER or OUTCODE_N_HITHER or OUTCODE_TOP or OUTCODE_N_TOP
	dword OUTCODE_TOP or OUTCODE_N_TOP
	dword OUTCODE_HITHER or OUTCODE_N_HITHER
	dword 0
outcodeTableLBY dword 0
	dword OUTCODE_YON or OUTCODE_N_YON
	dword OUTCODE_BOTTOM or OUTCODE_N_BOTTOM
	dword OUTCODE_YON or OUTCODE_N_YON or OUTCODE_BOTTOM or OUTCODE_N_BOTTOM
	dword OUTCODE_LEFT or OUTCODE_N_LEFT
	dword OUTCODE_LEFT or OUTCODE_N_LEFT or OUTCODE_YON or OUTCODE_N_YON
	dword OUTCODE_LEFT or OUTCODE_N_LEFT or OUTCODE_BOTTOM or OUTCODE_N_BOTTOM
	dword OUTCODE_LEFT or OUTCODE_N_LEFT or OUTCODE_YON or OUTCODE_N_YON or OUTCODE_BOTTOM or OUTCODE_N_BOTTOM

OUTCODE_TABLE macro name,gENameUCase,lNameUCase
table&name dword OUTCODE_&gENameUCase or OUTCODE_N_&gENameUCase
	dword OUTCODE_&gENameUCase or OUTCODE_N_&gENameUCase				
	dword 0

	dword 0

	dword OUTCODE_&gENameUCase or OUTCODE_N_&gENameUCase
	dword OUTCODE_&gENameUCase or OUTCODE_N_&gENameUCase
	dword OUTCODE_&gENameUCase or OUTCODE_N_&gENameUCase or OUTCODE_&lNameUCase or OUTCODE_N_&lNameUCase

	dword 0

	dword 0
	dword OUTCODE_&lNameUCase or OUTCODE_N_&lNameUCase
	dword 0

	dword 0

	dword OUTCODE_&gENameUCase or OUTCODE_N_&gENameUCase or OUTCODE_&lNameUCase or OUTCODE_N_&lNameUCase
	dword OUTCODE_&lNameUCase or OUTCODE_N_&lNameUCase
	dword OUTCODE_&gENameUCase or OUTCODE_N_&gENameUCase or OUTCODE_&lNameUCase or OUTCODE_N_&lNameUCase

	dword 0
endm

    OUTCODE_TABLE X,RIGHT,LEFT
    OUTCODE_TABLE Y,TOP,BOTTOM
    OUTCODE_TABLE Z,HITHER,YON

absW dword ?
signW dword ?

endif

end
