/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: custsupt.h 1.1 1997/12/10 16:41:17 jon Exp $
 * $Locker: $
 *
 * Definitions for support routines available during render callbacks
 *
 */
#ifndef _CUSTSUPT_H_
#define _CUSTSUPT_H_

/*
 * Outcode bits
 */
#define OUTCODE_LEFT		0x00000001
#define OUTCODE_RIGHT		0x00000002
#define OUTCODE_TOP			0x00000004
#define OUTCODE_BOTTOM		0x00000008
#define OUTCODE_HITHER		0x00000010
#define OUTCODE_YON			0x00000020

#define OUTCODE_USER		0x00000040
#define OUTCODE_USER_ALL	0x00000FC0

#define OUTCODES_ALL		0x00000FFF

#define OUTCODE_N_LEFT		0x00010000
#define OUTCODE_N_RIGHT		0x00020000
#define OUTCODE_N_TOP		0x00040000
#define OUTCODE_N_BOTTOM	0x00080000
#define OUTCODE_N_HITHER	0x00100000
#define OUTCODE_N_YON		0x00200000
#define OUTCODE_N_USER		0x00400000
#define OUTCODE_N_USER_ALL	0x0FC00000

#define OUTCODES_NOT		0x0FFF0000


/*
 * Values for on screen test
 */
#define OSC_REJECT		BRT_REJECT
#define OSC_PARTIAL		BRT_PARTIAL
#define OSC_ACCEPT		BRT_ACCEPT

#endif


