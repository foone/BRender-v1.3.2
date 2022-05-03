;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: mouint.asm 1.1 1997/12/10 16:41:00 jon Exp $
;; $Locker: $
;;
;; Real mode interrupt handler for mouse events
;;
		.386

		include eventq.inc

_TEXT16         SEGMENT DWORD PUBLIC USE16 'CODE'
	assume cs:_TEXT16
	assume ds:_TEXT16
	assume ss:_TEXT16

	public _MouseHandler
	public _MouseEnd

	public _MouseEventQSeg
	public _MouseEventQOff

; Mask values for interrupt handler
;
MASK_MOVE	equ	1
MASK_L_DOWN	equ	2
MASK_L_UP	equ	4
MASK_R_DOWN	equ	8
MASK_R_UP	equ	16
MASK_M_DOWN	equ	32
MASK_M_UP	equ	64


; _MouseHandler - attached to mouse events
;	
; Called with -
;
; AX: event mask
; BX: Buttons
; CX: X
; DX: Y
; SI: Raw X
; DI: Raw Y
;
_MouseHandler proc far
		push	eax
		push	ebx
		push	ecx
		push	edx
		push	si
		push	di
		push	ds
		push	es

	; Set up DS
	;
		push	cs
		pop	ds

	; Remember state
	;
		mov	_MouseButtons,bx
		mov	_MouseX,si
		mov	_MouseY,di

	; Generate an event for each bit set in the mask
	;
		movsx	ecx,si
		movsx	edx,di

		xor	ebx,ebx
		mov	si,ax

		cmp	_MouseEventQSeg,0
		je	done

		les	di,dword ptr _MouseEventQOff

		test	si,MASK_L_DOWN
		je	no_l_down
		mov	eax,DOSIO_EVENT_POINTER1_DOWN
		call	_EventAdd
no_l_down:	test	si,MASK_L_UP
		je	no_l_up
		mov	eax,DOSIO_EVENT_POINTER1_UP
		call	_EventAdd
no_l_up:

		test	si,MASK_M_DOWN
		je	no_m_down
		mov	eax,DOSIO_EVENT_POINTER2_DOWN
		call	_EventAdd
no_m_down:	test	si,MASK_M_UP
		je	no_m_up
		mov	eax,DOSIO_EVENT_POINTER2_UP
		call	_EventAdd
no_m_up:

		test	si,MASK_R_DOWN
		je	no_r_down
		mov	eax,DOSIO_EVENT_POINTER3_DOWN
		call	_EventAdd	       
no_r_down:	test	si,MASK_R_UP
		je	no_r_up
		mov	eax,DOSIO_EVENT_POINTER3_UP
		call	_EventAdd
no_r_up:

		test	si,MASK_MOVE
		je	no_move
		mov	eax,DOSIO_EVENT_POINTER_MOVE
		call	_EventAdd
no_move:
done:
		pop	es
		pop	ds
		pop	di
		pop	si
		pop	edx
		pop	ecx
		pop	ebx
		pop	eax
		retf
_MouseHandler endp

	; Include EventAdd() code
	;	
	include rmeventq.inc

; Data
;
	public _MouseEventQOff
	public _MouseEventQSeg
	public _MouseX
	public _MouseY
	public _MouseButtons

_MouseEventQOff	dw	0
_MouseEventQSeg	dw	0

_MouseX		dw	0
_MouseY		dw	0
_MouseButtons	dw	0

_MouseEnd	db	0

_TEXT16	ends

	end


