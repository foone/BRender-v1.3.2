/*
 * Table of exported names
 */
static char * namePointers_BRHOST1[] = {
	"_HostExceptionHook",
	"_HostExceptionUnhook",
	"_HostFarBlockFill",
	"_HostFarBlockRead",
	"_HostFarBlockWrite",
	"_HostFarByteRead",
	"_HostFarByteWrite",
	"_HostFarDWordRead",
	"_HostFarDWordWrite",
	"_HostFarStringRead",
	"_HostFarStringWrite",
	"_HostFarWordRead",
	"_HostFarWordWrite",
	"_HostInfo",
	"_HostInterruptCall",
	"_HostInterruptHook",
	"_HostInterruptUnhook",
	"_HostLock",
	"_HostRealAllocate",
	"_HostRealBlockFill",
	"_HostRealBlockRead",
	"_HostRealBlockWrite",
	"_HostRealByteRead",
	"_HostRealByteWrite",
	"_HostRealDWordRead",
	"_HostRealDWordWrite",
	"_HostRealFree",
	"_HostRealInterruptCall",
	"_HostRealInterruptGet",
	"_HostRealInterruptSet",
	"_HostRealStringRead",
	"_HostRealStringWrite",
	"_HostRealWordRead",
	"_HostRealWordWrite",
	"_HostRegistersGet",
	"_HostSelectorAllocate",
	"_HostSelectorAllocateAlias",
	"_HostSelectorAllocateLinear",
	"_HostSelectorAllocatePhysical",
	"_HostSelectorBaseQuery",
	"_HostSelectorBaseSet",
	"_HostSelectorCS",
	"_HostSelectorDS",
	"_HostSelectorES",
	"_HostSelectorFree",
	"_HostSelectorLimitQuery",
	"_HostSelectorLimitSet",
	"_HostSelectorReal",
	"_HostSelectorSS",
	"_HostUnlock",
};

/*
 * Table of ordinals
 */
static br_uint_16 nameOrdinals_BRHOST1[] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
	13,
	14,
	15,
	16,
	17,
	18,
	19,
	20,
	21,
	22,
	23,
	24,
	25,
	26,
	27,
	28,
	29,
	30,
	31,
	32,
	33,
	34,
	35,
	36,
	37,
	38,
	39,
	40,
	41,
	42,
	43,
	44,
	45,
	46,
	47,
	48,
	49,
};

/*
 * Table of ordinals
 */
static void * functionPointers_BRHOST1[] = {
	HostExceptionHook                                               , /* 0 */
	HostExceptionUnhook                                             , /* 1 */
	HostFarBlockFill                                                , /* 2 */
	HostFarBlockRead                                                , /* 3 */
	HostFarBlockWrite                                               , /* 4 */
	HostFarByteRead                                                 , /* 5 */
	HostFarByteWrite                                                , /* 6 */
	HostFarDWordRead                                                , /* 7 */
	HostFarDWordWrite                                               , /* 8 */
	HostFarStringRead                                               , /* 9 */
	HostFarStringWrite                                              , /* 10 */
	HostFarWordRead                                                 , /* 11 */
	HostFarWordWrite                                                , /* 12 */
	HostInfo                                                        , /* 13 */
	HostInterruptCall                                               , /* 14 */
	HostInterruptHook                                               , /* 15 */
	HostInterruptUnhook                                             , /* 16 */
	HostLock                                                        , /* 17 */
	HostRealAllocate                                                , /* 18 */
	HostRealBlockFill                                               , /* 19 */
	HostRealBlockRead                                               , /* 20 */
	HostRealBlockWrite                                              , /* 21 */
	HostRealByteRead                                                , /* 22 */
	HostRealByteWrite                                               , /* 23 */
	HostRealDWordRead                                               , /* 24 */
	HostRealDWordWrite                                              , /* 25 */
	HostRealFree                                                    , /* 26 */
	HostRealInterruptCall                                           , /* 27 */
	HostRealInterruptGet                                            , /* 28 */
	HostRealInterruptSet                                            , /* 29 */
	HostRealStringRead                                              , /* 30 */
	HostRealStringWrite                                             , /* 31 */
	HostRealWordRead                                                , /* 32 */
	HostRealWordWrite                                               , /* 33 */
	HostRegistersGet                                                , /* 34 */
	HostSelectorAllocate                                            , /* 35 */
	HostSelectorAllocateAlias                                       , /* 36 */
	HostSelectorAllocateLinear                                      , /* 37 */
	HostSelectorAllocatePhysical                                    , /* 38 */
	HostSelectorBaseQuery                                           , /* 39 */
	HostSelectorBaseSet                                             , /* 40 */
	HostSelectorCS                                                  , /* 41 */
	HostSelectorDS                                                  , /* 42 */
	HostSelectorES                                                  , /* 43 */
	HostSelectorFree                                                , /* 44 */
	HostSelectorLimitQuery                                          , /* 45 */
	HostSelectorLimitSet                                            , /* 46 */
	HostSelectorReal                                                , /* 47 */
	HostSelectorSS                                                  , /* 48 */
	HostUnlock                                                      , /* 49 */
};

static br_image Image_BRHOST1 = {
	{0},
	"BRHOST1",						/* Identifier				*/
	BR_IMG_RESIDENT,				/* Image type				*/
	0,								/* Reference count			*/

	1,									/* Ordinal base				*/
	BR_ASIZE(functionPointers_BRHOST1),	/* Number of functions		*/
	functionPointers_BRHOST1,				/* Table of functions		*/

	BR_ASIZE(namePointers_BRHOST1),	/* Number of names			*/
	namePointers_BRHOST1,				/* Table of names 			*/
	nameOrdinals_BRHOST1,				/* Table of name ordinals	*/
	
	0,								/* Number of imports		*/
	NULL,							/* Table of imports			*/

	0,								/* Number of sections		*/
	NULL,							/* Table of sections		*/

	NULL,							/* Type pointer				*/
};

