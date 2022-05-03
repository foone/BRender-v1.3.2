.586p
.model	flat,c

include fpwork.inc
include	drv.inc
include fpsetup.inc
include common.inc

	; Cheat case entry points for perspective correct texture mapper
	;
        externdef c TriangleRasterise_Z_I8_D16:proc
		externdef c TriangleRasterise_ZTI_I8_D16_256:proc
        externdef c TriangleRasterise_ZTIF_I8_D16_256:proc

.code

;*****************
; FLAT RASTERISER
;*****************

DRAW_Z_I8_D16 macro minorX,direction,half
	local drawPixel,drawLine,done,lineDrawn,noPlot
; height test

	mov ebx,workspace.&half&Count
	mov ebp,workspace.depthAddress

	cmp ebx,0 
	jl done														  

	mov eax,workspace.s_z
	mov esi,workspace.d_z_x

	mov ebx,workspace.minorX
	mov ecx,workspace.xm

drawLine:
	ror eax,16
	
	shr ebx,16
	mov edi,workspace.scanAddress

	shr ecx,16
	add edi,ebx

	sub ecx,ebx
	jg_d lineDrawn,direction

	lea ebp,[ebp+2*ebx]
	xor ebx,ebx ;used to insure that cf is clear

	mov edx,eax ;needed to insure upper parts of reg are correct for the first pixel
	mov bl,byte ptr workspace.colour

drawPixel:
;regs ebp,depth edi,dest esi,dz eax,c_z ebx, ecx,count edx,old_z
	adc_d eax,0,direction
	mov dl,[ebp+2*ecx]

;The following line needs some more experimentation to prove its usefullness in real application

	mov dh,[ebp+2*ecx+1]

	cmp eax,edx
	ja noPlot												  

; writes
	mov [ebp+2*ecx],ax
	mov [edi+ecx],bl

noPlot:
	add_d eax,esi,direction ;wastes a cycle here
	inc_d ecx,direction

	mov edx,eax
	jle_d drawPixel,direction

lineDrawn:
;perform per line updates

	mov ebp,workspace.xm_f
	mov edi,workspace.d_xm_f
	
	add ebp,edi
	mov eax,workspace.s_z

	sbb edi,edi
	mov workspace.xm_f,ebp

	mov ebp,workspace.depthAddress
	mov edx,workspace.&half&Count

	mov ebx,[workspace.d_z_y_0+edi*8]
	mov edi,workspace.scanAddress

	add eax,ebx
	mov ebx,workspace.minorX

	adc eax,0
	mov ecx,workspace.xm

	add ecx,workspace.d_xm
	add edi,work.colour.stride_b

	mov workspace.s_z,eax
	mov workspace.scanAddress,edi

	add ebp,work.depth.stride_b
	add ebx,workspace.d_&minorX

	mov workspace.minorX,ebx
	mov workspace.xm,ecx

	dec edx
	mov workspace.depthAddress,ebp

	mov workspace.&half&Count,edx
	jge drawLine
done:
endm

	if PARTS and PART_8Z

TriangleRender_Flat macro procName,rasteriseName,setupName

procName proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex
	mov	eax,v0
	mov	ecx,v1	

	mov	edx,v2
	mov workspace.v0,eax

	mov workspace.v1,ecx

	mov workspace.v2,edx

; half cycle wasted
	call setupName

rasteriseName label proc

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

	mov eax,workspace.xm

	shl eax,16
	mov ebx,workspace.d_xm

	shl ebx,16
	mov workspace.xm_f,eax

	mov edx,workspace.d_z_x
	cmp edx,80000000
    adc edx,-1

	ror edx,16
	mov workspace.d_xm_f,ebx
	mov workspace.d_z_x,edx
	mov	eax,workspace.flip
	;half cycle wasted

	test eax,eax
	jnz	drawRL

	DRAW_Z_I8_D16 x1,DRAW_LR,top
	DRAW_Z_I8_D16 x2,DRAW_LR,bottom
	ret

