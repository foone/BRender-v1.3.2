.586p
.model	flat,c

include	drv.inc



PROLOGUE MACRO procname, flags, argbytes, localbytes, reglist, userparms:VARARG
	push ebp
	
	mov ebp,esp
	push ebx

	push edi
	push esi
	exitm <0>
endm



EPILOGUE MACRO procname, flags, argbytes, localbytes, reglist, userparms:VARARG
	pop	esi													   
	pop	edi

	pop	ebx
	pop	ebp

	ret
endm



OPTION PROLOGUE:PROLOGUE 
OPTION EPILOGUE:EPILOGUE

.code



COMPUTE_COMPONENT_MID_POINT_VALUES macro C_P
assume eax:ptr dword, ebx:ptr dword, ecx:ptr dword, edx:ptr dword, edi:ptr dword, esi:ptr dword
; compiler = 27 cycles per component
;15 cycles per component
									;	0			1			2			3			4			5			6			7
	fld	[edx+4*C_P]					;	p0
	fadd [edi+4*C_P]				;	m2*2
	fld	[edi+4*C_P]					;	p1			m2*2
	fadd [esi+4*C_P]				;	m0*2		m2*2
	 fxch st(1)						;	m2*2		m0*2
	fmul fp_half					;	m2			m0*2
	fld [edx+4*C_P]					;	p0			m2			m0*2
	 fxch st(2)						;	m0*2		m2			p0
	fmul fp_half					;	m0			m2			p0
	 fxch st(2)						;	p0			m2			m0
	fadd [esi+4*C_P]				;	m1*2		m2			m0
	 fxch st(1)						;	m2			m1*2		m0
	fstp [ecx+4*C_P]				;	m1*2		m0
	fmul fp_half					;	m1			m0
	 fxch st(1)						;	m0			m1
	fstp [eax+4*C_P]				;	m1
	fstp [ebx+4*C_P]				;
endm



PROJECT_COMPONENT macro C_P,C_SP
assume ebp:ptr br_renderer
;21 cycles
	fld [ebp]._state.cache.comp_scales[4*C_SP]		;	s			1/w1		1/w0		1/w2
	fmul [eax+4*C_P]								;   sc0			1/w1		1/w0		1/w2
	fld [ebp]._state.cache.comp_scales[4*C_SP]		;	s			sc0			1/w1		1/w0		1/w2
	fmul [ebx+4*C_P]								;	sc1			sc0			1/w1		1/w0		1/w2
	fld [ebp]._state.cache.comp_scales[4*C_SP]		;   s			sc1			sc0			1/w1		1/w0		1/w2
	 fxch st(2)										;   sc0			sc1			s			1/w1		1/w0		1/w2
	fmul st,st(4)									;	sc0/w0		sc1			s			1/w1		1/w0		1/w2
	 fxch st(1)										;	sc1			sc0/w0		s			1/w1		1/w0		1/w2
;stall
	fmul st,st(3)									;	sc1/w1		sc0/w0		s			1/w1		1/w0		1/w2
	 fxch st(1)										;	sc0/w0		sc1/w1		s			1/w1		1/w0		1/w2
	fadd [ebp]._state.cache.comp_offsets[4*C_SP]	;	p0			sc1/w1		s			1/w1		1/w0		1/w2
	 fxch st(2)										;	s			sc1/w1		p0			1/w1		1/w0		1/w2
	fmul [ecx+4*C_P]								;	sc2			sc1/w1		p0			1/w1		1/w0		1/w2
	 fxch st(1)										;	sc1/w1		sc2			p0			1/w1		1/w0		1/w2
	fadd [ebp]._state.cache.comp_offsets[4*C_SP]	;	p1			sc2			p0			1/w1		1/w0		1/w2
	 fxch st(2)										;	p0			sc2			p1			1/w1		1/w0		1/w2
	fstp [eax+4*C_SP]								;	sc2			p1			1/w1		1/w0		1/w2
	fmul st,st(4)									;	sc2/w2		p1			1/w1		1/w0		1/w2
	 fxch st(1)										;	p1			sc2/w2		1/w1		1/w0		1/w2
	fstp [ebx+4*C_SP]								;	sc2/w2		1/w1		1/w0		1/w2
	fadd [ebp]._state.cache.comp_offsets[4*C_SP]	;	p2			1/w1		1/w0		1/w2
;stall
;stall
	fstp [ecx+4*C_SP]								;	1/w1		1/w0		1/w2
endm



TEST_AND_PROJECT_VERTEX macro reg					;	0			1			2			3			4			5			6			7
	local done

	mov edx,[reg]
	test edx,OUTCODES_ALL
	jnz done
	fld fp_one										;	1
	fdiv [reg+4*C_W]								;	1/w
