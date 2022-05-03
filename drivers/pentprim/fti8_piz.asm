; Pentium optimized rasterisers using floating point setup code
;
	.586p
	.model flat,c

	include	drv.inc
	include 586_macs.inc
	include fpwork.inc
	include fpsetup.inc
	include	common.inc


	; Cheat case entry points for perspective correct texture mapper
	;
externdef TriangleRasterise_ZI_I8_D16:proc
externdef TriangleRasterise_ZT_I8_D16_256:proc
externdef TriangleRasterise_ZTF_I8_D16_256:proc


if 0
	.data

	public	setup_cycles
	public	outer_cycles
	public	inner_cycles

setup_cycles	dq	0
outer_cycles	dq	0
inner_cycles	dq	0

CLEAR_TIMER	macro	section
		mov		[dword ptr &section&_cycles],0
		mov		[dword ptr &section&_cycles+4],0
		endm

START_TIMER	macro	section
		push	eax
		push	edx
		rdtsc
		sub		[dword ptr &section&_cycles],eax
		sbb		[dword ptr &section&_cycles+4],edx
		pop		edx
		pop		eax
		endm

STOP_TIMER	macro	section
		push	eax
		push	edx
		rdtsc
		add		[dword ptr &section&_cycles],eax
		adc		[dword ptr &section&_cycles+4],edx
		pop		edx
		pop		eax
		endm
else
CLEAR_TIMER	macro	section
			endm
START_TIMER	macro	section
			endm
STOP_TIMER	macro	section
			endm
endif


; Setup routines.  These should be exported by a header file.
;
;extern TriangleSetup_ZTI_F:proc
;extern TriangleSetup_ZT_F:proc
;extern TriangleSetup_ZI_F:proc
;extern TriangleSetup_Z_F:proc


; Alternative versions of useful instructions for making forward and backward
; scanning versions of rasterisers
;
add_f	equ	add
add_b	equ	sub

adc_f	equ	adc
adc_b	equ	sbb

sub_f	equ	sub
sub_b	equ	add

sbb_f	equ	sbb
sbb_b	equ	adc

inc_f	equ	inc
inc_b	equ	dec

jg_f	equ	jg
jg_b	equ	jl

jle_f	equ	jle
jle_b	equ	jge

; Some equates that allow me to use the old style naming conventions (with .s replaced
; by _s) to refer to the workspace for the floating point setup
;
work_main_i			equ	workspace.xm
work_main_d_i		equ	workspace.d_xm

work_top_count		equ	workspace.topCount
work_top_i			equ	workspace.x1
work_top_d_i		equ	workspace.d_x1

work_bot_count		equ	workspace.bottomCount
work_bot_i			equ	workspace.x2
work_bot_d_i		equ	workspace.d_x2

work_pz_current		equ	workspace.s_z
work_pz_grad_x		equ	workspace.d_z_x
work_pz_d_nocarry	equ	workspace.d_z_y_0
work_pz_d_carry		equ	workspace.d_z_y_1

work_pu_current		equ	workspace.s_u
work_pu_grad_x		equ	workspace.d_u_x
work_pu_d_nocarry	equ	workspace.d_u_y_0
work_pu_d_carry		equ	workspace.d_u_y_1

work_pv_current		equ	workspace.s_v
work_pv_grad_x		equ	workspace.d_v_x
work_pv_d_nocarry	equ	workspace.d_v_y_0
work_pv_d_carry		equ	workspace.d_v_y_1

work_pi_current		equ	workspace.s_i
work_pi_grad_x		equ	workspace.d_i_x
work_pi_d_nocarry	equ	workspace.d_i_y_0
work_pi_d_carry		equ	workspace.d_i_y_1


	.code


; Trapezium loop for gouraud shaded z buffered faces
;
; Arguments control:
;	whether loop uses 'top' or 'bottom' variables
;	which direction scanline are rendered from the long edge
;
TRAPEZIUM_ZI_I8_D16	macro	half,dirn
   	local scan_loop,pixel_loop,pixel_behind,no_pixels
	local done_trapezium

		mov	ebx,work_&half&_count	; check for empty trapezium
		;vslot

		test	ebx,ebx
		jl	done_trapezium

		mov	eax,work_pi_current
		mov	edx,work_pz_current

		mov	edi,work_&half&_i
		mov	ebp,work_main_i

		shr	edi,16				; get integer part of end of first scanline
		mov	ebx,workspace.depthAddress

		shr	ebp,16				; get integer part of start of first scanline
		mov	esi,workspace.scanAddress

