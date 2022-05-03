#ifndef __MIP_H
#define __MIP_H

#include <brender.h>

#ifdef __cplusplus
	extern "C" {
#endif

void generateMipLevels(br_uint_32 base,br_uint_32 range);

#ifdef __cplusplus
	}
#endif

#endif //__MIP_H

