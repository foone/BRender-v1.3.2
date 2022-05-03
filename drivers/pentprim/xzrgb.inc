;; tzrgblm.inc
;;
;; Triangle rasterise loop Z, RGB, Large, MMX
;;
;; Conditional code can be added for dithering and screendoor transparency
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

START_MASKS	equ		right_masks
END_MASKS	equ		left_masks

WORD_STEP	=		-8
endif
    	sub		esp,PARAM_OFFSET


		UNPACK_PARAM_RGB
if SCREENDOOR
		UNPACK_SCREENDOOR_ALPHA
endif
		UNPACK_PARAM_16  PARAM.s_z,PARAM.d_z_x, WORK.z0,  WORK.d_z_y1,  WORK.d_z_y0,  WORK.d_z_x

	;; Setup for first iteration of loop and generate pointers to starting scanline
	;;
		movd		mm5,WORK.h.start_scanline
;V

		movq		mm6,qword ptr WORK.h.screen_stride
		punpckldq	mm5,mm5

		movq		mm2,qword ptr WORK.r01
		pmaddwd		mm5,mm6

		movq		mm3,qword ptr WORK.g01
;V

		movq		mm4,qword ptr WORK.b01
;V

		paddd		mm5,qword ptr WORK.h.screen_address
;V

		mov			eax,WORK.h.xm
		mov			ebx,WORK.h.x1

		movq		qword ptr WORK.h.screen_address,mm5
;V

		sar			eax,16
		mov			edi,WORK.h.screen_address

		sar			ebx,16
		mov			esi,WORK.h.depth_address

		mov			ecx,WORK.h.counts
		test		ecx,ecx

if SCREENDOOR
		mov			ebp,WORK.h.start_scanline
endif
		jns			start

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

		sar			ebx,16
		jmp			start

skip_triangle:
		RASTERISE_EXIT

	;; Swith to lower trapezoid
	;;
next_trapezoid_1:
		rol			ecx,16

next_trapezoid_2:
		test		ecx,ecx				; See if there is another count of bottom of dword
		js			skip_triangle

;sl_bottom:
		mov			ebx,WORK.h.d_x2

	; Version of X increments that loads lower X info
	;
	; Increment X's
	;
		mov			WORK.h.counts,ecx
		mov			eax,WORK.h.xm

		mov			WORK.h.d_x1,ebx
		mov			ecx,WORK.h.d_xm

		mov			edx,eax
		add			eax,ecx

		mov			WORK.h.xm,eax
		xor			edx,ecx

		xor			edx,eax
		mov			ebx,WORK.h.x2

		movq		mm0,qword ptr WORK.z0			; Load Z for parameter increment
;V

		shl			edx,14
		jmp			sl_bottom_cont

	; Jump here for empty or skipped line
	;
next_line:
		mov			eax,WORK.h.xm		; Load up edges for next loop
		mov			ebx,WORK.h.x1

		mov			ecx,WORK.h.counts
;V
		sub			ecx,10000h			; Decrement count (in hi word of dword)
		js			next_trapezoid_1

	;; Per scanline updates
	;;

sl_loop:

	; Increment X's and generate flag for carry from bit 17->18 (crossing dword)
	;
		mov			WORK.h.counts,ecx
		mov			ecx,WORK.h.d_xm

		mov			edx,eax
		add			eax,ecx

		mov			WORK.h.xm,eax
		xor			edx,ecx

		xor			edx,eax
		mov			ecx,WORK.h.d_x1

		movq		mm0,qword ptr WORK.z0			; Load Z for parameter increment
;V

		shl			edx,14
		lea			ebx,[ebx+ecx]

sl_bottom_cont:
		sbb			edx,edx
		mov			WORK.h.x1,ebx

			; eax:	start X (pixel)
			; ebx:	end Y	(pixel)
			;
			; edx:  -1 or 0 (if carry)

	; Increment parameters
	;
		movq		mm1,qword ptr WORK.z2
;V

		movq		mm5,qword ptr WORK.d_z_y0[edx*8]
