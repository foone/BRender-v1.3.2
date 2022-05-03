;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: t15_pip.asm 1.1 1997/12/10 16:48:57 jon Exp $
;; $Locker: $
;;
;; Scanline loop for perpsective correct texture mapping
;;
		.386p
		.model	c,flat
		include	drv.inc
		.data

z		dd	?
i		dd	?
du_numerator	dd	?
dv_numerator	dd	?
u_numerator	dd	?
v_numerator	dd	?
zdest		dd	?
dest		dd	?
dest2		dd	?
source		dd	?
denominator	dd	?
dz		dd	?

		align 4

		.code


; Smoothly lit perspective texture mapping with no Z buffer  - for horizon

ScanLinePITIP   macro   name,pre,incu,decu,incv,decv,post1,post2,BLEND
name		proc

		pushad

	; Copy various parameters into registers

                mov     esi,work.pu.current
                mov     ebx,work.pv.current
                mov     eax,work.pu.grad_x
                mov     edi,work.pv.grad_x

		mov	dv_numerator,edi
                mov     ecx,work.pi.current
                mov     edx,work.pq.current
		mov	v_numerator,ebx
		mov	denominator,edx
                mov     i,ecx
		
                mov     ebp,work.tsl._end
                mov     du_numerator,eax
		
                mov     edi,work.tsl.start
                mov     eax,work.tsl.source
		
                cmp     edi,ebp
                ja      rtol		; Jump if right to left	
		
	    ; Left to right scan
		
                mov     dest,edi
		mov	ebx,v_numerator
                mov     ebp,du_numerator
next_pixelb:
		jae     L50		; Jump if end of scan
		
                mov     source,eax	; Free up eax

                mov     edx,work.texture.base
		xor	ecx,ecx
		add	edx,eax
		mov	eax,work.shade_table
		mov	cl,[edx]
	        mov	edx,i		; Get lighting
		test	cl,cl
		jz	transp1
		mov	ch,byte ptr i+2
        lea     ecx,[ecx*2+eax] ; Address of lit texel
        mov     cx,[ecx]    ; Get lit texel
if BLEND
        mov     ax,[edi]    ; Get screen pixel
if BLEND-16
        shr     ecx,1       ; Halve src & dest colours
        and     eax,07bdeh  ;
        shr     eax,1       ;
        and     ecx,03defh  ;
else
        shr     ecx,1       ; Halve src & dest colours
        and     eax,0f7deh  ;
        shr     eax,1       ;
        and     ecx,07befh  ;
endif
        add     ecx,eax     ; Add together to blend them
endif
        mov     [edi],cx    ; Store texel
transp1:
		mov	eax,work.pi.grad_x
		add	edx,eax		; i += di
		mov	i,edx
		
		lea	edi,2[edi]
		cmp	edi,work.tsl._end
		jae	L50

	        mov	edx,denominator
                mov     dest,edi
		mov	edi,dv_numerator
                mov     ecx,work.pq.grad_x
                mov     eax,source	; Restore eax

	; Core perspective calculations

		pre
                sub     esi,ebp
                add     edx,ecx
                cmp     esi,edx
                jl      L36		; Jump if u not too large
		
                sub     esi,edx
incufloop:
                incu     		; Increment u
                add     ebp,ecx
                sub     esi,edx
                jge     incufloop
		add	esi,edx
		
		jmp	L37
L36:
		test    esi,esi
		jge	L37
decufloop:
                decu
                sub     ebp,work.pq.grad_x
                add     esi,edx
                jnc     decufloop
		
L37:
		sub     ebx,edi
		jge	notdecvf
decvfloop:       
                decv     		; Decrement v
                sub     edi,ecx
                add     ebx,edx
                jnc     decvfloop
		mov	dv_numerator,edi
		jmp	L40
notdecvf:
                cmp     ebx,edx
                jl      L40		; Jump if v not too small
