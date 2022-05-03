;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: rmeventq.asm 1.1 1997/12/10 16:41:00 jon Exp $
;; $Locker: $
;;
;; Real mode code for adding events to queue
;;
		.386
	include eventq.inc

_TEXT16         SEGMENT DWORD PUBLIC USE16 'CODE'
	assume cs:_TEXT16
	assume ds:_TEXT16
	assume ss:_TEXT16

	public _EventAdd

; _EventAdd - Add a new event to queue
;
; es:di - ptr to event queue structure
;
;  ax = type
;  bx = qualifier
; ecx = value1
; edx = value2
;
_EventAdd proc near
		push	bp
		push	si

	; Make sure interrupts are disabled for this process
	;
		pushf
		cli

	; Fetch head pointer
	;
		mov	bp,es:[di].eventq.head

	; Move head pointer on around buffer
	;	
		mov	si,bp
		inc	si
		cmp	si,es:[di].eventq.total
		jb	no_wrap
		xor	si,si
no_wrap:

	; Check we have not collided with tail
	;
		cmp	si,es:[di].eventq.tail
		je	no_space

	; Store event at head point
	;
		sal	ebx,16
		mov	bx,ax

		sal	bp,2
		mov	ax,bp
		add	bp,bp
		add	bp,ax

		mov	dword ptr es:[di+bp+eventq.slots+event.etype],ebx
		mov	es:[di+bp].eventq.slots+event.value_1,ecx
		mov	es:[di+bp].eventq.slots+event.value_2,edx

	; Save new head ptr
	;
		mov	es:[di].eventq.head,si

	; Keep track of number of outstanding events
	;
		inc	es:[di].eventq.count

no_space:	popf
		pop	si
		pop	bp
		ret
_EventAdd endp

_TEXT16	ends

	end


