/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: tokenval.c 1.3 1998/05/20 15:25:02 jon Exp $
 * $Locker: $
 *
 * Token/value list mangement
 */
#include "fw.h"
#include "brassert.h"

BR_RCS_ID("$Id: tokenval.c 1.3 1998/05/20 15:25:02 jon Exp $")

/*
 * Allocate a new template structure
 */
br_tv_template * BR_RESIDENT_ENTRY BrTVTemplateAllocate(void *res, br_tv_template_entry *entries, int n_entries)
{
    br_tv_template *t;

    t = BrResAllocate(res,sizeof(*t), BR_MEMORY_TOKEN_TEMPLATE);

    t->res = t;
    t->entries = entries;
    t->n_entries = n_entries;
    return t;
}

void BR_RESIDENT_ENTRY BrTVTemplateFree(br_tv_template *t)
{
    BrResFree(t);
}


/*
 * Convert any token names in template into token ids.
 */
static void templateResolveNames(br_tv_template *template)
{
	br_tv_template_entry *tp;
	int n;

	for(n = 0, tp = template->entries; n < template->n_entries; tp++, n++) {

		if(tp->name == NULL)
			continue;

		tp->token = BrTokenCreate(tp->name, BR_NULL_TOKEN);
	}
}

/*
 * Convert a template into a cached map
 */
static void templateMakeMap(br_tv_template *template)
{
	br_tv_template_entry *tp;
	char *map;
	br_int_32 min,max;
	int i,n,e;

	ASSERT(template);
	ASSERT(template->n_entries > 0);

	if(!template->names_resolved)
		templateResolveNames(template);

	/*
	 * Find range of tokens used 
	 */
	min = max = template->entries->token;

	tp = template->entries;
	for(n = 0 ; n < template->n_entries; tp++, n++) {

		if(tp->token == BR_NULL_TOKEN)
			continue;

		if(tp->token < min)
			min = tp->token;
		if(tp->token > max)
			max = tp->token;
	}

	 n = (max-min)+1;

	/*
	 * Allocate the map
	 */
	template->map_base = min;
	template->n_map_entries = n;

	map = BrResAllocate(template->res,
		n * (sizeof(*template->map_query_entry) +
			sizeof(*template->map_set_entry)),
		BR_MEMORY_TOKEN_MAP);

	template->map_query_entry = (void *)map;
	map += n * sizeof(*template->map_query_entry);
	template->map_set_entry = (void *)map;

	for(i=0; i < n; i++) {
		template->map_query_entry[i] = NULL;
		template->map_set_entry[i] = NULL;
	}

	/*
	 * Fill in map entries
	 */
	for(n=0, tp = template->entries; n < template->n_entries; tp++, n++) {

		if(tp->token == BR_NULL_TOKEN)
			continue;
		/*
		 * Check range
		 */
		ASSERT(tp->token >= template->map_base);
		ASSERT(tp->token < (template->map_base + template->n_map_entries));

		/*
		 * Check for daft combinations of bits
		 */
		ASSERT(!((tp->flags & BRTV_SET) && (tp->conv == BRTV_CONV_DIRECT)));

		e = tp->token - template->map_base;

		if(tp->flags & BRTV_QUERY)
			template->map_query_entry[e] = tp;

		if(tp->flags & BRTV_SET)
			template->map_set_entry[e] = tp;
	}
}

/*
 * Copy values onto the end of a buffer, whilst converting from float to fixed.
 *
 * The buffer pointer and remaining space are passed in by reference - and are
 * updated according to the size of the block
 *
 * Returns pointer to start of destination block, or NULL if there was not
 * enough space
 */
static br_fixed_ls * ConvertFloatToFixed(br_fixed_ls **pextra, br_float *src, br_int_32 count, br_size_t *pextra_space)
{
	br_fixed_ls *ret;

	if(pextra == NULL || *pextra == NULL || pextra_space == NULL)
		return NULL;

	ret = *pextra;
	/*
	 * Check there is space for the data
	 */
	if((count * sizeof(br_fixed_ls)) > *pextra_space)
		return NULL;

	*pextra_space -= count * sizeof(br_fixed_ls);

	while(count--)
		*(*pextra)++ = BrFloatToFixed(*src++);
		
	return ret;
}

/*
 * Copy values onto the end of a buffer, whilst converting from fixed to float
 *
 * The buffer pointer and remaining space are passed in by reference - and are
 * updated according to the size of the block
 *
 * Returns pointer to start of destination block, or NULL if there was not
 * enough space
 */
static br_float * ConvertFixedToFloat(br_float **pextra, br_fixed_ls *src, br_int_32 count, br_size_t *pextra_space)
{
	br_float *ret;

	if(pextra == NULL || *pextra == NULL || pextra_space == NULL)
		return NULL;

	ret = *pextra;

	/*
	 * Check there is space for the data
	 */
	if((count * sizeof(**pextra)) > *pextra_space)
		return NULL;

	*pextra_space -= count * sizeof(**pextra);

	while(count--)
		*(*pextra)++ = (float)BrFixedToFloat(*src++);
		
	return ret;
}

/*
 * Copy longword values onto the end of a buffer
 *
 * The buffer pointer and remaining space are passed in by reference - and are
 * updated according to the size of the block
 *
 * Returns pointer to start of destination block, or NULL if there was not
 * enough space
 */
static br_uint_32 * ConvertLongCopy(br_uint_32 **pextra, br_uint_32 *src, br_int_32 count, br_size_t *pextra_space)
{
	br_uint_32 *ret;

	if(pextra == NULL || *pextra == NULL || pextra_space == NULL)
		return NULL;

	ret = *pextra;

	/*
	 * Check there is space for the data
	 */
	if((count * sizeof(**pextra)) > *pextra_space)
		return NULL;

	*pextra_space -= count * sizeof(**pextra);

	while(count--)
		*(*pextra)++ = *src++;
		
	return ret;
}

/*
 * Fetch one value according to template information
 *
 * Returns:
 * 	BRE_OK 		if conversion suceeded
 *  BRE_UNKNOWN	if conversion type was not recognised
 *	BRE_OVERFLOW if extra buffer space was exhausted
 */
#define MEM(t)		(*(t *)(mem))
#define MEM_P(t)	((t *)(mem))

