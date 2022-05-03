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

#include "trackmem.h"
#include "calltrak.h"

struct mt_block {
	struct mt_block *next;
	struct mt_block **prev;
	void *caller;
	size_t size;
	int id;
	int tag;
	char *from;
	char *type;
	void **callstack;
	int calldepth;
	int magic;
};

#define MT_MAGIC_START 0x43E77AED
#define MT_MAGIC_END   0x43E77AEF

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
 * Allocate a new block of memory
 */
void *_track_alloc(size_t size, void *caller, char *from, char *type)
{
	void **ctp;
	struct mt_block *mtb;
	int i;

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
	mtb = _nmalloc(size + sizeof(*mtb)+sizeof(int)+_FunctionDepth * sizeof(void *));

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
	mtb->caller = caller;

	/*
	 * Fill in magic numbers
	 */
	mtb->magic = MT_MAGIC_START;

	*((int *)((char *)(mtb+1)+size)) = MT_MAGIC_END;

	/*
	 * Copy call trace to end of buffer
	 */
	mtb->calldepth = _FunctionDepth;

	ctp = (void **) ((char *)(mtb+1)+size+sizeof(int));

	for(i=0; i < _FunctionDepth; i++)
		ctp[i] = _FunctionStack[i];

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
 * Release a block of mmeory allocated with _track_alloc()
 */
void _track_free( void *ptr , void *caller)
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
		fprintf(stderr,"_track_free: Bad MAGIC_START\n");
		return;
	}

	if(*((int *)((char *)(mtb+1)+mtb->size)) != MT_MAGIC_END) {
		fprintf(stderr,"_track_free: Bad MAGIC_END\n");
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

	/*
	 * Unlink block from list
	 */
	if(mtb->next)
		mtb->next->prev = mtb->prev;
	*mtb->prev = mtb->next;

	if(MtFreeing) {
		/*
		 * Release memory
		 */
		_nfree(mtb);
	}

	(void *)caller;
}

/*
 * Set the tag value to be used for future allocations
 */
void _track_set_tag( int tag)
{
	MtCurrentTag = tag;
}

/*
 * Set the log flag
 */
void _track_set_log( int flag)
{
	MtLogging = flag;
}

/*
 * Set the free flag
 */
void _track_set_free( int flag)
{
	MtFreeing = flag;
}

/*
 * Dump the current memeory chain to a file
 * (or stderr if filename == NULL)
 */
static char scratch_string[256];

void _track_dump(char *filename, char *header)
{
	void **ctp;
	struct mt_block *mtb;
	FILE *ofh;
	int n;

	if(filename) {
		ofh = fopen(filename,"wt");
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
"%10d (%d %s %s): %08x %d", mtb->id, mtb->tag,  mtb->from, mtb->type,
		mtb+1, mtb->size);

		/*
		 * Pad to fixed column
		 */
		while(n++ < 65)
			putc(' ',ofh);

		/*
		 * Call trace when block allocated
		 */
		ctp = (void **) ((char *)(mtb+1)+mtb->size+sizeof(int));
		CallTrace(ctp,mtb->calldepth,scratch_string,sizeof(scratch_string));

		fputs(scratch_string,ofh);
		fputs("\n",ofh);
	}

	if(filename)
		fclose(ofh);
}

/*
 * Wrappers around the STDLIB functions
 * that invoke the tracking system
 */
void *malloc(size_t size)
{
	return _track_alloc(size,caller(6),"malloc","-");
}

void *calloc( size_t n, size_t size )
{
	void *m;

	size *= n;

	m = _track_alloc(size,caller(5),"calloc","-");

	memset(m,0,size);

	return m;
}

void free( void *ptr )
{
	_track_free(ptr,caller(6));
}

