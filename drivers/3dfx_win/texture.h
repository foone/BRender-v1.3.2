/*
 * texture.h    : header for device-independent texture caching routines
 */

/* Sentinel this one just in case */
#ifndef _DAVES_TEXTURE_H_
#define _DAVES_TEXTURE_H_

#include <brender.h>


/* Definition for callback function called when the cache frees a texture
 * so the driver can free it as well */
typedef br_boolean (texture_freed_cbfn)(struct render_buffer *texture);


/*
 * Block structure for card memory malloc
 *
 * Blocks are maintained in order of card memory mapping, to speed up
 * and simplify freeing routines.
 */
typedef struct memory_block_tag memory_block;
struct memory_block_tag {
  br_int_32       card_mapping;   // Where this maps in card memory; an int so -1 can be returned
  br_uint_32      size;
  struct render_buffer *texture;  // BRender's definition of the texture
  br_int_16       this_frame;     // How many times accessed this frame
  br_int_16       last_frame;     // Same for last frame
  memory_block *  next;           // Next block on list
};


/* Start up texture cache */
br_boolean TextureCacheInit(texture_freed_cbfn *tcallback);

/* Add a memory block to the cache */
br_boolean TextureCacheAddBlock(br_int_32 base_mem_offset, br_uint_32 size);

/* Close it down */
void TextureCacheDeinit(br_boolean call_callback);
/* Perform per-frame processing - update internal statistics */
void TextureCacheFrameEnd(void);
/* Request a texture; returns offset of texture on card */
br_int_32 TextureCacheRequest(struct render_buffer *texture, br_uint_32 size);
/* Free a single just-allocated texture */
void TextureCacheRequestFree(struct render_buffer *texture);
/* Clear an entry out of the cache */
void TextureCacheClearEntry(struct render_buffer *texture);


#endif


