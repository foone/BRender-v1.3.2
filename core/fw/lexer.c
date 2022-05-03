/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: lexer.c 1.1 1997/12/10 16:41:08 jon Exp $
 * $Locker: $
 *
 * Generic lexer
 */
#include "fw.h"
#include "brassert.h"

#define MAX_STRING 256

static void lexerAdvance(struct br_lexer * l);
static void lexerAdvanceDump(struct br_lexer * l);

/*
 * Default error handler, does nothing
 */
static void BR_CALLBACK lexerError(struct br_lexer *l, char *string)
{
}

struct br_lexer * BR_RESIDENT_ENTRY BrLexerAllocate(struct br_lexer_keyword *keywords, int nkeywords)
{
	struct br_lexer * l;

	l = BrResAllocate(fw.res,sizeof(*l), BR_MEMORY_LEXER);

	if(l == NULL)
		return NULL;

	l->keywords = keywords;
	l->nkeywords = nkeywords;

	l->advance = lexerAdvance;
	l->source = NULL;

	l->string_buffer = BrResAllocate(l, MAX_STRING, BR_MEMORY_LEXER);
	l->string_buffer_size = MAX_STRING;
	l->error = lexerError;
	return l;
}

char BR_RESIDENT_ENTRY BrLexerCommentSet(struct br_lexer * l, char eol_comment)
{
	char old = l->eol_comment;

	l->eol_comment = eol_comment;

	return old;
}

br_lexer_error_cbfn * BR_RESIDENT_ENTRY BrLexerErrorSet(struct br_lexer * l, br_lexer_error_cbfn *error)
{
	br_lexer_error_cbfn *old = l->error;

	if(error == NULL)
		l->error = lexerError;
	else
		l->error = error;

	if(old == lexerError)
		return NULL;
	else
		return old;
}

void BR_RESIDENT_ENTRY BrLexerFree(struct br_lexer * l)
{
	/*
	 * Free up lexer (will free up all sources and any files)
	 */
	BrResFree(l);
}

static void BR_CALLBACK fileGetchar(struct br_lexer_source *source)
{
	source->next = BrFileGetChar(source->ptr);
}

br_error BR_RESIDENT_ENTRY BrLexerPushFile(struct br_lexer * l, char *file)
{
	br_lexer_source *s;
 	void *f;
	int mode = BR_FS_MODE_TEXT;

	/*
	 * Try to open file
	 */
	f = BrFileOpenRead(file, 0, NULL, &mode);

	if(f == NULL)
		return BRE_FAIL;

	s = BrResAllocate(l, sizeof(*s), BR_MEMORY_LEXER);

	if(s == NULL)
		return BRE_NO_MEMORY;

	/*
	 * Attach file resource to source 
	 */
	BrResAdd(s,f);

	s->name = file;
	s->line = 1;
	s->getchar = fileGetchar;
	s->ptr = f;
	
	/*
	 * Setup first charater
	 */
	s->getchar(s);

	/*
	 * Link onto front of list of sources
	 */
	s->prev = l->source;
	l->source = s;

	return BRE_OK;
}

static void BR_CALLBACK stringGetchar(struct br_lexer_source *source)
{
	char *cp = source->ptr;

	if(cp == NULL || *cp == '\0') {
		source->next = BR_EOF;
	} else {
		source->next = *cp;
		source->ptr = cp+1;
	}
}

br_error BR_RESIDENT_ENTRY BrLexerPushString(struct br_lexer * l, char *string, char *name)
{
	br_lexer_source *s;

	s = BrResAllocate(l, sizeof(*s), BR_MEMORY_LEXER);

	if(s == NULL)
		return BRE_NO_MEMORY;

	s->name = name;
	s->line = 1;
	s->getchar = stringGetchar;
	s->ptr = string;
	
	/*
	 * Setup first charater
	 */
	s->getchar(s);

	/*
	 * Link onto front of list of sources
	 */
	s->prev = l->source;
	l->source = s;

	return BRE_OK;
}

struct br_lexer_source * BR_RESIDENT_ENTRY BrLexerPop(struct br_lexer * l)
{
	br_lexer_source *s;

	s = l->source;

	if(s == NULL)
		return s;

	/*
	 * Set current source to previous
	 */
	l->source = s->prev;
	
	BrResFree(s);

	return s;
}