;V

		movq		mm2,qword ptr WORK.r01
		paddd		mm0,mm5

		movq		mm3,qword ptr WORK.g01
		paddd		mm1,mm5

		movq		mm4,qword ptr WORK.b01
;V

		paddw		mm2,qword ptr WORK.d_r_y0[edx*8]
;V

		paddw		mm3,qword ptr WORK.d_g_y0[edx*8]
;V

		paddw		mm4,qword ptr WORK.d_b_y0[edx*8]
;V

		movq		qword ptr WORK.z0,mm0
;V

		movq		qword ptr WORK.z2,mm1
;V

		movq		qword ptr WORK.r01,mm2
;V

		movq		qword ptr WORK.g01,mm3
;V

		movq		qword ptr WORK.b01,mm4
		pxor		mm5,mm5			; Clear combined Z mask

	; Increment addresses
	;
if  DITHER or SCREENDOOR
		mov			ebp,WORK.h.start_scanline
		inc			ebp
		mov			WORK.h.start_scanline,ebp
endif

		mov			edi,WORK.h.screen_address
		mov			esi,WORK.h.depth_address

		add			edi,WORK.h.screen_stride
		add			esi,WORK.h.depth_stride

		sar			eax,16			; Get integer part of X start
		mov			WORK.h.screen_address,edi

		sar			ebx,16			; Get integer part of X end
		mov			WORK.h.depth_address,esi

start:
	; Generate masks and pointers
	; 
		cmp			eax,ebx
		J_EMPTY		next_line		; No pixels on line at all

if SCREENDOOR
	; Lookup up screendoor transparency mask
	;
		mov			ecx,WORK.h._c
		and			ebp,3

		or			ebp,ecx
endif

		mov			ecx,eax
		mov			edx,ebx

if SCREENDOOR
		movq		mm6,qword ptr screendoor_masks[ebp*8]
endif

		and			eax,not 3
		and			ebx,not 3

		and			ecx,3
		and			edx,3

		sub			eax,ebx
		je			one_word		; Scanline fits in one word

	;; Render scanline
	;;
	;; Z Pass
	;;

if SCREENDOOR
		movq		qword ptr screendoor_mask,mm6
endif

	; Setup for Z
	;
		add			eax,eax			; convert pixel number to address offset
		mov			ebp,offset scanline_mask

		lea			esi,[esi+ebx*2]	; Move pointers on to end of scanline
		lea			edi,[edi+ebx*2]

	; Z read test, and write - 2.6 cycles per pixel
	;
	; Depth test and writeback for a scanline of 2 words or more
	;
	; At entry:
	;
	; mm0	Z01		16.16 16.16
	; mm1	Z23		16.16 16.16
	;
	; ebp	pointer to end of mask buffer
	; esi	pointer to end of scanline in depth buffer
	; eax	-count
	;
	; ecx	pixel within first word of scanline start
	; edx	pixel within last word of scanline end
	;

	; Loop head
	;
	;
		movq		mm2,mm0			; Make a copy of Z01
		movq		mm3,mm1			; Make a copy of Z23

		D_PADDD		mm0,qword ptr WORK.d_z_x	; Add delta to Z01
		psrad		mm2,16			; Shift to get integer parts

		D_PADDD		mm1,qword ptr WORK.d_z_x	; Add delta to Z23
		psrad		mm3,16

		mov			ebx,eax			; Copy of loop counter for second pass
		packssdw	mm2,mm3			; merge Zs down to 16 bits per pixel

		movq		mm7,[esi+eax]	; read current z buffer pixels
		movq		mm6,mm2			; make copy of new Z's

ifidni direction,<lr>
		sub			ebp,eax			; Set up mask pointer for left to right
endif
		psubusw		mm6,mm7			; Compare old and new Z's (Unsigned)

		pcmpeqw		mm6,qword ptr zeros		; Make a mask from the results of PSUB
;V

		pand		mm6,qword ptr START_MASKS[ecx*8]	; Mask for front of scanline
;V
if SCREENDOOR
		pand		mm6,qword ptr screendoor_mask
