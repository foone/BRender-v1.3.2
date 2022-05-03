;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: vesaloop.asm 1.1 1997/12/10 16:54:37 jon Exp $
;; $Locker: $
;;
	.386p
	.model flat,c

	include drv.inc

	.data
		db '$Id: vesaloop.asm 1.1 1997/12/10 16:54:37 jon Exp $',0
		align 4
	.data

; Macro to make getting at vesa work variables easier
;
vw	equ	<DriverDeviceVESA.work>

SCANLINE_SHIFT	equ	4

	if (sizeof vesa_scanline) NE (1 shl SCANLINE_SHIFT)
	.err <sizeof(vesa_scanline) is assumed to be 1<<SCANLINE_SHIFT>
	endif

; Scratch variables
;	
num_bytes		dd 0
byte_width		dd 0
break_in_scanline	dd 0
scanline_page		dd 0
temp_colour		dd 0

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

	; set VESA page to edx - trashes ebx
	;
SET_PAGE	macro
		local	no_change

		cmp	edx,vw.current_page
		je	no_change
		push	ebx
		mov	vw.current_page,edx
		xor	ebx,ebx
		mov	eax,04f05h
		call	vw.bank_function
		pop	ebx
no_change:
		endm

SET_PAGE_FORCE	macro
		push	ebx
		xor	ebx,ebx
		mov	eax,04f05h
		mov	vw.current_page,edx
		call	vw.bank_function
		pop	ebx
		endm

	; Move to next vesa page
	;
INC_PAGE	macro
		push	edx
		push	ebx
		mov	edx,vw.current_page
		add	edx,vw.bank_increment
		mov	vw.current_page,edx
		xor	ebx,ebx
		mov	eax,04f05h
		call	vw.bank_function

		pop	ebx	
		pop	edx	

		endm

REP_MOVSB_R	macro
		local	copy_loop,none
		and	ecx,ecx
		je	none
copy_loop:	mov	al,es:[edi]
		mov	fs:[esi],al
		inc	edi
		inc	esi
		dec	ecx
		jne	copy_loop
none:
		endm

REP_MOVSD_R		macro
		local	copy_loop,none
		and	ecx,ecx
		je	none
copy_loop:	mov	eax,es:[edi]
		mov	fs:[esi],eax
		add	edi,4
		add	esi,4
		dec	ecx
		jne	copy_loop
none:
		endm


	.code
		assume	fs:nothing

; void BR_ASM_CALL VESAWindowCopyToLines(br_uint_8 *mem, br_uint_32 mem_stride);
;
; Copy lines to a vesa window with no page breaks in scanlines
;
VESAWindowCopyToLines proc uses ebx esi edi es, mem:ptr byte, mem_stride:dword

		les	edi,fword ptr vw.window_start

	; Get source and sizes
	;
		mov	esi,mem			; 
		mov	ecx,mem_stride		; memory stride
		sar	ecx,2
		mov	mem_stride,ecx		; memory stride / 4

	; copy all of the full display pages
	;	
		mov	ecx,vw.full_banks	; number of full pages
		mov	ebx,0			; y
		mov	edx,0			; page
		
p_loop_0:	push	ecx
		push	ebx
		
	; get page from _screen_offset_table.page
	;
		shl	ebx,SCANLINE_SHIFT
		mov	eax,vw.scanline_table
		mov	edx,[ebx+eax].vesa_scanline._page

		SET_PAGE_FORCE
		
		pop	ebx
		
	; per display page
	;
		mov	ecx,vw.scanlines_per_page
		mov	edx,ebx
		shl	edx,SCANLINE_SHIFT
		add	edx,vw.scanline_table

	; per scanline
	;
p_loop_1:	push	ecx
		push	edi
		
		mov	ecx,mem_stride
		add	edi,[edx]
		rep	movsd
		
		add	edx,sizeof vesa_scanline
		inc	ebx
		
		pop	edi
		pop	ecx
		
		loop	p_loop_1
		
		pop	ecx
		loop	p_loop_0
		
		; remainder of scanlines in last page
		;
		mov	ecx,vw.scanlines_remainder
		cmp	ecx,0
		jz	p_fin
		
		push	ebx
		
		shl	ebx,4
		mov	eax,vw.scanline_table	
		mov	edx,4[ebx+eax]

		SET_PAGE_FORCE
		
		pop	ebx
		
		mov	ecx,vw.scanlines_remainder
		mov	edx,ebx
		shl	edx,4
		add	edx,vw.scanline_table
		
