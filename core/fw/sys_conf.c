/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: sys_conf.c 1.3 1998/02/27 18:11:28 jon Exp $
 * $Locker: $
 *
 * System configuration
 */
#include "fw.h"
#include "host.h"
#include <brassert.h>


#define BRENDER_INI_FILE "BRender.ini"
#define BRENDER_REGISTRY_KEY "SOFTWARE\\Argonaut\\BRender"

#define DEFAULT_BRENDER_PATH "dat;../dat;../../dat;../../../dat;../../../../dat;dll;../dll;../../dll;../../../dll;../../../../dll"


//extern br_framework_state BR_ASM_DATA fw ;


br_error BrSystemConfigBegin()
{
	char version[12], temp[255];
	br_value v ;

	fw.sys_config = BrAssociativeArrayAllocate();

	if (fw.sys_config == NULL) {

		BR_ERROR0("System config associative array allocate failed.\n");
		return BRE_FAIL ;
	}

	/*
	 * Get a string for the BRender version number
	 */
	BrSprintf(version, "%d.%d.%d",
		(__BRENDER__ & 0xff0000) >> 16,
		(__BRENDER__ & 0x00ff00) >> 8,
		(__BRENDER__ & 0x0000ff));

	/*
	 * Initially set default values
	 */
	v.str = DEFAULT_BRENDER_PATH;
	BrAssociativeArraySet(fw.sys_config, BRT_BRENDER_PATH_STR, v);

	v.str = HostDefaultDevice();
	if (v.str != NULL)
		BrAssociativeArraySet(fw.sys_config, BRT_DEFAULT_DEVICE_STR, v);
	
	/*
	 * Read basic configuration entries from the environment, obsolete
	 * variations first
	 */
	BrReadEnvironmentEntry(fw.sys_config, "BRENDER_DOS_GFX");

	BrReadEnvironmentEntry(fw.sys_config, "BR_DRIVERS");
	BrReadEnvironmentEntry(fw.sys_config, "BR_DEVICES");
	BrReadEnvironmentEntry(fw.sys_config, "BR_DEFAULT_DEVICE");

	BrReadEnvironmentEntry(fw.sys_config, "BRENDER_PATH");
	BrReadEnvironmentEntry(fw.sys_config, "BRENDER_DRIVERS");
	BrReadEnvironmentEntry(fw.sys_config, "BRENDER_DEVICES");
	BrReadEnvironmentEntry(fw.sys_config, "BRENDER_DEFAULT_DEVICE");

	BrReadEnvironmentEntry(fw.sys_config, "BRENDER_USE_MMX");

	BrReadEnvironmentEntry(fw.sys_config, "BRENDER_DEVELOPER_B");
	BrReadEnvironmentEntry(fw.sys_config, "BRENDER_USE_MMX_B");

	/*
	 * COMPATIBILITY - Read the non version specific section from the
	 * INI file
	 */
	BrReadIniSection(fw.sys_config, BRENDER_INI_FILE, "BRender");

	/*
	 * Read the section for the current version from the INI file
	 */
	BrReadIniSection(fw.sys_config, BRENDER_INI_FILE, version);

	/*
	 * Read the key for the current version from the registry file
	 */
	BrSprintf(temp, "%s\\%s", BRENDER_REGISTRY_KEY, version);
	BrReadRegistryKey(fw.sys_config, NULL, temp);

	return BRE_OK ;
}


br_error BR_PUBLIC_ENTRY BrSystemConfigLoad(br_token t, char *param1, void *param2)
{
	ASSERT(fw.sys_config);
	ASSERT(param1);
	 
	switch (t) {

	case BRT_REGISTRY_STR:

		return BrReadRegistryKey(fw.sys_config, param2, param1);

	case BRT_INI_STR:

		return BrReadIniSection(fw.sys_config, param1, (char *)param2);

	default:

		BR_ERROR0("Invalid system config load token. Must be REGISTRY or INI");
		return BRE_FAIL;
   }
}


