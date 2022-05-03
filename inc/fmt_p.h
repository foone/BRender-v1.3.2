/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: fmt_p.h 1.1 1997/12/10 16:41:00 jon Exp $
 * $Locker: $
 *
 * Function prototypes for foreign file format support
 */
#ifndef _FMT_P_H_
#define _FMT_P_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 3D Studio .3DS files
 */
br_fmt_results * BR_PUBLIC_ENTRY BrFmt3DSLoad(char *name, br_fmt_options *fmt_options, br_3ds_options *options);

/*
 * 3D Studio .ASC files
 */
br_uint_32 BR_PUBLIC_ENTRY BrFmtASCLoad(char *name, br_model **mtable, br_uint_16 max_models);

/*
 * Eric Haines' NFF format
 */
br_model * BR_PUBLIC_ENTRY BrFmtNFFLoad(char *name);

/*
 * Script files
 */
br_uint_32 BR_PUBLIC_ENTRY BrFmtScriptMaterialLoadMany(char *filename, br_material **materials, br_uint_16 num);
br_material * BR_PUBLIC_ENTRY BrFmtScriptMaterialLoad(char *filename);

br_uint_32 BR_PUBLIC_ENTRY BrFmtScriptMaterialSaveMany(char *filename, br_material **materials, br_uint_16 num);
br_uint_32 BR_PUBLIC_ENTRY BrFmtScriptMaterialSave(char *filename, br_material *ptr);

/*
 * Windows .BMP files
 */
br_pixelmap * BR_PUBLIC_ENTRY BrFmtBMPLoad(char *name,br_uint_32 flags);

/*
 * .TGA files
 */
br_pixelmap * BR_PUBLIC_ENTRY BrFmtTGALoad(char *name,br_uint_32 flags);

/*
 * .GIF files
 */
br_pixelmap * BR_PUBLIC_ENTRY BrFmtGIFLoad(char *name,br_uint_32 flags);

/*
 * .IFF/.LBM files
 */
br_pixelmap * BR_PUBLIC_ENTRY BrFmtIFFLoad(char *name,br_uint_32 flags);

/*
 * .VUE files
 */
br_vue * BR_PUBLIC_ENTRY BrVueAllocate(int nframes, int ntransforms);
void BR_PUBLIC_ENTRY BrVueFree(br_vue *vue);
void BR_PUBLIC_ENTRY BrLoadVUE(char *file_name, br_actor * root, br_vue *vue);
void BR_PUBLIC_ENTRY BrApplyVue( br_vue *vue, br_actor * actors);

#ifdef __cplusplus
};
#endif
#endif

