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
