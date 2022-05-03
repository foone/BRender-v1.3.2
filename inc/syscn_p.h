/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: syscn_p.h 1.1 1997/12/10 16:41:19 jon Exp $
 * $Locker: $
 *
 * Public function prototypes for BRender framework
 */
#ifndef _SYSCON_P_H_
#define _SYSCON_P_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _NO_PROTOTYPES


br_error BrSystemConfigBegin();
br_error BR_PUBLIC_ENTRY BrSystemConfigLoad( br_token t, char *Param1, void *Param2 );
br_error BR_PUBLIC_ENTRY BrSystemConfigSave( br_token t, char *Param1, void *Param2 );
br_error BR_PUBLIC_ENTRY BrSystemConfigQueryString( br_token t, char *string, int max_size );
br_error BR_PUBLIC_ENTRY BrSystemConfigSetString( br_token t, char *string );

br_error BR_PUBLIC_ENTRY BrSystemConfigSet(br_token t, br_value v);
br_error BR_PUBLIC_ENTRY BrSystemConfigQuery(br_token t, br_value *v);

br_error BR_PUBLIC_ENTRY BrReadEnvironmentEntry(br_associative_array *a, char *name);

br_error BR_PUBLIC_ENTRY BrReadIniSection(br_associative_array *a,
	char *filename, char *section);

br_error BR_PUBLIC_ENTRY BrWriteIniSection(br_associative_array *a,
	char *filename, char *section);

br_error BR_PUBLIC_ENTRY BrReadRegistryKey(br_associative_array *a,
	void *key, char *subkey);

br_error BR_PUBLIC_ENTRY BrWriteRegistryKey(br_associative_array *a,
	void *key, char *subkey);

#endif /* _NO_PROTOTYPES */

#ifdef __cplusplus
};
#endif
#endif