p_loop_2:	push	ecx
		push	edi
		
		mov	ecx,mem_stride
		add	edi,[edx]
		rep	movsd
		
		add	edx,sizeof vesa_scanline
		inc	ebx
	
		pop	edi
		pop	ecx
		loop	p_loop_2
		
p_fin:
		ret

VESAWindowCopyToLines endp

; void BR_ASM_CALL VESAWindowCopyToLinear(br_uint_8 *mem, br_uint_32 mem_stride);
;
; Copy linear memory to banked
;
VESAWindowCopyToLinear proc uses ebx esi edi es,
		mem:ptr byte, mem_stride:dword

		les	edi,fword ptr vw.window_start

	; Get source and sizes
	;
		mov	esi,mem		; source
		mov	ecx,mem_stride	; stride
		sar	ecx,2
		mov	mem_stride,ecx
		
		mov	ecx,vw.full_banks
		mov	ebx,0		; y
		mov	edx,0		; page
		
pb_l_0:		push	ecx
		push	ebx
	
		SET_PAGE_FORCE
		
		pop	ebx
	
	; per page
	;
		mov	ecx,vw.page_size
		sar	ecx,2
		
		push	edi
		
		rep	movsd
		
		pop	edi
		
		add	edx,vw.bank_increment
		inc	ebx
		
		pop	ecx
		loop	pb_l_0

	; do dribbly bits left over
	;
		mov	eax,vw.scanlines_remainder
		cmp	eax,0
		jz	pb_fin
	
		SET_PAGE_FORCE
		
		mov	eax,vw.scanlines_remainder
		mov	ecx,mem_stride
		mul	ecx
		
		mov	ecx,eax
		rep	movsd

pb_fin:		ret

VESAWindowCopyToLinear endp

;void BR_ASM_CALL VESAWindowRectangleCopyTo(
;	br_uint_8 *mem, br_uint_32 mem_stride,
;	br_uint_32 x, br_uint_32 y, br_uint_32 width,br_uint_32 height);
;
; rectangle copy to paged screen with no page breaks in scanlines
;
VESAWindowRectangleCopyTo proc uses ebx esi edi es fs,
		mem:ptr byte, mem_qual:dword, mem_stride:dword,
		px:dword, py:dword, pwidth:dword, pheight:dword

		les	edi,fword ptr vw.window_start

	; Get source and sizes
	;
		mov	eax,pwidth
		mul	vw.pixel_size
		mov	ebx,eax
		sar	eax,2
		mov	pwidth,eax
		mov	ecx,ebx
		shl	eax,2
		sub	ecx,eax
		mov	num_bytes,ecx
	
		lfs	esi,fword ptr mem		; source
		
		mov	ebx,py		; y
		
		mov	edx,px
		mov	eax,vw.pixel_size
		mul	edx
		mov	edx,eax		; x
		
		mov	ecx,pheight	; height
p_r_c_0:	push	ecx
		push	esi
		push	edi
		
	; per scanline
	;
	
		mov	eax,vw.scanline_table
		mov	ecx,ebx
		shl	ecx,SCANLINE_SHIFT
		add	ecx,eax
		
		add	edi,[ecx].vesa_scanline._offset	; addr
		add	edi,edx		; +x
		
		push	edx
		mov	edx,[ecx].vesa_scanline._page
		SET_PAGE
		pop	edx

	
	; pixel blt
	;
		mov	ecx,pwidth
		rep	movsd es:[edi],fs:[esi]
		
		mov	ecx,num_bytes
		rep	movsb es:[edi],fs:[esi]
		pop	edi
		pop	esi
		
		add	esi,mem_stride
		inc	ebx		; y++
		
		pop	ecx
		loop p_r_c_0
		
		ret
VESAWindowRectangleCopyTo endp

;void BR_ASM_CALL VESAWindowBreaksRectangleCopyTo(
;	br_uint_8 *mem, br_uint_16 mem_qual, br_uint_32 mem_stride,
;	br_uint_32 x, br_uint_32 y, br_uint_32 width,br_uint_32 height)
;
; rectangle copy to paged screen with page breaks in scanlines
;
VESAWindowBreaksRectangleCopyTo proc uses ebx esi edi es fs,
		mem:ptr byte, mem_qual:dword, mem_stride:dword,
		px:dword, py:dword, pwidth:dword, pheight:dword

		les	edi,fword ptr vw.window_start

	; Get source and sizes
	;
		mov	eax,pwidth
		mul	vw.pixel_size
		mov	byte_width,eax

		mov	ebx,eax
		sar	eax,2
		mov	pwidth,eax
		and	ebx,3
		mov	num_bytes,ebx
	

		lfs	esi,fword ptr mem		; source
		
		
		mov	eax,px
		mul	vw.pixel_size
		mov	px,eax		; x
		
		mov	ecx,pheight	; height

		mov	ebx,py		; y
		shl	ebx,SCANLINE_SHIFT
		add	ebx,vw.scanline_table
		

