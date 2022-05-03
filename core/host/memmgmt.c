/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: memmgmt.c 1.2 1997/12/11 13:37:39 jon Exp $
 * $Locker: $
 *
 * Wrappers for hooking interrupts and exceptions
 */
#include "host.h"
#include "host_ip.h"

/*
 * Register bock used by all these functions
 */
static host_regs regs;

#define PAGE_BITS 12
#define PAGE_SIZE 4096

#define	BIG_LIMIT	(1 << 20)

#pragma pack(1)
/*
 * The structure of an LDT entry
 */
struct ldt {
	unsigned long limit_0:16;
	unsigned long base_0:16;

	unsigned long base_1:8;
	unsigned long type:4;
	unsigned long application:1;
	unsigned long dpl:2;
	unsigned long present:1;

	unsigned long limit_1:4;
	unsigned long system:1;
	unsigned long reserved:1;
	unsigned long use32:1;
	unsigned long granularity:1;
	unsigned long base_2:8;
};
#pragma pack()


/*
 * Mark a section of memory (sel:offset + size) as needing to be locked in memory
 */
br_error BR_RESIDENT_ENTRY HostLock(br_uint_32 offset, br_uint_16 sel, br_size_t size)
{
#if defined(__DPMI__)
	/*
	 * Get segment base address
	 */
	regs.w.ax = 0x006;
	regs.w.bx = sel;
	HostInterruptCall(0x31,&regs);

	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	/*
	 * Add base to offset
	 */
	offset += regs.w.dx + (regs.w.cx << 16);	

	/*
	 * Lock linear region
	 */
	regs.w.ax = 0x600;
	regs.w.cx = offset & 0xffff;
	regs.w.bx = (offset >> 16) & 0xffff;
	regs.w.di = size & 0xffff;
	regs.w.si = (size >> 16) & 0xffff;
	HostInterruptCall(0x31,&regs);

	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	return BRE_OK;
#elif defined(__PHARLAP386__) || defined (__X32__)
	regs.w.ax = 0x252b;
	regs.h.bh = 5;
	regs.h.bl = 1;
	regs.w.es = sel;
	regs.x.ecx = offset;
	regs.x.edx = size;
	HostInterruptCall(0x21,&regs);

	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;


	return BRE_OK;
#else
	return BRE_FAIL;
#endif

}

/*
 * Unmark a locked section of memory (sel:offset + size)
 */
br_error BR_RESIDENT_ENTRY HostUnlock(br_uint_32 offset, br_uint_16 sel, br_size_t size)
{
#if defined(__DPMI__)
	/*
	 * Get segment base address
	 */
	regs.w.ax = 0x006;
	regs.w.bx = sel;
	HostInterruptCall(0x31,&regs);

	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	/*
	 * Add base to offset
	 */
	offset += regs.w.dx + (regs.w.cx << 16);	

	/*
	 * Unlock linear region
	 */
	regs.w.ax = 0x601;
	regs.w.cx = offset & 0xffff;
	regs.w.bx = (offset >> 16) & 0xffff;
	regs.w.di = size & 0xffff;
	regs.w.si = (size >> 16) & 0xffff;
	HostInterruptCall(0x31,&regs);

	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	return BRE_OK;

#elif defined(__PHARLAP386__) || defined (__X32__)

	regs.w.ax = 0x252b;
	regs.h.bh = 6;
	regs.h.bl = 1;
	regs.w.es = sel;
	regs.x.ecx = offset;
	regs.x.edx = size;
	HostInterruptCall(0x21,&regs);

	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	return BRE_OK;
#else
	return BRE_FAIL;
#endif
}

/*
 * Create a new selector with 0 base and range
 */
br_error BR_RESIDENT_ENTRY HostSelectorAllocate(br_uint_16 *selp)
{
#if defined(__DPMI__)

	/*
	 * Allocate a new LDT entry
	 */
	regs.w.ax = 0x0000;
	regs.w.cx = 1;
	HostInterruptCall(0x31,&regs);

	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	*selp = regs.w.ax;

	return BRE_OK;

#elif defined(__PHARLAP386__)
	/*
	 * Allocate an LDT entry (0 size)
	 */
	regs.w.ax = 0x4800;
	regs.x.ebx = 0;

	HostInterruptCall(0x21,&regs);
	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	*selp = regs.w.ax;

	return BRE_OK;

#elif defined (__X32__)
	regs.w.ax = 0x3501;
	HostInterruptCall(0x21,&regs);
	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	HostSelectorBaseSet(regs.w.ax,0);
	HostSelectorLimitSet(regs.w.ax,0);

	*selp = regs.w.ax;

	return BRE_OK;
#else
	return BRE_FAIL;
#endif
}