endif

		movq		mm4,mm6			; Make a copy of mask
		pand		mm2,mm6			; Select new Z's using mask

		pandn		mm6,mm7			; Select old Z's using mask
		add			eax,WORD_STEP	; Loop control

		por			mm6,mm2			; Combine old and new Z's
		je			z_loop_tail

z_loop_body:
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

		D_PADDD		mm0,qword ptr WORK.d_z_x	; Add delta to Z01
		psrad		mm2,16			; Shift to get integer parts of Z01

		D_PADDD		mm1,qword ptr WORK.d_z_x	; Add delta to Z23
		psrad		mm3,16			; Shift to get integer parts of Z23

		movq		mm7,[esi+eax]	; read current z buffer pixels
		packssdw	mm2,mm3			; merge Zs down to 16 bits per pixel

		movq		[esi+eax-WORD_STEP],mm6	; Write to prev. Z buffer pixels
		movq		mm6,mm2			; make copy of new Z's

	; 10 cycle stall here if destination is not in cache
	;		
		movq		[ebp+eax-WORD_STEP],mm4	; Write out previous mask
		psubusw		mm6,mm7			; Compare old and new Z's (Unsigned)

		pcmpeqw		mm6,qword ptr zeros		; Make a mask from the results of PSUB
		por			mm5,mm4			; Accumulate previous mask

if SCREENDOOR
		pand		mm6,qword ptr screendoor_mask
endif

		movq		mm4,mm6			; Make a copy of mask
		pand		mm2,mm6			; Select new Z's using mask

		pandn		mm6,mm7			; Select old Z's using mask
		add			eax,WORD_STEP

		por			mm6,mm2			; Combine old and new Z's
		jne			z_loop_body

	; Loop tail
	;
z_loop_tail:
		movq		mm7,[esi]		; read current z buffer pixels
		psrad		mm0,16			; Shift to get integer parts of Z01

		movq		[esi-WORD_STEP],mm6	; Write to prev. Z buffer pixels
		psrad		mm1,16			; Shift to get integer parts of Z23

		movq		[ebp-WORD_STEP],mm4	; Write out previous mask
		packssdw	mm0,mm1			; merge Zs down to 16 bits per pixel

		movq		mm2,qword ptr WORK.b01	; Load blues ready for colour pass
		movq		mm6,mm0			; make copy of new Z's

		movq		mm1,qword ptr WORK.g01	; Load greens ready for colour pass
		psubusw		mm6,mm7			; Compare old and new Z's (Unsigned)

		pcmpeqw		mm6,qword ptr zeros		; Make a mask from the results of PSUB
; V
		pand		mm6,qword ptr END_MASKS[edx*8]	; Mask for front of scanline
		por			mm5,mm4			; Accumulate previous mask
if SCREENDOOR
		pand		mm6,qword ptr screendoor_mask
endif
		movq		mm4,mm6			; Make a copy of mask

		pand		mm0,mm6			; Select new Z's using mask
		pandn		mm6,mm7			; Select old Z's using mask

		movq		mm7,qword ptr mask_6		; Set up a constant mask in mm7
		por			mm6,mm0			; Combine old and new Z's

		movq		[ebp],mm4		; Write out  mask
		por			mm5,mm4			; Accumulate  mask

		movq		[esi],mm6		; Write to Z buffer pixels
		packsswb	mm5,mm5

		movd		eax,mm5
		movq		mm4,mm1			; make a copy of greens

		test		eax,eax			; Early out of no pixels visible on scanline
		je			next_line

	;; Colour Pass
	;;

; loop head
;
	if DITHER
		movq		mm0,qword ptr WORK.r01		; Load reds
		mov			eax,WORK.h.start_scanline
		and			eax,3

		paddw		mm0,qword ptr dither_table_11[eax*8]
		paddw		mm1,qword ptr dither_table_10[eax*8]
		paddw		mm2,qword ptr dither_table_11[eax*8]

		movq		mm4,mm1				; make a copy of greens
		movq		mm5,mm2				; make a copy of blues

	else
		movq		mm0,qword ptr WORK.r01		; Load reds
		movq		mm5,mm2				; make a copy of blues
	endif

		D_PADDW		mm1,qword ptr WORK.d_g_x		; Green Increments
		movq		mm3,mm0				; make a copy of reds

		D_PADDW		mm0,qword ptr WORK.d_r_x		; Red Increments
