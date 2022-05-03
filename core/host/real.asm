;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: real.asm 1.1 1997/12/10 16:41:14 jon Exp $
;; $Locker: $
;;
;; Support routines for digging around with real mode and interrupts
;;
	.386p
	.model flat,c

ifdef __DPMI__
	include dpmi.inc
else
	include pl.inc
endif
	include host.inc

if ADD_RCS_ID
	.data
		db '$Id: real.asm 1.1 1997/12/10 16:41:14 jon Exp $',0
		align 4
endif
	.code

; br_error BR_RESIDENT_ENTRY HostRealAllocate(struct host_real_memory *mem, br_uint_32 size);
;
HostRealAllocate proc uses ebx ecx, drm: ptr host_real_memory, msize:dword

		mov	ebx,msize
		add	ebx,15
		shr	ebx,4

ifdef __GO32__
		mov	eax,0100h		; Allocate DOS Memory
		int	031h			; DPMI
		jc	error

		mov	ebx,drm
		mov	[ebx].host_real_memory.rm_seg,ax
		shl	eax,4
		mov	[ebx].host_real_memory.pm_off,eax

	extern _go32_conventional_mem_selector:proc
		call	_go32_conventional_mem_selector
		mov	[ebx].host_real_memory.pm_seg,ax

		xor	eax,eax
		mov	[ebx].host_real_memory.rm_off,ax

elseifdef __DPMI__
		mov	eax,0100h		; Allocate DOS Memory
		int	031h			; DPMI
		jc	error

		mov	ebx,drm
		mov	[ebx].host_real_memory.rm_seg,ax
		mov	[ebx].host_real_memory.pm_seg,dx

		xor	eax,eax
		mov	[ebx].host_real_memory.pm_off,eax
		mov	[ebx].host_real_memory.rm_off,ax

elseifdef __PHARLAP386__

		mov	eax,25C0h		; Allocate Real Memory
		int	021h
		jc	error

		mov	ebx,drm
		mov	[ebx].host_real_memory.rm_seg,ax

		shl	eax,4
		mov	[ebx].host_real_memory.pm_off,eax

		mov	ax,cs			; Get RPL from CS
		and	ax,3
		or	ax,034h			; OR into hardwired LDT selector
		mov	[ebx].host_real_memory.pm_seg,ax

		xor	eax,eax
		mov	[ebx].host_real_memory.rm_off,ax
elseifdef __X32__

		mov	eax,4800h		; Allocate Real Memory
		int	021h
		jc	error

		mov	ecx,drm
		mov	[ecx].host_real_memory.rm_seg,ax
		xor	ebx,ebx
		mov	[ecx].host_real_memory.rm_off,bx

		mov	bx,_x386_zero_base_selector
		mov	[ecx].host_real_memory.pm_seg,bx

		shl	eax,4
		mov	[ecx].host_real_memory.pm_off,eax
endif
		xor	eax,eax
		ret
error:
		mov	eax,BRE_FAIL
		ret

HostRealAllocate endp

; br_error BR_RESIDENT_ENTRY HostRealFree(struct host_real_memory *mem);
;
HostRealFree proc uses ebx, drm: ptr host_real_memory

ifdef __GO32__
	; XXX
	;
		ret
elseifdef __DPMI__
		xor	edx,edx
		mov	ebx,drm
		mov	dx,[ebx].host_real_memory.pm_seg
		mov	eax,0101h		; Free DOS Memory
		int	31h			; DPMI

elseifdef __PHARLAP386__

		xor	ecx,ecx
		mov	ebx,drm
		mov	cx,[ebx].host_real_memory.rm_seg
		mov	eax,25C1h
		int	21h

elseifdef __X32__
		; X-32 cannot release real-mode memory!
endif
		xor	eax,eax
		ret
HostRealFree endp

; br_error BR_RESIDENT_ENTRY HostRealInterruptGet(br_uint_8 vector, br_uint_16 *offp, br_uint_16 *vsegp);
;
; 
HostRealInterruptGet proc uses ebx, vector: dword, voffp:ptr word, vsegp:ptr word
ifdef __DPMI__
		xor	ebx,ebx
		mov	bl,byte ptr vector
		mov	eax,0200h		; Get Real Mode Interrupt Vector
		int	31h			; DPMI
		mov	ebx,vsegp
		mov	[ebx],cx
		mov	ebx,voffp
		mov	[ebx],dx

else ; PHARLAP or X-32
		xor	ecx,ecx
		mov	cl,byte ptr vector
		mov	eax,2503h		; Get Real Mode Interrupt Vector
		int	21h
		mov	eax,voffp
		mov	[eax],bx
		shr	ebx,16
		mov	eax,vsegp
		mov	[eax],bx
endif
		xor	eax,eax
		ret
HostRealInterruptGet endp

; br_error BR_RESIDENT_ENTRY HostRealInterruptSet(br_uint_8 vector, br_uint_16 off, br_uint_16 vseg);
;
HostRealInterruptSet proc uses ebx, vector: dword, voff:dword, vseg:dword

ifdef __DPMI__
		xor	ebx,ebx
		mov	bl,byte ptr vector
		mov	cx,word ptr vseg
		mov	dx,word ptr voff
		mov	eax,0201h		; Set Real Mode Interrupt Vector
		int	31h			; DPMI

else ; PHARLAP or X-32
		mov	cl,byte ptr vector
		mov	bx,word ptr vseg
		shl	ebx,16
		mov	bx,word ptr voff
		mov	eax,02505h		; Set Real Mode Interrupt Vector
		int	21h
endif
		xor	eax,eax
		ret
HostRealInterruptSet endp

