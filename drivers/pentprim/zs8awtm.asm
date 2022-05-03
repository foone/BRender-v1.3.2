.586p
.model flat,c
include drv.inc
include fpwork.inc
include fpsetup.inc
include common.inc

		externdef c TriangleRasterise_TI_I8:proc

.code

; Z-Sort Arbitrary Width Textured


PER_SCAN_T macro half,wrap_flag,minorX
	local uPerLineNoWrapNegative,uPerLineNoWrapPositive,vPerLineNoWrapNegative,vPerLineNoWrapPositive
	local uAboveRetest,uBelowRetest,vAboveRetest,vBelowRetest

	mov ebp,workspace.xm_f
	mov edi,workspace.d_xm_f

	add ebp,edi
	mov eax,workspaceA.svf

	sbb edi,edi
	mov workspace.xm_f,ebp
	
	mov esi,edi
	mov edx,workspaceA.su

	add edx,[workspaceA.duy0+8*edi]
	add eax,[workspaceA.dvy0f+4*edi]

	rcl esi,1
	mov workspaceA.svf,eax

	mov workspace.c_v,eax
	mov eax,workspaceA.sv

	mov ebx,workspace.minorX
	mov ecx,workspace.xm

	add eax,[workspaceA.dvy0+8*esi]
	add ebx,workspace.d_&minorX

ifidni <wrap_flag>,<WRAPPED>
vBelowRetest:
	cmp eax,work.texture.base
	jae vPerLineNoWrapNegative
	add eax,work.texture._size
	jmp vBelowRetest
vPerLineNoWrapNegative:
vAboveRetest:
	cmp eax,workspaceA.vUpperBound
	jb vPerLineNoWrapPositive
	sub eax,work.texture._size
	jmp vAboveRetest
vPerLineNoWrapPositive:

uBelowRetest:
	cmp edx,0
	jge uPerLineNoWrapNegative
	add edx,workspaceA.uUpperBound
	jmp uBelowRetest
uPerLineNoWrapNegative:
uAboveRetest:
	cmp edx,workspaceA.uUpperBound
	jl uPerLineNoWrapPositive
	sub edx,workspaceA.uUpperBound
	jmp uAboveRetest
uPerLineNoWrapPositive:
endif

	mov workspaceA.sv,eax
	mov workspaceA.su,edx

	mov workspace.c_u,edx
	mov workspace.minorX,ebx

	mov edi,workspace.scanAddress
	mov edx,workspace.&half&Count

	add ecx,workspace.d_xm
	add edi,work.colour.stride_b

	mov workspace.xm,ecx
	mov workspace.scanAddress,edi

	dec edx

endm



DRAW_T_I8 macro minorX,direction,half,wrap_flag
	local drawPixel,drawLine,done,lineDrawn,noPlot,noCarry,returnAddress
	local uPerPixelNoWrapNegative,uPerPixelNoWrapPositive,vPerPixelNoWrapNegative,vPerPixelNoWrapPositive
	local uAboveRetest,uBelowRetest,vAboveRetest,vBelowRetest
; height test

	mov ebx,workspace.&half&Count
	lea	eax,returnAddress
	mov workspaceA.retAddress,eax

	cmp ebx,0
	jl done

	mov eax,workspaceA.su
	mov ebx,workspaceA.svf

	mov workspace.c_u,eax
	mov workspace.c_v,ebx

	mov ebx,workspace.minorX
	mov ecx,workspace.xm

drawLine:
	shr ebx,16
	mov edi,workspace.scanAddress

	shr ecx,16
	add edi,ebx

	sub ecx,ebx
ifidni <wrap_flag>,<WRAPPED>
	jg_d lineDrawn,direction
else
	jg_d half&ZeroWidthHandler_T,direction
endif

	mov esi,workspaceA.sv
	mov eax,workspaceA.su

	shr eax,16

drawPixel:

	mov bl,[esi+eax]

;update v
	mov edx,workspace.c_v
	mov eax,workspace.c_u

	add_d eax,workspaceA.dux,direction
	add_d edx,workspaceA.dvxf,direction

	sbb ebp,ebp
	mov workspace.c_v,edx

	test bl,bl
	jz noPlot

; writes
	mov [edi+ecx],bl
noPlot:

	add_d esi,[workspaceA.dvx+8*ebp],direction

ifidni <wrap_flag>,<WRAPPED>
uBelowRetest:
	cmp eax,0
	jge uPerPixelNoWrapNegative
	add eax,workspaceA.uUpperBound
	jmp uBelowRetest
