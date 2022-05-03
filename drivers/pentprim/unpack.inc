; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;
; $Id: unpack.inc 1.1 1997/12/10 16:49:19 jon Exp $
; $Locker: $
;
;
; Macros to unpack setup information into workspace
;

UNPACK_PARAM_16 macro param_s, param_dx, work_p0, work_d_p_y1, work_d_p_y0, work_d_p_x

        movq        mm0,qword ptr param_s
;V
        movq        mm4,qword ptr param_dx
        movq        mm2,mm0

        movq        mm1,mm4			; mm1 = (d_z_y0, d_z_x)
        punpckldq   mm0,mm0         ; mm0 = (s_z, s_z)

        movq        mm3,mm4         ; mm3 = (d_z_y0, d_z_x)
        punpckldq   mm1,mm1         ; mm1 = (d_z_x, d_z_x)

        movq        mm5,mm1         ; mm5 = (d_z_x, d_z_x)
        psllq       mm4,32          ; mm4 = (d_z_x, 0)

        paddd       mm1,mm1         ; mm1 = (d_z_x*2, d_z_x*2)
        paddd       mm0,mm4			; mm0 = (sz+d_z_x , sz)

        paddd       mm1,mm0			; mm1 = (s_z+3*d_z_x , s_z+2*d_z_x)
        pslld       mm5,2           ; mm5 = (d_z_x*4, d_z_x*4)

        movq        qword ptr work_p0,mm0
        punpckhdq   mm2,mm2         ; replicate d_y1 into low dword

        movq        qword ptr work_p0+8,mm1
        punpckhdq   mm3,mm3         ; replicate d_y0 into low dword

        movq        qword ptr work_d_p_x,mm5
;V
        movq        qword ptr work_d_p_y1,mm2
;V
        movq        qword ptr work_d_p_y0,mm3
;V

    endm


UNPACK_PARAM_8  macro param_s, param_dx, work_p0, work_d_p_y1, work_d_p_y0, work_d_p_x

        movq        mm0,param_s
;V
        movq        mm4,param_dx
        movq        mm2,mm0

        movq        mm1,mm4			; mm1 = (d_p_y0, d_p_x)
        punpckldq   mm0,mm0         ; mm0 = (s_z, s_z)

        movq        mm3,mm4         ; mm3 = (d_p_y0, d_p_x)
        punpckldq   mm1,mm1         ; mm1 = (d_p_x, d_p_x)

        movq        mm5,mm1         ; mm5 = (d_p_x, d_p_x)
        psllq       mm4,32          ; mm4 = (d_p_x, 0)

        paddd       mm1,mm1         ; mm1 = (d_p_x*2, d_p_x*2)
        punpckhdq   mm2,mm2         ; replicate d_y1 into low dword

        paddd       mm0,mm4			; mm0 = (sz+d_p_x , sz)
        punpckhdq   mm3,mm3         ; replicate d_y0 into low dword

        paddd       mm1,mm0			; mm1 = (s_z+3*d_p_x , s_z+2*d_p_x)
        psrad       mm0,16

        psrad       mm1,16
        paddd       mm5,mm5         ; mm5 = (d_p_x*2, d_p_x*2)

        packssdw    mm0,mm1
        paddd       mm5,mm5         ; mm5 = (d_p_x*4, d_p_x*4)

        psrad       mm5,16
;V

        movq        work_p0,mm0
        packssdw    mm5,mm5

        psrad       mm2,16
;V

        movq        work_d_p_x,mm5
        packssdw    mm2,mm2

        psrad       mm3,16
;V

        packssdw    mm3,mm3
;V

        movq        work_d_p_y1,mm2
;V

        movq        work_d_p_y0,mm3
;V
    endm

UNPACK_PARAM_RGB  macro

	; Red
	;
        movq        mm0,qword ptr PARAM.s_r