static br_error ValueQuery(
	br_token_value *tv,			/* Destination for value					*/
	void **pextra,				/* Buffer for additional value data			*/
	br_size_t *pextra_size,
	void *block,				/* Base of block associated with template	*/
	br_tv_template_entry *tep)	/* Template entry to use for conversion		*/
{
	void *mem;
	br_uint_32 *lp;
	int t;
	br_tv_custom *custp;

	/*
	 * Setup memory pointer
	 */
	if(tep->flags & BRTV_ABS)
		mem = (char *)&tep->offset;
	else
		mem = (char *)block + tep->offset;

	switch(tep->conv) {
	case BRTV_CONV_COPY:
		tv->v.i32 = MEM(br_int_32);
		break;

	case BRTV_CONV_DIRECT:
		tv->v.i32 = tep->conv_arg;
		break;
		
	case BRTV_CONV_I32_I8:
		tv->v.i32 = MEM(br_int_8);
		break;
	case BRTV_CONV_I32_I16:
		tv->v.i32 = MEM(br_int_16);
		break;

	case BRTV_CONV_I32_U8:
		tv->v.i32 = MEM(br_uint_8);
		break;
	case BRTV_CONV_I32_U16:
		tv->v.i32 = MEM(br_uint_16);
		break;

	case BRTV_CONV_BIT:
		tv->v.i32 = (MEM(br_uint_32) & tep->conv_arg) != 0;
		break;

	case BRTV_CONV_NOT_BIT:
		tv->v.i32 = (MEM(br_uint_32) & tep->conv_arg) == 0;
		break;

	case BRTV_CONV_FIXED_FLOAT:
		tv->v.x = BrFloatToFixed(MEM(br_float));
		break;
	case BRTV_CONV_FLOAT_FIXED:
		tv->v.f = (float)BrFixedToFloat(MEM(br_fixed_ls));
		break;

/*
 * XXX Assumes that vectors and matrix elements are contiguous
 */
	case BRTV_CONV_V2_FIXED_FLOAT:
		if((tv->v.p = ConvertFloatToFixed((br_fixed_ls **)pextra, MEM_P(br_float), 2, pextra_size)) == NULL)
			return BRE_OVERFLOW;
		break;

	case BRTV_CONV_V2_FLOAT_FIXED:
		if((tv->v.p = ConvertFixedToFloat((br_float **)pextra, MEM_P(br_fixed_ls), 2, pextra_size)) == NULL)
			return BRE_OVERFLOW;
		break;

	case BRTV_CONV_V3_FIXED_FLOAT:
		if((tv->v.p = ConvertFloatToFixed((br_fixed_ls **)pextra, MEM_P(br_float), 3, pextra_size)) == NULL)
			return BRE_OVERFLOW;
		break;

	case BRTV_CONV_V3_FLOAT_FIXED:
		if((tv->v.p = ConvertFixedToFloat((br_float **)pextra, MEM_P(br_fixed_ls), 3, pextra_size)) == NULL)
			return BRE_OVERFLOW;
		break;

	case BRTV_CONV_V4_FIXED_FLOAT:
		if((tv->v.p = ConvertFloatToFixed((br_fixed_ls **)pextra, MEM_P(br_float), 4, pextra_size)) == NULL)
			return BRE_OVERFLOW;
		break;

	case BRTV_CONV_V4_FLOAT_FIXED:
		if((tv->v.p = ConvertFixedToFloat((br_float **)pextra, MEM_P(br_fixed_ls), 4, pextra_size)) == NULL)
			return BRE_OVERFLOW;
		break;

	case BRTV_CONV_M23_FIXED_FLOAT:
		if((tv->v.p = ConvertFloatToFixed((br_fixed_ls **)pextra, MEM_P(br_float), 6, pextra_size)) == NULL)
			return BRE_OVERFLOW;
		break;

	case BRTV_CONV_M23_FLOAT_FIXED:
		if((tv->v.p = ConvertFixedToFloat((br_float **)pextra, MEM_P(br_fixed_ls), 6, pextra_size)) == NULL)
			return BRE_OVERFLOW;
		break;

	case BRTV_CONV_M4_FIXED_FLOAT:
		if((tv->v.p = ConvertFloatToFixed((br_fixed_ls **)pextra, MEM_P(br_float), 16, pextra_size)) == NULL)
			return BRE_OVERFLOW;
		break;

	case BRTV_CONV_M4_FLOAT_FIXED:
		if((tv->v.p = ConvertFixedToFloat((br_float **)pextra, MEM_P(br_fixed_ls), 16, pextra_size)) == NULL)
			return BRE_OVERFLOW;
		break;

	case BRTV_CONV_M34_FIXED_FLOAT:
		if((tv->v.p = ConvertFloatToFixed((br_fixed_ls **)pextra, MEM_P(br_float), 12, pextra_size)) == NULL)
			return BRE_OVERFLOW;
		break;

	case BRTV_CONV_M34_FLOAT_FIXED:
		if((tv->v.p = ConvertFixedToFloat((br_float **)pextra, MEM_P(br_fixed_ls), 12, pextra_size)) == NULL)
			return BRE_OVERFLOW;
		break;

	case BRTV_CONV_V2_COPY:
		if((tv->v.p = ConvertLongCopy((br_uint_32 **)pextra, MEM_P(br_uint_32), 2, pextra_size)) == NULL)
			return BRE_OVERFLOW;
		break;

	case BRTV_CONV_V3_COPY:
		if((tv->v.p = ConvertLongCopy((br_uint_32 **)pextra, MEM_P(br_uint_32), 3, pextra_size)) == NULL)
			return BRE_OVERFLOW;
		break;

	case BRTV_CONV_V4_COPY:
		if((tv->v.p = ConvertLongCopy((br_uint_32 **)pextra, MEM_P(br_uint_32), 4, pextra_size)) == NULL)
			return BRE_OVERFLOW;
		break;

	case BRTV_CONV_M23_COPY:
		if((tv->v.p = ConvertLongCopy((br_uint_32 **)pextra, MEM_P(br_uint_32), 6, pextra_size)) == NULL)
			return BRE_OVERFLOW;
		break;

	case BRTV_CONV_M4_COPY:
		if((tv->v.p = ConvertLongCopy((br_uint_32 **)pextra, MEM_P(br_uint_32), 16, pextra_size)) == NULL)
			return BRE_OVERFLOW;
		break;

	case BRTV_CONV_M34_COPY:
		if((tv->v.p = ConvertLongCopy((br_uint_32 **)pextra, MEM_P(br_uint_32), 12, pextra_size)) == NULL)
			return BRE_OVERFLOW;
		break;

	case BRTV_CONV_STRING:
		/*
		 * longword align the destination string (so that further data does not get
		 * out of alignment
		 */
		t = BrStrLen(MEM(char *))+1;
		t = (t+(sizeof(br_uint_32)-1))/sizeof(br_uint_32);
		
		if((tv->v.p = ConvertLongCopy((br_uint_32 **)pextra, MEM(br_uint_32 *), t, pextra_size)) == NULL)
			return BRE_OVERFLOW;

		break;

	case BRTV_CONV_LIST:
		/*
		 * Find number of entries in list
		 */
		lp = MEM(br_uint_32 *);

		t = 0;
		if(lp) {
			while(*lp++)
				t++;

			if((tv->v.p = ConvertLongCopy((br_uint_32 **)pextra, MEM(br_uint_32 *), t+1, pextra_size)) == NULL)
				return BRE_OVERFLOW;
		} else {
			/*
			 * Make a list with 0 entries
			 */
			if((tv->v.p = ConvertLongCopy((br_uint_32 **)pextra, (br_uint_32 *)&lp, 1, pextra_size)) == NULL)
				return BRE_OVERFLOW;
		}
		break;

	case BRTV_CONV_BOOL_TOKEN:
		tv->v.b = (MEM(br_token) != BRT_NONE);
		break;

	case BRTV_CONV_CUSTOM:
		/*
		 * Use 'conv_arg' as a pointer to a block of function pointers
		 */
		custp = (br_tv_custom *)tep->conv_arg;
		if(custp && custp->query)
			return custp->query(&tv->v.u32,
				pextra, pextra_size, block, tep);

		/* FALL THROUGH */
	default:
		/*
		 * unrecognised operation (usually NOP)
		 */
		return BRE_UNKNOWN;
	}

	return BRE_OK;
}

/*
 * Set one value
 */
