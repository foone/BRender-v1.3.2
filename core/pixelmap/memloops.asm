;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: memloops.asm 1.1 1997/12/10 16:41:24 jon Exp $
;; $Locker: $
;;
;; Inner loops for operations on in-memory pixelmaps
;;
	.386p
	.model flat,c

	.data

if ADD_RCS_ID
	db '$Id: memloops.asm 1.1 1997/12/10 16:41:24 jon Exp $',0
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


;; Only one source byte wide
;;
COPY_BITS_CORE_1 macro	bpp
		local	v_loop

	if bpp eq 3
		mov	eax,ebx
		shr	eax,8
	endif
		and	dl,dh		; combine masks
		mov	ecx,n_rows
v_loop:
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
	if bpp eq 1
		mov	[edi+(x*1)],bl
	elseif bpp eq 2
		mov	[edi+(x*2)+0],bl
		mov	[edi+(x*2)+1],bh
	elseif bpp eq 3
		mov	[edi+(x*3)+0],bl
		mov	[edi+(x*3)+1],bh
		mov	[edi+(x*3)+2],ah
	elseif bpp eq 4
		mov	[edi+(x*4)],ebx
	endif
no_pixel:
x = x + 1
	endm

		add	edi,d_stride
		add	esi,s_stride

		dec	ecx
		jne	v_loop
		pop	ds
		ret
	endm

;; 2 or more source bytes wide
;;
COPY_BITS_CORE_N macro	bpp
		local	v_loop,h_loop,h_loop_last,done_row

	if bpp eq 3
		mov	eax,ebx
		shr	eax,8
	endif

v_loop:		push	ecx
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
	if bpp eq 1
		mov	[edi](x*1),bl
	elseif bpp eq 2
		mov	[edi]((x*2)+0),bl
		mov	[edi]((x*2)+1),bh
	elseif bpp eq 3
		mov	[edi]((x*3)+0),bl
		mov	[edi]((x*3)+1),bh
		mov	[edi]((x*3)+2),ah
	elseif bpp eq 4
		mov	[edi+(x*4)],ebx
	endif
no_pixel:
x = x + 1
	endm

	; Get next source byte
	;
		mov	al,ss:[esi]
		add	edi,8*bpp
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
		jne	v_loop
		pop	ds
		ret
	endm

; void BR_ASM_CALL _MemCopyBits_A(
;		char *dest, br_uint_32 dest_qual, br_int_32 d_stride,
;		br_uint_8 *src,br_uint_32 s_stride,br_uint_32 start_bit,br_uint_32 end_bit,
;		br_uint_32 nrows,br_uint_32 colour)
;
_MemCopyBits_A proc uses ebx esi edi es,
	dest:ptr word,	dest_qual: dword, d_stride:dword,
	src: ptr byte, s_stride:dword, start_bit:dword, end_bit:dword,
	n_rows:dword, bpp:dword, colour: dword

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
		mov	eax,bpp
		push	ds
		mov	ds,word ptr dest_qual

	; Go to appropriate core loops
	;
		test	ecx,ecx
		jne	multi_byte

		dec	eax
		je	bpp1_1
		dec	eax
		je	bpp1_2
		dec	eax
		je	bpp1_3
		dec	eax
		je	bpp1_4
		pop	ds
		ret
multi_byte:
		dec	eax
		je	bppn_1
		dec	eax
		je	bppn_2
		dec	eax
		je	bppn_3
		dec	eax
		je	bppn_4
		pop	ds
		ret

bpp1_1:		COPY_BITS_CORE_1	1
bpp1_2:		COPY_BITS_CORE_1	2
bpp1_3:		COPY_BITS_CORE_1	3
bpp1_4:		COPY_BITS_CORE_1	4
	
bppn_1:		COPY_BITS_CORE_N	1
bppn_2:		COPY_BITS_CORE_N	2
bppn_3:		COPY_BITS_CORE_N	3
bppn_4:		COPY_BITS_CORE_N	4

_MemCopyBits_A endp

; void BR_ASM_CALL _MemFill_A(char *dest, br_uint_32 dest_qual, br_uint_32 pixels, br_uint_32 bpp, br_uint_32 colour);
;
_MemFill_A proc uses ebx esi edi es,
	dest: ptr byte, dest_qual:dword, pixels: dword, bpp: dword, colour:dword

		mov	es,word ptr dest_qual
		mov	edi,dest
		mov	eax,colour
		mov	ecx,pixels

		mov	ebx,bpp
		cmp	ebx,4
		je	bpp_4
		cmp	ebx,3
		je	bpp_3
		cmp	ebx,2
		je	bpp_2
		cmp	ebx,1
		je	bpp_1
		ret

; 1 byte per pixel
;
bpp_1:
	; replicate colour into 32 bits
	;
		mov	ah,al
		mov	ebx,eax
		shl	eax,16
		mov	al,bl
		mov	ah,bl
bpp_1_fill:
	; Get aligned
	;
		mov	ebx,edi
		and	ebx,3
		je	aligned

align_loop:	mov	es:[edi],al
		inc	edi
		dec	ecx
		je	done
		dec	ebx
		jne	align_loop
aligned:
	; fill dwords
	;
		mov	ebx,ecx
		shr	ecx,2
		rep	stosd

	; Fill remaining bytes
	;
		mov	ecx,ebx
		and	ecx,3
		rep	stosb
done:
		ret		

; 2 byte per pixel
;
bpp_2:
	; replicate colour into 32 bits
	;
		mov	ebx,eax
		shl	eax,16
		mov	al,bl
		mov	ah,bh

	; fill dwords
	;
		shr	ecx,1
		rep	stosd

	; Fill remaining words
	;
		adc	ecx,ecx
		rep	stosw
		ret		

; 4 bytes per pixel - just go for it!
;
bpp_4:		rep	stosd
		ret

; 3 bytes per pixel
;	
bpp_3:
		cmp	al,ah	; If 3 fill bytes are the same, just use 1bpp fill
		jne	real_3
		mov	ebx,eax
		shr	ebx,8
		cmp	bh,al
		jne	real_3

		lea	ecx,[ecx*2+ecx]	; pixels *= 3
		rol	eax,8		; Set top byte of colour
		mov	al,ah
		jmp	bpp_1_fill
real_3:

	; Fill eax, ebx, edx with 4 repeats of colour
	;
					; EAX  EBX  EDX
					; LH   LH   LH
					; BGRX XXXX XXXX
		rol	eax,8		; XBGR XXXX XXXX
		mov	edx,eax		; XBGR XXXX XBGR
		mov	al,dh		; BBGR XXXX XBGR
		ror	eax,16		; GRBB XXXX XBGR
		mov	ebx,edx		; GRBB XBGR XBGR
		rol	ebx,8		; GRBB RXBG XBGR
		mov	bl,al		; GRBB GXBG XBGR
		mov	dl,ah		; GRBB GXBG RBGR
		mov	bh,ah		; GRBB GRBG RBGR
		rol	eax,8		; BGRB GRBG RBGR

		push	ds
		mov	ds,word ptr dest_qual

       	; Fill in 4 pixel chunks
	;
		mov	esi,ecx
		shr	esi,2
		je	bpp_3_last

bpp_3_loop:	mov	[edi+0],eax
		mov	[edi+4],ebx
		mov	[edi+8],edx
		add	edi,12
		dec	esi
		jne	bpp_3_loop

	; Do left over pixels a byte at a time
	;
bpp_3_last:
		and	ecx,3
		je	bpp_3_done
bpp_3_last_loop:
		mov	[edi+0],al
		mov	[edi+1],ah
		mov	[edi+2],bh
		add	edi,3
		dec	ecx
		jne	bpp_3_last_loop
bpp_3_done:
		pop	ds
		ret
_MemFill_A endp