static void lexerAdvance(struct br_lexer * l)
{
	int n;
	br_boolean got_point;

	for(;;) {

		if(l->source == NULL) {
			l->current.id = T_EOF;
			return;
		}

		if(l->source->next == BR_EOF) {
			/*
			 * End of current source - pop current and try again
			 */
			BrLexerPop(l);
			continue;

		} else if((l->eol_comment != '\0') && (l->source->next == l->eol_comment)) {
			/*
			 * Comment - skip until EOL or EOF
			 */
			for(;;) {
				l->source->getchar(l->source);
				if(l->source->next == '\n' ||
				   l->source->next == BR_EOF)
					break;
			}
			continue;

		} else if(l->source->next == '\n') {
			/*
			 * Newline - track line number
			 */
			l->source->line++;
			l->source->getchar(l->source);
			continue;

		} else if(l->source->next == '"') {
			/*
			 * Read a string - read characters until '"', newline, or EOF
			 */
			n = 0;
			for(n=0; n < l->string_buffer_size; ) {
				l->source->getchar(l->source);
				if(l->source->next == '"' ||
				   l->source->next == '\n' ||
				   l->source->next == BR_EOF)
					break;
				l->string_buffer[n++] = (char)l->source->next;
			}

			if(l->source->next != '"') {
				l->current.id = T_ERROR;
				l->current.v.string = "Unterminated string";
				l->source->getchar(l->source);
				return;
			}

			l->string_buffer[n] = '\0';
			l->current.v.string = l->string_buffer;
			l->current.id = T_STRING;
			l->source->getchar(l->source);
			return;

		} else if(BrIsSpace(l->source->next) || l->source->next == '\032') {
			/*
			 * white space - ignore
			 */
			l->source->getchar(l->source);
			continue;

		} else if(BrIsAlpha(l->source->next) || l->source->next == '_') {
			/*
			 * Idenitifier - read characters until not alpha, _ or 0-9
			 */
			l->string_buffer[0] = (char)l->source->next;

			for(n=1; n < l->string_buffer_size-1; ) {
				l->source->getchar(l->source);
				if(!(BrIsAlpha(l->source->next) ||
				     BrIsDigit(l->source->next) ||
				     l->source->next == '_'))
					break;
				l->string_buffer[n++] = (char)l->source->next;
			}
			l->string_buffer[n] = '\0';
			l->current.v.string = l->string_buffer;

			/*
			 * Try to look up a keyword
			 */
			for(n=0; n < l->nkeywords; n++) {
				if(l->keywords[n].name[0] == l->string_buffer[0] &&
					!BrStrICmp(l->keywords[n].name,l->string_buffer)) {
					/*
					 * Found a keyword - which are based at 128
					 */
					l->current.id = l->keywords[n].id;
					return;
				}
			}

			l->current.id = T_IDENT;
			l->current.v.string = l->string_buffer;
			return;

		} else if(BrIsDigit(l->source->next)) {
			/*
			 * Read a number
			 */
			l->string_buffer[0] = (char)l->source->next;
			l->source->getchar(l->source);
			got_point = BR_FALSE;

			for(n=1; n < l->string_buffer_size-1; n++) {
				
				switch(l->source->next) {
				case '0': case '1': case '2': case '3':	case '4': case '5':
				case '6': case '7': case '8': case '9':	case 'x': case 'X':
				case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
				case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
					l->string_buffer[n] = (char)l->source->next;
					l->source->getchar(l->source);
					break;
				
				case '.':
					got_point = BR_TRUE;
					l->string_buffer[n] = (char)l->source->next;
					l->source->getchar(l->source);
					break;

				default:
					goto done;
				}
			}
done:
			l->string_buffer[n] = '\0';

			/*
			 * If this is not a real number, then generate an integer token
			 */
			if(got_point) {
				l->current.v.real = BrStrToF(l->string_buffer, NULL);
				l->current.id = T_REAL;
				return;
			} else {
				l->current.v.integer = BrStrToL(l->string_buffer,0,0);
				l->current.id = T_INTEGER;
				return;
			}

		} else if(BrIsPrint(l->source->next)) {
			/*
			 * All other printable characters turn into tokens
			 */
			l->current.id = l->source->next;
			l->source->getchar(l->source);
			return;

		} else
			l->current.id = T_ERROR;
			l->current.v.string = "Unexpected character";
			l->source->getchar(l->source);
			return;
	}
}