static br_error ValueSet(
	void *block,				/* Destination memeory block				*/
	br_token_value *tv,			/* Source value								*/
	br_tv_template_entry *tep)	/* Template entry to use for conversion		*/
{
	void *mem;
	int i,t;
	br_tv_custom *custp;

	/*
	 * Setup pointer to memory
	 */
	if(tep->flags & BRTV_ABS)
		return BRE_FAIL;

	mem = (char *)block + tep->offset;

	switch(tep->conv) {
	case BRTV_CONV_COPY:
		MEM(br_int_32) = tv->v.i32;
		break;
		
	case BRTV_CONV_I32_I8:
		MEM(br_int_8) = (br_int_8)tv->v.i32;
		break;
	case BRTV_CONV_I32_I16:
		MEM(br_int_16) = (br_int_16)tv->v.i32;
		break;

	case BRTV_CONV_I32_U8:
		MEM(br_uint_8) = (br_uint_8)tv->v.i32;
		break;
	case BRTV_CONV_I32_U16:
		MEM(br_uint_16) = (br_uint_16)tv->v.i32;
		break;

	case BRTV_CONV_BIT:
		if(tv->v.i32)
			MEM(br_uint_32) |= tep->conv_arg;
		else
			MEM(br_uint_32) &= ~(tep->conv_arg);
		break;

	case BRTV_CONV_NOT_BIT:
		if(!tv->v.i32)
			MEM(br_uint_32) |= tep->conv_arg;
		else
			MEM(br_uint_32) &= ~(tep->conv_arg);
		break;

	case BRTV_CONV_FIXED_FLOAT:
		MEM(br_float) = (br_float)BrFixedToFloat(tv->v.x);
		break;
	case BRTV_CONV_FLOAT_FIXED:
		MEM(br_fixed_ls) = BrFloatToFixed(tv->v.f);
		break;

	case BRTV_CONV_V2_FIXED_FLOAT:
		t = 2;
		goto convert_to_float;
		break;

	case BRTV_CONV_V2_FLOAT_FIXED:
		t = 2;
		goto convert_to_fixed;
		break;

	case BRTV_CONV_V3_FIXED_FLOAT:
		t = 3;
		goto convert_to_float;
		break;

	case BRTV_CONV_V3_FLOAT_FIXED:
		t = 3;
		goto convert_to_fixed;
		break;

	case BRTV_CONV_V4_FIXED_FLOAT:
		t = 4;
		goto convert_to_float;
		break;


	case BRTV_CONV_V4_FLOAT_FIXED:
		t = 4;
		goto convert_to_fixed;
		break;

	case BRTV_CONV_M23_FIXED_FLOAT:
		t = 6;
		goto convert_to_float;
		break;

	case BRTV_CONV_M23_FLOAT_FIXED:
		t = 6;
		goto convert_to_fixed;
		break;

	case BRTV_CONV_M4_FIXED_FLOAT:
		t = 16;
		goto convert_to_float;
		break;

	case BRTV_CONV_M4_FLOAT_FIXED:
		t = 16;
		goto convert_to_fixed;
		break;

	case BRTV_CONV_M34_FIXED_FLOAT:
		t = 12;
	convert_to_float:
		for(i = 0; i < t; i++)
			MEM_P(br_float)[i] = (br_float)BrFixedToFloat(((br_fixed_ls *)(tv->v.p))[i]);
		break;

	case BRTV_CONV_M34_FLOAT_FIXED:
		t = 12;
	convert_to_fixed:
		for(i = 0; i < t; i++)
			MEM_P(br_fixed_ls)[i] = BrFloatToFixed(((br_float *)(tv->v.p))[i]);
		break;

	case BRTV_CONV_V2_COPY:
		t = 2;
		goto copy_words;

	case BRTV_CONV_V3_COPY:
		t = 3;
		goto copy_words;

	case BRTV_CONV_V4_COPY:
		t = 4;
		goto copy_words;

	case BRTV_CONV_M23_COPY:
		t = 6;
		goto copy_words;

	case BRTV_CONV_M4_COPY:
		t = 16;
		goto copy_words;

	case BRTV_CONV_M34_COPY:
		t = 12;
	copy_words:
		for(i = 0; i < t; i++)
			MEM_P(br_uint_32)[i] = ((br_uint_32 *)(tv->v.p))[i];
		break;

	case BRTV_CONV_STRING:
		BrStrNCpy(MEM(char *),tv->v.str,BR_MAX_IDENTIFIER);
		break;

	case BRTV_CONV_BOOL_TOKEN:
		if(tv->v.b)
			MEM(br_token) = tep->conv_arg;
		else
			MEM(br_token) = BRT_NONE;

		break;

	case BRTV_CONV_CUSTOM:
		custp = (br_tv_custom *)tep->conv_arg;

		if(custp && custp->set)
			return custp->set(block, &tv->v.u32, tep);

		/* FALL THROUGH */

	default:
		/*
		 * unrecognised operation (usually NOP)
		 */
		return BRE_UNKNOWN;
	}
	
	return BRE_OK;
}

/*
 * Find out how much extra buffer space a conversion would require
 */
static br_size_t ValueExtraSize(void *block, br_tv_template_entry *tep)
{
	void *mem;
	br_uint_32 *lp;
	int t;
	br_tv_custom *custp;

	/*
	 * Setup memory pointer
	 */
	if(tep->flags & BRTV_ABS)
		mem = (char *)&tep->offset;
	else
		mem = (char *)block + tep->offset;

	switch(tep->conv) {

	case BRTV_CONV_V2_FIXED_FLOAT:
	case BRTV_CONV_V2_FLOAT_FIXED:
	case BRTV_CONV_V2_COPY:
	  	return 2 * sizeof(br_uint_32);

	case BRTV_CONV_V3_FIXED_FLOAT:
	case BRTV_CONV_V3_FLOAT_FIXED:
	case BRTV_CONV_V3_COPY:
	  	return 3 * sizeof(br_uint_32);

	case BRTV_CONV_V4_FIXED_FLOAT:
	case BRTV_CONV_V4_FLOAT_FIXED:
	case BRTV_CONV_V4_COPY:
	  	return 4 * sizeof(br_uint_32);

	case BRTV_CONV_M23_FIXED_FLOAT:
	case BRTV_CONV_M23_FLOAT_FIXED:
	case BRTV_CONV_M23_COPY:
	  	return 6 * sizeof(br_uint_32);

	case BRTV_CONV_M4_FIXED_FLOAT:
	case BRTV_CONV_M4_FLOAT_FIXED:
	case BRTV_CONV_M4_COPY:
	  	return 16 * sizeof(br_uint_32);

	case BRTV_CONV_M34_FIXED_FLOAT:
	case BRTV_CONV_M34_FLOAT_FIXED:
	case BRTV_CONV_M34_COPY:
	  	return 12 * sizeof(br_uint_32);

	case BRTV_CONV_STRING:
		/*
		 * longword align the destination string (so that further data does not get
		 * out of alignment
		 */
		t = BrStrLen(MEM(char *))+1;
		t = (t+(sizeof(br_uint_32)-1))/sizeof(br_uint_32);
		
		return t * sizeof(br_uint_32);

	case BRTV_CONV_LIST:
		/*
		 * Find number of entries in list
		 */
		lp = MEM(br_uint_32 *);

		t = 1;
		if(lp) {
			while(*lp++)
				t++;
		}
		return t * sizeof(br_uint_32);

	case BRTV_CONV_CUSTOM:
		custp = (br_tv_custom *)tep->conv_arg;

		if(custp && custp->extra_size)
			return custp->extra_size(block, tep);
		else
			return 0;
	}

	return 0;
}