; void BR_ASM_CALL _MemRectFill_A(char *dest, br_uint_32 dest_qual, br_uint_32 pwidth, br_uint_32 pheight,
; 				  br_int_32 d_stride, br_uint_32 bpp, br_uint_32 colour);
;
_MemRectFill_A proc uses ebx esi edi es,
	dest: ptr byte, dest_qual:dword, pwidth: dword, pheight: dword, d_stride:dword, bpp: dword, colour:dword

		mov	es,word ptr dest_qual
		mov	esi,d_stride
		mov	edi,dest
		mov	eax,colour
		mov	ecx,pwidth

		mov	ebx,bpp
		cmp	ebx,4
		je	bpp_4
		cmp	ebx,3
		je	bpp_3
		cmp	ebx,2
		je	bpp_2
		cmp	ebx,1
		je	bpp_1
		ret

; 1 byte per pixel
;
bpp_1:
	; replicate colour into 32 bits
	;
		mov	ah,al
		mov	ebx,eax
		shl	eax,16
		mov	al,bl
		mov	ah,bl

bpp_1_fill:
		sub	esi,ecx
		mov	ebx,ecx

row_loop_1:
		mov	ecx,ebx

	; Get aligned
	;
		mov	edx,edi
		and	edx,3
		je	aligned

align_loop:	mov	es:[edi],al
		inc	edi
		dec	ecx
		je	done
		dec	edx
		jne	align_loop
aligned:
	; fill dwords
	;
		mov	edx,ecx
		shr	ecx,2
		rep	stosd

	; Fill remaining bytes
	;
		mov	ecx,edx
		and	ecx,3
		rep	stosb

done:		add	edi,esi
		dec	pheight
		jne	row_loop_1
		ret

; 2 byte per pixel
;
bpp_2:
	; replicate colour into 32 bits
	;
		mov	ebx,eax
		shl	eax,16
		mov	al,bl
		mov	ah,bh

		sub	esi,ecx
		sub	esi,ecx

		mov	ebx,ecx
row_loop_2:
		mov	ecx,ebx

	; fill dwords
	;
		shr	ecx,1
		rep	stosd

	; Fill remaining words
	;
		adc	ecx,ecx
		rep	stosw

		add	edi,esi
		dec	pheight
		jne	row_loop_2
		ret		

; 4 bytes per pixel - just go for it!
;
bpp_4:
		sub	esi,ecx
		sub	esi,ecx
		sub	esi,ecx
		sub	esi,ecx

		mov	ebx,ecx
row_loop_4:
		mov	ecx,ebx
		rep	stosd

		add	edi,esi
		dec	pheight
		jne	row_loop_4
		ret

; 3 bytes per pixel
;	
bpp_3:
		cmp	al,ah	; If 3 fill bytes are the same, just use 1bpp fill
		jne	real_3
		mov	ebx,eax
		shr	ebx,8
		cmp	bh,al
		jne	real_3

		lea	ecx,[ecx*2+ecx]	; pixels *= 3
		rol	eax,8		; Set top byte of colour
		mov	al,ah
		jmp	bpp_1_fill

real_3:

	; Fill eax, ebx, edx with 4 repeats of colour
	;
					; EAX  EBX  EDX
					; LH   LH   LH
					; BGRX XXXX XXXX
		rol	eax,8		; XBGR XXXX XXXX
		mov	edx,eax		; XBGR XXXX XBGR
		mov	al,dh		; BBGR XXXX XBGR
		ror	eax,16		; GRBB XXXX XBGR
		mov	ebx,edx		; GRBB XBGR XBGR
		rol	ebx,8		; GRBB RXBG XBGR
		mov	bl,al		; GRBB GXBG XBGR
		mov	dl,ah		; GRBB GXBG RBGR
		mov	bh,ah		; GRBB GRBG RBGR
		rol	eax,8		; BGRB GRBG RBGR


		push	ds
		mov	ds,word ptr dest_qual		

row_loop_3:	push	ecx
		push	edi

       	; Fill in 4 pixel chunks
	;
		mov	esi,ecx
		shr	esi,2
		je	bpp_3_last

bpp_3_loop:	mov	[edi+0],eax
		mov	[edi+4],ebx
		mov	[edi+8],edx
		add	edi,12
		dec	esi
		jne	bpp_3_loop

	; Do left over pixels a byte at a time
	;
bpp_3_last:
		and	ecx,3
		je	bpp_3_done
bpp_3_last_loop:
		mov	[edi+0],al
		mov	[edi+1],ah
		mov	[edi+2],bh
		add	edi,3
		dec	ecx
		jne	bpp_3_last_loop
bpp_3_done:
		pop	edi
		pop	ecx
		add	edi,d_stride
		dec	pheight
		jne	row_loop_3

		pop	ds
		ret
_MemRectFill_A endp

; void BR_ASM_CALL _MemRectCopy_A(char *dest, br_uint_32 dest_qual, char *src, br_uint_32 src_qual,
;				  br_uint_32 pwidth, br_uint_32 pheight,
; 				  br_int_32 d_stride,br_int_32 s_stride,
;				  br_uint_32 bpp);
;
_MemRectCopy_A proc uses ebx esi edi ds es,
	dest: ptr byte, dest_qual:dword, src:ptr byte, src_qual: dword,
	pwidth: dword, pheight: dword, d_stride:dword, s_stride:dword, bpp: dword

		mov	ds,word ptr src_qual
		mov	es,word ptr dest_qual
		mov	esi,src
		mov	edi,dest

		mov	eax,bpp
	   	mul	pwidth
		mov	ecx,eax

row_loop:	push	ecx
		push	esi
		push	edi

	; Get destination aligned
	;
		mov	edx,edi
		and	edx,3
		je	aligned

		neg	edx		; adjust = 4 - (start & 3)
		add	edx,4

align_loop:	mov	al,[esi]
		mov	es:[edi],al
		inc	esi
		inc	edi
		dec	ecx
		je	done
		dec	edx
		jne	align_loop
aligned:
	; Copy dwords
	;
		mov	edx,ecx
		shr	ecx,2
		rep	movsd

	; Copy remaining bytes
	;
		mov	ecx,edx
		and	ecx,3
		rep	movsb
done:
		pop	edi
		pop	esi
		pop	ecx

		add	edi,d_stride
		add	esi,s_stride
		dec	pheight
		jne	row_loop
		ret
_MemRectCopy_A endp

; void BR_ASM_CALL _MemCopy_A(char *dest, br_uint_32 dest_qual, char *src, br_uint_32 src_qual,
;				  br_uint_32 pixels,
;				  br_uint_32 bpp);
;
_MemCopy_A proc uses ebx esi edi ds es,
	dest: ptr byte, dest_qual:dword, src:ptr byte, src_qual:dword,
	pixels: dword, bpp: dword

		mov	ds,word ptr src_qual
		mov	es,word ptr dest_qual
		mov	esi,src
		mov	edi,dest

		mov	eax,bpp
	   	mul	pixels
		mov	ecx,eax

	; Get destination aligned
	;
		mov	edx,edi
		and	edx,3
		je	aligned

align_loop:	mov	al,[esi]
		mov	es:[edi],al
		inc	esi
		inc	edi
		dec	ecx
		dec	edx
		jne	align_loop

aligned:
	; Copy dwords
	;
		mov	edx,ecx
		shr	ecx,2
		rep	movsd

	; Copy remaining bytes
	;
		mov	ecx,edx
		and	ecx,3
		rep	stosb

		ret
_MemCopy_A endp


; void BR_ASM_CALL _MemRectCopySourceColourKey0_A(
;				  char *dest, br_uint_32 dest_qual,
; 				  char *src, br_uint_32 src_qual,
;				  br_uint_32 pwidth, br_uint_32 pheight,
; 				  br_int_32 d_stride,br_int_32 s_stride,
;				  br_uint_32 bpp);
;
_MemRectCopySourceColourKey0_A proc uses esi edi ds es,
	dest: ptr byte, dest_qual:dword, src:ptr byte, src_qual: dword,
	pwidth: dword, pheight: dword, d_stride:dword, s_stride:dword, bpp: dword

		mov	ds,word ptr src_qual
		mov	es,word ptr dest_qual

		mov	esi,src
		mov	edi,dest

		mov	eax,bpp
		mov	ecx,pwidth

		cmp	eax,1
		je	bpp_1

		cmp	eax,2
		je	bpp_2

		cmp	eax,3
		je	bpp_3

		cmp	eax,4
		je	bpp_4

		ret