drawRL:
	DRAW_Z_I8_D16 x1,DRAW_RL,top
	DRAW_Z_I8_D16 x2,DRAW_RL,bottom
	ret
procName endp
endm
endif

;***********************
; LIT LINEAR RASTERISER
;***********************

if 1 ; old but faster code.
majorScanAddress equ scratch0

DRAW_ZTI_I8_D16_256 macro minorX,direction,half
	local drawPixel,drawLine,done,lineDrawn,noPlot
; height test

	mov ebx,workspace.&half&Count
	mov ebp,workspace.depthAddress

	cmp ebx,0 
	jl done														  

; trapezoid setup

	mov edx,workspace.s_z
	mov eax,workspace.s_u

	mov workspace.c_z,edx
	mov workspace.c_u,eax

        mov eax,workspace.s_i
	mov ebx,workspace.s_v

	mov workspace.c_v,ebx
        mov workspace.c_i,eax

drawLine:
	mov edx,workspace.minorX
	mov ecx,workspace.xm

	shr edx,16 
	mov edi,workspace.scanAddress

	shr ecx,16
	add edi,edx

	sub ecx,edx
	mov workspace.majorScanAddress,edi

	lea ebp,[ebp+2*edx]
	jg_d lineDrawn,direction

drawPixel:
;z-buffer
	xor edx,edx ;/and edx,0ffffh
	mov eax,workspace.c_z

	shr eax,16
	mov dh,[ebp+2*ecx+1]

	xor ebx,ebx
	mov dl,[ebp+2*ecx]

	cmp eax,edx
	mov dl,byte ptr[workspace.c_u+2]

	mov esi,work.texture.base
	mov dh,byte ptr[workspace.c_v+2]

        mov bh,byte ptr[workspace.c_i+2]
	ja noPlot	

	mov edi,work.shade_table
	mov bl,[esi+edx]

	mov dl,bl
	add ebx,edi

	test dl,dl
	mov edi,workspace.majorScanAddress

	mov bl,[ebx]
	jz noPlot

; writes
	mov [ebp+2*ecx],ax ;wasted cycle
	mov [edi+ecx],bl
; add on deltas

noPlot:
;update z,a
	mov eax,workspace.c_z
        mov ebx,workspace.c_i

	mov esi,workspace.d_z_x
        mov edx,workspace.d_i_x

	add_d eax,esi,direction
	add_d ebx,edx,direction

	mov workspace.c_z,eax
        mov workspace.c_i,ebx

;update u,v
	mov eax,workspace.c_u
	mov ebx,workspace.c_v

	mov esi,workspace.d_u_x
	mov edx,workspace.d_v_x

	add_d eax,esi,direction
	add_d ebx,edx,direction

	mov workspace.c_u,eax
	mov workspace.c_v,ebx

	inc_d ecx,direction
	jle_d drawPixel,direction

lineDrawn:
;perform per line updates
	mov eax,workspace.xm_f
	mov ebx,workspace.d_xm_f

	add eax,ebx

	sbb esi,esi

	mov workspace.xm_f,eax

	mov eax,workspace.xm
	mov ecx,workspace.minorX

	mov edx,workspace.d_&minorX
	mov ebx,workspace.d_xm

	add eax,ebx
	add ecx,edx

	mov workspace.xm,eax
	mov workspace.minorX,ecx
	
;update u,v
	mov eax,workspace.s_u
	mov ebx,[workspace.d_u_y_0+esi*8]

	mov ecx,workspace.s_v
	mov edx,[workspace.d_v_y_0+esi*8]

	add eax,ebx
	add ecx,edx

	mov workspace.c_u,eax
	mov workspace.c_v,ecx

	mov workspace.s_u,eax
	mov workspace.s_v,ecx

