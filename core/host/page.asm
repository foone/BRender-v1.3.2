;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: page.asm 1.1 1997/12/10 16:41:14 jon Exp $
;; $Locker: $
;;
;; Support routines for PMMU things - locking pages and mapping devices
;;
	.386p
	.model flat,c
	include host.inc


if ADD_RCS_ID
	.data
		db '$Id: page.asm 1.1 1997/12/10 16:41:14 jon Exp $',0
		align 4
endif
	.code

; br_error BR_RESIDENT_ENTRY HostPhysicalMap(br_uint_16 *selp, br_uint_32 *offsetp, br_uint_32 phys_addr, br_size_t size);
;
;
HostPhysicalMap	proc
		mov	eax,BRE_FAIL
		ret
HostPhysicalMap	endp

; br_error BR_RESIDENT_ENTRY HostLock(br_uint_16 sel, br_uint_32 offset, br_size_t size);
;
;
HostLock		proc
		mov	eax,BRE_FAIL
		ret
HostLock		endp

; br_error BR_RESIDENT_ENTRY HostUnlock(br_uint_16 sel, br_uint_32 offset, br_size_t size);
;
;
HostUnlock	proc
		mov	eax,BRE_FAIL
		ret
HostUnlock	endp


	end