#undef VALUE
#undef MEM
#undef mem

/*
 * Get the value coresponding to one token
 */
br_error BR_RESIDENT_ENTRY BrTokenValueQuery(
	br_uint_32 *pvalue, br_uint_32 *extra, br_size_t extra_size,
	br_token t,
	void *block,
	br_tv_template *template)
{
	br_int_32 o;
	br_tv_template_entry *tep;
	br_token_value tv;
	br_error r;

	/*
	 * Make sure map info is built
	 */
	if(template->n_map_entries == 0)
		templateMakeMap(template);

	/*
	 * Convert token into offset
	 */
	o = t - template->map_base;

	/*
	 * If offset is outside range, return an error
	 */
	if(o < 0 || o >= template->n_map_entries)
		return BRE_UNKNOWN;

	tep = template->map_query_entry[o];

	if(tep) {
		r = ValueQuery(&tv, &extra, &extra_size, block, tep);
		*pvalue = tv.v.u32;
		return r;
	} else
		return BRE_UNKNOWN;
}

/*
 * Fetch values into a token/value list (with optional extra buffer) according
 * to the given template
 */
br_error BR_RESIDENT_ENTRY BrTokenValueQueryMany(
	br_token_value *tv, 
	void *extra, br_size_t extra_size,
	br_int_32 *pcount,
	void *block,
	br_tv_template *template)
{
	br_int_32 o;
	br_tv_template_entry *tep;
	br_error r;

	/*
	 * Make sure map info is built
	 */
	if(template->n_map_entries == 0)
		templateMakeMap(template);

	*pcount = 0;

	for(; tv->t != BR_NULL_TOKEN; tv++) {
		/*
		 * Convert token into offset
		 */
		o = tv->t - template->map_base;

		/*
		 * If offset is outside range, ignore token
		 */
		if(o < 0 || o >= template->n_map_entries)
			continue;

		tep = template->map_query_entry[o];

		if(tep == NULL)
			continue;

		r = ValueQuery(tv, &extra, &extra_size, block, tep);

		if(r == BRE_OK)
			(*pcount)++;
	}
	return BRE_OK;
}

/*
 * Work out how much extra storage would be needed for the values described in the given
 * token/value list
 */
br_error BR_RESIDENT_ENTRY BrTokenValueQueryManySize(
	br_size_t *psize,
	br_token_value *tv,
	void *block,
	br_tv_template *template)
{
	br_int_32 o;
	br_size_t extra_size = 0;

	/*
	 * Make sure map info is built
	 */
	if(template->n_map_entries == 0)
		templateMakeMap(template);

	for(; tv->t != BR_NULL_TOKEN; tv++) {
		/*
		 * Convert token into offset
		 */
		o = tv->t - template->map_base;

		/*
		 * If offset is outside range, ignore token
		 */
		if(o < 0 || o >= template->n_map_entries)
			continue;

		if(template->map_query_entry[o])
			extra_size += ValueExtraSize(block, template->map_query_entry[o]);
	}

	*psize = extra_size;

	return BRE_OK;
}

br_error BR_RESIDENT_ENTRY BrTokenValueQueryAll(
	br_token_value *buffer, br_size_t buffer_size,
	void *block,
	br_tv_template *template)
{
	br_tv_template_entry *tp;
	int n,e;
	br_token_value *tv;
	char *extra;
	br_size_t extra_size;
	br_error r;

	/*
	 * Put tokens at front of buffer
	 */
	tv = buffer;

	/*
	 * Make sure all token names are resolved
	 */
	if(!template->names_resolved)
		templateResolveNames(template);

	/*
	 * Count number of token/value pairs
	 */
	n = 1; /* Account for terminator */
	for(e = 0, tp = template->entries; e < template->n_entries; tp++, e++)
		if((tp->token != BR_NULL_TOKEN) && (tp->flags & BRTV_ALL))
			n++;

	/*
	 * Start using extra space after token/value pairs
	 * (including the terminator)
	 */
	extra = (char *)(buffer + n);

	if(buffer_size < sizeof(*buffer) * n)
		return BRE_OVERFLOW;

	extra_size = buffer_size - sizeof(*buffer) * n;

	for(e = 0, tp = template->entries; e < template->n_entries; tp++, e++) {

		if(tp->token == BR_NULL_TOKEN)
			continue;

		/*
		 * Ignore template entry if it is not flagged for 'ALL'
		 */
		if(!(tp->flags & BRTV_ALL))
			continue;

		/*
		 * Store token
		 */
		tv->t = tp->token;

		/*
		 * Fetch value
		 */
		r = ValueQuery(tv, &extra, &extra_size, block, tp);

		if(r != BRE_OK)
			return r;

		tv++;
	}

	/*
	 * Terminate list
	 */
	tv->t = BR_NULL_TOKEN;

	return BRE_OK;
}

br_error BR_RESIDENT_ENTRY BrTokenValueQueryAllSize(
	br_size_t *psize,
	void *block,
	br_tv_template *template)
{
	br_tv_template_entry *tp;
	int e,n;
	br_size_t extra_size;

	n = 1;
	extra_size = 0;

	/*
	 * Make sure all token names are resolved
	 */
	if(!template->names_resolved)
		templateResolveNames(template);

	for(e = 0, tp = template->entries; e < template->n_entries; tp++, e++) {

		if(tp->token == BR_NULL_TOKEN)
			continue;

		if(!(tp->flags & BRTV_ALL))
			continue;
		
		n ++;

		/*
		 * Account for any extra info. needed by tokens
		 */
		extra_size += ValueExtraSize(block, tp);
	}

	/*
	 * Total memory required is 2 words for each pair, one
	 * word for the terminator, and any extra bytes
	 */
	*psize = n * sizeof(br_token_value) + extra_size;

	return BRE_OK;
}

br_error BR_RESIDENT_ENTRY BrTokenValueSet(
	void *mem,
	br_uint_32 *pcombined_mask,
	br_token t,
	br_uint_32 value,
	br_tv_template *template)
{
	br_int_32 o;
	br_error r;
	br_token_value tv;

	tv.t = t;
	tv.v.u32 = value;

	/*
	 * Make sure map info is built
	 */
	if(template->n_map_entries == 0)
		templateMakeMap(template);

	/*
	 * Convert token into offset
	 */
	o = t - template->map_base;

	/*
	 * If offset is outside range, ignore token
	 */
	if(o < 0 || o >= template->n_map_entries)
		return BRE_UNKNOWN;

	if(template->map_set_entry[o])  {
		r = ValueSet(mem, &tv, template->map_set_entry[o]);
		if(r != BRE_OK)
			return r;

		/*
		 * Accumlate a mask of visted entries
		 */
		*pcombined_mask |= template->map_set_entry[o]->mask;

		return BRE_OK;
	} else {
		return BRE_UNKNOWN;
	}
}

br_error BR_RESIDENT_ENTRY BrTokenValueSetMany(
	void *mem,
	br_int_32 *pcount,
	br_uint_32 *pcombined_mask,
	br_token_value *tv,
	br_tv_template *template)
{
	br_int_32 o,n;
	br_error r;
	br_uint_32 cm = 0;

	/*
	 * Make sure map info is built
	 */
	if(template->n_map_entries == 0)
		templateMakeMap(template);

	n = 0;

	for(; tv->t != BR_NULL_TOKEN != 0; tv++) {
		/*
		 * Convert token into offset
		 */
		o = tv->t - template->map_base;

		/*
		 * If offset is outside range, ignore token
		 */
		if(o < 0 || o >= template->n_map_entries)
			continue;

		if(template->map_set_entry[o])  {
			r = ValueSet(mem, tv, template->map_set_entry[o]);
			if(r != BRE_OK)
				continue;

			/*
			 * Accumlate a mask of visted entries
			 */
			cm |= template->map_set_entry[o]->mask;
			n++;
		}
	}


	if(pcount)
		*pcount = n;
	if(pcombined_mask)
		*pcombined_mask |= cm;

	return BRE_OK;
}

