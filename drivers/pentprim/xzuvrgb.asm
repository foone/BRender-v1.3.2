; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;
; $Id: xzuvrgb.asm 1.1 1997/12/10 16:49:40 jon Exp $
; $Locker: $
;
; MMX Rasteriser - Texture * Interpolated colour
;
; All rasterisers are entered with esp pointing at the triangle parameter block
; and exit by popping setup block from stack and jumping to next function
;
		.586
		.MMX
		.model	flat,c

;		include iammx.inc
		include drv.inc
		include unpack.inc

		include rastmacs.inc

		.code
if 0
	; Let mkdep generate a dependency
	;
		include xzuvrgb.inc
endif

if PARTS and PART_16Z
COLOUR_TYPE_15=0
COLOUR_TYPE_16=1
;; Depth Buffered Textured, Constant colour modulate
;;
DITHER = 0
SCREENDOOR = 0
BLEND=0
RASTERISE_PAIR <work_mmx_zrgbuv>, <param_zrgbuv>, <xzuvrgb.inc>,RasteriseMMX565_Z_UV_RGB

DITHER = 1
SCREENDOOR = 0
BLEND=0
RASTERISE_PAIR <work_mmx_zrgbuv>, <param_zrgbuv>, <xzuvrgb.inc>,RasteriseMMX565_Z_UV_RGB_D

if 1
DITHER = 0
SCREENDOOR = 1
BLEND=0
RASTERISE_PAIR <work_mmx_zrgbuv>, <param_zrgbuv>, <xzuvrgb.inc>,RasteriseMMX565_Z_UV_RGB_S

DITHER = 1
SCREENDOOR = 1
BLEND=0
RASTERISE_PAIR <work_mmx_zrgbuv>, <param_zrgbuv>, <xzuvrgb.inc>,RasteriseMMX565_Z_UV_RGB_SD
endif
endif

		end

