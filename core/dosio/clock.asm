;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: clock.asm 1.1 1997/12/10 16:40:58 jon Exp $
;; $Locker: $
;;
	.386p
	.model flat

if ADD_RCS_ID
	.data
		db '$Id: clock.asm 1.1 1997/12/10 16:40:58 jon Exp $',0
		align 4
endif
	.data

; Offset of BIOS RAM - system ticks
;
TIMER_OFFSET	equ	046ch

; Port for mode control of 8253
;
PIT_MODE	equ	43h

; Port for reading/writing count 0 of PIT
;
PIT_TIMER0	equ	40h


	.code
		extrn _HostRealWordRead:proc

; void DOSClockBegin(void);
;
		public _DOSClockBegin
_DOSClockBegin proc
	; Make sure real mode descriptors are setup
	;

	; Linear count
	;
		mov	al,00110100b
		out	PIT_MODE,al
		xor	al,al
		out	PIT_TIMER0,al
		out	PIT_TIMER0,al
		ret
_DOSClockBegin endp


; void DOSClockEnd(void);
;
		public _DOSClockEnd
_DOSClockEnd proc
		ret
_DOSClockEnd endp

; unsigned DOSClockRead(void);
;
	public _DOSClockRead
_DOSClockRead proc

		push	es
		push	ecx
		push	edx

		cli

	; Read 1/18th ticks from BIOS work area
	;
		push	0
		push	TIMER_OFFSET
		call	_HostRealWordRead
		add	esp,8

		mov	dx,ax

	; Mode 2, read downcount
	;
		xor	al,al
		out	PIT_MODE,al

	; Read the PIT downcount and save in timer area
	;
		in	al,PIT_TIMER0
		mov	ah,al
		in	al,PIT_TIMER0
		xchg	al,ah
		neg	ax

		sti

	; Merge DX:AX into EAX
	;
		shl	edx,16
		mov	dx,ax
		mov	eax,edx

		pop	edx
		pop	ecx
		pop	es
		ret
_DOSClockRead endp

	end
