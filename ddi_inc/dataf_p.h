/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: dataf_p.h 1.1 1997/12/10 16:41:05 jon Exp $
 * $Locker: $
 *
 * Prototypes for functions internal to framework
 */
#ifndef _DATAF_P_H_
#define _DATAF_P_H_
#ifndef _NO_PROTOTYPES

#ifdef __cplusplus
extern "C" {
#endif


/*
 * datafile.c
 */
void *DfPop(int type, int *countp);
void DfPush(int type, void *value, int count);
void *DfTop(int type, int *countp);
int DfTopType(void);

int DfChunksInterpret(struct br_datafile *df, struct br_chunks_table *table);

br_uint_32 DfStructWriteArray(struct br_datafile *df, struct br_file_struct *str, void *base, int n);
br_uint_32 DfStructReadArray(struct br_datafile *df, struct br_file_struct *str,void *base,  int n);

struct br_datafile *DfOpen(char *name, int write, br_token scalar_type);
void DfClose(struct br_datafile *df);

/*
 * genfile.c
 */
int FopRead_END(struct br_datafile *df, br_uint_32 id, br_uint_32 length, br_uint_32 count);
int FopWrite_END(struct br_datafile *df);

int FopWrite_FILE_INFO(struct br_datafile *df, br_uint_32 type);

#ifdef __cplusplus
};
#endif
#endif
#endif