;update z,a
	mov eax,workspace.s_z
	mov ebx,[workspace.d_z_y_0+esi*8]

        mov ecx,workspace.s_i
        mov edx,[workspace.d_i_y_0+esi*8]

	add eax,ebx
	add ecx,edx

	mov workspace.c_z,eax
        mov workspace.c_i,ecx

	mov workspace.s_z,eax
        mov workspace.s_i,ecx

	mov ebp,workspace.depthAddress
	mov ebx,work.depth.stride_b

	mov edi,workspace.scanAddress
    mov eax,work.colour.stride_b

	add ebp,ebx
	add edi,eax

	mov ecx,workspace.&half&Count
	mov workspace.scanAddress,edi

	dec ecx
	mov workspace.depthAddress,ebp

	mov workspace.&half&Count,ecx
	jge drawLine
done:

endm
endif

if 0 ; needs some work
DRAW_ZTI_I8_D16_256 macro minorX,direction,half
local drawPixel,drawLine,done,lineDrawn,noPlot

	mov ebx,workspace.&half&Count
	mov edi,workspace.scanAddress
	
	cmp ebx,0
	jl done

	mov eax,workspace.s_u
	mov ebx,workspace.s_v

        mov ecx,workspace.s_i
	mov edx,workspace.s_z

	mov workspace.c_u,eax
	mov workspace.c_v,ebx
		              
        mov workspace.c_i,ecx
	mov workspace.c_z,edx

	mov ebp,workspace.depthAddress
	mov edx,workspace.minorX

drawLine:

	shr edx,16
	mov ecx,workspace.xm

	shr ecx,16
	add edi,edx
	
	xor eax,eax
	sub ecx,edx

	lea ebp,[ebp+2*edx]	
	jg_d lineDrawn,direction

drawPixel:

	mov al,byte ptr[workspace.c_u+2]
	mov	esi,work.texture.base

	mov ah,byte ptr[workspace.c_v+2]
	mov bl,byte ptr[workspace.c_z+2]

	mov dl,[ebp+2*ecx]
	mov bh,byte ptr[workspace.c_z+3]

	mov al,[esi+eax]
	mov dh,[ebp+2*ecx+1]
	
	test al,al
	jz noPlot

        mov ah,byte ptr[workspace.c_i+2]
	mov esi,work.shade_table

	cmp bx,dx ;2 cycles
	ja noPlot
	
	mov [ebp+2*ecx],bx ; two cycles
	mov al,[esi+eax]

	mov [edi+ecx],al 

noPlot:

	mov ebx,workspace.d_u_x
	mov edx,workspace.d_v_x

	add_d workspace.c_u,ebx,direction ; 3 cycles
	add_d workspace.c_v,edx,direction

	mov ebx,workspace.d_z_x
        mov edx,workspace.d_i_x

	add_d workspace.c_z,ebx,direction ; 3 cycles
        add_d workspace.c_i,edx,direction

	inc_d ecx,direction
	jle_d drawPixel,direction

lineDrawn:
	mov ebx,workspace.xm_f
	mov edx,workspace.d_xm_f

	add ebx,edx
	mov ecx,workspace.&half&Count

	sbb esi,esi
	mov workspace.xm_f,ebx

	mov edi,workspace.scanAddress
	mov ebp,workspace.depthAddress

	add edi,work.colour.stride_b ;two cycles
	add ebp,work.depth.stride_b

	mov workspace.scanAddress,edi
	mov workspace.depthAddress,ebp

	mov ebx,workspace.s_u
	mov edx,workspace.s_v

	add ebx,[workspace.d_u_y_0+8*esi] ;two cycles
	add edx,[workspace.d_v_y_0+8*esi]

	mov workspace.s_u,ebx
	mov workspace.s_v,edx

	mov workspace.c_u,ebx
	mov workspace.c_v,edx
	
	mov ebx,workspace.s_z
        mov edx,workspace.s_i

	add ebx,[workspace.d_z_y_0+8*esi] ;two cycles
        add edx,[workspace.d_i_y_0+8*esi]

	mov workspace.s_z,ebx
        mov workspace.s_i,edx

	mov workspace.c_z,ebx
        mov workspace.c_i,edx

	mov ebx,workspace.xm
	mov edx,workspace.minorX

	add ebx,workspace.d_xm ;two cycles
	add edx,workspace.d_&minorX

	mov workspace.xm,ebx
	mov workspace.minorX,edx

	dec ecx

	mov workspace.&half&Count,ecx
	jge drawLine
