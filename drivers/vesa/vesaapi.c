/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: vesaapi.c 1.1 1997/12/10 16:54:28 jon Exp $
 * $Locker: $
 *
 * Device methods
 */
#include <stddef.h>
#include <string.h>

#include "host.h"
#include "vesaapi.h"
#include "brassert.h"

BR_RCS_ID("$Id: vesaapi.c 1.1 1997/12/10 16:54:28 jon Exp $");

#define SCRATCH_SIZE 1024

static host_real_memory scratch;
static br_boolean scratchAllocated = BR_FALSE;
static host_regs r;

#define MAX_MODES (sizeof(vip->reserved)/sizeof(br_uint_16))

br_error VESAInfo(struct vesa_info *vip)
{
	struct vesa_info_rm *vip_rm;
	br_uint_16 mode,o,s,*modes;
	br_uint_8 *cp;
	int n;

	ASSERT(sizeof(struct vesa_info) == sizeof(struct vesa_info_rm));

	/*
	 * Allocate scratch buffer
	 */
	if(!scratchAllocated) {
		HostRealAllocate(&scratch, SCRATCH_SIZE);
		scratchAllocated = BR_TRUE;
	}

	/*
	 * Call VBE status interrupt
	 */
	HostFarDWordWrite(scratch.pm_off,scratch.pm_seg, 'V' | ('B'<<8) | ('E'<<16) | ('2'<<24) );
	r.x.eax	= 0x4f00;
	r.w.es = scratch.rm_seg;
	r.w.di = scratch.rm_off;
	HostRealInterruptCall(0x10, &r);

	if(r.h.al != 0x4f)
		return BRE_UNSUPPORTED;

	if(r.h.ah != 0x00)
		return BRE_FAIL;

	/*
	 * Copy vesa info. block into user's block
	 */
	HostFarBlockRead(scratch.pm_off, scratch.pm_seg, vip, sizeof(struct vesa_info));
	vip_rm = (struct vesa_info_rm *)vip;

	/*
	 * Grab modes
	 */
	modes = (br_uint_16 *)vip->reserved;
	for(n = 0; n < MAX_MODES ; n++) {
		modes[n] = HostRealWordRead(vip_rm->videomode_off + n*2, vip_rm->videomode_seg);
		if(modes[n] == 0xFFFF)
			break;
	}
	vip->videomode_ptr = modes;
	vip->videomode_ptr[MAX_MODES-1] = 0xFFFF;

	/*	
	 * Grab OEM strings
	 */
	cp = vip->oem_data;

	n = HostRealStringRead(vip_rm->oem_string_off, vip_rm->oem_string_seg,
		cp, sizeof(vip->oem_data) - (cp - vip->oem_data));
	vip->oem_string_ptr = cp;
	cp += n;

	if(vip_rm->vbe_version >= 0x200) {
	
		n = HostRealStringRead(vip_rm->oem_vendor_name_off, vip_rm->oem_vendor_name_seg,
			cp, sizeof(vip->oem_data) - (cp - vip->oem_data));
		vip->oem_vendor_name_ptr = cp;
		cp += n;

		n = HostRealStringRead(vip_rm->oem_product_name_off, vip_rm->oem_product_name_seg,
			cp, sizeof(vip->oem_data) - (cp - vip->oem_data));
		vip->oem_product_name_ptr = cp;
		cp += n;

		n = HostRealStringRead(vip_rm->oem_product_rev_off, vip_rm->oem_product_rev_seg,
			cp, sizeof(vip->oem_data) - (cp - vip->oem_data));
		vip->oem_product_rev_ptr = cp;
		cp += n;
	}

	return BRE_OK;
}

