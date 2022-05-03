/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: brassert.h 1.1 1997/12/10 16:41:03 jon Exp $
 * $Locker: $
 *
 * Assertion macros
 *
 *	ASSERT() is for internal checks
 *	UASSERT() is for user checks
 */

/*
 * This file should _NOT_ have checks for single inclusion -
 * it is legitimate to re-include it with DEBUG or PARANOID set diffently
 */
#undef ASSERT

#if DEBUG
#define ASSERT(e)	((void)((e) || (_BrAssert(#e,__FILE__,__LINE__),1)))
#define ASSERT_MESSAGE(m,e) ((void)((e) || (_BrAssert(m " : " #e,__FILE__,__LINE__),1)))
void BR_RESIDENT_ENTRY _BrAssert(char *condition, char *file, unsigned line);
#define VERIFY(f) ASSERT(f)

#else
#define ASSERT(ignore)	((void) 0)
#define ASSERT_MESSAGE(ignore,ignoreMeToo)  ((void) 0)
#define VERIFY(f) (void)(f)

#endif

#undef UASSERT

#if PARANOID
#define UASSERT(e)	((void)((e) || (_BrUAssert(#e,__FILE__,__LINE__),1)))
#define UASSERT_MESSAGE(m,e)      ((void)((e) || (_BrUAssert(m " : " #e,__FILE__,__LINE__),1)))
void BR_RESIDENT_ENTRY _BrUAssert(char *condition, char *file, unsigned line);
#define UVERIFY(f) ASSERT(f)

#else
#define UASSERT(ignore)	((void) 0)
#define UASSERT_MESSAGE(ignore,ignoreMeToo)  ((void) 0)
#define UVERIFY(f) (void)(f)
#endif

