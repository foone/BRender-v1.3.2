/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: hstsetup.c 1.3 1997/12/11 14:54:26 jon Exp $
 * $Locker: $
 *
 * Setup for host extender (registers DLLs for drivers)
 */
#include "brddi.h"

#include "host.h"
#include "hostimg.h"
#include "host_ip.h"

void BR_PUBLIC_ENTRY HostBegin(void)
{
	RealSelectorBegin();

	BrImageAdd(&Image_BRHOST1);
}

void BR_PUBLIC_ENTRY HostEnd(void)
{
	BrImageRemove(&Image_BRHOST1);

	RealSelectorEnd();
}

static host_info hostInfo = {
	sizeof(host_info),
#if defined(__PHARLAP386__) 
	"Pharlap TNT",
		HOST_CAPS_REAL_MEMORY
		| HOST_CAPS_REAL_INT_CALL
		| HOST_CAPS_REAL_INT_HOOK
		| HOST_CAPS_PROTECTED_INT_CALL
		| HOST_CAPS_PROTECTED_INT_HOOK
		| HOST_CAPS_ALLOC_SELECTORS
		| HOST_CAPS_PHYSICAL_MAP,
#elif defined(__X32__)
	"Flashtek X-32VM",
		HOST_CAPS_REAL_MEMORY
		| HOST_CAPS_REAL_INT_CALL
		| HOST_CAPS_REAL_INT_HOOK
		| HOST_CAPS_PROTECTED_INT_CALL
		| HOST_CAPS_PROTECTED_INT_HOOK
		| HOST_CAPS_ALLOC_SELECTORS
		| HOST_CAPS_PHYSICAL_MAP,
	
#elif defined(__DOS4G__) 
	"Tenberry DOS4GW",
		HOST_CAPS_REAL_MEMORY
		| HOST_CAPS_REAL_INT_CALL
		| HOST_CAPS_REAL_INT_HOOK
		| HOST_CAPS_PROTECTED_INT_CALL
		| HOST_CAPS_PROTECTED_INT_HOOK
		| HOST_CAPS_ALLOC_SELECTORS
		| HOST_CAPS_PHYSICAL_MAP
		| HOST_CAPS_EXCEPTION_HOOK
		| HOST_CAPS_BASE_SELECTORS_WRITE,
	
#elif defined(__POWERPACK__) 
	"Borland Powerpack",
		HOST_CAPS_REAL_MEMORY
		| HOST_CAPS_REAL_INT_CALL
		| HOST_CAPS_REAL_INT_HOOK
		| HOST_CAPS_PROTECTED_INT_CALL
		| HOST_CAPS_PROTECTED_INT_HOOK
		| HOST_CAPS_ALLOC_SELECTORS
		| HOST_CAPS_PHYSICAL_MAP
		| HOST_CAPS_EXCEPTION_HOOK
		| HOST_CAPS_BASE_SELECTORS_WRITE,
	
#elif defined(__GO32__)
	"DJGPP GO32",
		HOST_CAPS_REAL_MEMORY
		| HOST_CAPS_REAL_INT_CALL
		| HOST_CAPS_REAL_INT_HOOK
		| HOST_CAPS_PROTECTED_INT_CALL
		| HOST_CAPS_PROTECTED_INT_HOOK
		| HOST_CAPS_EXCEPTION_HOOK
		| HOST_CAPS_BASE_SELECTORS_WRITE,
#elif defined(__WIN_32__)
	"Microsoft WIN32",
	0,
#else
	"Unknown",
	0,
#endif
	BRT_INTEL,
	BRT_INTEL_386,
};

br_error BR_RESIDENT_ENTRY HostInfo(host_info *buffer, br_size_t buffersize)
{
	br_uint_32 features;
	br_value use_mmx;
	br_value use_cmov;

	if(buffersize < sizeof(host_info)) {
		BrMemCpy(buffer, &hostInfo, buffersize);
		return BRE_OVERFLOW;
	}

	BrMemCpy(buffer, &hostInfo, sizeof(host_info));

	/*
	 * Set processor type field
	 */
	CPUInfo(&buffer->processor_type, &features);
	buffer->capabilities |= features;

	/*
	 * If the USE_MMX flag is present, and is false, disable the MMX
	 * capability
	 */
    if (BrSystemConfigQuery(BRT_BRENDER_USE_MMX_B, &use_mmx) == BRE_OK && !use_mmx.b)
		buffer->capabilities &= ~HOST_CAPS_MMX;

	/*
	 * If the USE_CMOV flag is present, and is false, disable the CMOV
	 * capability
	 */
    if (BrSystemConfigQuery(BRT_BRENDER_USE_CMOV_B, &use_cmov) == BRE_OK && !use_cmov.b)
		buffer->capabilities &= ~HOST_CAPS_CMOV;

	
	return BRE_OK;
}

