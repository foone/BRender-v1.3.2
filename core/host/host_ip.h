/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: host_ip.h 1.1 1997/12/10 16:41:13 jon Exp $
 * $Locker: $
 *
 * Prototypes for functions internal to host library
 */
#ifndef _HOST_IP_H_
#define _HOST_IP_H_

#ifndef NO_PROTOTYPES
#ifdef __cplusplus
extern "C" {
#endif

/*
 * real.asm
 */
br_error BR_ASM_CALL RealSelectorBegin(void);
void BR_ASM_CALL RealSelectorEnd(void);
extern br_uint_16 _RealSelector;

/*
 * prot.asm
 */
br_error BR_RESIDENT_ENTRY HostInterruptGet(br_uint_8 vector, br_uint_32 *offp, br_uint_16 *selp);
br_error BR_RESIDENT_ENTRY HostInterruptSet(br_uint_8 vector, br_uint_32 off, br_uint_16 sel);

br_error BR_RESIDENT_ENTRY HostExceptionGet(br_uint_8 exception, br_uint_32 *offp, br_uint_16 *selp);
br_error BR_RESIDENT_ENTRY HostExceptionSet(br_uint_8 exception, br_uint_32 off, br_uint_16 sel);

/*
 * Windows configuration
 */
//char * BR_RESIDENT_ENTRY GetWinProfileString(char *name);
char * BR_RESIDENT_ENTRY GetWinRegistryValue(char *name);

/*
 * cpuid.asm
 */
void BR_ASM_CALL CPUInfo(br_token *cpu_type, br_uint_32 *features);

#ifdef __cplusplus
};
#endif
#endif

#endif


