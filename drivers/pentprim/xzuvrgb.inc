;; tzuvlm.inc
;;
;; Triangle rasterise loop Z, Affine Texture, Constant colour MMX
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

M0			=		0
M1			=		1
M2			=		2
M3			=		3

	; Rather grim macros to parameterise the LR/RL texture fetch
	; It seems impossible to get a register equate using the
	; mmx registers!
	;
FETCH_T0	textequ	<movd		mm0,>
FETCH_T1	textequ	<movd		mm2,>

PACK_T01	textequ <punpcklbw	mm0,mm2>
PSRLD_T01	textequ <psrld		mm0,1>
PAND_T01	textequ <pand		mm0,mm4>

FETCH_T2	textequ	<movd		mm1,>
FETCH_T3	textequ	<movd		mm2,>

PACK_T23	textequ <punpcklbw	mm1,mm2>
PSRLD_T23	textequ <psrld		mm1,1>
PAND_T23	textequ <pand		mm1,mm4>

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

FETCH_T0	textequ	<movd		mm2,>
FETCH_T1	textequ	<movd		mm1,>

PACK_T01	textequ <punpcklbw	mm1,mm2>
PSRLD_T01	textequ <psrld		mm1,1>
PAND_T01	textequ <pand		mm1,mm4>

FETCH_T2	textequ	<movd		mm2,>
FETCH_T3	textequ	<movd		mm0,>

PACK_T23	textequ <punpcklbw	mm0,mm2>
PSRLD_T23	textequ <psrld		mm0,1>
PAND_T23	textequ <pand		mm0,mm4>

endif

		; Use function pointer field in header to accumulate transparency mask
		;
				temp_mask equ WORK.h.function

		; Setup work area
		;
    			sub			esp,PARAM_OFFSET

		        UNPACK_UV_32
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

				pmaddwd		mm5,mm6

				mov			eax,WORK.h.xm
				mov			ebx,WORK.h.x1

				sar			eax,16
				mov			ecx,WORK.h.counts

				paddd		mm5,qword ptr WORK.h.screen_address

				sar			ebx,16
if DITHER or SCREENDOOR
				mov			ebp,WORK.h.start_scanline
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
				test		ecx,ecx				; See if there is another count of bottom of dword
				js			skip_triangle

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

   ;; Increment parameters
   ;;

				movq		mm5,qword ptr WORK.d_z_y0[edx*8]
;V
				movq		mm1,qword ptr WORK.z2

				paddd		mm0,mm5
				paddd		mm1,mm5

				mov			esi,WORK.u
				mov			edi,WORK.v

				movq		qword ptr WORK.z0,mm0
;V

				movq		qword ptr WORK.z2,mm1
;V

				add			esi,WORK.d_u_y0[edx*8]
				add			edi,WORK.d_v_y0[edx*8]

				and			esi,WORK.u_mask
				and			edi,WORK.v_mask

				mov			WORK.u,esi
				mov			WORK.v,edi


				movq		mm2,qword ptr WORK.r01

				movq		mm3,qword ptr WORK.g01

				movq		mm4,qword ptr WORK.b01
;V
				paddw		mm2,qword ptr WORK.d_r_y0[edx*8]
;V

				paddw		mm3,qword ptr WORK.d_g_y0[edx*8]
;V

				paddw		mm4,qword ptr WORK.d_b_y0[edx*8]
;V

				movq		qword ptr WORK.r01,mm2
;V

				movq		qword ptr WORK.g01,mm3
;V

				movq		qword ptr WORK.b01,mm4

		; Increment addresses
		;
if  DITHER or SCREENDOOR
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
				mov			ebp,offset scanline_mask
				add			eax,eax			; convert pixel number to address offset

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
	    ;						2222 2222 1111 1111 1100 0000 0000
	    ;                       7654 3210 9876 5432 1098 7654 3210
	    ; ecx	U accumulator - UUUU UU00 0000 00UU.0uuu uuuu uuuu
	    ; ebp   V accumulator - 0000 00VV VVVV VV00.0vvv vvvv vvvv
	    ;
	    ; dl	texel index
	    ; esi	Texture
	    ; edi	Palette
	    ; eax	Loop control
	    ; ebx	Texel address
	    ;
			    mov			eax,ebx
			    xor			edx,edx

			    mov			WORK.mask_end,ebp
			    mov			WORK.screen_end,edi

			    mov			ecx,WORK.u
			    mov			ebp,WORK.v

			    movq		mm7,qword ptr WORK.r01
			    pcmpeqw		mm4,mm4					; mm4 = 0ffffffffh

			    movq		mm6,qword ptr WORK.g01
			    psrlw		mm4,9					; mm4 = 0007f007fh

			    movq		mm5,qword ptr WORK.b01
			    packuswb	mm4,mm4					; mm4 = 07f7f7f7fh	- Mask for ARGB

