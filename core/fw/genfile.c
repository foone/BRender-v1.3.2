/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: genfile.c 1.1 1997/12/10 16:41:08 jon Exp $
 * $Locker: $
 *
 * General per-chunk file operations
 *
 */
#include <stddef.h>

#include "fw.h"
#include "brassert.h"
#include "datafile.h"

BR_RCS_ID("$Id: genfile.c 1.1 1997/12/10 16:41:08 jon Exp $")

/**
 ** End Marker
 **/
int FopRead_END(br_datafile *df, br_uint_32 id, br_uint_32 length, br_uint_32 count)
{
	return 1;
}

int FopWrite_END(br_datafile *df)
{
	df->prims->chunk_write(df,FID_END,0);

	return 0;
}

/**
 ** File Info
 **/
struct file_info {
	br_uint_32 type;
	br_uint_32 version;
};

br_file_enum_member file_type_FM[] = {
	_ENUM_MEMBER(FILE_TYPE_NONE),

	_ENUM_MEMBER(FILE_TYPE_ACTORS),
	_ENUM_MEMBER(FILE_TYPE_PIXELMAP),
	_ENUM_MEMBER(FILE_TYPE_LIGHT),
	_ENUM_MEMBER(FILE_TYPE_CAMERA),
    _ENUM_MEMBER(FILE_TYPE_MATERIAL_OLD),

	_ENUM_MEMBER(FILE_TYPE_MODEL),
	_ENUM_MEMBER(FILE_TYPE_ANIMATION),
	_ENUM_MEMBER(FILE_TYPE_TREE),

    _ENUM_MEMBER(FILE_TYPE_MATERIAL)
};

STATIC _FILE_ENUM(file_type);

#define _STRUCT_NAME struct file_info
STATIC br_file_struct_member file_info_FM[] = {
	_ENUM_32(type,file_type_F),
	_UINT_32(version),
};
STATIC _FILE_STRUCT(file_info);
#undef _STRUCT_NAME

#if 0
int FopRead_FILE_INFO(br_datafile *df, br_uint_32 id, br_uint_32 length, br_uint_32 count)
{
	struct file_info fi;

	df->prims->struct_read(df,&file_info_F, &fi);

	return fi.type;
}
#endif

int FopWrite_FILE_INFO(br_datafile *df, br_uint_32 type)
{
	struct file_info fi;

	fi.type = type;
	fi.version = BR_FILE_VERSION;

	df->prims->chunk_write(df,FID_FILE_INFO,df->prims->struct_size(df,&file_info_F, &fi));
	df->prims->struct_write(df,&file_info_F, &fi);

	return 0;
}