#if DEBUG

static void DumpMatrixInteger(br_int_32 *ip, int rows, int cols,
	char *prefix, char *info_0, char *info_n,
	br_putline_cbfn *putline, void *arg)
{
	int i,j;
	char *cp;
	char value[128];

	for(i=0; i < rows; i++) {
		cp = value+BrSprintf(value,"%s%s[",
			prefix,
			(i == 0)?info_0:info_n);
		
		for(j=0; j < cols; j++)
			cp += BrSprintf(cp,"%s%d",(j != 0)?",":"",*ip++);

		BrStrCpy(cp,"]");

		putline(value,arg);
	}
}

static void DumpMatrixFixed(br_fixed_ls *xp, int rows, int cols,
	char *prefix, char *info_0, char *info_n,
	br_putline_cbfn *putline, void *arg)
{
	int i,j;
	char *cp;
	char value[128];

	for(i=0; i < rows; i++) {
		cp = value+BrSprintf(value,"%s%s[",
			prefix,
			(i == 0)?info_0:info_n);
		
		for(j=0; j < cols; j++)
			cp += BrSprintf(cp,"%s%f",(j != 0)?",":"",BrFixedToFloat(*xp++));

		BrStrCpy(cp,"]");

		putline(value,arg);
	}
}

static void DumpMatrixFloat(br_float *fp, int rows, int cols,
	char *prefix, char *info_0, char *info_n,
	br_putline_cbfn *putline, void *arg)
{
	int i,j;
	char *cp;
	char value[128];

	for(i=0; i < rows; i++) {
		cp = value+BrSprintf(value,"%s%s[",
			prefix,
			(i == 0)?info_0:info_n);
		
		for(j=0; j < cols; j++)
			cp += BrSprintf(cp,"%s%f",(j != 0)?",":"",*fp++);

		BrStrCpy(cp,"]");

		putline(value,arg);
	}
}

static void DumpObject(br_object *h,
	char *prefix, char *info,
	br_putline_cbfn *putline, void *arg)
{
	char value[128];
	char *dev_ident;
	char *h_ident;
	br_object *dev;

	if(h == NULL) {
		BrSprintf(value,"%s%sNULL OBJECT",
			prefix, info);
	} else {
		dev = (br_object *)ObjectDevice(h);
		if(dev)
			dev_ident = ObjectIdentifier(dev);
		else
			dev_ident = "<NULL DEVICE>";
	
		if(dev_ident == NULL)
			dev_ident = "<NULL>";

		h_ident = ObjectIdentifier(h);

		if(h_ident == NULL)
			h_ident = "<NULL>";

		BrSprintf(value,"%s%s%08x %s:%s  %s",
			prefix, info,h,
			dev_ident,h_ident,
			BrTokenIdentifier(ObjectType(h)));
	}

	putline(value,arg);
}
#endif

/*
 * Dumps a token/value list using a supplied putline function
 */