c_loop_body:
			    lea			ebx,[ebp+ecx] 			; Generate texture address
			    mov			esi,WORK.texture_address

			    shr			ebx,FRACTION_BITS+1		; Get significant part of merged U&V
			    mov			edi,WORK.map_address

			    add			ecx,WORK.d_u_x	   		; Update U&V for next texel
			    add			ebp,WORK.d_v_x

			    mov			dl,[esi+ebx]   	   		; Read texel
		    nop

			    and			ecx,WORK.u_mask	   		; Tidy up U accumulator
			    and			ebp,WORK.v_mask	 		; Tidy up V accumulator

			    FETCH_T0	[edi+edx*4]	   			; Read palette (8.8.8)
;V

			    lea			ebx,[ebp+ecx]	   		; Generate texture address
			    mov			byte ptr temp_mask+M0,dl

			    shr			ebx,FRACTION_BITS+1		; Get significant part of merged U&V
		    nop

			    add			ecx,WORK.d_u_x	 		; Update U&V for next texel
			    add			ebp,WORK.d_v_x

			    mov			dl,[esi+ebx]   	   		; Read texel
		    nop

			    and			ecx,WORK.u_mask	   		; Tidy up U accumulator
			    and			ebp,WORK.v_mask	   		; Tidy up V accumulator

			    FETCH_T1	[edi+edx*4]	   			; Read palette (8.8.8) and merge texels 0 and 1 into same register
;V

			    lea			ebx,[ebp+ecx]	   		; Generate texture address
			    mov			byte ptr temp_mask+M1,dl

			    shr			ebx,FRACTION_BITS+1		; Get significant part of merged U&V
			    PACK_T01

			    add			ecx,WORK.d_u_x			; Update U&V for next texel
			    add			ebp,WORK.d_v_x

			    mov			dl,[esi+ebx]   	  		; Read texel
			    PSRLD_T01

			    and			ecx,WORK.u_mask	   		; Tidy up U accumulator
			    and			ebp,WORK.v_mask	   		; Tidy up V accumulator

			    FETCH_T2	[edi+edx*4]	  			; Read palette (8.8.8)
			    PAND_T01

			    lea			ebx,[ebp+ecx]	   		; Generate texture address
			    mov			byte ptr temp_mask+M2,dl

			    shr			ebx,FRACTION_BITS+1		; Get significant part of merged U&V
		    nop

			    add			ecx,WORK.d_u_x			; Update U&V for next texel
			    add			ebp,WORK.d_v_x

			    mov			dl,[esi+ebx]   	  		; Read texel
if DITHER
				mov			ebx,WORK.h.start_scanline
endif
			    and			ecx,WORK.u_mask	   		; Tidy up U accumulator
			    and			ebp,WORK.v_mask	   		; Tidy up V accumulator

			    FETCH_T3	[edi+edx*4]	   			; Read palette (8.8.8)

			    mov			byte ptr temp_mask+M3,dl
			    PACK_T23

			    PSRLD_T23
			    movd		mm3,temp_mask	   		; Get the 4 texel indices

			    PAND_T23
			    punpcklbw	mm3,mm3			   		; Convert bytes to words

			    movq		mm2,mm0
			    punpcklwd	mm0,mm1

			    punpckhwd	mm2,mm1
			    movq		mm1,mm0

			    punpcklbw	mm2,mm2		; Reds
			    pcmpeqw		mm3,qword ptr zeros		   		; mm3 now has transparency mask for 4 texels

			    punpcklbw	mm0,mm0		; Blues
			    pmulhw		mm2,mm7			; Modulate reds with interpolated colour

			    punpckhbw	mm1,mm1		; Greens
			    pmulhw		mm0,mm5			; Modulate blues with interpolated colour

			    pmulhw		mm1,mm6			; Modulate greens with interpolated colour
			    D_PADDW		mm7,qword ptr WORK.d_r_x

	    ; Now have the four colours and transparency mask:
	    ;
	    ; mm0 = B3.B2.B1.B0
	    ; mm1 = G3.G2.G1.G0
	    ; mm2 = R3.R2.R1.R0
	    ; mm3 = M3.M2.M1.M0
	    ;
    	    ; Convert to RGB 565
		    ;
				mov			esi,WORK.mask_end
				mov			edi,WORK.screen_end

if DITHER
				paddw		mm2,qword ptr dither_table_9[ebx*8]
;V
			    paddw		mm0,qword ptr dither_table_9[ebx*8]
			    pand		mm2,qword ptr mask_5d

			    paddw		mm1,qword ptr dither_table_8[ebx*8]
			    pand		mm0,qword ptr mask_5d