uPerPixelNoWrapNegative:
uAboveRetest:
	cmp eax,workspaceA.uUpperBound
	jl uPerPixelNoWrapPositive
	sub eax,workspaceA.uUpperBound
	jmp uAboveRetest
uPerPixelNoWrapPositive:
endif
	mov workspace.c_u,eax

ifidni <wrap_flag>,<WRAPPED>
vBelowRetest:
	cmp esi,work.texture.base
	jae vPerPixelNoWrapNegative
	add esi,work.texture._size
	jmp vBelowRetest
vPerPixelNoWrapNegative:
vAboveRetest:
	cmp esi,workspaceA.vUpperBound
	jb vPerPixelNoWrapPositive
	sub esi,work.texture._size
	jmp vAboveRetest
vPerPixelNoWrapPositive:
endif
	sar eax,16

	inc_d ecx,direction
	jle_d drawPixel,direction

lineDrawn:
;perform per line updates

	PER_SCAN_T half,wrap_flag,minorX

returnAddress:
	mov workspace.&half&Count,edx
	jge drawLine

done:
endm


TriangleRender_T_I8 proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_T_ARBITRARY

	mov eax,work.texture.base
	mov ebx,workspaceA.sv

	add ebx,eax
	
	mov workspaceA.sv,ebx

	mov eax,work.colour.stride_b
	mov ebx,workspace.t_y

	dec ebx
	mov edi,work.colour.base

	imul ebx

	add edi,eax
	mov eax,workspace.xm

	shl eax,16
	dec edi

	mov	edx,workspaceA.flags
	mov ebx,workspace.d_xm

	shl ebx,16
	mov workspace.scanAddress,edi

	mov ecx,[jumpTable_T_I8+4*edx]
	mov workspace.depthAddress,ebp

	mov workspace.xm_f,eax
	mov workspace.d_xm_f,ebx

	jmp ecx

if 0 ; set to zero to force wrapped mode, 1 for non-wrapped mode.
ECHO NON-WRAPPED SPECIAL CASE ENABLED
Draw_T_I8_NWLR::
	DRAW_T_I8 x1,DRAW_LR,top,NON_WRAPPED
	DRAW_T_I8 x2,DRAW_LR,bottom,NON_WRAPPED
	ret

Draw_T_I8_NWRL::
	DRAW_T_I8 x1,DRAW_RL,top,NON_WRAPPED
	DRAW_T_I8 x2,DRAW_RL,bottom,NON_WRAPPED
	ret

else
ECHO NON-WRAPPED SPECIAL CASE HAS BEEN DISABLED !

Draw_T_I8_NWLR::
	DRAW_T_I8 x1,DRAW_LR,top,WRAPPED
	DRAW_T_I8 x2,DRAW_LR,bottom,WRAPPED
	ret

Draw_T_I8_NWRL::
	DRAW_T_I8 x1,DRAW_RL,top,WRAPPED
	DRAW_T_I8 x2,DRAW_RL,bottom,WRAPPED
	ret
endif
;Wraps

Draw_T_I8_DWLR::
	DRAW_T_I8 x1,DRAW_LR,top,WRAPPED
	DRAW_T_I8 x2,DRAW_LR,bottom,WRAPPED
	ret

Draw_T_I8_DWRL::
	DRAW_T_I8 x1,DRAW_RL,top,WRAPPED
	DRAW_T_I8 x2,DRAW_RL,bottom,WRAPPED
	ret

topZeroWidthHandler_T::
;perform per line updates

	PER_SCAN_T top,WRAPPED,x1

	mov ebp,workspaceA.retAddress
	jmp ebp


bottomZeroWidthHandler_T::
;perform per line updates

	PER_SCAN_T bottom,WRAPPED,x2

	mov ebp,workspaceA.retAddress
	jmp ebp


TriangleRender_T_I8 endp




; Arbitrary Width Lit Textured 



