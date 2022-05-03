/*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: host.h 1.1 1997/12/10 16:41:13 jon Exp $
 * $Locker: $
 */
#ifndef _HOST_H_
#define _HOST_H_

#ifndef _BRENDER_H_
#include "brender.h"
#endif

/*
 * Structure used to describe host information
 */
typedef struct host_info {
	br_uint_32 size;
	char identifier[40];
	br_uint_32 capabilities;
	br_token processor_family;
	br_token processor_type;
} host_info;

#define HOST_CAPS_REAL_MEMORY			0x00000001	/* Can allocate/read/write real-mode mem.	*/
#define HOST_CAPS_REAL_INT_CALL			0x00000002	/* Can invoke real mode interrupts			*/
#define HOST_CAPS_REAL_INT_HOOK			0x00000004	/* Can hook real mode interrupts			*/
#define HOST_CAPS_PROTECTED_INT_CALL	0x00000008	/* Can invoke protected mode interrupts		*/
#define HOST_CAPS_PROTECTED_INT_HOOK	0x00000010	/* Can hook prot. mode interrupts			*/
#define HOST_CAPS_ALLOC_SELECTORS		0x00000020	/* Can allocate new selectors				*/
#define HOST_CAPS_PHYSICAL_MAP			0x00000040	/* Can map physical memory -> linear		*/
#define HOST_CAPS_EXCEPTION_HOOK		0x00000080	/* Can hook exceptions						*/
#define HOST_CAPS_BASE_SELECTORS_WRITE	0x00000100	/* Can modify base/limit of cs,ds,es,ss selectors	*/
#define HOST_CAPS_PORTS					0x00000200	/* Can use IO ports							*/
#define HOST_CAPS_MMX					0x00000400	/* Has MMX extensions						*/
#define HOST_CAPS_FPU					0x00000800	/* Has hardware FPU							*/
#define HOST_CAPS_CMOV                                  0x00001000      /* Has CMOV extensions */


/*
 * Aggregate used to represent a block of real-mode memory
 */
typedef struct host_real_memory {
	br_uint_32 pm_off;
	br_uint_16 pm_seg;

	br_uint_16 _reserved;

	br_uint_16 rm_off;
	br_uint_16 rm_seg;
} host_real_memory;

/*
 * Structure for passing register sets around - 'happens' to be
 * the same as DPMI
 */
typedef union host_regs {
	struct {
		br_uint_32 edi;
		br_uint_32 esi;
		br_uint_32 ebp;
		br_uint_32 _res;
		br_uint_32 ebx;
		br_uint_32 edx;
		br_uint_32 ecx;
		br_uint_32 eax;

		br_uint_16 flags;
		br_uint_16 es;
		br_uint_16 ds;
		br_uint_16 fs;
		br_uint_16 gs;
		br_uint_16 ip;
		br_uint_16 cs;
		br_uint_16 sp;
		br_uint_16 ss;
	} x;
	struct {
		br_uint_16 di, _pad0;
		br_uint_16 si, _pad1;
		br_uint_16 bp, _pad2;
		br_uint_16 _res,_pad3;
		br_uint_16 bx, _pad4;
		br_uint_16 dx, _pad5;
		br_uint_16 cx, _pad6;
		br_uint_16 ax, _pad7;

		br_uint_16 flags;
		br_uint_16 es;
		br_uint_16 ds;
		br_uint_16 fs;
		br_uint_16 gs;
		br_uint_16 ip;
		br_uint_16 cs;
		br_uint_16 sp;
		br_uint_16 ss;
	} w;
	struct {
		br_uint_32 _pad0[4];
		br_uint_8 bl, bh, _pad1, _pad2;
		br_uint_8 dl, dh, _pad3, _pad4;
		br_uint_8 cl, ch, _pad5, _pad6;
		br_uint_8 al, ah, _pad7, _pad8;
  } h;
} host_regs;

enum host_flags {
	HOST_FLAG_CARRY		=0x0001,
	HOST_FLAG_PARITY	=0x0004,
	HOST_FLAG_AUX_CARRY	=0x0010,
	HOST_FLAG_ZERO		=0x0040,
	HOST_FLAG_SIGN		=0x0080
};

/*
 * Structures use to hook interrupts and exceptions
 */
typedef struct host_interrupt_hook {
		br_uint_8	vector;
		br_boolean	active;

		br_uint_32	old_offset;
		br_uint_16	old_sel;
} host_interrupt_hook ;

typedef struct host_exception_hook {
		br_uint_8	exception;
		br_boolean	active;

		br_uint_32	old_offset;
		br_uint_16	old_sel;

		br_uint_8	scratch[256];
} host_exception_hook ;

/*
 * Types of config. string supported
 */
enum config_types {
		HOST_CS_NONE,
		HOST_CS_DRIVERS,
		HOST_CS_OUTPUT_TYPE,
		HOST_CS_SEARCH_PATH
};

#ifndef _HOST_P_H_
#include "host_p.h"
#endif

#endif