scan_loop:

	; Calculate pixel count and end addresses for line
	; Adjust i & z for the inner loop
	;
		sub	ebp,edi				; calculate pixel count
		jg_&dirn	no_pixels

		add	esi,edi				; calculate end colour buffer pointer
		lea	edi,[ebx+edi*2]		; calculate end depth buffer pointer

		ror	eax,16				; swap words of i
		mov	ecx,work_pi_grad_x

		ror	edx,16				; swap words of z
		sub_&dirn	eax,ecx		; cancel out first step of i in loop

		sbb_&dirn	eax,0		; also clear carry
		mov	ebx,edx				; need same junk in high word of old and new z

	STOP_TIMER	outer
	START_TIMER	inner

	; eax = i
	; ebx = old z, dz
	; ecx =	di
	; edx =	z
	; ebp =	count
	; esi =	frame buffer ptr
	; edi =	z buffer ptr
	;
pixel_loop:

		adc_&dirn	edx,0		; carry into integer part of z
		mov	bl,[edi+ebp*2]		; fetch old z

		add_&dirn	eax,ecx		; step i
		mov	bh,[edi+ebp*2+1]

		adc_&dirn	eax,0		; carry into integer part of i
		cmp	edx,ebx				; compare z (identical junk in top words does not affect result)

		mov	ebx,work_pz_grad_x
		ja	pixel_behind

		mov	[edi+ebp*2],dx		; store pixel and depth (prefix cannot be avoided since
		mov	[esi+ebp],al		; two byte writes would fill the write buffers)

pixel_behind:

		add_&dirn	edx,ebx		; step z
		inc_&dirn	ebp			; increment (negative) counter/offset

		mov	ebx,edx				; need same junk in high word of old and new z
		jle_&dirn	pixel_loop	; loop

	STOP_TIMER	inner
	START_TIMER	outer

no_pixels:

	; Updates for next scanline:
	;
		mov	esi,workspace.scanAddress
		mov	ecx,work.colour.stride_b

		mov	ebx,workspace.depthAddress
		mov	edx,work.depth.stride_b

		add	esi,ecx				; move down one line in colour buffer
		add	ebx,edx				; move down one line in depth buffer

		mov	workspace.scanAddress,esi
		mov	workspace.depthAddress,ebx

		mov	ebp,work_main_i
		mov	edi,work_&half&_i

		add	ebp,work_main_d_i	; step major edge
		add	edi,work_&half&_d_i	; step minor edge

		mov	work_main_i,ebp
		mov	work_&half&_i,edi

		mov	eax,work.main.f
		mov	ecx,work.main.d_f

		shr	ebp,16				; get integer part of start of first scanline
		add	eax,ecx

		sbb	ecx,ecx				; get (0 - carry)

		shr	edi,16				; get integer part of end of first scanline
		mov	work.main.f,eax

		mov	eax,work_pi_current
		mov	edx,work_pz_current

		add	eax,[work_pi_d_nocarry+ecx*8]	; step i according to carry from major edge	; *4 for old workspace
		add	edx,[work_pz_d_nocarry+ecx*8]	; step z according to carry from major edge	; *4 for old workspace

		mov	work_pi_current,eax
		mov	ecx,work_&half&_count

		mov	work_pz_current,edx
		dec	ecx					; decrement line counter

		mov	work_&half&_count,ecx
		jge scan_loop

done_trapezium:
		endm


; Trapezium loop for gouraud shaded z buffered faces
;
; Arguments control:
;	whether loop uses 'top' or 'bottom' variables
;	which direction scanline are rendered from the long edge
;

majorScanAddress        equ     scratch0

