;; tzrgbsm.inc
;;
;; Triangle rasterise loop Z, RGB, Small, MMX
;;
;; XXX Could try to have two seperate loops, one for long scan lines, and one for 1 word lines.
;; and ping-pong between them. The 1 word loop does not need to keep memory copies of RGB, Z and scanline pointers
;;

; Select a bunch of instructions based on the direction that we are going
;
ifidni direction,<lr>

D_PADDW		macro	_arg0,_arg1
			paddw	_arg0,_arg1
			endm

D_PADDD		macro	_arg0,_arg1
			paddd	_arg0,_arg1
			endm

WORD_STEP	=		8

J_EMPTY		macro	lab
			jg		lab
			endm

J_EMPTY_2	macro	lab
			jns		lab
			endm

START_MASKS	equ		left_masks
END_MASKS	equ		right_masks

endif

ifidni direction,<rl>

D_PADDW		macro	_arg0,_arg1
			psubw	_arg0,_arg1
			endm

D_PADDD		macro	_arg0,_arg1
			psubd	_arg0,_arg1
			endm

J_EMPTY		macro	lab
			jl		lab
			endm

J_EMPTY_2	macro	lab
			js		lab
			endm

START_MASKS	equ		right_masks
END_MASKS	equ		left_masks

WORD_STEP	=		-8
endif

    	sub		esp,PARAM_OFFSET

	;; Unpack Z parameter
	;;
		UNPACK_PARAM_16  PARAM.s_z,PARAM.d_z_x, WORK.z0,  WORK.d_z_y1,  WORK.d_z_y0,  WORK.d_z_x

	;; Setup for first iteration of loop and generate pointers to starting scanline
	;;
		movd		mm6,WORK.h.start_scanline
;V
		movq		mm5,WORK.h.screen_stride
		punpckldq	mm6,mm6

		mov			eax,WORK.h.xm
		pmaddwd		mm6,mm5

		sar			eax,16						; Get integer part of X start
		mov			ebx,WORK.h.x1

		sar			ebx,16						; Get integer part of X end
		mov			ebp,offset scanline_mask

		paddd		mm6,WORK.h.screen_address

		xor			edi,edi
		mov			ecx,WORK.h.counts

		movq		WORK.h.screen_address,mm6
		pxor		mm5,mm5

        mov			esi,WORK.h.depth_address

		test		ecx,ecx
		jns			p1_start

	; Top trapezoid is empty
	;
		rol			ecx,16
; V

		test		ecx,ecx
		js			skip_triangle
		mov			WORK.h.counts,ecx
		
		mov			ecx,WORK.h.d_x2
		mov			ebx,WORK.h.x2

		mov			WORK.h.d_x1,ecx
		mov			WORK.h.x1,ebx

		sar			ebx,16						; Get integer part of X end
		jmp			p1_start

	;; Swith to lower trapezoid
	;;
p1_sl_bottom:
		
		mov		WORK.h.counts,ecx
		nop

	; Version of X increments that loads lower X info
	;
	; Increment X's
	;
		mov		ebx,WORK.h.d_x2
		mov		eax,WORK.h.xm

		mov		WORK.h.d_x1,ebx
		mov		ecx,WORK.h.d_xm

		mov		edi,eax
		add		eax,ecx

		mov		WORK.h.xm,eax
		xor		edi,ecx

		xor		edi,eax
		mov		ebx,WORK.h.x2

		shl		edi,14
		jmp		p1_sl_bottom_cont

	; Loop jumps here for empty line
	;
p1_next_line:

		mov		ecx,WORK.h.counts
		mov		[ebp+4],edi

ifidni direction,<lr>
		mov		eax,1
else
		mov		eax,-1
endif
		nop

		mov		[ebp],eax
		add		ebp,8

		sub		ecx,10000h			; Decrement count (in hi word of dword)
		js		next_count

	;; Per scanline loop
	;;