void BR_RESIDENT_ENTRY BrTokenValueDump(br_token_value *tv, char *prefix, br_putline_cbfn *putline, void *arg)
{
#if DEBUG
	char *id;
	char value[128],tmp[128],info_0[64],*info_n,*pvalue;
	int n,i;
	br_object **pph;
	void **ppp;
	br_token *pt;

#define VALUE(t)	(*(t *)(tv+1))

	info_n = "                                         ";

	for(n=0 ; tv->t != BR_NULL_TOKEN; tv++, n++) {
		id = BrTokenIdentifier(tv->t);

		/*
		 * catch unknown tokens
		 */
		if(id == NULL) {
			BrSprintf(tmp,"%s  %5d:  ??? (%08x) %08x", prefix, n, tv[0], tv[1]);
			putline(tmp,arg);
			continue;
		}
	
		i = BrSprintf(info_0,"%5d:  %-32s ",n, id);

		/*
		 * Convert the token's value into a string
		 */
		pvalue = value;

		switch(BrTokenType(tv->t)) {
		case BRT_NONE:
			value[0] = '\0';
			break;

		case BRT_BOOLEAN:
			pvalue = tv->v.b?"TRUE":"FALSE";
			break;

		case BRT_POINTER:
		case BRT_HANDLE:
			BrSprintf(value,"%08x",tv->v.u32);
			break;

		case BRT_TOKEN:
			pvalue = BrTokenIdentifier(tv->v.t);
			if(pvalue == NULL) {
				BrSprintf(value,"??? %d", tv->v.t);
				pvalue = value;
			}
			break;

		case BRT_INT_8:
			BrSprintf(value,"%d",tv->v.i8);
			break;

		case BRT_UINT_8:
			BrSprintf(value,"%u (0x%02x)",tv->v.u8,tv->v.u8);
			break;

		case BRT_INT_16:
			BrSprintf(value,"%d",tv->v.i16);
			break;

		case BRT_UINT_16:
			BrSprintf(value,"%u (0x%04x)",tv->v.u16,tv->v.u16);
			break;

		case BRT_INT_32:
			BrSprintf(value,"%d",tv->v.i32);
			break;

		case BRT_UINT_32:
			BrSprintf(value,"%u (0x%08x)",tv->v.u32,tv->v.u32);
			break;

		case BRT_FIXED:
			BrSprintf(value,"%f",BrFixedToFloat(tv->v.x));
			break;

		case BRT_FLOAT:
			BrSprintf(value,"%f",tv->v.f);
			break;

		case BRT_ANGLE:
			BrSprintf(value,"%f",BrAngleToDegree(tv->v.a));
			break;

		case BRT_COLOUR_RGB:
			BrSprintf(value,"(%d,%d,%d)",BR_RED(tv->v.rgb),BR_GRN(tv->v.rgb),BR_BLU(tv->v.rgb));
			break;

		case BRT_OBJECT:
			DumpObject(tv->v.o, prefix, info_0, putline, arg);
			continue;

		case BRT_STRING:
			if(tv->v.str == NULL) {
				pvalue = "NULL";
				break;
			}

			pvalue = tv->v.str;
			break;

		case BRT_CONSTANT_STRING:
			if(tv->v.cstr == NULL) {
				pvalue = "NULL";
				break;
			}

			pvalue = tv->v.cstr;
			break;

		case BRT_OBJECT_LIST:
			if(tv->v.ol == NULL) {
				pvalue = "NULL";
				break;
			}

			pph = tv->v.ol;

			if(*pph == NULL) {
				pvalue = "Empty List";
				break;
			}

			for(i=0; *pph != NULL ; i++, pph++)
				DumpObject(*pph, prefix, (i == 0) ? info_0:info_n, putline, arg);

			continue;

		case BRT_TOKEN_LIST:
			if(tv->v.tl == NULL) {
				pvalue = "NULL";
				break;
			}

			pt = tv->v.tl;

			if(*pt == 0) {
				pvalue = "Empty List";
				break;
			}

			for(i=0; *pt != BR_NULL_TOKEN ; i++, pt++) {
				pvalue = BrTokenIdentifier(*pt);

				if(pvalue == NULL) {
					BrSprintf(value,"??? %d", *pt);
					pvalue = value;
				}

				BrSprintf(tmp,"%s%s%s",prefix, (i == 0)?info_0:info_n, pvalue);
				putline(tmp,arg);
			}

			continue;

        case BRT_TOKEN_VALUE_LIST:
            if(tv->v.tvl == NULL) {
				pvalue = "NULL";
				break;
			}

            BrTokenValueDump(tv->v.tvl, prefix, putline, arg);

			continue;

		case BRT_POINTER_LIST:
			if(tv->v.pl == NULL) {
				pvalue = "NULL";
				break;
			}

			ppp = tv->v.pl;

			if(*ppp == NULL) {
				pvalue = "Empty List";
				break;
			}

			for(i=0; *ppp != NULL ; i++, ppp++) {

				BrSprintf(value,"%08x", *ppp);
				pvalue = value;

				BrSprintf(tmp,"%s%s%s",prefix, (i == 0)?info_0:info_n, pvalue);
				putline(tmp,arg);
			}
			continue;

		case BRT_VECTOR2_INTEGER:
			if(tv->v.p == NULL) {
				pvalue = "NULL";
				break;
			}

			DumpMatrixInteger((br_int_32 *)tv->v.p,1,2, prefix, info_0, info_n, putline, arg);
			continue;

		case BRT_VECTOR3_INTEGER:
			if(tv->v.p == NULL) {
				pvalue = "NULL";
				break;
			}

			DumpMatrixInteger((br_int_32 *)tv->v.p,1,3, prefix, info_0, info_n, putline, arg);
			continue;

		case BRT_VECTOR4_INTEGER:
			if(tv->v.p == NULL) {
				pvalue = "NULL";
				break;
			}

			DumpMatrixInteger((br_int_32 *)tv->v.p,1,4, prefix, info_0, info_n, putline, arg);
			continue;

		case BRT_VECTOR2_FIXED:
			if(tv->v.p == NULL) {
				pvalue = "NULL";
				break;
			}

			DumpMatrixFixed((br_fixed_ls *)tv->v.p,1,2, prefix, info_0, info_n, putline, arg);
			continue;

		case BRT_VECTOR3_FIXED:
			if(tv->v.p == NULL) {
				pvalue = "NULL";
				break;
			}

			DumpMatrixFixed((br_fixed_ls *)tv->v.p,1,3, prefix, info_0, info_n, putline, arg);
			continue;

		case BRT_VECTOR4_FIXED:
			if(tv->v.p == NULL) {
				pvalue = "NULL";
				break;
			}

			DumpMatrixFixed((br_fixed_ls *)tv->v.p,1,4, prefix, info_0, info_n, putline, arg);
			continue;

		case BRT_VECTOR2_FLOAT:
			if(tv->v.p == NULL) {
				pvalue = "NULL";
				break;
			}

			DumpMatrixFloat((br_float *)tv->v.p,1,2, prefix, info_0, info_n, putline, arg);
			continue;

		case BRT_VECTOR3_FLOAT:
			if(tv->v.p == NULL) {
				pvalue = "NULL";
				break;
			}

			DumpMatrixFloat((br_float *)tv->v.p,1,3, prefix, info_0, info_n, putline, arg);
			continue;

		case BRT_VECTOR4_FLOAT:
			if(tv->v.p == NULL) {
				pvalue = "NULL";
				break;
			}

			DumpMatrixFloat((br_float *)tv->v.p,1,4, prefix, info_0, info_n, putline, arg);
			continue;

		case BRT_MATRIX23_FIXED:
			if(tv->v.p == NULL) {
				pvalue = "NULL";
				break;
			}

			DumpMatrixFixed((br_fixed_ls *)tv->v.p,3,2, prefix, info_0, info_n, putline, arg);
			continue;

		case BRT_MATRIX34_FIXED:
			if(tv->v.p == NULL) {
				pvalue = "NULL";
				break;
			}

			DumpMatrixFixed((br_fixed_ls *)tv->v.p,4,3, prefix, info_0, info_n, putline, arg);
			continue;

		case BRT_MATRIX4_FIXED:
			if(tv->v.p == NULL) {
				pvalue = "NULL";
				break;
			}

			DumpMatrixFixed((br_fixed_ls *)tv->v.p,4,4, prefix, info_0, info_n, putline, arg);
			continue;

		case BRT_MATRIX23_FLOAT:
			if(tv->v.p == NULL) {
				pvalue = "NULL";
				break;
			}

			DumpMatrixFloat((br_float *)tv->v.p,3,2, prefix, info_0, info_n, putline, arg);
			continue;

		case BRT_MATRIX34_FLOAT:
			if(tv->v.p == NULL) {
				pvalue = "NULL";
				break;
			}

			DumpMatrixFloat((br_float *)tv->v.p,4,3, prefix, info_0, info_n, putline, arg);
			continue;

		case BRT_MATRIX4_FLOAT:
			if(tv->v.p == NULL) {
				pvalue = "NULL";
				break;
			}

			DumpMatrixFloat((br_float *)tv->v.p,4,4, prefix, info_0, info_n, putline, arg);
			continue;
		}

		/*
		 * Print the token and value out
		 */
		BrSprintf(tmp,"%s%s%s",prefix, info_0, pvalue);
		putline(tmp,arg);
	}
#endif
}
#undef VALUE

/*
 * Convert a NUL terminated string into a token/value list
 *
 * String is parsed as:
 * tv_string:	<tv_pair> { ',' <tv_pair> }
 * tv_pair:		<tidentifier>
 * 				<tidentifier> ':' <t_value>
 * 				<tidentifier> '=' <t_value>
 * tidentifier:	identifier
 * 				"w"
 * 				"h"
 * 				"m"
 * 				"l"
 * t_value:		integer
 * 				float
 * 				'"' string '"'			
 * 				identifer
 *				"true"
 *				"t"
 *				"false"
 *				"f"
 */
static br_error	parseTokenValue(br_lexer *l, br_token_value *tv, br_size_t size);

/*
 * Keyword tokens for lexer
 */
enum {
	T_FALSE = T_KEYWORD+0,
	T_TRUE = T_KEYWORD+1,
};

br_error BR_RESIDENT_ENTRY BrStringToTokenValue(br_token_value *buffer, br_size_t buffer_size, char *str)
{
	br_lexer *l;
	br_error r;
	static br_lexer_keyword keywords[] = {
		{"true",	T_TRUE },
		{"t",		T_TRUE },
		{"false",	T_FALSE },
		{"f",		T_FALSE },
	};

	l = BrLexerAllocate(keywords, BR_ASIZE(keywords));
	if(l == NULL)
		return BRE_NO_MEMORY;

	r = BrLexerPushString(l,str, NULL);

	if(r != BRE_OK) {
	 	BrLexerFree(l);
		return r;
	}

	/*
	 * Start up the pipleline
	 */
	BrLexerAdvance(l);

	parseTokenValue(l, buffer, buffer_size);

	BrLexerFree(l);

	return BRE_OK;
}