; br_error BR_RESIDENT_ENTRY HostRealInterruptCall(br_uint_8 vector, host_regs *regs);
;
; Invokes an interrupt in real mode
;
HostRealInterruptCall proc uses ebx esi edi es, vec:dword, regs: ptr host_regs

ifdef __DPMI__

	; Use DPMI to call vector
	;
	;  - Registers are in correct format
	;
		mov	ax,ds
		mov	es,ax
		mov	edi,regs
		xor	ebx,ebx
		mov	[edi].dpmi_registers.rss,bx	; NULL Stack pointer
		mov	[edi].dpmi_registers.rsp,bx
		mov	bl, byte ptr vec
		xor	ecx,ecx
		mov	eax,300h	    		; Simulate Real Mode Interrupt
		int	31h				; DPMI

else ; PHARLAP or X-32
	; Transcribe input registers
	;
		mov	bx, word ptr vec
		mov	real_regs.interrupt,bx

		push	ebp
		mov	edx,regs

		mov	edi,[edx].host_regs.x._edi
		mov	esi,[edx].host_regs.x._esi
		mov	ebp,[edx].host_regs.x._ebp
		mov	ecx,[edx].host_regs.x._ecx
		mov	ebx,[edx].host_regs.x._ebx

		mov	eax,[edx].host_regs.x._eax
		mov	real_regs.reax,eax
		mov	eax,[edx].host_regs.x._edx
		mov	real_regs.redx,eax

		mov	ax,[edx].host_regs.w._ds
		mov	real_regs.rds,ax
		mov	ax,[edx].host_regs.w._es
		mov	real_regs.res,ax
		mov	ax,[edx].host_regs.w._fs
		mov	real_regs.rfs,ax
		mov	ax,[edx].host_regs.w._gs
		mov	real_regs.rgs,ax

		push	edx
		mov	eax,2511h
		mov	edx,offset real_regs
		int	21h
		mov	real_regs.reax,eax
		pushf
		pop	ax
		pop	edx

		mov	[edx].host_regs.w.flags,ax

		mov	[edx].host_regs.x._edi,edi
		mov	[edx].host_regs.x._esi,esi
		mov	[edx].host_regs.x._ebp,ebp
		mov	[edx].host_regs.x._ecx,ecx
		mov	[edx].host_regs.x._ebx,ebx

		mov	eax,real_regs.reax
		mov	[edx].host_regs.x._eax,eax
		mov	eax,real_regs.redx
		mov	[edx].host_regs.x._edx,eax

		mov	ax,real_regs.rds
		mov	[edx].host_regs.w._ds,ax
		mov	ax,real_regs.res
		mov	[edx].host_regs.w._es,ax
		mov	ax,real_regs.rfs
		mov	[edx].host_regs.w._fs,ax
		mov	ax,real_regs.rgs
		mov	[edx].host_regs.w._gs,ax

		pop	ebp
endif
		xor	eax,eax
		ret
HostRealInterruptCall endp

if 0
; br_error BR_RESIDENT_ENTRY HostRealFarCall(br_uint_16 seg, br_uint_16 off, host_regs *ri, host_regs *ro, host_sregs *sr);
;
; Call a rea mode procedure that returns with RETF
;
HostRealFarCall proc uses ebx edi, _seg:dword, _off:dword, ri:dword, ro:dword, sr:dword
error:		mov	eax,BRE_FAIL
		ret
HostRealFarCall endp
endif

; br_error BR_ASM_CALL RealSelectorBegin(void);
;
; Create a public selector to the real mode 1M
;
RealSelectorBegin proc uses ebx

		cmp	_RealSelector,0
		je	not_active
		mov	eax, BRE_ALLREADY_ACTIVE	
		ret
not_active:

ifdef __DOS4G__
		mov	ax,ds
elseifdef __GO32__

	extern _go32_conventional_mem_selector:proc
		call	_go32_conventional_mem_selector

elseifdef __DPMI__

	; Use DPMI services to create a selector
	; Set the base to zero, and limit to 4Gbytes
	;

		mov	ecx,1
		mov	eax,0
		int	31h
		jc	error
		mov	bx,ax

		xor	ecx,ecx
		xor	edx,edx
		mov	eax,7
		int	31h
		jc	error

		dec	ecx
		dec	edx
		mov	eax,8
		int	31h
		jc	error
	
		mov	ax,bx

elseifdef __PHARLAP386__

		mov	ax,cs			; Get RPL from CS
		and	ax,3
		or	ax,034h			; OR into hardwired LDT selector

elseifdef __X32__

		mov	ax,_x386_zero_base_selector
endif
		mov	_RealSelector,ax

		xor	eax,eax
		ret

error:		mov	eax,BRE_FAIL
		ret
RealSelectorBegin endp

; void BR_ASM_CALL RealSelectorEnd(void);
;
RealSelectorEnd proc

ifdef __DOS4G__
		; NOTHING
elseifdef __DPMI__
		mov	bx,_RealSelector
		mov	eax,1
		int	31h
		xor	eax,eax
		mov	_RealSelector,0

elseifdef __PHARLAP386__
		; NOTHING
elseifdef __X32__
		; NOTHING
endif
		ret 
RealSelectorEnd endp

; void BR_ASM_CALL HostSelectorReal(br_uint_16 *selp);
;
HostSelectorReal proc , selp: ptr word
		mov	eax,selp
		mov	dx,_RealSelector
		mov	[eax],dx
		xor	eax,eax
		ret
HostSelectorReal endp

	.data

ifndef __DPMI__
real_regs	pl_registers	<,>
endif

ifdef __X32__
		extern _x386_zero_base_selector:word
endif

	public _RealSelector

_RealSelector	dw	0
	end

