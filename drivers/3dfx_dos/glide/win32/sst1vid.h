/*
** Copyright (c) 1995, 3Dfx Interactive, Inc.
** All Rights Reserved.
**
** This is UNPUBLISHED PROPRIETARY SOURCE CODE of 3Dfx Interactive, Inc.;
** the contents of this file may not be disclosed to third parties, copied or
** duplicated in any form, in whole or in part, without the prior written
** permission of 3Dfx Interactive, Inc.
**
** RESTRICTED RIGHTS LEGEND:
** Use, duplication or disclosure by the Government is subject to restrictions
** as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
** and Computer Software clause at DFARS 252.227-7013, and/or in similar or
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
** rights reserved under the Copyright Laws of the United States.
**
** $Header: P:/brender/source/drivers/3dfx_dos/RCS/glide/win32/sst1vid.h 1.1 1997/12/11 11:52:56 jon Exp $
** $Log: sst1vid.h $
** Revision 1.1  1997/12/11 11:52:56  jon
** Initial revision
 * 
 * 5     7/24/96 3:43p Sellers
 * added 512x384 @ 60 Hz for arcade monitors
 * added 512x256 @ 60 Hz for arcade monitors
 * 
 * 4     7/18/96 10:58a Sellers
 * fixed FT and TF clock delay values for lower frequencies with
 * .5/.5 combos
 * 
 * 3     6/18/96 6:54p Sellers
 * added sst1InitShutdownSli() to fix Glide Splash screen problems with
 * SLI
 * 
 * 2     6/13/96 7:45p Sellers
 * added "voodoo.ini" support
 * added DirectX support
 * misc cleanup
 * 
 * 2     6/11/96 1:43p Sellers
 * added support for 60, 75, 85, and 120 Hz refresh rates for "most"
 * resolutions
 * 
 * 1     5/08/96 5:43p Paik
 * Video definitions
*/
#ifndef __SST1VID_H__
#define __SST1VID_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Video defines */

typedef FxI32 GrScreenRefresh_t;
#define GR_REFRESH_60Hz   0x0
#define GR_REFRESH_70Hz   0x1
#define GR_REFRESH_72Hz   0x2
#define GR_REFRESH_75Hz   0x3
#define GR_REFRESH_80Hz   0x4
#define GR_REFRESH_90Hz   0x5
#define GR_REFRESH_100Hz  0x6
#define GR_REFRESH_85Hz   0x7
#define GR_REFRESH_120Hz  0x8
#define GR_REFRESH_NONE   0xff

typedef FxI32 GrScreenResolution_t;
#define GR_RESOLUTION_320x200   0x0
#define GR_RESOLUTION_320x240   0x1
#define GR_RESOLUTION_400x256   0x2
#define GR_RESOLUTION_512x384   0x3
#define GR_RESOLUTION_640x200   0x4
#define GR_RESOLUTION_640x350   0x5
#define GR_RESOLUTION_640x400   0x6
#define GR_RESOLUTION_640x480   0x7
#define GR_RESOLUTION_800x600   0x8
#define GR_RESOLUTION_960x720   0x9
#define GR_RESOLUTION_856x480   0xa
#define GR_RESOLUTION_512x256   0xb
#define GR_RESOLUTION_NONE      0xff

#ifdef __cplusplus
}
#endif

#endif /* __SST1VID_H__ */
