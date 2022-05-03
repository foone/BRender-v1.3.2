; outer loop could easily be optimised

.586p
.model	flat,c

include	drv.inc
include common.inc
include fpsetup.inc
include fpwork.inc

.code
majorScanAddress equ scratch0

DRAW_ZI_I8_D16_ShadeTable macro minorX,direction,half
local drawPixel,drawLine,done,lineDrawn,noPlot

	mov ebx,workspace.&half&Count
	mov edi,workspace.scanAddress
	
	cmp ebx,0
	jl done

    mov esi,workspace.s_i
	mov edx,workspace.s_z

    mov workspace.c_i,esi
	mov workspace.c_z,edx
	
	mov ecx,workspace.s_i
	mov ebx,workspace.s_z

	mov ebp,workspace.depthAddress
	mov edx,workspace.minorX

drawLine:

	shr edx,16
	mov esi,workspace.xm
                                                                                                                                        
	shr esi,16
	add edi,edx
	
	ror ecx,16
	xor eax,eax

	ror ebx,16
	sub esi,edx

	mov workspace.majorScanAddress,edi


	lea ebp,[ebp+2*edx]	
	jg_d lineDrawn,direction

drawPixel:
	mov al,byte ptr work.index_base
	mov edi,workspace.d_i_x

    mov ah,cl
	mov dl,[ebp+2*esi]

    add_d ecx,edi,direction
	mov edi,work.shade_table

	adc_d ecx,0,direction
	mov dh,[ebp+2*esi+1]

	mov al,[edi+eax]
	mov edi,workspace.majorScanAddress

	cmp bx,dx ;2 cycles
	ja noPlot

	mov [ebp+2*esi],bl 
	mov [edi+esi],al

	mov [ebp+2*esi+1],bh 
noPlot:
	mov edi,workspace.d_z_x

	add_d ebx,edi,direction 
; stall

	adc_d ebx,0,direction
	inc_d esi,direction

; stall
	jle_d drawPixel,direction

lineDrawn:
	mov ebx,workspace.xm_f
	mov edx,workspace.d_xm_f

	add ebx,edx

	sbb ecx,ecx
	mov workspace.xm_f,ebx

	mov edi,workspace.scanAddress
	add edi,work.colour.stride_b ;two cycles
	mov workspace.scanAddress,edi

	mov ebp,workspace.depthAddress
	add ebp,work.depth.stride_b
	mov workspace.depthAddress,ebp

	mov ebx,workspace.s_z
	add ebx,[workspace.d_z_y_0+8*ecx] ;two cycles
	mov workspace.s_z,ebx

    mov edx,workspace.s_i
    add edx,[workspace.d_i_y_0+8*ecx]
    mov workspace.s_i,edx
	mov ecx,edx ;probably will optimise out

	mov esi,workspace.xm
	add esi,workspace.d_xm ;two cycles
	mov workspace.xm,esi

	mov edx,workspace.minorX
	add edx,workspace.d_&minorX
	mov workspace.minorX,edx

	mov esi,workspace.&half&Count
	dec esi
	mov workspace.&half&Count,esi
	jge drawLine
done:
endm

if PARTS and PART_8Z

TriangleRender_ZI_I8_D16_ShadeTable proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_ZTI

TriangleRasterise_ZTI_I8_D16_256 label proc
; Floating point address calculation - 20 cycles, (Integer=26)
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
	;stall
	faddp st(2),st					;	ca			da
	fstp qword ptr workspace.scanAddress
	fstp qword ptr workspace.depthAddress

	mov edx,workspace.d_z_x
	cmp edx,80000000
    adc edx,-1
	ror edx,16
	mov workspace.d_z_x,edx

	mov edx,workspace.d_i_x
	cmp edx,80000000
    adc edx,-1
	ror edx,16
	mov workspace.d_i_x,edx

	mov eax,workspace.xm

	shl eax,16
	mov ebx,workspace.d_xm

	shl ebx,16
	mov workspace.xm_f,eax

	mov workspace.d_xm_f,ebx
	mov	eax,workspace.flip

	test eax,eax
	jnz	drawRL

	DRAW_ZI_I8_D16_ShadeTable x1,DRAW_LR,top
	DRAW_ZI_I8_D16_ShadeTable x2,DRAW_LR,bottom
	ret

drawRL:
	DRAW_ZI_I8_D16_ShadeTable x1,DRAW_RL,top
	DRAW_ZI_I8_D16_ShadeTable x2,DRAW_RL,bottom
	ret
TriangleRender_ZI_I8_D16_ShadeTable endp

endif


.data

fp_one			dword	1.0
fp_two			dword	2.0
fp_conv_d		dword	(127+52-0) shl 23 + (1 shl 22)

end

