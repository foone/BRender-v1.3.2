ifndef __COMMON_INC
__COMMON_INC equ 1

; Fix to stop masm creating prologue\epilogue to handle stack frame. 

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


DRAW_LR equ 0
DRAW_RL equ 1

NON_WRAPPED equ 0
WRAPPED equ 1

;macro to add/subtract according to direction
add_d macro reg0,reg1,direction 
ifidn <direction>,<DRAW_LR>
	add reg0,reg1
else 
	sub reg0,reg1
endif	
endm

;macro to add/subtract according to direction
adc_d macro reg0,reg1,direction
ifidn <direction>,<DRAW_LR>
	adc reg0,reg1
else 
	sbb reg0,reg1
endif	
endm

;macro to increment/decrement according to direction
inc_d macro reg0,direction
ifidn <direction>,<DRAW_LR>
	inc reg0
else 
	dec reg0
endif	
endm

;macro to decrement/increment according to direction
dec_d macro reg0,direction
ifidn <direction>,<DRAW_LR>
	dec reg0
else 
	inc reg0
endif	
endm

;macro to jg/jl according to direction
jg_d macro address,direction
ifidn <direction>,<DRAW_LR>
	jg address
else 
	jl address
endif	
endm

;macro to jl/jg according to direction
jl_d macro address,direction
ifidn <direction>,<DRAW_LR>
	jl address
else 
	jg address
endif	
endm

;macro to jge/jle according to direction
jge_d macro address,direction
ifidn <direction>,<DRAW_LR>
	jge address
else 
	jle address
endif	
endm

;macro to jle/jge according to direction
jle_d macro address,direction
ifidn <direction>,<DRAW_LR>
	jle address
else 
	jge address
endif	
endm

MAKE_N_LOW_BIT_MASK macro name,n
	local mask,count
	mask=0
	count=0
	repeat n
		mask=mask or (1 shl count)
		count=count+1
	endm
	name=mask
endm

endif