scanline_loop:	push	ecx
		push	ebx
		push	esi
		push	edi

	; per scanline
	;
		add	edi,[ebx].vesa_scanline._offset	; addr
		add	edi,px				; +x
		
		mov	edx,[ebx].vesa_scanline._page
		SET_PAGE

	; pixel blt
	;
		cmp	[ebx].vesa_scanline.split,0		; page break on scanline
		jne	has_break
		
	;	scanlines with no page breaks
	;
copy_and_loop:	
		mov	ecx,pwidth
		rep	movsd es:[edi],fs:[esi]
		
		mov	ecx,num_bytes
		rep	movsb es:[edi],fs:[esi]

		jmp	next_scanline
		
has_break:
	;	scanlines with page breaks
	;
		cmp	edi,vw.window_end

	;	< 0  either before break or crossing break
	;	> 0  after break
	
		ja	after_break
		
		mov	eax,edi
		add	eax,byte_width

		cmp	eax,vw.window_end
		
	;	> 0  spanning break
	;	< 0  all before break
		
		ja	spans_break
	    	jmp	copy_and_loop
		
after_break:	
	;	data after page break
	;
		INC_PAGE
		
		sub	edi,vw.page_size

		jmp	copy_and_loop
		
spans_break:
	;	data spanning break
	;
	
		mov	ecx,vw.window_end
		sub	ecx,edi

		mov	ebx,byte_width
		sub	ebx,ecx
		
		rep	movsb es:[edi],fs:[esi]
		
		INC_PAGE
		
		mov	ecx,ebx
		sub	edi,vw.page_size
		rep	movsb es:[edi],fs:[esi]
		
next_scanline:	pop	edi
		pop	esi
		
		add	esi,mem_stride

		pop	ebx
		add	ebx,sizeof vesa_scanline	; y++
		
		pop	ecx
		dec	ecx
		jne	scanline_loop

		ret
VESAWindowBreaksRectangleCopyTo endp

;void BR_ASM_CALL VESAWindowRectangleCopyFrom (
;	br_uint_8 *mem, br_uint_16 mem_qual, br_uint_32 mem_stride,
;	br_uint_32 x, br_uint_32 y, br_uint_32 width,br_uint_32 height)
;
; rectangle copy from paged screen with no page breaks in scanlines
;
	
VESAWindowRectangleCopyFrom proc uses ebx esi edi es fs,
		mem:ptr byte, mem_qual:dword, mem_stride:dword,
		px:dword, py:dword, pwidth:dword, pheight:dword

		les	edi,fword ptr vw.window_start

	; Get source and sizes
	;
		mov	eax,pwidth
		mul	vw.pixel_size
		mov	ebx,eax
		sar	eax,2
		mov	pwidth,eax
		mov	ecx,ebx
		shl	eax,2
		sub	ecx,eax
		mov	num_bytes,ecx
	
		lfs	esi,fword ptr mem ; source
		
		mov	ebx,py		; y
		
		mov	edx,px
		mov	eax,vw.pixel_size
		mul	edx
		mov	edx,eax		; x
		
		mov	ecx,pheight	; height

row_loop:	push	ecx
		push	esi
		push	edi
		
	; per scanline
	;
	
		mov	eax,vw.scanline_table
		mov	ecx,ebx
		shl	ecx,SCANLINE_SHIFT
		add	ecx,eax
		
		add	edi,[ecx].vesa_scanline._offset	; addr
		add	edi,edx				; +x
		
		push	edx
		mov	edx,[ecx].vesa_scanline._page
		SET_PAGE
		pop	edx

	
	; pixel blt
	;
		mov	ecx,pwidth
		REP_MOVSD_R
		
		mov	ecx,num_bytes
		REP_MOVSB_R
		pop	edi
		pop	esi
		
		add	esi,mem_stride
		inc	ebx				; y++
		
		pop	ecx
		loop 	row_loop
		
		ret
