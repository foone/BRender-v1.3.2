;; tzuvlm.inc
;;
;; Triangle rasterise loop Z, Affine Texture, MMX
;;
DITHER_BILINEAR = 0

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

M0			=		0
M1			=		1
M2			=		2
M3			=		3

mm_t0		equ		<mm0>
mm_t1		equ		<mm1>
mm_01l		equ		<mm0>
mm_01r		equ		<mm1>

mm_t2		equ		<mm1>
mm_t3		equ		<mm2>
mm_23l		equ		<mm1>
mm_23r		equ		<mm2>

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

M0			=		3
M1			=		2
M2			=		1
M3			=		0

mm_t0		textequ		<mm0>
mm_t1		textequ		<mm1>
mm_01l		textequ		<mm1>
mm_01r		textequ		<mm0>

mm_t2		textequ		<mm2>
mm_t3		textequ		<mm0>
mm_23l		textequ		<mm0>
mm_23r		textequ		<mm2>

endif

	; Use function pointer field in header to accumulate transparency mask
	;

	temp_mask equ WORK.h.function

    			sub			esp,PARAM_OFFSET


;	Unpack U&V (with optional tiling), to use 32 bit accumulators
;
	;; Unpack U & V
	;;

		; If going right to left, flip d_x direction and advance start position
		; to end of 4 pixel boundary
		;
ifidni direction,<rl>
		        mov			eax,PARAM.s_u
		        mov			ebx,PARAM.s_v

		        mov			ecx,PARAM.d_u_x
		        mov			edx,PARAM.d_v_x

		        add			eax,ecx
		        add			ebx,edx

   		        add			eax,ecx
		        add			ebx,edx

		        add			eax,ecx
		        add			ebx,edx

		        neg			ecx

		        neg			edx

		        mov			PARAM.s_u,eax
		        mov			PARAM.s_v,ebx

		        mov			PARAM.d_u_x,ecx
		        mov			PARAM.d_v_x,edx
endif

		; Find the shifts for the texture and tile size, and generate masks for each part
		;

		        xor			eax,eax
		        xor			ebx,ebx

		        xor			ecx,ecx
		        mov			bl,PARAM.tinfo.width_s

		        mov			cl,PARAM.tinfo.height_s

		        mov			al,PARAM.tinfo.tile_s
		        add			ecx,ebx

   		        add			ebx,eax

		        movq		mm5,qword ptr uv_masks[eax*8]			; Tile mask

		        movq		mm3,qword ptr uv_masks[ebx*8]			; U mask
		        movq		mm6,mm5

		        movq		mm4,qword ptr uv_masks[ecx*8]			; V mask
		        movq		mm7,mm3

				pandn		mm7,mm4						; Clear low bits of V mask
				pandn		mm6,mm3						; Clear low bits of U mask

		        sub			ebx,eax					; Adjust V shift

		; mm5 = V low integer mask	(2 copies)
		; mm6 = U integer mask 		(2 copies)
		; mm7 = V high integer mask (2 copies)
		;
		; eg: (256x256 with 2 bit tile)
		;
		; mm5: 00000000 00000000 00110000 00000000
		; mm6: 00000000 00111111 11000000 00000000
		; mm7: 00111111 11000000 00000000 00000000
		;
PACK_U			macro bridge:=<1>
				movd		mm4,eax
		        movq		mm1,mm0				; make copies of values


		        psrad		mm0,1				; align fraction bits
		        pslld		mm1,mm4				; shift integer part up

		        pand		mm0,qword ptr fraction_mask	; Mask of fraction bits
		        pand		mm1,mm6				; Mask out integer bits

		        por			mm0,mm1				; Merge integer and fraction
if bridge
		        por			mm0,qword ptr fraction_bit 	; set bridging bits
				por			mm0,mm5
endif
				endm

PACK_V			macro	bridge:=<1>
				movd		mm4,ebx
		        movq		mm1,mm0				; make copies of values
		        movq		mm2,mm0


		        psrad		mm0,1				; align fraction bits
		        pslld		mm1,mm4				; shift integer part up

		        pand		mm0,qword ptr fraction_mask	; Mask of fraction bits
		        pand		mm1,mm7				; Mask out high integer bits
				pand		mm2,mm5				; mask out low integer bits

		        por			mm0,mm1				; Merge integer and fraction
				por			mm0,mm2