TRAPEZIUM_ZIF_I8_D16     macro   half,dirn
   	local scan_loop,pixel_loop,pixel_behind,no_pixels
	local done_trapezium

		mov	ebx,work_&half&_count	; check for empty trapezium
		;vslot

		test	ebx,ebx
		jl	done_trapezium

		mov	eax,work_pi_current
		mov	edx,work_pz_current

		mov	edi,work_&half&_i
		mov	ebp,work_main_i

		shr	edi,16				; get integer part of end of first scanline
		mov	ebx,workspace.depthAddress

		shr	ebp,16				; get integer part of start of first scanline
		mov	esi,workspace.scanAddress

scan_loop:

	; Calculate pixel count and end addresses for line
	; Adjust i & z for the inner loop
	;
		sub	ebp,edi				; calculate pixel count
		jg_&dirn	no_pixels

		add	esi,edi				; calculate end colour buffer pointer
		lea	edi,[ebx+edi*2]		; calculate end depth buffer pointer

		ror	eax,16				; swap words of i
        xor     ebx,ebx

        mov     workspace.majorScanAddress,esi
        mov     esi,work_pi_grad_x

	STOP_TIMER	outer
	START_TIMER	inner

	; eax = i
    ; ebx = old z, dz, fog table
    ; ecx = z, fog lookup
	; edx =	z
	; ebp =	count
    ; esi = frame buffer ptr, di
	; edi =	z buffer ptr
	;
pixel_loop:
        xor     ebx,ebx                 ; empty out z delta
        mov     ecx,edx

        shr     ecx,16                  ; shift out fraction
        mov     bl,[edi+ebp*2]          ; fetch old z

        mov     bh,[edi+ebp*2+1]

        cmp     ecx,ebx                 ; compare z
        ja      pixel_behind

        mov     [edi+ebp*2],cl          ; store low byte of new z
        mov     cl,al                   ; low byte replaced with i

        mov     ebx,work.fog_table      ; fetch fog table address
        mov     esi,workspace.majorScanAddress  ; fetch screen address

        mov     [edi+ebp*2+1],ch        ; store high byte of new z
                                                ; agi here:
        mov     cl,[ecx+ebx]            ; perform fog lookup

        mov     [esi+ebp],cl            ; store fogged intensity
        mov     esi,work_pi_grad_x      ; fetch i gradient back in
pixel_behind:
        mov     ebx,work_pz_grad_x      ; fetch z gradient back in
        add_&dirn       eax,esi         ; step i

		adc_&dirn	eax,0		; carry into integer part of i
        add_&dirn       edx,ebx         ; step z

        inc_&dirn       ebp
        jle_&dirn       pixel_loop

	STOP_TIMER	inner
	START_TIMER	outer

no_pixels:

	; Updates for next scanline:
	;
		mov	esi,workspace.scanAddress
		mov	ecx,work.colour.stride_b

		mov	ebx,workspace.depthAddress
		mov	edx,work.depth.stride_b

		add	esi,ecx				; move down one line in colour buffer
		add	ebx,edx				; move down one line in depth buffer

		mov	workspace.scanAddress,esi
		mov	workspace.depthAddress,ebx

		mov	ebp,work_main_i
		mov	edi,work_&half&_i

		add	ebp,work_main_d_i	; step major edge
		add	edi,work_&half&_d_i	; step minor edge

		mov	work_main_i,ebp
		mov	work_&half&_i,edi

		mov	eax,work.main.f
		mov	ecx,work.main.d_f

		shr	ebp,16				; get integer part of start of first scanline
		add	eax,ecx

		sbb	ecx,ecx				; get (0 - carry)

		shr	edi,16				; get integer part of end of first scanline
		mov	work.main.f,eax

		mov	eax,work_pi_current
		mov	edx,work_pz_current

		add	eax,[work_pi_d_nocarry+ecx*8]	; step i according to carry from major edge	; *4 for old workspace
		add	edx,[work_pz_d_nocarry+ecx*8]	; step z according to carry from major edge	; *4 for old workspace

		mov	work_pi_current,eax
		mov	ecx,work_&half&_count

		mov	work_pz_current,edx
		dec	ecx					; decrement line counter

		mov	work_&half&_count,ecx
		jge scan_loop

done_trapezium:
		endm