; wait a long time
	fst	[reg+4*C_Q]									;	1/w
	fld [reg+4*C_X]									;	x			1/w
	fmul st,st(1)									;	x/w			1/w
	fld [reg+4*C_Y]									;	y			x/w			1/w
	fmul st,st(2)									;	y/w			x/w			1/w
	 fxch st(2)										;	1/w			x/w			y/w
;stall
	fmul [reg+4*C_Z]								;	z/w			x/w			y/w
	 fxch st(1)										;	x/w			z/w			y/w
;stall
	fmul [ebp]._state.cache.comp_scales[4*C_SX]		;	sx/w		z/w			y/w
	 fxch st(2)										;	y/w			z/w			sx/w
;stall
	fmul [ebp]._state.cache.comp_scales[4*C_SY]		;	sy/w		z/w			sx/w
	 fxch st(2)										;	sx/w		z/w			sy/w
	fadd [ebp]._state.cache.comp_offsets[4*C_SX]	;	SX			z/w			sy/w
	 fxch st(1)										;	z/w			SX			sy/w
	fmul [ebp]._state.cache.comp_scales[4*C_SZ]		;	sz/w		SX			sy/w
	 fxch st(2)										;	sy/w		SX			sz/w
	fadd [ebp]._state.cache.comp_offsets[4*C_SY]	;	SY			SX			sz/w
	 fxch st(1)										;	SX			SY			sz/w
	fstp [reg+4*C_SX]								;	SY			sz/w
	fstp [reg+4*C_SY]								;	sz/w
	fadd [ebp]._state.cache.comp_offsets[4*C_SZ]	;   SZ
;stall
;stall
	fstp [reg+4*C_SZ]
done:
endm 



OUTCODE_ORDINATE macro lValue,rValue,tableName1,reg0,reg1
;assumes edx contains outcode flags,esi target for discriptor edi,reg0,reg1
;14 cycles - (at least 2.5 wasted cycles)
	mov reg0,rValue
	mov esi,080000000h

	and esi,reg0
	mov reg1,lValue

	mov edi,080000000h
	and reg0,07fffffffh

	shr esi,1
	and edi,reg1
	
	or esi,edi
	mov edi,reg0

	and reg1,07fffffffh
	;stall

	sub edi,reg1
	sub reg1,reg0

	shr edi,3
	and reg1,080000000h

	shr reg1,2
	or esi,edi

	or esi,reg1
	lea reg0,tableName1

	shr esi,28
	;stall

	;stall
	;stall

	mov reg0,[reg0+4*esi]
	;stall

	xor edx,reg0
endm



DOT_PRODUCT_4 macro reg0,reg1,dest
assume reg0:ptr dword
assume reg1:ptr dword
;18 cycles
							;	0			1			2			3			4			5			6			7
	fld [reg0]				;	u0
	fmul [reg1]				;	u0*v0
	fld [reg0+4]			;	u1			u0*v0
	fmul [reg1+4]			;	u1*v1		u0*v0
	fld [reg0+8]			;	u2			u1*v1		u0*v0
	fmul [reg1+8]			;	u2*v2		u1*v1		u0*v0
	fld [reg0+12]			;	u3			u2*v2		u1*v1		u0*v0
	fmul [reg1+12]			;	u3*v3		u2*v2		u1*v1		u0*v0
	 fxch st(3)				;	u0*v0		u2*v2		u1*v1		u3*v3
	faddp st(2),st			;	u2*v2		p0+p1		u3*v3
	;stall					;
	faddp st(2),st			;	p0+p1		p2+p3
	;stall
	;stall
	faddp st(1),st			;	prod
	;stall
	;stall
	fstp dest				;
endm

ifdef BASED_FLOAT
if BASED_FLOAT
averageVerticesOnScreen proc renderer:dword, m0:dword, m1:dword, m2:dword, v0:dword, v1:dword, v2:dword
	mov edi,v1
	mov esi,v2

	mov ecx,m2
	mov edx,v0

	mov eax,m0
	mov ebx,m1

	COMPUTE_COMPONENT_MID_POINT_VALUES C_X
	COMPUTE_COMPONENT_MID_POINT_VALUES C_Y
	COMPUTE_COMPONENT_MID_POINT_VALUES C_Z
	COMPUTE_COMPONENT_MID_POINT_VALUES C_W