br_error BR_PUBLIC_ENTRY BrSystemConfigSave(br_token t, char *param1, void *param2)
{
	ASSERT(fw.sys_config);
	ASSERT(param1);
	 
	switch (t) {

	case BRT_REGISTRY_STR:

		return BrWriteRegistryKey(fw.sys_config, param2, param1);

	case BRT_INI_STR:

		return BrWriteIniSection(fw.sys_config, param1, (char *)param2);

	default:

		BR_ERROR0("Invalid system config load token. Must be REGISTRY or INI");
		return BRE_FAIL;
   }
}


br_error BR_PUBLIC_ENTRY BrSystemConfigSetString( br_token t, char *string )
{
   br_value v ;

   ASSERT( fw.sys_config );
   ASSERT( string );

   v.cstr = string ;

   return ( BrAssociativeArraySet( fw.sys_config, t, v ) );
}



br_error BR_PUBLIC_ENTRY BrSystemConfigQueryString( br_token t, char *string, int max_size )
{
   br_error r ;
   br_value v ;

   ASSERT( fw.sys_config );

   r = BrAssociativeArrayQuery( fw.sys_config, t, &v );

   if ( r == BRE_OK  )
   {
	   if ( v.str )
         BrStrNCpy( string, v.str, max_size );
       else
		   *string = '\0' ;
   }
   else
      *string = '\0' ;

   return r ;
}


br_error BR_PUBLIC_ENTRY BrSystemConfigSet(br_token t, br_value v)
{
   ASSERT(fw.sys_config);

   return BrAssociativeArraySet(fw.sys_config, t, v);
}



br_error BR_PUBLIC_ENTRY BrSystemConfigQuery(br_token t, br_value *v)
{
   ASSERT(fw.sys_config);

   return BrAssociativeArrayQuery(fw.sys_config, t, v);
}


/*
 * Routine used for parsing textual configuration entries
 */
static br_error	parseEntryLine(br_lexer *l, br_token_value *tv, br_size_t size);
static char *valueToString(br_token_value *tv);

/*
 * Keyword tokens for lexer
 */
enum {
	T_FALSE = T_KEYWORD+0,
	T_TRUE = T_KEYWORD+1,
};

br_lexer_keyword keywords[] = {
	{"true",	T_TRUE },
	{"t",		T_TRUE },
	{"false",	T_FALSE },
	{"f",		T_FALSE },
	{"TRUE",	T_TRUE },
	{"T",		T_TRUE },
	{"FALSE",	T_FALSE },
	{"F",		T_FALSE },
};


/*
 * Read a single entry from the environment into an associative array
 */
br_error BR_PUBLIC_ENTRY BrReadEnvironmentEntry(br_associative_array *a, char *name)
{
	br_lexer *l;
	br_error r;
	char entry[256], tv[256], *value;

	/*
	 * Read the environment variable and use the parser for INI files
	 * to get the value (slightly circuitous!)
	 */
	value = BrGetEnv(name);
	if (value == NULL)
		return BRE_FAIL;

	BrSprintf(entry, "%s=%s", name, value);

	l = BrLexerAllocate(keywords, BR_ASIZE(keywords));
	if (l == NULL)
		return BRE_NO_MEMORY;

	r = BrLexerPushString(l, entry, NULL);

	if (r != BRE_OK) {
	 	BrLexerFree(l);
		return r;
	}

	/*
	 * Start up the pipeline
	 */
	BrLexerAdvance(l);

	r = parseEntryLine(l, (br_token_value *)tv, sizeof(tv));

	BrLexerPop(l);

	if (r != BRE_OK || ((br_token_value *)tv)->t == BR_NULL_TOKEN)
		return BRE_FAIL;

	/*
	 * Check for config strings that didn't have quotes, and parse them
	 * separately
	 */
	if (BrTokenType(((br_token_value *)tv)->t) == BRT_STRING &&
		((br_token_value *)tv)->v.str == NULL)

		((br_token_value *)tv)->v.str = BrGetEnv(name);

	r = BrAssociativeArraySet(a, ((br_token_value *)tv)->t,
		((br_token_value *)tv)->v);

	BrLexerFree(l);

	return r;
}


