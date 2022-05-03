/*
** Copyright (c) 1995, 3Dfx Interactive, Inc.
** All Rights Reserved.
**
** This is UNPUBLISHED PROPRIETARY SOURCE CODE of 3Dfx Interactive, Inc.;
** the contents of this file may not be disclosed to third parties, copied or
** duplicated in any form, in whole or in part, without the prior written
** permission of 3Dfx Interactive, Inc.
**
** RESTRICTED RIGHTS LEGEND:
** Use, duplication or disclosure by the Government is subject to restrictions
** as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
** and Computer Software clause at DFARS 252.227-7013, and/or in similar or
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished  -
** rights reserved under the Copyright Laws of the United States.
**
** $Revision: 1.1 $
** $Date: 1997/12/11 11:52:57 $
*/

#ifndef __TEXUSINT_H__
#define __TEXUSINT_H__
 
#include "texus.h"

#define GR_TEXFMT_SIZE(x)                               ((x < GR_TEXFMT_16BIT) ? 1:     \
                                                                                 ((x < GR_TEXFMT_32BIT) ? 2: 4))

#define TX_OFORMAT_3DF                                  10
#define TX_OFORMAT_TGA                                  11
#define TX_OFORMAT_PPM                                  12

#define TX_MAX_LEVEL       16
typedef struct  _TxMip {
        int             format;                         // GR_TEXFMT_...
        int             width;                          // pixels
        int             height;                         // pixels
        int             depth;                          // mipmap levels
        int             size;                           // bytes
        void    *data[TX_MAX_LEVEL];
        FxU32   pal[256];
} TxMip;

FxBool  txMipRead(TxMip *txMip, const char *filename, int preferredFormat);
FxBool  txMipReadFromFP(TxMip *txMip, const char *debug_filename, FILE *file, int preferredFormat);
void    txMipWrite(TxMip *txMip, char *file, char *ext, int split);
void    txMipResample(TxMip *destMip, TxMip *srcMip);
void    txMipClamp( TxMip *dstMip, TxMip *srcMip );
void    txMipMipmap(TxMip *txMip);

void    txMipQuantize(TxMip *pxMip, TxMip *txMip, int fmt, FxU32 d, FxU32 comp);
void    txMipNcc(TxMip *pxMip, TxMip *txMip, int fmt, FxU32 dither, FxU32 comp);
void    txMipNccNNet(TxMip *pxMip, TxMip *txMip, int fmt, FxU32 dither, FxU32 comp);
int     txMipPal256(TxMip *pxMip, TxMip *txMip, int fmt, FxU32 dither, FxU32 comp);

void    txMipDequantize(TxMip *txMip, TxMip *pxMip);
void    txMipView(TxMip *txMip, char *filename, int wait, int bgcolor);
void    txViewClose();

int             txLog2(int n);
int     txFloorPow2(int n);
int             txCeilPow2(int n);
int     txGCD(int a, int b);
int             txAspectRatio(int w, int h);
void    txPanic(char *);
void    txError(char *);
void    txYABtoPal256(long *palette, const long* yabTable);
void    txRectCopy(FxU8 *dst, int dstStride, const FxU8 *src, int srcStride,
                        int width, int height);
FxBool  txMipAlloc(TxMip *txMip);
int             txMemRequired(TxMip *txMip);
void    txBasename(const char *name, char *base);
void    txPathAndBasename(const char *name, char* pathbase);
void    txExtension(const char *name, char *ext);

void txMipFree( TxMip *mip );
void txMipTrueToFixedPal( TxMip *outputMip, TxMip *trueColorMip, const FxU32 *pal,
                          FxU32 flags );



extern  int txVerbose;
extern  int *explode3;
#define DISTANCE(ar, ag, ab, br, bg, bb) \
                ((explode3[(ar)-(br)] << 1) + (explode3[(ag)-(bg)]<<2) + explode3[(ab)-(bb)])

void    txDiffuseIndex(TxMip *pxMip, TxMip *txMip, int pixsize, 
                const FxU32 *palette, int       ncolors);
int             txNearestColor(long ir, long ig, long ib, const FxU32 *pal, int npal);

FxBool _txReadTGAHeader( FILE *stream, FxU32 cookie, TxMip *info);
FxBool _txReadTGAData( FILE *stream, TxMip *info);

FxBool _txReadRGTHeader( FILE *stream, FxU32 cookie, TxMip *info);
FxBool _txReadRGTData( FILE *stream, TxMip *info);

FxBool _txReadSBIHeader( FILE *stream, FxU32 cookie, TxMip *info);
FxBool _txReadSBIData( FILE *stream, TxMip *info);

FxBool _txReadPPMHeader( FILE *stream, FxU32 cookie, TxMip *info);
FxBool _txReadPPMData( FILE *stream, TxMip *info);

FxBool _txRead3DFHeader( FILE *stream, FxU32 cookie, TxMip *info);
FxBool _txRead3DFData( FILE *stream, TxMip *info);

int _txReadHeader( FILE *stream, TxMip *info );

#define MAX_TEXWIDTH    1024

extern  char *Format_Name[];

#endif  /* __TEXUSINT_H__ */
