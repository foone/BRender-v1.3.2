/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: hostcfg.c 1.2 1998/02/26 21:15:44 jon Exp $
 * $Locker: $
 *
 * Reading config. strings from host environment
 */

#include <stdio.h>

#include "brender.h"
#include "host.h"


#ifdef __WIN_32__
#define DEFAULT_DEVICE "DDRAW"
#else
#define DEFAULT_DEVICE "MCGA"
#endif





// Read requested environment variable. If not set, use one of our
// defaults if applicable.


char * BR_RESIDENT_ENTRY HostDefaultDevice()
{
   return DEFAULT_DEVICE ;
}





#ifdef __WIN_32__

#include <windows.h>


br_boolean BR_RESIDENT_ENTRY HostIniSectionExists( char *ini_file, char *section_name )
{
	DWORD Size ;
	
	char TmpBuffer[5];
    
	Size = GetPrivateProfileSection( section_name, TmpBuffer, 5, ini_file );
   
   return( Size != 0 );
}


br_error BR_RESIDENT_ENTRY HostIniQuery( char *ini_file, char *section_name,
                                         char *entry, char *Buffer, int max,
                                         br_uint_16 *size )

{
   *size = (int) GetPrivateProfileString( section_name, entry,
                                          "", Buffer,
                                          max, ini_file );

   return BRE_OK ;
}


br_error BR_RESIDENT_ENTRY HostRegistryQuery( void *hKey, char *Path,
                             char *entry, char *Buffer, 
                             br_uint_16 max, br_uint_16 *size )
{
   HKEY key ,key2;
   DWORD type ;
   DWORD dwsize = 0 ;

   if ( hKey )
      key = (HKEY) hKey ;
    else
      key = HKEY_LOCAL_MACHINE ;

    *size = 0 ;

   if ( RegOpenKeyEx( key, Path, 0, KEY_READ, &key2 ) != ERROR_SUCCESS ) 
       return BRE_FAIL ;

   dwsize = (DWORD) max ;

   if ( RegQueryValueEx( key2, entry, NULL, &type, Buffer, &dwsize ) != ERROR_SUCCESS )
      return BRE_FAIL ;
 
   if ( type != REG_SZ )
   {
      BR_ERROR0("Registry entry type is not a string");
      return BRE_FAIL ;
   } 

   *size = (br_uint_16) dwsize ;

   return BRE_OK ;
}


br_error BR_RESIDENT_ENTRY HostIniReadSection(char *filename, char *section,
                                              char *buffer, br_size_t size)
{
	br_size_t chars;

	chars = GetPrivateProfileSection(section, buffer, size, filename);

	/*
	 * According to the documentation, the value returned is the number
	 * of characters copied into the buffer, and is (size - 2) if the
	 * buffer was not large enough.  Seems strange, but anyway...
	 */
	if (chars == size - 2)
		return BRE_OVERFLOW;

	return BRE_OK;
}

br_error BR_RESIDENT_ENTRY HostIniWriteSection(char *filename, char *section,
                                              char *buffer, br_size_t size)
{
	/*
	 * Check that the section is correctly terminated
	 */
	if (buffer[size - 1] != '\0' || buffer[size - 2] != '\0')
		return BRE_FAIL;

	if (WritePrivateProfileSection(section, buffer, filename))
		return BRE_OK;
	else
		return BRE_FAIL;
}


/*
 * Reads all values in a registry key and returns them in the same format
 * as an ini section (i.e. a NULL-terminates sequence of NULL-terminated
 * strings of the form name=value).  Actually ignores many key types at
 * the moment.
 *
 * The key should be passed with permission to read
 */
