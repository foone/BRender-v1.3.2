.586p
.model flat,c
include drv.inc
include fpwork.inc
include fpsetup.inc
include common.inc

		externdef c TriangleRasterise_ZTI_I8_D16:proc
        externdef c TriangleRasterise_ZTIF_I8_D16:proc
        externdef c TriangleRasterise_ZTIB_I8_D16:proc
        externdef c TriangleRasterise_ZTIF_BI8_D16:proc

.code

; Z-Buffered Arbitrary Width Textured

PER_SCAN_ZT macro half,wrap_flag,minorX
	local uPerLineNoWrapNegative,uPerLineNoWrapPositive,vPerLineNoWrapNegative,vPerLineNoWrapPositive
	local uAboveRetest,uBelowRetest,vAboveRetest,vBelowRetest

	mov ebp,workspace.xm_f
	mov edi,workspace.d_xm_f

	add ebp,edi
	mov eax,workspaceA.svf

	sbb edi,edi
	mov workspace.xm_f,ebp
	
	mov esi,edi													
	mov ecx,workspace.s_z

	mov edx,workspaceA.su
	mov ebp,[workspace.d_z_y_0+8*edi]

	add edx,[workspaceA.duy0+8*edi]
	add eax,[workspaceA.dvy0f+4*edi]

	rcl esi,1
	mov workspaceA.svf,eax

	mov workspace.c_v,eax
	add ecx,ebp

	mov eax,workspaceA.sv
	mov workspace.s_z,ecx

	mov workspace.c_z,ecx
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
	mov ebp,workspace.depthAddress

	add ebp,work.depth.stride_b ;two cycles
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
	mov workspace.depthAddress,ebp


endm



DRAW_ZT_I8 macro minorX,direction,half,wrap_flag,fogging,blend
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

	mov ecx,workspace.s_z
	mov workspace.c_u,eax

	mov workspace.c_v,ebx
	mov workspace.c_z,ecx

	mov ebx,workspace.minorX
	mov ecx,workspace.xm


drawLine:
	mov ebp,workspace.depthAddress

	shr ebx,16
	mov edi,workspace.scanAddress

	shr ecx,16
	add edi,ebx

	sub ecx,ebx
ifidni <wrap_flag>,<WRAPPED>
	jg_d lineDrawn,direction
else
if blend
if fogging
	jg_d half&ZeroWidthHandler_ZTFB,direction
else
	jg_d half&ZeroWidthHandler_ZTB,direction
endif
else
if fogging
	jg_d half&ZeroWidthHandler_ZTF,direction
else
	jg_d half&ZeroWidthHandler_ZT,direction
endif
endif
endif

	mov esi,workspaceA.sv
	mov eax,workspaceA.su

	shr eax,16
	lea ebp,[ebp+2*ebx]

drawPixel:
	mov bx,[ebp+2*ecx]
	mov dx,word ptr workspace.c_z+2

	cmp dx,bx
	ja noPlot

	mov bl,[esi+eax]

	test bl,bl
	jz noPlot

; writes
if fogging
if blend
    mov eax,work.fog_table
    mov bh,dh

    mov bl,[ebx+eax]
    mov eax,work.blend_table

    mov bh,[edi+ecx]

    mov bl,[eax+ebx]
    
    mov [edi+ecx],bl
else
    mov eax,work.fog_table
    mov bh,dh

    mov [ebp+2*ecx],dx
    mov bl,[ebx+eax]

    mov [edi+ecx],bl
endif
else
if blend
    mov eax,work.blend_table
    mov bh,[edi+ecx]

    mov bl,[eax+ebx]
    
    mov [edi+ecx],bl
else
    mov [ebp+2*ecx],dx
	mov [edi+ecx],bl
endif
endif
noPlot:

	mov edx,workspace.c_v
	add_d edx,workspaceA.dvxf,direction
	mov workspace.c_v,edx

	sbb edx,edx

	add_d esi,[workspaceA.dvx+8*edx],direction
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

	mov eax,workspace.c_u
	add_d eax,workspaceA.dux,direction
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
	sar eax,16

	mov edx,workspace.c_z
	add_d edx,workspace.d_z_x,direction
	mov workspace.c_z,edx
	shr edx,16

	inc_d ecx,direction
	jle_d drawPixel,direction




lineDrawn:
;perform per line updates

	PER_SCAN_ZT half,wrap_flag,minorX

returnAddress:
	mov workspace.&half&Count,edx
	jge drawLine

