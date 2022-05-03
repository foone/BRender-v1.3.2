/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: pm_p.h 1.3 1998/11/06 14:54:25 jon Exp $
 * $Locker: $
 *
 * Public function prototypes for BRender pixelmap support
 */
#ifndef _PM_P_H_
#define _PM_P_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _NO_PROTOTYPES

/*
 * Pixelmap support setup
 */
void BR_PUBLIC_ENTRY BrPixelmapBegin(void);
void BR_PUBLIC_ENTRY BrPixelmapEnd(void);

/*
 * Pixelmap management
 */
br_pixelmap * BR_RESIDENT_ENTRY BrPixelmapAllocate(br_uint_8 type,
	br_int_32 w,br_int_32 h, void *pixels, int flags);
br_uint_16 BR_RESIDENT_ENTRY BrPixelmapPixelSize(br_pixelmap *pm);
br_uint_16 BR_RESIDENT_ENTRY BrPixelmapChannels(br_pixelmap *pm);

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapAllocateSub(br_pixelmap *pm,
								   br_int_32 x, br_int_32 y,
								   br_int_32 w, br_int_32 h);


void BR_PUBLIC_ENTRY BrPixelmapFree(br_pixelmap *pm);

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapResize(br_pixelmap *pm,
	br_int_32 width, br_int_32 height);

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapMatch(br_pixelmap *src,
	br_uint_8 match_type);

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapMatchSized(br_pixelmap *src,
	br_uint_8 match_type, br_int_32 width, br_int_32 height);

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapMatchTyped(br_pixelmap *src,
	br_uint_8 match_type, br_uint_8 pixelmap_type);

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapMatchTypedSized(br_pixelmap *src,
	br_uint_8 match_type, br_uint_8 pixelmap_type,
	br_int_32 width, br_int_32 height);

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapClone(br_pixelmap *src);

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapDirectLock(br_pixelmap *src, br_boolean block);
br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapDirectUnlock(br_pixelmap *src);

/*
 * Pixelmap operations
 */
void BR_PUBLIC_ENTRY BrPixelmapFill(br_pixelmap *dst, br_uint_32 colour);

void BR_PUBLIC_ENTRY BrPixelmapRectangle(br_pixelmap *dst,
	br_int_32 x,br_int_32 y,br_int_32 w,br_int_32 h,br_uint_32 colour);
void BR_PUBLIC_ENTRY BrPixelmapRectangle2(br_pixelmap *dst,
	br_int_32 x,br_int_32 y,br_int_32 w,br_int_32 h,
	br_uint_32 colour1, br_uint_32 colour2);
void BR_PUBLIC_ENTRY BrPixelmapRectangleCopy(
	br_pixelmap *dst,br_int_32 dx,br_int_32 dy,
	br_pixelmap *src,br_int_32 sx,br_int_32 sy,br_int_32 w,br_int_32 h);
void BR_PUBLIC_ENTRY BrPixelmapRectangleStretchCopy(
	br_pixelmap *dst,br_int_32 dx,br_int_32 dy,br_int_32 dw,br_int_32 dh,
	br_pixelmap *src,br_int_32 sx,br_int_32 sy,br_int_32 sw,br_int_32 sh);
void BR_PUBLIC_ENTRY BrPixelmapRectangleFill(
	br_pixelmap *dst,br_int_32 x,br_int_32 y,br_int_32 w,br_int_32 h,
	br_uint_32 colour);

void BR_PUBLIC_ENTRY BrPixelmapDirtyRectangleCopy(br_pixelmap *dst,
	br_pixelmap *src,br_int_32 x,br_int_32 y,br_int_32 w,br_int_32 h);
void BR_PUBLIC_ENTRY BrPixelmapDirtyRectangleClear(
	br_pixelmap *dst,br_int_32 x,br_int_32 y,br_int_32 w,br_int_32 h,
	br_uint_32 colour);
void BR_PUBLIC_ENTRY BrPixelmapDirtyRectangleDoubleBuffer(br_pixelmap *dst,
	br_pixelmap *src,br_int_32 x,br_int_32 y,br_int_32 w,br_int_32 h);

/*
 * Backwards compatibility
 */
#define BrPixelmapDirtyRectangleFill BrPixelmapDirtyRectangleClear

void BR_PUBLIC_ENTRY BrPixelmapPixelSet(br_pixelmap *dst,
	br_int_32 x,br_int_32 y,br_uint_32 colour);
br_uint_32 BR_PUBLIC_ENTRY BrPixelmapPixelGet(br_pixelmap *dst,
	br_int_32 x,br_int_32 y);
