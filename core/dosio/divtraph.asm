;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: divtraph.asm 1.1 1997/12/10 16:40:58 jon Exp $
;; $Locker: $
;;
;; Supress integer divide by zero exceptions by stepping on to the
;; next insn
;;
;; Assumes that exception always happens in USE32 code and data
;;
	.386p
	.model flat

if ADD_RCS_ID
	.data
		db '$Id: divtraph.asm 1.1 1997/12/10 16:40:58 jon Exp $',0
		align 4
endif
	.code

; Count incremented by overflow handler (lives in CS: so that handler can get to it
;
DivTrapCounter		dd	0

_DivTrapHandler proc

	; Set up a stack frame to get at the saved CS:EIP
	;
	;		SS			32	| Application
	;		ESP			28	|
	;		EFLAGS			24	|
	;		CS			20	|
	;		EIP			16	|
	;
	;		Error			12
	;		CS	(Return)	8	| DPMI server
	;		EIP	(Return)	4	|
	;SS:EBP->	EBP			0	
	;
	;
		push	ebp
		mov	ebp,esp

		push	eax
		push	esi
		push	es

	; Get CS:EIP
	;
		les	esi,[ebp+16]

	; Eat up any valid insn prefixes
	;
again:
		lods	byte ptr es:[esi]
		cmp	al,0f0h	; LOCK
		je	again
		cmp	al,02eh	; CS override
		je	again
		cmp	al,036h	; SS override
		je	again
		cmp	al,03eh	; DS override
		je	again
		cmp	al,026h	; ES override
		je	again
		cmp	al,064h	; FS override
		je	again
		cmp	al,065h	; GS override
		je	again
		cmp	al,066h	; Operand Size prefix
		je	again

		cmp	al,067h	; Address Size prefix - can't handle
		je	panic

	; put insn in ah , get regmod byte into al
	;
		mov	ah,al
		lods	byte ptr es:[esi]

	; Check insn is a divide
	;
		cmp	ah,0f6h	; byte (i)div
		je	insn_ok

		cmp	ah,0f7h	; word (i)div
		jne	panic
insn_ok:

	; Check regmod for 16 bit address size
	;
	;	- opcode field is either 6 or 7 (div vs. idiv)
	;
		mov	ah,al
		and	ah,00110000b 	; top 2 bits of opcode
		cmp	ah,00110000b
		jne	panic

	; rm = 11iiixxx is register
	;
		cmp	al,011000000b
		jae	restore

; Consume any data after insn
;
		and	al,011000111b	; get rid of opcode

	; special case of disp32
	;
		cmp	al,000000101b
		je	four_bytes

	; account for any SIB
	;
		mov	ah,al
		and	ah,000000111b
		cmp	ah,000000100b
		jne	no_sib
		inc	esi
no_sib:

	; Consume displacement
	;
		and	al,011000000b 

		cmp	al,010000000b	; disp32
		je	four_bytes

		cmp	al,001000000b	; disp8
		je	one_byte

	; No more insn data to be skipped
	;
		jmp	restore

	; Skip two bytes following regmod
	;
four_bytes:
	      	add	esi,3

	; Skip one byte following regmod
	;
one_byte:
		inc	esi

restore:
	; Incement counter
	;
		inc	ds:DivTrapCounter	; XXX Assumes CS and DS are aliases

	; Put new EIP back into stack and exit
	;		
		mov	[ebp+16],esi

		pop	es
		pop	esi
		pop	eax
		pop	ebp
		retf

	; Could not handle situation - chain to previous handler
	;
panic:
		db	0EAh	; JMP FAR

	; Previous handler pointer (becomes part of above jump
	;
old_db0_handler	label fword

		df 0
	
_DivTrapHandler endp

if 0
; int ASM_CALL DOSDivTrapBegin(void);
;
; Install divide exception handler
;
; Returns 0 if sucessful
;
_DOSDivTrapBegin proc
		push	ebx
	; Fetch old exception handler
	;
		mov	eax,0202h
		mov	bl,0
		int	31h
		jc	failed

		mov	dword ptr ds:old_db0_handler,edx
		mov	word ptr ds:old_db0_handler+4,cx

	; Set new exception handler
	;
		mov	eax,0203h
		mov	bl,0
		mov	cx,cs
		mov	edx,offset _DivTrapHandler
		int	31h

	; Return success/fail flag
	;
failed:		sbb	eax,eax
		pop	ebx
		ret
_DOSDivTrapBegin endp

; int ASM_CALL DOSDivTrapEnd(void);
;
; Restore previous divide exception handler
;
_DOSDivTrapEnd	proc
		push	ebx
	; Set new exception handler
	;
		mov	eax,0203h
		mov	bl,0
		mov	edx,dword ptr old_db0_handler
		mov	cx,word ptr old_db0_handler+4
		int	31h
		sbb	eax,eax
		pop	ebx
		ret
_DOSDivTrapEnd	endp

endif

; int ASM_CALL DOSDivTrapCount(int reset);
;
; Returns number of divide traps that have happened
_DOSDivTrapCount proc
		mov	eax,DivTrapCounter
		cmp	dword ptr 4[esp],0
		je	done
		mov	DivTrapCounter,0
done:		ret
_DOSDivTrapCount endp
	

	end