/*
 * BrReadIniSection - reads a section of an INI file into an associative
 * array.
 *
 * Uses a modified version of BrStringToTokenValue, which creates tokens
 * if necessary, and copes with the configuration strings not having
 * quotes
 *
 * With the exception of the configuration strings, all keys should be
 * a valid token identifier, and must include the suffix unless it is a 
 * predefined token
 */
br_error BR_PUBLIC_ENTRY BrReadIniSection(br_associative_array *a,
	char *filename, char *section)
{
	br_lexer *l;
	br_error r;
	char *buffer, *pos, tv[256];

	/*
	 * Allocate a temporary buffer for the ini section
	 */
	buffer = BrMemAllocate(32767, BR_MEMORY_SCRATCH);
	if (buffer == NULL)
		return BRE_NO_MEMORY;

	/*
	 * Read the INI section in its entirety
	 */
	r = HostIniReadSection(filename, section, buffer, 32767);
	if (r != BRE_OK) {
		BrMemFree(buffer);
		return r;
	}

	l = BrLexerAllocate(keywords, BR_ASIZE(keywords));
	if(l == NULL) {
		BrMemFree(buffer);
		return BRE_NO_MEMORY;
	}

	/*
	 * Convert each line into a token value pair.
	 */
	for (pos = buffer; *pos != '\0'; pos += BrStrLen(pos) + 1) {

		r = BrLexerPushString(l, pos, NULL);

		if (r != BRE_OK) {
		 	BrLexerFree(l);
			BrMemFree(buffer);
			return r;
		}

		/*
		 * Start up the pipeline
		 */
		BrLexerAdvance(l);

		r = parseEntryLine(l, (br_token_value *)tv, sizeof(tv));

		BrLexerPop(l);

		if (r != BRE_OK || ((br_token_value *)tv)->t == BR_NULL_TOKEN)
			continue;

		/*
		 * Check for config strings that didn't have quotes, and parse them
		 * separately
		 */
		if (BrTokenType(((br_token_value *)tv)->t) == BRT_STRING &&
			((br_token_value *)tv)->v.str == NULL) {

			while (*pos != '=' && *pos != ':' && *pos != '\0')
				pos++;

			if (*pos == '\0')
				continue;

			pos++;

			while (BrIsSpace(*pos))
				pos++;

			((br_token_value *)tv)->v.str = pos;
		}

		r = BrAssociativeArraySet(a, ((br_token_value *)tv)->t,
			((br_token_value *)tv)->v);

		if (r != BRE_OK) {
			BrLexerFree(l);
			BrMemFree(buffer);
			return r;
		}
	}

	BrLexerFree(l);
	BrMemFree(buffer);

	return BRE_OK;
}

/*
 * BrWriteIniSection - writes the contents of an associative array to a
 * section of an INI file.
 */
br_error BR_PUBLIC_ENTRY BrWriteIniSection(br_associative_array *a,
	char *filename, char *section)
{
	br_error r;

	char *buffer, *pos, *name, *value;
	br_size_t space, len;
	br_uint_32 i;

	/*
	 * Allocate a temporary buffer for the ini section
	 */
	buffer = BrMemAllocate(32767, BR_MEMORY_SCRATCH);
	if (buffer == NULL)
		return BRE_NO_MEMORY;

	/*
	 * Track space left, remembering to leave one character free for
	 * final terminator
	 */
	pos = buffer;
	space = 32767 - 1;

	/*
	 * Write each entry in the associative array into the buffer
	 */
	for (i = 0; i < a->num_elements; i++) {

		name = BrTokenIdentifier(a->tv[i].t);

		/*
		 * Get a string for the value
		 */
		value = valueToString(&a->tv[i]);

		/*
		 * Calculate the length of the entry
		 */
		len = BrStrLen(name);
		if (value != NULL)
			len += 1 + BrStrLen(value);

		/*
		 * Check that there is enough space
		 */
		if (space < len + 1) {
			BrMemFree(buffer);
			return BRE_FAIL;
		}

		/*
		 * Write the entry
		 */
		if (value != NULL)
			BrSprintf(pos, "%s=%s", name, value);
		else
			BrStrCpy(pos, name);

		pos += len + 1;
	}

	/*
	 * Terminate list of entries
	 */
	*pos++ = '\0';

	/*
	 * Write the INI section in its entirety
	 */
	r = HostIniWriteSection(filename, section, buffer, pos - buffer);

	BrMemFree(buffer);

	return r;
}