if bridge
		        por			mm0,qword ptr fraction_bit 	; set bridging bits
				por			mm0,mm6
endif
				endm


	;; U
	;;
		        movd 		mm0,PARAM.d_u_y1	; Load dy1
				punpckldq	mm0,qword ptr PARAM.d_u_y0	;          and dy0

				PACK_U

				movq		mm1,mm0
				punpckldq	mm0,mm0

				punpckhdq	mm1,mm1

		        movq 		qword ptr WORK.d_u_y1,mm0
		        movq 		qword ptr WORK.d_u_y0,mm1

				movd		mm0,PARAM.d_u_x
				punpckldq	mm0,mm0
				pslld		mm0,2

				PACK_U

		        movd 		WORK.d_u_x,mm0


				movd		mm0,PARAM.s_u		; Replicate start value into mm4
				pxor		mm1,mm1

				movd		mm3,PARAM.d_u_x
				punpckldq	mm0,mm0

		        punpckldq	mm1,mm3			    ; Load dx into hi word of mm0

				punpckldq	mm3,mm3
				paddd		mm1,mm1

				paddd		mm0,mm1				; Adjust the second start pixel's value

				paddd		mm3,mm0

				PACK_U	0

		        movq 		qword ptr WORK.u0,mm0

				movq		mm0,mm3

				PACK_U	0

		        movq 		qword ptr WORK.u1,mm0

	;; V
	;;
		        movd 		mm0,PARAM.d_v_y1	; Load dy1
				punpckldq	mm0,qword ptr PARAM.d_v_y0	;          and dy0

				PACK_V

				movq		mm1,mm0
				punpckldq	mm0,mm0

				punpckhdq	mm1,mm1

		        movq 		qword ptr WORK.d_v_y1,mm0
		        movq 		qword ptr WORK.d_v_y0,mm1

				movd		mm0,PARAM.d_v_x
				punpckldq	mm0,mm0
				pslld		mm0,2

				PACK_V

		        movd 		WORK.d_v_x,mm0


				movd		mm0,PARAM.s_v		; Replicate start value into mm4
				pxor		mm1,mm1

				movd		mm3,PARAM.d_v_x
				punpckldq	mm0,mm0

		        punpckldq	mm1,mm3			    ; Load dx into hi word of mm0

				punpckldq	mm3,mm3
				paddd		mm1,mm1

				paddd		mm0,mm1				; Adjust the second start pixel's value

				paddd		mm3,mm0

				PACK_V	0

		        movq 		qword ptr WORK.v0,mm0

				movq		mm0,mm3

				PACK_V	0

		        movq 		qword ptr WORK.v1,mm0


		; Build U & V masks
		;
if DITHER_BILINEAR
				movq		mm0,mm5
				movq		mm1,mm6

				por			mm0,fraction_bit

				por			mm1,fraction_bit

				movd		WORK.u_bridge_bits,mm0

				movd		WORK.v_bridge_bits,mm1
endif

                por			mm7,mm5

				por			mm6,qword ptr fraction_mask
				por			mm7,qword ptr fraction_mask

				movd		WORK.u_mask,mm6

				movd		WORK.v_mask,mm7


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

if SCREENDOOR or DITHER or DITHER_BILINEAR
				mov			ebp,WORK.h.start_scanline
endif
				pmaddwd		mm5,mm6

				mov			eax,WORK.h.xm
				mov			ebx,WORK.h.x1

				sar			eax,16
				mov			ecx,WORK.h.counts

				paddd		mm5,qword ptr WORK.h.screen_address

				sar			ebx,16

if SCREENDOOR or DITHER or DITHER_BILINEAR
				and			ebp,3

				mov			WORK.h.start_scanline,ebp
endif

				movq		qword ptr WORK.h.screen_address,mm5

				mov			edi,WORK.h.screen_address
				test		ecx,ecx

				mov			esi,WORK.h.depth_address
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
				test		ecx,ecx				        ; See if there is another count of bottom of dword
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

