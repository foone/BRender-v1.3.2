/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: brerror.h 1.1 1997/12/10 16:41:15 jon Exp $
 * $Locker: $
 *
 * Error type
 */
#ifndef _BRERR_H_
#define _BRERR_H_

/*
 * Error types are 32 bit integers, broken into four fields -
 *
 * Reserved Class    Subclass
 *
 * 33222222 22221111 1111110000000000
 * 10987654 32109876 5432109876543210
 *
 * A value of all zeros is reserved to indiciate no error
 *
 * Class indicates which part of Brender originated the error
 * Subclass indicates the particular error
 */
typedef br_uint_32 br_error;

/*
 * OK error return
 */
#define BRE_OK	0x0


/*
 * Make an error value from the seperate fields
 */
#define BR_ERROR_MAKE(class,sub) ((class) | (sub))

/*
 * Extract fields from an error
 */
#define BR_ERROR_CLASS(error)    (error & 0xFF0000)
#define BR_ERROR_SUBCLASS(error) (error & 0x00FFFF)

/*
 * Defined error classes
 */
#define BR_ECLASS_NONE			0x0000
#define BR_ECLASS_MISC			0x1000
#define BR_ECLASS_APPLICATION	0x2000
#define BR_ECLASS_DATABASE		0x3000
#define BR_ECLASS_RENDERER		0x4000
#define BR_ECLASS_FILE			0x5000
#define BR_ECLASS_TOKEN			0x6000
#define BR_ECLASS_MATH			0x7000
#define BR_ECLASS_DOSIO			0x8000
#define BR_ECLASS_HOST			0x9000
#define BR_ECLASS_DEVICE		0xA000

/*
 * Some generic error values
 */
#define BRE_UNKNOWN			BR_ERROR_MAKE(BR_ECLASS_MISC,1)
#define BRE_FAIL			BR_ERROR_MAKE(BR_ECLASS_MISC,2)
#define BRE_NO_MEMORY		BR_ERROR_MAKE(BR_ECLASS_MISC,3)
#define BRE_OVERFLOW		BR_ERROR_MAKE(BR_ECLASS_MISC,4)
#define BRE_UNDERFLOW		BR_ERROR_MAKE(BR_ECLASS_MISC,5)
#define BRE_NOT_ACTIVE		BR_ERROR_MAKE(BR_ECLASS_MISC,6)
#define BRE_ALLREADY_ACTIVE	BR_ERROR_MAKE(BR_ECLASS_MISC,7)
#define BRE_UNSUPPORTED		BR_ERROR_MAKE(BR_ECLASS_MISC,8)
#define BRE_INVALID			BR_ERROR_MAKE(BR_ECLASS_MISC,9)


#define BRE_DEV_FAIL                    BR_ERROR_MAKE(BR_ECLASS_DEVICE, 1)  // All general device errors
#define BRE_DEV_HARDWARE_NOT_PRESENT    BR_ERROR_MAKE(BR_ECLASS_DEVICE, 2)  // Hardware not detected: usually device library failed init
#define BRE_DEV_NO_MEMORY               BR_ERROR_MAKE(BR_ECLASS_DEVICE, 3)  // Driver failed in mem allocate
#define BRE_DEV_NO_VIDEO_MEMORY         BR_ERROR_MAKE(BR_ECLASS_DEVICE, 4)  // Driver failed in video mem allocate
#define BRE_DEV_UNSUPPORTED             BR_ERROR_MAKE(BR_ECLASS_DEVICE, 5)  // Device does not support capabilities
#define BRE_DEV_HARDWARE_SET            BR_ERROR_MAKE(BR_ECLASS_DEVICE, 6)  // Driver failed some hardware setup call
#define BRE_DEV_HARDWARE_NOT_READY      BR_ERROR_MAKE(BR_ECLASS_DEVICE, 7)
#define BRE_DEV_NOT_FOUND               BR_ERROR_MAKE(BR_ECLASS_DEVICE, 8)  // Device driver not present (BRender should return this)
#define BRE_DEV_REFUSED                 BR_ERROR_MAKE(BR_ECLASS_DEVICE, 9)  // Some request refused
#define BRE_DEV_LOCK_FAIL               BR_ERROR_MAKE(BR_ECLASS_DEVICE, 10) // Some locking call failed
#define BRE_DEV_OUTPUT_UNAVAILABLE      BR_ERROR_MAKE(BR_ECLASS_DEVICE, 11) // Output facility is not available
#define BRE_DEV_ALREADY_INIT            BR_ERROR_MAKE(BR_ECLASS_DEVICE, 12) // Some facility is already initialised
#define BRE_DEV_DIRECTDRAW_ERROR        BR_ERROR_MAKE(BR_ECLASS_DEVICE, 13) // Direct Draw returned an error not covered by other codes

#endif


