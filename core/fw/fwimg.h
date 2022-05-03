/*
 * Table of exported names
 */
static char * namePointers_BRCORE1[] = {
	"_BrAbort",
	"_BrAddHead",
	"_BrAddTail",
	"_BrDevAdd",
	"_BrDevAddStatic",
	"_BrDevBegin",
	"_BrDevBeginVar",
	"_BrDevCheckAdd",
	"_BrDevContainedCount",
	"_BrDevContainedFind",
	"_BrDevContainedFindMany",
	"_BrDevCount",
	"_BrDevFind",
	"_BrDevFindMany",
	"_BrDevLastBeginQuery",
	"_BrDevLastBeginSet",
	"_BrDevRemove",
	"_BrFailure",
	"_BrFatal",
	"_BrFileAdvance",
	"_BrFileAttributes",
	"_BrFileClose",
	"_BrFileEof",
	"_BrFileGetChar",
	"_BrFileGetLine",
	"_BrFileOpenRead",
	"_BrFileOpenWrite",
	"_BrFilePrintf",
	"_BrFilePutChar",
	"_BrFilePutLine",
	"_BrFileRead",
	"_BrFileWrite",
	"_BrFloatAtan2",
	"_BrFloatCeil",
	"_BrFloatFloor",
	"_BrFloatPow",
	"_BrFloatSqrt",
	"_BrGetEnv",
	"_BrImageAdd",
	"_BrImageDereference",
	"_BrImageLookupName",
	"_BrImageLookupOrdinal",
	"_BrImageReference",
	"_BrImageRemove",
	"_BrInsert",
	"_BrIsAlpha",
	"_BrIsDigit",
	"_BrIsPrint",
	"_BrIsSpace",
	"_BrLastErrorGet",
	"_BrLastErrorSet",
	"_BrLexerAllocate",
	"_BrLexerCommentSet",
	"_BrLexerDumpSet",
	"_BrLexerErrorSet",
	"_BrLexerFree",
	"_BrLexerPop",
	"_BrLexerPosition",
	"_BrLexerPushFile",
	"_BrLexerPushString",
	"_BrLexerTokenError",
	"_BrLogPrintf",
	"_BrMemAllocate",
	"_BrMemCalloc",
	"_BrMemCmp",
	"_BrMemCpy",
	"_BrMemFree",
	"_BrMemInquire",
	"_BrMemSet",
	"_BrMemStrDup",
	"_BrNamePatternMatch",
	"_BrNewList",
	"_BrObjectContainerFree",
	"_BrObjectListAllocate",
	"_BrParseFixed",
	"_BrParseFloat",
	"_BrParseInteger",
	"_BrParseMatrixFixed",
	"_BrParseMatrixFloat",
	"_BrParseVectorFixed",
	"_BrParseVectorFloat",
	"_BrPrimitiveLibraryFind",
	"_BrQsort",
	"_BrRegistryAdd",
	"_BrRegistryAddMany",
	"_BrRegistryAddStatic",
	"_BrRegistryClear",
	"_BrRegistryCount",
	"_BrRegistryEnum",
	"_BrRegistryFind",
	"_BrRegistryFindMany",
	"_BrRegistryNew",
	"_BrRegistryNewStatic",
	"_BrRegistryRemove",
	"_BrRegistryRemoveMany",
	"_BrRegistryRemoveStatic",
	"_BrRemHead",
	"_BrRemTail",
	"_BrRemove",
	"_BrRendererFacilityFind",
	"_BrResAdd",
	"_BrResAllocate",
	"_BrResCheck",
	"_BrResChildEnum",
	"_BrResClass",
	"_BrResDump",
	"_BrResFree",
	"_BrResFreeNoCallback",
	"_BrResIsChild",
	"_BrResRemove",
	"_BrResSize",
	"_BrResSizeTotal",
	"_BrResStrDup",
	"_BrSScanf",
	"_BrScratchAllocate",
	"_BrScratchFlush",
	"_BrScratchFree",
	"_BrScratchInquire",
	"_BrScratchString",
	"_BrScratchStringSize",
	"_BrSimpleAddHead",
	"_BrSimpleInsert",
	"_BrSimpleNewList",
	"_BrSimpleRemHead",
	"_BrSimpleRemove",
	"_BrSprintf",
	"_BrSprintfN",
	"_BrStrCat",
	"_BrStrCmp",
	"_BrStrCpy",
	"_BrStrICmp",
	"_BrStrLen",
	"_BrStrNCmp",
	"_BrStrNCpy",
	"_BrStrNICmp",
	"_BrStrRChr",
	"_BrStrToD",
	"_BrStrToF",
	"_BrStrToL",
	"_BrStrToUL",
	"_BrStringToTokenValue",
	"_BrSwap16",
	"_BrSwap32",
	"_BrSwapBlock",
	"_BrSwapFloat",
	"_BrTVTemplateAllocate",
	"_BrTVTemplateFree",
	"_BrTokenCount",
	"_BrTokenCreate",
	"_BrTokenFind",
	"_BrTokenFindMany",
	"_BrTokenIdentifier",
	"_BrTokenType",
	"_BrTokenValueCompare",
	"_BrTokenValueComparePartial",
	"_BrTokenValueDump",
	"_BrTokenValueQuery",
	"_BrTokenValueQueryAll",
	"_BrTokenValueQueryAllSize",
	"_BrTokenValueQueryMany",
	"_BrTokenValueQueryManySize",
	"_BrTokenValueSet",
	"_BrTokenValueSetMany",
	"_BrVSScanf",
	"_BrVSprintf",
	"_BrVSprintfN",
	"_BrWarning",
	"__BrAssert",
	"__BrExceptionBegin",
	"__BrExceptionEnd",
	"__BrExceptionResource",
	"__BrExceptionThrow",
	"__BrExceptionValueFetch",
	"__BrUAssert",
	"__EPI",
	"__M_br_object_container_addFront",
	"__M_br_object_container_count",
	"__M_br_object_container_find",
	"__M_br_object_container_findMany",
	"__M_br_object_container_remove",
	"__M_br_object_container_removeFront",
	"__M_br_object_container_tokensMatch",
	"__M_br_object_container_tokensMatchBegin",
	"__M_br_object_container_tokensMatchEnd",
	"__M_br_object_query",
	"__M_br_object_queryAll",
	"__M_br_object_queryAllSize",
	"__M_br_object_queryBuffer",
	"__M_br_object_queryMany",
	"__M_br_object_queryManySize",
	"__PRO",
};