p1_sl_loop:

	; Increment X's and generate flag for carry from bit 17->18 (crossing quadword)
	;
		mov		WORK.h.counts,ecx
		mov		eax,WORK.h.xm

		mov		ecx,WORK.h.d_xm
		mov		ebx,WORK.h.x1

		mov		edi,eax
		add		eax,ecx

		xor		edi,ecx
		mov		ecx,WORK.h.d_x1

		xor		edi,eax
		mov		WORK.h.xm,eax

		shl		edi,14
		lea		ebx,[ebx+ecx]

p1_sl_bottom_cont:
		sbb		edi,edi
		mov		esi,WORK.h.depth_address

		mov		WORK.h.x1,ebx
		mov		ecx,WORK.h.depth_stride

		movq	mm2,WORK.d_z_y0[edi*8]
;V
		movq	mm0,WORK.z0
;V
		movq	mm1,WORK.z2
		paddd	mm0,mm2

		sar		eax,16				; Get integer part of X start
		add		esi,ecx

		movq	WORK.z0,mm0
		paddd	mm1,mm2

		sar		ebx,16				; Get integer part of X end
		mov		WORK.h.depth_address,esi

		movq	WORK.z2,mm1
;V

	; eax:	start X (pixel)
	; ebx:	end Y	(pixel)
	;
	; edi:  -1 or 0 (if carry)
	;
	; esi: Pointer to start of Z buffer scanline
	;
	; mm0:	Z0 Z1
	; mm1	Z2 Z3	
	;

p1_start:
	; Generate masks and pointers
	;
		cmp		eax,ebx
		J_EMPTY	p1_next_line		; No pixels on line at all

		mov		ecx,eax				; Make copies
		mov		edx,ebx

		and		eax,not 3			; Find quad word boundaries for start and end
		and		ebx,not 3

		and		ecx,3				; Find pixel within byte for start and end
		and		edx,3

		sub		eax,ebx
		je		p1_one_word			; Scanline fits in one word

	; Start setting up scanline loop
	;
		shl		edi,31				; Get carry bit into top of edi
		add		ebp,8

		lea		esi,[esi+ebx*2]		; Point esi at end of scanline
		add		ebx,edi				; Add carry bit to top of ebx

		mov		[ebp-8],eax			; Write out scan convertion info
		mov		[ebp+4-8],ebx

	; Z read test, and write - 2.6 cycles per pixel
	;

	; Loop head
	;
	;
		movq		mm2,mm0			; Make a copy of Z01
		movq		mm3,mm1			; Make a copy of Z23

		D_PADDD		mm0,WORK.d_z_x	; Add delta to Z01
		psrad		mm2,16			; Shift to get integer parts

		D_PADDD		mm1,WORK.d_z_x	; Add delta to Z23
		psrad		mm3,16

		movq		mm7,[esi+eax*2]	; read current z buffer pixels
		packssdw	mm2,mm3			; merge Zs down to 16 bits per pixel

		add			eax,eax			; pixels to bytes
		movq		mm6,mm2			; make copy of new Z's

ifidni direction,<lr>
		sub			ebp,eax			; Advance mask pointer to end of buffer
else
		mov			ebx,eax			; Remember length of line
endif
		psubusw		mm6,mm7			; Compare old and new Z's (Unsigned)

		pcmpeqw		mm6,zeros		; Make a mask from the results of psub
;V

		pand		mm6,START_MASKS[ecx*8]	; Mask for front of scanline
;V

		movq		mm4,mm6			; Make a copy of mask
		pand		mm2,mm6			; Select new Z's using mask

		pandn		mm6,mm7			; Select old Z's using mask
		add			eax,WORD_STEP	; Loop control

		por			mm6,mm2			; Combine old and new Z's
		Je			p1_loop_tail

