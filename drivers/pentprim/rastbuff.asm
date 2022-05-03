; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;
; $Id: rastbuff.asm 1.1 1997/12/10 16:48:02 jon Exp $
; $Locker: $
;
; Rasteriser buffer system
;
; All rasterisers are entered with esp pointing at the triangle parameter block
; and exit by popping setup block from stack and jumping to next function
;
; XXX should rearrange this so that stack is built bottom to top, and then flushed -
;     means that prims are rasterised in order
;
;
		.586
		.MMX
		.model	flat,c

;		include iammx.inc
		include drv.inc
		include unpack.inc

		include rastmacs.inc

		.code

; void BR_ASM_CALL RasteriseBufferDone(void);
;
; Termination function for chain of rasterise blocks - restores original stack pointer and
; returns
;
RasteriseBufferDone proc
		; Tidy up MMX
		;
			emms

		; Reset rasterise stack pointer
		;
			mov		eax,offset rasteriseBufferLast

			mov		esp,rasteriseBufferSaved

		; Restore original stack pointer
		;
			mov		rasteriseBufferTop,eax

			pop		ebx
			pop		edi
			pop		esi
			pop		ebp

			ret
RasteriseBufferDone endp


RasteriseBufferExit proc

			mov		esp,rasteriseBufferSaved

			pop		ebx
			pop		edi
			pop		esi
			pop		ebp

			ret
RasteriseBufferExit endp


; void BR_ASM_CALL RasteriseBufferDisable(void);
;
; Initialise rasterisation stack
;
RasteriseBufferDisable proc

			mov		eax,offset rasteriseBufferOff
			mov     rasteriseBufferTop,eax
			ret
RasteriseBufferDisable endp

; void BR_ASM_CALL RasteriseBufferBegin(void);
;
; Initialise rasterisation stack
;
RasteriseBufferBegin proc

			mov		eax,offset rasteriseBufferLast
			mov     rasteriseBufferTop,eax
			ret
RasteriseBufferBegin endp

; void BR_ASM_CALL RasteriseBufferEnd(void);
;
RasteriseBufferEnd proc
			ret
RasteriseBufferEnd endp

; void * BR_ASM_CALL RasteriseBufferAllocate(br_size_t param_size,br_size_t work_size);
;
; Allocate workspace on the rasterise stack
;
RasteriseBufferAllocate proc uses esi edi ebx, param_size:dword, work_size:dword

		; See how mush free space is left on stack
		;
			mov		eax,offset rasteriseBuffer
			mov		edx,rasteriseBufferTop

			sub		edx,eax

			cmp		edx,work_size
			jae		no_flush

		; Flush out existing stack
		;
			call    RasteriseBufferFlush


no_flush:	mov		eax,rasteriseBufferTop
			sub		eax,param_size
			mov		rasteriseBufferTop,eax
			ret

RasteriseBufferAllocate endp

; void BR_ASM_CALL RasteriseBufferFlush(void);
;
; Executes the current set of stacked rasterisation blocks
;
;
RasteriseBufferFlush proc
		; Save registers on original stack
		;
			push	ebp
			push	esi
			push	edi
			push	ebx

		; Get pointer to top of rasterise stack
		;
			mov		eax,rasteriseBufferTop


		; Get pointer to first function
		;
			mov		edx,[eax].tsb_header.function

		; Save original stack pointer
		;
			mov		rasteriseBufferSaved,esp
			mov		esp,eax

			jmp		edx

RasteriseBufferFlush endp

		.data


	; Stack of stored rasterisation blocks
	;
		public rasteriseBuffer
		public rasteriseBufferTop

					qword	40 dup(?)	; Leeway for workspace and stack frames

rasteriseBuffer		qword	200 dup(?)

	; Fake header that does nothing
	;
rasteriseBufferOff	dword	RasteriseBufferExit

	; Fake header that terminates stack
	;
rasteriseBufferLast	dword	RasteriseBufferDone

	; Pointer to current top of stack
	;
rasteriseBufferTop	dword   rasteriseBufferLast

	; Saved stack pointer during rasterisation
	;
rasteriseBufferSaved dword	0
					dword	0	; Pad


		end

