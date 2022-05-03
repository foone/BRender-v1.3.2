; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;
; $Id: rastrise.asm 1.1 1997/12/10 16:48:06 jon Exp $
; $Locker: $
;
; Rasteriser buffer system
;
; All rasterisers are entered with esp pointing at the triangle parameter block
; and exit by popping setup block from stack and jumping to next function
;
; XXX should rearrange this so that stack is built bottom to top, and then flushed -
;     means that prims are rasterised in order
;
;
		.586
		.MMX
		.model	flat,c

;		include iammx.inc
		include drv.inc
		include unpack.inc

		include rastmacs.inc

		.code

		.data

zeros				dword	000000000h,000000000h
mask_5				dword	0f800f800h,0f800f800h
mask_6				dword	0fc00fc00h,0fc00fc00h

mask_5s				dword	07c007c00h,07c007c00h
mask_6s				dword	07e007e00h,07e007e00h

mask_5d				dword	03e003e00h,03e003e00h
mask_6d				dword	03f003f00h,03f003f00h

mask_argb 			dword	07f7f7f7fh,07f7f7f7fh

right_masks label dword
					word	0ffffh,00000h,00000h,00000h
					word	0ffffh,0ffffh,00000h,00000h
					word	0ffffh,0ffffh,0ffffh,00000h
left_masks label dword
					word	0ffffh,0ffffh,0ffffh,0ffffh
					word	00000h,0ffffh,0ffffh,0ffffh
					word	00000h,00000h,0ffffh,0ffffh
					word	00000h,00000h,00000h,0ffffh

	; Workspace for scanline masks
	;
scanline_mask		word	8192 dup(?)
scanline_mask_end	label word

;; Useful static data for the rasterisers
;;
; Dither Tables
;
dither_current 		dword	0,0

	; 4x4 magic square for dithered 5.11
	;
dither_table_11	label dword
					word 0007fh,0037fh,004ffh,007ffh
					word 005ffh,006ffh,0017fh,0027fh
					word 003ffh,000ffh,0077fh,0047fh
					word 0067fh,0057fh,002ffh,001ffh

	; Same, but with columns 1 & 2 swapped
	;
dither_table_11x label dword
					word 0007fh,004ffh,0037fh,007ffh
					word 005ffh,0017fh,006ffh,0027fh
					word 003ffh,0077fh,000ffh,0047fh
					word 0067fh,002ffh,0057fh,001ffh

	; 4x4 magic square for dithered 6.10
	;
dither_table_10	label dword
					word 0003fh,001bfh,0027fh,003ffh
					word 002ffh,0037fh,000bfh,0013fh
					word 001ffh,0007fh,003bfh,0023fh
					word 0033fh,002bfh,0017fh,000ffh

	; 4x4 magic square for dithered 7.9
	;
dither_table_9 	label dword
					word 0001fh,000dfh,0013fh,001ffh
					word 0017fh,001bfh,0005fh,0009fh
					word 000ffh,0003fh,001dfh,0011fh
					word 0019fh,0015fh,000bfh,0007fh

	; 4x4 magic square for dithered 8.8
	;
dither_table_8 	label dword
					word 0000fh,0006fh,0009fh,000ffh
					word 000bfh,000dfh,0002fh,0004fh
					word 0007fh,0001fh,000efh,0008fh
					word 000cfh,000afh,0005fh,0003fh

; Texture mapping Tables
;
fraction_mask		dword	(1 shl FRACTION_BITS)-1,(1 shl FRACTION_BITS)-1
fraction_bit		dword	(1 shl FRACTION_BITS),(1 shl FRACTION_BITS)


uv_masks	label dword
			x = 0
		rept 21
					dword	((1 shl x)-1) shl (FRACTION_BITS+1), ((1 shl x)-1) shl (FRACTION_BITS+1)
			x = x+1
		endm

; Screendoor transparency tables
;
screendoor_mask		dword	0,0

screendoor_masks label dword

; Level 0
		word 00000h,00000h,00000h,00000h
		word 00000h,00000h,00000h,00000h
		word 00000h,00000h,00000h,00000h
		word 00000h,00000h,00000h,00000h
