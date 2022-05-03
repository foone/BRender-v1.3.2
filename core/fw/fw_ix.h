
/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Extra glue for C<->ASM generated via gemasmpr.pl
 */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __IBMC__
#pragma map(_reciprocal,"__reciprocal")
#pragma map(fw,"_fw")
#pragma map(BrFixedAddCarry,"_BrFixedAddCarry")
#pragma map(_FontFixed3x5,"__FontFixed3x5")
#pragma map(_FontProp4x6,"__FontProp4x6")
#pragma map(_FontProp7x9,"__FontProp7x9")
#pragma map(_MemCopyBits_A,"__MemCopyBits_A")
#pragma map(_MemFill_A,"__MemFill_A")
#pragma map(_MemRectFill_A,"__MemRectFill_A")
#pragma map(_MemCopy_A,"__MemCopy_A")
#pragma map(_MemRectCopy_A,"__MemRectCopy_A")
#pragma map(_MemPlot,"__MemPlot")
#pragma map(_GetSysQual,"__GetSysQual")
#endif /* __IBMC__ */

#ifdef __HIGHC__
#pragma Alias(_reciprocal,"__reciprocal")
#pragma Alias(fw,"_fw")
#pragma Alias(BrFixedAddCarry,"_BrFixedAddCarry")
#pragma Alias(_FontFixed3x5,"__FontFixed3x5")
#pragma Alias(_FontProp4x6,"__FontProp4x6")
#pragma Alias(_FontProp7x9,"__FontProp7x9")
#pragma Alias(_MemCopyBits_A,"__MemCopyBits_A")
#pragma Alias(_MemFill_A,"__MemFill_A")
#pragma Alias(_MemRectFill_A,"__MemRectFill_A")
#pragma Alias(_MemCopy_A,"__MemCopy_A")
#pragma Alias(_MemRectCopy_A,"__MemRectCopy_A")
#pragma Alias(_MemPlot,"__MemPlot")
#pragma Alias(_GetSysQual,"__GetSysQual")
#endif /* __IBMC__ */

#ifdef __cplusplus
	};
#endif
