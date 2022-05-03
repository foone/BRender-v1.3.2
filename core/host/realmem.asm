;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: realmem.asm 1.1 1997/12/10 16:41:14 jon Exp $
;; $Locker: $
;;
;; Support routines for accessing real memory
;;
	.386p
	.model flat,c

	.data
	extern _RealSelector:word

	.code

; void BR_RESIDENT_ENTRY HostRealBlockWrite(short offset, short seg, char *block, int count);
;
HostRealBlockWrite proc uses es esi edi, p_off:dword, p_seg:dword, block:dword, count:dword

	mov	es,_RealSelector
	mov	edi,p_seg
	shl	edi,4
	add	edi,p_off

	mov	esi,block
	mov	ecx,count
	shr	ecx,1
	rep	movsw
	adc	ecx,ecx
	rep	movsb
	ret
HostRealBlockWrite endp

; void BR_RESIDENT_ENTRY HostRealBlockFill(short offset, short seg, unsigned byte value, int count);
;
HostRealBlockFill proc uses es esi edi, p_off:dword, p_seg:dword, value:dword, count:dword

	mov	es,_RealSelector
	mov	edi,p_seg
	shl	edi,4
	add	edi,p_off

	mov	al,byte ptr value
	mov	ecx,count
	mov	ah,al
	shr	ecx,1
	rep	stosw
	adc	ecx,ecx
	rep	stosb
	ret
HostRealBlockFill endp

; void BR_RESIDENT_ENTRY HostRealBlockRead(short offset, short seg, char *block, int count);
;
HostRealBlockRead proc uses ds esi edi, p_off:dword, p_seg:dword, block:dword, count:dword

	mov	ds,_RealSelector
	mov	esi,p_seg
	shl	esi,4
	add	esi,p_off

	mov	edi,block
	mov	ecx,count
	shr	ecx,1
	rep	movsw
	adc	ecx,ecx
	rep	movsb
	ret
HostRealBlockRead endp

; int BR_RESIDENT_ENTRY HostRealStringWrite(short offset, short seg, char *block, int max);
;
HostRealStringWrite proc uses es esi edi, p_off:dword, p_seg:dword, block:dword, count:dword

	mov	es,_RealSelector
	mov	edi,p_seg
	shl	edi,4
	add	edi,p_off

	mov	esi,block
	mov	ecx,count
	xor	eax,eax
	and	ecx,ecx
	je	exit

lp:	mov	dl,[esi]
	mov	es:[edi],dl
	inc	eax
	and	dl,dl
	je	exit
	inc	esi
	inc	edi
	dec	ecx
	jne	lp

	mov	byte ptr es:[edi-1],0
exit:
	ret

HostRealStringWrite endp

; int BR_RESIDENT_ENTRY HostRealStringRead(short offset, short seg, char *block, int max);
;
HostRealStringRead proc uses es esi edi, p_off:dword, p_seg:dword, block:dword, count:dword

	mov	es,_RealSelector
	mov	edi,p_seg
	shl	edi,4
	add	edi,p_off

	mov	esi,block
	mov	ecx,count
	xor	eax,eax
	and	ecx,ecx
	je	exit

lp:
	mov	dl,es:[edi]
	mov	[esi],dl
	inc	eax
	and	dl,dl
	je	exit
	inc	esi
	inc	edi
	dec	ecx
	jne	lp

	mov	byte ptr [esi-1],0
exit:
	ret
HostRealStringRead endp


; void BR_RESIDENT_ENTRY HostRealByteWrite(short offset, short seg, char value);
;
HostRealByteWrite proc uses ds, p_off:dword, p_seg:dword, value:dword

	mov	ds,_RealSelector
	mov	edx,p_seg
	shl	edx,4
	add	edx,p_off

	mov	al,byte ptr value
	mov	[edx],al
	ret
HostRealByteWrite endp

; unsigned char BR_RESIDENT_ENTRY HostRealByteRead(short offset, short seg);
;
HostRealByteRead proc uses ds, p_off:dword, p_seg:dword

	mov	ds,_RealSelector
	mov	eax,p_seg
	shl	eax,4
	add	eax,p_off

	mov	al,[eax]
	ret
HostRealByteRead endp

; void BR_RESIDENT_ENTRY HostRealWordWrite(short offset, short seg, unsigned short value);
;
HostRealWordWrite proc uses ds, p_off:dword, p_seg:dword, value:dword

	mov	ds,_RealSelector
	mov	edx,p_seg
	shl	edx,4
	add	edx,p_off

	mov	ax,word ptr value
	mov	[edx],ax
	ret
HostRealWordWrite endp

; unsigned short BR_RESIDENT_ENTRY HostRealWordRead(short offset, short seg);
;
HostRealWordRead proc uses ds, p_off:dword, p_seg:dword

	mov	ds,_RealSelector
	mov	eax,p_seg
	shl	eax,4
	add	eax,p_off

	mov	ax,[eax]
	ret
HostRealWordRead endp

; void BR_RESIDENT_ENTRY HostRealDWordWrite(short offset, short seg, unsigned long value);
;
HostRealDWordWrite proc uses ds, p_off:dword, p_seg:dword, value:dword

	mov	ds,_RealSelector
	mov	edx,p_seg
	shl	edx,4
	add	edx,p_off

	mov	eax,value
	mov	[edx],eax
	ret
HostRealDWordWrite endp

; unsigned long BR_RESIDENT_ENTRY HostRealDWordRead(short offset, short seg);
;
HostRealDWordRead proc uses ds, p_off:dword, p_seg:dword

	mov	ds,_RealSelector
	mov	edx,p_seg
	shl	edx,4
	add	edx,p_off

	mov	eax,p_seg
	shl	eax,4
	add	eax,p_off

	mov	eax,[eax]
	ret
HostRealDWordRead endp

	end

