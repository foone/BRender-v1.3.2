;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: t15_pizp.asm 1.1 1997/12/10 16:48:58 jon Exp $
;; $Locker: $
;;
;; Scanline loop for 15 bit perpsective correct texture mapping
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


; Smoothly lit perspective texture mapping

ScanLinePIZ2TIP macro   name,pre,incu,decu,incv,decv,post1,post2,BLEND
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
		

                mov     ecx,work.pz.current
                mov     z,ecx
                mov     ecx,work.tsl.zstart
                mov     zdest,ecx

                mov     ebp,work.tsl._end
                mov     du_numerator,eax
		
                mov     edi,work.tsl.start
                mov     eax,work.tsl.source
		
                cmp     edi,ebp
		mov	ebp,zdest
                ja      rtol		; Jump if right to left	
		
	    ; Left to right scan
		
                mov     dest,edi
next_pixelb:
		jae     L50		; Jump if end of scan
		
                mov     source,eax	; Free up eax

	    ; Texel fetch and store section

	    ; z buffer test

		mov	dl,[ebp]
		mov	ebx,z		; Get new z value
		mov	dh,1[ebp]
		ror	ebx,16
                cmp     bx,dx
                ja      L34		; Jump if new pixel behind old
		
                mov     edx,work.texture.base
		xor	ecx,ecx

		;; Get texel and test for transparency

		mov	cl,[eax+edx]
if 1
		test	cl,cl
		jz	L34
endif

		;; Store texel and z

		mov	eax,work.shade_table
		mov	ch,byte ptr i+2
		lea	eax,[2*ecx+eax]
if BLEND
        mov     cx,[eax]    ; Get lit texel
        mov     dx,[edi]    ; Get screen pixel
if BLEND-16
        shr     ecx,1       ; Halve src & dest colours
        and     edx,07bdeh  ;
        shr     edx,1       ;
        and     ecx,03defh  ;
else
        shr     ecx,1       ; Halve src & dest colours
        and     edx,0f7deh  ;
        shr     edx,1       ;
        and     ecx,07befh  ;
endif
        add     ecx,edx     ; Add together to blend them
else
        mov     [ebp],bx    ; Store new z
        mov     cx,[eax]    ; Get lit texel
endif
        mov     [edi],cx    ; Store texel

L34:
		ror	ebx,16
                mov     edx,work.pz.grad_x	; Get z step
                add     ebp,2		; zdest++
                add     ebx,edx		; z += dz
	        mov	edx,i		; Get lighting
		mov	eax,work.pi.grad_x
		add	edx,eax		; i += di
		mov	i,edx
		
L34a:
		lea	edi,2[edi]
if	1
		cmp	edi,work.tsl._end
		jae	L50
endif
	        mov	edx,denominator
                mov     dest,edi
		mov	edi,dv_numerator
                mov     z,ebx
                mov     zdest,ebp
                mov     ecx,work.pq.grad_x
                mov     ebp,du_numerator
		mov	ebx,v_numerator
                mov     eax,source	; Restore eax

	; Perspective calculation section

		pre
                sub     esi,ebp
                add     edx,ecx
                cmp     esi,edx
                jl      L36		; Jump if u not too large
		
                mov     ecx,work.pq.grad_x
		mov     ebp,du_numerator
                sub     esi,edx
		
incufloop:
                incu     		; Increment u
                add     ebp,ecx
                sub     esi,edx
                jge     incufloop
		add	esi,edx
		
                mov     du_numerator,ebp
		jmp	L37

L36:
		test    esi,esi
		jge	L37

                mov     ebp,du_numerator
decufloop:
                decu
                sub     ebp,work.pq.grad_x
                add     esi,edx
                jnc     decufloop
		
                mov     du_numerator,ebp
		
L37:
		sub     ebx,edi
		jl	decvf
L40a:
                cmp     ebx,edx
                jge     incvf		; Jump if v not too small
		jmp	L40
decvf:
                mov     ebp,work.pq.grad_x
decvfloop:       
                decv     		; Decrement v
                sub     edi,ebp
                add     ebx,edx
                jnc     decvfloop
		mov	dv_numerator,edi
L40:
		post1
		post2
		mov	denominator,edx
		mov	v_numerator,ebx
		mov	edi,dest
                cmp     edi,work.tsl._end
		mov     ebp,zdest
                jmp     next_pixelb

	    ; Right to left
rtol:
                mov     dest,edi
next_pixelbr:
		jbe     L50		; Jump if end of scan
		
                mov     source,eax	; Free up eax

	    ; z buffer test

		mov	dl,[ebp]
		mov	ebx,z		; Get new z value
		mov	dh,1[ebp]
		ror	ebx,16
                cmp     bx,dx
                ja      R34		; Jump if new pixel behind old
		
                mov     edx,work.texture.base
		xor	ecx,ecx

		;; Get texel and test for transparency

		mov	cl,[edx+eax]
