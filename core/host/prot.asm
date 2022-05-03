;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: prot.asm 1.1 1997/12/10 16:41:14 jon Exp $
;; $Locker: $
;;
;; Support routines for digging around with protected mode
;;
;; XXX Add thunks for non-DPMI exception handler
;;
	.386p
	.model flat,c
	include host.inc

if ADD_RCS_ID
	.data
		db '$Id: prot.asm 1.1 1997/12/10 16:41:14 jon Exp $',0
		align 4
endif
	.code

; br_error BR_RESIDENT_ENTRY HostInterruptGet(br_uint_8 vector, br_uint_32 *offp, br_uint_16 *selp);
;
HostInterruptGet	proc uses ebx es, vector: dword, voffp:ptr dword, vsegp:ptr word
	ifdef __DPMI__
		xor	ebx,ebx
		mov	bl,byte ptr vector
		mov	eax,0204h		; Get Protected Mode Interrupt Vector
		int	31h			; DPMI
		mov	ebx,vsegp
		mov	[ebx],cx
		mov	ebx,voffp
		mov	[ebx],edx

	else ; PHARLAP or X-32
		xor	ecx,ecx
		mov	cl,byte ptr vector
		mov	eax,02502h
		int	21h
		mov	eax,vsegp
		mov	[eax],es
		mov	eax,voffp
		mov	[eax],ebx
	endif
		xor	eax,eax
		ret
HostInterruptGet	endp

; br_error BR_RESIDENT_ENTRY HostInterruptSet(br_uint_8 vector, br_uint_32 off, br_uint_16 sel);
;
;
HostInterruptSet	proc uses ebx ds, vector: dword, voff:dword, vseg:dword
	ifdef __DPMI__
		xor	ebx,ebx
		mov	bl,byte ptr vector
		mov	cx,word ptr vseg
		mov	edx,voff
		mov	eax,0205h		; Set Protected Mode Interrupt Vector
		int	31h			; DPMI
	else ; PHARLAP or X-32
		mov	cl,byte ptr vector
		mov	edx,voff
		mov	ds,word ptr vseg
		mov	eax,02504h
		int	21h
	endif
		xor	eax,eax
		ret
HostInterruptSet	endp


; br_error BR_RESIDENT_ENTRY HostExceptionGet(br_uint_8 exception, br_uint_32 *offp, br_uint_16 *selp);
;
;
HostExceptionGet	proc uses ebx, vector: dword, voffp:ptr dword, vsegp:ptr word
	ifdef __DPMI__
		xor	ebx,ebx
		mov	bl,byte ptr vector
		mov	eax,0202h		; Get Exception Handler Vector
		int	31h			; DPMI
		mov	ebx,vsegp
		mov	[ebx],cx
		mov	ebx,voffp
		mov	[ebx],edx
	else ; PHARLAP or X-32
		xor	ecx,ecx
		mov	cl,byte ptr vector
		mov	eax,02532h
		int	21h
		mov	eax,vsegp
		mov	[eax],es
		mov	eax,voffp
		mov	[ebx],eax
	endif
		xor	eax,eax
		ret
HostExceptionGet	endp

; br_error BR_RESIDENT_ENTRY HostExceptionSet(br_uint_8 exception, br_uint_32 off, br_uint_16 sel);
;
;
HostExceptionSet	proc uses ebx, vector: dword, voff:dword, vseg:dword
	ifdef __DPMI__
		xor	ebx,ebx
		mov	bl,byte ptr vector
		mov	cx,word ptr vseg
		mov	edx,voff
		mov	eax,0203h		; Set Exception Handler Vector
		int	31h			; DPMI
	else ; PHARLAP or X-32
		mov	cl,byte ptr vector
		mov	edx,voff
		mov	ds,word ptr vseg
		mov	eax,02533h
		int	21h
	endif
		xor	eax,eax
		ret
HostExceptionSet	endp

; br_error BR_RESIDENT_ENTRY HostInterruptCall(br_uint_8 vector, union host_regs *regs);
;
; Invokes the given software interrupt. The registers are set up accoring to the contents
; of regs (except cs:eip and ss:esp)
;
; The state after the interrupt is written back into the same place
;