PER_SCAN_TI macro half,wrap_flag,minorX
	local uPerLineNoWrapNegative,uPerLineNoWrapPositive,vPerLineNoWrapNegative,vPerLineNoWrapPositive
	local uAboveRetest,uBelowRetest,vAboveRetest,vBelowRetest

	mov ebp,workspace.xm_f
	mov edi,workspace.d_xm_f

	add ebp,edi
	mov eax,workspaceA.svf

	sbb edi,edi
	mov workspace.xm_f,ebp
	
	mov esi,edi													
	mov ecx,workspace.s_i

	mov edx,workspaceA.su
	mov ebp,[workspace.d_i_y_0+8*edi]

	add edx,[workspaceA.duy0+8*edi]
	add eax,[workspaceA.dvy0f+4*edi]

	rcl esi,1
	mov workspaceA.svf,eax

	mov workspace.c_v,eax
	add ecx,ebp

	mov eax,workspaceA.sv
	mov workspace.s_i,ecx

	mov workspace.c_i,ecx
	nop

	mov ebx,workspace.minorX
	mov ecx,workspace.xm

	add eax,[workspaceA.dvy0+8*esi]
	add ebx,workspace.d_&minorX

ifidni <wrap_flag>,<WRAPPED>
vBelowRetest:
	cmp eax,work.texture.base
	jae vPerLineNoWrapNegative
	add eax,work.texture._size
	jmp vBelowRetest
vPerLineNoWrapNegative:
vAboveRetest:
	cmp eax,workspaceA.vUpperBound
	jb vPerLineNoWrapPositive
	sub eax,work.texture._size
	jmp vAboveRetest
vPerLineNoWrapPositive:

uBelowRetest:
	cmp edx,0
	jge uPerLineNoWrapNegative
	add edx,workspaceA.uUpperBound
	jmp uBelowRetest
uPerLineNoWrapNegative:
uAboveRetest:
	cmp edx,workspaceA.uUpperBound
	jl uPerLineNoWrapPositive
	sub edx,workspaceA.uUpperBound
	jmp uAboveRetest
uPerLineNoWrapPositive:
endif

	mov workspaceA.sv,eax
	mov workspaceA.su,edx

	mov workspace.c_u,edx
	mov workspace.minorX,ebx

	mov edi,workspace.scanAddress
	mov edx,workspace.&half&Count

	add ecx,workspace.d_xm
	add edi,work.colour.stride_b

	mov workspace.xm,ecx
	mov workspace.scanAddress,edi

	dec edx


endm



DRAW_TI_I8 macro minorX,direction,half,wrap_flag
	local drawPixel,drawLine,done,lineDrawn,noPlot,noCarry,returnAddress
	local uPerPixelNoWrapNegative,uPerPixelNoWrapPositive,vPerPixelNoWrapNegative,vPerPixelNoWrapPositive
	local uAboveRetest,uBelowRetest,vAboveRetest,vBelowRetest
; height test

	mov ebx,workspace.&half&Count
	lea	eax,returnAddress
	mov workspaceA.retAddress,eax

	cmp ebx,0
	jl done

	mov eax,workspaceA.su
	mov ebx,workspaceA.svf

	mov ecx,workspace.s_i
	mov workspace.c_u,eax

	mov workspace.c_v,ebx
	mov workspace.c_i,ecx

	mov ebx,workspace.minorX
	mov ecx,workspace.xm

drawLine:
	shr ebx,16
	mov edi,workspace.scanAddress

	shr ecx,16
	add edi,ebx

	sub ecx,ebx
ifidni <wrap_flag>,<WRAPPED>
	jg_d lineDrawn,direction
else
	jg_d half&ZeroWidthHandler_TI,direction
endif

	mov esi,workspaceA.sv
	mov eax,workspaceA.su

	shr eax,16

drawPixel:
	mov bl,[esi+eax]
	mov eax,workspace.c_u

	mov edx,workspace.c_v

	add_d eax,workspaceA.dux,direction
	add_d edx,workspaceA.dvxf,direction

	sbb ebp,ebp
	mov workspace.c_v,edx

	mov edx,workspace.c_i

	add_d edx,workspace.d_i_x,direction
	add_d esi,[workspaceA.dvx+8*ebp],direction

ifidni <wrap_flag>,<WRAPPED>
uBelowRetest:
	cmp eax,0
	jge uPerPixelNoWrapNegative
	add eax,workspaceA.uUpperBound
	jmp uBelowRetest
uPerPixelNoWrapNegative:
uAboveRetest:
	cmp eax,workspaceA.uUpperBound
	jl uPerPixelNoWrapPositive
	sub eax,workspaceA.uUpperBound
	jmp uAboveRetest
uPerPixelNoWrapPositive:
endif

ifidni <wrap_flag>,<WRAPPED>
vBelowRetest:
	cmp esi,work.texture.base
	jae vPerPixelNoWrapNegative
	add esi,work.texture._size
	jmp vBelowRetest
