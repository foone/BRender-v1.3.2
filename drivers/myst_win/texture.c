/*
 * texture.c    : Device-independent texture caching routines
 *
 * This is an attempt to recode Andy's original texture caching routines
 * into something totally device-independent, capable of handling
 * multiple caching algorithms and generally doing everything much more
 * cleanly than the old system where everyone had to do their own
 * specific hacks into the texture system for their particular
 * 3D card.
 */

#include "texture.h"
#include "drv.h"
/*
 * Texture cache management
 *
 * Thesis/Idiot's Guide/Deranged Ramblings (delete as applicable)
 * by Dave Oldcorn of Volume 11 Software.
 *
 * Managing the texture cache is an important problem (in general on
 * a 2M card in 16-bit mode there will only be about 280k free for
 * texturing.
 *
 * There are two separate cases; the Z-sort and Z-buffered renderers.
 *
 * In the Z-buffered case, cache management is less important because
 * of sort-by-material. Here the cache manager will only help
 * significantly if the working store per frame is approximately the
 * same or somewhat smaller than the texture buffer. If the texture buffer
 * is significantly larger than the working store, the card memory will
 * absorb the lot and no swapping will occur. If the texture memory is much
 * smaller - say less than half the working store - then so much swapping
 * will be going on that the actual algorithm used isn't likely to matter
 * too much.
 *    An efficient caching algorithm will work by trying to maximise
 * the frame-to-frame coherency of the texture buffer. Textures should
 * be freed if they are not going to be used in a frame. In practice,
 * this is unlikely to be achievable, so they should be freed if they
 * haven't been used in the last frame.
 *    It is also an advantage to swap out textures that have already
 * been used for rendering rather than those that have already been used;
 * however, this should be considered carefully as it can ruin the
 * frame-frame coherency described above. I'm not sure if you actually
 * gain or lose anything by considering this point.
 *    A good algorithm for Z-buffered caching would go something like this:
 * Until memory fills, keep filling the card.
 * If there isn't any memory on the card, we need to find space on the card.
 * Look for the least-used texture present on the card and free it.
 *    That algorithm does assume that the textures are all the same size.
 * It would need more consideration for mixed texture sizes.
 *    It works on the principle that if a texture is heavily
 * used in the current frame, it will be heavily used in the next frame,
 * and if it is rarely used in the current frame it will be rarely used
 * in the next frame.
 *    Practical implementations will require a definition for 'how much
 * a texture is used'. The simplest parameter available for tracking is
 * how many polygons it is used on. This is OK, although it will
 * fall down on items such as backgrounds which will tend to be composed
 * of a few large polygons. A pixels-per-frame count would be much better
 * if practically implementable.
 *    If more information was available from the application about the
 * texture type, then a more efficient method could be defined. Possible
 * markings would mark always-required textures, rapidly animated textures
 * such as explosions (which may be large and changing every frame and
 * therefore might be retained in the cache between frames even when not
 * required) - I'm sure I could come up with several more if I thought
 * about it a bit.
 *    All in all, however, the actual algorithm used is a bit less than
 * critical as long as it doesn't leave any textures on the card that
 * won't be required for a while, because every texture that doesn't stay
 * cached has to go into the card at least once per frame - the amount of
 * memory that passes through the card is the same whatever the algorithm
 * is if there aren't any expired textures. The best performance can then
 * be acheived if you aim to swap only the overspill of textures from one
 * frame to the other.
 *    What I have done here is collected 'used' and 'not used' flags
 * and thrown away not used textures in preference. If there aren't any
 * not used textures, then throw away any texture that has already been
 * sent this frame - the exact one isn't really important. If nothing
 * reasonable can be found - only likely for either the first texture per
 * frame or a texture of radically different size to the rest - then
 * it simply discards as many textures as it has to starting at the first.
 *    The least pleasant cases for this algorithm probably involve mixing
 * lots of different texture sizes (unlikely) or using textures that change
 * every frame (likely).
 *
 *
 *
 *    This is not true for the Z-sort renderer. Without the advantages of
 * sort-by-material the textures can arrive in any order, and each texture
 * may be required several times during the course of a frame. This changes
 * the problem significantly. Although it is not quite this bad - because
 * the same materials will tend to be used on the same objects which will
 * have large numbers of polygons at roughly the same Z - there are plenty
 * of problems.
 *    For example, consider the case of a sphere where its halves are mapped
 * with a different texture and the sphere is being viewed so an equal amount
 * of both halves are present. If an algorithm the same as the Zb is used and
 * the buffer is full, the 'left' texture will be used once, then because
 * 1) it has only been used once for a small polygon and 2) it has already
 * been used this frame, it will be swapped out to make way for the 'right'
 * texture, and this process will repeat until the texture buffer melts.
 *    The process is therefore concerned far more with maintaining the
 * inside-one-frame cache coherency than the frame-frame coherency - which
 * although still useful as above is likely to be difficult - again as an
 * example, background textures will be used every frame, but 1) first
 * and 2) only once per frame. So is it a good idea to keep it or not?
 * Your guess is as good as mine.
 *    In general, writing a good Zs texture manager is likely to be tricky
 * and almost anything you can come up with will probably have some appalling
 * pathological cases, particularly if the number of textures cachable is
 * quite low (say < 10 textures of the typical size used by the application).
 *    The total polygon area will probably be the best marker here as well.
 *
 *
 * Due to the difficulties of a good Zs cache algorithm and the fact that
 * BRender and hardware is aimed mostly at Z-buffered applications, this
 * is designed to perform well only for the Z buffered case.
 *
 */






