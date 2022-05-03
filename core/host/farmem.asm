;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: farmem.asm 1.1 1997/12/10 16:41:12 jon Exp $
;; $Locker: $
;;
;; Support routines for accessing far memory
;;
	.386p
	.model flat,c

	.code

; void BR_RESIDENT_ENTRY HostFarBlockWrite(int offset, short seg, char *block, int count);
;
HostFarBlockWrite proc uses es esi edi, p_off:dword, p_seg:dword, block:dword, count:dword

	les	edi,fword ptr p_off
	mov	esi,block
	mov	ecx,count
	shr	ecx,1
	rep	movsw
	adc	ecx,ecx
	rep	movsb
	ret
HostFarBlockWrite endp

; void BR_RESIDENT_ENTRY HostFarBlockFill(int offset, short seg, unsigned byte value, int count);
;
HostFarBlockFill proc uses es esi edi, p_off:dword, p_seg:dword, value:dword, count:dword

	les	edi,fword ptr p_off
	mov	al,byte ptr value
	mov	ecx,count
	mov	ah,al
	shr	ecx,1
	rep	stosw
	adc	ecx,ecx
	rep	stosb
	ret
HostFarBlockFill endp

; void BR_RESIDENT_ENTRY HostFarBlockRead(int offset, short seg, char *block, int count);
;
HostFarBlockRead proc uses ds esi edi, p_off:dword, p_seg:dword, block:dword, count:dword

	lds	esi,fword ptr p_off
	mov	edi,block
	mov	ecx,count
	shr	ecx,1
	rep	movsw
	adc	ecx,ecx
	rep	movsb
	ret
HostFarBlockRead endp

; int BR_RESIDENT_ENTRY HostFarStringWrite(int offset, short seg, char *block, int max);
;
HostFarStringWrite proc uses es esi edi, p_off:dword, p_seg:dword, block:dword, count:dword

	les	edi,fword ptr p_off
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

HostFarStringWrite endp

; int BR_RESIDENT_ENTRY HostFarStringRead(int offset, short seg, char *block, int max);
;
HostFarStringRead proc uses es esi edi, p_off:dword, p_seg:dword, block:dword, count:dword

	les	edi,fword ptr p_off
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
HostFarStringRead endp


; void BR_RESIDENT_ENTRY HostFarByteWrite(int offset, short seg, char value);
;
HostFarByteWrite proc uses ds, p_off:dword, p_seg:dword, value:dword

	lds	edx,fword ptr p_off
	mov	al,byte ptr value
	mov	[edx],al
	ret
HostFarByteWrite endp

; unsigned char BR_RESIDENT_ENTRY HostFarByteRead(int offset, short seg);
;
HostFarByteRead proc uses ds, p_off:dword, p_seg:dword

	lds	eax,fword ptr p_off
	mov	al,[eax]
	ret
HostFarByteRead endp

; void BR_RESIDENT_ENTRY HostFarWordWrite(int offset, short seg, unsigned short value);
;
HostFarWordWrite proc uses ds, p_off:dword, p_seg:dword, value:dword

	lds	edx,fword ptr p_off
	mov	ax,word ptr value
	mov	[edx],ax
	ret
HostFarWordWrite endp

; unsigned short BR_RESIDENT_ENTRY HostFarWordRead(int offset, short seg);
;
HostFarWordRead proc uses ds, p_off:dword, p_seg:dword

	lds	eax,fword ptr p_off
	mov	ax,[eax]
	ret
HostFarWordRead endp

; void BR_RESIDENT_ENTRY HostFarDWordWrite(int offset, short seg, unsigned long value);
;
HostFarDWordWrite proc uses ds, p_off:dword, p_seg:dword, value:dword

	lds	edx,fword ptr p_off
	mov	eax,value
	mov	[edx],eax
	ret
HostFarDWordWrite endp

; unsigned long BR_RESIDENT_ENTRY HostFarDWordRead(int offset, short seg);
;
HostFarDWordRead proc uses ds, p_off:dword, p_seg:dword

	lds	eax,fword ptr p_off
	mov	eax,[eax]
	ret
HostFarDWordRead endp

	end