#if DEBUG
static void lexerAdvanceDump(struct br_lexer * l)
{
	char *tname,*tvalue;
	char val[40];
	char tmp[256];

	/*
	 * Do the normal advance operation
	 */
	lexerAdvance(l);

	switch(l->current.id) {
	case T_EOF:
		tname = "EOF";
		tvalue = NULL;
		break;

	case T_IDENT:
		tname = "IDENT";
		tvalue = l->current.v.string;
		break;

	case T_ERROR:
		tname = "ERROR";
		tvalue = l->current.v.string;
		break;

	case T_STRING:
		tname = "STRING";
		tvalue = l->current.v.string;
		break;


	case T_REAL:
		tname = "REAL";
		BrSprintfN(val, BR_ASIZE(val), "%g", l->current.v.real);
		tvalue = val;
		break;

	case T_INTEGER:
		tname = "INTEGER";
		BrSprintfN(val, BR_ASIZE(val), "%d",l->current.v.integer);
		tvalue = val;
		break;

	default:
		if(l->current.id >= T_KEYWORD) {
			tname = l->current.v.string;
			tvalue = NULL;
		} else if(BrIsPrint(l->current.id)) {
			tname = val;
			val[0] = l->current.id;
			val[1] = '\0';
			tvalue = NULL;
		} else {
			BrSprintfN(val, BR_ASIZE(val), "??? %d", l->current.id);
			tname = val;
			tvalue = NULL;
		}
	}

	tmp[0] = '<';
	BrStrCpy(tmp+1,tname);

	if(tvalue) {
		BrStrCat(tmp,":");
		BrStrCat(tmp,tvalue);
	}
	BrStrCat(tmp,">");

	l->putline(tmp, l->putline_arg);
}	   	
#endif

br_error BR_RESIDENT_ENTRY BrLexerDumpSet(struct br_lexer * l, br_putline_cbfn *putline, void *putline_arg)
{
#if DEBUG
	if(putline) {
		l->advance = lexerAdvanceDump;
		l->putline = putline;
		l->putline_arg = putline_arg;
	} else {
		l->advance = lexerAdvance;
	}
#endif

	return BRE_OK;
}

void BR_RESIDENT_ENTRY BrLexerTokenError(struct br_lexer * l, br_lexer_token_id t)
{
 	int i;
	char tmp[256];

	if(l->current.id == T_ERROR) {
	} else 
	switch(t) {
	case T_EOF:
		BrLexerError(l, "expected end of file");
		break;

	case T_ERROR:
		BrLexerError(l, l->current.v.string);
		break;

	case T_IDENT:
		BrLexerError(l, "expected an identifier");
		break;

	case T_STRING:
		BrLexerError(l, "expected a string");
		break;

	case T_INTEGER:
		BrLexerError(l, "expected an integer");
		break;

	case T_REAL:
		BrLexerError(l, "expected a real number");
		break;

	default:
		if(t >= T_KEYWORD) {
			for(i=0; i < l->nkeywords; i++) {
				if(l->keywords[i].id == t)
					break;
			}

			if(i < l->nkeywords)
				BrSprintfN(tmp, BR_ASIZE(tmp), "expected '%s'", l->keywords[i].name);
			else
				BrSprintfN(tmp, BR_ASIZE(tmp), "expected unknown keyword %d", t);

		} else if(BrIsPrint(t)) {
			BrSprintfN(tmp, BR_ASIZE(tmp), "expected '%c'", t);
		} else {
			BrSprintfN(tmp, BR_ASIZE(tmp), "expected unknown token %d", t);
		}
		BrLexerError(l, tmp);
	}

	/*
	 * Advance until expected token or EOF
	 */
	while(BrLexerCurrent(l) != t && BrLexerCurrent(l) != T_EOF) {
		BrLexerAdvance(l);
	}
}

/*
 * Generate a string describing the current source position
 */
void BR_RESIDENT_ENTRY BrLexerPosition(struct br_lexer * l, char *buf, br_size_t buf_size)
{
	ASSERT(buf_size >= 1);

	if(l->source == NULL || l->source->name == NULL) {
		buf[0] = '\0'; 
		return;
	}

	BrSprintfN(buf, buf_size, "%s:%d ",l->source->name, l->source->line);
}

