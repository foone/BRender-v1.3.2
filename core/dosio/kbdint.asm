;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: kbdint.asm 1.1 1997/12/10 16:40:59 jon Exp $
;; $Locker: $
;;
;; Real mode interrupt handler that hooks INT15 and maintains a key matrix
;;
		.386

_TEXT16         SEGMENT DWORD PUBLIC USE16 'CODE'
	assume cs:_TEXT16
	assume ds:_TEXT16
	assume ss:_TEXT16

	include scancode.inc
	include eventq.inc

	public _KeyboardHandler
	public _KeyboardEnd

	public _KeyboardOldSeg
	public _KeyboardOldOff
	public _KeyboardEventQSeg
	public _KeyboardEventQOff
	public _KeyboardEnableBIOS
	public _KeyboardMap

JPRESSED	MACRO	scancode,label
		test	keyboard_map[scancode],QUAL_NONE
		jnz	label
		ENDM

JNPRESSED	MACRO	scancode,label
		test	keyboard_map[scancode],QUAL_NONE
		jz	label
		ENDM

_KeyboardHandler proc
	; See if this is INT15, AH=4F
	;
		cmp	ah,4fh
		je	hook_this

		db	0eah
_KeyboardOldOff	dw	0babeh
_KeyboardOldSeg	dw	0deadh

	; Got a keyboard interrupt
	;
hook_this:
		push	eax
		push	ebx
		push	ecx
		push	edx
		push	edi
		push	ds
		push	es

		mov	bx,cs
		mov	ds,bx

		mov	bl,al

	; If scancode is 0E0h (extended keycode), then set a flag and abort
	;
		cmp	bl,0E0h
		jne	not_extended
		mov	extended_flag,080h
		jmp done
not_extended:

	; Put up/down state into ah
	;
		xor	ah,ah

		shl	bl,1
		cmc
		rcl	ah,1
		shr	bl,1

	; Or Extended Bit into bl
	; Remeber unextended keycode in cl
	;
		mov	cl,bl
		or	bl,extended_flag
	
		xor	bh,bh
		mov	ds:extended_flag,bh

	; Build up a qualifier mask
	;
		mov	al,QUAL_NONE OR REPT_AUTO_REPT

		JPRESSED <bx>,not_first
		or	al,REPT_FIRST_DOWN
not_first:
		JNPRESSED <SC_LSHIFT>,no_lshift
		or	al,QUAL_SHIFT
no_lshift:
		JNPRESSED <SC_RSHIFT>,no_rshift
		or	al,QUAL_SHIFT
no_rshift:
		JNPRESSED <SC_CTRL>,no_lctrl
		or	al,QUAL_CTRL
no_lctrl:
		JNPRESSED <SC_CTRL+SC_EXTENDED>,no_rctrl
		or	al,QUAL_CTRL
no_rctrl:
		JNPRESSED <SC_ALT>,no_lalt
		or	al,QUAL_ALT
no_lalt:
		JNPRESSED <SC_ALT+SC_EXTENDED>,no_ralt
		or	al,QUAL_ALT
no_ralt:

	; ah contains 0 or 1, if 0 then QUAL_NONE bit of byte in key table is
	; cleared and nothing else is done, otherwise QUAL_NONE bit is set,
	; appropriate shift and repeat qualifiers are added and new the byte
	; is stored in the keyboard table.
	;
		and	ah,ah
		jz	key_up

		mov	keyboard_map[bx],al
		movzx	ecx,bx
		mov	bl,al
		mov	eax,DOSIO_EVENT_KEY_DOWN
		jmp	done_map

key_up:		and	keyboard_map[bx],NOT QUAL_NONE
		movzx	ecx,bx
		mov	bl,al
		mov	eax,DOSIO_EVENT_KEY_UP
done_map:

		cmp	_KeyboardEventQSeg,0
		je	done

; Generate an event - dosio qualifiers match scancode qualifiers (which match brwrap qualifiers)
;
		and	ebx,DOSIO_QUAL_SHIFT or DOSIO_QUAL_CONTROL or DOSIO_QUAL_ALT
		xor	edx,edx
		les	di,dword ptr _KeyboardEventQOff
		call	_EventAdd

done:
	; See if things should not be sent to the BIOS
	;
		cmp	_KeyboardEnableBIOS,0
		jne	to_bios

	; Always pass some qualifiers throught to BIOS
	;
		and	cl, not SC_EXTENDED
		cmp	cl,SC_ALT
		je	to_bios
		cmp	cl,SC_CTRL
		je	to_bios
		cmp	cl,SC_LSHIFT
		je	to_bios
		cmp	cl,SC_RSHIFT
		je	to_bios
		cmp	cl,SC_CAPSLOCK
		je	to_bios
		cmp	cl,SC_SCRLOCK
		je	to_bios
		cmp	cl,SC_NUMLOCK
		je	to_bios

	; Clear carry on stack to mark key as consumed
	;
		mov	bx,sp
		and	byte ptr ss:[bx+28],0feh
to_bios:

		pop	es
		pop	ds
		pop	edi
		pop	edx
		pop	ecx
		pop	ebx
		pop	eax
		iret
_KeyboardHandler endp

	; Include EventAdd() code
	;	
	include rmeventq.inc

; Data
;
_KeyboardEventQOff	dw	0
_KeyboardEventQSeg	dw	0

; Flag , true if all keycodes are passed on to BIOS
;
_KeyboardEnableBIOS	dw	0

_KeyboardMap label byte
keyboard_map	db	256 dup(0)

extended_flag	db	0
		db	0


_KeyboardEnd	db	0

_TEXT16	ends

	end _KeyboardHandler