;V
        movq        mm6,qword ptr PARAM.d_r_x
        movq        mm4,mm0

        movq        mm3,mm6			; mm3 = (d_r_y0, d_r_x)
        punpckldq   mm0,mm0         ; mm0 = (s_z, s_z)

        movq        mm5,mm6         ; mm5 = (d_r_y0, d_r_x)
        punpckldq   mm3,mm3         ; mm3 = (d_r_x, d_r_x)

        movq        mm7,mm3         ; mm7 = (d_r_x, d_r_x)
        psllq       mm6,32          ; mm6 = (d_r_x, 0)

        paddd       mm3,mm3         ; mm3 = (d_r_x*2, d_r_x*2)
        punpckhdq   mm4,mm4         ; replicate d_y1 into low dword

        paddd       mm0,mm6			; mm0 = (sz+d_r_x , sz)
        punpckhdq   mm5,mm5         ; replicate d_y0 into low dword

        paddd       mm3,mm0			; mm3 = (s_z+3*d_r_x , s_z+2*d_r_x)
        psrad       mm0,16

        psrad       mm3,16
        paddd       mm7,mm7         ; mm7 = (d_r_x*2, d_r_x*2)

        packssdw    mm0,mm3
        paddd       mm7,mm7         ; mm7 = (d_r_x*4, d_r_x*4)

        movq        mm1,qword ptr PARAM.s_g	; Start setting up green
        psrad       mm7,16

        movq        qword ptr WORK.r01,mm0
        packssdw    mm7,mm7

        movq        mm6,qword ptr PARAM.d_g_x
        psrad       mm4,16

        movq        qword ptr WORK.d_r_x,mm7
        packssdw    mm4,mm4

        psrad       mm5,16
        movq        mm3,mm6			; mm3 = (d_g_y0, d_g_x)

        packssdw    mm5,mm5
;V
        movq        qword ptr WORK.d_r_y1,mm4
        movq        mm4,mm1

        movq        qword ptr WORK.d_r_y0,mm5
        punpckldq   mm1,mm1         ; mm1 = (s_z, s_z)

	; Green
	;
        movq        mm5,mm6         ; mm5 = (d_g_y0, d_g_x)
        punpckldq   mm3,mm3         ; mm3 = (d_g_x, d_g_x)

        movq        mm7,mm3         ; mm7 = (d_g_x, d_g_x)
        psllq       mm6,32          ; mm6 = (d_g_x, 0)

        paddd       mm3,mm3         ; mm3 = (d_g_x*2, d_g_x*2)
        punpckhdq   mm4,mm4         ; replicate d_y1 into low dword

        paddd       mm1,mm6			; mm1 = (sz+d_g_x , sz)
        punpckhdq   mm5,mm5         ; replicate d_y0 into low dword

        paddd       mm3,mm1			; mm3 = (s_z+3*d_g_x , s_z+2*d_g_x)
        psrad       mm1,16

        psrad       mm3,16
        paddd       mm7,mm7         ; mm7 = (d_g_x*2, d_g_x*2)

        packssdw    mm1,mm3
        paddd       mm7,mm7         ; mm7 = (d_g_x*4, d_g_x*4)

        movq        mm2,qword ptr PARAM.s_b	; Start setting up blue
        psrad       mm7,16

        movq        qword ptr WORK.g01,mm1
        packssdw    mm7,mm7

        movq        mm6,qword ptr PARAM.d_b_x
        psrad       mm4,16

        movq        qword ptr WORK.d_g_x,mm7
        packssdw    mm4,mm4

        psrad       mm5,16
        movq        mm3,mm6			; mm3 = (d_b_y0, d_b_x)

        packssdw    mm5,mm5
