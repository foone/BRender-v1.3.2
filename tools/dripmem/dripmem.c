/*
 * Tracking version of malloc()
 *
 * Allocated blocks are maintained on a list, which includes
 * the callers return address
 */
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "dripmem.h"

struct mt_block {
	struct mt_block *next;
	struct mt_block **prev;
	size_t size;
	int id;
	int tag;
	char *from;
	char *type;
	int flags;
	int magic;
};

#define MT_MAGIC_START 0x43E77AED
#define MT_MAGIC_END   0x43E77AEF

#define MT_FLAG_ALLOCED 0x00000001
#define MT_FLAG_FREED   0x00000002

/*
 * Doubly linked list of all allocated blocks
 */
static struct mt_block *MtChain;
static int MtBlocks;
static int MtTotal;

/*
 * A user settable ID number that is recorded
 * in each block
 */
static int MtCurrentID = 0;
static int MtCurrentTag = 0;
static int MtLogging = 0;
static int MtFreeing = 1;

/*
 * Drip checkpoint count and total allocation at last checkpoint
 */
static int MtCheckpointBlocks;
static int MtCheckpointTotal;

/*
 * Allocate a new block of memory
 */
void *cdecl _drip_alloc(size_t size, char *from, char *type)
{
	struct mt_block *mtb;

	/*
	 * Write message to log
	 */
	if(MtLogging)
		fprintf(stderr,"\tMALLOC(%d,%d);\n",MtCurrentID, size);

	/*
	 * Use underlying allocator to get block
	 * including enough space for header and
	 * trailing magic number and call trace
	 */
	mtb = malloc(size + sizeof(*mtb)+sizeof(int));
	if (mtb == NULL)
		return NULL;

	/*
	 * Fill in header and link into current chain
	 */
	mtb->next = MtChain;
	MtChain = mtb;
	mtb->prev = &MtChain;
	if(mtb->next)
		mtb->next->prev = & mtb->next;
	
	mtb->id = MtCurrentID++;
	mtb->tag = MtCurrentTag;
	mtb->from = from;
	mtb->type = type;
	mtb->size = size;
	mtb->flags = MT_FLAG_ALLOCED;

	/*
	 * Fill in magic numbers
	 */
	mtb->magic = MT_MAGIC_START;

	*((int *)((char *)(mtb+1)+size)) = MT_MAGIC_END;

	/*
	 * Keep track of totals
	 */
	MtTotal += mtb->size;
	MtBlocks ++;

	/*
	 * Give the space following the header back to user
	 */
	return (void *)(mtb+1);
}

/*
 * Release a block of mmeory allocated with _drip_alloc()
 */
void cdecl _drip_free(void *ptr)
{
	struct mt_block *mtb;

	/*
	 * Get pointer to header
	 */
	mtb = (struct mt_block *)ptr - 1;

	/*
	 * Check magic numbers
	 */
	if(mtb->magic != MT_MAGIC_START) {
		fprintf(stderr,"_drip_free: Bad MAGIC_START\n");
		return;
	}

	if(*((int *)((char *)(mtb+1)+mtb->size)) != MT_MAGIC_END) {
		fprintf(stderr,"_drip_free: Bad MAGIC_END\n");
		return;
	}

	/*
	 * Write message to log
	 */
	if(MtLogging)
		fprintf(stderr,"\tFREE(%d);\n",mtb->id);

	/*
	 * Keep track of totals
	 */
	MtTotal -= mtb->size;
	MtBlocks --;

	mtb->flags |= MT_FLAG_FREED;

	if(MtFreeing) {
		/*
		 * Unlink block from list
	 	 */
		if(mtb->next)
			mtb->next->prev = mtb->prev;
		*mtb->prev = mtb->next;

		/*
		 * Release memory
		 */
		free(mtb);
	}
}

/*
 * Set the tag value to be used for future allocations
 */
void cdecl _drip_set_tag(int tag)
{
	MtCurrentTag = tag;
}

/*
 * Set the log flag
 */
void cdecl _drip_set_log(int flag)
{
	MtLogging = flag;
}

/*
 * Set the free flag
 */
void cdecl _drip_set_free(int flag)
{
	MtFreeing = flag;
}

/*
 * Dump the current memeory chain to a file
 * (or stderr if filename == NULL)
 */
void cdecl _drip_checkpoint(char *filename, char *header, int dump_blocks)
{
	struct mt_block *mtb;
	FILE *ofh;
	int n;

	if(filename) {
		ofh = fopen(filename,"at");
	} else {
		ofh = stderr;
	}

	/*
	 * Print a header
	 */
	fprintf(ofh,"-- %s   %d Blocks (%+d)   %d Bytes (%+d)\n",header,MtBlocks,MtBlocks-MtCheckpointBlocks,MtTotal,MtTotal-MtCheckpointTotal);

	MtCheckpointBlocks = MtBlocks;
	MtCheckpointTotal = MtTotal;

	/*
	 * Print a line for each block allocated or freed since the last checkpoint
	 * and properly free the freed blocks
	 */
	for(mtb = MtChain; mtb; mtb = mtb->next) {

		if (mtb->flags & MT_FLAG_ALLOCED) {

			/*
			 * Basic information about block
			 */
			if (dump_blocks)
				n = fprintf(ofh,
"ALLOCATED %10d (%d %s %s): %08x %d\n", mtb->id, mtb->tag,  mtb->from, mtb->type,
				mtb+1, mtb->size);

			mtb->flags &= ~MT_FLAG_ALLOCED;
		}

		if (mtb->flags & MT_FLAG_FREED) {

			/*
			 * Basic information about block
			 */
			if (dump_blocks)
				n = fprintf(ofh,
"FREED     %10d (%d %s %s): %08x %d\n", mtb->id, mtb->tag,  mtb->from, mtb->type,
				mtb+1, mtb->size);

			/*
		 	 * Unlink block from list
	 		 */
			if(mtb->next)
				mtb->next->prev = mtb->prev;
			*mtb->prev = mtb->next;

			/*
			 * Release memory
		 	 */
			free(mtb);
		}
	}

	if(filename)
		fclose(ofh);
}

/*
 * Dump the current memeory chain to a file
 * (or stderr if filename == NULL)
 */
void cdecl _drip_dump(char *filename, char *header)
{
	struct mt_block *mtb;
	FILE *ofh;
	int n;

	if(filename) {
		ofh = fopen(filename,"at");
	} else {
		ofh = stderr;
	}

	/*
	 * Print a header
	 */
	fprintf(ofh,"-- %s   %d Blocks   %d Bytes\n",header,MtBlocks,MtTotal);

	/*
	 * Print a line for each block
	 */
	for(mtb = MtChain; mtb; mtb = mtb->next) {
		/*
		 * Basic information about block
		 */
		n = fprintf(ofh,
"%10d (%d %s %s): %08x %d\n", mtb->id, mtb->tag,  mtb->from, mtb->type,
		mtb+1, mtb->size);
	}

	if(filename)
		fclose(ofh);
}
