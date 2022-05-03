;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: memloops.asm 1.1 1998/02/04 16:47:02 jon Exp $
;; $Locker: $
;;
;; Inner loops for operations on in-memory pixelmaps
;;
	.386p
	.model flat,c

	.data

if ADD_RCS_ID
	db '$Id: memloops.asm 1.1 1998/02/04 16:47:02 jon Exp $',0
	align	4
endif

bit_to_mask_s	db	11111111b
		db	01111111b
		db	00111111b
		db	00011111b
		db	00001111b
		db	00000111b
		db	00000011b
		db	00000001b
		db	00000000b

bit_to_mask_e	db	00000000b
		db	10000000b
		db	11000000b
		db	11100000b
		db	11110000b
		db	11111000b
		db	11111100b
		db	11111110b
		db	11111111b


; FPU control word definitions
;
FPU_CW_IC				equ	01000h
FPU_CW_IC_PROJECTIVE 	equ	00000h
FPU_CW_IC_AFFINE     	equ	01000h

FPU_CW_RC				equ	00c00h
FPU_CW_RC_NEAREST		equ	00000h
FPU_CW_RC_DOWN			equ	00400h
FPU_CW_RC_UP			equ	00800h
FPU_CW_RC_CHOP			equ	00c00h

FPU_CW_PC				equ	00300h
FPU_CW_PC_SINGLE		equ	00000h
FPU_CW_PC_DOUBLE		equ	00200h
FPU_CW_PC_EXTENDED		equ	00300h

FPU_CW_IE				equ	00080h

FPU_CW_PM				equ	00020h
FPU_CW_UM				equ	00010h
FPU_CW_OM				equ	00008h
FPU_CW_ZM				equ	00004h
FPU_CW_DM				equ	00002h
FPU_CW_IM				equ	00001h

FPU_CW_MASK_ALL			equ 0003fh

fpu_cw	dw		FPU_CW_IC_AFFINE+FPU_CW_RC_CHOP+FPU_CW_PC_EXTENDED+FPU_CW_MASK_ALL

	.code



; void BR_ASM_CALL _MemCopyBits_A(
;		char *dest, br_uint_32 dest_qual, br_int_32 d_stride,
;		br_uint_8 *src,br_uint_32 s_stride,br_uint_32 start_bit,br_uint_32 end_bit,
;		br_uint_32 nrows,br_uint_32 colour)
;
_MemCopyBits3DFX_A proc uses ebx esi edi es,
	dest:ptr word,	dest_qual: dword, d_stride:dword,
	src: ptr byte, s_stride:dword, start_bit:dword, end_bit:dword,
	n_rows:dword, colour: dword

		mov	edi,dest
		mov	esi,src

	; Find count, and masks at start and end of line
	;
		mov	edx,start_bit	; start bit must be 0-7

		mov	ecx,end_bit
		mov	eax,ecx

		shr	ecx,3		; n_bytes = n_bits/8
		and	eax,7		; bit in byte

		mov	dl,bit_to_mask_s[edx]
		mov	dh,bit_to_mask_e[eax]

		mov	ebx,colour

		push	ds
		mov	ds,word ptr dest_qual

	; Go to appropriate core loops
	;
		test	ecx,ecx
		jne	multi_byte

;; Only one source byte wide
;;
		and	dl,dh		; combine masks
		mov	ecx,n_rows
v_loop1:
	; Get first source byte and mask it
	;
		mov	al,ss:[esi]
		and	al,dl

	; Set destination pixels according to byte mask
	;
x = 0
	rept 8
	local no_pixel
		add	al,al
		jnc	no_pixel
		mov	[edi+(x*2)],bx
no_pixel:
x = x + 1
	endm

		add	edi,d_stride
		add	esi,s_stride

		dec	ecx
		jne	v_loop1
		pop	ds
		ret

;; 2 or more source bytes wide
;;
multi_byte:
v_loopn:		push	ecx
		push	esi
		push	edi

	; Get first source byte and mask it
	;
		mov	al,ss:[esi]
		and	al,dl
h_loop:		inc	esi
h_loop_last:
	; Set destination pixels according to byte mask
	;
x = 0
	rept 8
	local no_pixel
		add	al,al
		jnc	no_pixel
		mov	[edi+(x*2)],bx
no_pixel:
x = x + 1
	endm

	; Get next source byte
	;
		mov	al,ss:[esi]
		add	edi,8*2
		dec	ecx
		jg	h_loop
		js	done_row

	; If it is the last byte, mask and look one final time
	;
		and	al,dh
		jmp	h_loop_last
done_row:

	; End of row
	;
		pop	edi
		pop	esi
		pop	ecx
		add	edi,d_stride
		add	esi,s_stride

		dec	n_rows
		jne	v_loopn
		pop	ds
		ret

_MemCopyBits3DFX_A endp

	end