; Cycle

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
				movq		mm0,qword ptr WORK.u0
				movq		mm1,qword ptr WORK.v0
				movq		mm2,qword ptr WORK.u1
				movq		mm3,qword ptr WORK.v1

				paddd		mm0,qword ptr WORK.d_u_y0[edx*8]
				paddd		mm1,qword ptr WORK.d_v_y0[edx*8]

				paddd		mm2,qword ptr WORK.d_u_y0[edx*8]
				paddd		mm3,qword ptr WORK.d_v_y0[edx*8]

				movd		mm4,WORK.u_mask
				punpckldq	mm4,mm4

				movd		mm5,WORK.v_mask
				punpckldq	mm5,mm5

				pand		mm0,mm4
				pand		mm1,mm5

				pand		mm2,mm4
				pand		mm3,mm5

				movq		qword ptr WORK.u0,mm0
				movq		qword ptr WORK.v0,mm1

				movq		qword ptr WORK.u1,mm2
				movq		qword ptr WORK.v1,mm3


				movq		mm5,qword ptr WORK.d_z_y0[edx*8]
;V
				movq		mm0,qword ptr WORK.z0
				movq		mm1,qword ptr WORK.z2

				paddd		mm0,mm5
				paddd		mm1,mm5


				movq		qword ptr WORK.z0,mm0
;V

				movq		qword ptr WORK.z2,mm1
;V

	; Increment addresses
	;
if  DITHER or SCREENDOOR or DITHER_BILINEAR
				mov			ebp,WORK.h.start_scanline
				inc			ebp
				and			ebp,3
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
				pxor		mm5,mm5

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

				movq		mm6,mm0			; make copy of new Z's
;UV

				psubusw		mm6,mm7			; Compare old and new Z's (Unsigned)
;UV

				pcmpeqw		mm6,qword ptr zeros		; Make a mask from the results of PSUB
;V

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
;V

				test		eax,eax			; Early out of no pixels visible on scanline
				je			next_line

	;; Colour Pass
	;;

	;; MANY WORD CASE
	;;

	    ; MMX Linear texture mapping, 4 pixels at a time, with transparency
	    ;
		; mm0 = pixels 0 & 1
		; mm1 = pixels 2 & 3
		; mm2 = scratch
		; mm3 = scratch
		; mm4 = U0.U1
		; mm5 = V0.V1
		; mm6 = U2.U3
		; mm7 = V2.V3
	    ;
			    mov			WORK.mask_end,ebp
			    mov			WORK.screen_end,edi

			    mov			eax,ebx
			    xor			edx,edx
			    xor			ecx,ecx

if DITHER_BILINEAR
				mov			ebx,WORK.h.start_scanline

				movd		mm2,WORK.u_bridge_bits
				pxor		mm5,mm5

				movq		mm4,dither_table_11x[ebx*8]
				movq		mm6,mm4

				movd		mm3,WORK.v_bridge_bits

				punpcklwd	mm4,mm5

				punpckhwd	mm6,mm5

				movq		mm5,mm4
				movq		mm7,mm6

				punpckldq	mm2,mm2
				punpckldq	mm3,mm3

				por			mm4,mm2
				por			mm6,mm2

				por			mm5,mm3
				por			mm7,mm3

				paddd		mm4,WORK.u0

				paddd		mm6,WORK.u1

				paddd		mm5,WORK.v0

				paddd		mm7,WORK.v1
; XXX Mask
		        movq		mm0,WORK.u_mask

				movq		mm1,mm0
				punpckldq	mm0,mm0

				punpckhdq	mm1,mm1

				pand		mm4,mm0
				pand		mm5,mm1
				pand		mm6,mm0
				pand		mm7,mm1

else
				movq		mm4,qword ptr WORK.u0

				movq		mm5,qword ptr WORK.v0

				movq		mm6,qword ptr WORK.u1

				movq		mm7,qword ptr WORK.v1
endif

		        mov			esi,WORK.texture_address
		        movq		mm1,mm6				    ; Copy of V's