bpp_1:
		add	esi,ecx
		add	edi,ecx
		neg	ecx

bpp_1_row:
		push	ecx
		push	esi
		push	edi

bpp_1_loop:
		mov	al,[esi+ecx]

		and	al,al
		jz	bpp_1_next

		mov	[edi+ecx],al

bpp_1_next:
		inc	ecx
		jnz	bpp_1_loop

		pop	edi
		pop	esi
		pop	ecx

		add	edi,d_stride
		add	esi,s_stride
		dec	pheight
		jne	bpp_1_row
		ret

bpp_2:
		add	ecx,ecx
		add	esi,ecx
		add	edi,ecx
		neg	ecx
		xor	eax,eax

bpp_2_row:
		push	ecx
		push	esi
		push	edi

bpp_2_loop:
		mov	ax,[esi+ecx]

		and	eax,eax
		jz	bpp_2_next

		mov	[edi+ecx],ax

bpp_2_next:
		add	ecx,2
		jnz	bpp_2_loop

		pop	edi
		pop	esi
		pop	ecx

		add	edi,d_stride
		add	esi,s_stride
		dec	pheight
		jne	bpp_2_row
		ret

bpp_3:
		mov	eax,ecx
		add	ecx,ecx
		add	ecx,eax

		add	esi,ecx
		add	edi,ecx
		neg	ecx

bpp_3_row:
		push	ecx
		push	esi
		push	edi

bpp_3_loop:
		xor	eax,eax
		mov	al,[esi+ecx+0]
		shl	eax,16
		mov	ah,[esi+ecx+1]
		mov	al,[esi+ecx+2]

		and	eax,eax
		jz	bpp_3_next

		mov	[edi+ecx],al

bpp_3_next:
		add	ecx,3
		jnz	bpp_3_loop

		pop	edi
		pop	esi
		pop	ecx

		add	edi,d_stride
		add	esi,s_stride
		dec	pheight
		jne	bpp_3_row
		ret

bpp_4:
		add	ecx,ecx
		add	ecx,ecx
		add	esi,ecx
		add	edi,ecx
		neg	ecx

bpp_4_row:
		push	ecx
		push	esi
		push	edi

bpp_4_loop:
		mov	eax,[esi+ecx]

		and	eax,eax
		jz	bpp_4_next

		mov	[edi+ecx],eax

bpp_4_next:
		add	ecx,4
		jnz	bpp_4_loop

		pop	edi
		pop	esi
		pop	ecx

		add	edi,d_stride
		add	esi,s_stride
		dec	pheight
		jne	bpp_4_row
		ret

_MemRectCopySourceColourKey0_A endp

; void BR_ASM_CALL _MemCopySourceColourKey0_A(
;				  char *dest, br_uint_32 dest_qual,
;				  char *src, br_uint_32 src_qual,
;				  br_uint_32 pixels,
;				  br_uint_32 bpp);
;
_MemCopySourceColourKey0_A proc uses ebx esi edi ds es,
	dest: ptr byte, dest_qual:dword, src:ptr byte, src_qual:dword,
	pixels: dword, bpp: dword

		mov	ds,word ptr src_qual
		mov	es,word ptr dest_qual

		mov	esi,src
		mov	edi,dest

		mov	eax,bpp
		mov	ecx,pixels

		cmp	eax,1
		je	bpp_1

		cmp	eax,2
		je	bpp_2

		cmp	eax,3
		je	bpp_3

		cmp	eax,4
		je	bpp_4

		ret

bpp_1:
		add	esi,ecx
		add	edi,ecx
		neg	ecx

bpp_1_loop:
		mov	al,[esi+ecx]

		and	al,al
		jz	bpp_1_next

		mov	[edi+ecx],al

bpp_1_next:
		inc	ecx
		jnz	bpp_1_loop
		ret

bpp_2:
		add	ecx,ecx
		add	esi,ecx
		add	edi,ecx
		neg	ecx
		xor	eax,eax

bpp_2_loop:
		mov	ax,[esi+ecx]

		and	eax,eax
		jz	bpp_2_next

		mov	[edi+ecx],ax

bpp_2_next:
		add	ecx,2
		jnz	bpp_2_loop
		ret

bpp_3:
		mov	eax,ecx
		add	ecx,ecx
		add	ecx,eax
		add	esi,ecx
		add	edi,ecx
		neg	ecx

bpp_3_loop:
		xor	eax,eax
		mov	al,[esi+ecx+0]
		shl	eax,16
		mov	ah,[esi+ecx+1]
		mov	al,[esi+ecx+2]

		and	eax,eax
		jz	bpp_3_next

		mov	[edi+ecx],al

bpp_3_next:
		add	ecx,3
		jnz	bpp_3_loop
		ret

bpp_4:
		add	ecx,ecx
		add	ecx,ecx
		add	esi,ecx
		add	edi,ecx
		neg	ecx

bpp_4_loop:
		mov	eax,[esi+ecx]

		and	eax,eax
		jz	bpp_4_next

		mov	[edi+ecx],eax

bpp_4_next:
		add	ecx,4
		jnz	bpp_4_loop
		ret

_MemCopySourceColourKey0_A endp


; void BR_ASM_CALL _MemLine(char *dest, br_uint_32 dest_qual, br_int_32 x1, br_int_32 y1,br_int_32 x2, br_int_32 y2, br_uint_32 bpp, br_uint_32 colour)
;
; Draw a line given address of first pixel, and y1 <= y2
;
; XXX


; void BR_ASM_CALL _MemPixelSet(char *dest, br_uint_32 dest_qual, br_uint_32 bytes, br_uint_32 colour)
;
; Plot a pixel
;
_MemPixelSet proc uses ebx esi edi es, dest: ptr byte, dest_qual:dword, bytes:dword, colour:dword
		mov	eax,colour
		mov	ebx,bytes
		mov	edi,dest
		mov	es,dest_qual
		cmp	ebx,1
		je	bpp_1
		cmp	ebx,2
		je	bpp_2
		cmp	ebx,3
		je	bpp_3
		cmp	ebx,4
		je	bpp_4
		ret

bpp_1:		mov	es:[edi],al
		ret

bpp_2:		mov	es:[edi],ax
		ret

bpp_4:		mov	es:[edi],eax
		ret

bpp_3:		mov	es:[edi+0],al
		mov	es:[edi+1],ah
		shr	eax,8
		mov	es:[edi+2],ah
     		ret
_MemPixelSet endp

; br_uint_32 BR_ASM_CALL _MemPixelGet(char *dest, br_uint_32 dest_qual, br_uint_32 bytes)
;
; Read a pixel
;
_MemPixelGet proc uses ebx esi edi es, dest: ptr byte, dest_qual:dword, bytes:dword
		mov	ebx,bytes
		mov	edi,dest
		mov	es,dest_qual
		xor	eax,eax

		cmp	ebx,1
		je	bpp_1
		cmp	ebx,2
		je	bpp_2
		cmp	ebx,3
		je	bpp_3
		cmp	ebx,4
		je	bpp_4
		ret

bpp_1:		mov	al,es:[edi]
		ret

bpp_2:		mov	ax,es:[edi]
		ret

bpp_4:		mov	eax,es:[edi]
		ret

bpp_3:		mov	ah,es:[edi+2]
		shl	eax,8
		mov	ah,es:[edi+1]
		mov	al,es:[edi+0]
     		ret
_MemPixelGet endp

; br_uint_16 BR_ASM_CALL _GetSysQual(void);
;
; Return a value for the qualifier argument that renders into system memory
;
_GetSysQual	proc
		mov	ax,ds
		ret
_GetSysQual	endp


; void BR_ASM_CALL _MemFillFPU_A(char *dest,
; 	br_uint_32 dest_qual, br_uint_32 pixels, br_uint_32 bpp,
; 	br_uint_32 colour);