/*
 * Table of ordinals
 */
static br_uint_16 nameOrdinals_BRCORE1[] = {
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
	50,
	51,
	52,
	53,
	54,
	55,
	56,
	57,
	58,
	59,
	60,
	61,
	62,
	63,
	64,
	65,
	66,
	67,
	68,
	69,
	70,
	71,
	72,
	73,
	74,
	75,
	76,
	77,
	78,
	79,
	80,
	81,
	82,
	83,
	84,
	85,
	86,
	87,
	88,
	89,
	90,
	91,
	92,
	93,
	94,
	95,
	96,
	97,
	98,
	99,
	100,
	101,
	102,
	103,
	104,
	105,
	106,
	107,
	108,
	109,
	110,
	111,
	112,
	113,
	114,
	115,
	116,
	117,
	118,
	119,
	120,
	121,
	122,
	123,
	124,
	125,
	126,
	127,
	128,
	129,
	130,
	131,
	132,
	133,
	134,
	135,
	136,
	137,
	138,
	139,
	140,
	141,
	142,
	143,
	144,
	145,
	146,
	147,
	148,
	149,
	150,
	151,
	152,
	153,
	154,
	155,
	156,
	157,
	158,
	159,
	160,
	161,
	162,
	163,
	164,
	165,
	166,
	167,
	168,
	169,
	170,
	171,
	172,
	173,
	174,
	175,
	176,
	177,
	178,
	179,
	180,
	181,
	182,
	183,
	184,
	185,
	186,
	187,
	188,
	189,
	190,
};

/*
 * Table of ordinals
 */
