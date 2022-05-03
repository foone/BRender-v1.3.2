/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: host_p.h 1.1 1997/12/10 16:41:13 jon Exp $
 * $Locker: $
 *
 * Function prototypes for BRender Host library
 */
#ifndef _HOST_P_H_
#define _HOST_P_H_

#ifndef _NO_PROTOTYPES
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Setup
 */
void BR_PUBLIC_ENTRY HostBegin(void);
void BR_PUBLIC_ENTRY HostEnd(void);

br_error BR_RESIDENT_ENTRY HostInfo(struct host_info *buffer, br_size_t buffersize);

/*
 * Real mode access
 */
br_error BR_RESIDENT_ENTRY HostRealAllocate(struct host_real_memory *mem, br_uint_32 size);
br_error BR_RESIDENT_ENTRY HostRealFree(struct host_real_memory *mem);
br_error BR_RESIDENT_ENTRY HostRealInterruptGet(br_uint_8 vector, br_uint_16 *offp, br_uint_16 *vsegp);
br_error BR_RESIDENT_ENTRY HostRealInterruptSet(br_uint_8 vector, br_uint_16 voff, br_uint_16 vseg);
br_error BR_RESIDENT_ENTRY HostRealInterruptCall(br_uint_8 vector, union host_regs *regs);

#if 0
br_error BR_RESIDENT_ENTRY HostRealFarCall(br_uint_16 off, br_uint_16 seg, union host_regs *regs);
#endif

#if 0
/* Idea:
 * Temporary buffer, only one allocation can be outstanding - no DOS functions can be called
 * whilst it is in use.
 */
br_error BR_RESIDENT_ENTRY HostRealAllocateScratch(struct host_real_memory *mem, br_uint_32 size);
br_error BR_RESIDENT_ENTRY HostRealFreeScratch(struct host_real_memory *mem);
#endif

/*
 * Real memory access
 */
void BR_RESIDENT_ENTRY HostRealBlockWrite(br_uint_16 offset, br_uint_16 seg, void *block, br_uint_32 count);
void BR_RESIDENT_ENTRY HostRealBlockRead(br_uint_16 offset, br_uint_16 seg, void *block, br_uint_32 count);

br_uint_32 BR_RESIDENT_ENTRY HostRealStringWrite(br_uint_16 offset, br_uint_16 seg, br_uint_8 *string, br_uint_32 max);
br_uint_32 BR_RESIDENT_ENTRY HostRealStringRead(br_uint_16 offset, br_uint_16 seg, br_uint_8 *string, br_uint_32 max);

void BR_RESIDENT_ENTRY HostRealBlockFill(br_uint_16 offset, br_uint_16 seg, br_uint_8 value, br_uint_32 count);

void BR_RESIDENT_ENTRY HostRealByteWrite(br_uint_16 offset, br_uint_16 seg, br_uint_8 value);
void BR_RESIDENT_ENTRY HostRealWordWrite(br_uint_16 offset, br_uint_16 seg, br_uint_16 value);
void BR_RESIDENT_ENTRY HostRealDWordWrite(br_uint_16 offset, br_uint_16 seg, br_uint_32 value);

br_uint_8 BR_RESIDENT_ENTRY HostRealByteRead(br_uint_16 offset, br_uint_16 seg);
br_uint_16 BR_RESIDENT_ENTRY HostRealWordRead(br_uint_16 offset, br_uint_16 seg);
br_uint_32 BR_RESIDENT_ENTRY HostRealDWordRead(br_uint_16 offset, br_uint_16 seg);

/*
 * Protected mode access
 */
br_error BR_RESIDENT_ENTRY HostInterruptHook(host_interrupt_hook *h, br_uint_8 vector, br_uint_32  off,  br_uint_16  sel);
br_error BR_RESIDENT_ENTRY HostInterruptUnhook(host_interrupt_hook *h);

br_error BR_RESIDENT_ENTRY HostExceptionHook(host_exception_hook *h, br_uint_8 exception, br_uint_32  off,  br_uint_16  sel);
br_error BR_RESIDENT_ENTRY HostExceptionUnhook(host_exception_hook *h);

br_error BR_RESIDENT_ENTRY HostInterruptCall(br_uint_8 vector, union host_regs *regs);
br_error BR_RESIDENT_ENTRY HostRegistersGet(union host_regs *regs);

/**
 ** Page locking
 **/
br_error BR_RESIDENT_ENTRY HostLock(br_uint_32 offset, br_uint_16 sel, br_size_t size);
br_error BR_RESIDENT_ENTRY HostUnlock(br_uint_32 offset, br_uint_16 sel, br_size_t size);

/**
 ** Selector mangement
 **/
br_error BR_RESIDENT_ENTRY HostSelectorAllocate(br_uint_16 *selp);
br_error BR_RESIDENT_ENTRY HostSelectorAllocateAlias(br_uint_16 *aliasp, br_uint_16 sel);
br_error BR_RESIDENT_ENTRY HostSelectorAllocateLinear(br_uint_16 *selp, br_uint_32 linear_addr, br_size_t size);
br_error BR_RESIDENT_ENTRY HostSelectorAllocatePhysical(br_uint_16 *selp, br_uint_32 phys_addr, br_size_t size);
br_error BR_RESIDENT_ENTRY HostSelectorFree(br_uint_16 sel);

