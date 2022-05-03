.586p
.model flat,c
include drv.inc
include fpwork.inc
include fpsetup.inc
include common.inc


	; Cheat case entry points for perspective correct texture mapper
	;
		externdef c TriangleRasterise_TI_I8_256:proc
		externdef c TriangleRasterise_T_I8_256:proc
                externdef c TriangleRasterise_I_I8:proc

.code

;*****************
; FLAT RASTERISER
;*****************

DRAW_I8 macro minorX,direction,half
	local drawLine,done,lineDrawn,mask,noPlot,postOdd,aligned,alignedDone
; height test
	mov ebx,workspace.&half&Count
	mov edi,workspace.scanAddress

	cmp ebx,0 
	jl done														  

drawLine:

ifidni <direction>,<DRAW_LR>
	mov ecx,workspace.minorX
	mov edx,workspace.xm
else
	mov edx,workspace.minorX
	mov ecx,workspace.xm
endif

	shr edx,16
	mov eax,workspace.colour

	shr ecx,16
	add edi,edx

	sub ecx,edx
	mov esi,edi

	inc ecx
	jz lineDrawn

	mov ebx,ecx
	add esi,3

	and esi,-4

	sub esi,edi

	cmp ecx,esi
	jle	postOdd

	sub ebx,esi
	mov ecx,esi
;preodd
	rep stosb

	mov ecx,ebx
	shr ecx,2

	mov edx,ecx
	jz alignedDone

	rep stosd

alignedDone:
	mov ecx,ebx
	and ecx,3

postOdd:
	rep stosb

lineDrawn:
;perform per line updates
	mov eax,workspace.xm
	mov ecx,workspace.minorX

	mov edx,workspace.d_&minorX
	mov ebx,workspace.d_xm

	add eax,ebx
	add ecx,edx

	mov workspace.xm,eax
	mov workspace.minorX,ecx
	
	mov edi,workspace.scanAddress
	mov eax,work.colour.stride_b

	add edi,eax
	mov ecx,workspace.&half&Count

	mov workspace.scanAddress,edi
	dec ecx

	mov workspace.&half&Count,ecx
	jge drawLine
done:
endm

TriangleRender_I8 proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex
	mov	eax,v0
	mov	ecx,v1

	mov workspace.v0,eax
	mov workspace.v1,ecx

	mov edx,v2

	mov workspace.v2,edx

	call TriangleSetup

	mov dl,byte ptr workspace.colour
	xor ebx,ebx

	mov bl,dl
	mov dh,byte ptr workspace.colour

	shl edx,16
	mov bh,byte ptr workspace.colour

	or ebx,edx

	mov workspace.colour,ebx

	mov eax,work.colour.stride_b
	mov ebx,workspace.t_y

	dec ebx
	mov edi,work.colour.base

	imul ebx ;yes I know they don't pair
	cld 
	
	add edi,eax
	mov eax,workspace.flip

	dec edi
	test eax,eax

	fld qword ptr [workspace.colour]

	mov workspace.scanAddress,edi
	jnz	drawRL

	DRAW_I8 x1,DRAW_LR,top
	DRAW_I8 x2,DRAW_LR,bottom
	fstp qword ptr [workspace.colour]
	ret

drawRL:
	DRAW_I8 x1,DRAW_RL,top
	DRAW_I8 x2,DRAW_RL,bottom
	fstp qword ptr [workspace.colour]
	ret
TriangleRender_I8 endp


;********
; SMOOTH
;********

DRAW_I_I8 macro minorX,direction,half
	local drawLine,done,lineDrawn,drawPixel
; height test
	mov ebx,workspace.&half&Count
	mov edi,workspace.scanAddress

	test ebx,08000h
	jnz done														  
	
drawLine:
	mov edx,workspace.minorX
	mov ecx,workspace.xm

	shr edx,16 
	mov edi,workspace.scanAddress

	shr ecx,16
	add edi,edx

	sub ecx,edx
	jg_d lineDrawn,direction

        mov ebx,workspace.s_i
	ror ebx,16

        mov eax,workspace.d_i_x
drawPixel:
	mov [edi+ecx],bl										   
	add_d ebx,eax,direction										   

	adc_d ebx,0,direction

	inc_d ecx,direction
	jle_d drawPixel,direction

