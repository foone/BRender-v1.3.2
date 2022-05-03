.586p
.model	flat,c

include fpwork.inc
include	drv.inc
include fpsetup.inc
include common.inc

if PARTS and PART_8Z
        externdef c TriangleRasterise_ZTI_I8_D16_32:proc
	externdef c TriangleRasterise_ZTI_I8_D16_64:proc
	externdef c TriangleRasterise_ZTI_I8_D16_128:proc
	externdef c TriangleRasterise_ZTI_I8_D16_256:proc
	externdef c TriangleRasterise_ZTI_I8_D16_1024:proc

	externdef c TriangleRasterise_ZTI_I8_D16:proc
	externdef c TriangleRasterise_ZI_I8_D16:proc
	externdef c TriangleRasterise_Z_I8_D16:proc
endif 

if PARTS and PART_8
	externdef c TriangleRasterise_TI_I8_256:proc
	externdef c TriangleRasterise_TI_I8:proc
	externdef c TriangleRasterise_I_I8:proc
	externdef c TriangleRasterise_I8:proc
endif


.code


decal macro fnId,setupId,backId,frontId

TriangleRender&fnId proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

;										st(0)		st(1)		st(2)		st(3)		st(4)		st(5)		st(6)		st(7)
	fild work.decal_index_base
									;	base
	fild work.decal_index_range
									;	range		base
	fild work.decal_shade_height	
									;	shade		range		base
	mov	eax,v0
	mov	ecx,v1	

	mov	edx,v2
	mov workspace.v0,eax

	fmul fp_1_256
	fxch st(1)						;	range		shade		base

	mov workspace.v1,ecx
	mov workspace.v2,edx

	fmul fp_1_256

	mov ebp,[eax+4*C_I]
	mov esi,[ecx+4*C_I]

	mov edi,[edx+4*C_I]
	mov v0CI,ebp

	mov v1CI,esi
	mov v2CI,edi
	
        mov ebp,[eax+4*C_S]
        mov esi,[ecx+4*C_S]

        mov edi,[edx+4*C_S]
        mov v0CS,ebp

        mov v1CS,esi
        mov v2CS,edi
	
;										st(0)		st(1)		st(2)		st(3)		st(4)		st(5)		st(6)		st(7)
;										range		shade		base
	fld dword ptr [eax+4*C_I]		;	v0			range		shade		base
	fmul st,st(2)					;	v0A			range		shade		base
	fld dword ptr [ecx+4*C_I]		;	v1			v0A			range		shade		base	
	fmul st,st(3)					;	v1A			v0A			range		shade		base	
	fld dword ptr [edx+4*C_I]		;	v2			v1A			v0A			range		shade		base	
	fmul st,st(4)					;	v2A			v1A			v0A			range		shade		base	
	 fxch st(2)						;	v0A			v1A			v2A			range		shade		base	
        fstp dword ptr [eax+4*C_S]              ;       v1A                     v2A                     range           shade           base 
        fstp dword ptr [ecx+4*C_S]              ;       v2A                     range           shade           base 
        fstp dword ptr [edx+4*C_S]              ;       range           shade           base
	fld dword ptr [eax+4*C_I]		;	v0			range		shade		base             
	fmul st,st(1)					;	v0I			range		shade		base             
	fld dword ptr [ecx+4*C_I]		;	v1			v0I			range		shade		base
	fmul st,st(2)					;	v1I			v0I			range		shade		base
	fld dword ptr [edx+4*C_I]		;	v2			v1I			v0I			range		shade		base
	fmul st,st(3)					;	v2I			v1I			v0I			range		shade		base
	 fxch st(2)						;	v0I			v1I			v2I			range		shade		base
	fadd st,st(5)					;	v0I			v1I			v2I			range		shade		base
	 fxch st(1)						;	v1I			v0I			v2I			range		shade		base
	fadd st,st(5)					;	v1I			v0I			v2I			range		shade		base
	 fxch st(2)						;	v2I			v0I			v1I			range		shade		base
	fadd st,st(5)					;	v2I			v0I			v1I			range		shade		base
	 fxch st(1)						;	v0I			v2I			v1I			range		shade		base
	fstp dword ptr [eax+4*C_I]		;	v2I			v1I			range		shade		base
	fstp dword ptr [edx+4*C_I]		;	v1I			range		shade		base
	fstp dword ptr [ecx+4*C_I]		;	range		shade		base
	fstp st							;	shade		base
	fstp st							;	base
	fstp st							;	

	call TriangleSetup&setupId

	mov eax,workspace.topCount
	mov ebx,workspace.bottomCount

	mov ecx,workspace.s_u
	mov edx,workspace.s_v

	mov esi,workspace.d_z_x

	mov topCount,eax
	mov bottomCount,ebx

	mov s_u,ecx
	mov s_v,edx

	mov d_z_x,esi

	mov eax,workspace.s_z
	mov ebx,workspace.xm

	mov ecx,workspace.x1
	mov edx,workspace.x2

	mov s_z,eax
	mov xm,ebx

	mov x1,ecx
	mov x2,edx

	lea ebp,ReturnAddress0&fnId

	push ebp
	push eax ;padding stack with crap

	push ebp ;padding stack with crap
	push eax ;padding stack with crap

	push ebp ;padding stack with crap
	jmp TriangleRasterise&backId