VESAWindowRectangleCopyFrom endp

;void BR_ASM_CALL VESAWindowBreaksRectangleCopyFrom(
;	br_uint_8 *mem, br_uint_16 mem_qual, br_uint_32 mem_stride,
;	br_uint_32 x, br_uint_32 y, br_uint_32 width,br_uint_32 height)
;
; rectangle copy from paged screen with page breaks in scanlines
;
VESAWindowBreaksRectangleCopyFrom proc uses ebx esi edi es fs,
		mem:ptr byte, mem_qual:dword, mem_stride:dword,
		px:dword, py:dword, pwidth:dword, pheight:dword

		les	edi,fword ptr vw.window_start

	; Get source and sizes
	;
		mov	eax,pwidth
		mul	vw.pixel_size
		mov	byte_width,eax

		mov	ebx,eax
		sar	eax,2
		mov	pwidth,eax
		and	ebx,3
		mov	num_bytes,ebx
	

		lfs	esi,fword ptr mem 		; source
		
		
		mov	eax,px
		mul	vw.pixel_size
		mov	px,eax		; x
		
		mov	ecx,pheight	; height

		mov	ebx,py		; y
		shl	ebx,SCANLINE_SHIFT
		add	ebx,vw.scanline_table
		

scanline_loop:	push	ecx
		push	ebx
		push	esi
		push	edi

	; per scanline
	;
		add	edi,[ebx].vesa_scanline._offset	; addr
		add	edi,px				; +x
		
		mov	edx,[ebx].vesa_scanline._page
		SET_PAGE

	; pixel blt
	;
		cmp	[ebx].vesa_scanline.split,0		; page break on scanline
		jne	has_break
		
	;	scanlines with no page breaks
	;
copy_and_loop:	
		mov	ecx,pwidth

		REP_MOVSD_R
		mov	ecx,num_bytes

		REP_MOVSB_R

		jmp	next_scanline
		
has_break:
	;	scanlines with page breaks
	;
		cmp	edi,vw.window_end

	;	< 0  either before break or crossing break
	;	> 0  after break
	
		ja	after_break
		
		mov	eax,edi
		add	eax,byte_width

		cmp	eax,vw.window_end
		
	;	> 0  spanning break
	;	< 0  all before break
		
		ja	spans_break
	    	jmp	copy_and_loop
		
after_break:	
	;	data after page break
	;
		INC_PAGE
		
		sub	edi,vw.page_size

		jmp	copy_and_loop
		
spans_break:
	;	data spanning break
	;
	
		mov	ecx,vw.window_end
		sub	ecx,edi

		mov	ebx,byte_width
		sub	ebx,ecx
		
		REP_MOVSB_R

		INC_PAGE
		
		mov	ecx,ebx
		sub	edi,vw.page_size

		REP_MOVSB_R
		
next_scanline:	pop	edi
		pop	esi
		
		add	esi,mem_stride

		pop	ebx
		add	ebx,sizeof vesa_scanline		; y++
		
		pop	ecx
		dec	ecx
		jne	scanline_loop

		ret
VESAWindowBreaksRectangleCopyFrom endp

;void BR_ASM_CALL VESAWindowRectangleFill(
;	br_uint_32 x, br_uint_32 y, br_uint_32 width,br_uint_32 height,
;	br_uint_32 colour)
;
; rectangle clear to paged screen with no page breaks in scanlines
;
VESAWindowRectangleFill proc uses ebx esi edi es,
		px:dword, py:dword, pwidth:dword, pheight:dword,
		colour:dword

		les	edi,fword ptr vw.window_start

	; Get source and sizes
	;
		mov	ebx,py		; y
		
		mov	edx,px
		mov	eax,vw.pixel_size
		mul	edx
		mov	edx,eax		; x
		
		mov	ecx,pheight	; height
row_loop:	push	ecx
		push	edi
		
	; per scanline
	;
	
		mov	eax,vw.scanline_table
		mov	ecx,ebx
		shl	ecx,SCANLINE_SHIFT
		add	ecx,eax
		
		add	edi,[ecx].vesa_scanline._offset	; addr
		add	edi,edx				; +x
		
		push	edx
		mov	edx,[ecx].vesa_scanline._page
		SET_PAGE
		pop	edx

	; pixel blt
	;
		mov	edx,pwidth
			