void BR_PUBLIC_ENTRY BrPixelmapCopy(br_pixelmap *dst,br_pixelmap *src);
void BR_PUBLIC_ENTRY BrPixelmapLine(br_pixelmap *dst,
	br_int_32 x1, br_int_32 y1, br_int_32 x2, br_int_32 y2,
	br_uint_32 colour);
void BR_PUBLIC_ENTRY BrPixelmapText(br_pixelmap *dst,
	br_int_32 x, br_int_32 y,br_uint_32 colour, br_font *font, char *text);
void BR_PUBLIC_ENTRY BrPixelmapTextF(br_pixelmap *dst,
	br_int_32 x, br_int_32 y,br_uint_32 colour, br_font *font, char *fmt,...);

void BR_PUBLIC_ENTRY BrPixelmapCopyBits(br_pixelmap *dst,
	br_int_32 x,br_int_32 y,
	br_uint_8 *src,br_int_32 s_stride,
	br_int_32 start_bit,br_int_32 end_bit,
	br_int_32 nrows,
	br_uint_32 colour);

br_uint_16 BR_PUBLIC_ENTRY BrPixelmapTextWidth(br_pixelmap *dst,
	br_font *font, char *text);
br_uint_16 BR_PUBLIC_ENTRY BrPixelmapTextHeight(br_pixelmap *dst, br_font *font);

void BR_PUBLIC_ENTRY BrPixelmapDoubleBuffer(br_pixelmap *dst,br_pixelmap *src);

void BR_PUBLIC_ENTRY BrPixelmapPaletteSet(br_pixelmap *pm, br_pixelmap *pal);
void BR_PUBLIC_ENTRY BrPixelmapPaletteEntrySet(br_pixelmap *pm, br_int_32 i,br_colour colour);
void BR_PUBLIC_ENTRY BrPixelmapPaletteEntrySetMany(br_pixelmap *pm, br_int_32 index , br_int_32 ncolours, br_colour *colours);

/*
 * Backwards compatibility
 */
#define BrPixelmapPlot BrPixelmapPixelSet

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapLoad(char *filename);
br_uint_32 BR_PUBLIC_ENTRY BrPixelmapSave(char *filename,
	br_pixelmap *pixelmap);
br_uint_32 BR_PUBLIC_ENTRY BrPixelmapLoadMany(char *filename,
	br_pixelmap **pixelmaps,br_uint_16 num);
br_uint_32 BR_PUBLIC_ENTRY BrPixelmapSaveMany(char *filename,
	br_pixelmap **pixelmaps,br_uint_16 num);

/*
 * Scaling (XXX interface will change)
 */
void BR_PUBLIC_ENTRY BrScaleBegin(void);
void BR_PUBLIC_ENTRY BrScaleEnd(void);

/*
 * scale pixelmap src to new_x,new_y with filter width fwidth
 * src must be BR_PMT_RGB_888
 */
br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapScale(br_pixelmap *src,
	br_uint_32 new_x,br_uint_32 new_y,float fwidth);

/*
 * Qunatization  (XXX interface will change)
 */
void BR_PUBLIC_ENTRY BrQuantBegin(void);
void BR_PUBLIC_ENTRY BrQuantEnd(void);

/*
 * add array of <size> rgb values to quantizer
 */
void BR_PUBLIC_ENTRY BrQuantAddColours(br_uint_8 *colours, br_uint_32 size);

/*
 * make optimum palette
 */
void BR_PUBLIC_ENTRY BrQuantMakePalette(int base,int num_entries,
	br_pixelmap *palette);

/*
 * given palette with base and num_entries, setup internal map
 */
void BR_PUBLIC_ENTRY BrQuantPrepareMapping(int base,int num_entries,
	br_pixelmap *palette);

/*
 * get <size> index values for array of <size> rgb values
 */
void BR_PUBLIC_ENTRY BrQuantMapColours(int base,br_uint_8 *colours,
	br_uint_8 *mapped_colours, int size);

/*
 * Produce a pixelmap containing mip levels from a pixelmap. 
 */

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapMakeMipMap(br_pixelmap *source,
	br_uint_32 destinationType,br_pixelmap *palette,br_uint_32 base,
	br_uint_32 range,br_uint_32 quantizationMethod);


br_error BR_PUBLIC_ENTRY BrPixelmapGetControls(br_pixelmap *pm, br_display_controls * controls);
br_error BR_PUBLIC_ENTRY BrPixelmapSetControls(br_pixelmap *pm, br_display_controls * controls);

#endif /* _NO_PROTOTYPES */

#ifdef __cplusplus
};
#endif
#endif

