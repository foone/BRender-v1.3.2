/*
 * Tracking version of malloc()
 */
#ifdef __cplusplus
extern "C" {
#endif

void *cdecl _drip_alloc(size_t size, char *from, char *type);
void cdecl _drip_free(void *ptr);
void cdecl _drip_set_tag(int tag);
void cdecl _drip_set_log(int flag);
void cdecl _drip_set_free(int flag);
void cdecl _drip_checkpoint(char *filename, char *header, int dump_blocks);
void cdecl _drip_dump(char *filename, char *header);

#ifdef __cplusplus
};
#endif