endif 


	if PARTS and PART_8Z

TriangleRender_ZTI_I8_D16_256_FLAT proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_ZTI_FLAT

	jmp	TriangleRasterise_ZTI_I8_D16_256

TriangleRender_ZTI_I8_D16_256_FLAT endp


TriangleRender_ZTI_I8_D16_256 proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

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

	mov eax,workspace.xm

	shl eax,16
	mov ebx,workspace.d_xm

	shl ebx,16
	mov workspace.xm_f,eax

	mov workspace.d_xm_f,ebx
	mov	eax,workspace.flip

	test eax,eax
	jnz	drawRL

	DRAW_ZTI_I8_D16_256 x1,DRAW_LR,top
	DRAW_ZTI_I8_D16_256 x2,DRAW_LR,bottom
	ret

drawRL:
	DRAW_ZTI_I8_D16_256 x1,DRAW_RL,top
	DRAW_ZTI_I8_D16_256 x2,DRAW_RL,bottom
	ret
TriangleRender_ZTI_I8_D16_256 endp

	endif

;******************************
; LIT LINEAR FOGGED RASTERISER
;******************************

majorScanAddress equ scratch0


DRAW_ZTIF_I8_D16_256 macro minorX,direction,half
	local drawPixel,drawLine,done,lineDrawn,noPlot
; height test

	mov ebx,workspace.&half&Count
	mov ebp,workspace.depthAddress

	cmp ebx,0 
	jl done														  

; trapezoid setup

	mov edx,workspace.s_z
	mov eax,workspace.s_u

	mov workspace.c_z,edx
	mov workspace.c_u,eax

        mov eax,workspace.s_i
	mov ebx,workspace.s_v

	mov workspace.c_v,ebx
        mov workspace.c_i,eax

drawLine:
	mov edx,workspace.minorX
	mov ecx,workspace.xm

	shr edx,16 
	mov edi,workspace.scanAddress

	shr ecx,16
	add edi,edx

	sub ecx,edx
	mov workspace.majorScanAddress,edi

	lea ebp,[ebp+2*edx]
	jg_d lineDrawn,direction

drawPixel:
;z-buffer
	xor edx,edx ;/and edx,0ffffh
	mov eax,workspace.c_z

	shr eax,16
	mov dh,[ebp+2*ecx+1]

	xor ebx,ebx
	mov dl,[ebp+2*ecx]

	cmp eax,edx
	mov dl,byte ptr[workspace.c_u+2]

	mov esi,work.texture.base
	mov dh,byte ptr[workspace.c_v+2]

        mov bh,byte ptr[workspace.c_i+2]
	ja noPlot	

	mov edi,work.shade_table
	mov bl,[esi+edx]

        mov dh,ah
        test bl,bl

        mov dl,[edi+ebx]
        mov edi,work.fog_table

        mov esi,workspace.majorScanAddress
	jz noPlot

; writes
        mov bl,[edi+edx]
        mov [ebp+2*ecx],al

        mov [ebp+2*ecx+1],ah
        mov [esi+ecx],bl
; add on deltas

noPlot:
;update z,a
	mov eax,workspace.c_z
        mov ebx,workspace.c_i

	mov esi,workspace.d_z_x
        mov edx,workspace.d_i_x

	add_d eax,esi,direction
	add_d ebx,edx,direction

	mov workspace.c_z,eax
        mov workspace.c_i,ebx

;update u,v
	mov eax,workspace.c_u
	mov ebx,workspace.c_v

	mov esi,workspace.d_u_x
	mov edx,workspace.d_v_x

	add_d eax,esi,direction
	add_d ebx,edx,direction

	mov workspace.c_u,eax
	mov workspace.c_v,ebx

	inc_d ecx,direction
	jle_d drawPixel,direction

