;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: cpuid.asm 1.3 1998/08/18 10:32:49 johng Exp $
;; $Locker: $
;;
;; Inquiry to find processor type and features
;;
;; Based on Intel code
;;
				.586
				.model   small,c

				include host.inc

				.code

CPUInfo			proc	uses ebx esi edi, cpu_type:ptr dword, features: ptr dword

	; Assume 386 as worst case
	;
				mov		esi,features
				mov		dword ptr [esi],0

				mov		esi,cpu_type
				mov		dword ptr [esi],BRT_INTEL_386

				pushfd                                ; push original EFLAGS
				pop		eax                           ; get original EFLAGS
				mov		ecx, eax                      ; save original EFLAGS
				xor		eax, 40000h                   ; flip AC bit in EFLAGS
				push	eax                           ; save new EFLAGS value on stack
				popfd                                 ; replace current EFLAGS value
				pushfd                                ; get new EFLAGS
				pop		eax                           ; store new EFLAGS in EAX
				xor		eax, ecx                      ; can't toggle AC bit, processor=80386
				jz		quit			              ; jump if 80386 processor

				push	ecx
				popfd                                 ; restore AC bit in EFLAGS first

	; Intel486 processor check
	; Checking for ability to set/clear ID flag (Bit 21) in EFLAGS
	; which indicates the presence of a processor with the CPUID
	; instruction.
				mov		esi,cpu_type
				mov		dword ptr [esi],BRT_INTEL_486

				mov		eax, ecx                      ; get original EFLAGS
				xor		eax, 200000h                  ; flip ID bit in EFLAGS
				push	eax                           ; save new EFLAGS value on stack
				popfd                                 ; replace current EFLAGS value
				pushfd                                ; get new EFLAGS
				pop		eax                           ; store new EFLAGS in EAX
				xor		eax, ecx                      ; can't toggle ID bit,
				je		quit                          ; processor=80486

; Execute CPUID instruction to determine vendor, family,
; model, stepping and features. For the purpose of this
; code, only the initial set of CPUID information is saved.
;
				mov		eax, 0                        ; set up for CPUID instruction
				cpuid
							                          ; get and save vendor ID

				cmp		eax, 1                        ; make sure 1 is valid input for CPUID
				jl		quit

                mov     eax, 1
				cpuid			                      ; get family/model/stepping/features


				shr		eax, 8							  ; isolate family
				and		eax, 0fh

                                cmp             eax,(cpu_types_end-cpu_types)/sizeof(dword)
                                jl              known_cpu                                

				mov		eax,6							  ; treat any future cpus as p-pros.
known_cpu:

				lea		ebx,cpu_types				; Convert to token
				mov		eax,[ebx+4*eax]
				mov		esi,cpu_type
				mov		[esi],eax

				xor		eax,eax

				test	edx,1
				je		no_fpu
				or		eax,HOST_CAPS_FPU
no_fpu:

				test	edx,(1 shl 23)
				je		no_mmx
				or		eax,HOST_CAPS_MMX

no_mmx:
                test    edx,(1 shl 15)
                je      no_cmov
                or      eax,HOST_CAPS_CMOV

no_cmov:
				mov		esi,features
				mov		[esi],eax

; check to see if the processor is a cyrix, mask out cmov if it is
; a small number of older cyrix processsors have a bug relating to their
; implementation of new floating point instructions indicated by cmov bit. 

				mov		eax,00000000
				cpuid                  ; cpuid w/eax=0, gets the VendorID
				cmp		ebx,069727943h    ; "Cyri"
				jnz		NotCyrix
				cmp		edx,0736e4978h    ; "xins"
				jnz		NotCyrix
				cmp		ecx,064616574h    ; "tead"
				jnz		NotCyrix
				mov		eax,[esi]
				and		eax,NOT HOST_CAPS_CMOV   ; clear CMOV bit
				mov		[esi],eax
NotCyrix:

quit:			ret


CPUInfo			endp


.data

cpu_types       dword   BRT_INTEL_386           ; 0
				dword	BRT_INTEL_386			; 1
				dword	BRT_INTEL_386			; 2
				dword	BRT_INTEL_386			; 3
				dword	BRT_INTEL_486			; 4
				dword	BRT_INTEL_PENTIUM		; 5
                                dword   BRT_INTEL_PENTIUM_PRO           ; 6
cpu_types_end                   dword   0 

				end