; Trapezium loop for linear texture mapped z buffered faces
;
; Arguments control:
;	whether loop uses 'top' or 'bottom' variables
;	which direction scanline are rendered from the long edge
;
TRAPEZIUM_ZT_I8_D16_256	macro	half,dirn
   	local scan_loop,pixel_loop,pixel_behind,no_pixels
	local done_trapezium

		mov	ebx,work_&half&_count	; check for empty trapezium
		mov	edx,work_pz_current

		test	ebx,ebx
		jl	done_trapezium

		mov	edi,work_&half&_i
		mov	ebp,work_main_i

		shr	edi,16				; get integer part of end of first scanline
		mov	ecx,work_pv_current

		shr	ebp,16				; get integer part of start of first scanline
		mov	esi,work_pu_current

scan_loop:

	; Calculate pixel count and end addresses for next scanline
	;
		mov	ebx,workspace.depthAddress
		sub	ebp,edi				; calculate pixel count

		mov	eax,workspace.scanAddress
		jg_&dirn	no_pixels

		add	eax,edi				; calculate end colour buffer pointer
		lea	edi,[ebx+edi*2]		; calculate end depth buffer pointer

		mov	work.tsl.dest,eax
		mov	work.tsl.zdest,edi

		ror	edx,16				; swap words of z for loop
		mov	eax,esi				; make copy of u for loop

		shr	eax,16				; shift integer part of u into place for loop
		mov	ebx,edx				; make copy of z for loop

	STOP_TIMER	outer
	START_TIMER	inner

	; eax = texel offset, texel
	; ebx = oldz, base, temp
	; ecx = v
	; edx = z
	; esi = u
	; edi = zbase, texture base
	; ebp = count
	;
pixel_loop:

		ror	ecx,16
		mov	bl,[edi+ebp*2]

		mov	ah,cl
		mov	bh,[edi+ebp*2+1]

		mov	edi,work.texture.base
		cmp	edx,ebx

		mov	ebx,work.tsl.dest
		ja	pixel_behind

		mov	al,[eax+edi]
		mov	edi,work.tsl.zdest

		and	al,al
		je	pixel_behind

		mov	[edi+ebp*2],dx
		mov	[ebx+ebp],al

pixel_behind:

		ror	ecx,16
		mov	edi,work.tsl.zdest

		add_&dirn	esi,work_pu_grad_x
		add_&dirn	ecx,work_pv_grad_x

		mov	eax,esi
		mov	ebx,work_pz_grad_x

		shr	eax,16
		add_&dirn	edx,ebx

		adc_&dirn	edx,0
		inc_&dirn	ebp

		mov	ebx,edx
		jle_&dirn	pixel_loop

	STOP_TIMER	inner
	START_TIMER	outer

no_pixels:

	; Updates for next scanline:
	;
		mov	eax,workspace.scanAddress
		mov	ebx,work.colour.stride_b

		mov	ecx,workspace.depthAddress
		mov	edx,work.depth.stride_b

		add	eax,ebx				; move down one line in colour buffer
		add	ecx,edx				; move down one line in depth buffer

		mov	workspace.scanAddress,eax
		mov	workspace.depthAddress,ecx

		mov	ebp,work_main_i
		mov	edi,work_&half&_i

		add	ebp,work_main_d_i	; step major edge
		add	edi,work_&half&_d_i	; step minor edge

		mov	work_main_i,ebp
		mov	work_&half&_i,edi

		mov	eax,work.main.f
		mov	ebx,work.main.d_f

		shr	ebp,16				; get integer part of start of first scanline
		add	eax,ebx

		sbb	ebx,ebx				; get (0 - carry)

		shr	edi,16				; get integer part of end of first scanline
		mov	work.main.f,eax

		mov	edx,work_pz_current
		mov	eax,[work_pz_d_nocarry+ebx*8]	; *4 for old workspace

		add	edx,eax				; step z according to carry from major edge
		mov	ecx,work_pv_current

		mov	esi,work_pu_current
		mov	work_pz_current,edx

		add	esi,[work_pu_d_nocarry+ebx*8]	; step u according to carry from major edge	; *4 for old workspace
		add	ecx,[work_pv_d_nocarry+ebx*8]	; step v according to carry from major edge	; *4 for old workspace

		mov	work_pv_current,ecx
		mov	ebx,work_&half&_count

		mov	work_pu_current,esi
		dec	ebx					; decrement line counter

		mov	work_&half&_count,ebx
		jge scan_loop

