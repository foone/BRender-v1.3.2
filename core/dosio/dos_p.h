/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: dos_p.h 1.1 1997/12/10 16:40:59 jon Exp $
 * $Locker: $
 *
 * Function prototypes for Brender DOS IO library
 */
#ifndef _DOS_P_H_
#define _DOS_P_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _NO_PROTOTYPES
/*
 * Setup/teardown
 */
br_error BR_PUBLIC_ENTRY DOSBegin(void);
void BR_PUBLIC_ENTRY DOSEnd(void);

/*
 * Mouse reading
 */
br_error BR_PUBLIC_ENTRY DOSMouseBegin(void);
void BR_PUBLIC_ENTRY DOSMouseEnd(void);
br_error BR_PUBLIC_ENTRY DOSMouseRead(br_int_32 *mouse_x, br_int_32 *mouse_y, br_uint_32 *mouse_buttons);

/*
 * Reading system clock
 */
br_error BR_ASM_CALL DOSClockBegin(void);
br_error BR_ASM_CALL DOSClockEnd(void);
br_uint_32 BR_ASM_CALL DOSClockRead(void);

/*
 * Keyboard UP/DOWN handling
 */
br_error BR_PUBLIC_ENTRY DOSKeyBegin(void);
void BR_PUBLIC_ENTRY DOSKeyEnd(void);
br_boolean BR_PUBLIC_ENTRY DOSKeyTest(br_uint_8 scancode,br_uint_8 qualifiers, br_uint_8 repeats);
br_error BR_PUBLIC_ENTRY DOSKeyEnableBIOS(br_boolean flag);

/*
 * Divide overflow suppressor
 */
br_error BR_PUBLIC_ENTRY DOSDivTrapBegin(void);
void BR_PUBLIC_ENTRY DOSDivTrapEnd(void);
br_uint_32 BR_ASM_CALL DOSDivTrapCount(br_boolean reset);

/*
 * Event queue for mouse and keyboard
 */
br_error BR_PUBLIC_ENTRY DOSEventBegin(void);
void BR_PUBLIC_ENTRY DOSEventEnd(void);
br_boolean BR_PUBLIC_ENTRY DOSEventWait(struct dosio_event *event, br_boolean block);

/*
 * Backwards compatibility
 */
#define DOSGfxBegin BrDevBeginOld
#define DOSGfxEnd BrDevEndOld

#define DOSGfxPaletteSet BrDevPaletteSetOld
#define DOSGfxPaletteSetEntry BrDevPaletteSetEntryOld

#endif /* _NO_PROTOTYPES */
#ifdef __cplusplus
};
#endif
#endif

