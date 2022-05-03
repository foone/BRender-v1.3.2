
/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Extra glue for C<->ASM generated via gemasmpr.pl
 */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __IBMC__
#pragma map(DOSClockBegin,"_DOSClockBegin")
#pragma map(DOSClockEnd,"_DOSClockEnd")
#pragma map(DOSClockRead,"_DOSClockRead")
#pragma map(DOSDivTrapBegin,"_DOSDivTrapBegin")
#pragma map(DOSDivTrapEnd,"_DOSDivTrapEnd")
#pragma map(DOSDivTrapCount,"_DOSDivTrapCount")
#endif /* __IBMC__ */

#ifdef __HIGHC__
#pragma Alias(DOSClockBegin,"_DOSClockBegin")
#pragma Alias(DOSClockEnd,"_DOSClockEnd")
#pragma Alias(DOSClockRead,"_DOSClockRead")
#pragma Alias(DOSDivTrapBegin,"_DOSDivTrapBegin")
#pragma Alias(DOSDivTrapEnd,"_DOSDivTrapEnd")
#pragma Alias(DOSDivTrapCount,"_DOSDivTrapCount")
#endif /* __IBMC__ */

#ifdef __cplusplus
	};
#endif
