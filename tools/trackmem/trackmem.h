/*
 * Tracking version of malloc()
 */
void *_track_alloc(size_t size, void *caller, char *from, char *type);
void _track_free( void *ptr , void *caller);
void _track_set_tag( int tag);
void _track_set_log( int flag);
void _track_set_free( int flag);
void _track_dump(char *filename, char *header);

/*
 * Inline ASM to get return address of to caller
 */
void *caller(int offset);
#pragma aux caller =\
	"mov	eax,[ebp+eax*4]"\
	parm [ eax ] ;