; Build local varibales explicitly (to avoid masm's use of ebp)
;
hic_local	struct
thunk		dd	?
out_ds		dw	?
save_ds		dw	?
save_edi	dd	?
save_ebp	dd	?
hic_local	ends

HostInterruptCall proc uses ebx esi edi es fs gs, vec:dword, regs: ptr host_regs

	; Set up local workspace on stack
	;
		sub	esp,sizeof hic_local

	; Put a thunk on stack to invoke the correct interrupt
	;
		mov	[esp].hic_local.thunk, 00c300cdh		; INT 0, RET
		mov	al,byte ptr vec
		mov	byte ptr [esp].hic_local.thunk+1,al

	; Set up all the registers and call the thunk
	;
		mov	edi,regs

		mov	[esp].hic_local.save_ds,ds
		mov	[esp].hic_local.save_edi,edi
		mov	[esp].hic_local.save_ebp,ebp

	; Push return address and thunk ptr.
	;
		lea	eax,[esp].hic_local.thunk
		push	return_here
		push	eax

		mov	es,[edi].host_regs.w._ds
		push	es

		mov	es,[edi].host_regs.w._es
		mov	fs,[edi].host_regs.w._fs
		mov	gs,[edi].host_regs.w._gs

		mov	eax,[edi].host_regs.x._eax
		mov	ebx,[edi].host_regs.x._ebx
		mov	ecx,[edi].host_regs.x._ecx
		mov	edx,[edi].host_regs.x._edx
		mov	ebp,[edi].host_regs.x._ebp
		mov	esi,[edi].host_regs.x._esi
		mov	edi,[edi].host_regs.x._edi

		pop	ds

		db	0c3h ;RET					; Calls thunk and returns

return_here:								; to here

	; Copy all the registers back
	;
		mov	[esp].hic_local.out_ds,ds
		mov	ds,[esp].hic_local.save_ds
		xchg	[esp].hic_local.save_ebp,ebp
		xchg	[esp].hic_local.save_edi,edi

		pushf

		mov	[edi].host_regs.x._eax,eax
		mov	[edi].host_regs.x._ebx,ebx
		mov	[edi].host_regs.x._ecx,ecx
		mov	[edi].host_regs.x._edx,edx
		mov	[edi].host_regs.x._esi,esi

		pop	ax
		mov	[edi].host_regs.w.flags,ax

		mov	ax,[esp].hic_local.out_ds
		mov	[edi].host_regs.w._ds,ax
		mov	[edi].host_regs.w._es,es
		mov	[edi].host_regs.w._fs,fs
		mov	[edi].host_regs.w._gs,gs

		mov	eax,[esp].hic_local.save_ebp
		mov	[edi].host_regs.x._ebp,eax
		mov	eax,[esp].hic_local.save_edi
		mov	[edi].host_regs.x._edi,eax

		add	esp,sizeof hic_local

		xor	eax,eax
		ret
HostInterruptCall endp

; br_error BR_RESIDENT_ENTRY HostRegistersGet(union host_regs *regs);
;
HostRegistersGet proc
		push	edi
		mov	edi,[esp+12]

		pushf
		pop	[edi].host_regs.w.flags

		mov	[edi].host_regs.x._eax,eax
		mov	[edi].host_regs.x._ebx,ebx
		mov	[edi].host_regs.x._ecx,ecx
		mov	[edi].host_regs.x._edx,edx
		mov	[edi].host_regs.x._ebp,ebp
		mov	[edi].host_regs.x._esi,esi

		mov	[edi].host_regs.w._ds,ds
		mov	[edi].host_regs.w._es,es
		mov	[edi].host_regs.w._fs,fs
		mov	[edi].host_regs.w._gs,gs
		mov	[edi].host_regs.w._ss,ss
		mov	[edi].host_regs.w._cs,cs

		mov	eax,[esp+8]
		mov	[edi].host_regs.x._edi,eax

		add	esp,4
		xor	eax,eax
		ret
HostRegistersGet endp

; br_error HostSelectorDS(br_uint_16 *psel)
;
HostSelectorDS proc near , selp: ptr word
		mov	eax,selp
		mov	word ptr [eax],ds
		xor	eax,eax
		ret
HostSelectorDS endp

; br_error HostSelectorCS(br_uint_16 *psel)
;
HostSelectorCS proc near , selp: ptr word
		mov	eax,selp
		mov	word ptr [eax],cs
		xor	eax,eax
		ret
HostSelectorCS endp

; br_error HostSelectorSS(br_uint_16 *psel)
;
HostSelectorSS proc near , selp: ptr word
		mov	eax,selp
		mov	word ptr [eax],ss
		xor	eax,eax
		ret
HostSelectorSS endp

; br_error HostSelectorES(br_uint_16 *psel)
;
HostSelectorES proc near , selp: ptr word
		mov	eax,selp
		mov	word ptr [eax],es
		xor	eax,eax
		ret
HostSelectorES endp

	end