lineDrawn:
;perform per line updates
	mov ebp,workspace.xm_f
	mov edi,workspace.d_xm_f
	
	add ebp,edi
	sbb edi,edi

	mov workspace.xm_f,ebp
        mov ebx,workspace.s_i

        mov eax,[workspace.d_i_y_0+edi*8]
	add ebx,eax
        mov workspace.s_i,ebx
		
	mov eax,workspace.xm
	mov ecx,workspace.minorX

	mov edx,workspace.d_&minorX
	mov ebx,workspace.d_xm

	add eax,ebx
	add ecx,edx

	mov workspace.xm,eax
	mov workspace.minorX,ecx
	
	mov edi,workspace.scanAddress
	mov eax,work.colour.stride_b

	add edi,eax
	mov ecx,workspace.&half&Count

	mov workspace.scanAddress,edi
	dec ecx

	mov workspace.&half&Count,ecx
	jge drawLine
done:
endm

TriangleRender_I_I8 proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex
	mov	eax,v0
	mov	ecx,v1

	mov edx,v2
	mov workspace.v0,eax

	mov workspace.v1,ecx
	mov workspace.v2,edx

	;half a cycle wasted here
	call TriangleSetup_I

TriangleRasterise_I_I8 label proc

	mov eax,workspace.xm
	shl eax,16
	mov workspace.xm_f,eax

	mov eax,workspace.d_xm
	shl eax,16
	mov workspace.d_xm_f,eax
	
        mov eax,workspace.d_i_x
	cmp eax,80000000h
        adc eax,-1

	ror eax,16
        mov workspace.d_i_x,eax

	mov eax,work.colour.stride_b
	mov ebx,workspace.t_y

	dec ebx
	mov edi,work.colour.base

	imul ebx 

	add edi,eax
	mov eax,workspace.flip

	dec edi
	test eax,eax

	mov workspace.scanAddress,edi
	jnz	drawRL

	DRAW_I_I8 x1,DRAW_LR,top
	DRAW_I_I8 x2,DRAW_LR,bottom
	ret

drawRL:
	DRAW_I_I8 x1,DRAW_RL,top
	DRAW_I_I8 x2,DRAW_RL,bottom
	ret
TriangleRender_I_I8 endp


;*******************************
; UNLIT LINEAR TEXTURED 256x256
;*******************************

fdu equ scratch0																			   

;Register Usage during inner loop:
;eax=fu,idv,idu
;ebx=fv,0,texelvalue
;ecx=count
;edx=0,v,u
;edi=destination
;esi=source
;ebp=fdv,0,0

DRAW_T_I8_256 macro minorX,direction,half
	local drawPixel,drawLine,done,lineDrawn,noPlot
; height test
	mov ebx,workspace.&half&Count
	mov edi,workspace.scanAddress

	cmp ebx,0 
	jl done														  

; trapezoid setup

	mov edx,workspace.minorX
	mov ecx,workspace.xm

drawLine:

	shr edx,16 
	mov ebx,workspace.s_v
	
	shr ecx,16
	add edi,edx

	sub ecx,edx
	jg_d lineDrawn,direction

	mov eax,workspace.s_u
	xor edx,edx

	shl eax,16
	mov dl,byte ptr [workspace.s_u+2]

	shl ebx,16
	mov dh,byte ptr [workspace.s_v+2]

	mov al,byte ptr [workspace.d_u_x+2]
	mov esi,work.texture.base 		   

	mov ah,byte ptr [workspace.d_v_x+2]
	mov bl,[esi+edx] 

drawPixel:
	test bl,bl
	jz noPlot
		
; writes
; add on deltas
	mov [edi+ecx],bl
noPlot:
	add_d eax,workspace.fdu,direction ; two cycles

	adc_d dl,al,direction
	add_d ebx,ebp,direction

	adc_d dh,ah,direction
	inc_d ecx,direction

	mov bl,[esi+edx]	;agi
	jle_d drawPixel,direction
									  
lineDrawn:
	mov edx,workspace.minorX
	mov eax,workspace.xm_f

	add edx,workspace.d_&minorX			;2 cycles
	add eax,workspace.d_xm_f			;2 cycles

	sbb edi,edi
	mov workspace.xm_f,eax

	mov workspace.minorX,edx
	mov ebx,workspace.s_v

	mov eax,workspace.s_u
  	mov ecx,workspace.xm

	add ebx,[workspace.d_v_y_0+edi*8]	;2 cycles
	add eax,[workspace.d_u_y_0+edi*8]	;2 cycles

	mov workspace.s_v,ebx
	mov workspace.s_u,eax

	mov edi,workspace.scanAddress
	mov eax,workspace.&half&Count

	add ecx,workspace.d_xm				;2 cycles
   	add edi,work.colour.stride_b		;2 cycles

	mov workspace.xm,ecx
	mov workspace.scanAddress,edi
	
	dec eax

	mov workspace.&half&Count,eax
	jge drawLine
