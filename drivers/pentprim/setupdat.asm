; Copyright (c) 1992,1993-1996 Argonaut Technologies Limited. All rights reserved.
;
; $Id: setupdat.asm 1.1 1997/12/10 16:48:14 jon Exp $
; $Locker: $
;
; Common data for setup code
;
		.586p
		.model	flat,c

 		include setupdat.inc

		.data

	; Convertion values for major and minor edges, indexed by LR/RL flag*8
	;
		; Fixed
		;
xconv_d16_12	qword	04338000000000000h	; Conv + 0.0
xconv_d16_m		qword	04338000000010000h 	; Conv + 1.0
				qword	04338000000000000h	; Conv + 0.0

		; Float
		;
fconv_d16_12	qword	04238000000000000h	; Conv + 0.0
fconv_d16_m		qword	04238000000010000h 	; Conv + 1.0
				qword	04238000000000000h	; Conv + 0.0

fp_one			dword	1.0
fp_one_fixed	dword	65536.0
fp_rone_fixed	dword	0.00001525878906

fp_conv_dm4		dword	(127+52+4) shl 23 + (1 shl 22)
fp_conv_d		dword	(127+52-0) shl 23 + (1 shl 22)
fp_conv_d8		dword	(127+52-8) shl 23 + (1 shl 22)
fp_conv_d12		dword	(127+52-12) shl 23 + (1 shl 22)
fp_conv_d16		dword	(127+52-16) shl 23 + (1 shl 22)
fp_conv_d24		dword	(127+52-24) shl 23 + (1 shl 22)

fp_conv_sm16	dword	(127+23+16) shl 23 + (1 shl 22)
fp_conv_sm8		dword	(127+23+8) shl 23 + (1 shl 22)
fp_conv_s		dword	(127+23-0) shl 23 + (1 shl 22)
fp_conv_s8		dword	(127+23-8) shl 23 + (1 shl 22)

    ; Converts results of comparisons to sort order for vertices
    ;
sort_table_1    dword   1*4
                dword   2*4
                dword   0*4
sort_table_0    dword   0*4
                dword   0*4
                dword   0*4
sort_table_2    dword   2*4
                dword   1*4
                dword   0*4
                dword   1*4
                dword   2*4
                dword   0*4
                dword   0*4
                dword   0*4

	; Parity table of 3 bit number
	;
flip_table		dword	000000000h		;000
				dword	080000000h		;001
				dword	080000000h		;010
				dword	000000000h		;011
				dword	080000000h		;100
				dword	000000000h		;101
				dword	000000000h		;110
				dword	080000000h		;111

		end