br_error BR_RESIDENT_ENTRY HostRegistryReadKey(void *key, char *subkey,
                                               char *buffer, br_size_t size)
{
	HKEY hkey;
	DWORD num_values, max_name_len, max_value_len, name_len, value_len, type, dword;
	char *name, *value;
	br_uint_32 i;
	br_error r = BRE_OK;

	if (RegOpenKeyEx(key? (HKEY)key: HKEY_LOCAL_MACHINE, subkey, 0, KEY_READ, &hkey) != ERROR_SUCCESS)
		return BRE_FAIL;

	if (RegQueryInfoKey(hkey, NULL, NULL, NULL, NULL, NULL, NULL,
		&num_values, &max_name_len, &max_value_len, NULL, NULL) != ERROR_SUCCESS) {
		RegCloseKey(hkey);
		return BRE_FAIL;
	}

	name = BrMemAllocate(max_name_len + 1, BR_MEMORY_SCRATCH);
	if (name == NULL) {
		RegCloseKey(hkey);
		return BRE_FAIL;
	}

	value = BrMemAllocate(max_value_len, BR_MEMORY_SCRATCH);
	if (value == NULL) {
		BrMemFree(name);
		RegCloseKey(hkey);
		return BRE_FAIL;
	}

	for (i = 0; i < num_values; i++) {

		name_len = max_name_len + 1;
		value_len = max_value_len;

		if (RegEnumValue(hkey, i, name, &name_len, NULL, &type, value, &value_len) != ERROR_SUCCESS)
			continue;

		if (size < name_len + 2) {
			r = BRE_OVERFLOW;
			break;
		}
			
		/*
		 * Ought to check that the name is a valid identifier
		 */

		/*
		 * Write the entry out according to the type of data
		 */
		switch (type) {

			case REG_DWORD_LITTLE_ENDIAN:

				dword = *(DWORD *)value;
				BrSprintf(value, "%d", dword);
				value_len = BrStrLen(value);

				goto string;
					
			case REG_DWORD_BIG_ENDIAN:

				dword = *(BYTE *)value << 24 |
					*(BYTE *)(value + 1) << 16 |
					*(BYTE *)(value + 2) << 8 |
					*(BYTE *)(value + 3);
				BrSprintf(value, "%d", dword);
				value_len = BrStrLen(value);

				goto string;

			case REG_NONE:

				if (size < name_len + 1)
					r = BRE_OVERFLOW;

				BrMemCpy(buffer, name, name_len + 1);

				buffer += name_len + 1;
				size -= name_len + 1;

				break;
					
			case REG_SZ:
			string:

				if (size < name_len + 1 + value_len)
					r = BRE_OVERFLOW;
					
				BrMemCpy(buffer, name, name_len);
				buffer[name_len] = '=';
				BrMemCpy(buffer + name_len + 1, value, value_len);

				buffer += name_len + 1 + value_len;
				size -= name_len + 1 + value_len;

				break;

			case REG_BINARY:
			case REG_EXPAND_SZ:
			case REG_LINK:
			case REG_MULTI_SZ:
			case REG_RESOURCE_LIST:
			default:
				break;
		}
	}

	/*
	 * Terminate buffer
	 */
	*buffer++ = '\0';

	BrMemFree(value);
	BrMemFree(name);
	RegCloseKey(hkey);

	return r;
}

br_error BR_RESIDENT_ENTRY HostRegistryWriteKey(void *key, char *subkey,
                                                char *buffer, br_size_t size)
{
	HKEY hkey;
	DWORD disposition, value_len;
	char *name, *value, separator;
	br_error r = BRE_OK;

	if (subkey != NULL) {
		if (RegCreateKeyEx(key? (HKEY)key: HKEY_LOCAL_MACHINE, subkey, 0,
			"", REG_OPTION_NON_VOLATILE, KEY_WRITE,
			NULL, &hkey, &disposition) != ERROR_SUCCESS)
			return BRE_FAIL;
	} else {
		if (RegOpenKeyEx(key? (HKEY)key: HKEY_LOCAL_MACHINE, NULL, 0, KEY_WRITE, &hkey) != ERROR_SUCCESS)
			return BRE_FAIL;
	}

	name = buffer;

	while (*name != '\0') {

		for (value = name; *value != '=' && *value != ':' && *value != '\0'; value++);

		separator = *value;

		if (separator == '\0') {

			if (RegSetValueEx((HKEY)hkey, name, 0, REG_SZ, "", 1) != ERROR_SUCCESS)
				r = BRE_FAIL;

			name = value + 1;

		} else {

			*value++ = '\0';

			value_len = BrStrLen(value) + 1;

			if (RegSetValueEx((HKEY)hkey, name, 0, REG_SZ, value, value_len) != ERROR_SUCCESS)
				r = BRE_FAIL;

			*(value - 1) = separator;

			name = value + value_len;
		}
	}

	RegCloseKey(hkey);

	return r;
}

#else


// INI file & registry queries are currently only defined under Windows.
// We should add ini file parsing from DOS if anyone can be arsed.



br_boolean BR_RESIDENT_ENTRY HostIniSectionExists( char *ini_file, char *section_name )
{
   return BR_FALSE ;
}

br_error BR_RESIDENT_ENTRY HostIniQuery( char *ini_file, char *section_name,
                                            char *entry, char *Buffer, int max,
                                            br_uint_16 *size )
{
   *size = 0 ;
   *Buffer = '\0' ;

   return BRE_FAIL ;
}


br_error BR_RESIDENT_ENTRY HostRegistryQuery( void *hKey, char *Path,
                            char *entry, char *Buffer, 
                            br_uint_16 max, br_uint_16 *size )
{
   *size = 0 ;
   *Buffer = '\0' ;

   return BRE_FAIL ;
}


br_error BR_RESIDENT_ENTRY HostIniReadSection(char *filename, char *section,
                                              char *buffer, br_size_t size)
{
	return BRE_FAIL;
}

br_error BR_RESIDENT_ENTRY HostIniWriteSection(char *filename, char *section,
                                              char *buffer, br_size_t size)
{
	return BRE_FAIL;
}

br_error BR_RESIDENT_ENTRY HostRegistryReadKey(void *key, char *subkey,
                                               char *buffer, br_size_t size)
{
	return BRE_FAIL;
}

br_error BR_RESIDENT_ENTRY HostRegistryWriteKey(void *key, char *subkey,
                                                char *buffer, br_size_t size)
{
	return BRE_FAIL;
}

#endif