_MemFillFPU_A proc uses ebx ecx edx edi ds es,
	dest: ptr byte, dest_qual:dword, pixels: dword, bpp: dword, colour:dword

	local	value64:qword
	local	old_fpu_cw:word

	; Set FPU to affine, extended precision, chop, all exceptions masked
	;
	; 9 cycles
	;
		fnstcw	old_fpu_cw
		fldcw	fpu_cw

	; Set es for alignment and finishing loops
	;
	; 3 cycles
	;
		mov	es,word ptr dest_qual

	; Select a routine based on bpp
	;
	; 2-5 cycles + 0-12 cycles branch misprediction
	;
		mov	ebx,bpp

		cmp	ebx,4
		je	bpp_4

		cmp	ebx,3
		je	bpp_3

		cmp	ebx,2
		je	bpp_2

		cmp	ebx,1
		je	bpp_1

	; Restore FPU control word
	;
	; 7 cycles
	;
		fldcw	old_fpu_cw

		ret

; 1 byte per pixel
;
bpp_1:

	; Replicate colour into 32 bits and calculate alignment
	;
	; 6 cycles + 0-3 cycles branch misprediction
	;
		mov	eax,colour
		mov	edi,dest

		mov	ebx,eax
		mov	ah,al

		shl	eax,16
		mov	ecx,8

		mov	al,bl
		sub	ecx,edi

		mov	ah,bl
		and	ecx,7

		mov	ebx,pixels
		jz	bpp_1_aligned
		
bpp_1_align:

	; Subtract alignment from pixel count, check if alignment covers
	; entire fill, perform alignment
	;
	; 11-17 cycles plus 0-3 cycles branch misprediction
	;
		sub	ebx,ecx
		jle	bpp_1_narrow

		rep	stosb

bpp_1_aligned:

	; Calculate number of quadwords and bytes to finish off
	;
	; 2 cycles + 0-3 cycles branch misprediction
	;
		mov	ecx,ebx
		and	ebx,7

		and	ecx,NOT 7
		jz	bpp_1_finish

	; Calculate number of complete and partial loops
	; Bias the pointer correctly for the partial loop iteration
	;
	; 2 cycles
	;
		mov	edx,ecx
		and	ecx,7*8

		shr	edx,6
		add	edi,ecx

	; Get the fill value into the FPU
	;
	; 2 cycles (+ latency of 2 cycles on fild)
	;
		mov	dword ptr value64,eax
		mov	dword ptr value64+4,eax

		fild	value64

	; Set ds for main loop
	; Start with partial iteration of the loop
	;
	; 5? cycles + 0-3 cycles branch misprediction
	; (segment override is masked I believe)
	;
		shr	ecx,1
		mov	ds,word ptr dest_qual
		jmp	cs:bpp_1_start_table[ecx]

bpp_1_start_table:
		dd	bpp_1_start
		dd	bpp_1_start7
		dd	bpp_1_start6
		dd	bpp_1_start5
		dd	bpp_1_start4
		dd	bpp_1_start3
		dd	bpp_1_start2
		dd	bpp_1_start1

bpp_1_narrow:

	; Do narrow fills entirely as bytes
	;
	; 12-18 cycles + 0-3 cycles branch misprediction
	;
		add	ecx,ebx

		rep	stosb

	; Restore FPU control word
	;
	; 7 cycles
	;
		fldcw	old_fpu_cw

		ret

bpp_1_start:

	; Bias loop count when no partial iteration
	;
	; 1 cycle + 0-3 cycles branch misprediction
	;
		dec	edx
		jl	bpp_1_end

	; Inner loop is unrolled 8 times
	;
	; 58 cycles per iteration + 0-3 cycles branch misprediction
	; 9-58 cycles for partial iteration
	; Total branch misprediction is 0 or 6 cycles
	;
bpp_1_inner:

		lea	edi,[edi+040h]

	for count,<0,1,2,3,4,5,6,7>

bpp_1_start&count:

		fld	st(0)
		fistp	qword ptr [edi]+count*8-040h

	endm
     	
		dec	edx
		jge	bpp_1_inner

bpp_1_end:

	; Clear the FPU stack
	;
	; 1 cycle
	;
		fstp	st(0)

bpp_1_finish:

	; Do final group of pixels
	;
	; 7-17 cycles
	;
		mov	ecx,ebx
		rep	stosb

	; Restore FPU control word
	;
	; 7 cycles
	;
		fldcw	old_fpu_cw

		ret

; 2 bytes per pixel
;
bpp_2:

	; Replicate colour into 32 bits and calculate alignment
	;
		mov	eax,colour
		mov	edi,dest

		mov	ebx,eax
		mov	ecx,8

		shl	eax,16
		sub	ecx,edi

		mov	al,bl
		and	ecx,6

		shr	ecx,1
		mov	ah,bh

		mov	ebx,pixels
		jz	bpp_2_aligned
		
	; Subtract alignment from pixel count, check if alignment covers
	; entire fill, perform alignment
	;
		sub	ebx,ecx
		jle	bpp_2_narrow

		rep	stosw

bpp_2_aligned:

	; Calculate number of quadwords and words to finish off
	;
		mov	ecx,ebx
		and	ebx,3

		and	ecx,NOT 3
		jz	bpp_2_finish

	; Calculate number of complete and partial loops
	; Bias the pointer correctly for the partial loop iteration
	;
		mov	edx,ecx
		and	ecx,7*4

		shr	edx,5
		lea	edi,[edi+ecx*2]

	; Get the fill value into the FPU
	;
		mov	dword ptr value64,eax
		mov	dword ptr value64+4,eax

		fild	value64

	; Set ds for main loop
	; Start with partial iteration of the loop
	;
		mov	ds,word ptr dest_qual
		jmp	cs:bpp_2_start_table[ecx]

bpp_2_start_table:
		dd	bpp_2_start
		dd	bpp_2_start7
		dd	bpp_2_start6
		dd	bpp_2_start5
		dd	bpp_2_start4
		dd	bpp_2_start3
		dd	bpp_2_start2
		dd	bpp_2_start1

bpp_2_narrow:

	; Do narrow fills entirely as words
	;
		add	ecx,ebx

		rep	stosw

	; Restore FPU control word
	;
		fldcw	old_fpu_cw

		ret

bpp_2_start:

	; Bias loop count when no partial iteration
	;
		dec	edx
		jl	bpp_2_end

	; Inner loop is unrolled 8 times
	;
bpp_2_inner:

		lea	edi,[edi+040h]

	for count,<0,1,2,3,4,5,6,7>

bpp_2_start&count:

		fld	st(0)
		fistp	qword ptr [edi]+count*8-040h

	endm
     	
		dec	edx
		jge	bpp_2_inner

bpp_2_end:

	; Clear the FPU stack
	;
		fstp	st(0)

bpp_2_finish:

	; Do final group of pixels
	;
		mov	ecx,ebx

		rep	stosw

	; Restore FPU control word
	;
		fldcw	old_fpu_cw

		ret

; 3 bytes per pixel
;
bpp_3:

	; If 3 fill bytes are the same, just use 1bpp fill
	;
		mov	eax,colour
		mov	edi,dest

		mov	ebx,eax
		mov	ecx,8

		ror	ebx,16
		sub	ecx,edi

		cmp	al,ah
		jne	real_3

		cmp	bl,al
		jne	real_3

	; Replicate colour into 32 bits and calculate alignment
	;
	; 6 cycles + 0-3 cycles branch misprediction
	;
		rol	eax,8
		mov	ebx,pixels

		mov	al,ah			; set top byte of colour
		and	ecx,7

		lea	ebx,[ebx*2+ebx]	; pixels *= 3
		jz	bpp_1_aligned
		
		jmp	bpp_1_align

real_3:

	; Fill eax, ebx, edx with 4 repeats of colour and calculate alignment
	;
							;  EAX  EBX  EDX
							;   HL   HL   HL
							; XRGB GBXR XXXX

		rol	eax,8			; RGBX GBXR XXXX
		mov	bh,ah			; RGBX GBBR XXXX

		mov	edx,eax			; RGBX GBBR RGBX
		mov	al,bl			; RGBR GBBR RGBX

		rol	edx,8			; RGBR GBBR GBXR
		mov	esi,pixels

		ror	ebx,16			; RGBR BRGB GBXR
		and	ecx,7

		mov	dh,dl			; RGBR BRGB GBRR
		mov	dl,bh			; RGBR BRGB GBRG
		
	; Look up number of pixels to next qword boundary starting on a pixel
	;
		mov	ecx,bpp_3_align_table[ecx*4]

		.data