br_fixed_ls BR_RESIDENT_ENTRY BrParseFixed(struct br_lexer * l)
{
	br_boolean neg = BR_FALSE;
	br_fixed_ls x;

	if(BrLexerCurrent(l) == T_PLUS)
		BrLexerAdvance(l);

	if(BrLexerCurrent(l) == T_DASH) {
		neg = BR_TRUE;
		BrLexerAdvance(l);
	}

	if(BrLexerCurrent(l) == T_REAL) {
		x = BrFloatToFixed(BrLexerReal(l));
		BrLexerAdvance(l);
	} else if(BrLexerCurrent(l) == T_INTEGER) {
		x = BrIntToFixed(BrLexerInteger(l));
		BrLexerAdvance(l);
	} else {
		BrLexerError(l, "expecting a fixed");
		BrLexerAdvance(l);
                x = BrScalarToFixed(BR_SCALAR(0.0));
	}

	if(neg)
		return -x;
	else
		return x;
}

br_float BR_RESIDENT_ENTRY BrParseFloat(struct br_lexer * l)
{
	br_boolean neg = BR_FALSE;
	br_float f;

	if(BrLexerCurrent(l) == T_PLUS)
		BrLexerAdvance(l);

	if(BrLexerCurrent(l) == T_DASH) {
		neg = BR_TRUE;
		BrLexerAdvance(l);
	}

	if(BrLexerCurrent(l) == T_REAL) {
		f = BrLexerReal(l);
		BrLexerAdvance(l);
	} else if(BrLexerCurrent(l) == T_INTEGER) {
		f = (br_float)BrLexerInteger(l);
		BrLexerAdvance(l);
	} else {
		BrLexerError(l, "expecting a float");
		BrLexerAdvance(l);
		f = 0.0F;
	}

	if(neg)
		return -f;
	else
		return f;
}

br_int_32 BR_RESIDENT_ENTRY BrParseInteger(struct br_lexer * l)
{
	br_boolean neg = BR_FALSE;
	br_int_32 i;

	if(BrLexerCurrent(l) == T_PLUS)
		BrLexerAdvance(l);

	if(BrLexerCurrent(l) == T_DASH) {
		neg = BR_TRUE;
		BrLexerAdvance(l);
	}

	if(BrLexerCurrent(l) == T_INTEGER) {
		i = BrLexerInteger(l);
		BrLexerAdvance(l);
	} else {
		BrLexerError(l, "expecting an integer");
		BrLexerAdvance(l);
		i = 0;
	}

	if(neg)
		return -i;
	else
		return i;
}

br_int_32 BR_RESIDENT_ENTRY BrParseVectorFixed(struct br_lexer * l, br_fixed_ls *v, br_int_32 max)
{
	int n;

	BrLexerExpect(l, T_LSQUARE);

	for(n=0;n < max; n++) {
		*v++ = BrParseFixed(l);

		if(BrLexerCurrent(l) == T_RSQUARE)
			break;
		BrLexerExpect(l, T_COMMA);
	}

	BrLexerExpect(l, T_RSQUARE);
	return n+1;
}

br_int_32 BR_RESIDENT_ENTRY BrParseVectorFloat(struct br_lexer * l, br_float *v, br_int_32 max)
{
	int n;

	BrLexerExpect(l, T_LSQUARE);

	for(n=0;n < max; n++) {
		*v++ = BrParseFloat(l);

		if(BrLexerCurrent(l) == T_RSQUARE)
			break;
		BrLexerExpect(l, T_COMMA);
	}

	BrLexerExpect(l, T_RSQUARE);
	return n+1;
}

br_int_32 BR_RESIDENT_ENTRY BrParseMatrixFixed(struct br_lexer * l, br_fixed_ls *m, br_int_32 width, br_int_32 max_h)
{
	int n;

	BrLexerExpect(l, T_LSQUARE);

	for(n=0; n < max_h; n++, m+= width) {
		BrParseVectorFixed(l,m,width);

		if(BrLexerCurrent(l) == T_RSQUARE)
			break;

		BrLexerExpect(l, T_COMMA);
	}

	BrLexerExpect(l, T_RSQUARE);

	return n;
}

br_int_32 BR_RESIDENT_ENTRY BrParseMatrixFloat(struct br_lexer * l, br_float *m, br_int_32 width, br_int_32 max_h)
{
	int n;

	BrLexerExpect(l, T_LSQUARE);

	for(n=0; n < max_h; n++, m+= width) {
		BrParseVectorFloat(l,m,width);

		if(BrLexerCurrent(l) == T_RSQUARE)
			break;

		BrLexerExpect(l, T_COMMA);
	}

	BrLexerExpect(l, T_RSQUARE);

	return n;
}