if COLOUR_TYPE_15
		pand		mm4,qword ptr mask_5				; mask out greens
endif
if COLOUR_TYPE_16
		pand		mm4,qword ptr mask_6				; mask out greens
endif

		pand		mm3,qword ptr mask_5			; mask out reds
if COLOUR_TYPE_15
		psrlw		mm4,6				; shift greens into position
endif
if COLOUR_TYPE_16
		psrlw		mm4,5				; shift greens into position
endif

if COLOUR_TYPE_15
		psrlw		mm3,1
endif

		psrlw		mm5,11				; shift blues ino position
		por			mm3,mm4				; merge greens into reds

		movq		mm6,[ebp+ebx]		; Read current mask
		por			mm3,mm5				; merge blues into reds and greens

		movq		mm4,[edi+ebx]		; Read current pixels
		pand		mm3,mm6				; Mask out new pixels

		pandn		mm6,mm4				; Mask out old pixels
		add			ebx,WORD_STEP		; Loop control

		por			mm6,mm3				; Merge new and old pixels
		je			c_loop_tail

c_loop_body:
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

		D_PADDW		mm2,qword ptr WORK.d_b_x		; Blue increments (rolled)
		movq		mm3,mm0				; make a copy of reds

		movq		[edi+ebx-WORD_STEP],mm6	; Write previous pixels
		movq		mm4,mm1				; make a copy of greens

		D_PADDW		mm0,qword ptr WORK.d_r_x		; Red Increments
		movq		mm5,mm2				; make a copy of blues

		D_PADDW		mm1,qword ptr WORK.d_g_x		; Green Increments
if COLOUR_TYPE_15
		pand		mm4,qword ptr mask_5				; mask out greens
endif
if COLOUR_TYPE_16
		pand		mm4,qword ptr mask_6				; mask out greens
endif

		pand		mm3,qword ptr mask_5			; mask out reds
if COLOUR_TYPE_15
		psrlw		mm4,6				; shift greens into position
endif
if COLOUR_TYPE_16
		psrlw		mm4,5				; shift greens into position
endif

if COLOUR_TYPE_15
		psrlw		mm3,1
endif

		psrlw		mm5,11				; shift blues ino position
		por			mm3,mm4				; merge greens into reds

		movq		mm6,[ebp+ebx]		; Read current mask
		por			mm3,mm5				; merge blues into reds and greens

		movq		mm4,[edi+ebx]		; Read current pixels
		pand		mm3,mm6				; Mask out new pixels

		pandn		mm6,mm4				; Mask out old pixels
		add			ebx,WORD_STEP		; Loop control

		por			mm6,mm3				; Merge new and old pixels
		jne			c_loop_body

c_loop_tail:
		D_PADDW		mm2,qword ptr WORK.d_b_x		; Blue increments (rolled)
if COLOUR_TYPE_15
		pand		mm1,qword ptr mask_5				; mask out greens
endif
if COLOUR_TYPE_16
		pand		mm1,qword ptr mask_6				; mask out greens
endif

		movq		[edi-WORD_STEP],mm6	; Write previous pixels
if COLOUR_TYPE_15
		psrlw		mm1,6				; shift greens into position
endif
if COLOUR_TYPE_16
		psrlw		mm1,5				; shift greens into position
endif

		pand		mm0,qword ptr mask_5			; mask out reds
		psrlw		mm2,11				; shift blues ino position

if COLOUR_TYPE_15
		psrlw		mm0,1
endif 

		movq		mm6,[ebp]			; Read current mask
		por			mm0,mm1				; merge greens into reds

		movq		mm4,[edi]			; Read current pixels
		por			mm0,mm2				; merge blues into reds and greens

		pand		mm0,mm6				; Mask out new pixels
		pandn		mm6,mm4				; Mask out old pixels

		mov			ecx,WORK.h.counts
		por			mm6,mm0				; Merge new and old pixels

		mov			eax,WORK.h.xm		; Load up edges for next loop
		mov			ebx,WORK.h.x1

		movq		[edi],mm6			; Write last pixels
