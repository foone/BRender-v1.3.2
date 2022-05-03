;; Copyright (c) 1991,1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: fnt3x5.asm 1.1 1997/12/10 16:41:23 jon Exp $
;; $Locker: $
;;
	.386p
	.model flat

	.data
		db '$Id: fnt3x5.asm 1.1 1997/12/10 16:41:23 jon Exp $',0
		align 4
	.data
	public __FontFixed3x5

__FontFixed3x5 label dword
	dd	0			; flags
	dw	3			; glyph_x
	dw	5			; glyph_y
	dw	4			; spacing_x
	dw	6			; spacing_y
	dd	0			; widths
	dd	_FontF3x5_Encoding	; enconding
	dd	_FontF3x5_Glyphs	; glyphs


GLYPH	macro name
	dw	glyph_&name&-_FontF3x5_Glyphs
	endm

	; Encoding table
	;
_FontF3x5_Encoding label byte

		GLYPH space	; 00 NUL
		GLYPH space	; 01 
		GLYPH space	; 02 
		GLYPH space	; 03 
		GLYPH space	; 04 
		GLYPH space	; 05 
		GLYPH space	; 06 
		GLYPH bullet	; 07 
		GLYPH space	; 08 
		GLYPH space	; 09
		GLYPH space	; 0A
		GLYPH space	; 0B 
		GLYPH space	; 0C 
		GLYPH space	; 0D
		GLYPH space	; 0E 
		GLYPH space	; 0F 
		GLYPH space	; 10 
		GLYPH space	; 11 
		GLYPH space	; 12 
		GLYPH space	; 13 
		GLYPH space	; 14 
		GLYPH space	; 15 
		GLYPH space	; 16 
		GLYPH space	; 17 
		GLYPH uparrow	; 18 
		GLYPH downarrow	; 19 
		GLYPH rightarrow; 1A
		GLYPH leftarrow	; 1B 
		GLYPH space	; 1C 
		GLYPH space	; 1D 
		GLYPH uparrow	; 1E 
		GLYPH downarrow ; 1F 
		GLYPH space	; 20 SPC
		GLYPH exclam	; 21 !
		GLYPH dquote	; 22 "
		GLYPH hash	; 23 #
		GLYPH pound	; 24 $
		GLYPH percent	; 25 %
		GLYPH ampersand	; 26 &
		GLYPH quote	; 27 '
		GLYPH lparen	; 28 (
		GLYPH rparen	; 29 )
		GLYPH asterisk	; 2A *
		GLYPH plus	; 2B +
		GLYPH comma	; 2C ,
		GLYPH minus	; 2D -
		GLYPH period	; 2E .
		GLYPH slash	; 2F /
		GLYPH 0		; 30 0
		GLYPH 1		; 31 1
		GLYPH 2		; 32 2
		GLYPH 3		; 33 3
		GLYPH 4		; 34 4
		GLYPH 5		; 35 5
		GLYPH 6		; 36 6
		GLYPH 7		; 37 7
		GLYPH 8		; 38 8
		GLYPH 9		; 39 9
		GLYPH colon	; 3A :
		GLYPH semicolon	; 3B ;
		GLYPH langle	; 3C <
		GLYPH equal	; 3D =
		GLYPH rangle	; 3E >
		GLYPH query	; 3F ?
		GLYPH comat	; 40 @
		GLYPH A		; 41 A
		GLYPH B		; 42 B
		GLYPH C		; 43 C
		GLYPH D		; 44 D
		GLYPH E		; 45 E
		GLYPH F		; 46 F
		GLYPH G		; 47 G
		GLYPH H		; 48 H
		GLYPH I		; 49 I
		GLYPH J		; 4A J
		GLYPH K		; 4B K
		GLYPH L		; 4C L
		GLYPH M		; 4D M
		GLYPH N		; 4E N
		GLYPH O		; 4F O
		GLYPH P		; 50 P
		GLYPH Q		; 51 Q
		GLYPH R		; 52 R
		GLYPH S		; 53 S
		GLYPH T		; 54 T
		GLYPH U		; 55 U
		GLYPH V		; 56 V
		GLYPH W		; 57 W
		GLYPH X		; 58 X
		GLYPH Y		; 59 Y
		GLYPH Z		; 5A Z
		GLYPH lsquare	; 5B [
		GLYPH bslash	; 5C \
		GLYPH rsquare	; 5D ]
		GLYPH caret	; 5E ^
		GLYPH underbar	; 5F _
		GLYPH bquote	; 60 `
		GLYPH a		; 61 a
		GLYPH b		; 62 b
		GLYPH c		; 63 c
		GLYPH d		; 64 d
		GLYPH e		; 65 e
		GLYPH f		; 66 f
		GLYPH g		; 67 g
		GLYPH h		; 68 h
		GLYPH i		; 69 i
		GLYPH j		; 6A j
		GLYPH k		; 6B k
		GLYPH l		; 6C l
		GLYPH m		; 6D m
		GLYPH n		; 6E n
		GLYPH o		; 6F o
		GLYPH p		; 70 p
		GLYPH q		; 71 q
		GLYPH r		; 72 r
		GLYPH s		; 73 s
		GLYPH t		; 74 t
		GLYPH u		; 75 u
		GLYPH v		; 76 v
		GLYPH w		; 77 w
		GLYPH x		; 78 x
		GLYPH y		; 79 y
		GLYPH z		; 7A z
		GLYPH lbrace	; 7B {
		GLYPH vbar	; 7C |
		GLYPH rbrace	; 7D }
		GLYPH tilde	; 7E ~
		GLYPH arrow	; 7F 
		GLYPH space	; 80 Ä
		GLYPH space	; 81 Å
		GLYPH space	; 82 Ç
		GLYPH space	; 83 É
		GLYPH space	; 84 Ñ
		GLYPH space	; 85 Ö
		GLYPH space	; 86 Ü
		GLYPH space	; 87 á
		GLYPH space	; 88 à
		GLYPH space	; 89 â
		GLYPH space	; 8A ä
		GLYPH space	; 8B ã
		GLYPH space	; 8C å
		GLYPH space	; 8D ç
		GLYPH space	; 8E é
		GLYPH space	; 8F è
		GLYPH space	; 90 ê
		GLYPH space	; 91 ë
		GLYPH space	; 92 í
		GLYPH space	; 93 ì
		GLYPH space	; 94 î
		GLYPH space	; 95 ï
		GLYPH space	; 96 ñ
		GLYPH space	; 97 ó
		GLYPH space	; 98 ò
		GLYPH space	; 99 ô
		GLYPH space	; 9A ö
		GLYPH space	; 9B õ
		GLYPH space	; 9C ú
		GLYPH space	; 9D ù
		GLYPH space	; 9E û
		GLYPH space	; 9F ü
		GLYPH space	; A0 †
		GLYPH space	; A1 °
		GLYPH space	; A2 ¢
		GLYPH space	; A3 £
		GLYPH space	; A4 §
		GLYPH space	; A5 •
		GLYPH space	; A6 ¶
		GLYPH space	; A7 ß
		GLYPH space	; A8 ®
		GLYPH space	; A9 ©
		GLYPH space	; AA ™
		GLYPH space	; AB ´
		GLYPH space	; AC ¨
		GLYPH space	; AD ≠
		GLYPH space	; AE Æ
		GLYPH space	; AF Ø
		GLYPH space	; B0 ∞
		GLYPH space	; B1 ±
		GLYPH space	; B2 ≤
		GLYPH ld_tb	; B3 ≥
		GLYPH ld_tbl	; B4 ¥
		GLYPH ld_tbL	; B5 µ
		GLYPH ld_TBl	; B6 ∂
		GLYPH ld_Bl	; B7 ∑
		GLYPH ld_bL	; B8 ∏
		GLYPH ld_TBL	; B9 π
		GLYPH ld_TB	; BA ∫
		GLYPH ld_BL	; BB ª
		GLYPH ld_TL	; BC º
		GLYPH ld_Tl	; BD Ω
		GLYPH ld_tL	; BE æ
		GLYPH ld_bl	; BF ø
		GLYPH ld_tr	; C0 ¿
		GLYPH ld_tlr	; C1 ¡
		GLYPH ld_blr	; C2 ¬
		GLYPH ld_tbr	; C3 √
		GLYPH ld_lr	; C4 ƒ
		GLYPH ld_tblr	; C5 ≈
		GLYPH ld_tbR	; C6 ∆
		GLYPH ld_TBr	; C7 «
		GLYPH ld_TR	; C8 »
		GLYPH ld_BR	; C9 …
		GLYPH ld_TLR	; CA  
		GLYPH ld_BLR	; CB À
		GLYPH ld_TBR	; CC Ã
		GLYPH ld_LR	; CD Õ
		GLYPH ld_TBLR	; CE Œ
		GLYPH ld_tLR	; CF œ
		GLYPH ld_Tlr	; D0 –
		GLYPH ld_bLR	; D1 —
		GLYPH ld_Blr	; D2 “
		GLYPH ld_Tr	; D3 ”
		GLYPH ld_tR	; D4 ‘
		GLYPH ld_bR	; D5 ’
		GLYPH ld_Br	; D6 ÷
		GLYPH ld_TBlr	; D7 ◊
		GLYPH ld_tbLR	; D8 ÿ
		GLYPH ld_tl	; D9 Ÿ
		GLYPH ld_br	; DA ⁄
		GLYPH space	; DB €
		GLYPH space	; DC ‹
		GLYPH space	; DD ›
		GLYPH space	; DE ﬁ
		GLYPH space	; DF ﬂ
		GLYPH alpha	; E0 ‡
		GLYPH space	; E1 ·
		GLYPH space	; E2 ‚
		GLYPH space	; E3 „
		GLYPH space	; E4 ‰
		GLYPH space	; E5 Â
		GLYPH space	; E6 Ê
		GLYPH space	; E7 Á
		GLYPH space	; E8 Ë
		GLYPH space	; E9 È
		GLYPH space	; EA Í
		GLYPH space	; EB Î
		GLYPH space	; EC Ï
		GLYPH space	; ED Ì
		GLYPH space	; EE Ó
		GLYPH space	; EF Ô
		GLYPH space	; F0 
		GLYPH space	; F1 Ò
		GLYPH space	; F2 Ú
		GLYPH space	; F3 Û
		GLYPH space	; F4 Ù
		GLYPH space	; F5 ı
		GLYPH space	; F6 ˆ
		GLYPH space	; F7 ˜
		GLYPH degree	; F8 ¯
		GLYPH space	; F9 ˘
		GLYPH space	; FA ˙
		GLYPH tick	; FB ˚
		GLYPH space	; FC ¸
		GLYPH sup2	; FD ˝
		GLYPH space	; FE ˛
		GLYPH space	; FF ˇ


	; Glyphs
	;
.radix 2
_FontF3x5_Glyphs label byte

glyph_space	db 00000000
		db 00000000
		db 00000000
		db 00000000
		db 00000000

glyph_exclam	db 01000000
		db 01000000
		db 01000000
		db 00000000
		db 01000000

glyph_dquote	db 10100000
		db 10100000
		db 00000000
		db 00000000
		db 00000000

glyph_pound	db 01100000
		db 01000000
		db 11100000
		db 01000000
		db 11100000

glyph_hash	db 01000000
		db 11100000
		db 01000000
		db 11100000
		db 01000000

glyph_percent	db 10100000
		db 00100000
		db 01000000
		db 10000000
		db 10100000

glyph_ampersand	db 01000000
		db 10100000
		db 10100000
		db 01000000
		db 10100000

glyph_quote	db 01000000
		db 10000000
		db 00000000
		db 00000000
		db 00000000

glyph_lparen	db 00100000
		db 01000000
		db 01000000
		db 01000000
		db 00100000

glyph_rparen	db 10000000
		db 01000000
		db 01000000
		db 01000000
		db 10000000

glyph_asterisk	db 00000000
		db 10100000
		db 01000000
		db 10100000
		db 00000000

glyph_plus	db 00000000
		db 01000000
		db 11100000
		db 01000000
		db 00000000

glyph_comma	db 00000000
		db 00000000
		db 00000000
		db 01000000
		db 10000000

glyph_minus	db 00000000
		db 00000000
		db 11100000
		db 00000000
		db 00000000

glyph_period	db 00000000
		db 00000000
		db 00000000
		db 00000000
		db 01000000

glyph_slash	db 00100000
		db 00100000
		db 01000000
		db 10000000
		db 10000000

glyph_0		db 11100000
		db 10100000
		db 10100000
		db 10100000
		db 11100000

glyph_1		db 11000000
		db 01000000
		db 01000000
		db 01000000
		db 11100000

glyph_2		db 11100000
		db 00100000
		db 11100000
		db 10000000
		db 11100000

glyph_3		db 11100000
		db 00100000
		db 01100000
		db 00100000
		db 11100000

glyph_4		db 10000000
		db 10000000
		db 10100000
		db 11100000
		db 00100000

glyph_5		db 11100000
		db 10000000
		db 11100000
		db 00100000
		db 11100000

glyph_6		db 11100000
		db 10000000
		db 11100000
		db 10100000
		db 11100000

glyph_7		db 11100000
		db 00100000
		db 00100000
		db 00100000
		db 00100000

glyph_8		db 11100000
		db 10100000
		db 11100000
		db 10100000
		db 11100000

glyph_9		db 11100000
		db 10100000
		db 11100000
		db 00100000
		db 11100000

glyph_colon	db 00000000
		db 01000000
		db 00000000
		db 01000000
		db 00000000

glyph_semicolon	db 00000000
		db 01000000
		db 00000000
		db 01000000
		db 10000000

glyph_langle	db 00100000
		db 01000000
		db 10000000
		db 01000000
		db 00100000

glyph_equal	db 00000000
		db 11100000
		db 00000000
		db 11100000
		db 00000000

glyph_rangle	db 10000000
		db 01000000
		db 00100000
		db 01000000
		db 10000000

glyph_query	db 11100000
		db 00100000
		db 01100000
		db 00000000
		db 01000000

glyph_comat	db 11100000
		db 10100000
		db 10100000
		db 10000000
		db 11100000

glyph_A		db 01000000
		db 10100000
		db 10100000
		db 11100000
		db 10100000

glyph_B		db 11000000
		db 10100000
		db 11000000
		db 10100000
		db 11000000

glyph_C		db 01100000
		db 10000000
		db 10000000
		db 10000000
		db 01100000

glyph_D		db 11000000
		db 10100000
		db 10100000
		db 10100000
		db 11000000

glyph_E		db 11100000
		db 10000000
		db 11000000
		db 10000000
		db 11100000

glyph_F		db 11100000
		db 10000000
		db 11000000
		db 10000000
		db 10000000

glyph_G		db 01100000
		db 10000000
		db 10100000
		db 10100000
		db 01100000

glyph_H		db 10100000
		db 10100000
		db 11100000
		db 10100000
		db 10100000

glyph_I		db 11100000
		db 01000000
		db 01000000
		db 01000000
		db 11100000

glyph_J		db 11100000
		db 01000000
		db 01000000
		db 01000000
		db 11000000

glyph_K		db 10100000
		db 11100000
		db 11000000
		db 11100000
		db 10100000

glyph_L		db 10000000
		db 10000000
		db 10000000
		db 10000000
		db 11100000

glyph_M		db 10100000
		db 11100000
		db 11100000
		db 10100000
		db 10100000

glyph_N		db 10100000
		db 11100000
		db 11100000
		db 11100000
		db 10100000

glyph_O		db 01000000
		db 10100000
		db 10100000
		db 10100000
		db 01000000

glyph_P		db 11000000
		db 10100000
		db 11000000
		db 10000000
		db 10000000

glyph_Q		db 01000000
		db 10100000
		db 10100000
		db 10100000
		db 01100000

glyph_R		db 11000000
		db 10100000
		db 11000000
		db 10100000
		db 10100000

glyph_S		db 01100000
		db 10000000
		db 01000000
		db 00100000
		db 11000000

glyph_T		db 11100000
		db 01000000
		db 01000000
		db 01000000
		db 01000000	

glyph_U		db 10100000
		db 10100000
		db 10100000
		db 10100000
		db 11100000

glyph_V		db 10100000
		db 10100000
		db 10100000
		db 10100000
		db 01000000

glyph_W		db 10100000
		db 10100000
		db 11100000
		db 11100000
		db 10100000

glyph_X		db 10100000
		db 10100000
		db 01000000
		db 10100000
		db 10100000

glyph_Y		db 10100000
		db 10100000
		db 11100000
		db 01000000
		db 01000000

glyph_Z		db 11100000
		db 00100000
		db 01000000
		db 10000000
		db 11100000

glyph_lsquare	db 11100000
		db 10000000
		db 10000000
		db 10000000
		db 11100000

glyph_bslash	db 10000000
		db 10000000
		db 01000000
		db 00100000
		db 00100000

glyph_rsquare	db 11100000
		db 00100000
		db 00100000
		db 00100000
		db 11100000

glyph_caret	db 01000000
		db 10100000
		db 00000000
		db 00000000
		db 00000000

glyph_underbar	db 00000000
		db 00000000
		db 00000000
		db 00000000
		db 11100000

glyph_bquote	db 01000000
		db 00100000
		db 00000000
		db 00000000
		db 00000000

glyph_lbrace	db 01100000
		db 01000000
		db 11000000
		db 01000000
		db 01100000

glyph_vbar	db 10000000
		db 10000000
		db 10000000
		db 10000000
		db 10000000

glyph_rbrace	db 11000000
		db 01000000
		db 01100000
		db 01000000
		db 11000000

glyph_tilde	db 01010000
		db 10100000
		db 00000000
		db 00000000
		db 00000000

glyph_openbox	db 11110000
		db 10010000
		db 10010000
		db 10010000
		db 11110000

glyph_alpha	db 00000000
		db 01010000
		db 10100000
		db 10100000
		db 01010000

glyph_sup2	db 11000000
		db 00100000
		db 01000000
		db 11100000
		db 00000000

glyph_degree	db 11100000
		db 10100000
		db 11100000
		db 00000000
		db 00000000

glyph_a		db 00000000
		db 01000000
		db 10100000
		db 11100000
		db 10100000

glyph_b		db 00000000
		db 11000000
		db 11000000
		db 10100000
		db 11000000

glyph_c		db 00000000
		db 01100000
		db 10000000
		db 10000000
		db 01100000

glyph_d		db 00000000
		db 11000000
		db 10100000
		db 10100000
		db 11000000

glyph_e		db 00000000
		db 11100000
		db 11000000
		db 10000000
		db 11100000

glyph_f		db 00000000
		db 11100000
		db 10000000
		db 11000000
		db 10000000

glyph_g		db 00000000
		db 01100000
		db 10000000
		db 10100000
		db 01100000

glyph_h		db 00000000
		db 10100000
		db 10100000
		db 11100000
		db 10100000

glyph_i		db 00000000
		db 11100000
		db 01000000
		db 01000000
		db 11100000

glyph_j		db 00000000
		db 11100000
		db 01000000
		db 01000000
		db 11000000

glyph_k		db 00000000
		db 10100000
		db 11000000
		db 11000000
		db 10100000

glyph_l		db 00000000
		db 10000000
		db 10000000
		db 10000000
		db 11100000

glyph_m		db 00000000
		db 10100000
		db 11100000
		db 10100000
		db 10100000

glyph_n		db 00000000
		db 10100000
		db 11100000
		db 11100000
		db 10100000

glyph_o		db 00000000
		db 01000000
		db 10100000
		db 10100000
		db 01000000

glyph_p		db 00000000
		db 11000000
		db 10100000
		db 11000000
		db 10000000

glyph_q		db 00000000
		db 01000000
		db 10100000
		db 10100000
		db 01100000

glyph_r		db 00000000
		db 11000000
		db 10100000
		db 11000000
		db 10100000

glyph_s		db 00000000
		db 01100000
		db 10000000
		db 00100000
		db 11000000

glyph_t		db 00000000
		db 11100000
		db 01000000
		db 01000000
		db 01000000	

glyph_u		db 00000000
		db 10100000
		db 10100000
		db 10100000
		db 11100000

glyph_v		db 00000000
		db 10100000
		db 10100000
		db 10100000
		db 01000000

glyph_w		db 00000000
		db 10100000
		db 10100000
		db 11100000
		db 10100000

glyph_x		db 00000000
		db 10100000
		db 01000000
		db 01000000
		db 10100000

glyph_y		db 00000000
		db 10100000
		db 10100000
		db 01000000
		db 01000000

glyph_z		db 00000000
		db 11100000
		db 00100000
		db 11000000
		db 11100000

glyph_ld_tb	db 01000000
		db 01000000
		db 01000000
		db 01000000
		db 01000000

glyph_ld_tbl	db 01000000
		db 01000000
		db 11000000
		db 01000000
		db 01000000

glyph_ld_tbL	db 01000000
		db 11000000
		db 01000000
		db 11000000
		db 01000000

glyph_ld_TBl	db 01100000
		db 01100000
		db 11100000
		db 01100000
		db 01100000

glyph_ld_Bl	db 00000000
		db 00000000
		db 11100000
		db 01100000
		db 01100000

glyph_ld_bL	db 00000000
		db 11100000
		db 00100000
		db 11100000
		db 00100000

glyph_ld_TBL	db 01100000
		db 11100000
		db 01100000
		db 11100000
		db 01100000

glyph_ld_TB	db 01100000
		db 01100000
		db 01100000
		db 01100000
		db 01100000

glyph_ld_BL	db 00000000
		db 01110000
		db 01100000
		db 01110000
		db 01100000
		      
glyph_ld_TL	db 01100000
		db 01110000
		db 01100000
		db 01110000
		db 00000000

glyph_ld_Tl	db 01100000
		db 01100000
		db 11100000
		db 00000000
		db 00000000

glyph_ld_tL	db 01000000
		db 11000000
		db 01000000
		db 11000000
		db 00000000

glyph_ld_bl	db 00000000
		db 00000000
		db 11000000
		db 01000000
		db 01000000

glyph_ld_tr	db 01000000
		db 01000000
		db 01110000
		db 00000000
		db 00000000

glyph_ld_tlr	db 01000000
		db 01000000
		db 11110000
		db 00000000
		db 00000000

glyph_ld_blr	db 00000000
		db 00000000
		db 11110000
		db 01000000
		db 01000000

glyph_ld_tbr	db 00000000
		db 00000000
		db 00000000
		db 00000000
		db 00000000

glyph_ld_lr	db 00000000
		db 00000000
		db 11110000
		db 00000000
		db 00000000

glyph_ld_tblr	db 01000000
		db 01000000
		db 11110000
		db 01000000
		db 01000000

glyph_ld_tbR	db 01000000
		db 01110000
		db 01000000
		db 01110000
		db 01000000

glyph_ld_TBr	db 01100000
		db 01100000
		db 01110000
		db 01100000
		db 01100000


glyph_ld_TR	db 01100000
		db 01110000
		db 01100000
		db 01110000
		db 00000000

glyph_ld_BR	db 00000000
		db 01110000
		db 01100000
		db 01110000
		db 01100000

glyph_ld_TLR	db 01100000
		db 11110000
		db 01100000
		db 11110000
		db 00000000

glyph_ld_BLR	db 00000000
		db 11110000
		db 01100000
		db 11110000
		db 01100000

glyph_ld_TBR	db 01100000
		db 01110000
		db 01100000
		db 01110000
		db 01100000

glyph_ld_LR	db 00000000
		db 11110000
		db 00000000
		db 11110000
		db 00000000

glyph_ld_TBLR	db 01100000
		db 11110000
		db 01100000
		db 11110000
		db 01100000

glyph_ld_tLR	db 01000000
		db 11110000
		db 01000000
		db 11110000
		db 00000000

glyph_ld_Tlr	db 01100000
		db 01100000
		db 11110000
		db 00000000
		db 00000000

glyph_ld_bLR	db 00000000
		db 11110000
		db 01000000
		db 11110000
		db 01000000

glyph_ld_Blr	db 00000000
		db 00000000
		db 11110000
		db 01100000
		db 01100000

glyph_ld_Tr	db 01100000
		db 01100000
		db 01110000
		db 00000000
		db 00000000

glyph_ld_tR	db 01000000
		db 01110000
		db 01000000
		db 01110000
		db 00000000

glyph_ld_bR	db 00000000
		db 01110000
		db 01000000
		db 01110000
		db 01000000

glyph_ld_Br	db 00000000
		db 00000000
		db 01110000
		db 01100000
		db 01100000

glyph_ld_TBlr	db 01100000
		db 01100000
		db 11110000
		db 01100000
		db 01100000

glyph_ld_tbLR	db 01000000
		db 11110000
		db 01000000
		db 11110000
		db 01000000

glyph_ld_tl	db 01000000
		db 01000000
		db 11000000
		db 00000000
		db 00000000

glyph_ld_br	db 00000000
		db 00000000
		db 01110000
		db 01000000
		db 01000000

glyph_arrow	db 11111000
		db 11110000
		db 11110000
		db 11001000
		db 10000100

glyph_uparrow	db 01000000
		db 11100000
		db 00000000
		db 00000000
		db 00000000

glyph_downarrow	db 00000000
		db 00000000
		db 00000000
		db 11100000
		db 01000000

glyph_leftarrow	db 00000000
		db 01000000
		db 11000000
		db 01000000
		db 00000000

glyph_rightarrow db 00000000
		db 01000000
		db 01100000
		db 01000000
		db 00000000

glyph_tick	db 00000000
		db 01100000
		db 01000000
		db 11000000
		db 01000000

glyph_bullet	db 00000000
		db 00000000
		db 01000000
		db 00000000
		db 00000000

	end

