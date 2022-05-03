/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: compiler.h 1.1 1997/12/10 16:41:17 jon Exp $
 * $Locker: $
 *
 * Misc host compiler configuration (types & special declarators etc.)
 */
#ifndef _COMPILER_H_
#define _COMPILER_H_

/*
 * Fixed bitsize integers
 */
typedef signed long br_int_32;
typedef unsigned long br_uint_32;

typedef signed short br_int_16;
typedef unsigned short br_uint_16;

typedef signed char br_int_8;
typedef unsigned char br_uint_8;

/*
 * Generic size type (in case target environment does not have size_t)
 */
typedef unsigned int br_size_t;

/*
 * Boolean type
 */
typedef int br_boolean;

#define BR_TRUE		1
#define BR_FALSE	0

#define BR_BOOLEAN(a)	((a) != 0)

/*
 * 32 bit floating point
 */
typedef float br_float;

/**
 ** Compiler dependant type specifiers
 **/

/*
 * WATCOM C/C++ 32
 */
#if defined(__WATCOMC__)

/*
 * versions <= 10.0  have cdecl wrong (trashes ebx)
 */
#if __WATCOMC__ <= 1000
#pragma aux __cdecl "_*" parm caller [] modify [eax ecx edx] ;
#endif

#if defined(__TARGET_MSC__)
#define stricmp _stricmp
#define strnicmp _strnicmp
#define memccpy _memccpy
#endif

/*
 * Function qualifiers
 */
/*
 * Public entry point into library
 */
#define BR_PUBLIC		__cdecl
#define BR_PUBLIC_VA	__cdecl /* varargs version */

/*
 * Entry point exported to drivers via modules
 */
#define BR_EXPORT		__cdecl
#define BR_EXPORT_VA 	__cdecl /* varargs version */

/*
 * A driver object method
 */
#define BR_METHOD		__cdecl
#define BR_METHOD_VA	__cdecl	/* varargs version */

/*
 * A user defined callback function
 */
#define BR_CALLBACK		__cdecl

#ifdef __cplusplus
#define BR_ASM_DATA
#define BR_ASM_DATA_EXTRA(x)
#else
#define BR_ASM_DATA __cdecl
#endif

#define BR_ASM_CALL __cdecl
#define BR_ASM_CALLBACK __cdecl

#define BR_SUFFIX_HOST "-WTC"

#define BR_HAS_FAR	1
/*
 * Stop unreferenced variables producing a warning
 * Things like "rcsid" and unused fucntion arguments
 */
#pragma off (unreferenced);

#define BR_ENDIAN_BIG		0
#define BR_ENDIAN_LITTLE	1

#ifndef __H2INC__
#pragma pack(4);
#endif

/*
 * Microsoft Visual C++
 */
#elif defined (_MSC_VER)

/*
 * Function qualifiers
 */
/*
 * Public entry point into library
 */
#define BR_PUBLIC		__cdecl
#define BR_PUBLIC_VA	__cdecl /* varargs version */

/*
 * Entry point exported to drivers via modules
 */
#define BR_EXPORT		__cdecl
#define BR_EXPORT_VA 	__cdecl /* varargs version */

/*
 * A driver object method
 */
#define BR_METHOD		__cdecl
#define BR_METHOD_VA	__cdecl	/* varargs version */

/*
 * A user defined callback function
 */
#define BR_CALLBACK		__cdecl

#define BR_ASM_DATA
#define BR_ASM_CALL __cdecl
#define BR_ASM_CALLBACK __cdecl

#define BR_SUFFIX_HOST "-VISUALC"

#define BR_HAS_FAR	0

#define BR_ENDIAN_BIG		0
#define BR_ENDIAN_LITTLE	1

#ifndef __H2INC__
#pragma warning(disable:4103)
#pragma pack(4)
#endif

/*
 * Borland BC 4
 */
#elif defined (__BORLANDC__)

/*
 * Function qualifiers
 */
/*
 * Public entry point into library
 */
#define BR_PUBLIC		__cdecl
#define BR_PUBLIC_VA	__cdecl /* varargs version */

/*
 * Entry point exported to drivers via modules
 */
#define BR_EXPORT		__cdecl
#define BR_EXPORT_VA 	__cdecl /* varargs version */

/*
 * A driver object method
 */
#define BR_METHOD		__cdecl
#define BR_METHOD_VA	__cdecl	/* varargs version */

/*
 * A user defined callback function
 */
#define BR_CALLBACK		__cdecl

#define BR_ASM_DATA __cdecl
#define BR_ASM_CALL __cdecl
#define BR_ASM_CALLBACK __cdecl

#define BR_SUFFIX_HOST "-BORLAND"

#define BR_HAS_FAR	0

#define BR_ENDIAN_BIG		0
#define BR_ENDIAN_LITTLE	1

#ifndef __H2INC__
#pragma option -a4
#pragma option -w-stu
#endif

/*
 * GNU C
 */
#elif defined (__GNUC__)

/*
 * Function qualifiers
 */
/*
 * Public entry point into library
 */
#define BR_PUBLIC		
#define BR_PUBLIC_VA	/* varargs version */

/*
 * Entry point exported to drivers via modules
 */
#define BR_EXPORT		
#define BR_EXPORT_VA 	/* varargs version */

/*
 * A driver object method
 */
#define BR_METHOD		
#define BR_METHOD_VA	/* varargs version */

/*
 * A user defined callback function
 */
#define BR_CALLBACK

#define BR_ASM_DATA
#define BR_ASM_CALL 
#define BR_ASM_CALLBACK 