/*
 * Create a new selector coresponding to a linear range of memory
 */
br_error BR_RESIDENT_ENTRY HostSelectorAllocateLinear(br_uint_16 *selp, br_uint_32 base, br_size_t size)
{
	br_uint_16 sel;
	br_error r;

	r = HostSelectorAllocate(&sel);
	if(r != BRE_OK) return r;

	r = HostSelectorBaseSet(sel, base);
	if(r != BRE_OK) return r;

	r = HostSelectorLimitSet(sel, size-1);
	if(r != BRE_OK) return r;

	*selp = sel;

	return BRE_OK;
}

br_error BR_RESIDENT_ENTRY HostSelectorAllocateAlias(br_uint_16 *aliasp, br_uint_16 sel)
{
	br_uint_16 alias;
	br_uint_32 base,limit;
	br_error r;

	r = HostSelectorBaseQuery(&base,sel);
	if(r != BRE_OK) return r;

	r = HostSelectorLimitQuery(&limit,sel);
	if(r != BRE_OK) return r;

	r = HostSelectorAllocateLinear(&alias, base, limit+1);
	if(r != BRE_OK) return r;

	*aliasp = alias;

	return BRE_OK;
}

/*
 * Map a range of physical memory into a new selector
 */
br_error BR_RESIDENT_ENTRY HostSelectorAllocatePhysical(br_uint_16 *selp, br_uint_32 phys_addr, br_size_t size)
{

#if defined(__DPMI__)

	regs.w.ax = 0x800;
	regs.w.cx = phys_addr & 0xffff;
	regs.w.bx = (phys_addr >> 16) & 0xffff;
	regs.w.di = size & 0xffff;
	regs.w.si = (size >> 16) & 0xffff;

	HostInterruptCall(0x31,&regs);

	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	return HostSelectorAllocateLinear(selp, regs.w.cx + (regs.w.bx << 16), size);

#elif defined(__PHARLAP386__)
	br_uint_16 sel;

	/*
	 * Allocate a 0 size LDT entry
	 */
	regs.w.ax = 0x4800;
	regs.x.ebx = 0;
	HostInterruptCall(0x21,&regs);
	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	sel = regs.w.ax;

	/*
	 * Map phyical memory to segment
	 */
	regs.w.ax = 0x250A;
	regs.w.es = sel;
	regs.x.ebx = phys_addr;
	regs.x.ecx = (size + (PAGE_SIZE-1)) >> PAGE_BITS;
		
	HostInterruptCall(0x21,&regs);
	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	*selp = sel;

	return BRE_OK;

#elif defined (__X32__)
	br_uint_32 base;

	/*
	 * Map memory
	 */
	regs.w.ax = 0x350a;
	regs.x.ebx = phys_addr;
	regs.x.ecx = size;

	HostInterruptCall(0x21,&regs);
	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;
	
	base = regs.x.ebx;

	/*
	 * Convert to linear address
	 */
	regs.w.ax = 0x3504;	
	HostSelectorDS(&regs.w.bx);
	HostInterruptCall(0x21,&regs);
	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	base -= regs.x.ecx;
	
	/*
	 * Create new selector
	 */
	return HostSelectorAllocateLinear(selp, base, size);

#else
	return BRE_FAIL;
#endif
}

/*
 * Release a selector allocated by HostSelectorAllocateXXX()
 */
br_error BR_RESIDENT_ENTRY HostSelectorFree(br_uint_16 sel)
{
	br_uint_16 rsel;

	/*
	 * If this is the REAL 1Mbyte segment, don't free it
	 */
	HostSelectorReal(&rsel);
	
	if(sel == rsel)
		return BRE_OK;

#if defined(__DPMI__)
	/*
	 * Free an LDT entry
	 */
	regs.w.ax = 0x0001;
	regs.w.cx = 1;
	regs.w.bx = sel;
	HostInterruptCall(0x31,&regs);

	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	return BRE_OK;
#elif defined(__PHARLAP386__) 

	/*
	 * Free segment
	 */
	regs.w.ax = 0x4900;
	regs.w.es = sel;
	HostInterruptCall(0x21,&regs);
	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	return BRE_OK;
#elif defined (__X32__)

	/*
	 * Delallocate protected mode selector
	 */
	regs.w.ax = 0x3502;
	regs.w.bx = sel;

	HostInterruptCall(0x21,&regs);
	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	return BRE_OK;
#else
	return BRE_FAIL;

#endif
}