if COLOUR_TYPE_15
			    pand		mm1,qword ptr mask_5d
endif
if COLOUR_TYPE_16
			    pand		mm1,qword ptr mask_6d
endif
if COLOUR_TYPE_15
			    psllw		mm2,1
endif
if COLOUR_TYPE_16
			    psllw		mm2,2
endif

			    D_PADDW		mm6,qword ptr WORK.d_g_x
  			    psrlw		mm0,9

if COLOUR_TYPE_15
			    psrlw		mm1,4
endif
if COLOUR_TYPE_16
			    psrlw		mm1,3
endif

			    pandn		mm3,[esi+eax]		; Merge transp with current mask
			    por			mm0,mm2
else
    	; Convert to RGB 565
		;
			    pand		mm2,qword ptr mask_5d

			    pand		mm0,qword ptr mask_5d
if COLOUR_TYPE_15
  			    psllw		mm2,1
endif
if COLOUR_TYPE_16
  			    psllw		mm2,2
endif

if COLOUR_TYPE_15
			    pand		mm1,qword ptr mask_5d
endif
if COLOUR_TYPE_16
			    pand		mm1,qword ptr mask_6d
endif
  			    psrlw		mm0,9

			    D_PADDW		mm6,qword ptr WORK.d_g_x
if COLOUR_TYPE_15
  			    psrlw		mm1,4
endif
if COLOUR_TYPE_16
  			    psrlw		mm1,3
endif

  			    pandn		mm3,[esi+eax]		; Merge transp with current mask
			    por			mm0,mm2
endif

			    movq		mm2,[edi+eax]		; Read old destination
			    por			mm0,mm1

			    pand		mm0,mm3
			    pandn		mm3,mm2

			    por			mm0,mm3
			    test		eax,eax

		; Step colour components on
		;
	  			D_PADDW		mm5,qword ptr WORK.d_b_x
;V
				movq		[edi+eax],mm0

				lea			eax,[eax+WORD_STEP]
				jne			c_loop_body


		        mov			ecx,WORK.h.counts

		        mov			eax,WORK.h.xm		; Load up edges for next loop
		        mov			ebx,WORK.h.x1

		        sub			ecx,10000h			; Decrement count (in hi word of dword)
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

		        pand		mm0,mm6			; Mask for length scanline

		        movq		mm5,mm0			; copy mask for early out test

		        pand		mm1,mm0			; Select new Z's using mask
		        pandn		mm0,mm7			; Select old Z's using mask

		        por			mm0,mm1			; Combine old and new Z's
		        movq		mm6,mm5			; copy mask for colour pass

		        movq		[esi+ebx*2],mm0	; Write to  Z buffer pixels
		        packsswb	mm5,mm5			; Merge mask down to 32 bits

		        movd		eax,mm5

		        test		eax,eax			; Early out of no pixels visible on scanline
		        je			next_line

	;; ONE WORD CASE
	;;
	;; mm6 = pixel mask

	    ; MMX Linear texture mapping, 4 pixels at a time, with transparency
	    ;
	    ;						2222 2222 1111 1111 1100 0000 0000
	    ;                       7654 3210 9876 5432 1098 7654 3210
	    ; ecx	U accumulator - UUUU UU00 0000 00UU.0uuu uuuu uuuu
	    ; ebp   V accumulator - 0000 00VV VVVV VV00.0vvv vvvv vvvv
	    ;
	    ; dl	texel index
	    ; esi	Texture
	    ; edi	Palette
	    ; eax	Loop control
	    ; ebx	Texel address
	    ;
			    mov			eax,ebx
			    xor			edx,edx

			    mov			ecx,WORK.u
			    mov			ebp,WORK.v

			    lea			ebx,[ebp+ecx] 			; Generate texture address
			    mov			esi,WORK.texture_address

			    shr			ebx,FRACTION_BITS+1		; Get significant part of merged U&V
			    mov			edi,WORK.map_address

			    add			ecx,WORK.d_u_x	   		; Update U&V for next texel
			    add			ebp,WORK.d_v_x

			    mov			dl,[esi+ebx]   	   		; Read texel
			    pcmpeqw		mm4,mm4					; mm4 = 0ffffffffh

			    and			ecx,WORK.u_mask	   		; Tidy up U accumulator
			    and			ebp,WORK.v_mask	 		; Tidy up V accumulator

			    FETCH_T0	[edi+edx*4]	   			; Read palette (8.8.8)
			    psrlw		mm4,9					; mm4 = 0007f007fh

			    lea			ebx,[ebp+ecx]	   		; Generate texture address
			    mov			byte ptr temp_mask+M0,dl

			    shr			ebx,FRACTION_BITS+1		; Get significant part of merged U&V
			    packuswb	mm4,mm4					; mm4 = 07f7f7f7fh	- Mask for ARGB

			    add			ecx,WORK.d_u_x	 		; Update U&V for next texel
			    add			ebp,WORK.d_v_x

			    mov			dl,[esi+ebx]   	   		; Read texel
		    nop

			    and			ecx,WORK.u_mask	   		; Tidy up U accumulator
			    and			ebp,WORK.v_mask	   		; Tidy up V accumulator

			    FETCH_T1	[edi+edx*4]	   			; Read palette (8.8.8) and merge texels 0 and 1 into same register