done:
endm


	if PARTS and PART_8Z

TriangleRender_ZT_I8_D16 proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_ZT_ARBITRARY

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

	mov eax,work.texture.base
	mov ebx,workspaceA.sv

	add ebx,eax
	mov eax,workspace.xm
	
	shl eax,16
	mov workspaceA.sv,ebx

	mov	edx,workspaceA.flags
	mov ebx,workspace.d_xm

	mov ecx,[jumpTable_ZT_I8+4*edx]

	shl ebx,16
	mov workspace.xm_f,eax

	mov workspace.d_xm_f,ebx
	jmp ecx

if 0 ; set to zero to force wrapped mode, 1 for non-wrapped mode.
ECHO NON-WRAPPED SPECIAL CASE ENABLED
Draw_ZT_I8_NWLR::
	DRAW_ZT_I8 x1,DRAW_LR,top,NON_WRAPPED
	DRAW_ZT_I8 x2,DRAW_LR,bottom,NON_WRAPPED
	ret

Draw_ZT_I8_NWRL::
	DRAW_ZT_I8 x1,DRAW_RL,top,NON_WRAPPED
	DRAW_ZT_I8 x2,DRAW_RL,bottom,NON_WRAPPED
	ret

else
ECHO NON-WRAPPED SPECIAL CASE HAS BEEN DISABLED !

Draw_ZT_I8_NWLR::
	DRAW_ZT_I8 x1,DRAW_LR,top,WRAPPED
	DRAW_ZT_I8 x2,DRAW_LR,bottom,WRAPPED
	ret

Draw_ZT_I8_NWRL::
	DRAW_ZT_I8 x1,DRAW_RL,top,WRAPPED
	DRAW_ZT_I8 x2,DRAW_RL,bottom,WRAPPED
	ret
endif
;Wraps

Draw_ZT_I8_DWLR::
	DRAW_ZT_I8 x1,DRAW_LR,top,WRAPPED
	DRAW_ZT_I8 x2,DRAW_LR,bottom,WRAPPED
	ret

Draw_ZT_I8_DWRL::
	DRAW_ZT_I8 x1,DRAW_RL,top,WRAPPED
	DRAW_ZT_I8 x2,DRAW_RL,bottom,WRAPPED
	ret

topZeroWidthHandler_ZT::
;perform per line updates

	PER_SCAN_ZT top,WRAPPED,x1

	mov ebp,workspaceA.retAddress
	jmp ebp


bottomZeroWidthHandler_ZT::
;perform per line updates

	PER_SCAN_ZT bottom,WRAPPED,x2

	mov ebp,workspaceA.retAddress
	jmp ebp


TriangleRender_ZT_I8_D16 endp

	endif

	if PARTS and PART_8Z_FOG

TriangleRender_ZTF_I8_D16 proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_ZT_ARBITRARY

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

	mov eax,work.texture.base
	mov ebx,workspaceA.sv

	add ebx,eax
	mov eax,workspace.xm
	
	shl eax,16
	mov workspaceA.sv,ebx

	mov	edx,workspaceA.flags
	mov ebx,workspace.d_xm

    mov ecx,[jumpTable_ZTF_I8+4*edx]

	shl ebx,16
	mov workspace.xm_f,eax

	mov workspace.d_xm_f,ebx
	jmp ecx

if 0 ; set to zero to force wrapped mode, 1 for non-wrapped mode.
ECHO NON-WRAPPED SPECIAL CASE ENABLED
Draw_ZTF_I8_NWLR::
    DRAW_ZT_I8 x1,DRAW_LR,top,NON_WRAPPED,1
    DRAW_ZT_I8 x2,DRAW_LR,bottom,NON_WRAPPED,1
	ret

Draw_ZTF_I8_NWRL::
    DRAW_ZT_I8 x1,DRAW_RL,top,NON_WRAPPED,1
    DRAW_ZT_I8 x2,DRAW_RL,bottom,NON_WRAPPED,1
	ret

else
ECHO NON-WRAPPED SPECIAL CASE HAS BEEN DISABLED !

Draw_ZTF_I8_NWLR::
    DRAW_ZT_I8 x1,DRAW_LR,top,WRAPPED,1
    DRAW_ZT_I8 x2,DRAW_LR,bottom,WRAPPED,1
	ret

Draw_ZTF_I8_NWRL::
    DRAW_ZT_I8 x1,DRAW_RL,top,WRAPPED,1
    DRAW_ZT_I8 x2,DRAW_RL,bottom,WRAPPED,1
	ret
