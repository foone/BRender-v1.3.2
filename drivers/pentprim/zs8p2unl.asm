.586p
.model	flat,c

include fpwork.inc
include	drv.inc
include fpsetup.inc
include common.inc

.code

;**************************
; UnLit Textured Power Of 2
;**************************

; Note this rasterizer has not been paired, and still sets up the zbuffer scan address and fiddles with d_z_x

DRAW_T_I8_POW2 macro minorX,direction,half,pow2
	local drawPixel,drawLine,done,lineDrawn,noPlot,mask
; height test
	MAKE_N_LOW_BIT_MASK	mask,pow2

	mov ebp,workspace.&half&Count

	cmp ebp,0
	jl done

	mov ebx,workspace.minorX
	mov eax,workspace.s_u

	mov ecx,workspace.xm
	mov esi,workspace.s_v

	mov workspace.c_u,eax
	mov workspace.c_v,esi

	mov edi,workspace.scanAddress
	; unpaired instruction
drawLine:
	shr ebx,16

	shr ecx,16
	add edi,ebx

	ror edx,16

	sub ecx,ebx
	jg_d lineDrawn,direction

drawPixel:
	shr esi,16-pow2

	shr eax,16
	and esi,mask shl pow2

	and eax,mask

	or eax,esi
	mov esi,work.texture.base

	mov al,[esi+eax] 

	test al,al
	jz noPlot

	mov [edi+ecx],al

noPlot:
	mov esi,workspace.c_v
	mov ebx,workspace.d_v_x

	add_d esi,ebx,direction
	mov eax,workspace.c_u

	mov workspace.c_v,esi
	mov ebx,workspace.d_u_x

	add_d eax,ebx,direction
	inc_d ecx,direction

	mov workspace.c_u,eax
	jle_d drawPixel,direction

lineDrawn:
;perform per line updates

	mov ebp,workspace.xm_f
	mov edi,workspace.d_xm_f

	add ebp,edi
	mov eax,workspace.s_u

	sbb edx,edx
	mov workspace.xm_f,ebp

	mov ecx,[workspace.d_u_y_0+edx*8]
	mov esi,workspace.s_v

	add eax,ecx
	mov ebx,[workspace.d_v_y_0+edx*8]

	mov workspace.s_u,eax
	add esi,ebx

	mov workspace.c_u,eax
	mov workspace.c_v,esi

	mov workspace.s_v,esi

	mov edi,workspace.scanAddress

	mov ebx,work.colour.stride_b
	mov ecx,workspace.d_&minorX

	add edi,ebx
	mov ebx,workspace.minorX

	mov workspace.scanAddress,edi
	add ebx,ecx

	mov workspace.minorX,ebx
	mov ecx,workspace.xm

	mov ebp,workspace.d_xm
	;unpaired instruction

	add ecx,ebp
	mov ebp,workspace.&half&Count

	dec ebp
	mov workspace.xm,ecx

	mov workspace.&half&Count,ebp
	jge drawLine

done:
endm

TriangleRender_T_I8_POW2 macro pow2,powName
externdef c TriangleRasterise_T_I8&powName&:proc
TriangleRender_T_I8&powName proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex
	mov	eax,v0
	mov	ecx,v1	

	mov	edx,v2
	mov workspace.v0,eax

	mov workspace.v1,ecx
	mov workspace.v2,edx

	;half cycle wasted
	call TriangleSetup_T

TriangleRasterise_T_I8&powName label proc

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

	DRAW_T_I8_POW2 x1,DRAW_LR,top,pow2
	DRAW_T_I8_POW2 x2,DRAW_LR,bottom,pow2
	ret

drawRL:
	DRAW_T_I8_POW2 x1,DRAW_RL,top,pow2
	DRAW_T_I8_POW2 x2,DRAW_RL,bottom,pow2
	ret
TriangleRender_T_I8&powName endp
endm

	if PARTS and PART_8

TriangleRender_T_I8_POW2 5,_32
TriangleRender_T_I8_POW2 6,_64
TriangleRender_T_I8_POW2 7,_128
;TriangleRender_T_I8_POW2 8,_256
TriangleRender_T_I8_POW2 10,_1024

	endif

.data

fp_one			dword	1.0
fp_two			dword	2.0
fp_conv_d		dword	(127+52-0) shl 23 + (1 shl 22)

end