; Level 1
		word 0FFFFh,00000h,00000h,00000h
		word 00000h,00000h,00000h,00000h
		word 00000h,00000h,00000h,00000h
		word 00000h,00000h,00000h,00000h
; Level 2
		word 0FFFFh,00000h,00000h,00000h
		word 00000h,00000h,00000h,00000h
		word 00000h,00000h,0FFFFh,00000h
		word 00000h,00000h,00000h,00000h
; Level 3
		word 0FFFFh,00000h,0FFFFh,00000h
		word 00000h,00000h,00000h,00000h
		word 00000h,00000h,0FFFFh,00000h
		word 00000h,00000h,00000h,00000h
; Level 4
		word 0FFFFh,00000h,0FFFFh,00000h
		word 00000h,00000h,00000h,00000h
		word 0FFFFh,00000h,0FFFFh,00000h
		word 00000h,00000h,00000h,00000h
; Level 5
		word 0FFFFh,00000h,0FFFFh,00000h
		word 00000h,0FFFFh,00000h,00000h
		word 0FFFFh,00000h,0FFFFh,00000h
		word 00000h,00000h,00000h,00000h
; Level 6
		word 0FFFFh,00000h,0FFFFh,00000h
		word 00000h,0FFFFh,00000h,00000h
		word 0FFFFh,00000h,0FFFFh,00000h
		word 00000h,00000h,00000h,0FFFFh
; Level 7
		word 0FFFFh,00000h,0FFFFh,00000h
		word 00000h,0FFFFh,00000h,0FFFFh
		word 0FFFFh,00000h,0FFFFh,00000h
		word 00000h,00000h,00000h,0FFFFh
; Level 8
		word 0FFFFh,00000h,0FFFFh,00000h
		word 00000h,0FFFFh,00000h,0FFFFh
		word 0FFFFh,00000h,0FFFFh,00000h
		word 00000h,0FFFFh,00000h,0FFFFh
; Level 9
		word 0FFFFh,0FFFFh,0FFFFh,00000h
		word 00000h,0FFFFh,00000h,0FFFFh
		word 0FFFFh,00000h,0FFFFh,00000h
		word 00000h,0FFFFh,00000h,0FFFFh
; Level 10
		word 0FFFFh,0FFFFh,0FFFFh,00000h
		word 00000h,0FFFFh,00000h,0FFFFh
		word 0FFFFh,00000h,0FFFFh,0FFFFh
		word 00000h,0FFFFh,00000h,0FFFFh
; Level 11
		word 0FFFFh,0FFFFh,0FFFFh,0FFFFh
		word 00000h,0FFFFh,00000h,0FFFFh
		word 0FFFFh,00000h,0FFFFh,0FFFFh
		word 00000h,0FFFFh,00000h,0FFFFh
; Level 12
		word 0FFFFh,0FFFFh,0FFFFh,0FFFFh
		word 00000h,0FFFFh,00000h,0FFFFh
		word 0FFFFh,0FFFFh,0FFFFh,0FFFFh
		word 00000h,0FFFFh,00000h,0FFFFh
; Level 13
		word 0FFFFh,0FFFFh,0FFFFh,0FFFFh
		word 0FFFFh,0FFFFh,00000h,0FFFFh
		word 0FFFFh,0FFFFh,0FFFFh,0FFFFh
		word 00000h,0FFFFh,00000h,0FFFFh
; Level 14
		word 0FFFFh,0FFFFh,0FFFFh,0FFFFh
		word 0FFFFh,0FFFFh,00000h,0FFFFh
		word 0FFFFh,0FFFFh,0FFFFh,0FFFFh
		word 00000h,0FFFFh,0FFFFh,0FFFFh
; Level 15
		word 0FFFFh,0FFFFh,0FFFFh,0FFFFh
		word 0FFFFh,0FFFFh,0FFFFh,0FFFFh
		word 0FFFFh,0FFFFh,0FFFFh,0FFFFh
		word 00000h,0FFFFh,0FFFFh,0FFFFh

		end

