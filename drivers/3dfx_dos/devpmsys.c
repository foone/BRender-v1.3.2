

#include "drv.h"
#include "pm.h"
#include "host.h"
#include "shortcut.h"
#include "brassert.h"



br_token BR_CMETHOD_DECL(br_device_pixelmap_3dfx, type)(br_device_pixelmap *self);
br_boolean BR_CMETHOD_DECL(br_device_pixelmap_3dfx, isType)(br_device_pixelmap *self, br_token t);
br_int_32 BR_CMETHOD_DECL(br_device_pixelmap_3dfx, space)(br_device_pixelmap *self);
struct br_tv_template * BR_CMETHOD_DECL(br_device_pixelmap_3dfx,queryTemplate)
	(br_device_pixelmap *self);
br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, rectangleStretchCopy)\
	(br_device_pixelmap *self, br_rectangle *d, br_device_pixelmap *src, br_rectangle *s);



static struct br_device_pixelmap_dispatch devicePixelmapDispatchSysmem;

/*
 * Generic system memory Device Pixelmaps
 *
 * All they need is allocate and free - everything else goes down to the
 * memory pixelmaps!
 *
 * Except rectangle stretchcopies!
 */


/* THIS FIRST BIT NICKED OUT OF BRENDER */

/*
 * Useful info about each pixelmap type
 */
struct pm_type_info {
	/*
	 * Size, in bits, of each pixel (including padding)
	 */
	br_uint_16 bits;

	/*
	 * Size, in bytes, that should be used for saving/loading
	 */
	br_uint_16 file_size;

	/*
	 * Alignment of rows, in pixels
	 */
	br_uint_16 align;

	/*
	 * Mask of channels in pixelmap
	 */
	br_uint_16 channels;

};

static struct pm_type_info pmTypeInfo[] = {
    { 1,    1,      32, BR_PMCHAN_INDEX,                        },
    { 2,    1,      16, BR_PMCHAN_INDEX,                        },
    { 4,    1,      8,  BR_PMCHAN_INDEX,                        },
    { 8,    1,      4,  BR_PMCHAN_INDEX,                        },

    { 16,   2,      2,  BR_PMCHAN_RGB,                          },
    { 16,   2,      2,  BR_PMCHAN_RGB,                          },
    { 24,   3,      4,  BR_PMCHAN_RGB,                          },
    { 32,   4,      1,  BR_PMCHAN_RGB,                          },
    { 32,   4,      1,  BR_PMCHAN_RGB | BR_PMCHAN_ALPHA,        },

    { 16,   1,      2,  BR_PMCHAN_YUV,                          },
    { 32,   1,      1,  BR_PMCHAN_YUV,                          },

    { 16,   2,      4,  BR_PMCHAN_DEPTH,                        },
    { 32,   4,      4,  BR_PMCHAN_DEPTH,                        },
    { 8,    1,      4,  BR_PMCHAN_ALPHA,                        },
    { 16,   2,      2,  BR_PMCHAN_INDEX | BR_PMCHAN_ALPHA       },
};

br_error Allocate3DfxSysMemPixelmap(br_device_pixelmap *self, br_device_pixelmap **newpm, int w, int h)
{
	br_uint_16 qual;
	br_device *dev;
	br_device_pixelmap *devpm;
    struct pm_type_info *tip = pmTypeInfo + self->pm_type;

	UASSERT(self != NULL);
	UASSERT(newpm != NULL);
	UASSERT((w > 0) && (h > 0));

	/* Allocate new pixelmap */
	dev = ObjectDevice(self);
	devpm = BrResAllocate(Device3DfxResource(dev), sizeof(*devpm), BR_MEMORY_OBJECT);

	/* Fill members, mostly by copy */
	*devpm = *self;
	devpm->dispatch = &devicePixelmapDispatchSysmem;
	devpm->pm_width = w;
	devpm->pm_height = h;
	devpm->pm_row_bytes = tip->bits * tip->align * ((w+tip->align-1) / tip->align) / 8;
	HostSelectorDS(&qual);
	devpm->pm_pixels_qualifier = qual;
	devpm->buffer_type = BT_SYSMEM;

	/* Allocate pixels */
	devpm->pm_pixels = BrResAllocate(devpm, devpm->pm_row_bytes*devpm->pm_height, BR_MEMORY_PIXELS);

	*newpm = devpm;

	return BRE_OK;
}