ReturnAddress0&fnId&:
	mov eax,topCount
	mov ebx,bottomCount

	mov ecx,s_u
	mov edx,s_v

	mov esi,d_z_x

	mov workspace.topCount,eax
	mov workspace.bottomCount,ebx

	mov workspace.s_u,ecx
	mov workspace.s_v,edx

	mov workspace.d_z_x,esi
	
	mov eax,s_z
	mov ebx,xm

	mov ecx,x1
	mov edx,x2

	mov workspace.s_z,eax
	mov workspace.xm,ebx

	mov workspace.x1,ecx
	mov workspace.x2,edx

        mov eax,workspace.s_s
        mov ebx,workspace.d_s_x

        mov ecx,workspace.d_s_y_0
        mov edx,workspace.d_s_y_1

	mov	workspace.s_i,eax    
	mov workspace.d_i_x,ebx  
		                 
	mov workspace.d_i_y_0,ecx
	mov workspace.d_i_y_1,edx

	lea ebp,ReturnAddress1&fnId
 
	push ebp
	push eax ;padding stack with crap

	push ebp ;padding stack with crap
	push eax ;padding stack with crap

	push ebp ;padding stack with crap
	jmp TriangleRasterise&frontId

ReturnAddress1&fnId&:

	mov eax,workspace.v0
	mov ecx,workspace.v1

	mov edx,workspace.v2
	mov ebp,v0CI

	mov esi,v1CI
	mov edi,v2CI

	mov [eax+4*C_I],ebp
	mov [ecx+4*C_I],esi

	mov [edx+4*C_I],edi
        mov ebp,v0CS

        mov esi,v1CS
        mov edi,v2CS

        mov [eax+4*C_S],ebp
        mov [ecx+4*C_S],esi

        mov [edx+4*C_S],edi
	ret

TriangleRender&fnId endp
endm

if PARTS and PART_8Z

ECHO DECAL ZBUFFERED
        decal _ZTID_I8_D16_32,_ZTIS,_ZI_I8_D16,_ZTI_I8_D16_32
        decal _ZTID_I8_D16_64,_ZTIS,_ZI_I8_D16,_ZTI_I8_D16_64
        decal _ZTID_I8_D16_128,_ZTIS,_ZI_I8_D16,_ZTI_I8_D16_128
        decal _ZTID_I8_D16_256,_ZTIS,_ZI_I8_D16,_ZTI_I8_D16_256
        decal _ZTID_I8_D16_1024,_ZTIS,_ZI_I8_D16,_ZTI_I8_D16_1024

; The flat versions could be sped up by using the special flat rasterisers. 
        decal _ZTID_I8_D16_32_FLAT,_ZTIS_FLAT,_ZI_I8_D16,_ZTI_I8_D16_32
        decal _ZTID_I8_D16_64_FLAT,_ZTIS_FLAT,_ZI_I8_D16,_ZTI_I8_D16_64
        decal _ZTID_I8_D16_128_FLAT,_ZTIS_FLAT,_ZI_I8_D16,_ZTI_I8_D16_128
        decal _ZTID_I8_D16_256_FLAT,_ZTIS_FLAT,_ZI_I8_D16,_ZTI_I8_D16_256
        decal _ZTID_I8_D16_1024_FLAT,_ZTIS_FLAT,_ZI_I8_D16,_ZTI_I8_D16_1024

        decal _ZTID_I8_D16,_ZTIS_ARBITRARY,_ZI_I8_D16,_ZTI_I8_D16
        decal _ZTID_I8_D16_FLAT,_ZTIS_ARBITRARY_FLAT,_ZI_I8_D16,_ZTI_I8_D16

endif

if PARTS and PART_8

ECHO DECAL NON ZBUFFERED
        decal _TID_I8_256,_TIS,_I_I8,_TI_I8_256
        decal _TID_I8_256_FLAT,_TIS_FLAT,_I_I8,_TI_I8_256
        decal _TID_I8,_TIS_ARBITRARY,_I_I8,_TI_I8
        decal _TID_I8_FLAT,_TIS_ARBITRARY_FLAT,_I_I8,_TI_I8

endif


.data


	v0CI dword ?
	v1CI dword ?
	v2CI dword ?

        v0CS dword ?
        v1CS dword ?
        v2CS dword ?

	topCount dword ?
	bottomCount dword ?
	s_u dword ?
	s_v dword ?
	s_z dword ?
	d_z_x dword ?
	xm dword ?
	x1 dword ?
	x2 dword ?

	fp_1_256 dword  0.00390625f ; might be better to work out a better representation 

end