/* TEXTURE MEMORY MANAGER */

/* Based on code originally written by whoever wrote malloc
 * Rewritten by Andy to do it in card memory
 * Rewritten by just about everyone else for their own device drivers
 * Rewritten by Dave to genericise it and try to make it coherent again
 */


#if 0
//#include "log.h"
#define memlog0(s)          { syslog(s); }
#define memlog1(s, n)       { syslog(s, n); }
#define memlog2(s, n, n2)   { syslog(s, n, n2); }
#else
#define memlog0(s)
#define memlog1(s, n)
#define memlog2(s, n, n2)
#endif


static br_boolean texture_cache_initialised = BR_FALSE;



static texture_freed_cbfn *texture_callback = NULL;

/* This selects caching mode; this is not currently implemented */
#define CACHE_MODE_ZB   0
#define CACHE_MODE_ZS   1
static br_uint_32 cache_mode = CACHE_MODE_ZB;


/*
 * Block alignment
 */
#define ALIGN	8


/*
 * Pointer to the free and allocated lists of memory blocks
 * Both lists are maintained in card memory order
 *
 * This isn't an ideal solution, because it means that only the adjacent
 * allocated blocks are considered when freeing up multiple textures and
 * not the gaps between them, but it makes the card_mem_free function
 * simpler (i.e. possible to write with some hope that it might work)
 */
static memory_block *free_list = NULL;
static memory_block *alloc_list = NULL;


/*
 * Largest possible space; so for far-too-large textures won't
 * trash the texture cache on every call
 */
static br_uint_32 largest_mem_block = 0;


/*
 * Allocate 'size' bytes of memory from the free list.
 * Memory offsets calculated from texture_base
 *
 * Returns the memory block structure because free requires the pointer
 * to the memory block to organise freeing.
 */
static memory_block * card_mem_alloc(br_uint_32 size)
{
  br_uint_32 total_size;            // Size of block required in card memory
  br_int_32 offset;
  memory_block *fc, *fp, *new;

  /*
   * Find the first free block that's big enough
   */
  total_size = (size+(ALIGN-1))&(~(ALIGN-1));
  fp = NULL;
  fc = free_list;
  if (fc == NULL) {
    memlog0("card_mem_alloc : Memory manager not initialise");
    return(NULL);
  }
   
  memlog1("card_mem_alloc : Block allocation %ld",total_size);
  while(fc != NULL) {
    if (total_size <= fc->size) {
      offset = fc->card_mapping;
      break;
    }
    fp = fc;
    fc = fc->next;
  } 

  /*
   * Return if search unsuccessful
   */
  if (fc == NULL)
    return(NULL);

  memlog2("card_mem_alloc : Block Found, size %ld, offset %ld",fc->size, offset);

  /* fc is our block, offset is its base address in card memory */

  /*
   * Amend linked list of free blocks
   */
  if (total_size == fc->size) {
    /* Size of new and old block are the same (possibly quite likely in the
     * case of textures which are all likely to be the same size) so just
     * unlink the block and return it. */
    if (fp != NULL)
      fp->next = fc->next;
    else
      free_list = fc->next;

    new = fc;

  } else {
    /* Allocate a new memory block and split the old one */
    new = (memory_block *) BrResAllocate(DRIVER_RESOURCE,sizeof(memory_block),BR_MEMORY_SCRATCH);
    if (new == NULL) {
      memlog0("card_mem_alloc : BrResAllocate of new card alloc unit failed!");
      return(NULL);
    }
    fc->size -= total_size;
    fc->card_mapping += total_size;

    memlog1("card_mem_alloc : new block allocated, mapping = %ld", offset);

    new->size = total_size;
    new->card_mapping = offset;
  }

  /* Insert block on allocated list */
  if (!alloc_list) {
    alloc_list = new;
    new->next = NULL;
  } else {
    for(fp=NULL,fc=alloc_list; fc; fp=fc,fc=fc->next)
      if (fc->card_mapping >= new->card_mapping)
        break;
    if (fp == NULL) {
      new->next = alloc_list;
      alloc_list = new;
    } else {
      new->next = fp->next;
      fp->next = new;
    }
  }

  /*
   * Return pointer to memory block describing this memory
   */ 
  return(new);
}



