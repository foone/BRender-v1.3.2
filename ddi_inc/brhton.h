/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: brhton.h 1.1 1997/12/10 16:41:04 jon Exp $
 * $Locker: $
 *
 * Converting 'network' (big endian) byte order to host order
 *
 * This is a private implemenation - some platforms do not provide
 * this functionality. For those that do, this file should define
 * the operations in terms of the platform specific API
 */

#ifndef _BRHTON_H_
#define _BRHTON_H_

#if BR_ENDIAN_BIG

/*
 * Network<->Host
 */
#define BrHtoNL(x) (x)
#define BrNtoHL(x) (x)
#define BrHtoNS(x) (x)
#define BrNtoHS(x) (x)
#define BrHtoNF(x) (x)
#define BrNtoHF(x) (x)

/*
 * Big-Endian<->Host
 */
#define BrHtoBL(x) (x)
#define BrBtoHL(x) (x)
#define BrHtoBS(x) (x)
#define BrBtoHS(x) (x)
#define BrHtoBF(x) (x)
#define BrBtoHF(x) (x)

/*
 * Little-Endian<->Host
 */
#define BrLtoHL(x) BrSwap32(x)
#define BrHtoLL(x) BrSwap32(x)
#define BrLtoHS(x) BrSwap16(x)
#define BrHtoLS(x) BrSwap16(x)

#define BrHtoLF(x) BrSwapFloat(x)
#define BrLtoHF(x) BrSwapFloat(x)

#endif

#if BR_ENDIAN_LITTLE

/*
 * Network<->Host
 */
#define BrNtoHL(x) BrSwap32(x)
#define BrHtoNL(x) BrSwap32(x)
#define BrNtoHS(x) BrSwap16(x)
#define BrHtoNS(x) BrSwap16(x)

#define BrHtoNF(x) BrSwapFloat(x)
#define BrNtoHF(x) BrSwapFloat(x)

/*
 * Big-Endian<->Host
 */
#define BrHtoBL(x) BrSwap32(x)
#define BrBtoHL(x) BrSwap32(x)
#define BrHtoBS(x) BrSwap16(x)
#define BrBtoHS(x) BrSwap16(x)
#define BrHtoBF(x) BrSwapFloat(x)
#define BrBtoHF(x) BrSwapFloat(x)

/*
 * Little-Endian<->Host
 */
#define BrLtoHL(x) (x)
#define BrHtoLL(x) (x)
#define BrLtoHS(x) (x)
#define BrHtoLS(x) (x)
#define BrHtoLF(x) (x)
#define BrLtoHF(x) (x)

#endif

#endif