static br_error	parseTokenValue(br_lexer *l, br_token_value *tv, br_size_t size)
{
	int len;
	char name[40];
	br_token type;
	static br_token real_types[] = {BRT_FLOAT, BRT_FIXED};
	br_error r = BRE_OK;
	br_boolean negative;

	/*
	 * Check that there is enough space for terminator
	 */
	if(size < sizeof(br_token_value))
		return BRE_OVERFLOW;
	
	size -= sizeof(br_token_value);

	while (BrLexerCurrent(l) != T_EOF) {
		/*
		 * Consume 0 or more commas
		 */
		while(BrLexerCurrent(l) == T_COMMA)
			BrLexerAdvance(l);

		if(BrLexerCurrent(l) != T_IDENT)
			break;

		/*
		 * Check there is space for token/value and terminator
		 */
		if(size <= sizeof(br_token_value)) {
			r = BRE_OVERFLOW;
			break;
		}

		/*
		 * token name
		 */
		name[BR_ASIZE(name)-1] = '\0';
		BrStrNCpy(name,BrLexerString(l), BR_ASIZE(name)-1);
		BrLexerAdvance(l);

#if 1
		/*
		 * Convert some magic names for backwards compatibility
		 */
		if(name[1] == '\0') {
			if(name[0] == 'w' || name [0] == 'W')
				BrStrCpy(name,"width");
			else if(name[0] == 'h' || name [0] == 'H')
				BrStrCpy(name,"height");
			else if(name[0] == 'b' || name [0] == 'B')
				BrStrCpy(name,"pixel_bits");
			else if(name[0] == 'm' || name [0] == 'M')
				BrStrCpy(name,"mode");
		}
#endif

		/*
		 * Optional value
		 */
		tv->t = BR_NULL_TOKEN;

		if(BrLexerCurrent(l) == T_COLON ||
			   BrLexerCurrent(l) == T_EQUAL) {
			BrLexerAdvance(l);

			/*
			 * Check for negative scalar types
			 */
			negative = BrLexerCurrent(l) == T_DASH;

			if (negative) {

				BrLexerAdvance(l);

				if (BrLexerCurrent(l) != T_INTEGER && BrLexerCurrent(l) != T_REAL)
					break;
			}

			switch(BrLexerCurrent(l)) {

			/*
			 * Look for float or fixed
			 */
			case T_REAL: {
				static br_token real_types[] = {BRT_FLOAT, BRT_FIXED};

				tv->t = BrTokenFindType(&type, name, real_types, BR_ASIZE(real_types));

				if(tv->t == BR_NULL_TOKEN) 
					break;

				switch(type) {
				case BRT_FLOAT:
					tv->v.f = negative? -BrLexerReal(l): BrLexerReal(l);
					break;
				case BRT_FIXED:
					tv->v.x = BrFloatToFixed(negative? -BrLexerReal(l): BrLexerReal(l));
					break;
				}
				break;
			}

			/*
			 * if > 0 Look for i32, ui32, float, fixed or boolean
			 * else Look for i32, float, fixed or boolean
			 */
			case T_INTEGER: {
				static br_token pos_int_types[] = {
					BRT_INT_32, BRT_UINT_32, BRT_INT_16, BRT_UINT_16,
					BRT_INT_8, BRT_UINT_8, BRT_FLOAT, BRT_FIXED, BRT_BOOLEAN,
					BRT_POINTER, BRT_OBJECT, BRT_HANDLE};
				static br_token neg_int_types[] = {
					BRT_INT_32,	BRT_INT_16, BRT_INT_8,
					BRT_FLOAT, BRT_FIXED, BRT_BOOLEAN};

				if(negative)
					tv->t = BrTokenFindType(&type, name, neg_int_types, BR_ASIZE(neg_int_types));
				else
					tv->t = BrTokenFindType(&type, name, pos_int_types, BR_ASIZE(pos_int_types));

				if(tv->t == BR_NULL_TOKEN) 
					break;

				switch(type) {
				case BRT_INT_32:
					tv->v.i32 = negative? -BrLexerInteger(l): BrLexerInteger(l);
					break;

				case BRT_UINT_32:
					tv->v.u32 = BrLexerInteger(l);
					break;

				case BRT_INT_16:
					tv->v.i16 = (br_int_16)(negative? -BrLexerInteger(l): BrLexerInteger(l));
					break;

				case BRT_UINT_16:
					tv->v.u16 = (br_uint_16)BrLexerInteger(l);
					break;

				case BRT_INT_8:
					tv->v.i8 = (br_int_8)(negative? -BrLexerInteger(l): BrLexerInteger(l));
					break;

				case BRT_UINT_8:
					tv->v.u8 = (br_uint_8)BrLexerInteger(l);
					break;

				case BRT_FLOAT:
					tv->v.f = (br_float)(negative? -BrLexerInteger(l): BrLexerInteger(l));
					break;
				case BRT_FIXED:
					tv->v.x = BrIntToFixed(negative? -BrLexerInteger(l): BrLexerInteger(l));
					break;
				case BRT_BOOLEAN:
					tv->v.b = (BrLexerInteger(l) != 0);
					break;

				case BRT_POINTER:
				case BRT_OBJECT:
				case BRT_HANDLE:
					tv->v.p = (void *)BrLexerInteger(l);
					break;
				}
				break;
			}

			/*
			 * Look for token
			 */
			case T_IDENT: {
				static br_token ident_types[] = {BRT_TOKEN};

				tv->t = BrTokenFindType(&type, name, ident_types, BR_ASIZE(ident_types));

				if(tv->t == BR_NULL_TOKEN)
					break;
		
				tv->v.t = BrTokenFind(BrLexerString(l));
				if(tv->v.t == BR_NULL_TOKEN)
					tv->t = BR_NULL_TOKEN;
				break;
			}

			/*
			 * Look for boolean
			 */
			case T_TRUE:
			case T_FALSE: {
				static br_token bool_types[] = {BRT_BOOLEAN};

				tv->t = BrTokenFindType(&type, name, bool_types, BR_ASIZE(bool_types));

				if(tv->t == BR_NULL_TOKEN) 
					break;
	
				tv->v.b = BrLexerCurrent(l) == T_TRUE;
				break;
			}

			/*
			 * Look for string or constant_string
			 */
			case T_STRING: {
				static br_token string_types[] = {BRT_STRING, BRT_CONSTANT_STRING};

				tv->t = BrTokenFindType(&type, name, string_types, BR_ASIZE(string_types));

				if(tv->t == BR_NULL_TOKEN) 
					break;

				/*
				 * Allocate space from end of buffer
				 */
				len = BrStrLen(BrLexerString(l))+1; 
				if(len > (int)size) {
					tv->t = BR_NULL_TOKEN;
					r = BRE_OVERFLOW;
					break;
				}

				size -= len;
				tv->v.str = (char *)tv+size;
				BrMemCpy(tv->v.str,BrLexerString(l),len);
				break;
			}

			default:
				BrLexerError(l,"expecting a value");
				break;
			}

			BrLexerAdvance(l);

		} else {
			/*
			 * look for none or boolean
			 */
			static br_token none_types[] = {BRT_NONE, BRT_BOOLEAN};

			tv->t = BrTokenFindType(&type, name, none_types, BR_ASIZE(none_types));

			if(tv->t != BR_NULL_TOKEN) {
				if(type == BRT_BOOLEAN)
					tv->v.b = T_TRUE;
			}
		}

		if(tv->t != BR_NULL_TOKEN) {
			tv++;
			size -= sizeof(*tv);
		} else {
			BrLexerError(l,"unknown token");
		}
	}

	/*
	 * Terminate buffer
	 */
	tv->t = BR_NULL_TOKEN;
	tv->v.u32 = 0;

	return r;
}