br_error VESAModeInfo(struct vesa_modeinfo *vmip, br_uint_32 mode)
{
	/*
	 * Allocate scratch buffer
	 */
	if(!scratchAllocated) {
		HostRealAllocate(&scratch, SCRATCH_SIZE);
		scratchAllocated = BR_TRUE;
	}

	/*
	 * Call VBE mode information
	 */
	r.x.eax	= 0x4f01;
	r.w.cx = mode;
	r.w.es = scratch.rm_seg;
	r.w.di = scratch.rm_off;
	HostRealInterruptCall(0x10, &r);

	if(r.h.al != 0x4f)
		return BRE_UNSUPPORTED;

	if(r.h.ah != 0x00)
		return BRE_FAIL;

	/*
	 * Copy mode info. block into user's block
	 */
	HostFarBlockRead(scratch.pm_off, scratch.pm_seg, vmip, sizeof(struct vesa_modeinfo));

	return BRE_OK;
}

br_error VESAModeSet(br_uint_32 mode)
{
	r.x.eax	= 0x4f02;
	r.w.bx = mode;
	HostRealInterruptCall(0x10, &r);

	if(r.h.al != 0x4f)
		return BRE_UNSUPPORTED;

	if(r.h.ah != 0x00)
		return BRE_FAIL;

	return BRE_OK;
}

br_error VESAModeGet(br_uint_16 *mode)
{
	r.x.eax	= 0x4f03;
	HostRealInterruptCall(0x10, &r);

	if(r.h.al != 0x4f)
		return BRE_UNSUPPORTED;

	if(r.h.ah != 0x00)
		return BRE_FAIL;

	*mode = r.w.bx;

	return BRE_OK;
}


br_error VESAScanlineBytesSet(br_uint_32 width, br_uint_16 * bytes, br_uint_16 * pixels, br_uint_16 * scanlines)
{
	r.x.eax	= 0x4f06;
	r.h.bl = 2;
	r.w.cx = width;
	HostRealInterruptCall(0x10, &r);

	if(r.h.al != 0x4f)
		return BRE_UNSUPPORTED;

	if(r.h.ah != 0x00)
		return BRE_FAIL;

	*bytes = r.w.bx;
	*pixels = r.w.cx;
	*scanlines = r.w.dx;

	return BRE_OK;
}

br_error VESAScanlinePixelsSet(br_uint_32 width, br_uint_16 * bytes, br_uint_16 * pixels, br_uint_16 * scanlines)
{
	r.x.eax	= 0x4f06;
	r.h.bl = 0;
	r.w.cx = width;
	HostRealInterruptCall(0x10, &r);

	if(r.h.al != 0x4f)
		return BRE_UNSUPPORTED;

	if(r.h.ah != 0x00)
		return BRE_FAIL;

	*bytes = r.w.bx;
	*pixels = r.w.cx;
	*scanlines = r.w.dx;

	return BRE_OK;
}

br_error VESAScanlineLengthGet(br_uint_16 * bytes, br_uint_16 * pixels, br_uint_16 * scanlines)
{
	r.x.eax	= 0x4f06;
	r.h.bl = 1;
	HostRealInterruptCall(0x10, &r);

	if(r.h.al != 0x4f)
		return BRE_UNSUPPORTED;

	if(r.h.ah != 0x00)
		return BRE_FAIL;

	*bytes = r.w.bx;
	*pixels = r.w.cx;
	*scanlines = r.w.dx;

	return BRE_OK;
}


br_error VESADisplayStartSet(br_uint_32 x, br_uint_32 y)
{
	r.x.eax	= 0x4f07;
	r.h.bl = 0;
	r.w.cx = x;
	r.w.dx = y;
	HostRealInterruptCall(0x10, &r);

	if(r.h.al != 0x4f)
		return BRE_UNSUPPORTED;

	if(r.h.ah != 0x00)
		return BRE_FAIL;

	return BRE_OK;
}

br_error VESADisplayStartGet(br_uint_32 *px, br_uint_32 *py)
{
	r.x.eax	= 0x4f07;
	r.h.bl = 1;
	HostRealInterruptCall(0x10, &r);

	if(r.h.al != 0x4f)
		return BRE_UNSUPPORTED;

	if(r.h.ah != 0x00)
		return BRE_FAIL;

	*px = r.w.cx;
	*py = r.w.dx;

	return BRE_OK;
}