bpp_3_align_table	dd		0
					dd		3
					dd		6
					dd		1
					dd		4
					dd		7
					dd		2
					dd		5

		.code

	; Set ds for pixel writes
	;
		mov	ds,word ptr dest_qual

	; Subtract alignment from pixel count, check if alignment covers
	; entire fill, perform alignment
	;
		sub	esi,ecx
		jle	bpp_3_narrow

		dec	ecx
		js	bpp_3_aligned

bpp_3_align_loop:

		mov	[edi+0],bl	; B
		mov	[edi+1],dl	; G

		mov	[edi+2],al	; R
		add	edi,3

		dec	ecx
		jns	bpp_3_align_loop
		
bpp_3_aligned:

	; Calculate number of complete and partial loops
	;
		mov	ecx,esi
		and	esi,15

		shr	ecx,4
		jz	bpp_3_finish

	; Get the fill values into the FPU
	;
		mov	dword ptr value64,edx
		mov	dword ptr value64+4,eax

		fild	value64

		mov	dword ptr value64,eax
		mov	dword ptr value64+4,ebx

		fild	value64

		mov	dword ptr value64,ebx
		mov	dword ptr value64+4,edx

		fild	value64

	; Inner loop is 6 qwords (16 pixels)
	;
bpp_3_inner:

	for count,<0,1>

		fld	st(0)
		fistp	qword ptr [edi]+(count*3+0)*8

		fld	st(1)
		fistp	qword ptr [edi]+(count*3+1)*8

		fld	st(2)
		fistp	qword ptr [edi]+(count*3+2)*8

	endm
     	
		lea	edi,[edi+6*8]

		dec	ecx
		jnz	bpp_3_inner

bpp_3_end:

	; Clear the FPU stack
	;
		fstp	st(0)
		fstp	st(0)
		fstp	st(0)

bpp_3_finish:

	; Do final group of pixels
	;
		dec	esi
		js	bpp_3_done

bpp_3_finish_loop:

		mov	[edi+0],bl	; B
		mov	[edi+1],dl	; G

		mov	[edi+2],al	; R
		add	edi,3

		dec	esi
		jns	bpp_3_finish_loop
		
bpp_3_done:

	; Restore FPU control word
	;
	; 7 cycles
	;
		fldcw	old_fpu_cw

		ret

bpp_3_narrow:

	; Do narrow fills entirely as bytes
	;
	; 12-18 cycles + 0-3 cycles branch misprediction
	;
		add	esi,ecx
		jz	bpp_3_done

bpp_3_narrow_loop:

		mov	[edi+0],bl	; B
		mov	[edi+1],dl	; G

		mov	[edi+2],al	; R
		add	edi,3

		dec	esi
		jnz	bpp_3_narrow_loop
		
	; Restore FPU control word
	;
	; 7 cycles
	;
		fldcw	old_fpu_cw

		ret

; 4 bytes per pixel
;
bpp_4:

	; Calculate alignment
	;
		mov	eax,colour
		mov	edi,dest

		mov	ebx,pixels

		test edi,4
		jz	bpp_4_aligned
		
	; Subtract alignment from pixel count, check if alignment covers
	; entire fill, perform alignment
	;
		mov	es:[edi],eax

		add	edi,4
		dec	ebx

bpp_4_aligned:

	; Calculate number of quadwords and words to finish off
	;
		mov	ecx,ebx
		and	ebx,1

		and	ecx,NOT 1
		jz	bpp_4_finish

	; Calculate number of complete and partial loops
	; Bias the pointer correctly for the partial loop iteration
	;
		mov	edx,ecx
		and	ecx,7*2

		shr	edx,4
		lea	edi,[edi+ecx*4]

	; Get the fill value into the FPU
	;
		mov	dword ptr value64,eax
		mov	dword ptr value64+4,eax

		fild	value64

	; Set ds for main loop
	; Start with partial iteration of the loop
	;
		mov	ds,word ptr dest_qual
		jmp	cs:bpp_4_start_table[ecx*2]

bpp_4_start_table:
		dd	bpp_4_start
		dd	bpp_4_start7
		dd	bpp_4_start6
		dd	bpp_4_start5
		dd	bpp_4_start4
		dd	bpp_4_start3
		dd	bpp_4_start2
		dd	bpp_4_start1

bpp_4_start:

	; Bias loop count when no partial iteration
	;
		dec	edx
		jl	bpp_4_end

	; Inner loop is unrolled 8 times
	;
bpp_4_inner:

		lea	edi,[edi+040h]

	for count,<0,1,2,3,4,5,6,7>

bpp_4_start&count:

		fld	st(0)
		fistp	qword ptr [edi]+count*8-040h

	endm
     	
		dec	edx
		jge	bpp_4_inner

bpp_4_end:

	; Clear the FPU stack
	;
		fstp	st(0)

bpp_4_finish:

	; Do final pixel
	;
		test	ebx,ebx
		jz	bpp_4_done

		mov	es:[edi],eax

bpp_4_done:

	; Restore FPU control word
	;
		fldcw	old_fpu_cw

		ret

_MemFillFPU_A	endp


; void BR_ASM_CALL _MemRectFillFPU_A(char *dest,
; 	br_uint_32 dest_qual, br_uint_32 pwidth, br_uint_32 pheight,
;	br_uint_32 stride, br_uint_32 bpp, br_uint_32 colour);

; *** IMPORTANT ***
;
; This routine requires that the stride is a multiple of 8

_MemRectFillFPU_A proc uses ebx ecx edx esi edi ds es,
	dest: ptr byte, dest_qual:dword, pwidth: dword, pheight: dword, stride: dword, bpp: dword, colour:dword

	local	value64:qword
	local	old_fpu_cw:word
	local	align_count:dword
	local	partial_count:dword
	local	loop_count:dword
	local	finish_count:dword

	; Set FPU to affine, extended precision, chop, all exceptions masked
	;
	; 9 cycles
	;
		fnstcw	old_fpu_cw
		fldcw	fpu_cw

	; Set ds for main loop
	; Set es for alignment and finishing loops
	;
	; 6 cycles
	;
		mov	ds,word ptr dest_qual
		mov	es,word ptr dest_qual

	; Select a routine based on bpp
	;
	; 2-5 cycles + 0-12 cycles branch misprediction
	;
		mov	ebx,bpp

		cmp	ebx,4
		je	bpp_4

		cmp	ebx,3
		je	bpp_3

		cmp	ebx,2
		je	bpp_2

		cmp	ebx,1
		je	bpp_1

	; Restore FPU control word
	;
	; 7 cycles
	;
		fldcw	old_fpu_cw

		ret

; 1 byte per pixel
;
bpp_1:

	; Replicate colour into 32 bits and calculate alignment
	;
	; 6 cycles
	;
		mov	eax,colour
		mov	edi,dest

		mov	ebx,eax
		mov	ah,al

		shl	eax,16
		mov	ecx,8

		mov	al,bl
		sub	ecx,edi

		mov	ah,bl
		and	ecx,7

		mov	align_count,ecx
		mov	ebx,pwidth
		
bpp_1_align:

	; Subtract alignment from pixel count, check if alignment covers
	; entire fill and perform using separate loop
	;
	; 1 cycle + 0-3 cycles branch misprediction
	;
		sub	ebx,ecx
		jle	bpp_1_narrow

	; Calculate number of quadwords and bytes to finish off
	;
	; 2 cycles
	;
		mov	ecx,ebx
		and	ebx,7

		mov	finish_count,ebx
		and	ecx,NOT 7

	; Calculate number of complete and partial loops
	;
	; 3 cycles
	;
		mov	ebx,ecx
		and	ecx,7*8

		shr	ebx,6
		mov	partial_count,ecx

		mov	loop_count,ebx

	; Prepare for outer loop
	;
	; 1 cycle
	;
		mov	ebx,pheight
		mov	esi,stride

	; Get the fill value into the FPU
	;
	; 2 cycles (+ latency of 2 cycles on fild)
	;
		mov	dword ptr value64,eax
		mov	dword ptr value64+4,eax

		fild	value64