/*
 * Create a new selector coresponding to a linear range of memory
 */
br_error BR_RESIDENT_ENTRY HostSelectorBaseSet(br_uint_16 sel, br_uint_32 base)
{
#if defined(__DPMI__)

	regs.w.ax = 0x0007;
	regs.w.bx = sel;

	regs.w.dx = base & 0xffff;
	regs.w.cx = (base >> 16) & 0xffff;

	HostInterruptCall(0x31,&regs);

	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	return BRE_OK;
#elif defined(__PHARLAP386__)

	struct ldt ldt;

	/*
	 * Rewrite LDT with new base
	 */
	regs.w.ax = 0x2531;
	regs.w.bx = sel;
	HostSelectorDS(&regs.w.ds);
	regs.x.edx = (br_uint_32) &ldt;
	regs.x.ecx = 0;

	HostInterruptCall(0x21,&regs);
	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	ldt.base_0 = base & 0xffff;
	ldt.base_1 = (base >> 16) & 0xff;
	ldt.base_2 = (base >> 24) & 0xff;

	regs.w.ax = 0x2531;
	regs.w.bx = sel;
	HostSelectorDS(&regs.w.ds);
	regs.x.edx = (br_uint_32) &ldt;
	regs.x.ecx = 1;

	HostInterruptCall(0x21,&regs);
	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	return BRE_OK;
#elif defined (__X32__)

	regs.w.ax = 0x3503;
	regs.w.bx = sel;
	regs.x.ecx = base;
	HostInterruptCall(0x21,&regs);
	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	return BRE_OK;
#else
	return BRE_FAIL;
#endif
}

/*
 * Set the limit for a selector
 */
br_error BR_RESIDENT_ENTRY HostSelectorLimitSet(br_uint_16 sel, br_size_t limit)
{
#if defined(__DPMI__)

	regs.w.ax = 0x0008;
	regs.w.bx = sel;

	if(limit >= BIG_LIMIT)
		limit |= PAGE_SIZE-1;

	regs.w.dx = limit & 0xffff;
	regs.w.cx = (limit >> 16) & 0xffff;

	HostInterruptCall(0x31,&regs);

	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	return BRE_OK;
#elif defined(__PHARLAP386__)

	struct ldt ldt;

	/*
	 * Rewrite LDT with new limit
	 */
	regs.w.ax = 0x2531;
	regs.w.bx = sel;
	HostSelectorDS(&regs.w.ds);
	regs.x.edx = (br_uint_32) &ldt;
	regs.x.ecx = 0;

	HostInterruptCall(0x21,&regs);
	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	if(limit >= BIG_LIMIT) {
		ldt.granularity = 1;
		limit = limit >> PAGE_BITS;
	} else  {
		ldt.granularity = 0;
	}
	
	ldt.limit_0 = limit & 0xffff;
	ldt.limit_1 = (limit >> 16) & 0xf;

	regs.w.ax = 0x2531;
	regs.w.bx = sel;
	HostSelectorDS(&regs.w.ds);
	regs.x.edx = (br_uint_32) &ldt;
	regs.x.ecx = 1;

	HostInterruptCall(0x21,&regs);
	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;
		
	return BRE_OK;
#elif defined (__X32__)

	if(limit >= BIG_LIMIT)
		limit |= PAGE_SIZE-1;

	regs.w.ax = 0x3505;
	regs.w.bx = sel;
	regs.x.ecx = limit;
	HostInterruptCall(0x21,&regs);
	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;
		
	return BRE_OK;
#else
	return BRE_FAIL;
#endif
}

br_error BR_RESIDENT_ENTRY HostSelectorBaseQuery(br_uint_32 *basep, br_uint_16 sel)
{
#if defined(__DPMI__)

	br_uint_32 base;

	/*
	 * Get base address of segment
	 */
	regs.w.ax = 0x0006;
	regs.w.bx = sel;

	HostInterruptCall(0x31,&regs);

	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	base = (regs.w.cx << 16) + regs.w.dx;		


	*basep = base;

	return BRE_OK;

#elif defined(__PHARLAP386__) || defined (__X32__)

	br_uint_32 base;

	regs.w.ax = 0x2508;
	regs.w.bx = sel;
	HostInterruptCall(0x21,&regs);

	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	base = regs.x.ecx;


	*basep = base;

	return BRE_OK;

#else

	return BRE_FAIL;

#endif
}