lineDrawn:
;perform per line updates
	mov eax,workspace.xm_f
	mov ebx,workspace.d_xm_f

	add eax,ebx

	sbb esi,esi

	mov workspace.xm_f,eax

	mov eax,workspace.xm
	mov ecx,workspace.minorX

	mov edx,workspace.d_&minorX
	mov ebx,workspace.d_xm

	add eax,ebx
	add ecx,edx

	mov workspace.xm,eax
	mov workspace.minorX,ecx
	
;update u,v
	mov eax,workspace.s_u
	mov ebx,[workspace.d_u_y_0+esi*8]

	mov ecx,workspace.s_v
	mov edx,[workspace.d_v_y_0+esi*8]

	add eax,ebx
	add ecx,edx

	mov workspace.c_u,eax
	mov workspace.c_v,ecx

	mov workspace.s_u,eax
	mov workspace.s_v,ecx

;update z,a
	mov eax,workspace.s_z
	mov ebx,[workspace.d_z_y_0+esi*8]

        mov ecx,workspace.s_i
        mov edx,[workspace.d_i_y_0+esi*8]

	add eax,ebx
	add ecx,edx

	mov workspace.c_z,eax
        mov workspace.c_i,ecx

	mov workspace.s_z,eax
        mov workspace.s_i,ecx

	mov ebp,workspace.depthAddress
	mov ebx,work.depth.stride_b

	mov edi,workspace.scanAddress
    mov eax,work.colour.stride_b

	add ebp,ebx
	add edi,eax

	mov ecx,workspace.&half&Count
	mov workspace.scanAddress,edi

	dec ecx
	mov workspace.depthAddress,ebp

	mov workspace.&half&Count,ecx
	jge drawLine
done:
endm

	if PARTS and PART_8Z_FOG

TriangleRender_ZTIF_I8_D16_256_FLAT proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_ZTI_FLAT

	jmp	TriangleRasterise_ZTIF_I8_D16_256

TriangleRender_ZTIF_I8_D16_256_FLAT endp


TriangleRender_ZTIF_I8_D16_256 proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_ZTI

TriangleRasterise_ZTIF_I8_D16_256 label proc

	mov eax,work.colour.stride_b
	mov ebx,workspace.t_y

	dec ebx	

	mov edi,work.colour.base
	mov ebp,work.depth.base

	imul ebx

	add edi,eax

	dec edi
	mov eax,work.depth.stride_b

	imul ebx

	sub eax,2
	
	add ebp,eax
	mov workspace.scanAddress,edi

	mov workspace.depthAddress,ebp
	mov eax,workspace.xm

	shl eax,16
	mov ebx,workspace.d_xm

	shl ebx,16
	mov workspace.xm_f,eax

	mov workspace.d_xm_f,ebx
	mov	eax,workspace.flip

	test eax,eax
	jnz	drawRL

        DRAW_ZTIF_I8_D16_256 x1,DRAW_LR,top
        DRAW_ZTIF_I8_D16_256 x2,DRAW_LR,bottom
	ret

drawRL:
        DRAW_ZTIF_I8_D16_256 x1,DRAW_RL,top
        DRAW_ZTIF_I8_D16_256 x2,DRAW_RL,bottom
	ret
TriangleRender_ZTIF_I8_D16_256 endp

	endif

if PARTS and PART_8Z
    TriangleRender_Flat TriangleRender_Z_I8_D16,TriangleRasterise_Z_I8_D16,TriangleSetup_Z
    TriangleRender_Flat TriangleRender_Z_I8_D16_ShadeTable,TriangleRasterise_Z_I8_D16_ShadeTable,TriangleSetup_Z_ShadeTable
endif

.data

fp_one			dword	1.0
fp_two			dword	2.0
fp_conv_d		dword	(127+52-0) shl 23 + (1 shl 22)

end
