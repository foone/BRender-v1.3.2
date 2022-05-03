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
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
** rights reserved under the Copyright Laws of the United States.
**
** $Header: P:/brender/source/drivers/3dfx_dos/RCS/glide/win32/glideutl.h 1.1 1997/12/11 11:52:54 jon Exp $
** $Log: glideutl.h $
** Revision 1.1  1997/12/11 11:52:54  jon
** Initial revision
*/

/* Glide Utility routines */

#ifndef __GLIDEUTL_H__
#define __GLIDEUTL_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
** rendering functions
*/
FX_ENTRY void FX_CALL
guAADrawTriangleWithClip( const GrVertex *a, const GrVertex
                         *b, const GrVertex *c);

FX_ENTRY void FX_CALL
guDrawTriangleWithClip(
                       const GrVertex *a,
                       const GrVertex *b,
                       const GrVertex *c
                       );

FX_ENTRY void FX_CALL
guDrawPolygonVertexListWithClip( int nverts, const GrVertex vlist[] );

/*
** hi-level rendering utility functions
*/
FX_ENTRY void FX_CALL
guAlphaSource( GrAlphaSource_t mode );

FX_ENTRY void FX_CALL
guColorCombineFunction( GrColorCombineFnc_t fnc );

FX_ENTRY void FX_CALL
guFbReadRegion(
               const int src_x, const int src_y,
               const int w, const int h, const void *dst,
               const int strideInBytes
               );

FX_ENTRY void FX_CALL
guFbWriteRegion(
                const int dst_x, const int dst_y,
                const int w, const int h, const void *src,
                const int strideInBytes
                );
FX_ENTRY FxU16 * FX_CALL
guTexCreateColorMipMap( void );

#if ( GLIDE_PLATFORM & GLIDE_SST_SIM )
/*
** movie capture stuff
*/
FX_ENTRY void
FX_CALL guMovieStart( void );

FX_ENTRY void
FX_CALL guMovieStop( void );

FX_ENTRY void
FX_CALL guMovieSetName( const char *name );
#endif

/*
** fog stuff
*/
FX_ENTRY float FX_CALL
guFogTableIndexToW( int i );

FX_ENTRY void FX_CALL
guFogGenerateExp( GrFog_t fogtable[GR_FOG_TABLE_SIZE], float density );

FX_ENTRY void FX_CALL
guFogGenerateExp2( GrFog_t fogtable[GR_FOG_TABLE_SIZE], float density );

FX_ENTRY void FX_CALL
guFogGenerateLinear(
                    GrFog_t fogtable[GR_FOG_TABLE_SIZE],
                    float nearZ, float farZ );

/*
** endian stuff
*/
FX_ENTRY FxU32 FX_CALL
guEndianSwapWords( FxU32 value );

FX_ENTRY FxU16 FX_CALL
guEndianSwapBytes( FxU16 value );

/*
** hi-level texture manipulation tools.
*/
FX_ENTRY FxBool FX_CALL
gu3dfGetInfo( const char *filename, Gu3dfInfo *info );

FX_ENTRY FxBool FX_CALL
gu3dfLoad( const char *filename, Gu3dfInfo *data );

/*
** Glide structure-less primitive handling
*/
#define GU_PRIM_MAX_VERTICES 100

/* GMT: BUG what the hell is this!!! */
#if 0
extern GrVertex   __gu_prim_vertex_array[GU_PRIM_MAX_VERTICES];
extern GrVertex * __gu_prim_current_vtx;
extern int        __gu_prim_num_verts;

#define guPolygonBegin() __gu_prim_current_vtx = &__gu_prim_vertex_array[0]; __gu_prim_num_verts = 0;
#define guPolygonEnd()

#ifdef GLIDE_DEBUG
#  define guPolygonDraw()        grDrawPolygonVertexList( __gu_prim_num_verts, __gu_prim_vertex_array );
#  define guPolygonDrawClipped() guDrawPolygonVertexListWithClip( __gu_prim_num_verts, __gu_prim_vertex_array );
#else
#  define guPolygonDraw() if ( __gu_prim_num_verts > GU_VTX_MAX_VERTICES ) \
                            grErrorCallback( "guPolygonEnd (GLIDE.H) : GU_VTX_MAX_VERTICES exceed", FXTRUE ); \
                          if ( __gu_prim_num_verts < 3 ) \
                            grErrorCallback( "guPolygonEnd (GLIDE.H) : num_verts< 3", FXTRUE ); \
                          grDrawPolygonVertexList( __gu_prim_num_verts, __gu_prim_vertex_array );
#  define guPolygonDrawClipped() if ( __gu_prim_num_verts > GU_VTX_MAX_VERTICES ) \
                                   grErrorCallback( "guPolygonEnd (GLIDE.H) : GU_VTX_MAX_VERTICES exceed", FXTRUE ); \
                                 if ( __gu_prim_num_verts < 3 ) \
                                   grErrorCallback( "guPolygonEnd (GLIDE.H) : num_verts< 3", FXTRUE ); \
                                 guDrawPolygonVertexListWithClip( __gu_prim_num_verts, __gu_prim_vertex_array );
#endif

#define guVertexBegin()
#define   guVertexXY( x, y )          __gu_prim_current_vtx->x = x; __gu_prim_current_vtx->y = y;
#define   guVertexXYZ( x, y, z )      __gu_prim_current_vtx->x = x; __gu_prim_current_vtx->y = y; __gu_prim_current_vtx->ooz = z;
#define   guVertexXYW( x, y, w )      __gu_prim_current_vtx->x = x; __gu_prim_current_vtx->y = y; __gu_prim_current_vtx->oow = w;
#define   guVertexRGB( r, g, b )      __gu_prim_current_vtx->r = r; __gu_prim_current_vtx->g = g; __gu_prim_current_vtx->b = b;
#define   guVertexRGBA( r, g, b, a )  __gu_prim_current_vtx->r = r; __gu_prim_current_vtx->g = g; __gu_prim_current_vtx->b = b; __gu_prim_current_vtx->a = a;
#define   guVertexA( a )              __gu_prim_current_vtx->a = a;
#define   guVertexSTW0( c, s, t, w )  __gu_prim_current_vtx->tmuvtx[(c)]->sow = s; \
                                      __gu_prim_current_vtx->tmuvtx[(c)]->tow = t; \
                                      __gu_prim_current_vtx->tmuvtx[(c)]->oow = w;
#define guVertexEnd() __gu_prim_current_vtx++; __gu_prim_num_verts++;

#endif

#ifdef __cplusplus
}
#endif

#endif /* __GLIDEUTL_H__ */
