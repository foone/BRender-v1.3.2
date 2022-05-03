/*
 * Copyright (c) 1992,1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: mscifix.h 1.1 1997/12/10 16:41:18 jon Exp $
 * $Locker: $
 *
 * Inline fixed point ops for Microsoft Visual C++
 *
 */
#pragma warning( disable : 4035 )
__inline long VCIFixedMul(long a, long b){
	__asm{
		mov eax,a
		mov ebx,b
		imul   ebx
        shrd   eax,edx,16
     }
}

__inline long VCIFixedDiv(long a, long b){
__asm{
		mov edx,a
		mov ebx,b
		mov	eax,edx 
		shl	eax,16 
		sar	edx,16 
		idiv ebx
     }
}

__inline long IFixedDivR(long a, long b){
__asm{
		mov eax,a
		mov ebx,b
		cdq
		xchg	edx,eax 
		shrd	eax,edx,16 
		sar	edx,16 
		idiv	ebx 
     }
}

__inline long IFixedDivF(long a, long b){
__asm{
		mov edx,a
		mov ebx,b
		xor	eax,eax 
		sar	edx,1 
		rcr	eax,1 
		div	ebx 
     }
}

__inline long IFixedDivP(long a, long b){
__asm{
		mov edx,a
		mov ebx,b
		mov	eax,edx 
		shl	eax,30 
		sar	edx,2 
		idiv	ebx 
		neg	eax 
		add	eax,eax 
		add	eax,080000000h 
     }
}

__inline long IFixedRcp(long a){
__asm{
		mov ebx,a
		mov	edx,1 
		xor	eax,eax 
		idiv	ebx 
     }
}

__inline long IFixedMulDiv(long a, long b, long c){
__asm{
		mov eax,a
		mov ebx,b
		mov ecx,c
		imul	ebx 
		idiv	ecx 
     }
}

__inline long IFixedMac2(long a, long b, long c, long d){
__asm{
		mov eax,a
		mov edx,b
		mov ecx,c
		mov ebx,d
		imul	edx
		shrd	eax,edx,16
		xchg	ecx,eax
		imul	ebx
		shrd	eax,edx,16
		add	eax,ecx
     }
}

__inline long IFixedMac3(long a, long b, long c, long d, long e, long f){
__asm{
		mov eax,a
		mov edx,b
		mov ecx,c
		mov ebx,d
		mov esi,e
		mov edi,f
		imul	edx
		shrd	eax,edx,16
		xchg	ecx,eax
		imul	ebx
		shrd	eax,edx,16
		add	ecx,eax
		mov	eax,esi
		imul	edi
		shrd	eax,edx,16
		add	eax,ecx
     }
}

__inline long IFixedFMac2(short a, long b, short c, long d){
__asm{
		mov ax,a
		mov edx,b
		mov cx,c
		mov ebx,d
		cwde
		imul	edx
		shrd	eax,edx,15
		xchg	ecx,eax
		cwde
		imul	ebx
		shrd	eax,edx,15
		add	eax,ecx
     }
}

__inline long IFixedFMac3(short a, long b, short c, long d, short e, long f){
__asm{
		mov ax,a
		mov edx,b
		mov cx,c
		mov ebx,d
		mov si,e
		mov edi,f
		cwde
		imul	edx
		shrd	eax,edx,15
		xchg	ecx,eax
		cwde
		imul	ebx
		shrd	eax,edx,15
		add	ecx,eax
		mov	eax,esi
		cwde
		imul	edi
		shrd	eax,edx,15
		add	eax,ecx
     }
}

__inline long IFixedSqr(long a){
__asm{
		mov eax,a
		imul	eax 
		shrd	eax,edx,16 
     }
}

__inline long IFixedSqr2(long a, long b){
__asm{
		mov eax,a
		mov ebx,b
		imul	eax
		xchg	ebx,eax
		mov	ecx,edx
		imul	eax
		add	eax,ebx
		adc	edx,ecx
		shrd	eax,edx,16
     }
}

__inline long IFixedSqr3(long a, long b, long c){
__asm{
		mov eax,a
		mov ebx,b
		mov esi,c
		imul	eax
		xchg	ebx,eax
		mov	ecx,edx
		imul	eax
		add	ebx,eax
		adc	ecx,edx
		mov	eax,esi
		imul	eax
		add	eax,ebx
		adc	edx,ecx
		shrd	eax,edx,16
     }
}

__inline long IFixedSqr4(long a, long b, long c, long d){
__asm{
		mov eax,a
		mov ebx,b
		mov esi,c
		mov edi,d
		imul	eax
		xchg	ebx,eax
		mov	ecx,edx
		imul	eax
		add	ebx,eax
		adc	ecx,edx
		mov	eax,esi
		imul	eax
		add	ebx,eax
		adc	ecx,edx
		mov	eax,edi
		imul	eax
		add	eax,ebx
		adc	edx,ecx
		shrd	eax,edx,16
     }
}
#pragma warning( default : 4035 )

#define BrFixedMul(a,b)					VCIFixedMul(a,b)
#define BrFixedMac2(a,b,c,d)	 		IFixedMac2(a,b,c,d)
#define BrFixedMac3(a,b,c,d,e,f)		IFixedMac3(a,b,c,d,e,f)
#define BrFixedMac4(a,b,c,d,e,f,g,h)	(IFixedMac2(a,b,c,d)+IFixedMac2(e,f,g,h))
#define BrFixedDiv(a,b)					VCIFixedDiv(a,b)
#define BrFixedDivR(a,b)				IFixedDivR(a,b)
#define BrFixedDivF(a,b)				IFixedDivF(a,b)
#define _FixedDivP(a,b)					IFixedDivP(a,b)
#define BrFixedMulDiv(a,b,c)			IFixedMulDiv(a,b,c)
#define BrFixedRcp(a)					IFixedRcp(a)

#define BrFixedSqr(a)					IFixedSqr(a)
#define BrFixedSqr2(a,b)				IFixedSqr2(a,b)
#define BrFixedSqr3(a,b,c)				IFixedSqr3(a,b,c)
#define BrFixedSqr4(a,b,c,d)			IFixedSqr4(a,b,c,d)

#define BrFixedFMac2(a,b,c,d)	 		IFixedFMac2(a,b,c,d)
#define BrFixedFMac3(a,b,c,d,e,f)		IFixedFMac3(a,b,c,d,e,f)
#define BrFixedFMac4(a,b,c,d,e,f,g,h)	(IFixedFMac2(a,b,c,d)+IFixedFMac2(e,f,g,h))