#define BR_SUFFIX_HOST "-GCC"

#define BR_HAS_FAR	0

#define BR_ENDIAN_BIG		0
#define BR_ENDIAN_LITTLE	1

/*
 * IBM CSet++
 */
#elif defined (__IBMC__)

/*
 * Function qualifiers
 */
/*
 * Public entry point into library
 */
#define BR_PUBLIC		_System
#define BR_PUBLIC_VA	_System /* varargs version */

/*
 * Entry point exported to drivers via modules
 */
#define BR_EXPORT		_System
#define BR_EXPORT_VA 	_System /* varargs version */

/*
 * A driver object method
 */
#define BR_METHOD		_System
#define BR_METHOD_VA	_System	/* varargs version */

/*
 * A user defined callback function
 */
#define BR_CALLBACK		_System

#define BR_ASM_DATA
#define BR_ASM_CALL _System
#define BR_ASM_CALLBACK _System

#define BR_SUFFIX_HOST "-CSET"

#define BR_HAS_FAR	0

#define BR_ENDIAN_BIG		0
#define BR_ENDIAN_LITTLE	1

/*
 * Intel reference compiler
 */
#elif defined (___PROTON__)

/*
 * Function qualifiers
 */
/*
 * Function qualifiers
 */
/*
 * Public entry point into library
 */
#define BR_PUBLIC		__cdecl
#define BR_PUBLIC_VA	__cdecl /* varargs version */

/*
 * Entry point exported to drivers via modules
 */
#define BR_EXPORT		__cdecl
#define BR_EXPORT_VA 	__cdecl /* varargs version */

/*
 * A driver object method
 */
#define BR_METHOD		__cdecl
#define BR_METHOD_VA	__cdecl	/* varargs version */

/*
 * A user defined callback function
 */
#define BR_CALLBACK		__cdecl

#define BR_ASM_DATA __cdecl
#define BR_ASM_CALL __cdecl
#define BR_ASM_CALLBACK __cdecl

#define BR_SUFFIX_HOST "-PROTON"

#define BR_HAS_FAR	0

#define BR_ENDIAN_BIG		0
#define BR_ENDIAN_LITTLE	1

#endif


#if defined (__H2INC__)
/*
 * Avoid some tokens that masm chokes on
 */
#define align _align
#define seg _seg
#define offset _offset
#define type _type
#define size _size
#define page _page
#define mask _mask
#define state _state
#define ptr _ptr
#define a _a
#define b _b
#define c _c
#define width _width
#define end _end
#define out _out
#define str _str
#define ax _ax
#define bx _bx
#define cx _cx
#define dx _dx
#define si _si
#define di _di
#define bp _bp
#define ip _ip
#define sp _sp
#define eax _eax
#define ebx _ebx
#define ecx _ecx
#define edx _edx
#define esi _esi
#define edi _edi
#define ebp _ebp
#define eip _eip
#define esp _esp
#define al _al
#define bl _bl
#define cl _cl
#define dl _dl
#define ah _ah
#define bh _bh
#define ch _ch
#define dh _dh

#define es _es
#define cs _cs
#define ds _ds
#define ss _ss
#define fs _fs
#define gs _gs

#define low  _low
#define high _high



/*
 * Supress compiler specific declarators
 */
#undef BR_CALLBACK
#undef BR_ASM_DATA
#undef BR_ASM_CALL
#undef BR_ASM_CALLBACK

#define BR_CALLBACK
#define BR_ASM_DATA
#define BR_ASM_CALL
#define BR_ASM_CALLBACK

#endif

/*
 * Declare methods in C
 */
#define BR_CMETHOD_DECL(t,m)     	BR_METHOD _M_##t##_##m
#define BR_CMETHOD_VA_DECL(t,m)    	BR_METHOD _M_##t##_##m
#define BR_CMETHOD_PTR_DECL(t,m) 	(BR_METHOD * _##m)
#define BR_CMETHOD_VA_PTR_DECL(t,m) (BR_METHOD * _##m)

/*
 * Reference methods in C
 */

#define BR_CMETHOD(t,m)					(_M_##t##_##m)
#define BR_CMETHOD_REF(t,m) 		((void *)_M_##t##_##m)
#define BR_CMETHOD_CALL(t,m,o) 		(((t *)(o))->dispatch->_##m)

/*
 * Backwards compatability
 */
#define BR_RESIDENT_ENTRY BR_EXPORT
#define BR_PUBLIC_ENTRY BR_PUBLIC

#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG
#	define BR_SUFFIX_DEBUG "-DEBUG"
#else
#	define BR_SUFFIX_DEBUG ""
#endif

/*
 * Macros for producing banners & copyright messages
 */
#define BR_BANNER(title,year,revision)do {\
	static const char _revision[] = revision;\
	fprintf(stderr,title);\
	fwrite(_revision+10,1,sizeof(_revision)-12,stderr);\
	fprintf(stderr,"Copyright (C) " year " by Argonaut Technologies Limited\n");\
} while(0);

#if !defined(ADD_RCS_ID)
#define ADD_RCS_ID 0
#endif

#if ADD_RCS_ID
#define BR_RCS_ID(str) \
static const char rscid[] = str;
#else
#define BR_RCS_ID(str)
#endif

/*
 * Useful macro for sizing an array
 */
#define BR_ASIZE(a) (sizeof(a)/sizeof((a)[0]))

/*
 * Make sure NULL is defined
 */
#ifndef NULL
#define NULL	0
#endif

#endif
