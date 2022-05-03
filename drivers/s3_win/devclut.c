/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: devclut.c 1.1 1997/12/10 16:50:48 jon Exp $
 * $Locker: $
 *
 * CLUT methods
 */
#include <stddef.h>
#include <string.h>
#include <windows.h>
#include <ddraw.h>
#include <conio.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: devclut.c 1.1 1997/12/10 16:50:48 jon Exp $");

/*
 * Default dispatch table for device_clut (defined at end of file)
 */
static struct br_device_clut_dispatch deviceClutDispatch;

/*
 * Query template
 */
#define F(f)	offsetof(struct br_device_clut, f)

static struct br_tv_template_entry deviceClutTemplateEntries[] = {
	{BRT_IDENTIFIER_CSTR,	0,	F(identifier),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
};
#undef F

static struct br_tv_template deviceClutTemplate = {
	BR_ASIZE(deviceClutTemplateEntries),
	deviceClutTemplateEntries
};


void ClearDevclutTemplates(void)
{
	CLEAR_TEMPLATE(deviceClut);
}

/*
 * This is the default palette used for paletted textures with no palette data attached
 * It is used to convert them to 15 bit textures for true colour video modes
 * Conversion code is in sbuffer.c
 */
extern br_colour	S3_default_palette[256];


/*
 * Create a new device CLUT
 */
br_device_clut * DeviceClutS3Allocate(br_device *dev, br_device_pixelmap *devpm, char *identifier)
{
	int i, half_nstatic;
	br_size_t size;
	br_device_clut *self;
	HDC dc;
	LOGPALETTE *logpalette;

	UASSERT(dev != NULL);
	UASSERT(devpm != NULL);

	/*
	 * Allocate device CLUT structure
	 */
	self = BrResAllocate(DeviceS3Resource(dev), sizeof(*self), BR_MEMORY_OBJECT);

	/*
	 * Fill members
	 */
	self->dispatch = &deviceClutDispatch;
	if(identifier)
	 	self->identifier = identifier;
	self->devpm = devpm;

	if(devpm->pm_type == BR_PMT_INDEX_8)
		self->running_indexed = BR_TRUE;
	else
		self->running_indexed = BR_FALSE;

	/* Only do the setup for the direct draw palette if we are in an indexed mode */
	if(self->running_indexed)
      {
		/* Create a logical palette */
		dc = GetDC(NULL);
      /*	self->nstatic = GetDeviceCaps(dc, NUMCOLORS); */
      /* No static colours for S3 driver */
      self->nstatic = 0;
      self->syspal_size = GetDeviceCaps(dc, SIZEPALETTE);

		size = sizeof(LOGPALETTE) + self->syspal_size * sizeof(PALETTEENTRY);
		logpalette = (LOGPALETTE *)BrResAllocate(self, size, BR_MEMORY_OBJECT_DATA);
		self->logpalette = logpalette;

		logpalette->palVersion = 0x300;
		logpalette->palNumEntries = self->syspal_size;

		GetSystemPaletteEntries(dc, 0, self->syspal_size, &logpalette->palPalEntry[0]);
		ReleaseDC(NULL, dc);

		half_nstatic = self->nstatic / 2;
		for(i = 0; i < half_nstatic ; i++)
			logpalette->palPalEntry[i].peFlags = 0;
		for(; i < self->syspal_size - half_nstatic; i++)
			logpalette->palPalEntry[i].peFlags = PC_NOCOLLAPSE;
		for(; i < self->syspal_size; i++)
			logpalette->palPalEntry[i].peFlags = 0;

		self->palette = CreatePalette(logpalette);
		devpm->hpal = self->palette;
		}
	else
		{
		/* Zero the members of the default dummy palette */
		for(i=0;i<256;i++)
			S3_default_palette[i] = 0;
		}

	/* Attach CLUT to device */
	ObjectContainerAddFront(dev, (br_object *)self);

	return self;
}

/*
 * br_device_clut_s3::free
 */
static void BR_CMETHOD_DECL(br_device_clut_s3, free)\
	(br_device_clut *self)
{
	/* Detach CLUT from device */
	ObjectContainerRemove(ObjectDevice(self), (br_object *)self);

	if(self->running_indexed)
		{
		/* Release the logical palette */
		DeleteObject(self->palette);
		}

	/* Free resources */
	BrResFreeNoCallback(self);
}

static br_token BR_CMETHOD_DECL(br_device_clut_s3, type)\
	(br_device_clut *self)
{
	return BRT_DEVICE_CLUT;
}

static br_boolean BR_CMETHOD_DECL(br_device_clut_s3, isType)\
	(br_device_clut *self, br_token t)
{
	return (t == BRT_DEVICE_CLUT) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_device_clut_s3, space)\
	(br_device_clut *self)
{
	return sizeof(br_device_clut);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_device_clut_s3, queryTemplate)\
	(br_device_clut *self)
{
	deviceClutTemplate.res = DeviceS3Resource(ObjectS3Device(self));
	return &deviceClutTemplate;
}

/*
 * br_device_clut_s3::entrySet
 */
static br_error BR_CMETHOD_DECL(br_device_clut_s3, entrySet)\
	(br_device_clut *self, br_int_32 index, br_colour entry)
{
	int half_nstatic;
	PALETTEENTRY palette_entry;

	/* Only do direct draw palette setup if we are in an indexed video mode */
	if(self->running_indexed)
		{
		/* Check index does not reference a static colour */
		half_nstatic = self->nstatic / 2;
		if(index < half_nstatic)
			return BRE_FAIL;
		if(index >= (self->syspal_size - half_nstatic))
			return BRE_FAIL;

		/* Fill palette entry */
		palette_entry.peRed = (BYTE)BR_RED(entry);
		palette_entry.peGreen = (BYTE)BR_GRN(entry);
		palette_entry.peBlue = (BYTE)BR_BLU(entry);
		palette_entry.peFlags = PC_NOCOLLAPSE;
	
		/* Set the single palette entry */
		SetPaletteEntries(self->palette, index, 1, &palette_entry);
      IDirectDrawPalette_SetEntries(self->devpm->palette, 0, index, 1, &palette_entry);
      IDirectDrawSurface_SetPalette(self->devpm->surface, self->devpm->palette);
		}
	else
		/* Set entry in dummy default palette */
		S3_default_palette[index] = entry;

	return BRE_OK;
}

/*
 * br_device_clut_s3::entrySetMany
 */
static br_error BR_CMETHOD_DECL(br_device_clut_s3, entrySetMany)\
	(br_device_clut *self, br_int_32 index, br_int_32 count, br_colour *entries)
{
	int i;
	br_colour entry;
	PALETTEENTRY *palette_entries;

   if(!self->running_indexed)
      { 
	   /* Set the entries in the default dummy palette used for texture conversion */
	   if(index+count <= 256)
		   {
		   for(i=index;i<index+count;i++)
			   S3_default_palette[i] = entries[i-index];
		   }
      }

	/* Now do the direct draw palette setup if we are in an indexed mode */
	if(self->running_indexed)
		{
      /* Check there are still entries to set */
		if(count <= 0)
			return BRE_FAIL;

      /* Allocate temporary workspace */
		palette_entries = (PALETTEENTRY *)BrMemAllocate(count * sizeof(PALETTEENTRY), BR_MEMORY_SCRATCH);

      if(!palette_entries)
         return(BRE_FAIL);

		/* Fill palette entry structures */
		for(i = 0; i < count; i++)
			{
			entry = entries[i];
			palette_entries[i].peRed = (BYTE)BR_RED(entry);
			palette_entries[i].peGreen = (BYTE)BR_GRN(entry);
			palette_entries[i].peBlue = (BYTE)BR_BLU(entry);
			palette_entries[i].peFlags = PC_NOCOLLAPSE;
			}

		/* Set the palette entries */
      SetPaletteEntries(self->palette, index, count, palette_entries);
      IDirectDrawPalette_SetEntries(self->devpm->palette, 0, index, count, palette_entries);
      IDirectDrawSurface_SetPalette(self->devpm->surface, self->devpm->palette);

      /* Free workspace */
		BrMemFree(palette_entries);
		}

	return BRE_OK;
}


/*
 * br_device_clut_s3::entryQuery
 */
static br_error BR_CMETHOD_DECL(br_device_clut_s3, entryQuery)\
	(br_device_clut *self, br_colour *entry, br_int_32 index)
{
	return BRE_FAIL;
}

/**
 ** br_device_clut_s3::entryQueryMany
 **/
static br_error BR_CMETHOD_DECL(br_device_clut_s3, entryQueryMany)\
	(br_device_clut *self, br_colour *entries, br_int_32 index, br_int_32 count)
{
	return BRE_FAIL;
}

/*
 * Default dispatch table for device CLUT
 */
static struct br_device_clut_dispatch deviceClutDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_device_clut_s3,	free),
	BR_CMETHOD_REF(br_object_s3,		   identifier),
	BR_CMETHOD_REF(br_device_clut_s3,	type),
	BR_CMETHOD_REF(br_device_clut_s3,	isType),
	BR_CMETHOD_REF(br_object_s3,		   device),
	BR_CMETHOD_REF(br_device_clut_s3,	space),

	BR_CMETHOD_REF(br_device_clut_s3,	queryTemplate),
	BR_CMETHOD_REF(br_object,			   query),
	BR_CMETHOD_REF(br_object,			   queryBuffer),
	BR_CMETHOD_REF(br_object,			   queryMany),
	BR_CMETHOD_REF(br_object,			   queryManySize),
	BR_CMETHOD_REF(br_object,			   queryAll),
	BR_CMETHOD_REF(br_object,			   queryAllSize),

	BR_CMETHOD_REF(br_device_clut_s3,	entrySet),
	BR_CMETHOD_REF(br_device_clut_s3,	entryQuery),
	BR_CMETHOD_REF(br_device_clut_s3,	entrySetMany),
	BR_CMETHOD_REF(br_device_clut_s3,	entryQueryMany),
};