;V

			    lea			ebx,[ebp+ecx]	   		; Generate texture address
			    mov			byte ptr temp_mask+M1,dl

			    shr			ebx,FRACTION_BITS+1		; Get significant part of merged U&V

			    PACK_T01

			    add			ecx,WORK.d_u_x			; Update U&V for next texel
			    add			ebp,WORK.d_v_x

			    mov			dl,[esi+ebx]   	  		; Read texel
			    PSRLD_T01

			    and			ecx,WORK.u_mask	   		; Tidy up U accumulator
			    and			ebp,WORK.v_mask	   		; Tidy up V accumulator

			    FETCH_T2	[edi+edx*4]	  			; Read palette (8.8.8)
			    PAND_T01

			    lea			ebx,[ebp+ecx]	   		; Generate texture address
			    mov			byte ptr temp_mask+M2,dl

			    shr			ebx,FRACTION_BITS+1		; Get significant part of merged U&V
		    nop

			    mov			dl,[esi+ebx]   	  		; Read texel
if DITHER
			    mov			ebx,WORK.h.start_scanline
endif

			    FETCH_T3	[edi+edx*4]	   			; Read palette (8.8.8)
			    mov			byte ptr temp_mask+M3,dl

			    mov			edi,WORK.h.screen_address
			    PACK_T23

			    PSRLD_T23
			    movd		mm3,temp_mask	   		; Get the 4 texel indices

			    PAND_T23
			    punpcklbw	mm3,mm3			   		; Convert bytes to words

			    movq		mm2,mm0
			    punpcklwd	mm0,mm1

			    punpckhwd	mm2,mm1
			    movq		mm1,mm0

			    pcmpeqw		mm3,qword ptr zeros		   		; mm3 now has transparency mask for 4 texels
			    punpcklbw	mm2,mm2		; Reds

			    pmulhw		mm2,qword ptr WORK.r01			; Modulate reds with interpolated colour
			    punpcklbw	mm0,mm0		; Blues

			    pmulhw		mm0,qword ptr WORK.b01			; Modulate blues with interpolated colour
			    punpckhbw	mm1,mm1		; Greens

			    pmulhw		mm1,qword ptr WORK.g01			; Modulate greens with interpolated colour

	    ; Now have the four colours and transparency mask:
	    ;
	    ; mm0 = B3.B2.B1.B0
	    ; mm1 = G3.G2.G1.G0
	    ; mm2 = R3.R2.R1.R0
	    ; mm3 = M3.M2.M1.M0
	    ;
    	    ; Convert to RGB 565
		    ;

if DITHER
				paddw		mm2,qword ptr dither_table_9[ebx*8]
;V
			    paddw		mm0,qword ptr dither_table_9[ebx*8]
			    pand		mm2,qword ptr mask_5d

			    paddw		mm1,qword ptr dither_table_8[ebx*8]
			    pand		mm0,qword ptr mask_5d

if COLOUR_TYPE_15
			    pand		mm1,qword ptr mask_5d
endif
if COLOUR_TYPE_16
			    pand		mm1,qword ptr mask_6d
endif
if COLOUR_TYPE_15
			    psllw		mm2,1
endif
if COLOUR_TYPE_16
			    psllw		mm2,2
endif

  			    psrlw		mm0,9

else
    	; Convert to RGB 565
		;
			    pand		mm2,qword ptr mask_5d

			    pand		mm0,qword ptr mask_5d
if COLOUR_TYPE_15
  			    psllw		mm2,1
endif
if COLOUR_TYPE_16
  			    psllw		mm2,2
endif

if COLOUR_TYPE_15
			    pand		mm1,qword ptr mask_5d
endif
if COLOUR_TYPE_16
			    pand		mm1,qword ptr mask_6d
endif
  			    psrlw		mm0,9

endif
			    lea			edi,[edi+eax*2]
if COLOUR_TYPE_15
  			    psrlw		mm1,4
endif
if COLOUR_TYPE_16
  			    psrlw		mm1,3
endif

  			    pandn		mm3,mm6			; Merge transp with current mask
			    por			mm0,mm2

			    movq		mm2,[edi]		; Read old destination
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