p1_loop_body:
	; Loop body
	;
	; At this point:
	;
	; mm0 current Z01
	; mm1 current Z23
	; mm4 previous mask
	; mm5 previous accumulated mask
	; mm6 previous merged pixels
	;
	; edi end of line
	; eax current count
	;
		movq		mm2,mm0			; Make copies of the current Z01 values
		movq		mm3,mm1			; Make copies of the current Z23 values

		D_PADDD		mm0,WORK.d_z_x	; Add delta to Z01
		psrad		mm2,16			; Shift to get integer parts of Z01

		D_PADDD		mm1,WORK.d_z_x	; Add delta to Z23
		psrad		mm3,16			; Shift to get integer parts of Z23

		movq		mm7,[esi+eax]	; read current z buffer pixels
		packssdw	mm2,mm3			; merge Zs down to 16 bits per pixel

		movq		[esi+eax-WORD_STEP],mm6	; Write to prev. Z buffer pixels
		movq		mm6,mm2			; make copy of new Z's

	; 10 cycle stall here if destination is not in cache
	;		
		movq		[ebp+eax-WORD_STEP],mm4	; Write out previous mask
		psubusw		mm6,mm7			; Compare old and new Z's (Unsigned)

		pcmpeqw		mm6,zeros		; Make a mask from the results of PSUB
		por			mm5,mm4			; Accumulate previous mask

		movq		mm4,mm6			; Make a copy of mask
		pand		mm2,mm6			; Select new Z's using mask

		pandn		mm6,mm7			; Select old Z's using mask
		add			eax,WORD_STEP

		por			mm6,mm2			; Combine old and new Z's
		jne			p1_loop_body

	; Loop tail
	;
p1_loop_tail:
		movq		mm7,[esi]		; read current z buffer pixels
		psrad		mm0,16			; Shift to get integer parts of Z01

		movq		[esi-WORD_STEP],mm6	; Write to prev. Z buffer pixels
		psrad		mm1,16			; Shift to get integer parts of Z23

		movq		[ebp-WORD_STEP],mm4	; Write out previous mask
		packssdw	mm0,mm1			; merge Zs down to 16 bits per pixel

		movq		mm6,mm0			; make copy of new Z's
		psubusw		mm0,mm7			; Compare old and new Z's (Unsigned)

		pcmpeqw		mm0,zeros		; Make a mask from the results of PSUB
		por			mm5,mm4			; Accumulate previous mask

		pand		mm0,END_MASKS[edx*8]	; Mask for end of scanline
;V

		mov			ecx,WORK.h.counts
		movq		mm4,mm0			; Make a copy of mask

		pand		mm6,mm0			; Select new Z's using mask
		pandn		mm0,mm7			; Select old Z's using mask

		movq		[ebp],mm4		; Write out  mask
		por			mm6,mm0			; Combine old and new Z's

ifidni direction,<lr>
		add			ebp,8
else
		lea			ebp,[ebp+ebx+8]
endif
		por			mm5,mm4			; Accumulate  mask

		movq		[esi],mm6		; Write to Z buffer pixels
;V

	; Loop
	;
		sub		ecx,10000h			; Decrement count (in hi word of dword)
		jns		p1_sl_loop

next_count:
		rol		ecx,16				; Move lower count to top half of dword
;V

		test	ecx,ecx				; See if there is another count in bottom of dword
		jns		p1_sl_bottom

;U
		jmp			pass2

p1_one_word:
	; Write scan convertion info into mask buffer
	;
		shl			edi,31			; Get carry bit into top of ebx
		movq		mm2,mm0			; Make a copy of Z01

		movq		mm7,[esi+ebx*2]	; read current z buffer pixels
		psrad		mm2,16			; Shift to get integer parts

		movq		mm3,mm1			; Make a copy of Z23
		add			ebx,edi

		movq		mm4,START_MASKS[ecx*8]	; Mask for front of scanline
		psrad		mm3,16

		pand		mm4,END_MASKS[edx*8]	; Mask for back of scanline
		packssdw	mm2,mm3			; merge Zs down to 16 bits per pixel

		movq		mm6,mm2			; make copy of new Z's
		psubusw		mm2,mm7			; Compare old and new Z's (Unsigned)

		pcmpeqw		mm2,zeros		; Make a mask from the results of PSUB
; V
		
		pand		mm2,mm4			; Mask for length scanline
		mov			ecx,WORK.h.counts

		pand		mm6,mm2			; Select new Z's using mask
		por			mm5,mm2			; Accumulate mask

		movq		[ebp+8],mm2		; Write out mask
		pandn		mm2,mm7			; Select old Z's using mask

		por			mm2,mm6			; Combine old and new Z's
		mov			[ebp],eax		; Write out scan convertion info

		mov			[ebp+4],ebx
		add			ebp,16			; Advance mask pointer

		movq		[esi+ebx*2],mm2	; Write to  Z buffer pixels