bpp_1_outer:

	; Do alignment
	;
	; 10-17 cycles
	;
		push	edi
		mov	ecx,align_count

		rep	stosb

	; Read counts for main loop
	;
	; 1 cycle
	;
		mov	ecx,partial_count
		mov	edx,loop_count

	; Start with partial iteration of the loop
	; Bias the pointer correctly for the partial loop iteration
	;
	; 5? cycles + 0-3 cycles branch misprediction
	;
		add	edi,ecx

		shr	ecx,1

		jmp	cs:bpp_1_start_table[ecx]

bpp_1_start_table:
		dd	bpp_1_start
		dd	bpp_1_start7
		dd	bpp_1_start6
		dd	bpp_1_start5
		dd	bpp_1_start4
		dd	bpp_1_start3
		dd	bpp_1_start2
		dd	bpp_1_start1

bpp_1_start:

	; Bias loop count when no partial iteration
	;
	; 1 cycle + 0-3 cycles branch misprediction
	;
		dec	edx
		jl	bpp_1_end

	; Inner loop is unrolled 8 times
	;
	; 58 cycles per iteration + 0-3 cycles branch misprediction
	; 9-58 cycles for partial iteration
	; Total branch misprediction is 0 or 6 cycles
	;
bpp_1_inner:

		lea	edi,[edi+040h]

	for count,<0,1,2,3,4,5,6,7>

bpp_1_start&count:

		fld	st(0)
		fistp	qword ptr [edi]+count*8-040h

	endm
     	
		dec	edx
		jge	bpp_1_inner

bpp_1_end:

	; Do final group of pixels
	;
	; 7-17 cycles
	;
		mov	ecx,finish_count

		rep	stosb

	; Loop for next line
	;
	; 4 cycles + 0-3 cycles branch misprediction
	;
		pop	edi

		add	edi,esi

		dec	ebx
		jnz	bpp_1_outer

	; Clear the FPU stack
	;
	; 1 cycle
	;
		fstp	st(0)

	; Restore FPU control word
	;
	; 7 cycles
	;
		fldcw	old_fpu_cw

		ret

bpp_1_narrow:

	; Do narrow fills entirely as bytes
	;
		add	ecx,ebx
		mov	ebx,pheight

		mov	esi,stride
		mov	edx,ecx

bpp_1_narrow_loop:

		push	edi

		rep	stosb

		pop	edi

		add	edi,esi
		mov	ecx,edx

		dec	ebx
		jnz	bpp_1_narrow_loop

	; Restore FPU control word
	;
	; 7 cycles
	;
		fldcw	old_fpu_cw

		ret

; 2 bytes per pixel
;
bpp_2:

	; Replicate colour into 32 bits and calculate alignment
	;
		mov	eax,colour
		mov	edi,dest

		mov	ebx,eax
		mov	ecx,8

		shl	eax,16
		sub	ecx,edi

		mov	al,bl
		and	ecx,6

		mov	ah,bh
		mov	align_count,ecx

		mov	ebx,pwidth
		
	; Subtract alignment from pixel count, check if alignment covers
	; entire fill and perform using separate loop
	;
		sub	ebx,ecx
		jle	bpp_2_narrow

	; Calculate number of quadwords and bytes to finish off
	;
		mov	ecx,ebx
		and	ebx,3

		mov	finish_count,ebx
		and	ecx,NOT 3

	; Calculate number of complete and partial loops
	;
		mov	ebx,ecx
		and	ecx,7*4

		shr	ebx,5
		mov	partial_count,ecx

		mov	loop_count,ebx

	; Prepare for outer loop
	;
		mov	ebx,pheight
		mov	esi,stride

	; Get the fill value into the FPU
	;
		mov	dword ptr value64,eax
		mov	dword ptr value64+4,eax

		fild	value64

bpp_2_outer:

	; Do alignment
	;
		push	edi
		mov	ecx,align_count

		rep	stosw

	; Read counts for main loop
	;
		mov	ecx,partial_count
		mov	edx,loop_count

	; Start with partial iteration of the loop
	; Bias the pointer correctly for the partial loop iteration
	;
		lea	edi,[edi+ecx*2]

		jmp	cs:bpp_2_start_table[ecx]

bpp_2_start_table:
		dd	bpp_2_start
		dd	bpp_2_start7
		dd	bpp_2_start6
		dd	bpp_2_start5
		dd	bpp_2_start4
		dd	bpp_2_start3
		dd	bpp_2_start2
		dd	bpp_2_start1

bpp_2_start:

	; Bias loop count when no partial iteration
	;
		dec	edx
		jl	bpp_2_end

	; Inner loop is unrolled 8 times
	;
bpp_2_inner:

		lea	edi,[edi+040h]

	for count,<0,1,2,3,4,5,6,7>

bpp_2_start&count:

		fld	st(0)
		fistp	qword ptr [edi]+count*8-040h

	endm
     	
		dec	edx
		jge	bpp_2_inner

bpp_2_end:

	; Do final group of pixels
	;
		mov	ecx,finish_count

		rep	stosw

	; Loop for next line
	;
		pop	edi

		add	edi,esi

		dec	ebx
		jnz	bpp_2_outer

	; Clear the FPU stack
	;
		fstp	st(0)

	; Restore FPU control word
	;
		fldcw	old_fpu_cw

		ret

bpp_2_narrow:

	; Do narrow fills entirely as words
	;
		add	ecx,ebx
		mov	ebx,pheight

		mov	esi,stride
		mov	edx,ecx

bpp_2_narrow_loop:

		push	edi

		rep	stosw

		pop	edi

		add	edi,esi
		mov	ecx,edx

		dec	ebx
		jnz	bpp_2_narrow_loop

	; Restore FPU control word
	;
		fldcw	old_fpu_cw

		ret

; 3 bytes per pixel
;
bpp_3:

	; If 3 fill bytes are the same, just use 1bpp fill
	;
		mov	eax,colour
		mov	edi,dest

		mov	ebx,eax
		mov	ecx,8

		ror	ebx,16
		sub	ecx,edi

		cmp	al,ah
		jne	real_3

		cmp	bl,al
		jne	real_3

	; Replicate colour into 32 bits and calculate alignment
	;
	; 6 cycles + 0-3 cycles branch misprediction
	;
		rol	eax,8
		mov	ebx,pwidth

		mov	al,ah			; set top byte of colour
		and	ecx,7

		lea	ebx,[ebx*2+ebx]	; pixels *= 3
		mov	align_count,ecx

		jmp	bpp_1_align

real_3:

	; Fill eax, ebx, edx with 4 repeats of colour and calculate alignment
	;
							;  EAX  EBX  EDX
							;   HL   HL   HL
							; XRGB GBXR XXXX

		rol	eax,8			; RGBX GBXR XXXX
		mov	bh,ah			; RGBX GBBR XXXX

		mov	edx,eax			; RGBX GBBR RGBX
		mov	al,bl			; RGBR GBBR RGBX

		rol	edx,8			; RGBR GBBR GBXR
		mov	esi,pwidth

		ror	ebx,16			; RGBR BRGB GBXR
		and	ecx,7

		mov	dh,dl			; RGBR BRGB GBRR
		mov	dl,bh			; RGBR BRGB GBRG
		
	; Look up number of pixels to next qword boundary starting on a pixel
	;
		mov	ecx,bpp_3_align_table[ecx*4]

		mov	align_count,ecx

	; Subtract alignment from pixel count, check if alignment covers
	; entire fill and perform  using separate loop
	;
		sub	esi,ecx
		jle	bpp_3_narrow

	; Calculate number of complete and partial loops
	;
		mov	ecx,esi
		and	esi,15

		shr	ecx,4
		mov	finish_count,esi

		mov	loop_count,ecx

	; Get the fill values into the FPU
	;
		mov	dword ptr value64,edx
		mov	dword ptr value64+4,eax

		fild	value64

		mov	dword ptr value64,eax
		mov	dword ptr value64+4,ebx

		fild	value64

		mov	dword ptr value64,ebx
		mov	dword ptr value64+4,edx

		fild	value64

	; Prepare for outer loop
	;
	; 1 cycle
	;
		mov	ebx,pheight
		mov	esi,stride