pixel_loop:
		mov	ecx,vw.pixel_size
		lea	esi,colour
		rep	movsb
		
		dec	edx
		jne	pixel_loop

		pop	edx
		pop	edi

		inc	ebx		; y++

		pop	ecx
		dec	ecx
		jne	row_loop
	
		ret
VESAWindowRectangleFill endp

;void BR_ASM_CALL VESAWindowBreaksRectangleFill (
;	br_uint_32 x, br_uint_32 y, br_uint_32 width,br_uint_32 height,
;	br_uint_32 colour)
;
; rectangle clear to paged screen with page breaks in scanlines
;
VESAWindowBreaksRectangleFill proc uses ebx esi edi es,
		px:dword, py:dword, pwidth:dword, pheight:dword,
		colour:dword

		les	edi,fword ptr vw.window_start

	; Get source and sizes
	;
		mov	ebx,py	; y
		
		mov	edx,px
		mov	eax,vw.pixel_size
		mul	edx
		mov	edx,eax		; x
		
		mov	ecx,pheight	; height
p_r_clb_0:	push	ecx
		push	edi
		
	; per scanline
	;
	
		mov	eax,vw.scanline_table
		mov	ecx,ebx
		shl	ecx,SCANLINE_SHIFT
		add	ecx,eax
		
		add	edi,[ecx].vesa_scanline._offset	; addr
		add	edi,edx		; +x
		
		push	edx
		
		mov	edx,[ecx].vesa_scanline._page
		SET_PAGE
		pop	edx

	; pixel blt
	;
		cmp	[ecx].vesa_scanline.split,0	; page break on scanline
		jne	p_r_clb_2
		
	;	scanlines with no page breaks
	;
	
		mov	ecx,pwidth
p_r_clb_4:	push	ecx
		lea	esi,colour
		mov	ecx,vw.pixel_size
		
		rep	movsb

		pop	ecx
		loop	p_r_clb_4
	
		jmp	p_r_clb_3
		
p_r_clb_2:
	;	scanlines with page breaks
	;
		mov	ecx,vw.window_end
		sub	ecx,edi
		
	;	> 0  either before break or crossing break
	;	< 0  after break
	
		jle	p_r_clb_5
		
		;	ecx = bytes left in page
		
		push	edx
		
		mov	eax,pwidth
		mov	edx,vw.pixel_size
		mul	edx		; width*pixelsize
		pop	edx 		
		
		sub	ecx,eax		; bytes_left - width*pixelsize
		
	;	> 0  all before break
	;	< 0  spanning break
		
		jle	p_r_clb_6
		
		mov	ecx,pwidth
p_r_clb_8:	push	ecx
		mov	eax,ecx
		lea	esi,colour
		mov	ecx,vw.pixel_size
		
		rep	movsb
		
		pop	ecx
		loop	p_r_clb_8
		
		jmp	p_r_clb_3
		
p_r_clb_5:	
	;	data after page break
	;
		INC_PAGE
		sub	edi,vw.page_size

		mov	ecx,pwidth
p_r_clb_7:	push	ecx
		lea	esi,colour
		mov	ecx,vw.pixel_size
		
		rep	movsb
		
		pop	ecx
		loop	p_r_clb_7
		jmp	p_r_clb_3
		
p_r_clb_6:
	;	data spanning break
	;
		mov	ecx,vw.window_end
		sub	ecx,edi
		mov	eax,pwidth

p_r_clb_9:	lea	esi,colour
		mov	ecx,vw.pixel_size
		
		rep	movsb
		dec	eax		
		cmp	edi,vw.window_end
		jne	p_r_clb_9
			
	;	flip pages
	;
		push	eax
		INC_PAGE
		pop	eax
	
	;	copy rest
	;
		and	eax,eax
		jz p_r_clb_3

		mov	ecx,eax
		sub	edi,vw.page_size

p_r_clb_a:	push	ecx
		lea	esi,colour
		mov	ecx,vw.pixel_size
		
		rep	movsb
		
		pop	ecx
		loop	p_r_clb_a
		
p_r_clb_3:	pop	edi
		
		inc	ebx		; y++
		
		pop	ecx
		dec	ecx
		jz	p_r_clb_out
		jmp	p_r_clb_0
		
p_r_clb_out:
		ret
VESAWindowBreaksRectangleFill endp