br_error BR_PUBLIC_ENTRY BrReadRegistryKey(br_associative_array *a,
	void *key, char *subkey)
{
	br_lexer *l;
	br_error r;
	char *buffer, *pos, tv[256];

	/*
	 * Allocate a temporary buffer for the registry key
	 */
	buffer = BrMemAllocate(32767, BR_MEMORY_SCRATCH);
	if (buffer == NULL)
		return BRE_NO_MEMORY;

	/*
	 * Read the registry key in its entirety
	 */
	r = HostRegistryReadKey(key, subkey, buffer, 32767);
	if (r != BRE_OK) {
		BrMemFree(buffer);
		return r;
	}

	l = BrLexerAllocate(keywords, BR_ASIZE(keywords));
	if(l == NULL) {
		BrMemFree(buffer);
		return BRE_NO_MEMORY;
	}

	/*
	 * Convert each line into a token value pair.
	 */
	for (pos = buffer; *pos != '\0'; pos += BrStrLen(pos) + 1) {

		r = BrLexerPushString(l, pos, NULL);

		if (r != BRE_OK) {
		 	BrLexerFree(l);
			BrMemFree(buffer);
			return r;
		}

		/*
		 * Start up the pipeline
		 */
		BrLexerAdvance(l);

		r = parseEntryLine(l, (br_token_value *)tv, sizeof(tv));

		BrLexerPop(l);

		if (r != BRE_OK || ((br_token_value *)tv)->t == BR_NULL_TOKEN)
			continue;

		/*
		 * Check for config strings that didn't have quotes, and parse them
		 * separately
		 */
		if (BrTokenType(((br_token_value *)tv)->t) == BRT_STRING &&
			((br_token_value *)tv)->v.str == NULL) {

			while (*pos != '=' && *pos != ':' && *pos != '\0')
				pos++;

			if (*pos == '\0')
				continue;

			pos++;

			while (BrIsSpace(*pos))
				pos++;

			((br_token_value *)tv)->v.str = pos;
		}

		r = BrAssociativeArraySet(a, ((br_token_value *)tv)->t,
			((br_token_value *)tv)->v);

		if (r != BRE_OK) {
			BrLexerFree(l);
			BrMemFree(buffer);
			return r;
		}
	}

	BrLexerFree(l);
	BrMemFree(buffer);

	return BRE_OK;
}

br_error BR_PUBLIC_ENTRY BrWriteRegistryKey(br_associative_array *a,
	void *key, char *subkey)
{
	br_error r;

	char *buffer, *pos, *name, *value;
	br_size_t space, len;
	br_uint_32 i;

	/*
	 * Allocate a temporary buffer for the ini section
	 */
	buffer = BrMemAllocate(32767, BR_MEMORY_SCRATCH);
	if (buffer == NULL)
		return BRE_NO_MEMORY;

	/*
	 * Track space left, remembering to leave one character free for
	 * final terminator
	 */
	pos = buffer;
	space = 32767 - 1;

	/*
	 * Write each entry in the associative array into the buffer
	 */
	for (i = 0; i < a->num_elements; i++) {

		name = BrTokenIdentifier(a->tv[i].t);

		/*
		 * Get a string for the value
		 */
		value = valueToString(&a->tv[i]);

		/*
		 * Calculate the length of the entry
		 */
		len = BrStrLen(name);
		if (value != NULL)
			len += 1 + BrStrLen(value);

		/*
		 * Check that there is enough space
		 */
		if (space < len + 1) {
			BrMemFree(buffer);
			return BRE_FAIL;
		}

		/*
		 * Write the entry
		 */
		if (value != NULL)
			BrSprintf(pos, "%s=%s", name, value);
		else
			BrStrCpy(pos, name);

		pos += len + 1;
	}

	/*
	 * Terminate list of entries
	 */
	*pos++ = '\0';

	/*
	 * Write the INI section in its entirety
	 */
	r = HostRegistryWriteKey(key, subkey, buffer, pos - buffer);

	BrMemFree(buffer);

	return r;
}