bpp_3_outer:

	; Do alignment
	;
		push	edi
		mov	ecx,align_count

		dec	ecx
		js	bpp_3_aligned

bpp_3_align_loop:

		mov	[edi+0],ah	; B
		mov	[edi+1],dl	; G

		mov	[edi+2],al	; R
		add	edi,3

		dec	ecx
		jns	bpp_3_align_loop
		
bpp_3_aligned:

		mov	ecx,loop_count

		test	ecx,ecx
		jz	bpp_3_finish

	; Inner loop is 6 qwords (16 pixels)
	;
bpp_3_inner:

	for count,<0,1>

		fld	st(0)
		fistp	qword ptr [edi]+(count*3+0)*8

		fld	st(1)
		fistp	qword ptr [edi]+(count*3+1)*8

		fld	st(2)
		fistp	qword ptr [edi]+(count*3+2)*8

	endm
     	
		lea	edi,[edi+6*8]

		dec	ecx
		jnz	bpp_3_inner

bpp_3_finish:

	; Do final group of pixels
	;
		mov	ecx,finish_count

		dec	ecx
		js	bpp_3_finished

bpp_3_finish_loop:

		mov	[edi+0],ah	; B
		mov	[edi+1],dl	; G

		mov	[edi+2],al	; R
		add	edi,3

		dec	ecx
		jns	bpp_3_finish_loop
		
bpp_3_finished:

	; Loop for next line
	;
		pop	edi

		add	edi,esi

		dec	ebx
		jnz	bpp_3_outer

	; Clear the FPU stack
	;
		fstp	st(0)
		fstp	st(0)
		fstp	st(0)

bpp_3_done:

	; Restore FPU control word
	;
	; 7 cycles
	;
		fldcw	old_fpu_cw

		ret

bpp_3_narrow:

	; Do narrow fills entirely as bytes
	;
		add	ecx,esi
		jz	bpp_3_done

		mov	ebx,pheight
		mov	esi,stride

bpp_3_narrow_line:

		push	ecx
		push	edi

bpp_3_narrow_loop:

		mov	[edi+0],ah	; B
		mov	[edi+1],dl	; G

		mov	[edi+2],al	; R
		add	edi,3

		dec	ecx
		jnz	bpp_3_narrow_loop
		
		pop	edi
		pop	ecx

		add	edi,esi

		dec	ebx
		jnz	bpp_3_narrow_line

	; Restore FPU control word
	;
	; 7 cycles
	;
		fldcw	old_fpu_cw

		ret

; 4 bytes per pixel
;
bpp_4:

	; Calculate alignment
	;
		mov	eax,colour
		mov	edi,dest

		mov	ebx,edi
		mov	ecx,pwidth

		test	ebx,4
		jz	bpp_4_no_align

	; Subtract alignment from pixel count
	;
		dec	ecx

bpp_4_no_align:

	; Calculate number of quadwords and bytes to finish off
	;
		mov	finish_count,ecx
		and	ecx,NOT 1

	; Calculate number of complete and partial loops
	;
		mov	ebx,ecx
		and	ecx,7*2

		shr	ebx,4
		mov	partial_count,ecx

		mov	loop_count,ebx

	; Prepare for outer loop
	;
		mov	ebx,pheight
		mov	esi,stride

	; Get the fill value into the FPU
	;
		mov	dword ptr value64,eax
		mov	dword ptr value64+4,eax

		fild	value64

bpp_4_outer:

	; Do alignment
	;
		push	edi

		test	edi,4
		jz	bpp_4_aligned

		mov	es:[edi],eax
		add	edi,4

bpp_4_aligned:

	; Read counts for main loop
	;
		mov	ecx,partial_count
		mov	edx,loop_count

	; Start with partial iteration of the loop
	; Bias the pointer correctly for the partial loop iteration
	;
		lea	edi,[edi+ecx*4]

		jmp	cs:bpp_4_start_table[ecx*2]

bpp_4_start_table:
		dd	bpp_4_start
		dd	bpp_4_start7
		dd	bpp_4_start6
		dd	bpp_4_start5
		dd	bpp_4_start4
		dd	bpp_4_start3
		dd	bpp_4_start2
		dd	bpp_4_start1

bpp_4_start:

	; Bias loop count when no partial iteration
	;
		dec	edx
		jl	bpp_4_end

	; Inner loop is unrolled 8 times
	;
bpp_4_inner:

		lea	edi,[edi+040h]

	for count,<0,1,2,3,4,5,6,7>

bpp_4_start&count:

		fld	st(0)
		fistp	qword ptr [edi]+count*8-040h

	endm
     	
		dec	edx
		jge	bpp_4_inner

bpp_4_end:

	; Do final pixel
	;
		mov	ecx,finish_count

		test ecx,1
		jz	bpp_4_done

		mov	es:[edi],eax

bpp_4_done:

	; Loop for next line
	;
		pop	edi

		add	edi,esi

		dec	ebx
		jnz	bpp_4_outer

	; Clear the FPU stack
	;
		fstp	st(0)

	; Restore FPU control word
	;
		fldcw	old_fpu_cw

		ret

_MemRectFillFPU_A	endp


; void BR_ASM_CALL _MemCopyFPU_A(char *dest,
; 	br_uint_32 dest_qual, char *src, br_uint_32 src_qualifier, 
; 	 br_uint_32 pixels, br_uint_32 bpp);

_MemCopyFPU_A proc uses ebx ecx edx esi edi ds es,
	dest: ptr byte, dest_qual:dword, src:ptr byte, src_qual:dword,
	pixels: dword, bpp: dword

	local	old_fpu_cw:word

	; Set FPU to affine, extended precision, chop, all exceptions masked
	;
	; 9 cycles
	;
		fnstcw	old_fpu_cw
		fldcw	fpu_cw

	; Get source and destination pointers
	;
	; 7 cycles
	;
		mov	ds,word ptr src_qual
		mov	es,word ptr dest_qual

		mov	esi,src
		mov	edi,dest

	; Calculate byte count and alignment for destination
	;
	; 13 cycles + 0-3 cycles branch misprediction
	;
		mov	eax,bpp
		mov	ecx,8

	   	mul	pixels

		sub	ecx,edi

		and	ecx,7
		jz 	aligned
		
	; Subtract alignment from byte count, check if alignment covers
	; entire copy, perform alignment
	;
	; 15-21 cycles plus 0-3 cycles branch misprediction
	;
		sub	eax,ecx
		jle	narrow

		rep	movsb

aligned:

	; Calculate number of quadwords and bytes to finish off
	;
	; 2 cycles + 0-3 cycles branch misprediction
	;
		mov	ebx,eax
		and	eax,7

		shr	ebx,3
		jz	finish_bytes

	; Calculate number of complete and partial loops
	;
	; 2 cycles + 0-3 cycles branch misprediction
	;
		mov	ecx,ebx
		and	ebx,31

		shr	ecx,5
		jz	finish_qwords

	; Bias the pointers for the copy
	;
	; 1 cycle
	;
		add	esi,080h
		add	edi,080h

	; Outer loop copies 32 qwords
	;
	; 245 cycles per iteration + 0-3 cycles branch misprediction
	; Total branch misprediction is 0 or 6 cycles
	;
outer:

		mov	edx,4

	; Inner loop copies 8 qwords 4 qwords apart
	;
	; N.B. segment override does not appear to affect performance but
	; intra-segment copies could be handled by a separate routine if
	; necessary
	;
	; 59 cycles per iteration + 0-3 cycles branch misprediction
	; Total branch misprediction is 6 cycles
	;