; void BR_ASM_CALL VESAWindowPixelSet(br_uint_32 px, br_uint_32 py, br_uint_32 colour)
;	
; set pixel on paged screen no page breaks in scanlines
;
VESAWindowPixelSet proc uses ebx esi edi es,
		px:dword, py:dword,
		colour:dword

		les	edi,fword ptr vw.window_start
		
	; Get source and sizes
	;
		
		mov	edx,py		; y
		mov	ebx,vw.scanline_table
		shl	edx,SCANLINE_SHIFT
		add	ebx,edx
		add	edi,[ebx].vesa_scanline._offset
		
		mov	eax,px		; x
		mul	vw.pixel_size
		
		add	edi,eax
		
		mov	edx,[ebx].vesa_scanline._page
		SET_PAGE

		mov	ecx,vw.pixel_size
		
		lea	esi,colour

		rep	movsb
		
		ret
VESAWindowPixelSet endp

; void BR_ASM_CALL VESAWindowBreaksPixelSet(br_uint_32 px, br_uint_32 py, br_uint_32 colour)
;	
; set pixel on paged screen page breaks in scanlines
;
VESAWindowBreaksPixelSet proc uses ebx esi edi es,
		px:dword, py:dword,
		colour:dword

		les	edi,fword ptr vw.window_start
		
	; Get source and sizes
	;
		mov	edx,py		; y
		mov	ebx,vw.scanline_table
		shl	edx,SCANLINE_SHIFT
		add	ebx,edx

		add	edi,[ebx].vesa_scanline._offset
		
		mov	eax,px		; x
		mul	vw.pixel_size
		
		add	edi,eax
		
		mov	edx,[ebx].vesa_scanline._page
		cmp	[ebx].vesa_scanline.split,0
		je	p_spb_2		; no page break in scanline
		
		cmp	edi,vw.window_end
		jb	p_spb_2		; same page
	
		add	edx,vw.bank_increment ; next page
		sub	edi,vw.page_size
		
p_spb_2:
		SET_PAGE
		
		mov	ecx,vw.pixel_size
		lea	esi,colour
		rep	movsb
		
		ret
VESAWindowBreaksPixelSet endp


; br_uint_32 BR_ASM_CALL VESAWindowPixelGet(br_uint_32 px, br_uint_32 py)
;	
; get pixel on paged screen no page breaks in scanlines
;
VESAWindowPixelGet proc uses ebx esi edi es,
		px:dword, py:dword,

		les	edi,fword ptr vw.window_start

		xor	eax,eax
		mov	temp_colour,eax
		
	; Get source and sizes
	;
		
		mov	edx,py		; y
		mov	ebx,vw.scanline_table
		shl	edx,SCANLINE_SHIFT
		add	ebx,edx
		add	edi,[ebx].vesa_scanline._offset
		
		mov	eax,px		; x
		mul	vw.pixel_size
		
		add	edi,eax
		
		mov	edx,[ebx].vesa_scanline._page
		SET_PAGE

		mov	ecx,vw.pixel_size
		
		lea	esi,temp_colour

ploop:		mov	al,es:[edi]
		mov	[esi],al
		dec	ecx
		jne	ploop
		
		mov	eax,temp_colour		
		ret
VESAWindowPixelGet endp

; br_uint_32 BR_ASM_CALL VESAWindowBreaksPixelGet( br_uint_32 px, br_uint_32 py)
;	
; get pixel on paged screen page breaks in scanlines
;
VESAWindowBreaksPixelGet proc uses ebx esi edi es,
		px:dword, py:dword

		les	edi,fword ptr vw.window_start
		
		xor	eax,eax
		mov	temp_colour,eax

	; Get source and sizes
	;
		mov	edx,py		; y
		mov	ebx,vw.scanline_table
		shl	edx,SCANLINE_SHIFT
		add	ebx,edx

		add	edi,[ebx].vesa_scanline._offset
		
		mov	eax,px		; x
		mul	vw.pixel_size
		
		add	edi,eax
		
		mov	edx,[ebx].vesa_scanline._page
		cmp	[ebx].vesa_scanline.split,0
		je	p_spb_2		; no page break in scanline
		
		cmp	edi,vw.window_end
		jb	p_spb_2		; same page
	
		add	edx,vw.bank_increment ; next page
		sub	edi,vw.page_size
p_spb_2:
		SET_PAGE
		
		mov	ecx,vw.pixel_size
		lea	esi,temp_colour

ploop:		mov	al,es:[edi]
		mov	[esi],al
		dec	ecx
		jne	ploop

		mov	eax,temp_colour		
		ret
VESAWindowBreaksPixelGet endp