br_error VESAWindowSet(br_uint_32 window, br_uint_32 position)
{
	r.x.eax	= 0x4f05;
	r.h.bh = 0;
	r.h.bl = window;
	r.w.dx = position;

	HostRealInterruptCall(0x10, &r);

	if(r.h.al != 0x4f)
		return BRE_UNSUPPORTED;

	if(r.h.ah != 0x00)
		return BRE_FAIL;

	return BRE_OK;
}

br_error VESAWindowGet(br_uint_32 window, br_uint_32 *position)
{
	r.x.eax	= 0x4f05;
	r.h.bh = 1;
	r.h.bl = window;

	HostRealInterruptCall(0x10, &r);

	if(r.h.al != 0x4f)
		return BRE_UNSUPPORTED;

	if(r.h.ah != 0x00)
		return BRE_FAIL;

	*position = r.w.dx;

	return BRE_OK;
}

br_error VESAPaletteFormatSet(br_uint_32 format)
{
	r.x.eax	= 0x4f08;
	r.h.bl = 0;
	r.h.bh = format;

	HostRealInterruptCall(0x10, &r);

	if(r.h.al != 0x4f)
		return BRE_UNSUPPORTED;

	if(r.h.ah != 0x00)
		return BRE_FAIL;

	return BRE_OK;
}

br_error VESAPaletteFormatGet(br_uint_32 *formatp)
{
	r.x.eax	= 0x4f08;
	r.h.bl = 1;

	HostRealInterruptCall(0x10, &r);

	if(r.h.al != 0x4f)
		return BRE_UNSUPPORTED;

	if(r.h.ah != 0x00)
		return BRE_FAIL;

	*formatp = r.h.bh;

	return BRE_OK;
}

br_error VESAPaletteSet(br_int_32 *values, br_int_32 first, br_int_32 count, br_boolean blank)
{
	/*
	 * Allocate scratch buffer
	 */
	if(!scratchAllocated) {
		HostRealAllocate(&scratch, SCRATCH_SIZE);
		scratchAllocated = BR_TRUE;
	}

	r.x.eax	= 0x4f09;
	r.h.bl = 0;
	r.w.cx = count;
	r.w.dx = first;
	r.w.es = scratch.rm_seg;
	r.w.di = scratch.rm_off;

	HostFarBlockWrite(scratch.pm_off, scratch.pm_seg, (br_uint_8 *)values, count * sizeof(*values)); 

	HostRealInterruptCall(0x10, &r);

	if(r.h.al != 0x4f)
		return BRE_UNSUPPORTED;

	if(r.h.ah != 0x00)
		return BRE_FAIL;

	return BRE_OK;
}

br_error VESAPaletteGet(br_int_32 *values, br_int_32 first, br_int_32 count)
{
	/*
	 * Allocate scratch buffer
	 */
	if(!scratchAllocated) {
		HostRealAllocate(&scratch, SCRATCH_SIZE);
		scratchAllocated = BR_TRUE;
	}

	r.x.eax	= 0x4f09;
	r.h.bl = 1;
	r.w.cx = count;
	r.w.dx = first;
	r.w.es = scratch.rm_seg;
	r.w.di = scratch.rm_off;

	HostRealInterruptCall(0x10, &r);

	if(r.h.al != 0x4f)
		return BRE_UNSUPPORTED;

	if(r.h.ah != 0x00)
		return BRE_FAIL;

	HostFarBlockRead(scratch.pm_off, scratch.pm_seg, (br_uint_8 *)values, count * sizeof(*values)); 

	return BRE_OK;
}

br_error VESAProtectedModeInterface(br_uint_16 *poffset, br_uint_16 *pseg, br_uint_16 *psize)
{
	r.x.eax	= 0x4f0a;
	r.h.bl = 0;

	HostRealInterruptCall(0x10, &r);

	if(r.h.al != 0x4f)
		return BRE_UNSUPPORTED;

	if(r.h.ah != 0x00)
		return BRE_FAIL;

	*poffset = r.w.di;
	*pseg = r.w.es;
	*psize = r.w.cx;

	return BRE_OK;
}