;V
		sub			ecx,10000h			; Decrement count (in hi word of dword)
		jns			sl_loop

		rol			ecx,16
		jmp			next_trapezoid_2

one_word:
	; Depth test and writeback and RGB colour for a scanline of 1 word
	;
	; At entry:
	;
	; mm0	Z01		16.16 16.16
	; mm1	Z23		16.16 16.16
	; mm2	(R,R,R,R)
	; mm3	(G,G,G,G)
	; mm4	(B,B,B,B)
	;
	; mm7	mask for high 6 bits of each word
	;
	; esi	pointer to depth buffer word
	; edi	pointer to word in colour buffer
	;
	; ecx	pixel within word of scanline start
	; edx	pixel within word of scanline end
	;

if DITHER
		mov			eax,WORK.h.start_scanline
		and			eax,3

		paddw		mm2,qword ptr dither_table_11[eax*8]
		paddw		mm3,qword ptr dither_table_10[eax*8]
		paddw		mm4,qword ptr dither_table_11[eax*8]
endif

		movq		mm7,[esi+ebx*2]	; read current z buffer pixels
		psrad		mm0,16			; Shift to get integer parts

if SCREENDOOR
		pand		mm6,qword ptr START_MASKS[ecx*8]	; Mask for front of scanline
else
		movq		mm6,qword ptr START_MASKS[ecx*8]	; Mask for front of scanline
endif
		psrad		mm1,16

		pand		mm6,qword ptr END_MASKS[edx*8]	; Mask for back of scanline
		packssdw	mm0,mm1			; merge Zs down to 16 bits per pixel

		movq		mm1,mm0			; make copy of new Z's
		psubusw		mm0,mm7			; Compare old and new Z's (Unsigned)

		pcmpeqw		mm0,qword ptr zeros		; Make a mask from the results of PSUB
		psrlw		mm4,11			; shift blues ino position

if COLOUR_TYPE_15
		pand		mm3,qword ptr mask_5		; mask out greens
endif
if COLOUR_TYPE_16
		pand		mm3,qword ptr mask_6		; mask out greens
endif
		pand		mm0,mm6			; Mask for length scanline

		mov			edx,ebx
		movq		mm5,mm0			; copy mask for early out test

		pand		mm1,mm0			; Select new Z's using mask
		pandn		mm0,mm7			; Select old Z's using mask

		por			mm0,mm1			; Combine old and new Z's
		movq		mm6,mm5			; copy mask for colour pass

		pand		mm2,qword ptr mask_5		; mask out reds
if COLOUR_TYPE_15
		psrlw		mm3,6			; shift greens into position
endif
if COLOUR_TYPE_16
		psrlw		mm3,5			; shift greens into position
endif

if COLOUR_TYPE_15
		psrlw		mm2,1
endif

		movq		[esi+edx*2],mm0	; Write to  Z buffer pixels
		packsswb	mm5,mm5			; Merge mask down to 32 bits

		movd		eax,mm5
		por			mm2,mm3			; merge greens into reds

		test		eax,eax			; Early out of no pixels visible on scanline
		je			next_line

		movq		mm3,[edi+edx*2]	; Read current pixels
		por			mm2,mm4			; merge blues into reds and greens

		pand		mm2,mm6			; Mask out new pixels
		pandn		mm6,mm3			; Mask out old pixels

		mov			ecx,WORK.h.counts
		por			mm2,mm6			; Merge new and old pixels

		mov			eax,WORK.h.xm	; Load up edges for next loop
		mov			ebx,WORK.h.x1

		movq		[edi+edx*2],mm2	; Write pixels
;V
		sub			ecx,10000h		; Decrement count (in hi word of dword)
		jns			sl_loop

		rol			ecx,16
		jmp			next_trapezoid_2