; void BR_ASM_CALL VESAWindowCopyBits(
;		br_int_32 x,
;		br_int_32 y,
;		br_uint_8 *src,
;		br_uint_32 s_stride,
;		br_uint_32 start_bit,
;		br_uint_32 end_bit,
;               br_uint_32 n_rows,
;		br_uint_32 colour);
;

;	copy bits for paged VESA display with no page breaks in scanlines
;
	
VESAWindowCopyBits proc uses ebx esi edi es,
		px:dword, py:dword,
		src:ptr byte, src_stride:dword,
		start_bit:dword, end_bit:dword,
		n_rows:dword, colour:dword
	
		les	edi,fword ptr vw.window_start

		mov	esi,src

	; Advance to start byte
	;
		mov	eax,px
		mul	vw.pixel_size
		mov	px,eax		; x
	
		mov	ebx,py		; y
		shl	ebx,SCANLINE_SHIFT
		add	ebx,vw.scanline_table
	
		mov	eax,start_bit	; start_bit
		mov	ecx,end_bit	; end_bit
		mov	edx,eax

		shr	eax,3		; n_bytes = n_bits/8
		add	esi,eax
		shl	eax,3
		sub	ecx,eax

		push	edx
		mul	vw.pixel_size
		add	edi,eax
		pop	edx

	; Work out count, and masks at start and end of line
	;
		mov	eax,ecx
		and	edx,7
		and	eax,7
		mov	dl,bit_to_mask_s[edx]
		mov	dh,bit_to_mask_e[eax]
		shr	ecx,3		; n_bytes = n_bits/8

		test	ecx,ecx
		jne	multi_byte

	; Only one byte wide
	;
		and	dl,dh		; combine masks
		mov	ecx,n_rows	; n_rows

v1_loop:	push	edi
		add	edi,[ebx].vesa_scanline._offset
		add	edi,px
		
	; Get first source byte and mask it
	;
		mov	al,[esi]
		and	al,dl

	; Set destination pixels according to byte mask
	;
		push	ecx
		push	esi
		push	ebx
		
		push	eax
		push	edx
		mov	edx,[ebx].vesa_scanline._page
		SET_PAGE
		pop	edx
		pop	eax

		mov	ebx,0
		
	rept 8
	local no_pixel
		add	al,al
		jnc	no_pixel
		
		mov	ecx,vw.pixel_size
		lea	esi,colour
	
		push	edi
			
		add	edi,ebx
		
		rep	movsb
		
		pop	edi
no_pixel:
		add	ebx,vw.pixel_size
	endm
		pop	ebx
		pop	esi
		pop	ecx
		
		pop	edi

		add	esi,src_stride
		
		add	ebx,sizeof vesa_scanline

		dec	ecx
		jne	v1_loop

		ret

	; 2 or more source bytes wide
	;
		
multi_byte:

v_loop:		push	ebx
		push	ecx
		push	esi
		push	edi
		push	ecx

		add	edi,[ebx].vesa_scanline._offset
		add	edi,px

		push	edx		
		mov	edx,[ebx].vesa_scanline._page
		SET_PAGE
		pop	edx
		
		pop	ecx

	; Get first source byte and mask it
	;
		mov	al,[esi]
		and	al,dl
h_loop:		inc	esi
h_loop_last:
	; Set destination pixels according to byte mask
	;
		push	ecx
		push	esi
		
		mov	ebx,0
		
	rept 8
	local no_pixel
		add	al,al
		jnc	no_pixel
		
		push	edi
		lea	esi,colour
		add	edi,ebx
		mov	ecx,vw.pixel_size
		
		rep	movsb
		
		pop	edi
no_pixel:
		add	ebx,vw.pixel_size
	endm
		pop	esi
		pop	ecx

	; Get next source byte
	;
		mov	eax,vw.pixel_size
		shl	eax,3
		add	edi,eax
		
		mov	al,[esi]
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
		pop	ebx
		
		add	ebx,sizeof vesa_scanline
		
		add	esi,src_stride

		dec	n_rows
		jne	v_loop

		ret

VESAWindowCopyBits endp

; void BR_ASM_CALL VESAWindowBreaksCopyBits(
;		br_int_32 x,
;		br_int_32 y,
;		br_uint_8 *src,
;		br_uint_32 s_stride,
;		br_uint_32 start_bit,
;		br_uint_32 end_bit,
;               br_uint_32 n_rows,
;		br_uint_32 colour);
;
;	copy bits for paged VESA display with page breaks in scanlines
;

