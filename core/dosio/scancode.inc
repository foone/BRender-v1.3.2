;;
;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: scancode.inc 1.1 1997/12/10 16:41:00 jon Exp $
;; $Locker: $
;;
;;
; Scancodes for IBM keyboard
;
SC_ESC		equ	01h
SC_1		equ	02h
SC_2		equ	03h
SC_3		equ	04h
SC_4		equ	05h
SC_5		equ	06h
SC_6		equ	07h
SC_7		equ	08h
SC_8		equ	09h
SC_9		equ	0ah
SC_0		equ	0bh
SC_MINUS	equ	0ch
SC_EQUALS	equ	0dh
SC_BACKSPACE	equ	0eh
SC_TAB		equ	0fh
SC_Q		equ	10h
SC_W		equ	11h
SC_E		equ	12h
SC_R		equ	13h
SC_T		equ	14h
SC_Y		equ	15h
SC_U		equ	16h
SC_I		equ	17h
SC_O		equ	18h
SC_P		equ	19h
SC_LSQUARE	equ	1ah
SC_RSQUARE	equ	1bh
SC_ENTER	equ	1ch
SC_CTRL		equ	1dh
SC_A		equ	1eh
SC_S		equ	1fh
SC_D		equ	20h
SC_F		equ	21h
SC_G		equ	22h
SC_H		equ	23h
SC_J		equ	24h
SC_K		equ	25h
SC_L		equ	26h
SC_SEMI		equ	27h
SC_QUOTE	equ	28h
SC_BACKQUOTE	equ	29h
SC_LSHIFT	equ	2ah
SC_BACKSLASH	equ	2bh
SC_Z		equ	2ch
SC_X		equ	2dh
SC_C		equ	2eh
SC_V		equ	2fh
SC_B		equ	30h
SC_N		equ	31h
SC_M		equ	32h
SC_COMMA	equ	33h
SC_DOT		equ	34h
SC_SLASH	equ	35h
SC_RSHIFT	equ	36h
SC_KP_ASTERISK	equ	37h
SC_ALT		equ	38h
SC_SPACE	equ	39h
SC_CAPSLOCK	equ	3ah
SC_F1		equ	3bh
SC_F2		equ	3ch
SC_F3		equ	3dh
SC_F4		equ	3eh
SC_F5		equ	3fh
SC_F6		equ	40h
SC_F7		equ	41h
SC_F8		equ	42h
SC_F9		equ	43h
SC_F10		equ	44h
SC_NUMLOCK	equ	45h
SC_SCRLOCK	equ	46h
SC_HOME		equ	47h
SC_C_UP		equ	48h
SC_PG_UP	equ	49h
SC_KP_MINUS	equ	4ah
SC_C_LEFT	equ	4bh
SC_KP_5		equ	4ch
SC_C_RIGHT	equ	4dh
SC_KP_PLUS	equ	4eh
SC_END		equ	4fh
SC_C_DOWN	equ	50h
SC_PG_DOWN	equ	51h
SC_INSERT	equ	52h
SC_DELETE	equ	53h

SC_EXTENDED	equ	80h


; Shift qualifiers:
;  specifiy the appropriate combination to detect only the shifted key,
;  specify QUAL_NONE to detected only the unshifted key,
;  specify no shift qualifiers to detected the key in any shift state.
;
QUAL_SHIFT	equ	01h
QUAL_CTRL	equ	02h
QUAL_ALT	equ	04h
QUAL_NONE	equ	08h

QUAL_ALL	equ	0fh

; Repeat control qualifiers:
;  specify QUAL_FIRST_DOWN to detect the key once for each downstroke,
;  specify QUAL_AUTO_REPT to detect the key once for each downstroke and once
;  more for each subsequent keyboard auto-repeat,
;  specify no repeat qualifiers to detect the key continuously while it is
;  held down.
;
REPT_FIRST_DOWN equ	10h
REPT_AUTO_REPT	equ	20h