;get the projection divide going
;41 cycles
									;	0			1			2			3			4			5			6			7
	fld [eax+4*C_W]					;	w0
	fmul [ebx+4*C_W]				;	w1w0
	fld [ebx+4*C_W]					;	w1			w1w0
	fmul [ecx+4*C_W]				;   w1w2		w1w0
	fld st(1)						;	w1w0		w1w2		w1w0
	fmul [ecx+4*C_W]				;	w2w1w0		w1w2		w1w0
	fld [eax+4*C_W]					;	w0			w2w1w0		w1w2		w1w0
	fmul [ecx+4*C_W]				;	w2w0		w2w1w0		w1w2		w1w0
	 fxch st(1)						;	w2w1w0		w2w0		w1w2		w1w0
	fdivr fp_one					;	1/w2w1w0	w2w0		w1w2		w1w0

;perform a check to see if we can avoid doing any work.
;doesn't need to be fast as its hidden by the divide
	push ebp
	mov ebp,rend.block
assume ebp:ptr brp_block 
	mov ebp,[ebp].convert_mask_f
	and ebp,CM_R or CM_G or CM_B
	pop ebp
	mov ebp,renderer
	jz commonComponents

	COMPUTE_COMPONENT_MID_POINT_VALUES C_R
	COMPUTE_COMPONENT_MID_POINT_VALUES C_G
	COMPUTE_COMPONENT_MID_POINT_VALUES C_B

commonComponents:

	COMPUTE_COMPONENT_MID_POINT_VALUES C_I
	COMPUTE_COMPONENT_MID_POINT_VALUES C_U
	COMPUTE_COMPONENT_MID_POINT_VALUES C_V

;complete projection
													;	0			1			2			3			4			5			6			7
;5 cycles
	fmul st(2),st									;	1/w2w1w0	w2w0		1/w0		w1w0
;stall												
	fmul st(1),st									;	1/w2w1w0	1/w1		1/w0		w1w0
;stall												
	fmulp st(3),st									;	1/w1		1/w0		1/w2

	PROJECT_COMPONENT C_X,C_SX
	PROJECT_COMPONENT C_Y,C_SY
	PROJECT_COMPONENT C_Z,C_SZ


;6 cycles
	fstp [ebx+4*C_Q]
	fstp [eax+4*C_Q]
	fstp [ecx+4*C_Q]

	ret
averageVerticesOnScreen endp
endif
endif

subdivideCheck proc uses ebx ecx edx,
	v0 : ptr brp_vertex,
	v1 : ptr brp_vertex,
	v2 : ptr brp_vertex

		assume eax: ptr brp_vertex, ebx: ptr brp_vertex, ecx: ptr brp_vertex

	if BASED_FIXED

	; Fetch the absolute values of the homogenous Zs
	;
	; 6-9 cycles + 0-12 cycles branch misprediction
	;
		mov		eax,v0
		mov		ebx,v1

		mov		ecx,v2
		mov		eax,[eax].comp_f[C_Z*4]

		mov		ebx,[ebx].comp_f[C_Z*4]
		mov		ecx,[ecx].comp_f[C_Z*4]

		test	eax,eax
		jns		v0_zpos
		
		neg		eax
v0_zpos:

		test	ebx,ebx
		jns		v1_zpos
		
		neg		ebx
v1_zpos:

		test	ecx,ecx
		jns		v2_zpos
		
		neg		ecx
v2_zpos:

	; get the order
	;
	; 4
	;
		xor		edx,edx
		cmp		ebx,eax

		rcl		edx,1
		cmp		ecx,eax

		rcl		edx,1
		cmp		ecx,ebx

		rcl		edx,1

    ; If the ratio of the smallest and largest Z is greater
    ; than a threshold, then subdivide
    ;
	; 17 + 0/4
	;
		fild	rend.subdivide_threshold	;	t

		mov		eax,sort_table_2[edx*4]
		mov		ebx,sort_table_0[edx*4]

		mov		eax,[v0+eax]
		mov		ebx,[v0+ebx]

		fild	[eax].comp_f[C_Z*4]			;	zmax	t
		fild	[ebx].comp_f[C_Z*4]			;	zmin	zmax	t
		fxch	st(1)						;	zmax	zmin	t
		fmulp	st(2),st(0)					;	zmin	zmax*t

		fmul	fp_one_fixed				;	zmin*1	zmax*t
		 fxch	st(1)						;	zmax*t	zmin*1

		fstp	temp
		fstp	temp2

		mov		eax,temp
		mov		ebx,temp2

		and		eax,07fffffffh
		and		ebx,07fffffffh

		cmp		eax,ebx
		jge		subdivide

		mov		eax,BR_FALSE
		ret

