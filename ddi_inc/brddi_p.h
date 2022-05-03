/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: brddi_p.h 1.3 1998/11/12 13:14:55 johng Exp $
 * $Locker: $
 *
 * Prototypes for functions that are exposed to device drivers
 */
#ifndef _BRDDI_P_H_
#define _BRDDI_P_H_
#ifndef _NO_PROTOTYPES

#ifdef __cplusplus
extern "C" {
#endif

br_boolean BR_RESIDENT_ENTRY BrNamePatternMatch(char *p, char *s);

/*
 * Debugging printf
 */
int BR_RESIDENT_ENTRY BrLogPrintf(char *fmt,...);

/*
 * resource.c
 */
br_uint_32 BR_RESIDENT_ENTRY BrResCheck(void *vres, int no_tag);
void BR_RESIDENT_ENTRY BrResDump(void *vres, br_putline_cbfn *putline, void *arg);

/*
 * brexcept.c
 */
br_exception_handler * BR_RESIDENT_ENTRY _BrExceptionBegin(void);
void BR_RESIDENT_ENTRY _BrExceptionEnd(void);
void BR_RESIDENT_ENTRY _BrExceptionThrow(br_int_32 type, void *value);
br_exception BR_RESIDENT_ENTRY _BrExceptionValueFetch(br_exception type, void **evp);
void * BR_RESIDENT_ENTRY _BrExceptionResource(void);

/*
 * error.c
 */
void BR_RESIDENT_ENTRY BrLastErrorSet(br_error type, void *value);
br_error BR_RESIDENT_ENTRY BrLastErrorGet(void **valuep);

/*
 * token.c
 */
br_token BR_RESIDENT_ENTRY BrTokenCreate(char *identifier, br_token type);
char * BR_RESIDENT_ENTRY BrTokenIdentifier(br_token t);
br_token BR_RESIDENT_ENTRY BrTokenType(br_token t);
br_int_32 BR_RESIDENT_ENTRY BrTokenCount(char *pattern);
br_token BR_RESIDENT_ENTRY BrTokenFind(char *pattern);
br_int_32 BR_RESIDENT_ENTRY BrTokenFindMany(char *pattern, br_token *tokens, br_int_32 max_tokens);

/*
 * tokenval.c
 */
br_tv_template * BR_RESIDENT_ENTRY BrTVTemplateAllocate(void *res, br_tv_template_entry *entries, int n_entries);

void BR_RESIDENT_ENTRY BrTVTemplateFree(br_tv_template *t);

br_error BR_RESIDENT_ENTRY BrTokenValueQuery(
	br_uint_32 *pvalue, br_uint_32 *extra, br_size_t extra_size,
	br_token t,
	void *block,
    br_tv_template *_template);

br_error BR_RESIDENT_ENTRY BrTokenValueQueryMany(
	br_token_value *tv, 
	void *extra, br_size_t extra_size,
	br_int_32 *pcount,
	void *block,
    br_tv_template *_template);

br_error BR_RESIDENT_ENTRY BrTokenValueQueryManySize(
	br_size_t *psize,
	br_token_value *tv,
	void *block,
    br_tv_template *_template);

br_error BR_RESIDENT_ENTRY BrTokenValueQueryAll(
	br_token_value *buffer, br_size_t buffer_size,
	void *block,
    br_tv_template *_template);

br_error BR_RESIDENT_ENTRY BrTokenValueQueryAllSize(
	br_size_t *psize,
	void *block,
    br_tv_template *_template);

br_error BR_RESIDENT_ENTRY BrTokenValueSet(
	void *mem,
	br_uint_32 *pcombined_mask,
	br_token t,
	br_uint_32 value,
    br_tv_template *_template);

br_error BR_RESIDENT_ENTRY BrTokenValueSetMany(
	void *mem,
	br_int_32 *pcount,
	br_uint_32 *pcombined_mask,
	br_token_value *tv,
    br_tv_template *_template);

void BR_RESIDENT_ENTRY BrTokenValueDump(br_token_value *tv, char *prefix, br_putline_cbfn *putline, void *arg);
br_error BR_RESIDENT_ENTRY BrStringToTokenValue(br_token_value *buffer, br_size_t buffer_size, char *str);
br_boolean BR_RESIDENT_ENTRY BrTokenValueCompare(br_token_value *tv1, br_token_value *tv2);
br_boolean BR_RESIDENT_ENTRY BrTokenValueComparePartial(br_token_value *tv1, br_token_value *tv2, br_token *insignificant);


/*
 * diag.c
 */
void BR_RESIDENT_ENTRY _BrAssert(char *condition, char *file, unsigned line);
void BR_RESIDENT_ENTRY _BrUAssert(char *condition, char *file, unsigned line);

/*
 * lexer.c
 */
struct br_lexer * BR_RESIDENT_ENTRY BrLexerAllocate(struct br_lexer_keyword *keywords, int nkeywords);
void BR_RESIDENT_ENTRY BrLexerFree(struct br_lexer * l);
char BR_RESIDENT_ENTRY BrLexerCommentSet(struct br_lexer * l, char eol_comment);
br_lexer_error_cbfn * BR_RESIDENT_ENTRY BrLexerErrorSet(struct br_lexer * l, br_lexer_error_cbfn *error);
br_error BR_RESIDENT_ENTRY BrLexerPushFile(struct br_lexer * l, char *file);
br_error BR_RESIDENT_ENTRY BrLexerPushString(struct br_lexer * l, char *string, char *name);
struct br_lexer_source * BR_RESIDENT_ENTRY BrLexerPop(struct br_lexer * l);
br_error BR_RESIDENT_ENTRY BrLexerDumpSet(struct br_lexer * l, br_putline_cbfn *putline, void *putline_arg);
void BR_RESIDENT_ENTRY BrLexerTokenError(struct br_lexer * l, br_lexer_token_id t);
void BR_RESIDENT_ENTRY BrLexerPosition(struct br_lexer * l, char *buf, br_size_t buf_size);

br_int_32 BR_RESIDENT_ENTRY BrParseInteger(struct br_lexer * l);
br_float BR_RESIDENT_ENTRY BrParseFloat(struct br_lexer * l);
br_fixed_ls BR_RESIDENT_ENTRY BrParseFixed(struct br_lexer * l);

br_int_32 BR_RESIDENT_ENTRY BrParseVectorFloat(struct br_lexer * l, br_float *v, br_int_32 max);
br_int_32 BR_RESIDENT_ENTRY BrParseVectorFixed(struct br_lexer * l, br_fixed_ls *v, br_int_32 max);

br_int_32 BR_RESIDENT_ENTRY BrParseMatrixFloat(struct br_lexer * l, br_float *m, br_int_32 width, br_int_32 max_h);
br_int_32 BR_RESIDENT_ENTRY BrParseMatrixFixed(struct br_lexer * l, br_fixed_ls *m, br_int_32 width, br_int_32 max_h);

#if BASED_FIXED
#define BrParseScalar BrParseFixed
#define BrParseVector BrParseVectorFixed
#define BrParseMatrix BrParseMatrixFixed
#endif

#if BASED_FLOAT
#define BrParseScalar BrParseFloat
#define BrParseVector BrParseVectorFloat
#define BrParseMatrix BrParseMatrixFloat
#endif

/*
 * object.c
 */
br_error BR_CMETHOD_DECL(br_object,query)(struct br_object *self, br_uint_32 *pvalue, br_token t);
br_error BR_CMETHOD_DECL(br_object,queryBuffer)
	(struct br_object *self, br_uint_32 *pvalue, br_uint_32 *buffer, br_size_t buffer_size, br_token t);
br_error BR_CMETHOD_DECL(br_object,queryMany)
	(struct br_object *self, br_token_value *tv, void *extra, br_size_t extra_size, br_int_32 *pcount);
br_error BR_CMETHOD_DECL(br_object,queryManySize)
	(struct br_object *self, br_size_t *pextra_size, br_token_value *tv);
br_error BR_CMETHOD_DECL(br_object,queryAll)
	(struct br_object *self, br_token_value *buffer, br_size_t buffer_size);
br_error BR_CMETHOD_DECL(br_object,queryAllSize)
	(struct br_object *self, br_size_t *psize);

/*
 * objectc.c
 */
void * BR_RESIDENT_ENTRY BrObjectListAllocate(void *res);
br_error BR_CMETHOD_DECL(br_object_container, addFront)(struct br_object_container *self, struct br_object *ph);
br_error BR_CMETHOD_DECL(br_object_container, removeFront)(struct br_object_container *self, struct br_object **ph);
br_error BR_CMETHOD_DECL(br_object_container, remove)(struct br_object_container *self,	struct br_object *ph);
br_error BR_CMETHOD_DECL(br_object_container, find)(struct br_object_container *self,
	struct br_object **ph, br_token type, char *pattern, br_token_value *tv);
br_error BR_CMETHOD_DECL(br_object_container, findMany)(struct br_object_container *self,
	struct br_object **objects, br_int_32 max_objects, br_int_32 *num_objects, br_token type, char *pattern, br_token_value *tv);
br_error BR_CMETHOD_DECL(br_object_container, count)(struct br_object_container *self,
	br_uint_32 *pcount, br_token type, char *pattern, br_token_value *tv);

void * BR_CMETHOD_DECL(br_object_container, tokensMatchBegin)
		(struct br_object_container *self, br_token t, br_token_value *tv);
br_boolean BR_CMETHOD_DECL(br_object_container, tokensMatch)
		(struct br_object_container *self, struct br_object *h, void *arg);
void BR_CMETHOD_DECL(br_object_container, tokensMatchEnd)
		(struct br_object_container *self, void *arg);

br_error BR_RESIDENT_ENTRY BrObjectContainerFree(struct br_object_container *self, br_token type, char *pattern, br_token_value *tv);

/*
 * image.c
 */
struct br_image * BR_RESIDENT_ENTRY BrImageReference(char *name);
void BR_RESIDENT_ENTRY BrImageDereference(struct br_image *image);
void * BR_RESIDENT_ENTRY BrImageLookupName(struct br_image *img, char *name, br_uint_32 hint);
void * BR_RESIDENT_ENTRY BrImageLookupOrdinal(struct br_image *img, br_uint_32 ordinal);

br_boolean BR_RESIDENT_ENTRY BrImageAdd(br_image *ing);
br_boolean BR_RESIDENT_ENTRY BrImageRemove(br_image *img);

/*
 * Entry/exit tracking hooksfor Watcom C++
 */
void BR_RESIDENT_ENTRY _EPI(void);
void BR_RESIDENT_ENTRY _PRO(void);

/*
 * Registry lists
 */
void *BrRegistryClear(br_registry *reg);
int BrRegistryAddMany(br_registry *reg, void **item, int n);
int BrRegistryRemoveMany(br_registry *reg, void **item, int n);
void *BrRegistryFind(br_registry *reg, char *pattern);
int BrRegistryFindMany(br_registry *reg, char *pattern, void **item, int n);
int BrRegistryCount(br_registry *reg, char *pattern);
int BrRegistryEnum(br_registry *reg, char *pattern,
		br_enum_cbfn *callback, void *arg);

void *BrRegistryNew(br_registry *reg);
void *BrRegistryAdd(br_registry *reg, void *item);
void *BrRegistryRemove(br_registry *reg, void *item);

void *BrRegistryNewStatic(br_registry *reg, br_registry_entry *base, int limit);
void *BrRegistryAddStatic(br_registry *reg, br_registry_entry *base, void *item);
void *BrRegistryRemoveStatic(br_registry *reg, void *item);

/*
 * Nasty hack for finding out the last pixelmap that DevBegin() was called with
 */
br_pixelmap * BrDevLastBeginQuery(void);
void BrDevLastBeginSet(br_pixelmap *pm);

#ifdef __cplusplus
};
#endif
#endif
#endif