static br_error	parseEntryLine(br_lexer *l, br_token_value *tv, br_size_t size)
{
	int len;
	char name[40];
	br_token type;
	static br_token real_types[] = {BRT_FLOAT, BRT_FIXED};
	br_uint_32 i;
	br_boolean negative;

	static struct {

		br_token token;
		char *name;

	} config_strings[] = {

		BRT_BRENDER_PATH_STR, "BRENDER_PATH",
		BRT_BRENDER_DRIVERS_STR, "BRENDER_DRIVERS",
		BRT_BRENDER_DEVICES_STR, "BRENDER_DEVICES",
		BRT_DEFAULT_DEVICE_STR, "BRENDER_DEFAULT_DEVICE",
		BRT_DEFAULT_DEVICE_STR, "BRENDER_DOS_GFX",
		BRT_BRENDER_USE_MMX_B, "BRENDER_USE_MMX",
	};

	/*
	 * Check for empty lines
	 */
	if (BrLexerCurrent(l) == T_EOF)
		return BRE_OK;

	/*
	 * Definition must start with an identifier
	 */
	if (BrLexerCurrent(l) != T_IDENT)
		return BRE_OK;

	/*
	 * Check there is space for token/value
	 */
	if (size <= sizeof(br_token_value))
		return BRE_OVERFLOW;

	/*
	 * token name
	 */
	name[BR_ASIZE(name)-1] = '\0';
	BrStrNCpy(name, BrLexerString(l), BR_ASIZE(name)-1);
	BrLexerAdvance(l);

	/*
	 * Optional value
	 */
	tv->t = BR_NULL_TOKEN;

	/*
	 * Compatibility - look for the configuration strings
	 */
	for (i = 0; i < BR_ASIZE(config_strings); i++) {

		if (BrStrNICmp(name, config_strings[i].name, BrStrLen(config_strings[i].name)) == 0) {

			tv->t = config_strings[i].token;
			break;
		}
	}

	/*
	 * Otherwise get the token, creating it if necessary.  If this fails
	 * (because the name does not specify the type) the token will be
	 * looked up later, according to the type of the value
	 */
	if (tv->t == BR_NULL_TOKEN)
		tv->t = BrTokenCreate(name, BR_NULL_TOKEN);
	
	if(BrLexerCurrent(l) == T_COLON || BrLexerCurrent(l) == T_EQUAL) {

		BrLexerAdvance(l);

		/*
		 * Catch strings and mark them needing to be read separately if
		 * they are not enclosed in quotes (lexical analyser cannot return
		 * this information)
		 */
		if (BrTokenType(tv->t) == BRT_STRING && BrLexerCurrent(l) != T_STRING) {
				
			tv->v.str = NULL;
			return BRE_OK;
		}

		/*
		 * Check for negative scalar types
		 */
		negative = BrLexerCurrent(l) == T_DASH;

		if (negative) {

			BrLexerAdvance(l);

			if (BrLexerCurrent(l) != T_INTEGER && BrLexerCurrent(l) != T_REAL) {
				BrLexerError(l,"invalid scalar");
				return BRE_FAIL;
			}
		}

		switch (BrLexerCurrent(l)) {

		/*
		 * Look for float or fixed
		 */
		case T_REAL: {
		static br_token real_types[] = {BRT_FLOAT, BRT_FIXED, BRT_ANGLE};

			if (tv->t != BR_NULL_TOKEN)
				type = BrTokenType(tv->t);
			else
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
			case BRT_ANGLE:
				tv->v.a = BrScalarToAngle(BrFloatToScalar(negative? -BrLexerReal(l): BrLexerReal(l)));
				break;
			default:
				BrLexerError(l,"invalid token type");
				return BRE_FAIL;
			}
			break;
		}

		/*
		 * if positive look for i32/16/8, u32/16/8, float, fixed or boolean
		 * otherwise look for i32/16/8, float, fixed or boolean
		 */
		case T_INTEGER: {
			static br_token pos_int_types[] = {
				BRT_INT_32, BRT_UINT_32, BRT_INT_16, BRT_UINT_16,
				BRT_INT_8, BRT_UINT_8, BRT_FLOAT, BRT_FIXED, BRT_BOOLEAN, BRT_COLOUR_RGB};
			static br_token neg_int_types[] = {
				BRT_INT_32,	BRT_INT_16, BRT_INT_8,
				BRT_FLOAT, BRT_FIXED, BRT_BOOLEAN};

			if (tv->t != BR_NULL_TOKEN)
				type = BrTokenType(tv->t);
			else
				if (negative)
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
			case BRT_COLOUR_RGB:
				tv->v.rgb = (br_colour)BrLexerInteger(l);
				break;
			default:
				BrLexerError(l,"invalid token type");
				return BRE_FAIL;
			}
			break;
		}

		/*
		 * Look for token
		 */
		case T_IDENT: {
			static br_token ident_types[] = {BRT_TOKEN};

			if (tv->t != BR_NULL_TOKEN)
				type = BrTokenType(tv->t);
			else
				tv->t = BrTokenFindType(&type, name, ident_types, BR_ASIZE(ident_types));

			if(tv->t == BR_NULL_TOKEN)
				break;

			if (type == BRT_TOKEN) {

				tv->v.t = BrTokenFind(BrLexerString(l));
				if(tv->v.t == BR_NULL_TOKEN)
					tv->t = BR_NULL_TOKEN;

			} else {
				BrLexerError(l,"invalid token type");
				return BRE_FAIL;
			}

			break;
		}

		/*
		 * Look for boolean
		 */
		case T_TRUE:
		case T_FALSE: {
			static br_token bool_types[] = {BRT_BOOLEAN};

			if (tv->t != BR_NULL_TOKEN)
				type = BrTokenType(tv->t);
			else
				tv->t = BrTokenFindType(&type, name, bool_types, BR_ASIZE(bool_types));

			if(tv->t == BR_NULL_TOKEN) 
				break;

			if (type == BRT_BOOLEAN)
				tv->v.b = BrLexerCurrent(l) == T_TRUE;
			else {
				BrLexerError(l,"invalid token type");
				return BRE_FAIL;
			}

			break;
		}

		/*
		 * Look for string or constant_string
		 */
		case T_STRING: {
			static br_token string_types[] = {BRT_STRING, BRT_CONSTANT_STRING};

			if (tv->t != BR_NULL_TOKEN)
				type = BrTokenType(tv->t);
			else
				tv->t = BrTokenFindType(&type, name, string_types, BR_ASIZE(string_types));

			if(tv->t == BR_NULL_TOKEN) 
				break;

			if (type == BRT_STRING || type == BRT_CONSTANT_STRING) {

				/*
				 * Allocate space from end of buffer
				 */
				len = BrStrLen(BrLexerString(l))+1; 
				if(len > (int)size)
					return BRE_OVERFLOW;

				size -= len;
				tv->v.str = (char *)tv+size;
				BrMemCpy(tv->v.str,BrLexerString(l),len);

			} else {
				BrLexerError(l,"invalid token type");
				return BRE_FAIL;
			}

			break;
		}

		default: {

			/*
			 * look for any valid type and set it to the NULL or zero value - a bit gross
			 * but it works.
			 */
			static br_token none_types[] = {
				BRT_NONE, BRT_BOOLEAN, BRT_TOKEN, BRT_INT_8, BRT_UINT_8, BRT_INT_16,
				BRT_UINT_16, BRT_INT_32, BRT_UINT_32, BRT_FIXED, BRT_FLOAT, BRT_ANGLE,
				BRT_COLOUR_RGB, BRT_STRING, BRT_CONSTANT_STRING};

			if (tv->t != BR_NULL_TOKEN)
				type = BrTokenType(tv->t);
			else
				tv->t = BrTokenFindType(&type, name, none_types, BR_ASIZE(none_types));

			if(tv->t != BR_NULL_TOKEN) {

				switch (type) {

				case BRT_NONE:
					break;

				case BRT_INT_32:
				case BRT_UINT_32:
				case BRT_INT_16:
				case BRT_UINT_16:
				case BRT_INT_8:
				case BRT_UINT_8:
				case BRT_FIXED:
				case BRT_ANGLE:
				case BRT_COLOUR_RGB:
				case BRT_BOOLEAN:
				case BRT_TOKEN:
				case BRT_STRING:
				case BRT_CONSTANT_STRING:

					tv->v.u32 = 0;
					break;

				case BRT_FLOAT:

					tv->v.f = 0.0f;
					break;

				default:
					BrLexerError(l,"invalid token type");
					return BRE_FAIL;
				}
			}
		}
		}

		BrLexerAdvance(l);

	} else {
		/*
		 * look for any valid type and set it to the NULL or zero value - a bit gross
		 * but it works.
		 */
		static br_token none_types[] = {
			BRT_NONE, BRT_BOOLEAN, BRT_TOKEN, BRT_INT_8, BRT_UINT_8, BRT_INT_16,
			BRT_UINT_16, BRT_INT_32, BRT_UINT_32, BRT_FIXED, BRT_FLOAT, BRT_ANGLE,
			BRT_COLOUR_RGB, BRT_STRING, BRT_CONSTANT_STRING};

		if (tv->t != BR_NULL_TOKEN)
			type = BrTokenType(tv->t);
		else
			tv->t = BrTokenFindType(&type, name, none_types, BR_ASIZE(none_types));

		if(tv->t != BR_NULL_TOKEN) {

			switch (type) {

			case BRT_NONE:
				break;

			case BRT_INT_32:
			case BRT_UINT_32:
			case BRT_INT_16:
			case BRT_UINT_16:
			case BRT_INT_8:
			case BRT_UINT_8:
			case BRT_FIXED:
			case BRT_ANGLE:
			case BRT_COLOUR_RGB:
			case BRT_BOOLEAN:
			case BRT_TOKEN:
			case BRT_STRING:
			case BRT_CONSTANT_STRING:

				tv->v.u32 = 0;
				break;

			case BRT_FLOAT:

				tv->v.f = 0.0f;
				break;

			default:
				BrLexerError(l,"invalid token type");
				return BRE_FAIL;
			}
		}
	}

	if(tv->t == BR_NULL_TOKEN) {
		BrLexerError(l,"unknown token");
		return BRE_FAIL;
	}

	if (BrLexerCurrent(l) != T_EOF)
		BrLexerError(l,"garbage after definition");

	return BRE_OK;
}


