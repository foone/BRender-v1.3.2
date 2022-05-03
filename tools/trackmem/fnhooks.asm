;; Copyright (c) 1992,1993 Argonaut Software Ltd. All rights reserved.
;;
;; $Id: fnhooks.asm 1.1 1997/12/10 16:59:22 jon Exp $
;; $Locker: $
;;
;; Hooks the Watcom C++ epilogue and prologue calls to maintain a function
;; call list
;;
	.386p
	.model flat
	.data

CALLSTACK_SIZE	equ	100

	public __FunctionStack
	public __FunctionDepth

__FunctionStack	dd	CALLSTACK_SIZE dup (0)
__FunctionDepth	dd	0

	.code

; Called at every function entry
;
__PRO	proc
	; Work out entry point of function being called
	;
	; Take our return address-8 (push ebp; mov ebp,esp; call __PRO)
	;
		push	eax
		push	ebx
		mov	eax,8[esp]
		sub	eax,8

	; Find currnt position in call stack - don't
	; record function address if stack overflowed
	;
		mov	ebx,__FunctionDepth
		cmp	ebx,CALLSTACK_SIZE
		jae	overflow

		mov	__FunctionStack[ebx*4],eax

overflow:	inc	__FunctionDepth
		pop	ebx
		pop	eax
		ret
__PRO	endp

; Called at every function exit
;
__EPI	proc
	; Decrement call depth
	;
		dec	__FunctionDepth
		ret
__EPI	endp

	end