done:
endm

TriangleRender_T_I8_256 proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex
	
	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_T

TriangleRasterise_T_I8_256 label proc

	mov eax,work.colour.stride_b
	mov ebx,workspace.t_y

	dec ebx	
	mov edi,work.colour.base
	imul ebx

	add edi,eax

	dec edi
	mov workspace.scanAddress,edi
	mov eax,workspace.xm

	shl eax,16
	mov ebx,workspace.d_xm

	shl ebx,16
	mov workspace.xm_f,eax

	mov workspace.d_xm_f,ebx
	mov	eax,workspace.flip

	mov ebp,workspace.d_u_x
	shl ebp,16
	mov workspace.fdu,ebp
	mov ebp,workspace.d_v_x
	shl ebp,16

	test eax,eax
	jnz	drawRL

	DRAW_T_I8_256 x1,DRAW_LR,top
	DRAW_T_I8_256 x2,DRAW_LR,bottom
	ret
drawRL:

	DRAW_T_I8_256 x1,DRAW_RL,top
	DRAW_T_I8_256 x2,DRAW_RL,bottom
	ret
TriangleRender_T_I8_256 endp


;*******************************
; LIT LINEAR RASTERISER 256x256
;*******************************

majorScanAddress equ scratch0

DRAW_TI_I8_256 macro minorX,direction,half
	local drawPixel,drawLine,done,lineDrawn,noPlot
; height test

	mov ebx,workspace.&half&Count

	cmp ebx,0 
	jl done														  

; trapezoid setup

	mov eax,workspace.s_u
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
	jg_d lineDrawn,direction

drawPixel:
;z-buffer
	xor edx,edx ;/and edx,0ffffh
	xor ebx,ebx
	mov dl,byte ptr[workspace.c_u+2]

	mov esi,work.texture.base
	mov dh,byte ptr[workspace.c_v+2]
        mov bh,byte ptr[workspace.c_i+2]

	mov edi,work.shade_table
	mov bl,[esi+edx]

	mov dl,bl
	add ebx,edi

	test dl,dl
	mov edi,workspace.majorScanAddress

	mov bl,[ebx]
	jz noPlot

; writes
	mov [edi+ecx],bl
; add on deltas

noPlot:
;update z,a
        mov ebx,workspace.c_i

        mov edx,workspace.d_i_x

	add_d ebx,edx,direction

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

        mov ecx,workspace.s_i
        mov edx,[workspace.d_i_y_0+esi*8]

	add ecx,edx
        mov workspace.c_i,ecx
        mov workspace.s_i,ecx

	mov edi,workspace.scanAddress
    mov eax,work.colour.stride_b
	add edi,eax

	mov ecx,workspace.&half&Count
	mov workspace.scanAddress,edi

	dec ecx
	mov workspace.&half&Count,ecx
	jge drawLine
done:
endm


TriangleRender_TI_I8_256_FLAT proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_TI_FLAT

	jmp	TriangleRasterise_TI_I8_256

TriangleRender_TI_I8_256_FLAT endp


TriangleRender_TI_I8_256 proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_TI

TriangleRasterise_TI_I8_256 label proc

	mov eax,work.colour.stride_b
	mov ebx,workspace.t_y

	dec ebx	
	mov edi,work.colour.base
	imul ebx

	add edi,eax

	dec edi
	mov workspace.scanAddress,edi
	mov eax,workspace.xm

	shl eax,16
	mov ebx,workspace.d_xm

	shl ebx,16
	mov workspace.xm_f,eax

	mov workspace.d_xm_f,ebx
	mov	eax,workspace.flip

	test eax,eax
	jnz	drawRL

	DRAW_TI_I8_256 x1,DRAW_LR,top
	DRAW_TI_I8_256 x2,DRAW_LR,bottom
	ret

drawRL:
	DRAW_TI_I8_256 x1,DRAW_RL,top
	DRAW_TI_I8_256 x2,DRAW_RL,bottom
	ret
TriangleRender_TI_I8_256 endp

end