vPerPixelNoWrapNegative:
vAboveRetest:
	cmp esi,workspaceA.vUpperBound
	jb vPerPixelNoWrapPositive
	sub esi,work.texture._size
	jmp vAboveRetest
vPerPixelNoWrapPositive:
endif

	mov ebp,work.shade_table
	mov bh,byte ptr workspace.c_i+2

	mov workspace.c_u,eax
	mov workspace.c_i,edx

	sar eax,16
	test bl,bl

	mov bl,[ebp+ebx]
	jz noPlot

; writes
	mov [edi+ecx],bl
noPlot:

	inc_d ecx,direction
	jle_d drawPixel,direction

lineDrawn:
;perform per line updates

	PER_SCAN_TI half,wrap_flag,minorX

returnAddress:
	mov workspace.&half&Count,edx
	jge drawLine

done:
endm


TriangleRender_TI_I8_FLAT proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_TI_ARBITRARY_FLAT

	jmp	TriangleRasterise_TI_I8

TriangleRender_TI_I8_FLAT endp


TriangleRender_TI_I8 proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_TI_ARBITRARY

TriangleRasterise_TI_I8 label proc

	mov eax,work.texture.base
	mov ebx,workspaceA.sv

	add ebx,eax
	
	mov workspaceA.sv,ebx

	mov eax,work.colour.stride_b
	mov ebx,workspace.t_y

	dec ebx
	mov edi,work.colour.base

	imul ebx

	add edi,eax
	mov eax,workspace.xm

	shl eax,16
	dec edi

	mov	edx,workspaceA.flags
	mov ebx,workspace.d_xm

	shl ebx,16
	mov workspace.scanAddress,edi

	mov ecx,[jumpTable_TI_I8+4*edx]
	mov workspace.depthAddress,ebp

	mov workspace.xm_f,eax
	mov workspace.d_xm_f,ebx

	jmp ecx

if 0 ; set to zero to force wrapped mode, 1 for non-wrapped mode.
ECHO NON-WRAPPED SPECIAL CASE ENABLED
Draw_TI_I8_NWLR::
	DRAW_TI_I8 x1,DRAW_LR,top,NON_WRAPPED
	DRAW_TI_I8 x2,DRAW_LR,bottom,NON_WRAPPED
	ret

Draw_TI_I8_NWRL::
	DRAW_TI_I8 x1,DRAW_RL,top,NON_WRAPPED
	DRAW_TI_I8 x2,DRAW_RL,bottom,NON_WRAPPED
	ret

else
ECHO NON-WRAPPED SPECIAL CASE HAS BEEN DISABLED !

Draw_TI_I8_NWLR::
	DRAW_TI_I8 x1,DRAW_LR,top,WRAPPED
	DRAW_TI_I8 x2,DRAW_LR,bottom,WRAPPED
	ret

Draw_TI_I8_NWRL::
	DRAW_TI_I8 x1,DRAW_RL,top,WRAPPED
	DRAW_TI_I8 x2,DRAW_RL,bottom,WRAPPED
	ret
endif
;Wraps

Draw_TI_I8_DWLR::
	DRAW_TI_I8 x1,DRAW_LR,top,WRAPPED
	DRAW_TI_I8 x2,DRAW_LR,bottom,WRAPPED
	ret

Draw_TI_I8_DWRL::
	DRAW_TI_I8 x1,DRAW_RL,top,WRAPPED
	DRAW_TI_I8 x2,DRAW_RL,bottom,WRAPPED
	ret

topZeroWidthHandler_TI::
;perform per line updates

	PER_SCAN_TI top,WRAPPED,x1

	mov ebp,workspaceA.retAddress
	jmp ebp


bottomZeroWidthHandler_TI::
;perform per line updates

	PER_SCAN_TI bottom,WRAPPED,x2

	mov ebp,workspaceA.retAddress
	jmp ebp


TriangleRender_TI_I8 endp


.data

jumpTable_T_I8 dword Draw_T_I8_NWLR
dword	Draw_T_I8_NWRL
dword	Draw_T_I8_DWLR
dword	Draw_T_I8_DWRL

jumpTable_TI_I8 dword Draw_TI_I8_NWLR
dword	Draw_TI_I8_NWRL
dword	Draw_TI_I8_DWLR
dword	Draw_TI_I8_DWRL


END