endif
;Wraps

Draw_ZTF_I8_DWLR::
    DRAW_ZT_I8 x1,DRAW_LR,top,WRAPPED,1
    DRAW_ZT_I8 x2,DRAW_LR,bottom,WRAPPED,1
	ret

Draw_ZTF_I8_DWRL::
    DRAW_ZT_I8 x1,DRAW_RL,top,WRAPPED,1
    DRAW_ZT_I8 x2,DRAW_RL,bottom,WRAPPED,1
	ret

topZeroWidthHandler_ZTF::
;perform per line updates

	PER_SCAN_ZT top,WRAPPED,x1

	mov ebp,workspaceA.retAddress
	jmp ebp


bottomZeroWidthHandler_ZTF::
;perform per line updates

	PER_SCAN_ZT bottom,WRAPPED,x2

	mov ebp,workspaceA.retAddress
	jmp ebp


TriangleRender_ZTF_I8_D16 endp

	endif

	if PARTS and PART_8Z_BLEND

TriangleRender_ZTB_I8_D16 proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_ZT_ARBITRARY

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

	mov eax,work.texture.base
	mov ebx,workspaceA.sv

	add ebx,eax
	mov eax,workspace.xm
	
	shl eax,16
	mov workspaceA.sv,ebx

	mov	edx,workspaceA.flags
	mov ebx,workspace.d_xm

    mov ecx,[jumpTable_ZTB_I8+4*edx]

	shl ebx,16
	mov workspace.xm_f,eax

	mov workspace.d_xm_f,ebx
	jmp ecx

if 0 ; set to zero to force wrapped mode, 1 for non-wrapped mode.
ECHO NON-WRAPPED SPECIAL CASE ENABLED
Draw_ZTB_I8_NWLR::
    DRAW_ZT_I8 x1,DRAW_LR,top,NON_WRAPPED,0,1
    DRAW_ZT_I8 x2,DRAW_LR,bottom,NON_WRAPPED,0,1
	ret

Draw_ZTB_I8_NWRL::
    DRAW_ZT_I8 x1,DRAW_RL,top,NON_WRAPPED,0,1
    DRAW_ZT_I8 x2,DRAW_RL,bottom,NON_WRAPPED,0,1
	ret

else
ECHO NON-WRAPPED SPECIAL CASE HAS BEEN DISABLED !

Draw_ZTB_I8_NWLR::
    DRAW_ZT_I8 x1,DRAW_LR,top,WRAPPED,0,1
    DRAW_ZT_I8 x2,DRAW_LR,bottom,WRAPPED,0,1
	ret

Draw_ZTB_I8_NWRL::
    DRAW_ZT_I8 x1,DRAW_RL,top,WRAPPED,0,1
    DRAW_ZT_I8 x2,DRAW_RL,bottom,WRAPPED,0,1
	ret
endif
;Wraps

Draw_ZTB_I8_DWLR::
    DRAW_ZT_I8 x1,DRAW_LR,top,WRAPPED,0,1
    DRAW_ZT_I8 x2,DRAW_LR,bottom,WRAPPED,0,1
	ret

Draw_ZTB_I8_DWRL::
    DRAW_ZT_I8 x1,DRAW_RL,top,WRAPPED,0,1
    DRAW_ZT_I8 x2,DRAW_RL,bottom,WRAPPED,0,1
	ret

topZeroWidthHandler_ZTB::
;perform per line updates

	PER_SCAN_ZT top,WRAPPED,x1

	mov ebp,workspaceA.retAddress
	jmp ebp


bottomZeroWidthHandler_ZTB::
;perform per line updates

	PER_SCAN_ZT bottom,WRAPPED,x2

	mov ebp,workspaceA.retAddress
	jmp ebp


TriangleRender_ZTB_I8_D16 endp

	endif

	if PARTS and PART_8Z_FOG_BLEND

TriangleRender_ZTFB_I8_D16 proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_ZT_ARBITRARY

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

	mov eax,work.texture.base
	mov ebx,workspaceA.sv

	add ebx,eax
	mov eax,workspace.xm
	
	shl eax,16
	mov workspaceA.sv,ebx

	mov	edx,workspaceA.flags
	mov ebx,workspace.d_xm

    mov ecx,[jumpTable_ZTFB_I8+4*edx]

	shl ebx,16
	mov workspace.xm_f,eax

	mov workspace.d_xm_f,ebx
	jmp ecx