done_trapezium:
		endm


; Trapezium loop for linear texture mapped z buffered faces
;
; Arguments control:
;	whether loop uses 'top' or 'bottom' variables
;	which direction scanline are rendered from the long edge
;
TRAPEZIUM_ZTF_I8_D16_256 macro   half,dirn
   	local scan_loop,pixel_loop,pixel_behind,no_pixels
	local done_trapezium

		mov	ebx,work_&half&_count	; check for empty trapezium
        mov     esi,work_pz_current

		test	ebx,ebx
		jl	done_trapezium

		mov	edi,work_&half&_i
		mov	ebp,work_main_i

		shr	edi,16				; get integer part of end of first scanline
        mov     ebx,work_pv_current

		shr	ebp,16				; get integer part of start of first scanline
        mov     eax,work_pu_current

scan_loop:
        
        mov     edx,workspace.depthAddress
        sub     ebp,edi                         ; calculate pixel count

        mov     ecx,workspace.scanAddress
        jg_&dirn        no_pixels

        add     ecx,edi                 ; calculate end colour buffer pointer
        lea     edi,[edx+edi*2]         ; calculate end depth buffer pointer

        and     eax,000fffff0h          ; set up u & v regs according to
        and     ebx,000fffff0h          ; description below

        rol     ebx,12
        mov     work.tsl.dest,ecx       ; store major scan colour buffer address

        or      bx,ax
        mov     work.tsl.zdest,edi      ; store major scan z address

        shr     eax,16
        mov     ecx,esi                     ; preps for loop

        ror     ebx,16
        xor     edx,edx                     ;   "    "   "

	STOP_TIMER	outer
	START_TIMER	inner


; eax - texel offset, u coord: 000000uu
;     i=integer                ------ii
; ebx - v coord, u fraction:   uuuvvvvv
;     f=fraction, i=integer    fffiifff
; ecx - z temp, fog table
; edx - old z, z & u deltas, texel, fog lookup
; esi - z
; edi - z buffer, texture base, colour buffer
; ebp - pixel count

pixel_loop:
        ror ebx,4                       ; shift v to byte boundary
        mov dl,[edi+ebp*2]              ; fetch old z

        mov dh,[edi+ebp*2+1]
        mov ah,bh                       ; u+v -> texture offset

        shr ecx,16                      ; z integer
        mov edi,work.texture.base       ; fetch texture base

        rol ebx,4                       ; shift v back
        cmp edx,ecx                     ; z test

        mov dl,[edi+eax]                ; read texel
        mov edi,work.tsl.zdest          ; fetch z buffer address

        mov dh,ch                       ; want to use ecx soon, so store ch
        jl pixel_behind                 ; skip stuff below if pixel is behind

        test dl,dl                      ; transparency test
        je pixel_behind

        mov [edi+ebp*2],cl              ; store lower z byte
        mov ecx,work.fog_table          ; fetch fog table address

        mov [edi+ebp*2+1],dh            ; store upper z byte (kept previously)
        mov edi,work.tsl.dest           ; fetch colour buffer address

        mov ah,[ecx+edx]                ; fog lookup
                                        ; no pairing

        mov [edi+ebp],ah                ; write texel
        mov edi,work.tsl.zdest          ; fetch z buffer address

pixel_behind:

        mov edx,work_pz_grad_x          ; fetch deltas
        mov ecx,work_pv_grad_x

        add_&dirn esi,edx               ; step z
        mov edx,work_pu_grad_x

        add_&dirn ebx,ecx               ; step v & u.frac
        mov ecx,esi                     ; copy z for next iteration

        adc_&dirn al,dl                 ; step u
        xor edx,edx                     ; need edx empty

        inc_&dirn ebp                   ; pixel loop
        jle_&dirn pixel_loop



	STOP_TIMER	inner
	START_TIMER	outer

