/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: fw_ip.h 1.1 1997/12/10 16:41:07 jon Exp $
 * $Locker: $
 *
 * Prototypes for functions internal to framework
 */
#ifndef _FW_IP_H_
#define _FW_IP_H_
#ifndef _NO_PROTOTYPES

#ifdef __cplusplus
extern "C" {
#endif

/*
 * pool.c
 */
void BR_ASM_CALLBACK BrPoolAddChunk(br_pool *pool);

/*
 * Debugging writes
 */
int BR_PUBLIC_ENTRY BrLogWrite(void *buffer, br_size_t s, br_size_t n);

/*
 * file.c
 */
void BR_CALLBACK _BrFileFree(void *res, br_uint_8 res_class, br_size_t size);


/*
 * token.c
 */
void BrTokenBegin(void);
br_token BrTokenFindType(br_token *ptype, char *base, br_token *types, br_int_32 ntypes);


/*
 * object.c
 */
void BR_CALLBACK _BrObjectFree(void *res, br_uint_8 res_class, br_size_t size);


/*
 * image.c
 */
void BR_CALLBACK _BrImageFree(void *res, br_uint_8 res_class, br_size_t size);

/*
 * loader.c
 */
br_image *ImageLoad(char *name);

/*
 * brbhook.c
 */
void BR_CALLBACK _BrBeginHook(void);
void BR_CALLBACK _BrEndHook(void);

#ifdef __cplusplus
};
#endif
#endif
#endif