br_error BR_RESIDENT_ENTRY HostSelectorBaseSet(br_uint_16 sel, br_uint_32 base);
br_error BR_RESIDENT_ENTRY HostSelectorLimitSet(br_uint_16 sel, br_size_t limit);
br_error BR_RESIDENT_ENTRY HostSelectorBaseQuery(br_uint_32 *basep, br_uint_16 sel);
br_error BR_RESIDENT_ENTRY HostSelectorLimitQuery(br_uint_32 *limitp, br_uint_16 sel);

br_error BR_RESIDENT_ENTRY HostSelectorReal(br_uint_16 *selp);
br_error BR_RESIDENT_ENTRY HostSelectorDS(br_uint_16 *selp);
br_error BR_RESIDENT_ENTRY HostSelectorCS(br_uint_16 *selp);
br_error BR_RESIDENT_ENTRY HostSelectorSS(br_uint_16 *selp);
br_error BR_RESIDENT_ENTRY HostSelectorES(br_uint_16 *selp);

/**
 ** IO port access
 **/
/*
 * Indicate that we would like access to the specified range of IO ports
 */
br_error BR_RESIDENT_ENTRY HostPortEnable(br_uint_16 port, br_boolean exclusive);
br_error BR_RESIDENT_ENTRY HostPortDisable(br_uint_16 port);

br_error BR_RESIDENT_ENTRY HostPortRangeEnable(br_uint_16 low, br_uint_16 high, br_boolean exclusive);
br_error BR_RESIDENT_ENTRY HostPortRangeDisable(br_uint_16 low, br_uint_16 high);

/*
 * Far memory access
 */
void BR_RESIDENT_ENTRY HostFarBlockWrite(br_uint_32 offset, br_uint_16 sel, void *block, br_uint_32 count);
void BR_RESIDENT_ENTRY HostFarBlockRead(br_uint_32 offset, br_uint_16 sel, void *block, br_uint_32 count);

br_uint_32 BR_RESIDENT_ENTRY HostFarStringWrite(br_uint_32 offset, br_uint_16 sel, br_uint_8 *string, br_uint_32 max);
br_uint_32 BR_RESIDENT_ENTRY HostFarStringRead(br_uint_32 offset, br_uint_16 sel, br_uint_8 *string, br_uint_32 max);

void BR_RESIDENT_ENTRY HostFarBlockFill(br_uint_32 offset, br_uint_16 sel, br_uint_8 value, br_uint_32 count);

void BR_RESIDENT_ENTRY HostFarByteWrite(br_uint_32 offset, br_uint_16 sel, br_uint_8 value);
void BR_RESIDENT_ENTRY HostFarWordWrite(br_uint_32 offset, br_uint_16 sel, br_uint_16 value);
void BR_RESIDENT_ENTRY HostFarDWordWrite(br_uint_32 offset, br_uint_16 sel, br_uint_32 value);

br_uint_8 BR_RESIDENT_ENTRY HostFarByteRead(br_uint_32 offset, br_uint_16 sel);
br_uint_16 BR_RESIDENT_ENTRY HostFarWordRead(br_uint_32 offset, br_uint_16 sel);
br_uint_32 BR_RESIDENT_ENTRY HostFarDWordRead(br_uint_32 offset, br_uint_16 sel);

/*
 * Native loadable modules
 */
void * BR_RESIDENT_ENTRY HostImageLoad(char *name);
void BR_RESIDENT_ENTRY HostImageUnload(void *image);
void * BR_RESIDENT_ENTRY HostImageLookupName(void *img, char *name, br_uint_32 hint);
void * BR_RESIDENT_ENTRY HostImageLookupOrdinal(void *img, br_uint_32 ordinal);

/*
 * Configuration strings
 */
br_error BR_RESIDENT_ENTRY HostConfigStringSize(br_size_t *size, br_int_32 type);
br_error BR_RESIDENT_ENTRY HostConfigStringQuery(char *buffer, br_size_t buffer_size, br_int_32 type);


// INI File 

br_error BR_RESIDENT_ENTRY HostGetIniEntry( char *ini_file, char *section_name,
                                        char *entry, char *Buffer, int max );

br_error BR_RESIDENT_ENTRY HostIniReadSection(char *filename, char *section,
                                              char *buffer, br_size_t size);

br_error BR_RESIDENT_ENTRY HostIniWriteSection(char *filename, char *section,
                                               char *buffer, br_size_t size);

// Registry entry query

br_error BR_RESIDENT_ENTRY HostRegistryQuery( void *hKey, char *Path,
		                        char *entry, char *Buffer, 
       	       				   br_uint_16 max, br_uint_16 *size );


br_error BR_RESIDENT_ENTRY HostRegistryReadKey(void *key, char *subkey,
                                               char *buffer, br_size_t size);

br_error BR_RESIDENT_ENTRY HostRegistryWriteKey(void *key, char *subkey,
                                                char *buffer, br_size_t size);


// INI file query

br_error BR_RESIDENT_ENTRY HostIniQuery( char *ini_file, char *section_name,
                                            char *entry, char *Buffer, int max,
                                            br_uint_16 *size );



// Environment variable query.

br_boolean BR_RESIDENT_ENTRY HostIniSectionExists( char *ini_file, char *section_name );


char *BR_RESIDENT_ENTRY HostDefaultDevice();


#ifdef __cplusplus
};
#endif
#endif /* _NO_PROTOTYPES */
#endif

