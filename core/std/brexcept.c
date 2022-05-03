/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: brexcept.c 1.1 1997/12/10 16:41:27 jon Exp $
 * $Locker: $
 *
 * Simple exception handling
 */
#include <setjmp.h>

#include "brddi.h"

BR_RCS_ID("$Id: brexcept.c 1.1 1997/12/10 16:41:27 jon Exp $")

/*
 * Current head of exception handler chain
 */
static br_exception_handler *_BrExceptionHandler = NULL;

/*
 * Temporary global used to pass exception value
 */
static void * exceptionValue;

/*
 * Add an exception handler to the stack - returns a point to exception handler
 * into which the caller should save context
 */
br_exception_handler * BR_RESIDENT_ENTRY _BrExceptionBegin(void)
{
	br_exception_handler *h;

	/*
	 * Create an anchor resource
	 */
	h = BrResAllocate(0,sizeof(*h), BR_MEMORY_EXCEPTION_HANDLER);
	h->prev = _BrExceptionHandler;

	_BrExceptionHandler = h;

	return _BrExceptionHandler;
}

void BR_RESIDENT_ENTRY _BrExceptionEnd(void)
{
	br_exception_handler *old;

	if(_BrExceptionHandler == NULL)
		BR_ERROR("Unhandled exception: %d");

	old = _BrExceptionHandler;

	_BrExceptionHandler = _BrExceptionHandler->prev;

	/*
	 * Release all the attached resources
	 */
	BrResFree(old);
}

/*
 *  Unwind back to the last exception handler, passing a type and value
 */
void BR_RESIDENT_ENTRY _BrExceptionThrow(br_int_32 type, void *value)
{
	br_exception_handler h,*old;

	/*
	 * Make sure exception stack does not underflow
	 */
	if(_BrExceptionHandler == NULL)
		BR_ERROR("Unhandled exception: %d");

	h = *_BrExceptionHandler;
	old = _BrExceptionHandler;

	_BrExceptionHandler = _BrExceptionHandler->prev;

	/*
	 * Release all the attached resources
	 */
	BrResFree(old);

	/*
	 * Pop stack back down to handler context
	 */
	exceptionValue = value;
	longjmp(h.context, type);
}

/*
 * Wrapper function used in BrExceptionCatch() macro to stash the
 * value
 *
 * 
 */
br_exception BR_RESIDENT_ENTRY _BrExceptionValueFetch(br_exception type, void **evp)
{
	/*
	 * If type is a valid exception, save the exception value
	 */
	if(type && evp)
		*evp = exceptionValue;

	return type;
}

void * BR_RESIDENT_ENTRY _BrExceptionResource(void)
{
	return _BrExceptionHandler;
}

