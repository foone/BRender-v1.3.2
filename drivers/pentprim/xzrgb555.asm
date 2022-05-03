; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;
; $Id: xzrgb555.asm 1.1 1997/12/10 16:49:26 jon Exp $
; $Locker: $
;
; MMX Rasterisers - Interpolated colour
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
		include xzrgb.inc
endif

;; Depth Buffered, interpolated colour
;;

if PARTS and PART_15Z

COLOUR_TYPE_15 = 1
COLOUR_TYPE_16 = 0

DITHER = 0
SCREENDOOR = 0
BLEND=0
RASTERISE_PAIR <work_mmx_zrgb>, <param_zrgb>, <xzrgb.inc>,RasteriseMMX555_Z_RGB

DITHER = 1
SCREENDOOR = 0
BLEND=0
RASTERISE_PAIR <work_mmx_zrgb>, <param_zrgb>, <xzrgb.inc>,RasteriseMMX555_Z_RGB_D

DITHER = 0
SCREENDOOR = 1
BLEND=0
RASTERISE_PAIR <work_mmx_zrgb>, <param_zrgb>, <xzrgb.inc>,RasteriseMMX555_Z_RGB_S

DITHER = 1
SCREENDOOR = 1
BLEND=0
RASTERISE_PAIR <work_mmx_zrgb>, <param_zrgb>, <xzrgb.inc>,RasteriseMMX555_Z_RGB_SD

endif

		end