if 0 ; set to zero to force wrapped mode, 1 for non-wrapped mode.
ECHO NON-WRAPPED SPECIAL CASE ENABLED
Draw_ZTFB_I8_NWLR::
    DRAW_ZT_I8 x1,DRAW_LR,top,NON_WRAPPED,1,1
    DRAW_ZT_I8 x2,DRAW_LR,bottom,NON_WRAPPED,1,1
	ret

Draw_ZTFB_I8_NWRL::
    DRAW_ZT_I8 x1,DRAW_RL,top,NON_WRAPPED,1,1
    DRAW_ZT_I8 x2,DRAW_RL,bottom,NON_WRAPPED,1,1
	ret

else
ECHO NON-WRAPPED SPECIAL CASE HAS BEEN DISABLED !

Draw_ZTFB_I8_NWLR::
    DRAW_ZT_I8 x1,DRAW_LR,top,WRAPPED,1,1
    DRAW_ZT_I8 x2,DRAW_LR,bottom,WRAPPED,1,1
	ret

Draw_ZTFB_I8_NWRL::
    DRAW_ZT_I8 x1,DRAW_RL,top,WRAPPED,1,1
    DRAW_ZT_I8 x2,DRAW_RL,bottom,WRAPPED,1,1
	ret
endif
;Wraps

Draw_ZTFB_I8_DWLR::
    DRAW_ZT_I8 x1,DRAW_LR,top,WRAPPED,1,1
    DRAW_ZT_I8 x2,DRAW_LR,bottom,WRAPPED,1,1
	ret

Draw_ZTFB_I8_DWRL::
    DRAW_ZT_I8 x1,DRAW_RL,top,WRAPPED,1,1
    DRAW_ZT_I8 x2,DRAW_RL,bottom,WRAPPED,1,1
	ret

topZeroWidthHandler_ZTFB::
;perform per line updates

	PER_SCAN_ZT top,WRAPPED,x1

	mov ebp,workspaceA.retAddress
	jmp ebp


bottomZeroWidthHandler_ZTFB::
;perform per line updates

	PER_SCAN_ZT bottom,WRAPPED,x2

	mov ebp,workspaceA.retAddress
	jmp ebp


TriangleRender_ZTFB_I8_D16 endp

	endif


; Arbitrary Width Lit Textured

PER_SCAN_ZTI macro half,wrap_flag,minorX
	local uPerLineNoWrapNegative,uPerLineNoWrapPositive,vPerLineNoWrapNegative,vPerLineNoWrapPositive
	local uAboveRetest,uBelowRetest,vAboveRetest,vBelowRetest

	mov ebp,workspace.xm_f
	mov edi,workspace.d_xm_f

	add ebp,edi
	mov eax,workspaceA.svf

	sbb edi,edi
	mov ecx,workspace.s_i

	mov workspace.xm_f,ebp
	mov esi,edi													

	add ecx,[workspace.d_i_y_0+8*edi]
	mov edx,workspaceA.su

	mov workspace.s_i,ecx
	mov ebp,[workspace.d_z_y_0+8*edi]

	mov workspace.c_i,ecx
	mov ecx,workspace.s_z

	add edx,[workspaceA.duy0+8*edi]
	add eax,[workspaceA.dvy0f+4*edi]

	rcl esi,1
	mov workspaceA.svf,eax

	mov workspace.c_v,eax
	add ecx,ebp

	mov eax,workspaceA.sv
	mov workspace.s_z,ecx

	mov workspace.c_z,ecx
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
	mov ebp,workspace.depthAddress

	add ebp,work.depth.stride_b ;two cycles
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
	mov workspace.depthAddress,ebp


endm



DRAW_ZTI_I8 macro minorX,direction,half,wrap_flag,fogging,blend
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

	mov ecx,workspace.s_z
	mov workspace.c_u,eax

	mov workspace.c_v,ebx
	mov ebx,workspace.s_i

	mov workspace.c_z,ecx
	mov workspace.c_i,ebx

	mov ebx,workspace.minorX
	mov ecx,workspace.xm


drawLine:
	mov ebp,workspace.depthAddress

	shr ebx,16
	mov edi,workspace.scanAddress

	shr ecx,16
	add edi,ebx

	sub ecx,ebx
ifidni <wrap_flag>,<WRAPPED>
	jg_d lineDrawn,direction
else
if blend
if fogging
	jg_d half&ZeroWidthHandler_ZTIFB,direction
else
	jg_d half&ZeroWidthHandler_ZTIB,direction
