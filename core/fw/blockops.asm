;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: blockops.asm 1.1 1997/12/10 16:41:03 jon Exp $
;; $Locker: $
;;
;; Various operations on fixed 16.16 numbers
;;
	.386p
	.model flat,c
	.data
	db '$Id: blockops.asm 1.1 1997/12/10 16:41:03 jon Exp $',0
	align	4
	.code

; void BR_ASM_CALL BrBlockFill(void *dest_ptr, int value, int dwords);
;
BrBlockFill proc uses eax ecx edi es, dest:ptr dword,  value:dword, count:dword
		mov	ax,ds
		mov	es,ax
		mov	eax,value
		mov	ecx,count
		mov	edi,dest
		rep	stosd
		ret
BrBlockFill endp

; void BR_ASM_CALL BrBlockCopy(void *dest_ptr, void *src_ptr, int dwords);
;
BrBlockCopy proc uses ecx esi edi es, dest:ptr dword, src:ptr dword, count:dword
		mov	ax,ds
		mov	es,ax
		mov	esi,src
		mov	ecx,count
		mov	edi,dest
		rep	movsd
		ret
BrBlockCopy endp

; void BR_ASM_CALL BrFarBlockCopy(void __far *dest_ptr, void *src_ptr, int dwords);
;
BrFarBlockCopy proc uses ecx esi edi es, dest:far ptr dword, src:ptr dword, count:dword
		mov	ecx,count
		mov	esi,src
		les	edi,dest
		rep	movsd
		ret
BrFarBlockCopy endp

	end

	