/*
 * Return memory to the free list
 */
static void card_mem_free(memory_block *mem)
{
  memory_block *fc,*fp;

  /* Clear any texture reference in this texture */
  mem->texture = NULL;

  /*
   * Unlink from allocated list
   */
  if (alloc_list == mem)
    alloc_list = mem->next;
  else {
    for(fc=alloc_list; fc; fc=fc->next)
      if (fc->next == mem)
        break;
    if (fc) {   // Shouldn't fail this case but what the hell
      fc->next = mem->next;
    } else {
      memlog0("card_mem_free: Block not on alloc list in free");
    }
  }
  mem->next = NULL;

  /*
   * Catch case of empty free list , used when initialising..
   */
  if (free_list == NULL) {
    memlog0("card_mem_free: empty free list, initialising");
    free_list = mem;
    return;
  }

  /*
   * Find neighbouring free blocks
   */
  fp = NULL;
  fc = free_list;
  while (fc != NULL) {
    if (fc->card_mapping > mem->card_mapping)   // Works because ordered by card mapping
      break;
    fp = fc;
    fc = fc->next;
  }

  /*
   * Merge with following free block if possible
   */
  if (fc != NULL) {
    if ( (mem->card_mapping + mem->size) < fc->card_mapping) {
      /* Can't merge the blocks - another block has been allocated between
       * this one and the next, so insert it back into the list here. */
      mem->next = fc;
      memlog2("card_mem_free : Can't merge blocks mem = %ld fc = %ld",
                        mem->card_mapping, fc->card_mapping);
    } else {
      /* Next block is in the right place, so merge */
      memlog2("card_mem_free : Merging free blocks size %ld and %ld",mem->size,fc->size);
      memlog2("card_mem_free : At addresses %ld and %ld",mem->card_mapping,fc->card_mapping);
      mem->size += fc->size;
      mem->next = fc->next;
      BrResFree(fc);
    }
  } else
    mem->next = NULL;   // No next block to merge to

  /*
   * Merge with previous free block if possible
   */
  if (fp != NULL) {
    if ( (fp->card_mapping + fp->size) < mem->card_mapping) {
      /* Again, can't merge because there are missing blocks beforehand */
      fp->next = mem;
      memlog2("card_mem_free : Can't merge blocks fp = %ld mem = %ld",
                        fp->card_mapping,mem->card_mapping);
    } else {
      /* Previous block ends at this one; match 'em up */
      memlog2("card_mem_free : Merging free blocks size %ld and %ld",fp->size,mem->size);
      memlog2("card_mem_free : At addresses %ld and %ld",fp->card_mapping,mem->card_mapping);
      fp->size += mem->size;
      fp->next = mem->next;
      BrResFree(mem);
    }  
  } else {
    /* Is the first block */
    memlog0("card_mem_free : setting free_list = mem");
    free_list = mem;
  }
}



/*
 * Add an area of memory to the free list
 *
 * This is required to initialise the list. It only Has to be called once,
 * but Can be called as many times as you want.
 *
 * mem is the offset from the start of texture memory
 * size is the block size to allocate
 */
static br_boolean card_mem_add_block(br_uint_32 mem, br_uint_32 size)
{
  memory_block *new;

  memlog1("card_mem_add_block: adding block of %d bytes", size);

  new = (memory_block*) BrResAllocate(DRIVER_RESOURCE,sizeof(memory_block),BR_MEMORY_SCRATCH);

  if (new == NULL)
    return(BR_FALSE);

  if (size > largest_mem_block)
    largest_mem_block = size;

  new->size = size;
  new->card_mapping = mem;
  card_mem_free(new);

  return(BR_TRUE);
}




/*
 * Find out if this new texture is already on the card
 */
