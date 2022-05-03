/*
 * Keeps a call trace via the __PRO and _EPI hooks
 */
extern void *_FunctionStack[];
extern int _FunctionDepth;

int CallTrace(void *stack[], int depth, char *buffer, int limit);