c_loop_body:

		        mov			edi,WORK.map_address
		        movq		mm0,mm4				    ; Copy of U's

		        paddd		mm0,mm5				; Merge Us and Vs to get texel offsets
		        paddd		mm1,mm7

		        movq		mm2,qword ptr WORK.d_u_x
   		        psrld		mm0,FRACTION_BITS+1		; Get integer part of texel offsets

		        movq		mm3,mm2
		        psrld		mm1,FRACTION_BITS+1		; Get integer part of texel offsets

		        movd		ebx,mm0
		        punpckldq	mm2,mm2

		        movd		ebp,mm1
		        punpckhdq	mm3,mm3

		        mov			dl,[esi+ebx]			; Read Texel 0
		        psrlq		mm0,32

		        mov			cl,[esi+ebp]			; Read Texel 1
		        psrlq		mm1,32

		        mov			byte ptr temp_mask+M0,dl
		        paddd		mm4,mm2

		        mov			byte ptr temp_mask+M1,cl
		        paddd		mm6,mm2

		        movq		mm2,qword ptr WORK.u_mask
		        paddd		mm5,mm3

		        movd		ebx,mm0
		        paddd		mm7,mm3

		        movd		ebp,mm1
		        movq		mm3,mm2

		        movd		mm_t0,[edi+edx*4]
		        punpckldq	mm2,mm2

		        movd		mm_t1,[edi+ecx*4]
		        punpckhdq	mm3,mm3

		        mov			dl,[esi+ebx]			; Read Texel 2
		        punpcklbw	mm_01l,mm_01r

		        mov			cl,[esi+ebp]			; Read Texel 3
		        mov			byte ptr temp_mask+M2,dl

		        pand		mm4,mm2
		        pand		mm6,mm2

		        movd		mm_t2,[edi+edx*4]
		        pand		mm5,mm3

		        movd		mm_t3,[edi+ecx*4]
		        pand		mm7,mm3

		        mov			byte ptr temp_mask+M3,cl
		        punpcklbw	mm_23l,mm_23r


		; Now have the four texels and transparency mask
		;
		; mm0	a0.a1.r0.r1.g0.g1.b0.b1
		; mm1   a2.a3.r2.r3.g2.g3.b2.b3
		;
		; Unpack to:
		; mm0 = R3.R2.R1.R0
		; mm1 = G3.G2.G1.G0
		; mm2 = B3.B2.B1.B0
		; mm3 = M3.M2.M1.M0
		;
if DITHER
				movd		mm3,temp_mask	   		; Get the 4 texel indices
				movq		mm2,mm0

				mov			ebx,WORK.h.start_scanline
				punpcklwd	mm0,mm1

				mov			edi,WORK.screen_end
				punpckhwd	mm2,mm1

				movq		mm1,mm0
				punpcklbw	mm2,mm2					; Unpack blues

				paddusw		mm2,qword ptr dither_table_11[ebx*8]
				punpcklbw	mm0,mm0                 ; Unpack reds
						
				punpckhbw	mm1,mm1                 ; Unpack greens
				paddusw		mm0,qword ptr dither_table_11[ebx*8]

				paddusw		mm1,qword ptr dither_table_10[ebx*8]
				punpcklbw	mm3,mm3			   		; Convert mask bytes to words

				pcmpeqw		mm3,qword ptr zeros		   		; mm3 now has the transparency mask for 4 texels
				pand		mm2,qword ptr mask_5

if COLOUR_TYPE_15 
				pand		mm1,qword ptr mask_5
endif
if COLOUR_TYPE_15 
				pand		mm1,qword ptr mask_6
endif
				psrlw		mm0,11

if COLOUR_TYPE_15 
				psrlw		mm2,1
endif
				mov			esi,WORK.mask_end
				por			mm0,mm2

else
				movd		mm3,temp_mask	   		; Get the 4 texel indices
				movq		mm2,mm0

				pcmpeqb		mm3,qword ptr zeros		   		; mm4 now has the transparency mask for 4 texels
				punpcklwd	mm0,mm1

			    mov			edi,WORK.screen_end
				punpckhwd	mm2,mm1

				movq		mm1,mm0
				punpcklbw	mm2,mm2					; Unpack blues

				pand		mm2,qword ptr mask_5
				punpcklbw	mm0,mm0                 ; Unpack reds

				mov			esi,WORK.mask_end
				punpckhbw	mm1,mm1                 ; Unpack greens

if COLOUR_TYPE_15 
				pand		mm1,qword ptr mask_5
endif
if COLOUR_TYPE_16
				pand		mm1,qword ptr mask_6
endif
				psrlw		mm0,11

if COLOUR_TYPE_15 
				psrlw		mm2,1
				
endif
				punpcklbw	mm3,mm3			   		; Convert mask bytes to words
				por			mm0,mm2
endif

			    pandn		mm3,[esi+eax]			; Merge transp with current mask
if COLOUR_TYPE_15 
				psrlw		mm1,6
endif
if COLOUR_TYPE_16
				psrlw		mm1,5