/*
 * Return the linear address coresponding to a given selector+offset
 */
br_error BR_RESIDENT_ENTRY HostSelectorLimitQuery(br_uint_32 *limitp, br_uint_16 sel)
{
#if defined(__DPMI__)

	br_uint_32 limit;
	struct ldt ldt;

	/*
	 * Get base address of segment
	 */
	regs.w.ax = 0x000b;
	regs.w.bx = sel;
	HostSelectorDS(&regs.w.es);
	regs.x.edi = (br_uint_32) &ldt;

	HostInterruptCall(0x31,&regs);

	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;


	limit = ldt.limit_0 + (ldt.limit_1 << 16);

	if(ldt.granularity)
		limit = (limit << PAGE_BITS) + (PAGE_SIZE-1);

	*limitp = limit;

	return BRE_OK;

#elif defined(__PHARLAP386__) || defined (__X32__)

	br_uint_32 limit;
	struct ldt ldt;

	/*
	 * Read LDT
	 */
	regs.w.ax = 0x2531;
	regs.w.bx = sel;
	HostSelectorDS(&regs.w.ds);
	regs.x.edx = (br_uint_32) &ldt;
	regs.x.ecx = 0;

	HostInterruptCall(0x21,&regs);
	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;


	limit = ldt.limit_0 + (ldt.limit_1 << 16);

	if(ldt.granularity)
		limit = (limit << PAGE_BITS) + (PAGE_SIZE-1);

	*limitp = limit;

	return BRE_OK;

#else

	return BRE_FAIL;

#endif
}

#if 0
/*
 * Return an offset from selector that can be used to address the specified linear address
 */
br_error BR_RESIDENT_ENTRY HostLinearToOffset(br_uint_32 *poffset, br_uint_32 linear, br_uint_16 sel)
{
	br_uint_32 base;
	br_error err;

	/*
	 * Set limit to 2GB
	 */	
	
#if defined(__DPMI__)

	regs.w.ax = 0x0008;
	regs.w.bx = sel;
	regs.w.cx = 0xffff;	
	regs.w.dx = 0xffff;	
	HostInterruptCall(0x31,&regs);

	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	/*
	 * Work out an offset that will get to the correct linear address
	 * (this may wrap around the end of the segment)
	 */
	err = HostOffsetToLinear(&base, 0, sel);
	if(err != BRE_OK)
		return err;

	*poffset = linear - base;
	return BRE_OK;

#elif defined(__PHARLAP386__) 
	struct ldt ldt;

	regs.w.ax = 0x2531;
	regs.w.bx = sel;
	HostSelectorDS(&regs.w.ds);
	regs.x.edx = (br_uint_32) &ldt;
	regs.x.ecx = 0;

	HostInterruptCall(0x21,&regs);
	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	ldt.granularity = 1;
	ldt.limit_0 = 0xffff;
	ldt.limit_1 = 0xf;

	regs.w.ax = 0x2531;
	regs.w.bx = sel;
	HostSelectorDS(&regs.w.ds);
	regs.x.edx = (br_uint_32) &ldt;
	regs.x.ecx = 1;

	HostInterruptCall(0x21,&regs);
	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

	/*
	 * Work out an offset that will get to the correct linear address
	 * (this may wrap around the end of the segment)
	 */
	err = HostOffsetToLinear(&base, 0, sel);
	if(err != BRE_OK)
		return err;

	*poffset = linear - base;
	return BRE_OK;

#elif defined (__X32__)

#if 0
	BrMemSet(&r,0,sizeof(&regs));

	regs.w.ax = 0x3505;
	regs.w.bx = sel;
	regs.x.ecx = 0xffffffff;

	HostInterruptCall(0x21,&regs);

	if(regs.w.flags & HOST_FLAG_CARRY)
		return BRE_FAIL;

#endif

	/*
	 * Work out an offset that will get to the correct linear address
	 * (this may wrap around the end of the segment)
	 */
	err = HostOffsetToLinear(&base, 0, sel);
	if(err != BRE_OK)
		return err;

	*poffset = linear - base;
	return BRE_OK;

#else
	return BRE_FAIL;
#endif

}
#endif