inner:
		fild	qword ptr [esi]+000h-080h
		fild	qword ptr [esi]+020h-080h
		fild	qword ptr [esi]+040h-080h
		fild	qword ptr [esi]+060h-080h
		fild	qword ptr [esi]+080h-080h
		fild	qword ptr [esi]+0A0h-080h
		fild	qword ptr [esi]+0C0h-080h
		fild	qword ptr [esi]+0E0h-080h
		fxch
		fistp	qword ptr es:[edi]+0C0h-080h
		fistp	qword ptr es:[edi]+0E0h-080h
		fistp	qword ptr es:[edi]+0A0h-080h
		fistp	qword ptr es:[edi]+080h-080h
		fistp	qword ptr es:[edi]+060h-080h
		fistp	qword ptr es:[edi]+040h-080h
		fistp	qword ptr es:[edi]+020h-080h
		fistp	qword ptr es:[edi]+000h-080h
     	
		lea	esi,[esi+8]
		lea	edi,[edi+8]

		dec	edx
		jne	inner

		add	esi,224
		add	edi,224

		dec	ecx
		jne	outer

	; Unbias the pointers
	;
	; 1 cycle
	;
		sub	esi,080h
		sub	edi,080h

	; Copy remaining 0-31 qwords
	;
	; 1-342 cycles + 0-9 cycles branch misprediction
	;
finish_qwords:

		test	ebx,ebx
		jz	finish_bytes

qwords:
		fild	qword ptr [esi]
		fistp	qword ptr es:[edi]

		add	esi,8
		add	edi,8

		dec	ebx
		jnz	qwords

finish_bytes:

	; Copy remaining 0-7 bytes
	;
	; 11-21 cycles
	;
		mov	ecx,eax

		rep	movsb

	; Restore FPU control word
	;
	; 7 cycles
	;
		fldcw	old_fpu_cw

		ret

narrow:

	; Do narrow copies entirely as bytes
	;
	; 16-22 cycles + 0-3 cycles branch misprediction
	;
		add	ecx,eax

		rep	movsb

	; Restore FPU control word
	;
	; 7 cycles
	;
		fldcw	old_fpu_cw

		ret

_MemCopyFPU_A	endp


; void BR_ASM_CALL _MemRectCopyFPU_A(char *dest, br_uint_32 dest_qual, char *src, br_uint_32 src_qual,
;				  br_uint_32 pwidth, br_uint_32 pheight,
; 				  br_int_32 d_stride,br_int_32 s_stride,
;				  br_uint_32 bpp);
;
; *** IMPORTANT ***
;
; This routine requires that the destination stride is a multiple of 8

_MemRectCopyFPU_A proc uses ebx esi edi ds es,
	dest: ptr byte, dest_qual:dword, src:ptr byte, src_qual: dword,
	pwidth: dword, pheight: dword, d_stride:dword, s_stride:dword, bpp: dword

	local	old_fpu_cw:word
	local	align_count:dword
	local	loop_count:dword
	local	qword_count:dword
	local	byte_count:dword

	; Set FPU to affine, extended precision, chop, all exceptions masked
	;
	; 9 cycles
	;
		fnstcw	old_fpu_cw
		fldcw	fpu_cw

	; Get source and destination pointers
	;
	; 7 cycles
	;
		mov	ds,word ptr src_qual
		mov	es,word ptr dest_qual

		mov	esi,src
		mov	edi,dest

	; Calculate byte count and alignment for destination
	;
	; 13 cycles
	;
		mov	eax,bpp
		mov	ecx,8

	   	mul	pwidth

		sub	ecx,edi

		and	ecx,7
		
	; Subtract alignment from byte count, check if alignment covers
	; entire copy, perform alignment
	;
	; 15-21 cycles plus 0-3 cycles branch misprediction
	;
		sub	eax,ecx
		jle	narrow

	; Calculate number of quadwords and bytes to finish off
	;
	; 2 cycles + 0-3 cycles branch misprediction
	;
		mov	ebx,eax
		and	eax,7

		shr	ebx,3
		mov	align_count,ecx

	; Calculate number of complete and partial loops
	;
	; 2 cycles + 0-3 cycles branch misprediction
	;
		mov	ecx,ebx
		and	ebx,31

		shr	ecx,5
		mov	byte_count,eax

		mov	qword_count,ebx
		mov	loop_count,ecx

	; Prepare for outer loop
	;
	; 1 cycle
	;
		mov	eax,pheight

line:

	; Do alignment
	;
	; 11-18 cycles
	;
		push	esi
		push	edi

		mov	ecx,align_count

		rep	movsb

	; Read counts for main loop
	;
	; 2 cycles
	;
		mov	ecx,loop_count
		mov	ebx,qword_count

		test	ecx,ecx
		jz	finish_qwords

	; Bias the pointers for the copy
	;
	; 1 cycle
	;
		add	esi,080h
		add	edi,080h

	; Outer loop copies 32 qwords
	;
	; 245 cycles per iteration + 0-3 cycles branch misprediction
	; Total branch misprediction is 0 or 6 cycles
	;
outer:

		mov	edx,4

	; Inner loop copies 8 qwords 4 qwords apart
	;
	; N.B. segment override does not appear to affect performance but
	; intra-segment copies could be handled by a separate routine if
	; necessary
	;
	; 59 cycles per iteration + 0-3 cycles branch misprediction
	; Total branch misprediction is 6 cycles
	;
inner:
		fild	qword ptr [esi]+000h-080h
		fild	qword ptr [esi]+020h-080h
		fild	qword ptr [esi]+040h-080h
		fild	qword ptr [esi]+060h-080h
		fild	qword ptr [esi]+080h-080h
		fild	qword ptr [esi]+0A0h-080h
		fild	qword ptr [esi]+0C0h-080h
		fild	qword ptr [esi]+0E0h-080h
		fxch
		fistp	qword ptr es:[edi]+0C0h-080h
		fistp	qword ptr es:[edi]+0E0h-080h
		fistp	qword ptr es:[edi]+0A0h-080h
		fistp	qword ptr es:[edi]+080h-080h
		fistp	qword ptr es:[edi]+060h-080h
		fistp	qword ptr es:[edi]+040h-080h
		fistp	qword ptr es:[edi]+020h-080h
		fistp	qword ptr es:[edi]+000h-080h
     	
		lea	esi,[esi+8]
		lea	edi,[edi+8]

		dec	edx
		jne	inner

		add	esi,224
		add	edi,224

		dec	ecx
		jne	outer

	; Unbias the pointers
	;
	; 1 cycle
	;
		sub	esi,080h
		sub	edi,080h

	; Copy remaining 0-31 qwords
	;
	; 1-342 cycles + 0-9 cycles branch misprediction
	;
finish_qwords:

		test	ebx,ebx
		jz	finish_bytes

qwords:
		fild	qword ptr [esi]
		fistp	qword ptr es:[edi]

		add	esi,8
		add	edi,8

		dec	ebx
		jnz	qwords

finish_bytes:

	; Copy remaining 0-7 bytes
	;
	; 11-21 cycles
	;
		mov	ecx,byte_count

		rep	movsb

	; Loop for next line
	;
	; 4 cycles + 0-3 cycles branch misprediction
	;
		pop	edi
		pop	esi

		add	esi,s_stride
		add	edi,d_stride

		dec	eax
		jnz	line

done:

	; Restore FPU control word
	;
	; 7 cycles
	;
		fldcw	old_fpu_cw

		ret

narrow:

	; Do narrow copies entirely as bytes
	;

	; Prepare for outer loop
	;
	; 2 cycles
	;
		add	ecx,eax
		mov	eax,pheight

		mov	ebx,ecx

narrow_loop:

		push	esi
		push	edi

		rep	movsb

	; Loop for next line
	;
	; 4 cycles + 0-3 cycles branch misprediction
	;
		pop	edi
		pop	esi

		add	esi,s_stride
		add	edi,d_stride

		mov	ecx,ebx

		dec	eax
		jnz	narrow_loop

		jmp	done

_MemRectCopyFPU_A	endp

	end