no_pixels:

	; Updates for next scanline:
	;
		mov	eax,workspace.scanAddress
        mov     edx,work.colour.stride_b

		mov	ecx,workspace.depthAddress
        mov     ebx,work.depth.stride_b

        add     eax,edx                         ; move down one line in colour buffer
        add     ecx,ebx                         ; move down one line in depth buffer

		mov	workspace.scanAddress,eax
		mov	workspace.depthAddress,ecx

		mov	ebp,work_main_i
		mov	edi,work_&half&_i

		add	ebp,work_main_d_i	; step major edge
		add	edi,work_&half&_d_i	; step minor edge

		mov	work_main_i,ebp
		mov	work_&half&_i,edi

		mov	eax,work.main.f
		mov	edx,work.main.d_f

		shr	ebp,16				; get integer part of start of first scanline
		add	eax,edx

		sbb	edx,edx				; get (0 - carry)

		shr	edi,16				; get integer part of end of first scanline
		mov	work.main.f,eax

        mov     esi,work_pz_current
        mov     eax,[work_pz_d_nocarry+edx*8]   ; *4 for old workspace

        add     esi,eax                         ; step z according to carry from major edge
        mov     ebx,work_pv_current

        mov     eax,work_pu_current
        mov     work_pz_current,esi

        add     eax,[work_pu_d_nocarry+edx*8]   ; step u according to carry from major edge     ; *4 for old workspace
        add     ebx,[work_pv_d_nocarry+edx*8]   ; step v according to carry from major edge     ; *4 for old workspace

        mov     work_pv_current,ebx
        mov     ecx,work_&half&_count

        mov     work_pu_current,eax
        dec     ecx                             ; decrement line counter

        mov     work_&half&_count,ecx
        jge scan_loop

done_trapezium:
		endm


	if PARTS and PART_8Z

; TriangleRender_ZI_I8_D16
;
; Render a triangle into frame buffer
;
;	Linear interpolated colour index
;	Linear interpolated Z value
;	Real vertices
;	Perfect point sampling
; 
TriangleRender_ZI_I8_D16 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

	CLEAR_TIMER	setup
	CLEAR_TIMER	outer
	CLEAR_TIMER	inner

	START_TIMER	setup

	; Get pointers to vertex structures
	;
		mov	eax,pvertex_0
		mov	ecx,pvertex_1

		mov	edx,pvertex_2
		mov	workspace.v0,eax

		mov	workspace.v1,ecx
		mov	workspace.v2,edx

	; Call new floating point setup routine
	;
        call TriangleSetup_ZI
TriangleRasterise_ZI_I8_D16 label proc
	; Calculate address of first scanline in colour and depth buffers
	;
		mov	esi,workspace.t_y
		mov	eax,work.colour.base

		dec	esi
		mov	ebx,work.colour.stride_b

		mov	ecx,work.depth.base
		mov	edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add	eax,ebx
		add	ecx,edx

		dec	eax
		sub	ecx,2

		mov	workspace.scanAddress,eax
		mov	workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z & i gradients
	;
	; This will cause carry into fractional part for negative gradients so
	; subtract one from the fractional part to adjust accordingly
	;
		mov	eax,work_main_i
		mov	ebx,work_main_d_i

		shl	eax,16
		mov	ecx,work_pz_grad_x

		shl	ebx,16
		cmp	ecx,80000000h

		adc	ecx,-1
		mov	edx,work_pi_grad_x

		ror	ecx,16
		cmp	edx,80000000h

		adc	edx,-1
		mov	work.main.f,eax

		ror	edx,16
		mov	work.main.d_f,ebx

		mov	work_pz_grad_x,ecx
		mov	work_pi_grad_x,edx

	; Check scan direction and use appropriate rasteriser
	;
		mov	eax,workspace.flip
		;vslot

		test eax,eax
		jnz	reversed

	STOP_TIMER	setup
	START_TIMER	outer

		TRAPEZIUM_ZI_I8_D16 top,f
		TRAPEZIUM_ZI_I8_D16 bot,f

	STOP_TIMER	outer
quit:
		ret

reversed:
	STOP_TIMER	setup
	START_TIMER	outer

		TRAPEZIUM_ZI_I8_D16 top,b
		TRAPEZIUM_ZI_I8_D16 bot,b

	STOP_TIMER	outer

		ret

TriangleRender_ZI_I8_D16 endp

	endif

	if PARTS and PART_8Z_FOG