static void * functionPointers_BRCORE1[] = {
	BrAbort                                                         , /* 0 */
	BrAddHead                                                       , /* 1 */
	BrAddTail                                                       , /* 2 */
	BrDevAdd                                                        , /* 3 */
	BrDevAddStatic                                                  , /* 4 */
	BrDevBegin                                                      , /* 5 */
	BrDevBeginVar                                                   , /* 6 */
	BrDevCheckAdd                                                   , /* 7 */
	BrDevContainedCount                                             , /* 8 */
	BrDevContainedFind                                              , /* 9 */
	BrDevContainedFindMany                                          , /* 10 */
	BrDevCount                                                      , /* 11 */
	BrDevFind                                                       , /* 12 */
	BrDevFindMany                                                   , /* 13 */
	BrDevLastBeginQuery                                             , /* 14 */
	BrDevLastBeginSet                                               , /* 15 */
	BrDevRemove                                                     , /* 16 */
	BrFailure                                                       , /* 17 */
	BrFatal                                                         , /* 18 */
	BrFileAdvance                                                   , /* 19 */
	BrFileAttributes                                                , /* 20 */
	BrFileClose                                                     , /* 21 */
	BrFileEof                                                       , /* 22 */
	BrFileGetChar                                                   , /* 23 */
	BrFileGetLine                                                   , /* 24 */
	BrFileOpenRead                                                  , /* 25 */
	BrFileOpenWrite                                                 , /* 26 */
	BrFilePrintf                                                    , /* 27 */
	BrFilePutChar                                                   , /* 28 */
	BrFilePutLine                                                   , /* 29 */
	BrFileRead                                                      , /* 30 */
	BrFileWrite                                                     , /* 31 */
	BrFloatAtan2                                                    , /* 32 */
	BrFloatCeil                                                     , /* 33 */
	BrFloatFloor                                                    , /* 34 */
	BrFloatPow                                                      , /* 35 */
	BrFloatSqrt                                                     , /* 36 */
	BrGetEnv                                                        , /* 37 */
	BrImageAdd                                                      , /* 38 */
	BrImageDereference                                              , /* 39 */
	BrImageLookupName                                               , /* 40 */
	BrImageLookupOrdinal                                            , /* 41 */
	BrImageReference                                                , /* 42 */
	BrImageRemove                                                   , /* 43 */
	BrInsert                                                        , /* 44 */
	BrIsAlpha                                                       , /* 45 */
	BrIsDigit                                                       , /* 46 */
	BrIsPrint                                                       , /* 47 */
	BrIsSpace                                                       , /* 48 */
	BrLastErrorGet                                                  , /* 49 */
	BrLastErrorSet                                                  , /* 50 */
	BrLexerAllocate                                                 , /* 51 */
	BrLexerCommentSet                                               , /* 52 */
	BrLexerDumpSet                                                  , /* 53 */
	BrLexerErrorSet                                                 , /* 54 */
	BrLexerFree                                                     , /* 55 */
	BrLexerPop                                                      , /* 56 */
	BrLexerPosition                                                 , /* 57 */
	BrLexerPushFile                                                 , /* 58 */
	BrLexerPushString                                               , /* 59 */
	BrLexerTokenError                                               , /* 60 */
	BrLogPrintf                                                     , /* 61 */
	BrMemAllocate                                                   , /* 62 */
	BrMemCalloc                                                     , /* 63 */
	BrMemCmp                                                        , /* 64 */
	BrMemCpy                                                        , /* 65 */
	BrMemFree                                                       , /* 66 */
	BrMemInquire                                                    , /* 67 */
	BrMemSet                                                        , /* 68 */
	BrMemStrDup                                                     , /* 69 */
	BrNamePatternMatch                                              , /* 70 */
	BrNewList                                                       , /* 71 */
	BrObjectContainerFree                                           , /* 72 */
	BrObjectListAllocate                                            , /* 73 */
	BrParseFixed                                                    , /* 74 */
	BrParseFloat                                                    , /* 75 */
	BrParseInteger                                                  , /* 76 */
	BrParseMatrixFixed                                              , /* 77 */
	BrParseMatrixFloat                                              , /* 78 */
	BrParseVectorFixed                                              , /* 79 */
	BrParseVectorFloat                                              , /* 80 */
	BrPrimitiveLibraryFind                                          , /* 81 */
	BrQsort                                                         , /* 82 */
	BrRegistryAdd                                                   , /* 83 */
	BrRegistryAddMany                                               , /* 84 */
	BrRegistryAddStatic                                             , /* 85 */
	BrRegistryClear                                                 , /* 86 */
	BrRegistryCount                                                 , /* 87 */
	BrRegistryEnum                                                  , /* 88 */
	BrRegistryFind                                                  , /* 89 */
	BrRegistryFindMany                                              , /* 90 */
	BrRegistryNew                                                   , /* 91 */
	BrRegistryNewStatic                                             , /* 92 */
	BrRegistryRemove                                                , /* 93 */
	BrRegistryRemoveMany                                            , /* 94 */
	BrRegistryRemoveStatic                                          , /* 95 */
	BrRemHead                                                       , /* 96 */
	BrRemTail                                                       , /* 97 */
	BrRemove                                                        , /* 98 */
	BrRendererFacilityFind                                          , /* 99 */
	BrResAdd                                                        , /* 100 */
	BrResAllocate                                                   , /* 101 */
	BrResCheck                                                      , /* 102 */
	BrResChildEnum                                                  , /* 103 */
	BrResClass                                                      , /* 104 */
	BrResDump                                                       , /* 105 */
	BrResFree                                                       , /* 106 */
	BrResFreeNoCallback                                             , /* 107 */
	BrResIsChild                                                    , /* 108 */
	BrResRemove                                                     , /* 109 */
	BrResSize                                                       , /* 110 */
	BrResSizeTotal                                                  , /* 111 */
	BrResStrDup                                                     , /* 112 */
	BrSScanf                                                        , /* 113 */
	BrScratchAllocate                                               , /* 114 */
	BrScratchFlush                                                  , /* 115 */
	BrScratchFree                                                   , /* 116 */
	BrScratchInquire                                                , /* 117 */
	BrScratchString                                                 , /* 118 */
	BrScratchStringSize                                             , /* 119 */
	BrSimpleAddHead                                                 , /* 120 */
	BrSimpleInsert                                                  , /* 121 */
	BrSimpleNewList                                                 , /* 122 */
	BrSimpleRemHead                                                 , /* 123 */
	BrSimpleRemove                                                  , /* 124 */
	BrSprintf                                                       , /* 125 */
	BrSprintfN                                                      , /* 126 */
	BrStrCat                                                        , /* 127 */
	BrStrCmp                                                        , /* 128 */
	BrStrCpy                                                        , /* 129 */
	BrStrICmp                                                       , /* 130 */
	BrStrLen                                                        , /* 131 */
	BrStrNCmp                                                       , /* 132 */
	BrStrNCpy                                                       , /* 133 */
	BrStrNICmp                                                      , /* 134 */
	BrStrRChr                                                       , /* 135 */
	BrStrToD                                                        , /* 136 */
	BrStrToF                                                        , /* 137 */
	BrStrToL                                                        , /* 138 */
	BrStrToUL                                                       , /* 139 */
	BrStringToTokenValue                                            , /* 140 */
	BrSwap16                                                        , /* 141 */
	BrSwap32                                                        , /* 142 */
	BrSwapBlock                                                     , /* 143 */
	BrSwapFloat                                                     , /* 144 */
	BrTVTemplateAllocate                                            , /* 145 */
	BrTVTemplateFree                                                , /* 146 */
	BrTokenCount                                                    , /* 147 */
	BrTokenCreate                                                   , /* 148 */
	BrTokenFind                                                     , /* 149 */
	BrTokenFindMany                                                 , /* 150 */
	BrTokenIdentifier                                               , /* 151 */
	BrTokenType                                                     , /* 152 */
	BrTokenValueCompare                                             , /* 153 */
	BrTokenValueComparePartial                                      , /* 154 */
	BrTokenValueDump                                                , /* 155 */
	BrTokenValueQuery                                               , /* 156 */
	BrTokenValueQueryAll                                            , /* 157 */
	BrTokenValueQueryAllSize                                        , /* 158 */
	BrTokenValueQueryMany                                           , /* 159 */
	BrTokenValueQueryManySize                                       , /* 160 */
	BrTokenValueSet                                                 , /* 161 */
	BrTokenValueSetMany                                             , /* 162 */
	BrVSScanf                                                       , /* 163 */
	BrVSprintf                                                      , /* 164 */
	BrVSprintfN                                                     , /* 165 */
	BrWarning                                                       , /* 166 */
	_BrAssert                                                       , /* 167 */
	_BrExceptionBegin                                               , /* 168 */
	_BrExceptionEnd                                                 , /* 169 */
	_BrExceptionResource                                            , /* 170 */
	_BrExceptionThrow                                               , /* 171 */
	_BrExceptionValueFetch                                          , /* 172 */
	_BrUAssert                                                      , /* 173 */
	_EPI                                                            , /* 174 */
	_M_br_object_container_addFront                                 , /* 175 */
	_M_br_object_container_count                                    , /* 176 */
	_M_br_object_container_find                                     , /* 177 */
	_M_br_object_container_findMany                                 , /* 178 */
	_M_br_object_container_remove                                   , /* 179 */
	_M_br_object_container_removeFront                              , /* 180 */
	_M_br_object_container_tokensMatch                              , /* 181 */
	_M_br_object_container_tokensMatchBegin                         , /* 182 */
	_M_br_object_container_tokensMatchEnd                           , /* 183 */
	_M_br_object_query                                              , /* 184 */
	_M_br_object_queryAll                                           , /* 185 */
	_M_br_object_queryAllSize                                       , /* 186 */
	_M_br_object_queryBuffer                                        , /* 187 */
	_M_br_object_queryMany                                          , /* 188 */
	_M_br_object_queryManySize                                      , /* 189 */
	_PRO                                                            , /* 190 */
};

static br_image Image_BRCORE1 = {
	{0},
	"BRCORE1",						/* Identifier				*/
	BR_IMG_RESIDENT,				/* Image type				*/
	0,								/* Reference count			*/

	1,									/* Ordinal base				*/
	BR_ASIZE(functionPointers_BRCORE1),	/* Number of functions		*/
	functionPointers_BRCORE1,				/* Table of functions		*/

	BR_ASIZE(namePointers_BRCORE1),	/* Number of names			*/
	namePointers_BRCORE1,				/* Table of names 			*/
	nameOrdinals_BRCORE1,				/* Table of name ordinals	*/
	
	0,								/* Number of imports		*/
	NULL,							/* Table of imports			*/

	0,								/* Number of sections		*/
	NULL,							/* Table of sections		*/

	NULL,							/* Type pointer				*/
};