endif
			    movq		mm2,[edi+eax]			; Read old destination
			    por			mm0,mm1

			    pand		mm0,mm3
			    pandn		mm3,mm2

			    por			mm0,mm3
			    test		eax,eax

		        mov			esi,WORK.texture_address
		        movq		mm1,mm6				    ; Copy of V's

			    movq		[edi+eax],mm0

			    lea			eax,[eax+WORD_STEP]
			    jne			c_loop_body


			    mov			ecx,WORK.h.counts

			    mov			eax,WORK.h.xm		    ; Load up edges for next loop
			    mov			ebx,WORK.h.x1

			    sub			ecx,10000h			    ; Decrement count (in hi word of dword)
			    jns			sl_loop

			    rol			ecx,16
			    jmp			next_trapezoid_2

one_word:

		; Depth test and writeback for a scanline of 1 word
		;
		; At entry:
	    ;
	    ; mm0	Z01		16.16 16.16
	    ; mm1	Z23		16.16 16.16
	    ;
	    ; esi	pointer to depth buffer word
	    ; edi	pointer to word in colour buffer
	    ;
	    ; ecx	pixel within word of scanline start
	    ; edx	pixel within word of scanline end
	    ;
			    movq		mm7,[esi+ebx*2]		    ; read current z buffer pixels
			    psrad		mm0,16				    ; Shift to get integer parts

if SCREENDOOR
				pand		mm6,qword ptr START_MASKS[ecx*8]	; Mask for front of scanline
else
				movq		mm6,qword ptr START_MASKS[ecx*8]	; Mask for front of scanline
endif
				psrad		mm1,16

			    pand		mm6,qword ptr END_MASKS[edx*8]	; Mask for back of scanline
			    packssdw	mm0,mm1	  				; merge Zs down to 16 bits per pixel

			    movq		mm1,mm0	 				; make copy of new Z's
			    psubusw		mm0,mm7		  			; Compare old and new Z's (Unsigned)

			    pcmpeqw		mm0,qword ptr zeros				; Make a mask from the results of PSUB

			    pand		mm0,mm6					; Mask for length scanline

			    movq		mm5,mm0                 ; copy mask for early out test

			    pand		mm1,mm0			        ; Select new Z's using mask
			    pandn		mm0,mm7			        ; Select old Z's using mask

			    por			mm0,mm1			        ; Combine old and new Z's
			    movq		mm6,mm5			        ; copy mask for colour pass

			    movq		[esi+ebx*2],mm0	        ; Write to  Z buffer pixels
			    packsswb	mm5,mm5			        ; Merge mask down to 32 bits

			    movd		eax,mm5

			    test		eax,eax			        ; Early out of no pixels visible on scanline
			    je			next_line

	;; ONE WORD CASE
	;;
	;; mm6 = pixel mask
				mov			eax,ebx

if DITHER_BILINEAR
				mov			ebx,WORK.h.start_scanline

				movd		mm2,WORK.u_bridge_bits
				pxor		mm5,mm5

				movq		mm0,dither_table_11x[ebx*8]
				movq		mm1,mm0

				movd		mm3,WORK.v_bridge_bits

				punpcklwd	mm0,mm5

				punpckhwd	mm1,mm5

				movq		mm5,mm0
				movq		mm7,mm1

				punpckldq	mm2,mm2
				punpckldq	mm3,mm3

				por			mm0,mm2
				por			mm1,mm2

				por			mm5,mm3
				por			mm7,mm3

				paddd		mm0,WORK.u0

				paddd		mm1,WORK.u1

				paddd		mm5,WORK.v0

				paddd		mm7,WORK.v1
; XXX Mask
		        movq		mm2,WORK.u_mask

				movq		mm3,mm2
				punpckldq	mm2,mm2

				punpckhdq	mm3,mm3

				pand		mm0,mm2
				pand		mm5,mm3
				pand		mm1,mm2
				pand		mm7,mm3

				paddd		mm0,mm5
				paddd		mm1,mm7

else
				movq		mm0,qword ptr WORK.u0

				movq		mm1,qword ptr WORK.u1

				paddd		mm0,qword ptr WORK.v0

				paddd		mm1,qword ptr WORK.v1
