 /*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: keyboard.h 1.1 1997/12/10 16:41:00 jon Exp $
 * $Locker: $
 *
 * Keyboard driver definitions and scancodes
 */
#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#define SC_ESC			0x01
#define SC_1			0x02
#define SC_2			0x03
#define SC_3			0x04
#define SC_4			0x05
#define SC_5			0x06
#define SC_6			0x07
#define SC_7			0x08
#define SC_8			0x09
#define SC_9			0x0a
#define SC_0			0x0b
#define SC_MINUS		0x0c
#define SC_EQUALS		0x0d
#define SC_BACKSPACE	0x0e
#define SC_TAB			0x0f
#define SC_Q			0x10
#define SC_W			0x11
#define SC_E			0x12
#define SC_R			0x13
#define SC_T			0x14
#define SC_Y			0x15
#define SC_U			0x16
#define SC_I			0x17
#define SC_O			0x18
#define SC_P			0x19
#define SC_LSQUARE		0x1a
#define SC_RSQUARE		0x1b
#define SC_ENTER		0x1c
#define SC_CTRL			0x1d
#define SC_A			0x1e
#define SC_S			0x1f
#define SC_D			0x20
#define SC_F			0x21
#define SC_G			0x22
#define SC_H			0x23
#define SC_J			0x24
#define SC_K			0x25
#define SC_L			0x26
#define SC_SEMI			0x27
#define SC_QUOTE		0x28
#define SC_BACKQUOTE	0x29
#define SC_LSHIFT		0x2a
#define SC_BACKSLASH	0x2b
#define SC_Z			0x2c
#define SC_X			0x2d
#define SC_C			0x2e
#define SC_V			0x2f
#define SC_B			0x30
#define SC_N			0x31
#define SC_M			0x32
#define SC_COMMA		0x33
#define SC_DOT			0x34
#define SC_SLASH		0x35
#define SC_RSHIFT		0x36
#define SC_KP_ASTERISK	0x37
#define SC_ALT			0x38
#define SC_SPACE		0x39
#define SC_CAPSLOCK		0x3a
#define SC_F1			0x3b
#define SC_F2			0x3c
#define SC_F3			0x3d
#define SC_F4			0x3e
#define SC_F5			0x3f
#define SC_F6			0x40
#define SC_F7			0x41
#define SC_F8			0x42
#define SC_F9			0x43
#define SC_F10			0x44
#define SC_NUMLOCK		0x45
#define SC_SCRLOCK		0x45
#define SC_HOME			0x47
#define SC_C_UP			0x48
#define SC_PG_UP		0x49
#define SC_KP_MINUS		0x4a
#define SC_C_LEFT		0x4b
#define SC_KP_5			0x4c
#define SC_C_RIGHT		0x4d
#define SC_KP_PLUS		0x4e
#define SC_END			0x4f
#define SC_C_DOWN		0x50
#define SC_PG_DOWN		0x51
#define SC_INSERT		0x52
#define SC_DELETE		0x53

#define SC_EXTENDED		0x80
						
/*
 * Shift qualifiers:
 *  specifiy the appropriate combination to detect only the shifted key,
 *  specify QUAL_NONE to detected only the unshifted key,
 *  specify no shift qualifiers to detected the key in any shift state.
 */
#define QUAL_SHIFT		0x01
#define QUAL_CTRL		0x02
#define QUAL_ALT		0x04
#define QUAL_NONE		0x08

#define QUAL_ALL		0x0f

/*
 * Repeat control qualifiers:
 *  specify REPT_FIRST_DOWN to detect the key once for each downstroke,
 *  specify REPT_AUTO_REPT to detect the key once for each downstroke and once
 *  more for each subsequent keyboard auto-repeat,
 *  specify no repeat qualifiers to detect the key continuously while it is
 *  held down.
 */

#define REPT_FIRST_DOWN 0x10
#define REPT_AUTO_REPT	0x20

#endif