; V

	; Loop
	;
		sub		ecx,10000h			; Decrement count (in hi word of dword)
		jns		p1_sl_loop

 		rol		ecx,16				; Move lower count to top half of dword
; v

		test	ecx,ecx				; See if there is another count in bottom of dword
		jns		p1_sl_bottom

	; Early out if no pixels of triangle are visible
	;
pass2:
		packsswb	mm5,mm5

		movd		eax,mm5

		test		eax,eax
		je			skip_triangle

	; Unpack RGB parameters
	; 
		UNPACK_PARAM_RGB

		mov			ecx,ebp				; ECX = pointer to end of mask buffer
		mov			ebp,offset scanline_mask ; point at start of buffer

		pand		mm3,mask_5			; Mask red copies
		pand		mm4,mm7				; mask green copies

		psrlw		mm4,5				; shift greens into position
		mov			eax,[ebp]			; Line length

		psrlw		mm5,11				; shift blues into position
		mov			ebx,[ebp+4]			; Line end and carry

		mov			edi,WORK.h.screen_address
		jmp			p2_start

	; Loop jumps here for empty scanlines
	;
p2_next_line:

if DITHER
		mov			edx,WORK.h.start_scanline
		inc			edx
		mov			WORK.h.start_scanline,edx
endif

		mov		edx,[ebp+4+8]			; Next line end and carry
		add		ebp,8

		cmp		ebp,ecx
		jae		skip_triangle

		sar		edx,31
		nop	; V

p2_sl_loop:
	; Read scan convertion info from masks buffer
	; and increment screen address
	;
		mov		edi,WORK.h.screen_address
		mov		eax,[ebp]					; Line length	(Occasional bank conflict here)

p2_sl_loop_1:								; Loop from one word case

	; Increment parameters and make copies of red, greens and blues
	;
	; Generate a temp. copy of mask_5 in mm6
	;
	; Mask the RGB copies for first iteration of loop
	;
		movq	mm0,WORK.r01
		pcmpeqw	mm6,mm6						; mm6 = 11111....111111

		paddw	mm0,WORK.d_r_y0[edx*8]
		psllw	mm6,11						; mm6 = mask_5

		movq	mm1,WORK.g01
		movq	mm3,mm0

		paddw	mm1,WORK.d_g_y0[edx*8]
		pand	mm3,mm6						; mask reds

		movq	mm2,WORK.b01
		movq	mm4,mm1

		paddw	mm2,WORK.d_b_y0[edx*8]
		pand	mm4,mm7						; Mask greens

		movq	WORK.r01,mm0
		movq	mm5,mm2

		movq	WORK.g01,mm1
		psrlw	mm4,5						; shift greens into position

		movq	WORK.b01,mm2
		psrlw	mm5,11						; shift blues ino position


if DITHER
		mov			ebx,WORK.h.start_scanline
		and			ebx,3

		paddw		mm0,dither_table_5[ebx*8]
		paddw		mm1,dither_table_6[ebx*8]
		paddw		mm2,dither_table_5[ebx*8]

		inc			ebx
		mov			WORK.h.start_scanline,ebx
endif

		add		edi,WORK.h.screen_stride
		mov		ebx,[ebp+4]					; Line end


p2_start:
		test	eax,eax
		je		p2_one_word					; Scanline fits in one word

		mov		WORK.h.screen_address,edi	; Write screen address
		J_EMPTY_2 p2_next_line

		lea		edi,[edi+ebx*2]				; Carry bit is lost when ebx is scaled
		add		ebp,8						; Move on to mask data

	; Colour rendering for scanline
	;
		add		eax,eax

ifidni direction,<lr>
		sub		ebp,eax				; Point at end of scanline
else
		mov		ebx,eax				; remember line length
endif

; loop head
;

		D_PADDW	mm0,WORK.d_r_x		; Red Increments