static memory_block *card_mem_find(struct render_buffer *texture)
{
  memory_block *mem;

  for(mem=alloc_list; mem; mem=mem->next) {
    if (mem->texture == texture)
      return(mem);
  }

  return(NULL);
}





/*
 * Free a texture - this is the wrapper that additionally calls the
 * texture freeing callback function to inform the higher-level driver
 * what's going on down here.
 */
static br_boolean TextureCacheFree(memory_block *mem)
{
  memlog0("TextureCacheFree:");

  /* Firstly tell the high-level driver */
  if (texture_callback) {
    memlog0("   calling callback");
    if (!(*texture_callback)(mem->texture))
      return(BR_FALSE);
  }

  /* Then free the card memory */
  memlog0("   freeing memory");
  card_mem_free(mem);

  memlog0("   complete");
  return(BR_TRUE);
}



/*
 * Free a contiguous block of memory of size on the card
 */
static br_boolean card_mem_free_space(br_uint_32 size)
{
  memory_block *mem, *ok, *notbad, *next;
  br_uint_32 size_freed;

  memlog0("card_mem_free_space:");
  if (cache_mode == CACHE_MODE_ZB) {
    memlog0("   Zb mode\n   looking for unused in last frame");
    ok = NULL;
    notbad = NULL;
    for(mem=alloc_list; mem; mem=mem->next) {
      /* Best cases are both not used in last frame */
      if (mem->last_frame == 0) {
        /* First check for best case; a texture not used in last frame that
         * is exactly the right size */
        if (mem->size == size)
          break;
        /* OK case is not used in last frame and bigger OR not used in last
         * frame and followed by one also not used in last frame and BOTH
         * bigger - in theory you could keep on checking down the list of
         * next blocks here but that sounds too much like hard work. */
        if (mem->size >= size)
          ok = mem;
        else if (mem->next && (mem->next->last_frame <= 0) &&
                 ((mem->size + mem->next->size) >= size))
          notbad = mem;
      }
    }

    if (!mem && !ok && !notbad) {
      /* OK we didn't find anything by that method; repeat the procedure
       * but this time looking for blocks we've already used this frame.
       * In almost every imaginable case this should find something. */
      memlog0("   looking for already-used this frame");
      for(mem=alloc_list; mem; mem=mem->next) {
        if (mem->last_frame < 0) {
          if (mem->size == size)
            break;
          if (mem->size >= size)
            ok = mem;
          else if (mem->next && (mem->next->last_frame <= 0) &&
                   ((mem->size + mem->next->size) >= size))
            notbad = mem;
        }
      }
      if (!mem && !ok && !notbad) {
        /* STILL not found anything; just free up starting at the first block */
        memlog0("   can't find anything; using first block");
        mem = alloc_list;
      }
    }
    if (!mem) {
      if (!ok) {
        if (notbad)
          memlog0("   third choice found");
        mem = notbad;
      } else {
        memlog0("   second choice found");
        mem = ok;
      }
    } else
      memlog0("   first choice found");

    /* OK here we have the valid pointer - or if not we have to give up */
    if (!mem) {
      memlog0("   nothing found");
      return(BR_FALSE);
    }

    /* Free blocks from here until we have enough or run out of blocks.
     * Even if we run out of blocks, return OK, in the hope that the
     * free blocks in between have supplied the extra memory */
    memlog0("   freeing blocks from choice");
    for(size_freed=0; mem && (size_freed < size); mem=next) {
      next = mem->next;
      size_freed += mem->size;
      TextureCacheFree(mem);
    }

  } else    // Only Zb case implemented; all renderers use it
    return(BR_FALSE);

  memlog0("   complete");
  return(BR_TRUE);
}




/*
 * Set up the texture cache. This should only be called once. To
 * call it more than once, call deinit first.
 *
 * In : base offset in card memory of texture buffer
 *      size of texture memory
 *      Callback function for cache freeing
 *
 * Note that it is not defined how the 'offset' is specified. It is
 * totally transparent inside these routines as to how it is used; it
 * could be the offset from the start of card memory, from the start of
 * the texture buffer in card memory, or anything else. The only
 * limitation is that it must never be negative beween base_mem_offset
 * and size (because TextureCacheRequest returns a negative number on
 * error).
 */
