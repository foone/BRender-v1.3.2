/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: brexcept.h 1.1 1997/12/10 16:41:04 jon Exp $
 * $Locker: $
 *
 * Simple exception handling
 */
#ifndef _BREXCEPT_H_
#define _BREXCEPT_H_

#ifndef __H2INC__
#include <setjmp.h>
#endif

/*
 * Exception types are the same as error types
 *
 * A value of zero is reserved
 */
typedef br_error br_exception;

/*
 * Exception handler - allocated as a resource 
 */
typedef struct br_exception_handler {
	struct br_exception_handler *prev;

	/*
	 * setjmp/longjmp context to throw to
	 */
#ifndef __H2INC__
	jmp_buf context;
#endif
} br_exception_handler;

/*
 * Public macros
 */
#define BrExceptionBegin(evp) (_BrExceptionValueFetch((br_exception)setjmp(_BrExceptionCatch()->context),(evp)))
#define BrExceptionEnd() _BrExceptionEnd()

#define BrExceptionThrow(et,ev) _BrExceptionThrow((br_exception)(ev),(void *)(ev))
#define BR_EXCEPTION_RESOURCE _BrExceptionResource()

#endif