endif
else
if fogging
	jg_d half&ZeroWidthHandler_ZTIF,direction
else
	jg_d half&ZeroWidthHandler_ZTI,direction
endif
endif
endif

	mov esi,workspaceA.sv
	mov eax,workspaceA.su

	shr eax,16
	lea ebp,[ebp+2*ebx]

drawPixel:
	mov bx,word ptr [ebp+2*ecx]
	mov dx,word ptr workspace.c_z+2

	cmp dx,bx
	ja noPlot

	mov bl,byte ptr[esi+eax]
	mov eax,work.shade_table

	mov bh,byte ptr workspace.c_i+2

	test bl,bl
	jz noPlot

; writes
if fogging
if blend
	mov bl,byte ptr [eax+ebx]
    mov eax,work.fog_table

    mov bh,dh

    mov bl,[eax+ebx]
    mov eax,work.blend_table

    mov bh,[edi+ecx]

    mov bl,[eax+ebx]

	mov [edi+ecx],bl
else
	mov bl,byte ptr [eax+ebx]
    mov eax,work.fog_table

    mov bh,dh

	mov [ebp+2*ecx],dx

    mov bl,[eax+ebx]

	mov [edi+ecx],bl
endif
else
if blend
	mov bl,byte ptr [eax+ebx]
    mov eax,work.blend_table

    mov bh,[edi+ecx]

    mov bl,[eax+ebx]

	mov [edi+ecx],bl
else
	mov bl,byte ptr [eax+ebx]

	mov [ebp+2*ecx],dx
	mov [edi+ecx],bl
endif
endif
noPlot:

	mov eax,workspace.c_i
	mov edx,workspace.c_v

	add_d eax,workspace.d_i_x,direction
	add_d edx,workspaceA.dvxf,direction

	mov workspace.c_i,eax
	mov workspace.c_v,edx

	sbb edx,edx

	add_d esi,[workspaceA.dvx+8*edx],direction
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

	mov eax,workspace.c_u
	add_d eax,workspaceA.dux,direction
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
	sar eax,16

	mov edx,workspace.c_z
	add_d edx,workspace.d_z_x,direction
	mov workspace.c_z,edx
	shr edx,16

	inc_d ecx,direction
	jle_d drawPixel,direction


lineDrawn:
;perform per line updates

	PER_SCAN_ZTI half,wrap_flag,minorX

returnAddress:
	mov workspace.&half&Count,edx
	jge drawLine

done:
endm


	if PARTS and PART_8Z

TriangleRender_ZTI_I8_D16_FLAT proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_ZTI_ARBITRARY_FLAT

	jmp	TriangleRasterise_ZTI_I8_D16

TriangleRender_ZTI_I8_D16_FLAT endp


TriangleRender_ZTI_I8_D16 proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_ZTI_ARBITRARY

TriangleRasterise_ZTI_I8_D16 label proc

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

	mov eax,work.texture.base
	mov ebx,workspaceA.sv

	add ebx,eax
	mov eax,workspace.xm
	
	shl eax,16
	mov workspaceA.sv,ebx

	mov	edx,workspaceA.flags
	mov ebx,workspace.d_xm

	mov ecx,[jumpTable_ZTI_I8+4*edx]

	shl ebx,16
	mov workspace.xm_f,eax

	mov workspace.d_xm_f,ebx
	jmp ecx

if 0 ; set to zero to force wrapped mode, 1 for non-wrapped mode.
ECHO NON-WRAPPED SPECIAL CASE ENABLED
Draw_ZTI_I8_NWLR::
	DRAW_ZTI_I8 x1,DRAW_LR,top,NON_WRAPPED
	DRAW_ZTI_I8 x2,DRAW_LR,bottom,NON_WRAPPED
	ret

Draw_ZTI_I8_NWRL::
	DRAW_ZTI_I8 x1,DRAW_RL,top,NON_WRAPPED
	DRAW_ZTI_I8 x2,DRAW_RL,bottom,NON_WRAPPED
	ret

else
ECHO NON-WRAPPED SPECIAL CASE HAS BEEN DISABLED !

Draw_ZTI_I8_NWLR::
	DRAW_ZTI_I8 x1,DRAW_LR,top,WRAPPED
	DRAW_ZTI_I8 x2,DRAW_LR,bottom,WRAPPED
	ret

Draw_ZTI_I8_NWRL::
	DRAW_ZTI_I8 x1,DRAW_RL,top,WRAPPED
	DRAW_ZTI_I8 x2,DRAW_RL,bottom,WRAPPED
	ret