subdivide:	
		mov		eax,BR_TRUE
		ret

	endif	; BASED_FIXED

	if BASED_FLOAT

	; Sort the vertices in order of absolute value of homogeneous Z values
	;
		mov		eax,v0
		mov		ebx,v1

		mov		ecx,v2
		mov		eax,[eax].comp_f[C_Z*4]

		mov		ebx,[ebx].comp_f[C_Z*4]
		mov		ecx,[ecx].comp_f[C_Z*4]

		and		eax,07fffffffh
		and		ebx,07fffffffh

		and		ecx,07fffffffh

		xor		edx,edx
		cmp		ebx,eax

		rcl		edx,1
		cmp		ecx,eax

		rcl		edx,1
		cmp		ecx,ebx

		rcl		edx,1

    ; If the ratio of the smallest and largest Z is greater
    ; than a threshold, then subdivide
    ;
		fld		rend.subdivide_threshold

		mov		eax,sort_table_2[edx*4]
		mov		ebx,sort_table_0[edx*4]

		mov		eax,[v0+eax]
		mov		ebx,[v0+ebx]

		fmul	[eax].comp_f[C_Z*4]

		fstp	temp

		mov		eax,temp
		mov		ebx,[ebx].comp_f[C_Z*4]

		and		eax,07fffffffh
		and		ebx,07fffffffh

		cmp		eax,ebx
		jge		subdivide

		mov		eax,BR_FALSE
		ret

subdivide:	
		mov		eax,BR_TRUE
		ret

	endif	; BASED_FLOAT

subdivideCheck endp


ifdef BASED_FLOAT
if BASED_FLOAT
averageVertices proc renderer:dword, m0:dword, m1:dword, m2:dword, v0:dword, v1:dword, v2:dword
; There are a few probs with this code
; 1) Untested with user defined clip planes.

	mov edi,v1
	mov esi,v2

	mov ecx,m2
	mov edx,v0

	mov eax,m0
	mov ebx,m1

;perform averaging.
	COMPUTE_COMPONENT_MID_POINT_VALUES C_X
	COMPUTE_COMPONENT_MID_POINT_VALUES C_Y
	COMPUTE_COMPONENT_MID_POINT_VALUES C_Z
	COMPUTE_COMPONENT_MID_POINT_VALUES C_W
	COMPUTE_COMPONENT_MID_POINT_VALUES C_R
	COMPUTE_COMPONENT_MID_POINT_VALUES C_G
	COMPUTE_COMPONENT_MID_POINT_VALUES C_B
	COMPUTE_COMPONENT_MID_POINT_VALUES C_I
	COMPUTE_COMPONENT_MID_POINT_VALUES C_U
	COMPUTE_COMPONENT_MID_POINT_VALUES C_V

;outcoding 
	mov edx,OUTCODES_NOT
	mov eax,m0
	OUTCODE_ORDINATE [eax+4*C_X],[eax+4*C_W],rightLeftTable,ebx,ecx
	OUTCODE_ORDINATE [eax+4*C_Y],[eax+4*C_W],topBottomTable,ebx,ecx
	OUTCODE_ORDINATE [eax+4*C_Z],[eax+4*C_W],hitherYonTable,ebx,ecx
	mov [eax],edx

	mov edx,OUTCODES_NOT
	mov eax,m1
	OUTCODE_ORDINATE [eax+4*C_X],[eax+4*C_W],rightLeftTable,ebx,ecx
	OUTCODE_ORDINATE [eax+4*C_Y],[eax+4*C_W],topBottomTable,ebx,ecx
	OUTCODE_ORDINATE [eax+4*C_Z],[eax+4*C_W],hitherYonTable,ebx,ecx
	mov [eax],edx

	mov edx,OUTCODES_NOT
	mov eax,m2
	OUTCODE_ORDINATE [eax+4*C_X],[eax+4*C_W],rightLeftTable,ebx,ecx
	OUTCODE_ORDINATE [eax+4*C_Y],[eax+4*C_W],topBottomTable,ebx,ecx
	OUTCODE_ORDINATE [eax+4*C_Z],[eax+4*C_W],hitherYonTable,ebx,ecx
	mov [eax],edx
	push ebp

	mov ecx,m2
	mov eax,m0

	mov ebx,m1
	mov ebp,renderer

; perform clip plane outcoding if neccessary.
	mov edx,scache.user_clip_active
	mov edi,MAX_STATE_CLIP_PLANES-1

	test edx,edx
	jz clipDone
assume ebp:ptr br_renderer
clipPlane:
	lea ebp,[ebp]._state.clip+MAX_STATE_CLIP_PLANES*sizeof(state_clip)