br_boolean TextureCacheInit(br_int_32 base_mem_offset, br_uint_32 size,
                    texture_freed_cbfn *tcallback)
{
  memlog0("TextureCacheInit:");

  if (texture_cache_initialised)
    return(BR_FALSE);
  texture_cache_initialised = BR_TRUE;

  largest_mem_block = 0;

  /* Set up the card memory manager */
  if (!card_mem_add_block(base_mem_offset, size)) {
    memlog0("   failed to add base memory");
    return(BR_FALSE);
  }

  texture_callback = tcallback;

  memlog0("   complete");
  return(BR_TRUE);
}


/*
 * Free up all the memory and so on
 *
 * If call_callback is set, will call the texture callback for each
 * one it frees, although the result will be ignored
 */
void TextureCacheDeinit(br_boolean call_callback)
{
  memory_block *mem, *next;

  /* Catch failure just in case */
  if (!texture_cache_initialised)
    return;

  memlog0("TextureCacheDeInit:");

  /* If requested, use the callback method when clearing out the cache */
  if (call_callback)
    for(mem=alloc_list; mem; mem=next) {
      next = mem->next;
      TextureCacheFree(mem);
    }

  /* Free the allocated list: if the above function has been called,
   * this simply frees up anything which didn't get freed due to the
   * callback function returning a dodgy value. Bit icky, but since
   * it's about to die anyway, who cares? (FLW) */
  for(mem=alloc_list; mem; mem=next) {
    next = mem->next;
    card_mem_free(mem);
  }

  /* Reset the list manager */
  BrResFree(free_list);
  free_list = NULL;

  memlog0("   complete");
}




/*
 * Called at the end of each frame; goes through the texture buffer
 * resetting all the statistics on the textures
 */
void TextureCacheFrameEnd(void)
{
  memory_block *mem;

  memlog0("TextureCacheFrameEnd:");
  for(mem=free_list; mem; mem=mem->next) {
    mem->last_frame = mem->this_frame;
    mem->this_frame = 0;
  }
  memlog0("   complete");
}




/*
 * Driver requires texture
 *
 * This is the function that must be called every time a particular
 * texture is required. This has a significant overhead because it has
 * to walk the whole free list (in the worst case), but is not
 * a major problem (certainly not for a Z-buffered case anyway).
 *
 * Any failure returned from this routine almost certainly means that
 * the texture will never be cacheable for the current rendering, and
 * further requests to use this texture should be ignored.
 *
 * This also collects the statistics used to manage the texture buffer.
 *
 * In:  BRender's description of the texture, size of texture
 * Out: Offset of texture in card memory, -1 if failed
 */
br_int_32 TextureCacheRequest(struct render_buffer *texture, br_uint_32 size)
{
  memory_block *texmem;

  memlog1("TextureCacheRequest: size %d", size);
  if (largest_mem_block < size) {
    memlog0("   too big for cache so ignoring");
    return(-1);
  }

  memlog0("   doing search");
  texmem = card_mem_find(texture);
  if (!texmem) {
    memlog0("   not found\n   trying to alloc");
    /* Not already on card, so find the memory for it */
    texmem = card_mem_alloc(size);
    if (!texmem) {
      memlog0("   alloc failed, trying to make room");
      /* Hmmm, not enough room, make some */
      if (!card_mem_free_space(size)) {
        memlog0("   failed to make room");
        return(-1);     // Can't
      }
      memlog0("   made room, trying to realloc now");
      texmem = card_mem_alloc(size);
      if (!texmem) {
        memlog0("   failed realloc");
        return(-1);     // Could but didn't?
      }
    }
    memlog0("   card mem alloc success");
    texmem->texture = texture;

    /* Set up default statistics; mark it used this frame, with a special
     * 'just loaded' flag for last frame so it is known that it was loaded
     * this time */
    texmem->this_frame = 1;
    texmem->last_frame = -1;
  } else {
    memlog0("   found");
    /* Increase the statistics */
    texmem->this_frame++;
  }

  memlog0("   complete");
  return(texmem->card_mapping);
}



/*
 * Free directly rather than through callback
 *
 * Provided to allow blocking memory leaks in alloc function
 */
void TextureCacheRequestFree(struct render_buffer *texture)
{
  memory_block *texmem;

  texmem = card_mem_find(texture);
  if (texmem)
    card_mem_free(texmem);
}


/*
 * Remove any occurrences of a particular texture from card memory
 */
void TextureCacheClearEntry(struct render_buffer *texture)
{
  memory_block *mem, *next;

  memlog0("Clearing texture entry");
  for(mem=alloc_list; mem; mem=next) {
    next = mem->next;
    if (mem->texture == texture)
      TextureCacheFree(mem);
  }

  memlog0("   complete");
}



