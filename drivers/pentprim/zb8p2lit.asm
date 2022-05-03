.586p
.model	flat,c

include fpwork.inc
include	drv.inc
include fpsetup.inc
include common.inc

.code

;*************************
; Lit Textured Power Of 2
;*************************


DRAW_ZTI_I8_D16_POW2 macro minorX,direction,half,pow2
	local drawPixel,drawLine,done,lineDrawn,noPlot,mask
; height test
	MAKE_N_LOW_BIT_MASK	mask,pow2

	mov edx,workspace.&half&Count
	mov ebx,workspace.s_z

	cmp edx,0
	jl done

	mov edx,workspace.minorX
	mov esi,workspace.s_i

	mov ecx,workspace.xm
	mov eax,workspace.s_u

	mov workspace.c_i,esi
	mov edi,workspace.s_v

	mov workspace.c_u,eax
	mov workspace.c_v,edi

	mov ebp,workspace.depthAddress

drawLine:
	shr edx,16
	mov esi,workspace.scanAddress

	shr ecx,16
	add esi,edx

	lea ebp,[ebp+2*edx]
	mov workspace.scratch0,esi

	sub ecx,edx
	jg_d lineDrawn,direction

	ror ebx,16
	mov workspace.scratch1,ebp

	mov edi,workspace.s_v

drawPixel:
	shr eax,16
	mov ebp,workspace.scratch1
	
	shr edi,16-pow2
	and eax,mask

	mov esi,work.texture.base
	and edi,mask shl pow2

	mov dl,[ebp+2*ecx]
	or eax,edi

	mov dh,[ebp+2*ecx+1]
	mov edi,work.shade_table
	
	cmp bx,dx ;two cycles
	mov al,[esi+eax] 

	mov ah,byte ptr[workspace.c_i+2]
	ja noPlot

	test al,al
	mov esi,workspace.scratch0

	mov al,[edi+eax]
	jz noPlot

	mov [ebp+2*ecx],bx ;two cycles
	mov [esi+ecx],al
noPlot:
	mov eax,workspace.c_i
	mov ebp,workspace.d_z_x
	
	add_d ebx,ebp,direction
	mov esi,workspace.d_i_x

	adc_d ebx,0,direction
	mov edi,workspace.c_v

	add_d eax,esi,direction
	mov ebp,workspace.d_v_x

	add_d edi,ebp,direction
	mov workspace.c_i,eax

	mov eax,workspace.c_u
	mov ebp,workspace.d_u_x

	add_d eax,ebp,direction
	mov workspace.c_v,edi

	mov workspace.c_u,eax
	inc_d ecx,direction

	; cycle wasted
	jle_d drawPixel,direction

lineDrawn:
;perform per line updates

if 0 ; could save one cycle in outer loop by arranging s.t. edi does not need to be reloaded with s_v each line

	mov ebp,workspace.xm_f
	mov edi,workspace.d_xm_f

	add ebp,edi

	sbb edx,edx
	mov workspace.xm_f,ebp


	mov edi,workspace.s_i
	mov esi,[workspace.d_i_y_0+edx*8]
	add edi,esi
	mov workspace.c_i,edi
	mov workspace.s_i,edi

	mov ebx,workspace.s_z
	mov ebp,[workspace.d_z_y_0+edx*8]
	add ebx,ebp
	mov workspace.s_z,ebx


	mov eax,workspace.s_u
	mov ecx,[workspace.d_u_y_0+edx*8]
	add eax,ecx
	mov workspace.s_u,eax
	mov workspace.c_u,eax

	mov edi,workspace.s_v
	mov esi,[workspace.d_v_y_0+edx*8]
	add edi,esi
	mov workspace.c_v,edi
	mov workspace.s_v,edi


	mov ebp,workspace.depthAddress
	mov ecx,work.depth.stride_b
	add ebp,ecx
	mov workspace.depthAddress,ebp

	
	mov ecx,workspace.scanAddress
	mov esi,work.colour.stride_b
	add ecx,esi
	mov workspace.scanAddress,ecx


	mov edx,workspace.minorX
	mov ecx,workspace.d_&minorX
	add edx,ecx
	mov workspace.minorX,edx


	mov ecx,workspace.xm
	mov esi,workspace.d_xm

	add ecx,esi
	mov esi,workspace.&half&Count

	mov workspace.xm,ecx
	dec esi

	mov workspace.&half&Count,esi
	jge drawLine