endif
;Wraps

Draw_ZTI_I8_DWLR::
	DRAW_ZTI_I8 x1,DRAW_LR,top,WRAPPED
	DRAW_ZTI_I8 x2,DRAW_LR,bottom,WRAPPED
	ret

Draw_ZTI_I8_DWRL::
	DRAW_ZTI_I8 x1,DRAW_RL,top,WRAPPED
	DRAW_ZTI_I8 x2,DRAW_RL,bottom,WRAPPED
	ret

topZeroWidthHandler_ZTI::
;perform per line updates

	PER_SCAN_ZTI top,WRAPPED,x1

	mov ebp,workspaceA.retAddress
	jmp ebp


bottomZeroWidthHandler_ZTI::
;perform per line updates

	PER_SCAN_ZTI bottom,WRAPPED,x2

	mov ebp,workspaceA.retAddress
	jmp ebp

TriangleRender_ZTI_I8_D16 endp

	endif

	if PARTS and PART_8Z_FOG

TriangleRender_ZTIF_I8_D16_FLAT proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_ZTI_ARBITRARY_FLAT

	jmp	TriangleRasterise_ZTIF_I8_D16

TriangleRender_ZTIF_I8_D16_FLAT endp


TriangleRender_ZTIF_I8_D16 proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_ZTI_ARBITRARY

TriangleRasterise_ZTIF_I8_D16 label proc

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

	mov eax,work.texture.base
	mov ebx,workspaceA.sv

	add ebx,eax
	mov eax,workspace.xm
	
	shl eax,16
	mov workspaceA.sv,ebx

	mov	edx,workspaceA.flags
	mov ebx,workspace.d_xm

    mov ecx,[jumpTable_ZTIF_I8+4*edx]

	shl ebx,16
	mov workspace.xm_f,eax

	mov workspace.d_xm_f,ebx
	jmp ecx

if 0 ; set to zero to force wrapped mode, 1 for non-wrapped mode.
ECHO NON-WRAPPED SPECIAL CASE ENABLED
Draw_ZTIF_I8_NWLR::
    DRAW_ZTI_I8 x1,DRAW_LR,top,NON_WRAPPED,1
    DRAW_ZTI_I8 x2,DRAW_LR,bottom,NON_WRAPPED,1
	ret

Draw_ZTIF_I8_NWRL::
    DRAW_ZTI_I8 x1,DRAW_RL,top,NON_WRAPPED,1
    DRAW_ZTI_I8 x2,DRAW_RL,bottom,NON_WRAPPED,1
	ret

else
ECHO NON-WRAPPED SPECIAL CASE HAS BEEN DISABLED !

Draw_ZTIF_I8_NWLR::
    DRAW_ZTI_I8 x1,DRAW_LR,top,WRAPPED,1
    DRAW_ZTI_I8 x2,DRAW_LR,bottom,WRAPPED,1
	ret

Draw_ZTIF_I8_NWRL::
    DRAW_ZTI_I8 x1,DRAW_RL,top,WRAPPED,1
    DRAW_ZTI_I8 x2,DRAW_RL,bottom,WRAPPED,1
	ret
endif
;Wraps

Draw_ZTIF_I8_DWLR::
    DRAW_ZTI_I8 x1,DRAW_LR,top,WRAPPED,1
    DRAW_ZTI_I8 x2,DRAW_LR,bottom,WRAPPED,1
	ret

Draw_ZTIF_I8_DWRL::
    DRAW_ZTI_I8 x1,DRAW_RL,top,WRAPPED,1
    DRAW_ZTI_I8 x2,DRAW_RL,bottom,WRAPPED,1
	ret

topZeroWidthHandler_ZTIF::
;perform per line updates

	PER_SCAN_ZTI top,WRAPPED,x1

	mov ebp,workspaceA.retAddress
	jmp ebp


bottomZeroWidthHandler_ZTIF::
;perform per line updates

	PER_SCAN_ZTI bottom,WRAPPED,x2

	mov ebp,workspaceA.retAddress
	jmp ebp


TriangleRender_ZTIF_I8_D16 endp

	endif

	if PARTS and PART_8Z_BLEND

TriangleRender_ZTIB_I8_D16_FLAT proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_ZTI_ARBITRARY_FLAT

	jmp	TriangleRasterise_ZTIB_I8_D16

TriangleRender_ZTIB_I8_D16_FLAT endp


TriangleRender_ZTIB_I8_D16 proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_ZTI_ARBITRARY