; TriangleRender_ZIF_I8_D16
;
; Render a triangle into frame buffer
;
;	Linear interpolated colour index
;	Linear interpolated Z value
;	Real vertices
;	Perfect point sampling
; 
TriangleRender_ZIF_I8_D16_FLAT proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

	CLEAR_TIMER	setup
	CLEAR_TIMER	outer
	CLEAR_TIMER	inner

	START_TIMER	setup

	; Get pointers to vertex structures
	;
		mov	eax,pvertex_0
		mov	ecx,pvertex_1

		mov	edx,pvertex_2
		mov	workspace.v0,eax

		mov	workspace.v1,ecx
		mov	workspace.v2,edx

	; Call new floating point setup routine
	;
        call TriangleSetup_ZI_FLAT

		jmp	TriangleRasterise_ZIF_I8_D16

TriangleRender_ZIF_I8_D16_FLAT endp


TriangleRender_ZIF_I8_D16 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

	CLEAR_TIMER	setup
	CLEAR_TIMER	outer
	CLEAR_TIMER	inner

	START_TIMER	setup

	; Get pointers to vertex structures
	;
		mov	eax,pvertex_0
		mov	ecx,pvertex_1

		mov	edx,pvertex_2
		mov	workspace.v0,eax

		mov	workspace.v1,ecx
		mov	workspace.v2,edx

	; Call new floating point setup routine
	;
        call TriangleSetup_ZI

TriangleRasterise_ZIF_I8_D16 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov	esi,workspace.t_y
		mov	eax,work.colour.base

		dec	esi
		mov	ebx,work.colour.stride_b

		mov	ecx,work.depth.base
		mov	edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add	eax,ebx
		add	ecx,edx

		dec	eax
		sub	ecx,2

		mov	workspace.scanAddress,eax
		mov	workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z & i gradients
	;
	; This will cause carry into fractional part for negative gradients so
	; subtract one from the fractional part to adjust accordingly
	;
		mov	eax,work_main_i
		mov	ebx,work_main_d_i

		shl	eax,16
;                mov     ecx,work_pz_grad_x

		shl	ebx,16
;                cmp     ecx,80000000h

;                adc     ecx,-1
		mov	edx,work_pi_grad_x

;                ror     ecx,16
		cmp	edx,80000000h

		adc	edx,-1
		mov	work.main.f,eax

		ror	edx,16
		mov	work.main.d_f,ebx

;                mov     work_pz_grad_x,ecx
		mov	work_pi_grad_x,edx

	; Check scan direction and use appropriate rasteriser
	;
		mov	eax,workspace.flip
		;vslot

		test eax,eax
		jnz	reversed

	STOP_TIMER	setup
	START_TIMER	outer

                TRAPEZIUM_ZIF_I8_D16 top,f
                TRAPEZIUM_ZIF_I8_D16 bot,f

	STOP_TIMER	outer
quit:
		ret

reversed:
	STOP_TIMER	setup
	START_TIMER	outer

                TRAPEZIUM_ZIF_I8_D16 top,b
                TRAPEZIUM_ZIF_I8_D16 bot,b

	STOP_TIMER	outer

		ret

TriangleRender_ZIF_I8_D16 endp

	endif

	if PARTS and PART_8Z

; TriangleRender_ZT_I8_D16_256
;
; Render a triangle into frame buffer
;
;	Linear interpolated colour texture
;	Linear interpolated Z value
;	Real vertices
;	Perfect point sampling
; 
TriangleRender_ZT_I8_D16_256 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

	CLEAR_TIMER	setup
	CLEAR_TIMER	outer
	CLEAR_TIMER	inner

	START_TIMER	setup

	; Get pointers to vertex structures
	;
		mov	eax,pvertex_0
		mov	ecx,pvertex_1

		mov	edx,pvertex_2
		mov	workspace.v0,eax

		mov	workspace.v1,ecx
		mov	workspace.v2,edx

	; Call new floating point setup routine
	;
        call TriangleSetup_ZT

