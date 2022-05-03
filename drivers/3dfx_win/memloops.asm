;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: memloops.asm 1.2 1998/02/23 19:26:20 jon Exp $
;; $Locker: $
;;
;; Inner loops for operations on in-memory pixelmaps
;;
	.386p
	.model flat,c

	.data

if ADD_RCS_ID
	db '$Id: memloops.asm 1.2 1998/02/23 19:26:20 jon Exp $',0
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

; void BR_ASM_CALL _MemFill3DFX_A(char *dest,
; 	br_uint_32 dest_qual, br_uint_32 pixels,
; 	br_uint_32 colour);

_MemFill3DFX_A proc uses ebx ecx edx edi ds es,
	dest: ptr byte, dest_qual:dword, pixels: dword, colour:dword

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
		jz	aligned
		
	; Subtract alignment from pixel count, check if alignment covers
	; entire fill, perform alignment
	;
		sub	ebx,ecx
		jle	narrow

		rep	stosw

aligned:

	; Calculate number of quadwords and words to finish off
	;
		mov	ecx,ebx
		and	ebx,3

		and	ecx,NOT 3
		jz	finish

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
		jmp	cs:start_table[ecx]

start_table:
		dd	start
		dd	start7
		dd	start6
		dd	start5
		dd	start4
		dd	start3
		dd	start2
		dd	start1

narrow:

	; Do narrow fills entirely as words
	;
		add	ecx,ebx

		rep	stosw

	; Restore FPU control word
	;
		fldcw	old_fpu_cw

		ret

start:

	; Bias loop count when no partial iteration
	;
		dec	edx
		jl	done

	; Inner loop is unrolled 8 times
	;
inner:

		lea	edi,[edi+040h]

	for count,<0,1,2,3,4,5,6,7>

start&count:

		fld	st(0)
		fistp	qword ptr [edi]+count*8-040h

	endm
     	
		dec	edx
		jge	inner

done:

	; Clear the FPU stack
	;
		fstp	st(0)

finish:

	; Do final group of pixels
	;
		mov	ecx,ebx

		rep	stosw

	; Restore FPU control word
	;
		fldcw	old_fpu_cw

		ret

_MemFill3DFX_A	endp


; void BR_ASM_CALL _MemRectFill3DFX_A(char *dest,
; 	br_uint_32 dest_qual, br_uint_32 pwidth, br_uint_32 pheight,
;	br_uint_32 stride, br_uint_32 colour);

; *** IMPORTANT ***
;
; This routine requires that the stride is a multiple of 8

_MemRectFill3DFX_A proc uses ebx ecx edx esi edi ds es,
	dest: ptr byte, dest_qual:dword, pwidth: dword, pheight: dword, stride: dword, colour:dword

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
		jle	narrow

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

outer:

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

		jmp	cs:start_table[ecx]

start_table:
		dd	start
		dd	start7
		dd	start6
		dd	start5
		dd	start4
		dd	start3
		dd	start2
		dd	start1

start:

	; Bias loop count when no partial iteration
	;
		dec	edx
		jl	done

	; Inner loop is unrolled 8 times
	;
inner:

		lea	edi,[edi+040h]

	for count,<0,1,2,3,4,5,6,7>

start&count:

		fld	st(0)
		fistp	qword ptr [edi]+count*8-040h

	endm
     	
		dec	edx
		jge	inner

done:

	; Do final group of pixels
	;
		mov	ecx,finish_count

		rep	stosw

	; Loop for next line
	;
		pop	edi

		add	edi,esi

		dec	ebx
		jnz	outer

	; Clear the FPU stack
	;
		fstp	st(0)

	; Restore FPU control word
	;
		fldcw	old_fpu_cw

		ret

narrow:

	; Do narrow fills entirely as words
	;
		add	ecx,ebx
		mov	ebx,pheight

		mov	esi,stride
		mov	edx,ecx

narrow_loop:

		push	edi

		rep	stosw

		pop	edi

		add	edi,esi
		mov	ecx,edx

		dec	ebx
		jnz	narrow_loop

	; Restore FPU control word
	;
		fldcw	old_fpu_cw

		ret

_MemRectFill3DFX_A	endp


; void BR_ASM_CALL _MemCopy3DFX_A(char *dest,
; 	br_uint_32 dest_qual, char *src, br_uint_32 src_qualifier, 
; 	 br_uint_32 pixels);

_MemCopy3DFX_A proc uses ebx ecx edx esi edi ds es,
	dest: ptr byte, dest_qual:dword, src:ptr byte, src_qual:dword,
	pixels: dword

	local	old_fpu_cw:word

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
	; 13 cycles + 0-3 cycles branch misprediction
	;
		mov	eax,pixels
		mov	ecx,8

		add	eax,eax
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

_MemCopy3DFX_A	endp


; void BR_ASM_CALL _MemRectCopy3DFX_A(char *dest, br_uint_32 dest_qual, char *src, br_uint_32 src_qual,
;				  br_uint_32 pwidth, br_uint_32 pheight,
; 				  br_int_32 d_stride,br_int_32 s_stride);
;
; *** IMPORTANT ***
;
; This routine requires that the destination stride is a multiple of 8

_MemRectCopy3DFX_A proc uses ebx esi edi ds es,
	dest: ptr byte, dest_qual:dword, src:ptr byte, src_qual: dword,
	pwidth: dword, pheight: dword, d_stride:dword, s_stride:dword

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
		mov	eax,pwidth
		mov	ecx,8

	   	add	eax,eax
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

_MemRectCopy3DFX_A	endp

	end