TriangleRasterise_ZTIB_I8_D16 label proc

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

	mov eax,work.texture.base
	mov ebx,workspaceA.sv

	add ebx,eax
	mov eax,workspace.xm
	
	shl eax,16
	mov workspaceA.sv,ebx

	mov	edx,workspaceA.flags
	mov ebx,workspace.d_xm

    mov ecx,[jumpTable_ZTIB_I8+4*edx]

	shl ebx,16
	mov workspace.xm_f,eax

	mov workspace.d_xm_f,ebx
	jmp ecx

if 0 ; set to zero to force wrapped mode, 1 for non-wrapped mode.
ECHO NON-WRAPPED SPECIAL CASE ENABLED
Draw_ZTIB_I8_NWLR::
    DRAW_ZTI_I8 x1,DRAW_LR,top,NON_WRAPPED,0,1
    DRAW_ZTI_I8 x2,DRAW_LR,bottom,NON_WRAPPED,0,1
	ret

Draw_ZTIB_I8_NWRL::
    DRAW_ZTI_I8 x1,DRAW_RL,top,NON_WRAPPED,0,1
    DRAW_ZTI_I8 x2,DRAW_RL,bottom,NON_WRAPPED,0,1
	ret

else
ECHO NON-WRAPPED SPECIAL CASE HAS BEEN DISABLED !

Draw_ZTIB_I8_NWLR::
    DRAW_ZTI_I8 x1,DRAW_LR,top,WRAPPED,0,1
    DRAW_ZTI_I8 x2,DRAW_LR,bottom,WRAPPED,0,1
	ret

Draw_ZTIB_I8_NWRL::
    DRAW_ZTI_I8 x1,DRAW_RL,top,WRAPPED,0,1
    DRAW_ZTI_I8 x2,DRAW_RL,bottom,WRAPPED,0,1
	ret
endif
;Wraps

Draw_ZTIB_I8_DWLR::
    DRAW_ZTI_I8 x1,DRAW_LR,top,WRAPPED,0,1
    DRAW_ZTI_I8 x2,DRAW_LR,bottom,WRAPPED,0,1
	ret

Draw_ZTIB_I8_DWRL::
    DRAW_ZTI_I8 x1,DRAW_RL,top,WRAPPED,0,1
    DRAW_ZTI_I8 x2,DRAW_RL,bottom,WRAPPED,0,1
	ret

topZeroWidthHandler_ZTIB::
;perform per line updates

	PER_SCAN_ZTI top,WRAPPED,x1

	mov ebp,workspaceA.retAddress
	jmp ebp


bottomZeroWidthHandler_ZTIB::
;perform per line updates

	PER_SCAN_ZTI bottom,WRAPPED,x2

	mov ebp,workspaceA.retAddress
	jmp ebp


TriangleRender_ZTIB_I8_D16 endp

	endif

	if PARTS and PART_8Z_FOG_BLEND

TriangleRender_ZTIFB_I8_D16_FLAT proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_ZTI_ARBITRARY_FLAT

	jmp	TriangleRasterise_ZTIFB_I8_D16

TriangleRender_ZTIFB_I8_D16_FLAT endp


TriangleRender_ZTIFB_I8_D16 proc dummy:dword, v0:ptr brp_vertex, v1:ptr brp_vertex, v2:ptr brp_vertex

	mov	edx,v2
	mov	eax,v0

	mov	ecx,v1
	mov workspace.v2,edx

	mov workspace.v0,eax
	mov workspace.v1,ecx

	call TriangleSetup_ZTI_ARBITRARY

TriangleRasterise_ZTIFB_I8_D16 label proc

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

	mov eax,work.texture.base
	mov ebx,workspaceA.sv

	add ebx,eax
	mov eax,workspace.xm
	
	shl eax,16
	mov workspaceA.sv,ebx

	mov	edx,workspaceA.flags
	mov ebx,workspace.d_xm

    mov ecx,[jumpTable_ZTIFB_I8+4*edx]

	shl ebx,16
	mov workspace.xm_f,eax

	mov workspace.d_xm_f,ebx
	jmp ecx

if 0 ; set to zero to force wrapped mode, 1 for non-wrapped mode.
ECHO NON-WRAPPED SPECIAL CASE ENABLED
Draw_ZTIFB_I8_NWLR::
    DRAW_ZTI_I8 x1,DRAW_LR,top,NON_WRAPPED,1,1
    DRAW_ZTI_I8 x2,DRAW_LR,bottom,NON_WRAPPED,1,1
	ret