VESAWindowBreaksCopyBits proc uses ebx esi edi es,
		px:dword, py:dword,
		src:ptr byte, src_stride:dword,
		start_bit:dword, end_bit:dword,
		n_rows:dword, colour:dword
	
		les	edi,fword ptr vw.window_start
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

	; Set up pointers
	;
		push	edx
		mov	eax,px
		mul	vw.pixel_size
		mov	px,eax		; x
		pop	edx

		mov	ebx,py		; y
		shl	ebx,SCANLINE_SHIFT
		add	ebx,vw.scanline_table
	
		test	ecx,ecx
		jne	multi_byte

	; Only one byte wide
	;
		and	dl,dh		; combine masks
		mov	ecx,n_rows	; n_rows

v1_loop:	push	edi
		add	edi,[ebx].vesa_scanline._offset
		add	edi,px
		

		push	edx
		mov	edx,[ebx].vesa_scanline._page
		mov	scanline_page,edx
		SET_PAGE
		pop	edx

	; Get first source byte and mask it
	;
		mov	al,[esi]
		and	al,dl

	; Set destination pixels according to byte mask
	;
		push	ecx
		push	esi
		push	ebx
		
		mov	ecx,[ebx].vesa_scanline.split
		mov	break_in_scanline,ecx
		
		mov	ebx,0
		
	rept 8
	local no_pixel,no_page_1a
		add	al,al
		jnc	no_pixel
		
		mov	ecx,vw.pixel_size
		lea	esi,colour
	
		push	edi
			
		add	edi,ebx
		
		push	ecx
		
		mov	ecx,break_in_scanline
		jz	no_page_1a		; no break in scanline
		
		cmp	edi,vw.window_end
		jb	no_page_1a		; before break
		
		push	eax
		push	edx
		push	ebx

		mov	edx,vw.bank_increment
		
		add	edx,scanline_page	; next page to current scanline

		SET_PAGE_FORCE
		
		sub	edi,vw.page_size
		
		pop	ebx
		pop	edx
		pop	eax

no_page_1a:	pop	ecx
				
		rep	movsb
		
		pop	edi
no_pixel:
		add	ebx,vw.pixel_size
	endm
		pop	ebx
		pop	esi
		pop	ecx
		
		pop	edi

		add	esi,src_stride
		
		add	ebx,sizeof vesa_scanline

		dec	ecx
		jne	v1_loop
		
		ret

	; 2 or more source bytes wide
	;
		
multi_byte:

v_loop:		push	ebx
		push	ecx
		push	esi
		push	edi
		push	ecx
		
		add	edi,[ebx].vesa_scanline._offset
		add	edi,px
		
		mov	ecx,[ebx].vesa_scanline.split
		mov	break_in_scanline,ecx
		
		push	edx
		mov	edx,[ebx].vesa_scanline._page
		mov	scanline_page,edx
		SET_PAGE
		pop	edx

no_page_2:	pop	ecx

	; Get first source byte and mask it
	;
		mov	al,[esi]
		and	al,dl
h_loop:		inc	esi
h_loop_last:
	; Set destination pixels according to byte mask
	;
		push	ecx
		push	esi
		
		mov	ebx,0
		
	rept 8
	local no_pixel,no_page_2a
		add	al,al
		jnc	no_pixel
		
		push	edi
		lea	esi,colour
		add	edi,ebx
		mov	ecx,vw.pixel_size
		
		push	ecx
		
		cmp	break_in_scanline,0
		je	no_page_2a		; no page break in scanline
		
		cmp	edi,vw.window_end
		jb	no_page_2a		; before break in scanline
		
		push	eax
		push	ebx
		push	edx
		
		mov	edx,vw.bank_increment

		add	edx,scanline_page	; page after current scanline

		SET_PAGE_FORCE
		
		pop	edx
		pop	ebx
		pop	eax
		
		sub	edi,vw.page_size

no_page_2a:	pop	ecx
		
		rep	movsb
		
		pop	edi
no_pixel:
		add	ebx,vw.pixel_size
	endm
		pop	esi
		pop	ecx

	; Get next source byte
	;
		mov	eax,vw.pixel_size
		shl	eax,3
		add	edi,eax
		
		mov	al,[esi]
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
		pop	ebx
		
		add	ebx,sizeof vesa_scanline
		
		add	esi,src_stride

		dec	n_rows
		jne	v_loop

		ret

VESAWindowBreaksCopyBits endp

		end