/*
 * Compare two sets of token values for equality
 */
br_boolean BR_RESIDENT_ENTRY BrTokenValueCompare(br_token_value *tv1, br_token_value *tv2)
{
	int i;

	if(tv1 == NULL && tv2 == NULL)
		return BR_TRUE;

	if(tv1 == NULL || tv2 == NULL)
		return BR_FALSE;

	for(;;tv1++, tv2++) {
		if(tv1->t != tv2->t)
			return BR_FALSE;

		if(tv1->t == BR_NULL_TOKEN)
			return BR_TRUE;
		
		switch(BrTokenType(tv1->t)) {
		case BRT_NONE:
			break;

		case BRT_STRING:
		case BRT_CONSTANT_STRING:
			if(BrStrCmp(tv1->v.str, tv2->v.str))
				return BR_FALSE;
			break;

		case BRT_OBJECT_LIST:
		case BRT_TOKEN_LIST:
		case BRT_POINTER_LIST:
			for(i=0;;i++) {
				if(tv1->v.pl[i] != tv1->v.pl[i])
					return BR_FALSE;
				if(tv1->v.pl[i] == NULL)
					break;
			}
			break;

		case BRT_VECTOR2_INTEGER:
		case BRT_VECTOR2_FLOAT:
		case BRT_VECTOR2_FIXED:
			if(BrMemCmp(tv1->v.p, tv2->v.p, 2 * sizeof(br_int_32)))
				return BR_FALSE;

			break;

		case BRT_VECTOR3_INTEGER:
		case BRT_VECTOR3_FLOAT:
		case BRT_VECTOR3_FIXED:
			if(BrMemCmp(tv1->v.p, tv2->v.p, 3 * sizeof(br_int_32)))
				return BR_FALSE;

			break;

		case BRT_VECTOR4_INTEGER:
		case BRT_VECTOR4_FLOAT:
		case BRT_VECTOR4_FIXED:
			if(BrMemCmp(tv1->v.p, tv2->v.p, 4 * sizeof(br_int_32)))
				return BR_FALSE;
			break;

		case BRT_MATRIX23_FLOAT:
		case BRT_MATRIX23_FIXED:
			if(BrMemCmp(tv1->v.p, tv2->v.p, 6 * sizeof(br_int_32)))
				return BR_FALSE;
			break;

		case BRT_MATRIX34_FLOAT:
		case BRT_MATRIX34_FIXED:
			if(BrMemCmp(tv1->v.p, tv2->v.p, 12 * sizeof(br_int_32)))
				return BR_FALSE;
			break;

		case BRT_MATRIX4_FLOAT:
		case BRT_MATRIX4_FIXED:
			if(BrMemCmp(tv1->v.p, tv2->v.p, 16 * sizeof(br_int_32)))
				return BR_FALSE;
			break;

		case BRT_BOOLEAN:
			if(!tv1->v.u32 != !tv2->v.u32)
				return BR_FALSE;
			break;

		case BRT_INT_8:
		case BRT_UINT_8:
			if(tv1->v.u8 != tv2->v.u8)
				return BR_FALSE;
			break;

		case BRT_INT_16:
		case BRT_UINT_16:
			if(tv1->v.u16 != tv2->v.u16)
				return BR_FALSE;
			break;

		default:
			if(tv1->v.u32 != tv2->v.u32)
				return BR_FALSE;

			break;
		}
	}
}

/*
 * Compare two sets of token values for equality
 *
 * Allows a NULL terminated list of tokens which are not significant to the
 * comparision
 */
br_boolean BR_RESIDENT_ENTRY BrTokenValueComparePartial(br_token_value *tv1, br_token_value *tv2, br_token *insignificant)
{
	int i;
	br_token *t;

	if(tv1 == NULL && tv2 == NULL)
		return BR_TRUE;

	if(tv1 == NULL || tv2 == NULL)
		return BR_FALSE;

	for(;;tv1++, tv2++) {

		if(tv1->t != tv2->t)
			return BR_FALSE;

		if(tv1->t == BR_NULL_TOKEN)
			return BR_TRUE;
		
		for (t = insignificant; *t != BR_NULL_TOKEN; t++)
			if (tv1->t == *t)
				break;

		if (*t != BR_NULL_TOKEN)
			continue;

		switch(BrTokenType(tv1->t)) {
		case BRT_NONE:
			break;

		case BRT_STRING:
		case BRT_CONSTANT_STRING:
			if(BrStrCmp(tv1->v.str, tv2->v.str))
				return BR_FALSE;
			break;

		case BRT_OBJECT_LIST:
		case BRT_TOKEN_LIST:
		case BRT_POINTER_LIST:
			for(i=0;;i++) {
				if(tv1->v.pl[i] != tv1->v.pl[i])
					return BR_FALSE;
				if(tv1->v.pl[i] == NULL)
					break;
			}
			break;

		case BRT_VECTOR2_INTEGER:
		case BRT_VECTOR2_FLOAT:
		case BRT_VECTOR2_FIXED:
			if(BrMemCmp(tv1->v.p, tv2->v.p, 2 * sizeof(br_int_32)))
				return BR_FALSE;

			break;

		case BRT_VECTOR3_INTEGER:
		case BRT_VECTOR3_FLOAT:
		case BRT_VECTOR3_FIXED:
			if(BrMemCmp(tv1->v.p, tv2->v.p, 3 * sizeof(br_int_32)))
				return BR_FALSE;

			break;

		case BRT_VECTOR4_INTEGER:
		case BRT_VECTOR4_FLOAT:
		case BRT_VECTOR4_FIXED:
			if(BrMemCmp(tv1->v.p, tv2->v.p, 4 * sizeof(br_int_32)))
				return BR_FALSE;
			break;

		case BRT_MATRIX23_FLOAT:
		case BRT_MATRIX23_FIXED:
			if(BrMemCmp(tv1->v.p, tv2->v.p, 6 * sizeof(br_int_32)))
				return BR_FALSE;
			break;

		case BRT_MATRIX34_FLOAT:
		case BRT_MATRIX34_FIXED:
			if(BrMemCmp(tv1->v.p, tv2->v.p, 12 * sizeof(br_int_32)))
				return BR_FALSE;
			break;

		case BRT_MATRIX4_FLOAT:
		case BRT_MATRIX4_FIXED:
			if(BrMemCmp(tv1->v.p, tv2->v.p, 16 * sizeof(br_int_32)))
				return BR_FALSE;
			break;

		case BRT_BOOLEAN:
			if(!tv1->v.u32 != !tv2->v.u32)
				return BR_FALSE;
			break;

		case BRT_INT_8:
		case BRT_UINT_8:
			if(tv1->v.u8 != tv2->v.u8)
				return BR_FALSE;
			break;

		case BRT_INT_16:
		case BRT_UINT_16:
			if(tv1->v.u16 != tv2->v.u16)
				return BR_FALSE;
			break;

		default:
			if(tv1->v.u32 != tv2->v.u32)
				return BR_FALSE;

			break;
		}
	}
}

/*
 * Duplicate a set of tokens into a new allocated buffer
 */
br_error BrTokenValueDup(br_token_value **dstp, br_token_value *src)
{
	int c;
	br_token_value *tv;

	/*
	 * Find out how many tokens/value pairs there are, and
	 * the size of any extra info
	 */

	c = 0;
	for(tv = src; tv->t != BR_NULL_TOKEN; tv++)
		c++;


	return BRE_OK;	
}