endif

   		        psrld		mm0,FRACTION_BITS+1		; Get integer part of texel offsets

		        mov			esi,WORK.texture_address
		        psrld		mm1,FRACTION_BITS+1		; Get integer part of texel offsets

		        movd		ebx,mm0

		        movd		ebp,mm1

		        mov			dl,[esi+ebx]			; Read Texel 0
		        psrlq		mm0,32

		        mov			cl,[esi+ebp]			; Read Texel 1
		        psrlq		mm1,32

		        mov			edi,WORK.map_address
		        mov			byte ptr temp_mask+M0,dl

		        mov			byte ptr temp_mask+M1,cl

		        movd		ebx,mm0

		        movd		ebp,mm1

		        movd		mm_t0,[edi+edx*4]

		        movd		mm_t1,[edi+ecx*4]

		        mov			dl,[esi+ebx]			; Read Texel 2
		        punpcklbw	mm_01l,mm_01r

		        mov			cl,[esi+ebp]			; Read Texel 3
		        mov			byte ptr temp_mask+M2,dl

		        movd		mm_t2,[edi+edx*4]

		        movd		mm_t3,[edi+ecx*4]

		        mov			byte ptr temp_mask+M3,cl
		        punpcklbw	mm_23l,mm_23r

		; Now have the four texels and transparency mask
		;
		; mm0	a0.a1.r0.r1.g0.g1.b0.b1
		; mm1   a2.a3.r2.r3.g2.g3.b2.b3
		;
		; Unpack to:
		; mm0 = R3.R2.R1.R0
		; mm1 = G3.G2.G1.G0
		; mm2 = B3.B2.B1.B0
		; mm3 = M3.M2.M1.M0
		;
				mov			edi,WORK.h.screen_address

if DITHER
				movd		mm3,temp_mask	   		; Get the 4 texel indices
				movq		mm2,mm0

				mov			ebx,WORK.h.start_scanline
				punpcklwd	mm0,mm1

				lea			edi,[edi+eax*2]
				punpckhwd	mm2,mm1

				movq		mm1,mm0
				punpcklbw	mm2,mm2					; Unpack blues

				paddusw		mm2,qword ptr dither_table_11[ebx*8]
				punpcklbw	mm0,mm0                 ; Unpack reds
						
				punpckhbw	mm1,mm1                 ; Unpack greens
				paddusw		mm0,qword ptr dither_table_11[ebx*8]

				paddusw		mm1,qword ptr dither_table_10[ebx*8]
				pand		mm2,qword ptr mask_5
;V

				pcmpeqb		mm3,qword ptr zeros		   		; mm3 now has the transparency mask for 4 texels
if COLOUR_TYPE_15
                psrlw       mm2,1
endif

else
				movd		mm3,temp_mask	   		; Get the 4 texel indices
				movq		mm2,mm0

				pcmpeqb		mm3,qword ptr zeros		   		; mm4 now has the transparency mask for 4 texels
				punpcklwd	mm0,mm1

				lea			edi,[edi+eax*2]
				punpckhwd	mm2,mm1

				movq		mm1,mm0
				punpcklbw	mm2,mm2					; Unpack blues

				pand		mm2,qword ptr mask_5
				punpcklbw	mm0,mm0                 ; Unpack reds

				punpckhbw	mm1,mm1                 ; Unpack greens
if COLOUR_TYPE_15
                psrlw       mm2,1
endif
endif
if COLOUR_TYPE_15
                pand        mm1,qword ptr mask_5
endif
if COLOUR_TYPE_16
				pand		mm1,qword ptr mask_6
endif
				psrlw		mm0,11

				punpcklbw	mm3,mm3			   		; Convert mask bytes to words
				por			mm0,mm2

			    pandn		mm3,mm6					; Merge transp with current mask

if COLOUR_TYPE_15 
                psrlw       mm1,6
endif

if COLOUR_TYPE_16
				psrlw		mm1,5
endif 


				movq		mm2,[edi]		        ; Read old destination
				por			mm0,mm1

				pand		mm0,mm3
				pandn		mm3,mm2

				mov			ecx,WORK.h.counts
				por			mm0,mm3

				mov			eax,WORK.h.xm	; Load up edges for next loop
				mov			ebx,WORK.h.x1

				movq		[edi],mm0

				sub			ecx,10000h		; Decrement count (in hi word of dword)
				jns			sl_loop

				rol			ecx,16
				jmp			next_trapezoid_2

