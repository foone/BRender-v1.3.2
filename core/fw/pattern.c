/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: pattern.c 1.1 1997/12/10 16:41:09 jon Exp $
 * $Locker: $
 *
 * Pattern matcher
 */
#include "fw.h"
#include "brassert.h"

BR_RCS_ID("$Id: pattern.c 1.1 1997/12/10 16:41:09 jon Exp $")

/*
 * Compare a pattern against a string
 *
 * Return true if pattern matches string
 *
 * Patterns have the magic characters '*' and '?'
 * 
 * '*' matches 0 or more characters
 * '?' matches any single character
 * '/' terminates the pattern
 *
 * Patterns are anchored at start and end of string
 *
 * Recursive approach, implemented with tail recursion -
 *
 * match (p,s) 
 * 
 *	case first(p) in
 *
 *	NULL	- TRUE if s is empty else FALSE
 *
 *	'*'		- TRUE if match rest(q) against any substring from s to end of s
 *				else
 *			  FALSE
 *
 *	'?'		- TRUE if first(s) != NULL and match(rest(p),rest(r))
 *
 *	default - TRUE if first(p) == first(s) and match(rest(p),rest(r))
 *
 */

#if 1 /* case insensitive for the moment */
#define UPPER(c) (( (c) >= 'a' && (c) <= 'z' )?((c) - ('a'-'A')):(c))
#define MATCH_CHAR(a,b) (UPPER(a) == UPPER(b))
#else
#define MATCH_CHAR(a,b) ((a) == (b))
#endif

br_boolean BR_RESIDENT_ENTRY BrNamePatternMatch(char *p, char *s)
{
	char *cp;

	/*
	 * A NULL pattern matches everything
	 */
	if(p == NULL)
		return BR_TRUE;

	/*
	 * A NULL string never matches
	 */
	if(s == NULL)
		return BR_FALSE;

	for(;;) switch(*p) {

	case '/':
	case '\0':
		/*
		 * Empty pattern only matches empty string
		 */
		return *s == '\0';

	case '*':
		/*
		 * Match p+1 in any position from s to end of s
		 */
		cp = s;
		do
			if(BrNamePatternMatch(p+1,cp))
				return 1;
		while (*cp++);

		return BR_FALSE;

	case '?':
		/*
		 * Match any character followed by matching(p+1, s+1)
		 */
		if(*s == '\0')
			return BR_FALSE;

		p++, s++;	/* Tail recurse */
		continue;	

	default:
		/*
		 * Match this character followed by matching(p+1, s+1)
		 */
		if(!MATCH_CHAR(*s,*p))
			return BR_FALSE;

		p++, s++;	/* Tail recurse */
		continue;
	}	
}