Draw_ZTIFB_I8_NWRL::
    DRAW_ZTI_I8 x1,DRAW_RL,top,NON_WRAPPED,1,1
    DRAW_ZTI_I8 x2,DRAW_RL,bottom,NON_WRAPPED,1,1
	ret

else
ECHO NON-WRAPPED SPECIAL CASE HAS BEEN DISABLED !

Draw_ZTIFB_I8_NWLR::
    DRAW_ZTI_I8 x1,DRAW_LR,top,WRAPPED,1,1
    DRAW_ZTI_I8 x2,DRAW_LR,bottom,WRAPPED,1,1
	ret

Draw_ZTIFB_I8_NWRL::
    DRAW_ZTI_I8 x1,DRAW_RL,top,WRAPPED,1,1
    DRAW_ZTI_I8 x2,DRAW_RL,bottom,WRAPPED,1,1
	ret
endif
;Wraps

Draw_ZTIFB_I8_DWLR::
    DRAW_ZTI_I8 x1,DRAW_LR,top,WRAPPED,1,1
    DRAW_ZTI_I8 x2,DRAW_LR,bottom,WRAPPED,1,1
	ret

Draw_ZTIFB_I8_DWRL::
    DRAW_ZTI_I8 x1,DRAW_RL,top,WRAPPED,1,1
    DRAW_ZTI_I8 x2,DRAW_RL,bottom,WRAPPED,1,1
	ret

topZeroWidthHandler_ZTIFB::
;perform per line updates

	PER_SCAN_ZTI top,WRAPPED,x1

	mov ebp,workspaceA.retAddress
	jmp ebp


bottomZeroWidthHandler_ZTIFB::
;perform per line updates

	PER_SCAN_ZTI bottom,WRAPPED,x2

	mov ebp,workspaceA.retAddress
	jmp ebp


TriangleRender_ZTIFB_I8_D16 endp

	endif


.data

	if PARTS and PART_8Z

jumpTable_ZT_I8 dword Draw_ZT_I8_NWLR
dword	Draw_ZT_I8_NWRL
dword	Draw_ZT_I8_DWLR
dword	Draw_ZT_I8_DWRL

	endif

	if PARTS and PART_8Z_FOG

jumpTable_ZTF_I8 dword Draw_ZTF_I8_NWLR
dword   Draw_ZTF_I8_NWRL
dword   Draw_ZTF_I8_DWLR
dword   Draw_ZTF_I8_DWRL

	endif

	if PARTS and PART_8Z_BLEND

jumpTable_ZTB_I8 dword Draw_ZTB_I8_NWLR
dword   Draw_ZTB_I8_NWRL
dword   Draw_ZTB_I8_DWLR
dword   Draw_ZTB_I8_DWRL

	endif

	if PARTS and PART_8Z_FOG_BLEND

jumpTable_ZTFB_I8 dword Draw_ZTFB_I8_NWLR
dword   Draw_ZTFB_I8_NWRL
dword   Draw_ZTFB_I8_DWLR
dword   Draw_ZTFB_I8_DWRL

	endif

	if PARTS and PART_8Z

jumpTable_ZTI_I8 dword Draw_ZTI_I8_NWLR
dword	Draw_ZTI_I8_NWRL
dword	Draw_ZTI_I8_DWLR
dword	Draw_ZTI_I8_DWRL

	endif

	if PARTS and PART_8Z_FOG

jumpTable_ZTIF_I8 dword Draw_ZTIF_I8_NWLR
dword   Draw_ZTIF_I8_NWRL
dword   Draw_ZTIF_I8_DWLR
dword   Draw_ZTIF_I8_DWRL

	endif

	if PARTS and PART_8Z_BLEND

jumpTable_ZTIB_I8 dword Draw_ZTIB_I8_NWLR
dword   Draw_ZTIB_I8_NWRL
dword   Draw_ZTIB_I8_DWLR
dword   Draw_ZTIB_I8_DWRL

	endif

	if PARTS and PART_8Z_FOG_BLEND

jumpTable_ZTIFB_I8 dword Draw_ZTIFB_I8_NWLR
dword   Draw_ZTIFB_I8_NWRL
dword   Draw_ZTIFB_I8_DWLR
dword   Draw_ZTIFB_I8_DWRL

	endif

fp_one			dword	1.0
fp_two			dword	2.0
fp_conv_d		dword	(127+52-0) shl 23 + (1 shl 22)

END