;V

        movq        qword ptr WORK.d_g_y1,mm4
        movq        mm4,mm2

        movq        qword ptr WORK.d_g_y0,mm5
        punpckldq   mm2,mm2         ; mm2 = (s_z, s_z)

	; Blue
	;
        movq        mm5,mm6         ; mm5 = (d_b_y0, d_b_x)
        punpckldq   mm3,mm3         ; mm3 = (d_b_x, d_b_x)

        movq        mm7,mm3         ; mm7 = (d_b_x, d_b_x)
        psllq       mm6,32          ; mm6 = (d_b_x, 0)

        paddd       mm3,mm3         ; mm3 = (d_b_x*2, d_b_x*2)
        punpckhdq   mm4,mm4         ; replicate d_y1 into low dword

        paddd       mm2,mm6			; mm2 = (sz+d_b_x , sz)
        punpckhdq   mm5,mm5         ; replicate d_y0 into low dword

        paddd       mm3,mm2			; mm3 = (s_z+3*d_b_x , s_z+2*d_b_x)
        psrad       mm2,16

        psrad       mm3,16
        paddd       mm7,mm7         ; mm7 = (d_b_x*2, d_b_x*2)

        packssdw    mm2,mm3
        paddd       mm7,mm7         ; mm7 = (d_b_x*4, d_b_x*4)

        psrad       mm7,16
		 movq		mm3,mm0			; make copy of reds

        movq        qword ptr WORK.b01,mm2
        packssdw    mm7,mm7

        psrad       mm4,16
;V
        movq        qword ptr WORK.d_b_x,mm7
        packssdw    mm4,mm4

		 movq		mm7,qword ptr mask_6
        psrad       mm5,16

        packssdw    mm5,mm5
