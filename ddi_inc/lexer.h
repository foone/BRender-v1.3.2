/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: lexer.h 1.1 1997/12/10 16:41:08 jon Exp $
 * $Locker: $
 *
 * Generic lexer data structures
 */
#ifndef _LEXER_H_
#define _LEXER_H_

/*
 * Predefined tokens
 */
typedef enum br_lexer_token_id {
	T_EOF,

	/*
	 * Built in tokens
	 */
	T_ERROR,
	T_IDENT,
	T_STRING,
	T_INTEGER,
	T_REAL,

	/*
	 * Single characters
	 */
	T_EXCLAM	= '!',
	T_DQUOTE	= '"',
	T_HASH		= '#',
	T_DOLLAR	= '$',
	T_PERCENT	= '%',
	T_AMPERSAND	= '&',
	T_RQUOTE	= '\'',
	T_LPAREN	= '(',
	T_RPAREN	= ')',
	T_ASTERISK	= '*',
	T_PLUS		= '+',
	T_COMMA		= ',',
	T_DASH		= '-',
	T_PERIOD	= '.',
	T_SLASH		= '/',
	T_COLON		= ':',
	T_SEMICOLON	= ';',
	T_LANGLE	= '<',
	T_EQUAL		= '=',
	T_RANGLE	= '>',
	T_QUERY		= '?',
	T_COMAT		= '@',
	T_LSQUARE	= '[',
	T_BSLASH	= '\\',
	T_RSQUARE	= ']',
	T_CARET		= '^',
	T_UNDERBAR	= '_',
	T_LQUOTE	= '`',
	T_LBRACE	= '{',
	T_VBAR		= '|',
	T_RBRACE	= '}',
	T_TILDE		= '~',

	/*
	 * Keywords - start at 128
	 */
	T_KEYWORD = 128
} br_lexer_token_id;


/*
 * A token - id + value
 */
typedef struct br_lexer_token {
	/*
	 * Token ID
	 */
	br_lexer_token_id	id;

	/*
	 * Value
	 */
	union {
		/*
		 * Integer
		 */
		br_int_32		integer;

		/*
		 * Real number
		 */
		float			real;

		/*
		 * String 
		 */
		char *			string;
	} v;
} br_lexer_token;

typedef void BR_CALLBACK br_lexer_getchar_cbfn(struct br_lexer_source *source);
typedef void BR_CALLBACK br_lexer_error_cbfn(struct br_lexer *l, char *string);

typedef struct br_lexer {
	/*
	 * Current source of input charaters
	 */
	struct br_lexer_source *source;

	/*
	 * Current token
	 */
	br_lexer_token current;

	/*
	 * Keywords to recognise
	 */
	struct br_lexer_keyword *keywords;
	br_int_32 nkeywords;

	/*
	 * EOL comment character (or NULL if none)
	 */
	char eol_comment;

	/*
	 * Function to advance to next token
	 */
	void (*advance)(struct br_lexer *self);

	/*
	 * putline function for dump
	 */
	br_putline_cbfn *putline;
	void *putline_arg;

	/*
	 * Constructed string values are store here
	 */
	char *string_buffer;
	br_int_32 string_buffer_size;

	/*
	 * error handler
	 */
	br_lexer_error_cbfn *error;

} br_lexer;


typedef struct br_lexer_source {
	struct br_lexer_source *prev;

	/*
	 * Source name and line number for error reporting
	 */
	char *name;
	br_int_32 line;

	/*
	 * Next input character
	 */
	br_int_32 next;

	/*
	 * Function to update input character
	 */
	br_lexer_getchar_cbfn *getchar;

	/*
	 * Useful pointer for getchar fn.
	 */
	void *ptr;

} br_lexer_source;

typedef struct br_lexer_keyword {
	char *name;
	br_int_32 id;
} br_lexer_keyword;

/*
 * Macros for examining current token and advancing
 */
#define BrLexerCurrent(l)	((l)->current.id)
#define BrLexerString(l)	((l)->current.v.string)
#define BrLexerInteger(l)	((l)->current.v.integer)
#define BrLexerReal(l)		((l)->current.v.real)

#define BrLexerAdvance(l) ((l)->advance(l))

#define BrLexerExpect(l,t) do {if(BrLexerCurrent(l) != (t)) BrLexerTokenError(l,t); else BrLexerAdvance(l);} while(0)
#define BrLexerCheck(l,t) do {if(BrLexerCurrent(l) != (t)) BrLexerTokenError(l,t);} while(0)

#define BrLexerError(l,s) ((l)->error(l,s))

#endif

