/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: timing.h 1.1 1997/12/10 16:41:10 jon Exp $
 * $Locker: $
 *
 * Timing hooks
 */
#if defined(BR_TIMING) && BR_TIMING
typedef struct br_timer {
	unsigned long start_l;
	unsigned long start_h;

	unsigned long time_l;
	unsigned long time_h;
} br_timer;

void _TimestampStart(struct br_timer *t);
void _TimestampEnd(struct br_timer *t);
void _TimestampAdd(struct br_timer *t);

#if defined(__WATCOMC__)
#pragma aux _TimestampStart =\
	".586p"\
	"rdtsc"\
	"mov	[ebx],eax"\
	"mov	[ebx+4],edx"\
	parm  [ebx] \
	modify nomemory [edx] ;

#pragma aux _TimestampEnd =\
	".586p"\
	"rdtsc"\
	"sub	eax,[ebx]"\
	"sbb	edx,[ebx+4]"\
	"mov	[ebx+8],eax"\
	"mov	[ebx+12],edx"\
	parm  [ebx] \
	modify [edx] ;

#pragma aux _TimestampAdd =\
	".586p"\
	"rdtsc"\
	"sub	eax,[ebx]"\
	"sbb	edx,[ebx+4]"\
	"add	[ebx+8],eax"\
	"adc	[ebx+12],edx"\
	parm  [ebx] \
	modify [edx] ;

#define BR_TIMER_START(ident) do {\
	extern struct br_timer ident;\
	_TimestampStart(&ident);\
} while(0)

#define BR_TIMER_END(ident) do {\
	extern struct br_timer ident;\
	_TimestampEnd(&ident);\
} while(0)

#define BR_TIMER_ADD(ident) do {\
	extern struct br_timer ident;\
	_TimestampAdd(&ident);\
} while(0)
#endif

#else

#define BR_TIMER_START(ident)	((void)0)
#define BR_TIMER_END(ident)		((void)0)
#define BR_TIMER_ADD(ident)		((void)0)

#endif

