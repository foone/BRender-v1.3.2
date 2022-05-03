/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: file.c 1.1 1997/12/10 16:41:07 jon Exp $
 * $Locker: $
 *
 * Low level wrappers for file system access
 */
#include <stdarg.h>

#include "fw.h"
#include "brassert.h"

BR_RCS_ID("$Id: file.c 1.1 1997/12/10 16:41:07 jon Exp $")

/*
 * Resource block used to represent an open file
 */
typedef struct br_file {
	void *raw_file;
	br_boolean writing;
	int mode;
	char name[1];
} br_file;

/*
 * Resource destructor for files
 */
void BR_CALLBACK _BrFileFree(void *res, br_uint_8 res_class, br_size_t size)
{
	br_file *file = res;

	/*
	 * Close low level file
	 */
	ASSERT(fw.fsys->close != NULL);

	fw.fsys->close(((br_file *)file)->raw_file);
}

br_uint_32 BR_PUBLIC_ENTRY BrFileAttributes(void)
{
	ASSERT(fw.fsys->attributes != NULL);

	return fw.fsys->attributes();
}

void * BR_PUBLIC_ENTRY BrFileOpenRead(char *name, br_size_t n_magics,
		br_mode_test_cbfn *mode_test,
		int *mode_result)
{
	void *raw_file;
	br_file *file;
	int bin_mode = BR_FS_MODE_BINARY;

	ASSERT(fw.fsys->open_read != NULL);

	/*
	 * Invoke low level file access
	 */
	if(mode_result)
		raw_file = fw.fsys->open_read(name,n_magics,mode_test,mode_result);
	else
		raw_file = fw.fsys->open_read(name,n_magics,mode_test,&bin_mode);
	
	/*
	 * Catch failure
	 */
	if(raw_file == NULL)
		return NULL;

	/*
	 * Create a file block (which includes the name and mode)
	 */
	file = BrResAllocate(fw.res, sizeof(br_file) + BrStrLen(name)+1, BR_MEMORY_FILE);
	file->writing = BR_FALSE;

	if ( mode_result != NULL )
		file->mode = *mode_result;
	else
		file->mode = BR_FS_MODE_BINARY;

	file->raw_file = raw_file;
	BrStrCpy(file->name,name);

	return file;
}

void * BR_PUBLIC_ENTRY BrFileOpenWrite(char *name, int mode)
{
	void *raw_file;
	br_file *file;

	ASSERT(fw.fsys->open_write != NULL);

UASSERT_MESSAGE("Invalid mode passed to BrFileOpenWrite.\n\r Must be BR_FS_MODE_TEXT or BR_FS_MODE_BINARY.",((mode== BR_FS_MODE_TEXT) || (mode == BR_FS_MODE_BINARY)));

	raw_file = fw.fsys->open_write(name,mode);

	/*
	 * Catch failure
	 */
	if(raw_file == NULL)
		return NULL;

	/*
	 * Create a file block (which includes the name and mode)
	 */
	file = BrResAllocate(fw.res, sizeof(br_file) + BrStrLen(name)+1, BR_MEMORY_FILE);
	file->writing = BR_TRUE;
	file->mode = mode;
	file->raw_file = raw_file;
	BrStrCpy(file->name,name);

	return file;

}

void BR_PUBLIC_ENTRY BrFileClose(void *f)
{
UASSERT_MESSAGE("In BrFileClose",f!=NULL);
	BrResFree(f);
}

int BR_PUBLIC_ENTRY BrFileEof(void *f)
{
	ASSERT(fw.fsys->eof != NULL);

UASSERT_MESSAGE("In BrFileEof",f!=NULL);

	return fw.fsys->eof(((br_file *)f)->raw_file);
}

int BR_PUBLIC_ENTRY BrFileGetChar(void *f)
{
	ASSERT(fw.fsys->getchr != NULL);

UASSERT_MESSAGE("In BrFileGetChar",f!=NULL);

	return fw.fsys->getchr(((br_file *)f)->raw_file);
}

void BR_PUBLIC_ENTRY BrFilePutChar(int c, void *f)
{
	ASSERT(fw.fsys->putchr != NULL);

UASSERT_MESSAGE("In BrFilePutChar",f!=NULL);

	fw.fsys->putchr(c,((br_file *)f)->raw_file);
}

int BR_PUBLIC_ENTRY BrFileRead(void *buf, int size, int n, void *f)
{
UASSERT_MESSAGE("In BrFileRead",f!=NULL);
UASSERT_MESSAGE("In BrFileRead",buf!=NULL);

	ASSERT(fw.fsys->read != NULL);

	return fw.fsys->read(buf,size,n,((br_file *)f)->raw_file);
}

int BR_PUBLIC_ENTRY BrFileWrite(void *buf, int size, int n, void *f)
{
   UASSERT_MESSAGE("BrFileWrite",f!=NULL);
   UASSERT_MESSAGE("BrFileWrite",buf!=NULL);
                                 
   ASSERT(fw.fsys->write != NULL);

   return fw.fsys->write(buf,size,n,((br_file *)f)->raw_file);
}

int BR_PUBLIC_ENTRY BrFileGetLine(char *buf, br_size_t buf_len, void * f)
{
    UASSERT_MESSAGE("BrFileGetLine",f!=NULL);

	ASSERT( ((br_file *)f)->raw_file );

	return fw.fsys->getline(buf,buf_len,((br_file *)f)->raw_file);
}

void BR_PUBLIC_ENTRY BrFilePutLine(char *buf, void * f)
{
UASSERT_MESSAGE("BrFilePutLine",f!=NULL);

	ASSERT(fw.fsys->putline != NULL);


	fw.fsys->putline(buf,((br_file *)f)->raw_file);
}

void BR_PUBLIC_ENTRY BrFileAdvance(long int count, void *f)
{
UASSERT_MESSAGE("In BrFileAdvance",f!=NULL);

	ASSERT(fw.fsys->advance != NULL);

	fw.fsys->advance(count,((br_file *)f)->raw_file);
}


int BR_PUBLIC_ENTRY BrFilePrintf(void *f, char *fmt, ...)
{
	int n;
	va_list args;
UASSERT_MESSAGE("In BrFilePrintf",f!=NULL);

	ASSERT(fw.fsys->write != NULL);

	va_start(args, fmt);
	n = BrVSprintf(BrScratchString(), fmt, args);
	va_end(args);

	fw.fsys->write(BrScratchString(), 1, n, ((br_file *)f)->raw_file);

	return n;
}