static void BR_CMETHOD_DECL(br_device_pixelmap_3dfx_sysmem, free)(br_device_pixelmap *self)
{
	UASSERT(self != NULL);

	/*
	 * Free pixelmap structure
	 */
	BrResFreeNoCallback(self);
}




/*
 * Default dispatch table for device pixelmap
 */
static struct br_device_pixelmap_dispatch devicePixelmapDispatchSysmem = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_device_pixelmap_3dfx_sysmem, free),
	BR_CMETHOD_REF(br_object_3dfx,			identifier),
	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	type),
	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	isType),
	BR_CMETHOD_REF(br_object_3dfx,			device),
	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	space),

	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	queryTemplate),
	BR_CMETHOD_REF(br_object,				query),
	BR_CMETHOD_REF(br_object,				queryBuffer),
	BR_CMETHOD_REF(br_object,				queryMany),
	BR_CMETHOD_REF(br_object,				queryManySize),
	BR_CMETHOD_REF(br_object,				queryAll),
	BR_CMETHOD_REF(br_object,				queryAllSize),

	BR_CMETHOD_REF(br_device_pixelmap_mem, validSource),
	BR_CMETHOD_REF(br_device_pixelmap_null, resize),
	BR_CMETHOD_REF(br_device_pixelmap_mem, match),
	BR_CMETHOD_REF(br_device_pixelmap_mem, allocateSub),

	BR_CMETHOD_REF(br_device_pixelmap_gen, copyTo),
	BR_CMETHOD_REF(br_device_pixelmap_gen, copyTo),
	BR_CMETHOD_REF(br_device_pixelmap_gen, copyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_mem, fill),
	BR_CMETHOD_REF(br_device_pixelmap_mem, doubleBuffer),

	BR_CMETHOD_REF(br_device_pixelmap_gen, copyDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen, copyToDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen, copyFromDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen, fillDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen, doubleBufferDirty),

	BR_CMETHOD_REF(br_device_pixelmap_gen, rectangle),
	BR_CMETHOD_REF(br_device_pixelmap_gen, rectangle2),
	BR_CMETHOD_REF(br_device_pixelmap_mem, rectangleCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_mem, rectangleCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_mem, rectangleCopyFrom),
    BR_CMETHOD_REF(br_device_pixelmap_3dfx, rectangleStretchCopy),
    BR_CMETHOD_REF(br_device_pixelmap_mem, rectangleStretchCopyTo),
    BR_CMETHOD_REF(br_device_pixelmap_mem, rectangleStretchCopyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_mem, rectangleFill),
	BR_CMETHOD_REF(br_device_pixelmap_mem, pixelSet),
	BR_CMETHOD_REF(br_device_pixelmap_gen, line),
	BR_CMETHOD_REF(br_device_pixelmap_gen, copyBits),


	BR_CMETHOD_REF(br_device_pixelmap_gen, text),
	BR_CMETHOD_REF(br_device_pixelmap_gen, textBounds),

	BR_CMETHOD_REF(br_device_pixelmap_mem, rowSize),
	BR_CMETHOD_REF(br_device_pixelmap_mem, rowQuery),
	BR_CMETHOD_REF(br_device_pixelmap_mem, rowSet),

	BR_CMETHOD_REF(br_device_pixelmap_mem, pixelQuery),
	BR_CMETHOD_REF(br_device_pixelmap_mem, pixelAddressQuery),

	BR_CMETHOD_REF(br_device_pixelmap_mem, pixelAddressSet),
	BR_CMETHOD_REF(br_device_pixelmap_mem, originSet),

	BR_CMETHOD_REF(br_device_pixelmap_mem, flush),
	BR_CMETHOD_REF(br_device_pixelmap_mem, synchronise),
    BR_CMETHOD_REF(br_device_pixelmap_mem, directLock),
    BR_CMETHOD_REF(br_device_pixelmap_mem, directUnlock),
};
