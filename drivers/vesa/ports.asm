;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: ports.asm 1.1 1997/12/10 16:54:21 jon Exp $
;; $Locker: $
;;
	.386p
	.model flat,c
	.code
outp	proc port:dword, value:dword
		mov		edx,port
		mov		eax,value
		out		dx,al
		ret
outp	endp

outpw	proc port:dword, value:dword
		mov		edx,port
		mov		eax,value
		out		dx,ax
		ret
outpw	endp

outpd	proc port:dword, value:dword
		mov		edx,port
		mov		eax,value
		out		dx,eax
		ret
outpd	endp

inp		proc port:dword
		xor		eax,eax
		mov		edx,port
		in		al,dx
		ret
inp		endp

inpw	proc port:dword
		xor		eax,eax
		mov		edx,port
		in		ax,dx
		ret
inpw	endp

inpd	proc port:dword
		mov		edx,port
		in		eax,dx
		ret
inpd	endp

	end


