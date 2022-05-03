/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: token.c 1.1 1997/12/10 16:41:11 jon Exp $
 * $Locker: $
 *
 * Token management and manipulation
 */
#include "fw.h"
#include "brassert.h"

BR_RCS_ID("$Id: token.c 1.1 1997/12/10 16:41:11 jon Exp $")

/*
 * Private structure used to reference a registered token
 */
typedef struct br_token_entry {
	br_node node;				/* Linked list  						*/
	char *identifier;			/* Token name (including type suffix)	*/
	br_int_32 type;				/* Type of token						*/
	br_token token;				/* Actual token value					*/
	br_int_32 base_length;		/* Length of base identifier w/o type	*/
} br_token_entry;

/*
 * Predefined entries
 */
static br_token_entry predefinedTokens[] = {
#include "pretok.c"
};

/*
 * Token Types
 *
 * Type table, sorted by suffix length, largest first
 */
struct token_type {
	char		*identifier;
	br_token	type;
	int			length;
};

static struct token_type tokenTypes[] = {
#include "toktype.c"
};

/*
 * Initialise the token list
 */
void BrTokenBegin(void)
{
	int i;

	/*
	 * Set up token list
	 */
	BrNewList(&fw.tokens);

	/*
	 * Add all the predefined tokens
	 */
	for(i=0; i < BR_ASIZE(predefinedTokens); i++)
		BR_ADDHEAD(&fw.tokens, predefinedTokens+i);

	/*
	 * Setup next unused token id
	 */
	fw.next_free_token = NEXT_FREE_TOKEN;
}

/*
 * Convert a token name into a number, creating a new one if necessary
 */
br_token BR_RESIDENT_ENTRY BrTokenCreate(char *identifier, br_token type)
{
	br_token_entry *te;
	int i,l;

	l = BrStrLen(identifier);

	/*
	 * See if name already exists, if so, return that
	 */
	BR_FOR_LIST(&fw.tokens, te)
		if(!BrStrCmp(identifier,te->identifier))
			return te->token;


	if(type == BR_NULL_TOKEN) {
		/*
		 * If a null type is given, try to infer the type from the identifier's suffix
		 *
		 * This relies on the type table being sorted with longest suffixes first
		 */

		type = BRT_NONE;

		for(i=0; i < BR_ASIZE(tokenTypes); i++) {
			if(tokenTypes[i].length >= l)
				continue;

			if(!BrStrNCmp(identifier + l - tokenTypes[i].length,
				tokenTypes[i].identifier, tokenTypes[i].length)) {
				type = tokenTypes[i].type;
				break;
			}
		}
	} else {
		/*
		 * If a type is given, but suffix does not match, generate an error
		 */
		for(i=0; i < BR_ASIZE(tokenTypes); i++) {
			if(tokenTypes[i].type == type)
				break;
		}

		if(i >= BR_ASIZE(tokenTypes))
			/*
			 * Invalid type
			 */
			return BR_NULL_TOKEN;

		if(tokenTypes[i].length >= l)
			return BR_NULL_TOKEN;

		if(BrStrNCmp(identifier + l - tokenTypes[i].length,
			tokenTypes[i].identifier, tokenTypes[i].length)) {
			return BR_NULL_TOKEN;
		}
	}


	/*
	 * New name, add to list
	 */
	te = BrResAllocate(fw.res,sizeof(*te), BR_MEMORY_TOKEN);
	te->identifier = BrResStrDup(te, identifier);
	te->base_length = l - tokenTypes[i].length;
	te->type = type;
	te->token = fw.next_free_token++;

	BR_ADDHEAD(&fw.tokens, te);

	return te->token;
}

/*
 * Returns a pointer to the identifier for the token
 */
char * BR_RESIDENT_ENTRY BrTokenIdentifier(br_token t)
{
	br_token_entry *te;

	BR_FOR_LIST(&fw.tokens, te)
		if(t == te->token)
			return te->identifier;

	return NULL;
}

/*
 * Returns a token representing the type associated with the token
 */
br_token BR_RESIDENT_ENTRY BrTokenType(br_token t)
{
	br_token_entry *te;

	BR_FOR_LIST(&fw.tokens, te)
		if(t == te->token)
			return te->type;

	return BR_NULL_TOKEN;
}

/*
 * Count the number of registered tokens maching the given pattern
 */
br_int_32 BR_RESIDENT_ENTRY BrTokenCount(char *pattern)
{
	br_token_entry *te;
	int n = 0;

	if(pattern) {
		BR_FOR_LIST(&fw.tokens, te)
			if(BrNamePatternMatch(pattern,te->identifier))
				n++;
	} else {
		BR_FOR_LIST(&fw.tokens, te)
			n++;
	}
	return n;
}

/*
 * Find the first token matching the given pattern
 */
br_token BR_RESIDENT_ENTRY BrTokenFind(char *pattern)
{
	br_token_entry *te;

	/*
	 * Find item in list
	 */
	BR_FOR_LIST(&fw.tokens,te)
		if(BrNamePatternMatch(pattern,te->identifier))
			return te->token;

	return BR_NULL_TOKEN;
}

/*
 * Find all the tokens matching a given pattern
 */
br_int_32 BR_RESIDENT_ENTRY BrTokenFindMany(char *pattern, br_token *tokens, br_int_32 max_tokens)
{
	br_token_entry *te;
	int n=0;

	/*
	 * Find all matching items in list
	 */
	BR_FOR_LIST(&fw.tokens,te) {
		/*
		 * make sure there is space in output table
		 */
		if(n >= max_tokens)
			break;

		/*
		 * If entry matches, add to table
		 */
		if(BrNamePatternMatch(pattern,te->identifier)) {
			*tokens++ = te->token;
			n++;
		}
	}

	return n;
}

/*
 * Internal special case of find - look for a token given the base name and a list of acceptable types
 * (in order of importance)
 */
br_token BrTokenFindType(br_token *ptype, char *base, br_token *types, br_int_32 ntypes)
{
	br_token_entry *te;
	int l,t;

	l = BrStrLen(base);

	ASSERT(l >= 1);

	/*
	 * Find all matching items in list
	 */
	BR_FOR_LIST(&fw.tokens,te) {
		/*
		 * Check the basename
		 */
		if(l != te->base_length)
			continue;

		if(BrStrNICmp(te->identifier, base ,te->base_length))
			continue;

		/*
		 * Look through list of acceptable types
		 */
		for(t=0; t < ntypes; t++) {
			if(te->type == types[t]) {
				/*
				 * Got a match
				 */
				if(ptype)
					*ptype = te->type;

				return te->token;
			}
		}
	}

	return BR_NULL_TOKEN;
}