TriangleRasterise_ZT_I8_D16_256 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov	esi,workspace.t_y
		mov	eax,work.colour.base

		dec	esi
		mov	ebx,work.colour.stride_b

		mov	ecx,work.depth.base
		mov	edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add	eax,ebx
		add	ecx,edx

		dec	eax
		sub	ecx,2

		mov	workspace.scanAddress,eax
		mov	workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	;
	; This will cause carry into fractional part for negative gradients so
	; subtract one from the fractional part to adjust accordingly
	;
		mov	eax,work_main_i
		mov	ebx,work_main_d_i

		shl	eax,16
		mov	ecx,work_pz_grad_x

		shl	ebx,16
		cmp	ecx,80000000h

		adc	ecx,-1
		mov	work.main.f,eax

		ror	ecx,16
		mov	work.main.d_f,ebx

		mov	work_pz_grad_x,ecx
		mov	eax,workspace.flip

	; Check scan direction and use appropriate rasteriser
	;
		test eax,eax
		jnz	reversed

	STOP_TIMER	setup
	START_TIMER	outer

		TRAPEZIUM_ZT_I8_D16_256 top,f
		TRAPEZIUM_ZT_I8_D16_256 bot,f

	STOP_TIMER	outer
quit:
		ret

reversed:
	STOP_TIMER	setup
	START_TIMER	outer

		TRAPEZIUM_ZT_I8_D16_256 top,b
		TRAPEZIUM_ZT_I8_D16_256 bot,b

	STOP_TIMER	outer

		ret

TriangleRender_ZT_I8_D16_256 endp

	endif

	if PARTS and PART_8Z_FOG

; TriangleRender_ZTF_I8_D16_256
;
; Render a triangle into frame buffer
;
;	Linear interpolated colour texture
;	Linear interpolated Z value
;	Real vertices
;	Perfect point sampling
; 
TriangleRender_ZTF_I8_D16_256 proc uses eax ebx ecx edx esi edi,
		pblock : ptr dword,
		pvertex_0 : ptr word,
		pvertex_1 : ptr word,
		pvertex_2 : ptr word

	CLEAR_TIMER	setup
	CLEAR_TIMER	outer
	CLEAR_TIMER	inner

	START_TIMER	setup

	; Get pointers to vertex structures
	;
		mov	eax,pvertex_0
		mov	ecx,pvertex_1

		mov	edx,pvertex_2
		mov	workspace.v0,eax

		mov	workspace.v1,ecx
		mov	workspace.v2,edx

	; Call new floating point setup routine
	;
        call TriangleSetup_ZT

TriangleRasterise_ZTF_I8_D16_256 label proc

	; Calculate address of first scanline in colour and depth buffers
	;
		mov	esi,workspace.t_y
		mov	eax,work.colour.base

		dec	esi
		mov	ebx,work.colour.stride_b

		mov	ecx,work.depth.base
		mov	edx,work.depth.stride_b

		imul	ebx,esi

		imul	edx,esi

		add	eax,ebx
		add	ecx,edx

		dec	eax
		sub	ecx,2

		mov	workspace.scanAddress,eax
		mov	workspace.depthAddress,ecx

	; Swap integer and fractional parts of major edge starting value and delta and z gradient
	;
	; This will cause carry into fractional part for negative gradients so
	; subtract one from the fractional part to adjust accordingly
	;


		mov	eax,work_main_i
		mov	ebx,work_main_d_i

		shl	eax,16
        mov     ecx,work_pu_grad_x

		mov	work.main.f,eax
        mov     edx,work_pv_grad_x

        shl ebx,16
        cmp     edx,80000000h

        adc     ecx,-1
        and     edx,000fffff0h

        rol     edx,12
        and     ecx,000fffff0h

        or      dx,cx

        ror     edx,16
        shr     ecx,16

        mov     work_pu_grad_x,ecx
        mov     work_pv_grad_x,edx

		mov	work.main.d_f,ebx
		mov	eax,workspace.flip


	; Check scan direction and use appropriate rasteriser
	;
		test eax,eax
		jnz	reversed

	STOP_TIMER	setup
	START_TIMER	outer

        TRAPEZIUM_ZTF_I8_D16_256 top,f
        TRAPEZIUM_ZTF_I8_D16_256 bot,f

	STOP_TIMER	outer
quit:
		ret

reversed:
	STOP_TIMER	setup
	START_TIMER	outer

        TRAPEZIUM_ZTF_I8_D16_256 top,b
        TRAPEZIUM_ZTF_I8_D16_256 bot,b

	STOP_TIMER	outer

		ret

TriangleRender_ZTF_I8_D16_256 endp

	endif

		end
