; {{NO_DEPENDENCIES}}
; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;
; $Id: rastmacs.inc 1.1 1997/12/10 16:48:03 jon Exp $
; $Locker: $
;
; Macros for generating rasterisers
;

if 1
OUT_EXPR	macro _name,_total
		%out _name = _total
		endm
else
OUT_EXPR		macro x,y
		endm
endif

RASTERISE_EXIT	macro
		mov		eax,[esp+WORK_SIZE].tsb_header.function
		add		esp,WORK_SIZE

		jmp		eax

		endm

RASTERISE_EXIT_EARLY macro
		mov		eax,[esp+PARAM_SIZE].tsb_header.function
		add		esp,PARAM_SIZE
		jmp		eax
		endm

; Number of bits of fraction used for U & V in tiled texture mappers
;
FRACTION_BITS equ 11

; Differentiation between 15 and 16 bit rasterisers
;COLOUR_TYPE_15 equ 0
;COLOUR_TYPE_16 equ 1

; Generate a pair of rasterisers - general case
;
RASTERISE_PAIR  macro swork,sparam,inc,basename
	local _w,_p

PARAM_OFFSET = swork&.h
WORK_SIZE = sizeof swork
PARAM_SIZE = sizeof sparam

	WORK	equ [esp].&swork
	PARAM	equ [esp+PARAM_OFFSET].&sparam

basename&_LR proc
direction textequ <lr>
		include inc
		OUT_EXPR	<basename&_LR>,<%($-basename&_LR)>
basename&_LR endp


basename&_RL proc

direction textequ <rl>
		include inc
		OUT_EXPR	<basename&_RL>,<%($-basename&_RL)>
basename&_RL endp
	endm

		; Convenience macros for accessing components
		;
_x				textequ	<comp[C_SX*4]>
_y 				textequ	<comp[C_SY*4]>
_z 				textequ	<comp[C_SZ*4]>

_r				textequ	<comp[C_R*4]>
_g				textequ	<comp[C_G*4]>
_b				textequ	<comp[C_B*4]>
_a				textequ	<comp[C_A*4]>

_u				textequ	<comp[C_U*4]>
_v				textequ	<comp[C_V*4]>

		; Tables and data for rasterisers
		;
				externdef zeros:dword

				externdef mask_5:dword
				externdef mask_6:dword

				externdef mask_5s:dword
				externdef mask_6s:dword

				externdef mask_5d:dword
				externdef mask_6d:dword

				externdef right_masks:dword
				externdef left_masks:dword

				externdef scanline_mask:word
				externdef scanline_mask_end:word

				externdef rasteriseBuffer:qword
				externdef rasteriseBufferLast:dword
				externdef rasteriseBufferTop:dword
				externdef rasteriseBufferSaved:dword
				externdef dither_table_11:dword
				externdef dither_table_11x:dword
				externdef dither_table_10:dword
				externdef dither_table_9:dword
				externdef dither_table_8:dword
				externdef fraction_mask:dword
				externdef fraction_bit:dword
				externdef uv_masks:dword
				externdef screendoor_masks:dword

				externdef dither_current:dword
				externdef screendoor_mask:dword