if 1
		test	cl,cl		; Transparent?
		jz	R34
endif

		;; Store texel and z

		mov	eax,work.shade_table
		mov	ch,byte ptr i+2
		lea	eax,[2*ecx+eax]
if BLEND
        mov     cx,[eax]    ; Get lit texel
        mov     dx,[edi]    ; Get screen pixel
if BLEND-16
        shr     ecx,1       ; Halve src & dest colours
        and     edx,07bdeh  ;
        shr     edx,1       ;
        and     ecx,03defh  ;
else
        shr     ecx,1       ; Halve src & dest colours
        and     edx,0f7deh  ;
        shr     edx,1       ;
        and     ecx,07befh  ;
endif
        add     ecx,edx     ; Add together to blend them
else
        mov     [ebp],bx    ; Store new z
        mov     cx,[eax]    ; Get lit texel
endif
        mov     [edi],cx    ; Store texel
R34:
                sub     ebp,2		; zdest++

		;; Update lighting and z

		ror	ebx,16
                mov     edx,work.pz.grad_x	; Get z step
		mov	eax,work.pi.grad_x
                sub     ebx,edx		; z += dz
	        mov	edx,i		; Get lighting
		sub	edx,eax		; i += di
		mov	i,edx
		
R34a:
		lea	edi,(-2)[edi]
if	1
		cmp	edi,work.tsl._end
		jbe	L50
endif
	        mov	edx,denominator
                mov     dest,edi
		mov	edi,dv_numerator
                mov     z,ebx
                mov     zdest,ebp
                mov     ecx,work.pq.grad_x
                mov     ebp,du_numerator
		mov	ebx,v_numerator
                mov     eax,source	; Restore eax

	; Core perspective calculations

		pre
                add     esi,ebp
                sub     edx,ecx
                cmp     esi,edx
                jge     incub		; Jump if u not too large
		
R36:
		test    esi,esi
		jl	decub
R37:
		add     ebx,edi
		jl	decvb
                cmp     ebx,edx
                jge     incvb		; Jump if v not too small
R40:
		post1
		post2
		mov	denominator,edx
		mov	v_numerator,ebx
		mov	edi,dest
                cmp     edi,work.tsl._end
		mov     ebp,zdest
                jmp     next_pixelbr

L50:
		popad
		ret
		
		
incvf:
                mov     ecx,work.pq.grad_x
                sub     ebx,edx
		
incvfloop:
		incv     		; Increment v
                add     edi,ecx
                sub     ebx,edx
                jge     incvfloop
		
		add	ebx,edx
		mov	dv_numerator,edi
		jmp	L40

		; Back
decub:
                mov     ebp,du_numerator
decubloop:
                decu
                sub     ebp,work.pq.grad_x
                add     esi,edx
                jnc     decubloop
		
                mov     du_numerator,ebp
		jmp	R37
		
incub:
                mov     ecx,work.pq.grad_x
		mov     ebp,du_numerator
                sub     esi,edx
		
incubloop:
                incu     		; Increment u
                add     ebp,ecx
                sub     esi,edx
                jge     incubloop
		add	esi,edx
		
                mov     du_numerator,ebp
		jmp	R37

decvb:
                mov     ebp,work.pq.grad_x
decvbloop:       
                decv     		; Decrement v
                sub     edi,ebp
                add     ebx,edx
                jnc     decvbloop
		mov	dv_numerator,edi
		jmp	R40
		
incvb:
                mov     ecx,work.pq.grad_x
                sub     ebx,edx
		
incvbloop:
		incv     		; Increment v
                add     edi,ecx
                sub     ebx,edx
                jge     incvbloop
		
		add	ebx,edx
		mov	dv_numerator,edi
		jmp	R40
		
name		endp

		endm

	if PARTS and PART_15Z
ScanLinePIZ2TIP	ScanLinePIZ2TIP256_RGB_555,\
		<>,\
		<inc al>,<dec al>,<inc ah>,<dec ah>,\
		<>,<>

ScanLinePIZ2TIP ScanLinePIZ2TIPB256_RGB_555,\
		<>,\
		<inc al>,<dec al>,<inc ah>,<dec ah>,\
        <>,<>,15
	endif

    if PARTS and PART_16Z
ScanLinePIZ2TIP ScanLinePIZ2TIPB256_RGB_565,\
		<>,\
		<inc al>,<dec al>,<inc ah>,<dec ah>,\
        <>,<>,16
	endif
		end