endif 

	mov ebp,workspace.xm_f
	mov edi,workspace.d_xm_f

	add ebp,edi
	mov edi,workspace.s_i

	sbb edx,edx
	mov workspace.xm_f,ebp

	mov ebx,workspace.s_z
	mov ebp,workspace.depthAddress

	mov esi,[workspace.d_i_y_0+edx*8]
	mov eax,workspace.s_u

	add edi,esi
	mov ecx,[workspace.d_z_y_0+edx*8]

	mov workspace.c_i,edi
	add ebx,ecx

	mov workspace.s_i,edi
	mov workspace.s_z,ebx

	mov ecx,[workspace.d_u_y_0+edx*8]
	mov edi,workspace.s_v

	add eax,ecx
	mov esi,[workspace.d_v_y_0+edx*8]

	mov workspace.s_u,eax
	add edi,esi

	mov workspace.c_u,eax
	mov workspace.c_v,edi

	mov workspace.s_v,edi
	mov ecx,work.depth.stride_b

	add ebp,ecx
	mov edi,workspace.scanAddress

	mov workspace.depthAddress,ebp
	mov esi,work.colour.stride_b

	add edi,esi
	mov edx,workspace.minorX

	mov workspace.scanAddress,edi
	mov ecx,workspace.d_&minorX

	add edx,ecx
	mov ecx,workspace.xm

	mov esi,workspace.d_xm
	mov workspace.minorX,edx

	add ecx,esi
	mov esi,workspace.&half&Count

	dec esi
	mov workspace.xm,ecx

	mov workspace.&half&Count,esi
	jge drawLine

done:
endm

TriangleRender_ZTI_I8_D16_POW2 macro pow2,powName
externdef c TriangleRasterise_ZTI_I8_D16&powName&:proc

TriangleRender_ZTI_I8_D16&powName proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex
	mov	eax,v0
	mov	ecx,v1	

	mov	edx,v2
	mov workspace.v0,eax

	mov workspace.v1,ecx
	mov workspace.v2,edx

	;half cycle wasted
	call TriangleSetup_ZTI

TriangleRasterise_ZTI_I8_D16&powName label proc

; Floating point address calculation - 19 cycles, (Integer=26)
;										st(0)		st(1)		st(2)		st(3)		st(4)		st(5)		st(6)		st(7)
	fild work.colour.base			;	cb
	fild workspace.t_y				;	ty			cb
	fild work.depth.base			;	db			ty			cb
	fild work.colour.stride_b		;	cs			db			ty			cb
	fild work.depth.stride_b		;	ds			cs			db			ty			cb
	fxch st(4)						;	cb			cs			db			ty			ds
	fsub fp_one						;	cb-1		cs			db			ty			ds
	 fxch st(3)						;	ty			cs			db			cb-1		ds
	fsub fp_one						;	ty-1		cs			db			cb-1		ds
	 fxch st(2)						;	db			cs			ty-1		cb-1		ds
	fsub fp_two						;	db-2		cs			ty-1		cb-1		ds
	 fxch st(3)						;	cb-1		cs			ty-1		db-2		ds
	fadd fp_conv_d					;	cb-1I		cs			ty-1		db-2		ds
	 fxch st(1)						;	cs			cb-1I		ty-1		db-2		ds
	fmul st,st(2)					;	csy			cb-1I		ty-1		db-2		ds
	 fxch st(3)						;	db-2		cb-1I		ty-1		csy			ds
	fadd fp_conv_d					;	db-2I		cb-1I		ty-1		csy			ds
	 fxch st(2)						;	ty-1		cb-1I		db-2I		csy			ds
	fmulp st(4),st					;	cb-1I		db-2I		csy			dsy
	faddp st(2),st					;	db-2I		ca			dsy
	;x-stall
	mov eax,workspace.xm
	mov ebx,workspace.d_xm

	faddp st(2),st					;	ca			da
	;x-stall
	;x-stall

	shl eax,16
	mov edx,workspace.d_z_x

	shl ebx,16
	mov workspace.xm_f,eax

	fstp qword ptr workspace.scanAddress
	fstp qword ptr workspace.depthAddress

	mov workspace.d_xm_f,ebx
	cmp edx,80000000

    adc edx,-1
	mov	eax,workspace.flip

	ror edx,16
	test eax,eax

	mov workspace.d_z_x,edx
	jnz	drawRL

	DRAW_ZTI_I8_D16_POW2 x1,DRAW_LR,top,pow2
	DRAW_ZTI_I8_D16_POW2 x2,DRAW_LR,bottom,pow2
	ret

drawRL:
	DRAW_ZTI_I8_D16_POW2 x1,DRAW_RL,top,pow2
	DRAW_ZTI_I8_D16_POW2 x2,DRAW_RL,bottom,pow2
	ret
TriangleRender_ZTI_I8_D16&powName endp
endm

	if PARTS and PART_8Z

TriangleRender_ZTI_I8_D16_POW2 3,_8
TriangleRender_ZTI_I8_D16_POW2 4,_16
TriangleRender_ZTI_I8_D16_POW2 5,_32
TriangleRender_ZTI_I8_D16_POW2 6,_64
TriangleRender_ZTI_I8_D16_POW2 7,_128
;TriangleRender_ZTI_I8_D16_POW2 8,_256G
TriangleRender_ZTI_I8_D16_POW2 10,_1024

	endif

.data

fp_one			dword	1.0
fp_two			dword	2.0
fp_conv_d		dword	(127+52-0) shl 23 + (1 shl 22)

end