;V
		
        movq        qword ptr WORK.d_b_y1,mm4
		 movq		mm4,mm1			; make copy of greens

        movq        qword ptr WORK.d_b_y0,mm5
		 movq		mm5,mm2			; make copy of blues
    endm

	; Unpack constant colour so that it can be used in inner loop
	;
	; Optionally, move alpha to bottom of colour word (16 levels
	;
UNPACK_CONSTANT_COLOUR macro	screendoor:=<0>

		movd		mm0,WORK.h._c	;	0000ARGB

if screendoor
		mov			eax,WORK.h._c
endif
		punpcklbw	mm0,mm0         ;	AARRGGBB

if screendoor
		shr			eax,26
endif
		psrlw		mm0,1			; Reduce to 7 bits (multiply is signed :-( )

		movq		mm2,mm0			;	AARRGGBB
		punpcklwd	mm0,mm0         ;	GGGGBBBB

		movq		mm1,mm0			;	GGGGBBBB
		punpckhwd	mm2,mm2			;	AAAARRRR

if screendoor
		and			eax,03ch
endif
		punpckldq	mm0,mm0			;	BBBBBBBB

if screendoor
		mov			WORK.h._c,eax
endif
		punpckhdq	mm1,mm1         ;	GGGGGGGG

		movq		qword ptr WORK.cb01,mm0
		punpckldq	mm2,mm2			;	RRRRRRRR

		movq		qword ptr WORK.cg01,mm1

		movq		qword ptr WORK.cr01,mm2

		endm

UNPACK_CONSTANT_COLOUR_BLEND macro

		movd		mm0,WORK.h._c	;	0000ARGB
; V
		punpcklbw	mm0,mm0         ;	AARRGGBB
; UV

		psrlw		mm0,1			; Reduce to 7 bits (multiply is signed :-( )
; UV

		movq		mm2,mm0			;	AARRGGBB
		punpcklwd	mm0,mm0         ;	GGGGBBBB

		movq		mm1,mm0			;	GGGGBBBB
		punpckhwd	mm2,mm2			;	AAAARRRR

		movq		mm3,mm2
; UV

		punpckhdq	mm3,mm3			;	AAAAAAAA
		pcmpeqw		mm4,mm4			;	mm4 = 0ffffffffffffffffh

		punpckldq	mm0,mm0			;	BBBBBBBB
		psrlw		mm4,1			;	mm4 = 07fff7fff7fff7fffh

		punpckhdq	mm1,mm1         ;	GGGGGGGG
		pmulhw		mm0,mm3

		punpckldq	mm2,mm2			;	RRRRRRRR
		pmulhw		mm1,mm3

		pmulhw		mm2,mm3
		pxor		mm3,mm4			;	mm3 = 1-alpha

		psllw		mm0,1
; UV

		psllw		mm1,1
		movq		qword ptr WORK.alpha01,mm3

		psllw		mm2,1
		movq		qword ptr WORK.cb01,mm0

		movq		qword ptr WORK.cg01,mm1
; UV

		movq		qword ptr WORK.cr01,mm2
; UV
		endm

;	Unpack U&V (with optional tiling), to use 32 bit accumulators
;
UNPACK_UV_32	macro

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

	;; U
	;;
		movq 		mm0,qword ptr PARAM.s_u		; Load start and dy1
		 pandn		mm7,mm4						; Clear low bits of V mask

		movd		mm4,eax				; Get tile shift
		 pandn		mm6,mm3						; Clear low bits of U mask

		movq		mm2,mm0				; make copies of values
		psrad		mm0,1				; align fraction bits

		movq 		mm1,qword ptr PARAM.d_u_x     ; Load dx and dy0
		pslld		mm2,mm4				; shift integer part up

		pand		mm0,qword ptr fraction_mask	; Mask of fraction bits
		pand		mm2,mm6				; Mask out integer bits

		movq		mm3,mm1				;  ditto
		psrad		mm1,1

		pslld		mm3,mm4
		por			mm0,mm2				; Merge integer and fraction

		pand		mm1,qword ptr fraction_mask
		pand		mm3,mm6

		movq		mm2,qword ptr fraction_bit
		por			mm1,mm3

		por			mm2,mm5				; Build a mask of the carry bridge bits
		sub			ebx,eax					; Adjust V shift

		por			mm0,mm2
		por			mm1,mm2

		movd		mm4,ebx					; Load V shift

		movq 		qword ptr WORK.u,mm0			; Save out U params

		movq 		qword ptr WORK.d_u_x,mm1


	;; V
	;;
		movq 		mm0,qword ptr PARAM.s_v

		movq		mm2,mm0
		psrad		mm0,1

		movq 		mm1,qword ptr PARAM.d_v_x
		pslld		mm2,mm4

		pand		mm0,qword ptr fraction_mask
		pand		mm2,mm7

		movq		mm3,mm1
		psrad		mm1,1

		pslld		mm3,mm4
		por			mm0,mm2

		pand		mm1,qword ptr fraction_mask
		pand		mm3,mm7

		movq 		mm2,qword ptr PARAM.s_v
		por			mm1,mm3

		movq 		mm3,qword ptr PARAM.d_v_x
   		 por		mm7,mm5				; Start building masks

		pand		mm2,mm5
		pand		mm3,mm5

		por			mm0,mm2
		por			mm1,mm3

	; Set bridging bits in deltas
	;
		movq		mm2,qword ptr fraction_bit
		por			mm2,mm6

		por			mm0,mm2
		por			mm1,mm2

		punpckldq	mm6,mm7				; Duplicate masks

		movq 		qword ptr WORK.v,mm0

		movq 		qword ptr WORK.d_v_x,mm1

		por			mm6,qword ptr fraction_mask	; Add fraction to masks

		movq		qword ptr WORK.u_mask,mm6

	; Mask start values
	;
		mov			eax,WORK.u
		mov			ebx,WORK.v

		and			eax,WORK.u_mask
		and			ebx,WORK.v_mask

		mov			WORK.u,eax
		mov			WORK.v,ebx
endm

UNPACK_SCREENDOOR_ALPHA macro
	; Move alpha to bottom of colour word
	;
		mov			eax,WORK.h._c
		shr			eax,28
		shl			eax,2
		mov			WORK.h._c,eax
		endm