static char *valueToString(br_token_value *tv)
{
	static char buffer[256];

	switch (BrTokenType(tv->t)) {

	case BRT_NONE:

		return NULL;

	case BRT_TOKEN:

		return BrTokenIdentifier(tv->v.t);

	case BRT_STRING:
	case BRT_CONSTANT_STRING:

		if (tv->v.str == NULL)
			return NULL;

		if (BrStrLen(tv->v.str) > sizeof(buffer) - 2)
			return NULL;

		BrSprintf(buffer, "\"%s\"", tv->v.str);
		return buffer;

	case BRT_BOOLEAN:

		return tv->v.b? "TRUE": "FALSE";

	case BRT_INT_8:

		BrSprintf(buffer, "%d", tv->v.i8);
		return buffer;

	case BRT_UINT_8:

		BrSprintf(buffer, "%d", tv->v.u8);
		return buffer;

	case BRT_INT_16:

		BrSprintf(buffer, "%d", tv->v.i16);
		return buffer;

	case BRT_UINT_16:

		BrSprintf(buffer, "%d", tv->v.u16);
		return buffer;

	case BRT_INT_32:

		BrSprintf(buffer, "%d", tv->v.i32);
		return buffer;

	case BRT_UINT_32:

		BrSprintf(buffer, "%d", tv->v.u32);
		return buffer;

	case BRT_FIXED:

		BrSprintf(buffer, "%f", BrFixedToFloat(tv->v.x));
		return buffer;

	case BRT_FLOAT:

		BrSprintf(buffer, "%f", tv->v.f);
		return buffer;

	case BRT_ANGLE:

		BrSprintf(buffer, "%f", BrScalarToFloat(BrAngleToScalar(tv->v.f)));
		return buffer;

	case BRT_COLOUR_RGB:

		BrSprintf(buffer, "0x%08x", (br_uint_32)tv->v.rgb);
		return buffer;

	default:

		return NULL;
	}

}