; V
		D_PADDW	mm1,WORK.d_g_x		; Green Increments
		por		mm3,mm4				; merge greens into reds

		movq	mm6,[ebp+eax]		; Read current mask
		por		mm3,mm5				; merge blues into reds and greens

		movq	mm4,[edi+eax]		; Read current pixels
		pand	mm3,mm6				; Mask out new pixels

		pandn	mm6,mm4				; Mask out old pixels
		add		eax,WORD_STEP		; Loop control

		por		mm6,mm3				; Merge new and old pixels
		Je		p2_loop_tail

p2_loop_body:
	; At this point:
	;
	; mm0	(R,R,R,R)
	; mm1	(G,G,G,G)
	; mm2	(B,B,B,B) (previous)
	;
	; mm6	previous pixels
	;
	; mm7	mask for high 6 bits of each word
	;
		D_PADDW	mm2,WORK.d_b_x		; Blue increments (rolled)
		movq	mm3,mm0				; make a copy of reds

		movq	[edi+eax-WORD_STEP],mm6		; Write previous pixels
		movq	mm4,mm1				; make a copy of greens

		D_PADDW	mm0,WORK.d_r_x		; Red Increments
		movq	mm5,mm2				; make a copy of blues

		D_PADDW	mm1,WORK.d_g_x		; Green Increments
		pand	mm4,mm7				; mask out greens

		pand	mm3,mask_5			; mask out reds
		psrlw	mm4,5				; shift greens into position

		psrlw	mm5,11				; shift blues ino position
		por		mm3,mm4				; merge greens into reds

		movq	mm6,[ebp+eax]		; Read current mask
		por		mm3,mm5				; merge blues into reds and greens

		movq	mm4,[edi+eax]		; Read current pixels
		pand	mm3,mm6				; Mask out new pixels

		pandn	mm6,mm4				; Mask out old pixels
		add		eax,WORD_STEP		; Loop control

		por		mm6,mm3				; Merge new and old pixels
		jne		p2_loop_body

p2_loop_tail:
		D_PADDW	mm2,WORK.d_b_x		; Blue increments (rolled)
;V

		movq	[edi-WORD_STEP],mm6	; Write previous pixels
		pand	mm1,mm7				; mask out greens

		pand	mm0,mask_5			; mask out reds
		psrlw	mm1,5				; shift greens into position

		psrlw	mm2,11				; shift blues ino position
		por		mm0,mm1				; merge greens into reds

		movq	mm6,[ebp]			; Read current mask
		por		mm0,mm2				; merge blues into reds and greens

		movq	mm4,[edi]			; Read current pixels
		pand	mm0,mm6				; Mask out new pixels

; XXX AGI
ifidni direction,<lr>
		add		ebp,8				; Move on in mask buffer
else
		lea		ebp,[ebp+ebx+8]
endif
		pandn	mm6,mm4				; Mask out old pixels

		mov		edx,[ebp+4]			; Next line end and carry
		por		mm6,mm0				; Merge new and old pixels

		sar		edx,31				; Propogate carry down whole register
;v

		movq	[edi],mm6			; Write last pixels
;V

	; Loop while still scan converted mask data
	;
		cmp		ebp,ecx
		jb		p2_sl_loop

skip_triangle:
	;; Pop workspace stack and jump
	;;
		RASTERISE_EXIT

p2_one_word:
		mov		WORK.h.screen_address,edi	; Write screen address
		por		mm3,mm4						; merge greens into reds

		movq	mm4,[ebp+8]					; Read current mask
		por		mm3,mm5						; merge blues into reds and greens

		movq	mm6,[edi+ebx*2]				; Read current pixels
		pand	mm3,mm4						; Mask out new pixels

		pandn	mm4,mm6						; Mask out old pixels
		mov		edx,[ebp+20]				; Next line end and carry

		por		mm4,mm3						; Merge new and old pixels
		mov		eax,[ebp+16]				; Next line length

		sar		edx,31						; Propogate carry down whole register
		add		ebp,16

		movq	[edi+ebx*2],mm4				; Write pixels
;V

	; Loop while still scan converted mask data
	;
		cmp		ebp,ecx
		jb		p2_sl_loop_1

	;; Pop workspace stack and jump
	;;
		RASTERISE_EXIT