incvf:
                sub     ebx,edx
		
incvfloop:
		incv     		; Increment v
                add     edi,ecx
                sub     ebx,edx
                jge     incvfloop
		
		add	ebx,edx
		mov	dv_numerator,edi
L40:
		post1
		mov	denominator,edx
		mov	edi,dest
		post2
                cmp     edi,work.tsl._end
                jmp     next_pixelb

	    ; Right to left
rtol:
                mov     dest,edi
                mov     ebp,du_numerator
next_pixelbr:
		jbe     L50		; Jump if end of scan
		
                mov     source,eax	; Free up eax

                mov     edx,work.texture.base
		xor	ecx,ecx
		add	edx,eax
		mov	eax,work.shade_table
		mov	cl,[edx]
	        mov	edx,i		; Get lighting
		test	cl,cl
		jz	transp2

		mov	ch,byte ptr i+2
        lea     ecx,[eax+2*ecx] ; Address of lit texel
        mov     cx,[ecx]    ; Get lit texel
if BLEND
        mov     ax,[edi]    ; Get screen pixel
if BLEND-16
        shr     ecx,1       ; Halve src & dest colours
        and     eax,07bdeh  ;
        shr     eax,1       ;
        and     ecx,03defh  ;
else
        shr     ecx,1       ; Halve src & dest colours
        and     eax,0f7deh  ;
        shr     eax,1       ;
        and     ecx,07befh  ;
endif
        add     ecx,eax     ; Add together to blend them
endif
        mov     [edi],cx    ; Store texel
transp2:
		mov	eax,work.pi.grad_x
		sub	edx,eax		; i += di
		mov	i,edx

		lea	edi,(-2)[edi]
		cmp	edi,work.tsl._end
		jbe	L50

	        mov	edx,denominator
                mov     dest,edi
		mov	edi,dv_numerator
                mov     ecx,work.pq.grad_x
                mov     eax,source	; Restore eax

	; Core perspective calculations

		pre
                add     esi,ebp
                sub     edx,ecx
                cmp     esi,edx
                jl      R36 		; Jump if u not too large
incub:
                sub     esi,edx
		
incubloop:
                incu     		; Increment u
                add     ebp,ecx
                sub     esi,edx
                jge     incubloop
		add	esi,edx
		
		jmp	R37
		
R36:
		test    esi,esi
		jge	R37
decubloop:
                decu
                sub     ebp,work.pq.grad_x
                add     esi,edx
                jnc     decubloop
R37:
		add     ebx,edi
		jge	nodecvb
decvbloop:       
                decv     		; Decrement v
                sub     edi,ecx
                add     ebx,edx
                jnc     decvbloop
		mov	dv_numerator,edi
		jmp	R40
nodecvb:
                cmp     ebx,edx
                jl      R40		; Jump if v not too small
                sub     ebx,edx
incvbloop:
		incv     		; Increment v
                add     edi,ecx
                sub     ebx,edx
                jge     incvbloop
		
		add	ebx,edx
		mov	dv_numerator,edi
R40:
		post1
		mov	denominator,edx
		mov	edi,dest
		post2
                cmp     edi,work.tsl._end
                jmp     next_pixelbr

L50:
		popad
		ret
		

		; Back
		
		
name		endp

		endm


	if PARTS and PART_15
ScanLinePITIP	ScanLinePITIP256_RGB_555,\
		<>,\
		<inc al>,<dec al>,<inc ah>,<dec ah>,\
		<>,<>

ScanLinePITIP   ScanLinePITIPB256_RGB_555,\
		<>,\
		<inc al>,<dec al>,<inc ah>,<dec ah>,\
        <>,<>,15
	endif

    if PARTS and PART_16
ScanLinePITIP   ScanLinePITIPB256_RGB_565,\
		<>,\
		<inc al>,<dec al>,<inc ah>,<dec ah>,\
        <>,<>,16
	endif

    end
