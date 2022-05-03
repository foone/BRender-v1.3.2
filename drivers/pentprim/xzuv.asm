; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;
; $Id: xzuv.asm 1.1 1997/12/10 16:49:29 jon Exp $
; $Locker: $
;
; MMX Rasteriser - Texture
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
		include xzuv1.inc
endif

;; Depth Buffered Textured
;;


if PARTS and PART_16Z

COLOUR_TYPE_15=0
COLOUR_TYPE_16=1

DITHER = 0
SCREENDOOR = 0
BLEND=0
RASTERISE_PAIR <work_mmx_zuv4c>, <param_zuv>, <xzuv1.inc>,RasteriseMMX565_Z_UV

DITHER = 1
SCREENDOOR = 0
BLEND=0
RASTERISE_PAIR <work_mmx_zuv4c>, <param_zuv>, <xzuv1.inc>,RasteriseMMX565_Z_UV_D

DITHER = 0
SCREENDOOR = 1
BLEND=0
RASTERISE_PAIR <work_mmx_zuv4c>, <param_zuv>, <xzuv1.inc>,RasteriseMMX565_Z_UV_S

DITHER = 1
SCREENDOOR = 1
BLEND=0
RASTERISE_PAIR <work_mmx_zuv4c>, <param_zuv>, <xzuv1.inc>,RasteriseMMX565_Z_UV_SD

endif

		end