assume ebp:ptr state_clip
	cmp esi,BRT_PLANE
	jne clipNext
	lea edx,[ebp].plane
	lea esi,[eax+4*C_X]
	DOT_PRODUCT_4 esi,edx,dotProduct
	mov edx,dotProduct
	mov esi,[eax]
	test edx,080000000h
	jz clip1
	mov edx,OUTCODE_USER or OUTCODE_N_USER
	push ecx
	mov ecx,edi
	shl edx,cl
	pop ecx
	xor esi,edx
	mov [eax],esi
clip1:
	lea esi,[ebx+4*C_X]
	DOT_PRODUCT_4 esi,edx,dotProduct
	mov edx,dotProduct
	mov esi,[ebx]
	test edx,080000000h
	jz clip1
	mov edx,OUTCODE_USER or OUTCODE_N_USER
	push ecx
	mov ecx,edi
	shl edx,cl
	pop ecx
	xor esi,edx
	mov [ebx],esi
clip2:
	lea esi,[ecx+4*C_X]
	DOT_PRODUCT_4 esi,edx,dotProduct
	mov edx,dotProduct
	mov esi,[ecx]
	test edx,080000000h
	jz clip1
	mov edx,OUTCODE_USER or OUTCODE_N_USER
	push ecx
	mov ecx,edi
	shl edx,cl
	pop ecx
	xor esi,edx
	mov [ecx],esi
clipNext:
	sub ebp,sizeof(state_clip)
	dec edi
	jge clipPlane
clipDone:
;project if neccesary
	mov edx,[eax]
	pop ebp

	mov edi,[ebx]
	mov esi,[ecx]

	mov ebp,renderer

	test edx,OUTCODES_ALL
	jnz selectiveProjection

	test edi,OUTCODES_ALL
	jnz selectiveProjection

	test esi,OUTCODES_ALL
	jnz selectiveProjection
	
;41 cycles
									;	0			1			2			3			4			5			6			7
	fld [eax+4*C_W]					;	w0
	fmul [ebx+4*C_W]				;	w1w0
	fld [ebx+4*C_W]					;	w1			w1w0
	fmul [ecx+4*C_W]				;   w1w2		w1w0
	fld st(1)						;	w1w0		w1w2		w1w0
	fmul [ecx+4*C_W]				;	w2w1w0		w1w2		w1w0
	fld [eax+4*C_W]					;	w0			w2w1w0		w1w2		w1w0
	fmul [ecx+4*C_W]				;	w2w0		w2w1w0		w1w2		w1w0
	 fxch st(1)						;	w2w1w0		w2w0		w1w2		w1w0
	fdivr fp_one					;	1/w2w1w0	w2w0		w1w2		w1w0

;complete projection
									;	0			1			2			3			4			5			6			7
;5 cycles							                                                                                         
	fmul st(2),st					;	1/w2w1w0	w2w0		1/w0		w1w0                                             				
;stall								                                                                                         				
	fmul st(1),st					;	1/w2w1w0	1/w1		1/w0		w1w0                                             				
;stall								                                                                                         				
	fmulp st(3),st					;	1/w1		1/w0		1/w2                                                         				

	PROJECT_COMPONENT C_X,C_SX
	PROJECT_COMPONENT C_Y,C_SY
	PROJECT_COMPONENT C_Z,C_SZ

;6 cycles
	fstp [ebx+4*C_Q]
	fstp [eax+4*C_Q]
	fstp [ecx+4*C_Q]

	ret

selectiveProjection:
	TEST_AND_PROJECT_VERTEX eax
	TEST_AND_PROJECT_VERTEX ebx
	TEST_AND_PROJECT_VERTEX ecx
	ret
averageVertices endp
endif 
endif

.data

sort_table_1	dword	1*4
				dword	2*4
				dword	0*4
sort_table_0	dword	0*4
				dword	0*4
				dword	0*4
sort_table_2	dword	2*4
				dword	1*4
				dword	0*4
				dword	1*4
				dword	2*4
				dword	0*4
				dword	0*4
				dword	0*4

temp			dword	0
temp2			dword	0

fp_one			dword	1.0f
fp_half			dword	0.5f
fp_one_fixed	dword	65536.0

dotProduct dword ?

OUTCODE_TABLE macro name,gENameUCase,lNameUCase
name&Table	dword OUTCODE_&gENameUCase or OUTCODE_N_&gENameUCase
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

ifdef BASED_FLOAT
if BASED_FLOAT
        OUTCODE_TABLE rightLeft,RIGHT,LEFT
        OUTCODE_TABLE topBottom,TOP,BOTTOM
        OUTCODE_TABLE hitherYon,HITHER,YON
endif
endif


end
