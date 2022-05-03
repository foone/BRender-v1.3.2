/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Stored buffer methods
 */
#include <stddef.h>
#include <string.h>

#include "texture.h"

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"
#include <host.h>




#define USE_TEXTURE_HEAP		// MUST not be undefined for DOS version
#define USE_HEAP_FREEING




/*
 * Default dispatch table for primitive state (defined at end of file)
 */
static struct br_buffer_stored_dispatch bufferStoredDispatch;

/*
 * Primitive state info. template
 */
#define F(f)	offsetof(struct br_buffer_stored, f)

static struct br_tv_template_entry bufferStoredTemplateEntries[] = {
	{BRT_IDENTIFIER_CSTR,	0,	F(identifier),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
};
#undef F

static struct br_tv_template bufferStoredTemplate = {
	BR_ASIZE(bufferStoredTemplateEntries),
	bufferStoredTemplateEntries
};


/*
 * Translate any tokens passed down to the allocate
 */
struct sbufferAllocate_tokens {
	br_uint_32 flags;
};

#define F(f)	offsetof(struct sbufferAllocate_tokens, f)
static struct br_tv_template_entry sbufferAllocateTemplateEntries[] = {
	{BRT(PREFER_SHARE_B),	F(flags),	BRTV_SET | BRTV_QUERY | BRTV_ALL,	BRTV_CONV_BIT,	SBUF_PREFER_SHARE,	1},
};
#undef F

static struct br_tv_template sbufferAllocateTemplate = {
	BR_ASIZE(sbufferAllocateTemplateEntries),
	sbufferAllocateTemplateEntries
};




void BufferStoredMystClearTemplate(void)
{
	CLEAR_TEMPLATE(bufferStored);
	CLEAR_TEMPLATE(sbufferAllocate);
}








/*
 * Heap memory management
 *
 * Managed by 
 */
struct heap_memory_block_t;
typedef struct heap_memory_block_t heap_memory_block;
struct heap_memory_block_t {
	MSImemptr heap;				// Which heap is this block in
	MSImemptr mem;				// Pointer to the memory in the heap
	br_uint_32 size;			// Memory block size
	heap_memory_block *next;	// Next block on free list
};


static heap_memory_block *heap_list = NULL;

/* Allocate heap in blocks of this many 4K pages: 256 is 1M blocks */
#define HEAP_BLOCK_PAGES		256
#define MAX_HEAP_OBJECT_SIZE	(HEAP_BLOCK_PAGES * 4096)


/*
 * No heap memory available; expand the heap
 */
static br_error TextureHeapExpand(void)
{
	heap_memory_block *newblock;
	MSImemptr heap;

	heap = msiAllocTextureHeap(HEAP_BLOCK_PAGES);

	if (!heap)
		return(BRE_DEV_NO_MEMORY);

	newblock = BrResAllocate(DRIVER_RESOURCE, sizeof(heap_memory_block), BR_MEMORY_SCRATCH);
	if (!newblock) {
		msiFreeTextureHeap(heap);
		return(BRE_DEV_FAIL);		// Different error to indicate severe failure
	}

	/* Setup and link into list */
	newblock->heap = heap;
	newblock->mem  = heap;
	newblock->size = MAX_HEAP_OBJECT_SIZE;
	newblock->next = heap_list;
	heap_list = newblock;

	return(BRE_OK);
}


/*
 * Allocate heap memory
 */
static heap_memory_block *TextureHeapMalloc(br_uint_32 size)
{
	heap_memory_block *last, *block, *newblock;

	/* Align size to dword boundary to keep all allocations on dword boundaries */
	size = (size + 3) & ~3;

	/* Clip out anything too large to go in our heap */
	if (size > MAX_HEAP_OBJECT_SIZE)
		return(NULL);

	/* Search free list for a valid block */
	for(block=heap_list,last=NULL; block; last=block,block = block->next)
		if (size <= block->size)
			break;

	if (!block) {
		/* Expand heap if possible */
		if (TextureHeapExpand() != BRE_OK)
			return(NULL);			/* Can't expand, so give up */

		/* Can always fit into the first block on the list now */
		block = heap_list;
		last = NULL;
	}

	/* Can return entire block if it is exactly the right size, else need to split */
	if (block->size != size) {
		/* Split block to allocate */
		newblock = BrResAllocate(DRIVER_RESOURCE, sizeof(heap_memory_block), BR_MEMORY_SCRATCH);
		if (!newblock)
			return(NULL);
		newblock->heap		= block->heap;
		newblock->mem		= block->mem + size;
		newblock->size		= block->size - size;
		newblock->next		= block->next;
		block->size = size;

		block->next = newblock;
	}

	/* Unlink */
	if (last) last->next = block->next;
	else heap_list = block->next;
	block->next = NULL;

	return(block);
}


/*
 * Free heap memory
 */
static void TextureHeapFree(heap_memory_block *block)
{
	heap_memory_block *lastlast, *last, *next;
	MSImemptr heap;

	/* All checks must be in same heap */
	heap = block->heap;

	/* Relink into free list: warning, nightmare code! */
	for(next=heap_list,last=NULL,lastlast=NULL;
		next;
		lastlast=last,last=next,next = next->next) {
		/* Search until at least one of the two blocks is in the right heap */
		if ((last && (last->heap == heap)) || (next->heap == heap)) {

			/* Block must fit in between last and next in same heap: this is
			 * legal in DOS version as only comparing between memory on same
			 * heap, and therefore the same selector */
			if ((!last || (heap != last->heap) || (block->mem > last->mem)) &&
				((heap != next->heap) ||(block->mem < next->mem)) )
				break;
		}
	}

	/* Check for empty block afterwards: again, legal because same heap i.e. same selector
	 * is always checked before doing any pointer arithmetic */
	if (next) {
		if ((heap == next->heap) &&
				((block->mem + block->size) == next->mem)) {
			/* Concatenate */
			block->size += next->size;
			block->next = next->next;
			BrResFree(next);
		} else {
			/* Relink instead */
			block->next = next;
		}
	}

	/* Check for empty block before: again legal on same heap */
	if (last) {
		if ((heap == last->heap) && ((last->mem + last->size) == block->mem)) {

			/* Concatenate */
			last->size += block->size;
			last->next = block->next;
			BrResFree(block);
			block = last;		// Store for heap free check
			last = lastlast;	// Make last valid again
		} else {
			/* Relink instead */
			last->next = block;
		}
	} else {
		heap_list = block;
	}

#ifdef USE_HEAP_FREEING
	/* If this block is an entire texture heap, attempt to free it here, which
	 * is cleaner and neater and shuts up MSI's debug warnings. */
	if (block->size == MAX_HEAP_OBJECT_SIZE) {
		if (!msiFreeTextureHeap(block->heap))
			return;

		if (last)
			last->next = block->next;
		else
			heap_list = block->next;
	}
#endif

}





#ifdef _WIN32


#ifdef USE_TEXTURE_HEAP
#define HEAP_LIST_SIZE	4096			// Max of 4096 textures... should be OK
static br_buffer_stored *on_heap_list[HEAP_LIST_SIZE];
#endif



/*
 * Move a pixelmap's pixel data to the heap
 */
br_error MovePixelmapToHeap(br_buffer_stored *buffer)
{
#ifdef USE_TEXTURE_HEAP
	void *sysmemptr;
	heap_memory_block *heapptr;
	br_uint_32 size;
	int i;

	if (buffer->flags & SBUF_ON_HEAP)
		return(BRE_DEV_FAIL);

	sysmemptr = buffer->pixelmap->pixels;
	size = buffer->buffer.size;

	/* Allocate heap memory; if we can't, then the heap must be full */
	heapptr = TextureHeapMalloc(size);
	if (!heapptr)
		return(BRE_DEV_NO_VIDEO_MEMORY);

	/* Move it */
	memcpy(heapptr->mem, sysmemptr, size);

	/* Reprogram pixelmap structure */
	buffer->pixelmap->pixels = heapptr->mem;

	/* Clear out old pixel storage */
	BrResFree(sysmemptr);

	/* Mark it on the heap */
	buffer->flags |= SBUF_ON_HEAP;
	buffer->heap_block = heapptr;
	buffer->buffer.heap = heapptr->heap;
	buffer->buffer.base = heapptr->mem;

	/* Add to the list of blocks on the heap */
	for(i=0; i<HEAP_LIST_SIZE; i++) {
		if (on_heap_list[i] == NULL) {
			on_heap_list[i] = buffer;
			break;
		}
	}
#endif

	return(BRE_OK);
}


/*
 * Return a heap pixelmap pixel data to normal memory
 */
br_error MovePixelmapFromHeap(br_buffer_stored *buffer)
{
#ifdef USE_TEXTURE_HEAP
	void *sysmemptr;
	heap_memory_block *heapptr;
	br_uint_32 size;
	int i;

	if (!(buffer->flags & SBUF_ON_HEAP))
		return(BRE_DEV_FAIL);

	size = buffer->buffer.size;
	heapptr = buffer->heap_block;

	/* Reallocate system memory */
	sysmemptr = BrResAllocate(buffer->pixelmap, size, BR_MEMORY_PIXELS);
	if (!sysmemptr)
		return(BRE_DEV_NO_MEMORY);

	/* Move it */
	memcpy(sysmemptr, heapptr->mem, size);

	/* Reprogram pixelmap structure */
	buffer->pixelmap->pixels = sysmemptr;

	/* Clear out old pixel storage */
	TextureHeapFree(heapptr);

	/* Mark it on the heap */
	buffer->flags &= ~SBUF_ON_HEAP;
	buffer->heap_block = NULL;

	buffer->buffer.base = sysmemptr;

	buffer->buffer.heap = NULL;

	/* Remove from the list of blocks on the heap */
	for(i=0; i<HEAP_LIST_SIZE; i++) {
		if (on_heap_list[i] == buffer) {
			on_heap_list[i] = NULL;
			break;
		}
	}

#endif

	return(BRE_OK);
}


#ifdef USE_TEXTURE_HEAP
/*
 * Clear all the stored buffers out of the texture heap
 *
 * I know... this should be done automatically... but the timing gets a bit messy
 * on shutdown - the device pixelmap free doesn't free the stored buffers because
 * the stored buffers are children of the primitive library which is a child of the
 * device... <sigh> so instead it's hack time.
 */
void ClearHeap(void)
{
	int i;

	for(i=0; i<HEAP_LIST_SIZE; i++)
		if (on_heap_list[i])
			MovePixelmapFromHeap(on_heap_list[i]);
}
#endif



#else		// _WIN32



/*
 * DOS method to copy a pixelmap to the heap
 */
br_error CopyPixelmapToHeap(br_buffer_stored *buffer)
{
	void *sysmemptr;
	br_uint_32 size;

	sysmemptr = buffer->pixelmap->pixels;
	size = buffer->buffer.size;

	/* First time this one has been moved across: create memory */
	if (!buffer->buffer.heap) {
		heap_memory_block *heapptr;

		/* Allocate heap memory; if we can't, then the heap must be full */
		heapptr = TextureHeapMalloc(size);
		if (!heapptr)
			return(BRE_DEV_NO_VIDEO_MEMORY);

		buffer->heap_block = heapptr;
		buffer->buffer.heap = heapptr->heap;
		buffer->buffer.base = heapptr->mem;

		buffer->flags |= SBUF_ON_HEAP;

	}

	/* Copy it */
	_fmemcpy(buffer->buffer.base, sysmemptr, size);

	return(BRE_OK);
}




#endif		// _WIN32



/*
 * Check if a number of a power of 2
 */
static br_boolean isPowerOf2(br_int_32 x)
{
	return !((x-1) & x);
}

static pixelmapBPP(br_device_pixelmap *pm)
{
	switch(pm->pm_type) {
	default:
		return 1;
	case BR_PMT_RGB_555:
	case BR_PMT_RGB_565:
	case BR_PMT_DEPTH_16:
		return 2;

	case BR_PMT_RGB_888:
		return 3;

	case BR_PMT_RGBX_888:
	case BR_PMT_RGBA_8888:
		return 4;
	}
}



/*
 * Set up the stored buffer, inside which is the renderbuffer used to
 * render the texture.
 *
 * Under Windows, move the pixelmap to the heap if possible. Under DOS, copy the
 * pixelmap to the heap - if it isn't possible, it won't be rendered.
 */
br_error SetupMystBuffer(br_buffer_stored *buffer, br_device_pixelmap *pm)
{
	br_colour *map;
	heap_memory_block *palette_heap_block;
	br_uint_16 *op;
	br_uint_32 i;

	/* Free palette if already allocated internally */
	if (buffer->palette_heap_block) {
		TextureHeapFree(buffer->palette_heap_block);
		buffer->palette_heap_block = NULL;
	}
#ifdef _WIN32
	  else if (buffer->buffer.palette) {
		BrResFree(buffer->buffer.palette);
		buffer->buffer.palette = NULL;
	}
#endif

	/* Any failure inside here will make the texture uncacheable and unrenderable */
	buffer->buffer.uncacheable = BR_TRUE;

//return(BRE_DEV_UNSUPPORTED);

	/* Inspect the texture for legality */
	if(pm->pm_width < 8 || pm->pm_height < 8)
        return BRE_DEV_UNSUPPORTED;
	if(pm->pm_width > 1024 || pm->pm_height > 1024)
        return BRE_DEV_UNSUPPORTED;
	if(!isPowerOf2(pm->pm_width) || !isPowerOf2(pm->pm_height))
        return BRE_DEV_UNSUPPORTED;
	if ((pm->pm_base_x != 0) || (pm->pm_base_y != 0))
		return BRE_DEV_UNSUPPORTED;

	/* Copy across dimensions */
    buffer->buffer.width_p = pm->pm_width;
    buffer->buffer.width_b = pm->pm_width * pixelmapBPP(pm);
    buffer->buffer.height = pm->pm_height;

	/* BRender and the Mystique disagree slightly in terms of the definition of
	 * texture coordinates (I think). My interpretation of the problem is that the
	 * BRender specification is that 1.0 corresponds to the centre of the _last_
	 * column or row of pixels on the texture, whereas the Mystique interpretations is
	 * that 1.0 is the wrap-around, i.e. 1.0 and 0.0 are equivalent. Personally I
	 * think that the Mystique method is right. Therefore we just need to calculate
	 * the U and V ranges here, taking into account a correction factor to prevent
	 * partial textures seaming... */
#if 0
	/*
	 * No as usual Dave is mouthing off without checking with someone on the BRender team first.
	 * (0.0, 0.0) corresponds to the top-left of the top-left pixel, (1.0, 0.0) corresponds to the
	 * same place one tile to the right.
	 *
	 * Either the hardware rasterises badly or 0.0 is the CENTRE of the top-left pixel, in which case
	 * an offset of (-0.5,-0.5) is required.  But I tried that and it changes the problem, it didn't
	 * eliminate it.
	 */
	buffer->buffer.u_range = BrFloatToScalar(1.0f - (1.0f/(pm->pm_width-1.0f)));
	buffer->buffer.v_range = BrFloatToScalar(1.0f - (1.0f/(pm->pm_height-1.0f)));
#endif

	/* Pixelmap must be unstrided */
	if (pm->pm_row_bytes != buffer->buffer.width_b)
		return BRE_DEV_UNSUPPORTED;

	/* Set size, align to 32 bytes for maximum cache efficiency */
	buffer->buffer.size = (buffer->buffer.width_b * buffer->buffer.height + 0x1f) & ~0x1f;

	/*
	 * Set up the bits per pixel and any palette information:
	 */
    buffer->buffer.palette_heap = NULL;
    buffer->buffer.palette		= NULL;

	switch(pm->pm_type) {
	case BR_PMT_INDEX_8:
		buffer->buffer.bits = 8;

		/* Build palette: this is a 16-bit 256-entry palette */
        if (pm->pm_map)
			map = pm->pm_map->pixels;
		else
			map = ObjectDevice(buffer)->clut->entries;

		if (!map)
			return(BRE_DEV_UNSUPPORTED);

#ifdef USE_TEXTURE_HEAP
		if (palette_heap_block = TextureHeapMalloc(512)) {
			buffer->buffer.palette_heap = palette_heap_block->heap;
			buffer->buffer.palette		= palette_heap_block->mem;
			buffer->palette_heap_block	= palette_heap_block;
		} else
#endif

#ifdef _WIN32
		{	/* Else case if texture heap load failed */
			buffer->buffer.palette = BrResAllocate(DRIVER_RESOURCE, 512, BR_MEMORY_DRIVER);
			buffer->buffer.palette_heap = NULL;
			buffer->palette_heap_block = NULL;
		}
#else
			return(BRE_DEV_UNSUPPORTED);	// palettes must be on the heap in DOS version
#endif

		if (!buffer->buffer.palette)
			return(BRE_DEV_UNSUPPORTED);

		op = (br_uint_16 *) buffer->buffer.palette;
		for(i=0;i<256;i++) {
			*op++ = ((BR_RED(map[i]) & 0xf8) << 8) +
					((BR_GRN(map[i]) & 0xfc) << 3) +
					((BR_BLU(map[i]) & 0xf8) >> 3);
		}

		break;

	case BR_PMT_RGB_555:
		buffer->buffer.bits = 15;
		break;

	case BR_PMT_RGB_565:
		buffer->buffer.bits = 16;
		break;

	default:
		return(BRE_DEV_UNSUPPORTED);
	}

#ifdef _WIN32
	/* Having got this far, it's definitely possible to move the pixelmap across to heap memory.
	 * as long as: 1. it's not already there and 2. the application hasn't requested
	 * it not to move */
	if (!(buffer->flags & SBUF_ON_HEAP) && !(buffer->flags & SBUF_PREFER_SHARE))
		MovePixelmapToHeap(buffer);

#else
	/* Under DOS, copy it to the heap */
	CopyPixelmapToHeap(buffer);

	/* If it couldn't be copied to the heap, then it can't be rendered */
	if (!buffer->buffer.heap)
		return(BRE_DEV_NO_MEMORY);
#endif


	/* It's all OK, so we can mark it as usable now! */
    buffer->buffer.uncacheable = BR_FALSE;

    return BRE_OK;
}






/*
 * Set up stored buffer object to represent a texture
 */
struct br_buffer_stored *BufferStoredSoftAllocateMyst(struct br_primitive_library *plib,
	br_token use, struct br_device_pixelmap *pm, br_token_value *tv)
{
	struct br_buffer_stored * self;
	char *ident;
	struct sbufferAllocate_tokens sba;
	br_int_32 count;

	switch(use) {
	case BRT_TEXTURE_O:
	case BRT_COLOUR_MAP_O:
		ident = pm->pm_identifier;
		break;

	default:
		return NULL;
	}

	self = BrResAllocate(DRIVER_RESOURCE, sizeof(*self), BR_MEMORY_OBJECT);
	if(self == NULL)
		return NULL;

	/* Parse for buffer flags */
    sbufferAllocateTemplate.res = DRIVER_RESOURCE;
	sba.flags = SBUF_SHARED;
	BrTokenValueSetMany(&sba, &count, NULL, tv, &sbufferAllocateTemplate);
	self->flags = sba.flags;

	/* Set up defaults */
	self->dispatch			= &bufferStoredDispatch;
	if (ident)
		self->identifier	= BrResStrDup(self, ident);
	else
		self->identifier	= "Texture";
	self->plib				= plib;
	self->pixelmap			= (br_pixelmap *) pm;
    self->buffer.on_card	= BR_FALSE;
	self->heap_block		= NULL;
	self->palette_heap_block = NULL;

	/* Build the render-buffer */
	SetupMystBuffer(self, pm);

	ObjectContainerAddFront(plib, (br_object *)self);

	return self;
}


static br_error BR_CMETHOD_DECL(br_buffer_stored_mystique, update)(
	struct br_buffer_stored *self,
	struct br_device_pixelmap *pm,
	br_token_value *tv)
{
	/* Clear the entry out of the texture cache if it is already in there */
    TextureCacheClearEntry(&self->buffer);

	if (self->pixelmap != (br_pixelmap *) pm) {
		/* Hmmmm problems! */
		return(BRE_DEV_FAIL);
	}

	/* Rebuild the texture buffer */
	SetupMystBuffer(self, pm);

	return BRE_OK;
}


static void BR_CMETHOD_DECL(br_buffer_stored_mystique, free)(br_buffer_stored *self)
{
	/* Remove from card memory if uploaded */
    TextureCacheClearEntry(&self->buffer);

#ifdef _WIN32
	/* Return pixelmap to normal system memory */
	if (self->flags & SBUF_ON_HEAP)
		MovePixelmapFromHeap(self);

	/* Free palette if allocated */
	if (self->palette_heap_block)
		TextureHeapFree(self->palette_heap_block);
	else if (self->buffer.palette)
		BrResFree(self->buffer.palette);

#else
	/* DOS version has copy of pixelmap and palette on heap, so free them */
	if (self->heap_block)
		TextureHeapFree(self->heap_block);

	if (self->palette_heap_block)
		TextureHeapFree(self->palette_heap_block);
#endif

	/* Free object */
	ObjectContainerRemove(self->plib, (br_object *)self);
	BrResFreeNoCallback(self);
}

static br_token BR_CMETHOD_DECL(br_buffer_stored_mystique, type)(br_buffer_stored *self)
{
	return BRT_BUFFER_STORED;
}

static br_boolean BR_CMETHOD_DECL(br_buffer_stored_mystique, isType)(br_buffer_stored *self, br_token t)
{
	return (t == BRT_BUFFER_STORED) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_buffer_stored_mystique, space)(br_buffer_stored *self)
{
	return BrResSizeTotal(self);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_buffer_stored_mystique,templateQuery)
	(br_buffer_stored *self)
{
	bufferStoredTemplate.res = DRIVER_RESOURCE;
	return &bufferStoredTemplate;
}

/*
 * Default dispatch table for device
 */
static struct br_buffer_stored_dispatch bufferStoredDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
    BR_CMETHOD_REF(br_buffer_stored_mystique,   free),
    BR_CMETHOD_REF(br_object_mystique,          identifier),
    BR_CMETHOD_REF(br_buffer_stored_mystique,   type),
    BR_CMETHOD_REF(br_buffer_stored_mystique,   isType),
    BR_CMETHOD_REF(br_object_mystique,          device),
    BR_CMETHOD_REF(br_buffer_stored_mystique,   space),

    BR_CMETHOD_REF(br_buffer_stored_mystique,   templateQuery),
    BR_CMETHOD_REF(br_object,                   query),
    BR_CMETHOD_REF(br_object,                   queryBuffer),
    BR_CMETHOD_REF(br_object,                   queryMany),
    BR_CMETHOD_REF(br_object,                   queryManySize),
    BR_CMETHOD_REF(br_object,                   queryAll),
    BR_CMETHOD_REF(br_object,                   queryAllSize),

    BR_CMETHOD_REF(br_buffer_stored_mystique,   update),
};

